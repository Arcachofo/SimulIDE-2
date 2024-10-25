/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "avrusi.h"
#include "e_mcu.h"
#include "mcupin.h"
#include "avrtimer.h"
#include "mcuocunit.h"
#include "mcuinterrupts.h"
#include "datautils.h"
#include "regwatcher.h"

AvrUsi::AvrUsi( eMcu* mcu, QString name )
      : McuModule( mcu, name )
      , eElement( name )
{
}
AvrUsi::~AvrUsi(){}

void AvrUsi::setup()
{
    m_DOpin = nullptr;
    m_DIpin = nullptr;
    m_CKpin = nullptr;

    m_dataReg   = (uint8_t*) m_mcuRam->getReg("USIDR");
    m_bufferReg = (uint8_t*) m_mcuRam->getReg("USIBR");

    m_USITC  = m_mcuRam->getRegBits("USITC");
    m_USICLK = m_mcuRam->getRegBits("USICLK");
    m_USICS  = m_mcuRam->getRegBits("USICS0,USICS1");
    m_USIWM  = m_mcuRam->getRegBits("USIWM0,USIWM1");

    m_USICNT = m_mcuRam->getRegBits("USICNT0,USICNT1,USICNT2,USICNT3");
    m_USIPF  = m_mcuRam->getRegBits("USIPF");

    AvrTimer800* timer0 = (AvrTimer800*)m_mcu->getTimer("TIMER0");
    m_t0OCA  = timer0->getOcUnit("OCA");

    m_mcuRam->watchRegName("USIDR", R_WRITE, this, &AvrUsi::dataRegWritten );
}

void AvrUsi::reset()
{
    m_twi      = false;
    m_spi      = false;
    m_timer    = false;
    m_extClk   = false;
    m_usiClk   = false;
    m_usiSR    = false;
    m_clkEdge  = false;
    m_clkState = false;
    m_clockMode = 0;
    m_mode = 0;
    m_counter = 0;

    if( !m_DOpin ) qDebug() << "AvrUsi::configureA: Error: null DO Pin";
    if( !m_DIpin ) qDebug() << "AvrUsi::configureA: Error: null DI Pin";
    if( !m_CKpin ) qDebug() << "AvrUsi::configureA: Error: null CK Pin";
}

void AvrUsi::voltChanged()  // Clk Pin changed
{
    bool clkState = m_CKpin->getInpState();

    if( m_mode > 1 ) // TWI start/stop detector
    {
        bool sdaState = m_DIpin->getInpState();
        if( clkState ){
            if     (  m_sdaState && !sdaState){ if( m_startInte ) m_startInte->raise(); } // Start condition
            else if( !m_sdaState &&  sdaState) m_USIPF.set_08(); // Stop condition, set USIPF flag
        }
        m_sdaState = sdaState;
    }

    if( m_clkState == clkState ) return;
    if( m_extClk ){
        if( !m_usiClk ) stepCounter();              // Counter Both edges

        bool oldSRclock = m_clkEdge ? m_clkState : !m_clkState;
        bool newSRclock = m_clkEdge ?   clkState : !clkState;

        if     ( !oldSRclock &&  newSRclock ) shiftData(); // SR Leading  Edge
        else if(  oldSRclock && !newSRclock ) setOutput(); // SR Trailibg Edge
    }
    m_clkState = clkState;
}

void AvrUsi::callBack()  // Called at Timer0 Compare Match
{
    shiftData();
    stepCounter();
    setOutput();
}

void AvrUsi::configureA() // USICR
{
    uint8_t mode = m_USIWM.getRegBitsVal();
    if( m_mode != mode )
    {
        m_mode = mode;
        bool spi = false;
        bool twi = false;

        switch( mode ) {
            case 0:                break; // Disabled
            case 1: spi = true;    break; // 3 Wire mode: Uses DO, DI, and USCK pins.
            case 2:                       // 2 Wire mode: Uses SDA (DI) and SCL (USCK) pins.
            case 3: twi = true;           // Same as 2 wire above & SCL held low at counter overflow
        }
        if( m_spi != spi ){
            m_spi = spi;
            if( m_DOpin ) m_DOpin->controlPin( spi, false );
        }
        if( m_twi != twi ){
            m_twi = twi;
            if( twi ) // 2 Wire mode: SDA (DI) & SCL (USCK) open collector if DDRB=out, pullups disabled
            {
                m_sdaState = m_DIpin->getInpState();
                m_clkState = m_CKpin->getInpState();
            }
            if( m_DIpin ){
                m_DIpin->changeCallBack( this, twi );  // Used for Start/Stop detection
                m_DIpin->setOpenColl( twi );
            }
            if( m_CKpin ) m_CKpin->setOpenColl( twi );
        }
    }

    uint8_t clockMode = m_USICS.getRegBitsVal();
    if( m_clockMode != clockMode )
    {
        m_clockMode = clockMode;
        m_clkEdge   = false;
        bool extClk = false;
        bool timer  = false;

        switch( clockMode ){
            case 0:                   break; // Software clock strobe (USICLK)
            case 1:     timer = true; break; // Timer0 Compare Match
            case 2: m_clkEdge = true;        // External, shiftData() positive edge
            case 3:    extClk = true;        // External, shiftData() negative edge
        }
        if( m_extClk != extClk ){            // Activate/Deactivate External Clock
            m_extClk = extClk;
            if( m_CKpin ) m_CKpin->changeCallBack( this, extClk );
        }
        if( m_timer != timer ){
            m_timer = timer;
            m_t0OCA->getInterrupt()->callBack( this, timer );
        }
    }
    if( m_mode )
    {
        bool usiTc = m_USITC.getRegBitsBool(); // toggles the USCK/SCL Pin
        if( usiTc ) toggleClock();                        // USITC always toggles Clock (PORT Register)

        m_usiClk = m_USICLK.getRegBitsBool();

        if( !m_timer )
        {
            if( m_extClk ){        // USICS1 = 1
                if( m_usiClk && usiTc ) stepCounter(); // Software counter strobe (USITC)
            }
            else{                  // USICS1 = 0
                if( m_usiClk ){
                    stepCounter(); // Software counter strobe (USICLK)
                    shiftData();   // shiftData at Active edge
                }
                else setOutput();  // setOutput at Opposite edge
            }
        }
    }
    m_USICLK.clear_08();
    m_USITC.clear_08(); // USICLK & USITC always read as 0
}

void AvrUsi::configureB() // USISR
{
    m_counter = m_USICNT.getRegBitsVal(); // USICNT[3:0]: Counter Value

    bool newUsiSR = m_USIPF.getRegBitsBool();
    if( m_usiSR && newUsiSR ) m_USIPF.clear_08(); // clear USIPF by writing a 1 to it
    m_usiSR = newUsiSR;
}

void AvrUsi::dataRegWritten() // USIDR is being written
{
    m_DoState = *m_dataReg & 1<<7; // Fetch bit 7
    setOutput();
}

void AvrUsi::stepCounter()  // increment counter
{
    m_USICNT.write_08( m_counter ); // Write m_counter to USI status reg

    if( ++m_counter == 16 ){
        m_counter = 0;
        *m_bufferReg = *m_dataReg; // Transfer Data Register content to Buffer Register
        if( m_interrupt ) m_interrupt->raise();
    }
}

void AvrUsi::shiftData()
{
    *m_dataReg = *m_dataReg<<1;        // Shift Data Register
    m_DoState  = *m_dataReg & 1<<7;    // Fetch bit 7 to set output at falling edge

    if( !m_DIpin ) return;
    if( m_DIpin->getInpState() ) *m_dataReg |=  1; // Read input & store in Data Reg bit0
    else                         *m_dataReg &= ~1;
}

void AvrUsi::setOutput() // Set output *m_dataReg & 1<<7
{
    if( m_mode == 1 ){ if( m_DOpin ) m_DOpin->setOutState( m_DoState ); } // SPI
    else if( m_mode > 1 ) //writeBitsToReg( m_DIbit, m_DoState );    // TWI
    {
        /// Fixme: this is mostly a hack
        McuRegister* reg = m_DIbit.reg;
        m_DIbit.write_08( m_DoState );
        reg->write_08( reg->getAddress(), reg->read() );
    }
}

void AvrUsi::toggleClock()
{
    bool ck = m_CKbit.getRegBitsBool();
    if( ck ) m_CKbit.clear_08();
    else     m_CKbit.set_08();

    //writeBitsToReg( m_CKbit, !getRegBitsBool( m_CKbit ), m_mcuRam );
}

void AvrUsi::setPins( QString pinStr ) // "DO,DI,USCK"
{
    QStringList pins = pinStr.split(",");
    if( pins.size() < 3 ){ qDebug() << "AvrUsi::setPins Error:" << pinStr; return; }

    QString DIpin = pins.value(1);
    QString CKpin = pins.value(2);

    m_DOpin = m_mcu->getMcuPin( pins.value(0) );
    m_DIpin = m_mcu->getMcuPin( DIpin );
    m_CKpin = m_mcu->getMcuPin( CKpin );

    m_DIbit = m_mcuRam->getRegBits( DIpin );
    m_CKbit = m_mcuRam->getRegBits( CKpin );
}
