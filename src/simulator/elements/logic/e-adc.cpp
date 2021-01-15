/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
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

#include "e-adc.h"
#include "simulator.h"

eADC::eADC( QString id )
    : eLogicDevice( id )
{
}
eADC::~eADC()
{ 
}

void eADC::stamp()
{
    eNode* enode = m_input[0]->getEpin(0)->getEnode();
    if( enode ) enode->voltChangedCallback( this );

    eLogicDevice::stamp();
}

void eADC::voltChanged()
{
    double volt = m_input[0]->getVolt();
    m_value = (int)(volt*m_maxValue/m_maxVolt+0.1);

    Simulator::self()->addEvent( m_propDelay, this );
}

void eADC::runEvent()
{
    for( int i=0; i<m_numOutputs; ++i )
    {
        //qDebug() << "eADC::setVChanged" << i << (address & 1);
        m_output[m_numOutputs-1-i]->setTimedOut( m_value & 1 );
        m_value >>= 1;
    }
}
