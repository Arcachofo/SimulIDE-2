/***************************************************************************
 *   Copyright (C) 2020 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#ifndef MCUTIMER_H
#define MCUTIMER_H

#include<QHash>

#include "mcuprescaled.h"
#include "e-element.h"

class eMcu;
class McuPin;
class McuOcUnit;
class McuIcUnit;

class MAINMODULE_EXPORT McuTimer : public McuPrescaled, public eElement
{
        friend class McuCreator;

    public:
        McuTimer( eMcu* mcu, QString name );
        ~McuTimer();

        enum clkSource_t{
            clkMCU=0,
            clkEXT
        };

        virtual void initialize() override;
        virtual void runEvent() override;
        virtual void voltChanged() override;

        virtual void resetTimer();

        virtual void enable( uint8_t en );
        virtual void countWriteL( uint8_t val );
        virtual void countWriteH( uint8_t val );

        virtual void updtCount( uint8_t val=0 );

        virtual void addOcUnit( McuOcUnit* ocUnit ) { m_ocUnit.emplace_back( ocUnit ); }
        virtual McuOcUnit* getOcUnit( QString name ) {return NULL;}

        virtual void topReg0Changed( uint8_t val ){;}

        void enableExtClock( bool en );
        bool extClocked() { return m_extClock; }

        uint32_t getCount();
        QString  name()     { return m_name; }
        uint64_t scale()    { return m_scale; }
        uint16_t ovfMatch() { return m_ovfMatch; }
        bool     reverse()  { return m_reverse; }

    protected:
        virtual void sheduleEvents();
        virtual void updtCycles();
        void clockStep();

        int     m_number;

        uint64_t m_scale;                   // Picoseconds per timer Tick

        bool m_running;  // is Timer running?
        bool m_bidirec;  // is Timer bidirectional?
        bool m_reverse;  // is Timer counting backwards?
        bool m_extClock;

        uint8_t* m_countL; // Actual ram for counter Low byte
        uint8_t* m_countH; // Actual ram for counter High byte

        uint32_t m_countVal;   // Value of counter
        uint32_t m_countStart; // Value of counter after ovf

        uint16_t m_maxCount;   // Maximum value of the counter
        uint16_t m_ovfMatch;   // counter vale to match an overflow
        uint32_t m_ovfPeriod;  // overflow period
        uint64_t m_ovfCycle;   // absolute cycle of next overflow

        uint8_t m_mode;        // Can be used by each Timer as they want

        uint8_t* m_topReg0L;   // Register used as Top Low byte
        uint8_t* m_topReg0H;   // Register used as Top High byte

        //clkSource_t m_clkSrc;   // Source of Timer clock
        uint8_t     m_clkEdge;  // Clock edge in ext pin clock
        bool        m_clkState; // Lask Clock state
        McuPin*     m_clockPin; // External Clock pin

        McuIcUnit* m_ICunit;    // Input Capture unit;
        std::vector<McuOcUnit*> m_ocUnit; // Output Compare Units
};

#endif
