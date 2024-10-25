/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "avrwdt.h"
#include "e_mcu.h"
#include "cpubase.h"
#include "simulator.h"
#include "mcuram.h"

AvrWdt* AvrWdt::createWdt( eMcu* mcu, QString name )
{
    int type = name.right(2).toInt();
    switch ( type ){
        case 00: return new AvrWdt00( mcu, name ); break;
        case 01: return new AvrWdt01( mcu, name ); break;
        default: return NULL;
}   }

AvrWdt::AvrWdt( eMcu* mcu, QString name )
      : McuWdt( mcu, name )
{
}
AvrWdt::~AvrWdt(){}

void AvrWdt::setup()
{
    //m_WDTCSR = m_mcu->getReg( "WDTCSR" );

    m_WDCE  = m_mcuRam->getRegBits("WDCE");
    m_WDE   = m_mcuRam->getRegBits("WDE");
    m_WDP02 = m_mcuRam->getRegBits("WDP0,WDP1,WDP2");
    m_WDRF  = m_mcuRam->getRegBits("WDRF");
}

void AvrWdt::initialize()
{
    McuWdt::initialize();
    m_disabled = true;
    m_allowChanges = false;
    m_prescaler = 0;
    m_ovfPeriod = m_clkPeriod/m_prescList[ m_prescaler ];
}

void AvrWdt::runEvent()
{
    if( !m_wdtFuse && m_disabled ) return;

    if( m_allowChanges )
    {
        m_WDCE.clear_08();
        m_allowChanges = false;
    }
    else // McuWdt::runEvent();
    {
        if( m_ovfInter ) m_interrupt->raise();
        else if( m_ovfReset ) // No interrupt, just Reset
        {
            qDebug() << "AvrWdt::runEvent - Watchdog Reset\n";
            m_mcu->cpu()->reset();
        }
        Simulator::self()->addEvent( m_ovfPeriod, this );
    }
}

void AvrWdt::callBack() // WDT Overflow Interrupt just executed
{
    if( !m_wdtFuse && m_disabled ) return;
    qDebug() << "AvrWdt::callBack - Watchdog Reset\n";
    m_mcu->cpu()->reset();
}

void AvrWdt::configureA() // WDTCSR Written
{
    bool WDE  = m_WDE.getRegBitsBool();
    bool WDCE = m_WDCE.getRegBitsBool();

    if( WDCE && WDE )     // Allow WDP & WDE changes for next 4 cycles
    {
        m_allowChanges = true;
        Simulator::self()->cancelEvents( this );
        Simulator::self()->addEvent( 4*m_mcu->psInst(), this );
    }
    else if( m_allowChanges && !WDCE ) // WDP & WDE changes allowed
    {
        updtPrescaler();
        m_ovfPeriod  = m_clkPeriod/m_prescList[ m_prescaler ];
        m_ovfReset   = WDE;
        wdtEnable();
        runEvent();
        return;
    }
    if( m_ovfInter && !m_allowChanges ) wdtEnable();
}

void AvrWdt::wdtEnable()
{
    m_disabled = !(m_ovfInter || m_ovfReset);
    Simulator::self()->cancelEvents( this );
    if( m_wdtFuse || !m_disabled )
    {
        Simulator::self()->addEvent( m_ovfPeriod, this );
        // In Iterrupt + reset, first execute Interrupt, then reset
        if( m_interrupt ) m_interrupt->exitCallBack( this, m_ovfInter && m_ovfReset );
    }
}

void AvrWdt::reset()
{
    m_WDRF.set_08(); // MCUSR.WDRF
    Simulator::self()->cancelEvents( this );
    if( m_wdtFuse || !m_disabled ) Simulator::self()->addEvent( m_ovfPeriod, this );
}

//------------------------------------------------------
//-- AVR WDT Type 00 -----------------------------------

AvrWdt00::AvrWdt00( eMcu* mcu, QString name )
        : AvrWdt( mcu, name )
{
    m_clkPeriod = 8.192*1e12; // 1048576 cycles * 7812500 ps (128 KHz)

    m_WDIF = m_mcuRam->getRegBits("WDIF");
    m_WDIE = m_mcuRam->getRegBits("WDIE");
    m_WDP3 = m_mcuRam->getRegBits("WDP3");
}
AvrWdt00::~AvrWdt00(){}

void AvrWdt00::configureA() // WDTCSR Written
{
    /// Done in Interrupts??
    /*bool clearWdif = m_WDIF.getRegBitsVal();
    if( clearWdif )  /// Writting 1 to WDIF clears the flag
    {
        m_WDIF.clear_08(); // Clear WDIF flag
    }*/
    m_ovfInter = m_WDIE.getRegBitsVal();

    AvrWdt::configureA();

    // WDP & WDE changes not allowed, keep old values
    /// FIXME
    ///newWDTCSR = overrideBits( newWDTCSR, m_WDE );   // Keep old WDE
    ///newWDTCSR = overrideBits( newWDTCSR, m_WDP02 ); // Keep old WDP
    ///newWDTCSR = overrideBits( newWDTCSR, m_WDP3 );  // Keep old WDP
    ///m_mcuRam->m_regOverride = newWDTCSR;
}

void AvrWdt00::updtPrescaler() // WDTCSR
{
    m_prescaler  = m_WDP02.getRegBitsVal();
    m_prescaler |= m_WDP3.getRegBitsVal() << 3;
}

//------------------------------------------------------
//-- AVR WDT Type 01 -----------------------------------

AvrWdt01::AvrWdt01( eMcu* mcu, QString name )
        : AvrWdt( mcu, name )
{
    m_clkPeriod = 2.097152*1e12; // 2097152 cycles * 1000000 ps (1 MHz)
}
AvrWdt01::~AvrWdt01(){}

void AvrWdt01::configureA() // WDTCSR Written
{
    AvrWdt::configureA();

    // WDP & WDE changes not allowed, keep old values
    /// FIXME
    ///newWDTCSR = overrideBits( newWDTCSR, m_WDE );   // Keep old WDE
    ///newWDTCSR = overrideBits( newWDTCSR, m_WDP02 ); // Keep old WDP
    ///m_mcuRam->m_regOverride = newWDTCSR;
}

void AvrWdt01::updtPrescaler() // WDTCSR
{
    m_prescaler = m_WDP02.getRegBitsVal();
}
