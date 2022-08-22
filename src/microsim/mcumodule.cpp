/***************************************************************************
 *   Copyright (C) 2021 by santiago González                               *
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

#include "mcumodule.h"
#include "e_mcu.h"
#include "mcuinterrupts.h"

McuModule::McuModule( eMcu* mcu, QString name )
{
    m_mcu = mcu;
    m_name = name;
    m_sleepMode = 0;
    m_interrupt = NULL;
}
McuModule::~McuModule( ){}

void McuModule::sleep( int mode )
{
    if( mode < 0 ) m_sleeping = false;
    else           m_sleeping = (m_sleepMode & 1<<mode) > 0;
}

/*void McuModule::reset()
{
    Simulator::self()->cancelEvents( this );
}*/

/*void McuModule::raiseInt()
{
    if( m_interrupt ) m_interrupt->raise();
}*/
