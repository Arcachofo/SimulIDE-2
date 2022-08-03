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

#include "i51pin.h"
#include "iopin.h"
#include "simulator.h"
#include "datautils.h"
#include "mcuinterrupts.h"

I51Pin::I51Pin( McuPort* port, int i, QString id, Component* mcu )
      : McuPin( port, i, id, mcu )
{
    /// m_changeCB = false; // Call VoltChanged() only for Inputs
}
I51Pin::~I51Pin() {}

/*void I51Pin::initialize()
{
    m_outCtrl = false;
    m_dirCtrl = false;
    m_isAnalog = false;

    m_step = 0;
    m_steps = Simulator::self()->slopeSteps();
    //IoPin::initialize();

    //m_nextState = false;
    //m_outState  = false;
    //m_portState = false;
    //m_isOut     = false;
}*/

void I51Pin::setOutState( bool state )
{
    state = state && m_portState;
    IoPin::setOutState( state );
}

void I51Pin::ConfExtInt( uint8_t bits )
{
    bool fallEdge = getRegBitsBool( bits, m_extIntBits );
    m_extIntTrigger = fallEdge ? McuPin::pinFalling : McuPin::pinLow;
    m_extInt->setAutoClear( fallEdge );
}
