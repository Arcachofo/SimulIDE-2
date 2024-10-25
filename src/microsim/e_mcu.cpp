/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "e_mcu.h"
#include "mcu.h"
#include "cpubase.h"
#include "mcuport.h"
#include "mcupin.h"
#include "ioport.h"
#include "iopin.h"
#include "mcuwdt.h"
#include "usartmodule.h"
#include "usartrx.h"
#include "simulator.h"
#include "basedebugger.h"
#include "editorwindow.h"

#include "mcumodule.h"

eMcu* eMcu::m_pSelf = nullptr;

eMcu::eMcu( Mcu* comp, QString id )
    //: DataSpace()
    : eIou( comp, id )
    , m_interrupts( this )
{
    m_wdt         = nullptr;
    m_sleepModule = nullptr;

    m_vdd= 5;

    m_freq = 0;
    m_cPerInst = 1;

    m_debugger = nullptr;
    m_debugging = false;

    ////m_ramTable = new RamTable( nullptr, this, false );
}

eMcu::~eMcu()
{
    if( m_cpu ) delete m_cpu;
    m_interrupts.remove();
    for( McuModule* module : m_modules ) delete module;
    if( m_pSelf == this ) m_pSelf = nullptr;
}

void eMcu::setup()
{
    for( McuModule* module : m_modules ) module->setup();
}

void eMcu::stamp()
{
    //reset();
    //m_state = mcuStopped;
    m_clkState = false;
}

void eMcu::voltChanged()  // External clock
{
    if( m_state == mcuStopped ) return;

    bool clkState = m_clkPin->getInpState();
    if( m_clkState == clkState ) return;
    m_clkState = clkState;

    if( m_debugging )
    {
        if( cyclesDone > 1 ) cyclesDone -= 1;
        else                 m_debugger->stepDebug();
    }
    else if( m_state >= mcuRunning /*&& m_freq > 0*/ )
    {
        m_cpu->extClock( clkState );
    }
}

void eMcu::runEvent()
{
    if( m_state != mcuRunning ) return;

    if( m_debugging )
    {
        if( cyclesDone > 1 ) cyclesDone -= 1;
        else                 m_debugger->stepDebug();
        Simulator::self()->addEvent( m_psTick, this );
    }
    else if( m_state >= mcuRunning && m_freq > 0 )
    {
        stepCpu();
        int cycles = cyclesDone;
        if( cycles == 0 ) cycles = 1;                        // 8051: 2 Read cycles per Machine cycle
        Simulator::self()->addEvent( cycles*m_psTick, this );
    }
}

void eMcu::stepCpu()
{
    if( true /*!m_flashSize || m_cpu->getPC() < m_flashSize*/ ) /// Fixme
    {
        if( m_state == mcuRunning ) m_cpu->runStep();
        m_interrupts.runInterrupts();
    }else{
        m_state = mcuError;
        m_component->crash( true );
        /// qDebug() << "eMcu::stepCpu: Error PC =" << m_cpu->getPC() << "PGM size =" << m_flashSize;
        qDebug() << "MCU stopped";
    }
    m_cycle += cyclesDone;
}

void eMcu::setDebugger( BaseDebugger* deb )
{
    m_debugger = deb;
    ///m_ramTable->setDebugger( deb );
}

void eMcu::setDebugging( bool d )
{
    m_debugger->m_prevLine.lineNumber = -1;
    m_debugging = d;
}

void eMcu::reset()
{
    m_component->crash( false );
    m_state = mcuStopped;
    m_cycle = 0;
    cyclesDone = 0;

    for( McuModule* module : m_modules  ) { module->reset(); module->sleep(-1 ); }
    for( IoPort*    ioPort : m_ioPorts  ) ioPort->reset();
    for( McuPort*  mcuPort : m_mcuPorts ) mcuPort->reset();

    m_interrupts.resetInts();
    ///DataSpace::initialize();

    if( m_cpu ) m_cpu->reset(); // Must be after all modules reset
    else qDebug() << "ERROR: eMcu::reset nullptr Cpu";

    for( McuPort* mcuPort : m_mcuPorts ) mcuPort->readPort(); // Update Pin Input register
}

void eMcu::hardReset( bool r )
{
    bool isReset = (m_state == mcuStopped);
    if( r == isReset ) return;

    Simulator::self()->cancelEvents( this );
    if( m_clkPin ) m_clkPin->changeCallBack( this, false );  // External clock

    if( r ) reset();
    else    start();
}

void eMcu::start()
{
    if( m_state == mcuRunning ) return;
    m_state = mcuRunning;

    if     ( m_clkPin   ) m_clkPin->changeCallBack( this, true );  // External clock
    else if( m_freq > 0 ) Simulator::self()->addEvent( m_psTick, this );
}

void eMcu::sleep( bool s )
{
    if( !m_sleepModule || !m_sleepModule->enabled() ) return;

    int mode = -1;
    if( s )                       // Go to Sleep
    {
        mode = m_sleepModule->mode();
        m_state = mcuSleeping;
        qDebug() << "eMcu::sleep: Sleeping";
    }else{
        m_state = mcuRunning;    // Wakeup
        runEvent();
        qDebug() << "eMcu::sleep: Wakeup";
    }

    for( McuModule* module : m_modules ) module->sleep( mode );
}

void eMcu::setFreq( double freq )
{
    double extFreq = m_component->extFreq(); // Mcu freq Property
    freq = extFreq ? extFreq : freq;         // Use property if Mcu freq Property is set

    if     ( freq < 0       ) freq = 0;
    else if( freq > 100*1e6 ) freq = 100*1e6;

    if( freq > 0 )
    {
        m_psInst = 1e12*(m_cPerInst/freq); // Set Simulation cycles per Instruction cycle
        m_psTick = 1e12*(m_cPerTick/freq); // Set Simulation cycles per Instruction cycle
        if( m_freq == 0 && m_state >= mcuRunning )// Previously stopped by freq = 0
        {
            Simulator::self()->cancelEvents( this );
            if( !m_clkPin   ) Simulator::self()->addEvent( m_psTick, this );
        }
    }
    m_freq = freq;
}

McuTimer* eMcu::getTimer( QString name )
{
    McuTimer* timer = m_timerList.value( name );
    if( !timer ) qDebug() << "ERROR: nullptr Timer:"<< name;
    return timer;
}

McuPort* eMcu::getMcuPort( QString name )
{
    McuPort* port = m_mcuPorts.value( name );
    /// if( !port ) qDebug() << "ERROR: nullptr Port:"<< name;
    return port;
}

McuPin* eMcu::getMcuPin( QString pinName )
{
    if( pinName.isEmpty() ) return nullptr;
    McuPin* pin = nullptr;

    for( McuPort* port : m_mcuPorts )
    {
        pin = port->getPin( pinName );
        if( pin ) break;
    }
    if( !pin ) qDebug() << "ERROR: eMcu::getMcuPin nullptr Pin:"<< pinName;
    return pin;
}

IoPin*  eMcu::getIoPin( QString pinName )
{
    if( pinName.isEmpty() || pinName == "0" ) return NULL;
    IoPin* pin = eIou::getIoPin( pinName );

    if( !pin ) pin = getMcuPin( pinName );
    if( !pin ) qDebug() << "ERROR: eMcu::getIoPin NULL Pin:"<< pinName;
    return pin;
}

void eMcu::wdr() { if( m_wdt ) m_wdt->reset(); }

/*void eMcu::enableInterrupts( uint8_t en )
{
    if( en > 1 ) en = 1;
    m_interrupts.enableGlobal( en );
}*/


McuModule* eMcu::getModule( QString name )
{
    name = name.toLower();
    for( McuModule* module : m_modules )
        if( module->name().toLower() == name ) return module;

    return nullptr;
}
