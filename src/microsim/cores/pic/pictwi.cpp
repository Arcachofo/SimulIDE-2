/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "pictwi.h"
#include "mcupin.h"
#include "e_mcu.h"
#include "mcuinterrupts.h"
#include "mcuram.h"
#include "simulator.h"

PicTwi::PicTwi( eMcu* mcu, QString name )
      : McuTwi( mcu, name )
{
}
PicTwi::~PicTwi(){}

void PicTwi::setup()
{
    m_sleepMode = 0xFF;

    // SSPCON
    m_WCOL    = m_mcuRam->getRegBits("WCOL");

    // SSPCON2
    m_CGEN    = m_mcuRam->getRegBits("CGEN"   );
    m_ACKSTAT = m_mcuRam->getRegBits("ACKSTAT");
    m_ACKDT   = m_mcuRam->getRegBits("ACKDT"  );
    m_ACKEN   = m_mcuRam->getRegBits("ACKEN"  );
    m_RCEN    = m_mcuRam->getRegBits("RCEN"   );
    m_PEN     = m_mcuRam->getRegBits("PEN"    );
    m_RSEN    = m_mcuRam->getRegBits("RSEN"   );
    m_SEN     = m_mcuRam->getRegBits("SEN"    );

    //SSPSTAT
    m_P  = m_mcuRam->getRegBits("P" );
    m_S  = m_mcuRam->getRegBits("S" );
    m_RW = m_mcuRam->getRegBits("RW");
    m_BF = m_mcuRam->getRegBits("BF");
}

void PicTwi::initialize()
{
    McuTwi::initialize();
    m_bitRate = 0;
    m_enabled = false;
}

void PicTwi::setMode( twiMode_t mode )
{
    if( m_mode == mode ) return;

    bool oldEn = m_enabled;
    m_enabled  = mode != TWI_OFF;
    if( oldEn && !m_enabled )                 /// Disable TWI
    {
        m_sda->controlPin( false, false ); // Release control of MCU PIns
        m_scl->controlPin( false, false );
    }
    else if( !oldEn && m_enabled )            /// Enable TWI if it was disabled
    {
        m_sda->setPinMode( openCo );
        m_sda->controlPin( true, false ); // Get control of MCU PIns
        m_scl->setPinMode( openCo );
        m_scl->controlPin( true, false );
    }
    TwiModule::setMode( mode );
}

void PicTwi::configureA() // SSPCON
{
    /*bool wcol  = getRegBitsBool( newSSPCON, m_WCOL );
    bool oldWcol = getRegBitsBool( m_WCOL );
    wcol=false;*/
}

void PicTwi::configureB() // SSPCON2
{
    m_genCall = m_CGEN.getRegBitsBool(); /// Interrupt??

    bool newStart  = m_SEN.getRegBitsBool()
                   | m_RSEN.getRegBitsBool();
    if( newStart )              /// Generate Start Condition
    {
        if( m_mode == TWI_MASTER )
        {
            m_RW.clear_08();
            masterStart();
        }
    }

    bool newStop = m_PEN.getRegBitsBool();
    if( newStop  )              /// Generate Stop Condition
    {
        if( m_mode == TWI_MASTER ) // Master: Stop if I2C was started
        {
            if( getStaus() < TWI_NO_STATE )
            {
                m_RW.clear_08();
                masterStop();
            }
        }
        /// else setMode( TWI_SLAVE ); // Slave: Stop Cond restarts Slave mode (can be used to recover from an error condition)
    }
    if( m_mode == TWI_MASTER )
    {
        bool rcen  = m_RCEN.getRegBitsBool();
        if( rcen )
        {
            m_sendACK   = !m_ACKDT.getRegBitsBool(); // ACK State to send
            m_masterACK = m_ACKEN.getRegBitsBool(); // Send ACK action

            //if( m_twiState == TWI_MRX_ADR_ACK    // We sent Slave Address + R and received ACK
            // || m_twiState == TWI_MRX_DATA_ACK ) // We sent data and received ACK
            if( m_i2cState == I2C_IDLE ) masterRead( m_sendACK );     // Read a byte and send ACK/NACK
            if( m_masterACK && !m_sendACK ) m_RW.clear_08();
        }
    }
}

void PicTwi::writeAddrReg() // SSPADD
{
    //if( m_mode == TWI_MASTER ) // Baudrate
    {
        double freq = m_mcu->freq()/(4*(*m_addrReg+1));
        setFreqKHz( freq/1e3 );
    }
    //else
        m_address = *m_addrReg >> 1; // SLAVE Address
}

void PicTwi::writeStatus() // SSPSTAT
{
    /// Done by masking //m_mcu->m_regOverride = newTWSR | (*m_statReg & 0b11111100); // Preserve Status bits
}

void PicTwi::writeTwiReg() // SSPBUF is being written
{
    if( m_mode != TWI_MASTER ) return;

    if( m_BF.getRegBitsBool() )  // Check if BF is set (still transmitting)
    {
        m_WCOL.set_08();  // set Write Collision bit WCOL
        return;                // The access will be discarded
    }
    bool write = false;
    bool isAddr = m_S.getRegBitsBool(); // We just sent Start, so this must be slave address

    if( isAddr )
    {
        write = ((*m_dataReg & 1) == 0); // Sendind address for Read or Write?
        //writeRegBits( m_RW, !write );
    }
    m_RW.set_08();
    m_BF.set_08();

    masterWrite( *m_dataReg, isAddr, write );       /// Write data or address to Slave
}

void PicTwi::setTwiState( twiState_t state )  // Set new Status value
{
    m_S.clear_08();
    m_P.clear_08();

    if( state == TWI_START )                                           // Start sent
    {
        m_SEN.clear_08();
        m_RSEN.clear_08();
        m_S.set_08();
        m_interrupt->raise();
    }
    else if( state == TWI_NO_STATE && m_i2cState == I2C_STOP  )        // Stop sent
    {
        m_PEN.clear_08();
        m_P.set_08();
        m_interrupt->raise();
    }
    else if( state == TWI_MTX_ADR_ACK    // ACK Master Addr transmitted for Write
          || state == TWI_MRX_ADR_ACK    // ACK Master Addr transmitted for Read
          || state == TWI_MTX_DATA_ACK ) // ACK Master Data transmitted
    {
        if( m_mode == TWI_MASTER ) m_RW.clear_08();
        m_ACKSTAT.clear_08();
        m_interrupt->raise();
    }
    else if( state == TWI_MTX_ADR_NACK    // NACK Master Addr transmitted for Write
          || state == TWI_MRX_ADR_NACK    // NACK Master Addr transmitted for Read
          || state == TWI_MTX_DATA_NACK ) // NACK Master Data transmitted
    {
        if( m_mode == TWI_MASTER ) m_RW.clear_08();
        m_ACKSTAT.set_08();
        m_interrupt->raise();
    }
    else if( state == TWI_MRX_DATA_ACK || state == TWI_MRX_DATA_NACK ) // Master Data received
    {
        //*m_dataReg = m_rxReg; // Save data received into SSPBUF  // Done in readByte()
        m_ACKEN.clear_08();
    }
    else if( state == TWI_SRX_ADR_DATA_ACK || state == TWI_SRX_ADR_DATA_NACK   //Slave Addr received
          || state == TWI_SRX_GEN_DATA_ACK || state == TWI_SRX_GEN_DATA_NACK ) //Slave Data received
    {
        //*m_dataReg = m_rxReg; // Save data received into SSPBUF // Done in readByte()
    }

    TwiModule::setTwiState( state );
}

void PicTwi::writeByte() // Read from Data Register
{
    if( m_mode == TWI_SLAVE ) m_txReg = *m_dataReg;
    TwiModule::writeByte();
}

void PicTwi::bufferEmpty() // Data/Addr transmitted (before ACK)
{
    m_BF.clear_08();
}

void PicTwi::readByte()
{
    *m_dataReg = m_rxReg; // Save data received into SSPBUF
    m_BF.set_08();
    m_interrupt->raise();
    TwiModule::readByte();
}

void PicTwi::readTwiReg()
{
    m_BF.clear_08();
}

void PicTwi::sleep( int mode )
{
    McuModule::sleep( mode );
    if( m_sleeping ) pauseEvents();
    else             resumeEvents();
}
