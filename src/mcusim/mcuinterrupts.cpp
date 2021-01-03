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

#include "mcuinterrupts.h"
#include "mcucore.h"
#include "e_mcu.h"

Interrupt::Interrupt( QString name, uint16_t vector, eMcu* mcu )
{
    m_mcu = mcu;
    m_name = name;
    m_vector = vector;
    m_mode = 0;

    m_ram = mcu->getRam();
}
Interrupt::~Interrupt(){}

void Interrupt::reset()
{
    m_mode   = 0;
    m_raised = 0;
    m_enable = 0;
    m_priority = 0;
}

void Interrupt::clearFlag()
{
    m_ram[m_flagReg] &= ~m_flagMask; // Clear Interrupt flag
}

void Interrupt::enableFlag( uint8_t en )
{
    m_enable = en;
}

void Interrupt::setPriority( uint8_t p )
{
    m_priority = p;
}

void Interrupt::raise( uint8_t v )
{
    m_ram[m_flagReg] |= m_flagMask; // Set Interrupt flag

    if( !m_enable ) return;
    m_raised = true;

    // Interrupt are stored in static std::multimap
    // by priority order, highest at end
    m_interrupts->addToPending( m_priority, this ); // Add to pending interrupts
}

void Interrupt::execute()
{
    m_mcu->cpu->CALL_ADDR( m_vector );
}

void Interrupt::exitInt() // Exit from this interrupt
{
}

//------------------------               ------------------------
//---------------------------------------------------------------

Interrupts::Interrupts( eMcu* mcu )
{
    m_mcu = mcu;
    m_enGlobal = 0;
    m_active = 0l;
}
Interrupts::~Interrupts(){}

void Interrupts::retI()
{
    m_active->exitInt();

    if( m_running.size() ) // There are interrupts interrupted
    {
        auto it = prev( m_running.end() );
        m_active = it->second; // Set interrupted Interrupt as active
        m_running.erase( it ); // Remove Interrupt from running list
    }
    else m_active = NULL;
}

void Interrupts::enableGlobal( uint8_t en )
{
    m_enGlobal = en;
}

void Interrupts::runInterrupts()
{
    if( !m_enGlobal ) return;       // Global Interrupts disabled
    if( m_pending.empty() ) return; // No Interrupts to run

    auto it = prev( m_pending.end() );
    uint8_t     priority = it->first;
    Interrupt* interrupt = it->second;

    if( m_active ) // Only interrupt other Interrupts with lower priority
    {
        if( priority <= m_active->priority() ) return;

        // An interrupt being interrupted, add to running list.
        m_running.emplace( m_active->priority(), m_active );
    }
    m_pending.erase( it ); // Remove Interrupt from pending list
    interrupt->execute();
    m_active = interrupt;
}

void Interrupts::resetInts()
{
    m_active = NULL;
    m_pending.clear();
    m_running.clear();

    for( QString  inte : m_intList.keys() )
        m_intList.value( inte )->reset();
}

void Interrupts::remove()
{
    for( QString  inte : m_intList.keys() )
        delete m_intList.value( inte );

    m_intList.clear();
}

void Interrupts::addToPending( uint8_t pri, Interrupt* i )
{
    m_pending.emplace( pri, i );
}


