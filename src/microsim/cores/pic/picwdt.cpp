/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "picwdt.h"
#include "e_mcu.h"
#include "cpubase.h"
#include "simulator.h"
#include "mcuram.h"

PicWdt::PicWdt( eMcu* mcu, QString name )
      : McuWdt( mcu, name )
{
}
PicWdt::~PicWdt(){}

void PicWdt::setup()
{
    m_clkPeriod = 18*1e9; // 18 ms

    m_PS  = m_mcuRam->getRegBits("PS0, PS1, PS2");
    m_PSA = m_mcuRam->getRegBits("PSA");
}

void PicWdt::initialize()
{
    McuWdt::initialize();
    m_prescaler = 0;
    m_ovfPeriod = m_clkPeriod/m_prescList[ m_prescaler ];
    m_ovfReset = true;
}

void PicWdt::runEvent()
{
    if( m_sleeping )
    {
        m_mcu->sleep( false );
        m_mcu->cpu()->exitSleep(); // TO bit in the Status register is cleared
    }
    else McuWdt::runEvent();
}

void PicWdt::configureA() // OPTION Written
{
    if( !m_wdtFuse ) return;

    if( m_PSA.getRegBitsVal() )
         m_prescaler = m_PS.getRegBitsVal();  // Prescaler asigned to Watchdog
    else m_prescaler = 0;                                 // Prescaler asigned to TIMER0
    m_ovfPeriod = m_clkPeriod/m_prescList[ m_prescaler ];
}

void PicWdt::reset()
{
    Simulator::self()->cancelEvents( this );
    if( m_wdtFuse ) Simulator::self()->addEvent( m_ovfPeriod, this );
}

void PicWdt::sleep( int mode )
{
    McuModule::sleep( mode );
    reset();
}
