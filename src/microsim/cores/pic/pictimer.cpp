/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "pictimer.h"
#include "e_mcu.h"
#include "simulator.h"
#include "mcuram.h"
#include "datautils.h"

McuTimer* PicTimer::createTimer( eMcu* mcu, QString name, int type  ) // Static
{
    if     ( type == 800 ) return new PicTimer0( mcu, name );
    else if( type == 820 ) return new PicTimer2( mcu, name );
    //else if( type == 821 ) return new AvrTimer821( mcu, name );
    else if( type == 160 ) return new PicTimer160( mcu, name );
    else if( type == 161 ) return new PicTimer161( mcu, name );
    return NULL;
}

PicTimer::PicTimer(  eMcu* mcu, QString name )
        : McuTimer( mcu, name )
{
    m_sleepMode = 0xFF;
}
PicTimer::~PicTimer(){}

void PicTimer::initialize()
{
    McuTimer::initialize();

    m_ovfMatch  = m_maxCount;
    m_ovfPeriod = m_ovfMatch + 1;
}

/*void PicTimer::addOcUnit( McuOcUnit* ocUnit )
{
}

McuOcUnit* PicTimer::getOcUnit( QString name )
{
    return NULL;
}*/

void PicTimer::configureA( )
{
}

void PicTimer::configureB()
{
}

/*void PicTimer::configureClock()
{
    m_prescaler = m_prescList.at( m_mode );
    m_clkSrc = clkMCU;
}

void PicTimer::configureExtClock()
{
    m_prescaler = 1;
    m_clkSrc = clkEXT;
    /// if     ( m_mode == 6 ) m_clkEdge = Clock_Falling;
    /// else if( m_mode == 7 ) m_clkEdge = Clock_Rising;
}*/

//--------------------------------------------------
// TIMER 8 Bit--------------------------------------

PicTimer8bit::PicTimer8bit( eMcu* mcu, QString name )
            : PicTimer( mcu, name )
{
    m_maxCount = 0xFF;
}
PicTimer8bit::~PicTimer8bit(){}


//--------------------------------------------------
// TIMER 0 -----------------------------------------

PicTimer0::PicTimer0( eMcu* mcu, QString name)
         : PicTimer8bit( mcu, name )
{
    m_T0CS = m_mcuRam->getRegBits( "T0CS" );
    m_T0SE = m_mcuRam->getRegBits( "T0SE" );
    m_PSA  = m_mcuRam->getRegBits( "PSA" );
    m_PS   = m_mcuRam->getRegBits( "PS0,PS1,PS2" );
}
PicTimer0::~PicTimer0(){}

void PicTimer0::initialize()
{
    PicTimer::initialize();

    m_running = true;
    sheduleEvents();
}

void PicTimer0::configureA() // OPTION
{
    uint8_t ps = m_PS.getRegBitsVal();

    if( m_PSA.getRegBitsBool() )
         m_prescaler = 1;                    // Prescaler asigned to Watchdog
    else m_prescaler = m_prescList.at( ps ); // Prescaler asigned to TIMER0

    m_scale = m_prescaler*m_mcu->psInst();

    m_clkEdge = m_T0SE.getRegBitsVal();

    uint8_t mode =  m_T0CS.getRegBitsVal();
    if( mode != m_mode )
    {
        m_mode = mode;
        //enable( mode==0 );
        enableExtClock( mode );
    }
    else sheduleEvents();
}

//--------------------------------------------------
// TIMER 2 -----------------------------------------

PicTimer2::PicTimer2( eMcu* mcu, QString name)
         : PicTimer8bit( mcu, name )
{
    m_PR2 = (uint8_t*)m_mcuRam->getReg("PR2");
    m_TMR2ON =  m_mcuRam->getRegBits( "TMR2ON" );
    m_T2CKPS =  m_mcuRam->getRegBits( "T2CKPS0,T2CKPS1" );
    m_TOUTPS =  m_mcuRam->getRegBits( "TOUTPS0,TOUTPS1,TOUTPS2,TOUTPS3" );
}
PicTimer2::~PicTimer2(){}

void PicTimer2::configureA() // T2CON
{
    uint8_t presc = m_T2CKPS.getRegBitsVal();
    uint8_t postc = m_TOUTPS.getRegBitsVal();
    m_prescaler = m_prescList.at( presc ) * (postc+1);
    m_scale     = m_prescaler*m_mcu->psInst();

    bool en = m_TMR2ON.getRegBitsBool();
    if( en != m_running ) enable( en );
}

void PicTimer2::configureB() // PR2
{
    m_ovfMatch  = *m_PR2;
    m_ovfPeriod = m_ovfMatch + 1;
}

//--------------------------------------------------
// TIMER 16 Bit-------------------------------------


PicTimer16bit::PicTimer16bit( eMcu* mcu, QString name )
             : PicTimer( mcu, name )
{
    m_maxCount = 0xFFFF;

    m_T1CKPS  = m_mcuRam->getRegBits("T1CKPS0,T1CKPS1");
    m_T1OSCEN = m_mcuRam->getRegBits("T1OSCEN");
    m_T1SYNC  = m_mcuRam->getRegBits("T1SYNC");
    m_TMR1ON  = m_mcuRam->getRegBits("TMR1ON");
}
PicTimer16bit::~PicTimer16bit(){}

void PicTimer16bit::configureA() // T1CON
{
    m_t1sync = m_T1SYNC.getRegBitsVal() ? 0 : 1; // Used for sleep mode

    uint8_t ps = m_T1CKPS.getRegBitsVal();

    m_prescaler = m_prescList.at( ps );

    m_mode  = m_TMR1CS.getRegBitsVal();
    m_t1Osc = m_T1OSCEN.getRegBitsBool() && m_mode; // T1 osc depends on TMR1CS

    configureClock();

    bool en = m_TMR1ON.getRegBitsBool();
    if( en != m_running ) enable( en );
}

void PicTimer16bit::sheduleEvents()
{
    if( m_running && m_t1Osc ) /// TODO: RC oscillator // 32.768 KHz Oscillator
    {
        uint64_t circTime = Simulator::self()->circTime();
        m_scale = 30517578; // Sim cycs per Timer tick for 32.768 KHz

        uint32_t ovfPeriod = m_ovfPeriod;
        if( m_countVal > m_ovfPeriod ) ovfPeriod += m_maxCount;

        uint64_t cycles = (ovfPeriod-m_countVal)*m_scale; // cycles in ps
        m_ovfCycle = circTime + cycles;// In simulation time (ps)

        Simulator::self()->cancelEvents( this );
        Simulator::self()->addEvent( cycles, this );
    }
    else McuTimer::sheduleEvents();
}

void PicTimer16bit::sleep( int mode )
{
    m_sleepMode = m_mode ? (m_t1sync & m_mode) : 0xFF;
    McuTimer::sleep( mode );
}

//--------------------------------------------------
// TIMER 1 -----------------------------------------

PicTimer160::PicTimer160( eMcu* mcu, QString name)
           : PicTimer16bit( mcu, name )
{
    m_TMR1CS = m_mcuRam->getRegBits("TMR1CS");
}
PicTimer160::~PicTimer160(){}

void PicTimer160::configureClock()
{
    m_scale = m_prescaler*m_mcu->psInst();
    enableExtClock( m_mode == 1 );
}

//--------------------------------------------------
// TIMER 1 16f1826 ---------------------------------

PicTimer161::PicTimer161( eMcu* mcu, QString name)
           : PicTimer16bit( mcu, name )
{
    m_TMR1CS = m_mcuRam->getRegBits("TMR1CS0,TMR1CS1");
}
PicTimer161::~PicTimer161(){}

void PicTimer161::configureClock()
{
    switch( m_mode ) {
    case 0:
        m_scale = m_prescaler*m_mcu->psInst();
        enableExtClock( false );
        break;
    case 1:
        m_scale = m_prescaler*m_mcu->psInst()/4;
        enableExtClock( false );
        break;
    case 2:
        if( m_t1Osc ) ; /// TODO: If T1OSCEN = 1: Crystal oscillator on T1OSI/T1OSO pins
        else          enableExtClock( true );
        break;
    case 3: break;
        /// TODO: Timer1 clock source is Capacitive Sensing Oscillator (CAPOSC)
    }
}
