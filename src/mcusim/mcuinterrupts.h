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

#ifndef MCUINTERRUPTS_H
#define MCUINTERRUPTS_H

#include <QString>
#include <QHash>
#include <map>

class eMcu;
class Interrupts;

class MAINMODULE_EXPORT Interrupt
{
        friend class McuCreator;

    public:
        Interrupt( QString name, uint16_t vector, eMcu* mcu );
        virtual ~Interrupt();

        virtual void reset();
        virtual void raise( uint8_t v );
        virtual void execute();
        virtual void exitInt();

        void enableFlag( uint8_t en );
        void setPriority( uint8_t p );
        void setMode( uint8_t mod ){ m_mode = mod;}
        //void readFlag( uint8_t p );

        void clearFlag();

        uint8_t priority() { return m_priority; }

    protected:
        eMcu* m_mcu;
        uint8_t* m_ram;

        Interrupts* m_interrupts;

        QString  m_name;
        uint8_t  m_number;
        uint16_t m_vector;
        uint8_t  m_mode;

        uint8_t m_enable;

        uint8_t  m_flagMask;
        uint16_t m_flagReg;

        uint8_t m_priority;
        uint8_t m_raised;
};

//------------------------               ------------------------
//---------------------------------------------------------------
class MAINMODULE_EXPORT Interrupts
{
        friend class McuCreator;

    public:
        Interrupts( eMcu* mcu );
        virtual ~Interrupts();

       void enableGlobal( uint8_t en );
       void runInterrupts();
       void retI();
       void remove();
       void resetInts();
       void addToPending( uint8_t pri, Interrupt* i );

    protected:
        eMcu* m_mcu;

        uint8_t    m_enGlobal;   // Global Interrupt Flag
        Interrupt* m_active;     // Active interrupt

        std::multimap<uint8_t, Interrupt*> m_running; // Interrups that were interrupted
        std::multimap<uint8_t, Interrupt*> m_pending; // Interrupts pending to service
        QHash<QString, Interrupt*> m_intList;      // Access Interrupts by name
};

#endif
