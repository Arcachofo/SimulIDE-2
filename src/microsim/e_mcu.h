/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef EMCU_H
#define EMCU_H

#include "e_iou.h"
#include "mcutimer.h"
#include "mcuuart.h"
#include "mcuinterrupts.h"
#include "mcudataspace.h"
#include "mcusleep.h"

enum{
    R_READ = 0,
    R_WRITE,
};

enum mcuState_t{
    mcuStopped=0,
    mcuError,
    mcuRunning,
    mcuSleeping
};

class Mcu;
class McuPort;
class Component;
class CpuBase;
class McuTimer;
class McuWdt;
class McuSleep;

class eMcu : public DataSpace, public eIou
{
        friend class McuCreator;
        friend class McuCpu;
        friend class Mcu;

    public:
        eMcu( Mcu* comp, QString id );
        ~eMcu();

 static eMcu* self() { return m_pSelf; }

        void setup();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override;

        inline mcuState_t state() { return m_state; }
        inline int sleepMode() { return m_sleepModule->mode(); }

        void stepCpu();

        void setDebugger( BaseDebugger* deb );
        void setDebugging( bool d );

        uint64_t cycle(){ return m_cycle; }

        void hardReset( bool r );
        void sleep( bool s );
        void start();

        //QString getFileName() { return m_firmware; }

        double vdd() { return m_vdd; }

        double freq() { return m_freq; }
        void setFreq( double freq );
        uint64_t psInst() { return m_psInst; }  // picoseconds per instruction cycle
        void setInstCycle( double p ){ m_cPerInst = m_cPerTick = p; }

        McuTimer* getTimer( QString name );
        McuPort* getMcuPort( QString name );
        McuPin*  getMcuPin( QString pinName );

        McuModule* getModule( QString name );

        void wdr();

        void enableInterrupts( uint8_t en );

        int cyclesDone;

        void setMain() { m_pSelf = this; }

        Interrupts* interrupts() { return &m_interrupts; }

    protected:
 static eMcu* m_pSelf;

        void reset();

        mcuState_t m_state;

        uint64_t m_cycle;

        std::vector<McuModule*> m_modules;
        std::vector<McuUsart*> m_usarts;

        QHash<QString, McuTimer*> m_timerList;// Access TIMERS by name
        QHash<QString, McuPort*>  m_mcuPorts; // Access PORTS by name

        McuSleep* m_sleepModule;
        McuWdt*   m_wdt;

        Interrupts m_interrupts;

        double m_vdd;

        double m_freq;         // Clock Frequency in MegaHerzs
        double m_cPerInst;     // Clock ticks per Instruction Cycle
        double m_cPerTick;     // Clock ticks  per cpu Cycle
        uint64_t m_psInst;     // picoseconds per Instruction Cycle
        uint64_t m_psTick;     // picoseconds per Instruction Cycle

        bool m_clkState;

        // Debugger:
        BaseDebugger* m_debugger;
        bool          m_debugging;
};

#endif
