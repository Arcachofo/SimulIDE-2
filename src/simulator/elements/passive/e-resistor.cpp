/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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

#include "e-resistor.h"
#include "simulator.h"


eResistor::eResistor( QString id )
         : eElement( id )
{
    m_resist = 100;
    m_admit  = 1/m_resist;
    m_current = 0;
    m_ePin.resize(2);
}
eResistor::~eResistor(){}

void eResistor::stamp()
{
    m_ePin[0]->setEnodeComp( m_ePin[1]->getEnode() );
    m_ePin[1]->setEnodeComp( m_ePin[0]->getEnode() );
    stampAdmit();
}

void eResistor::stampAdmit()
{
    if( !m_ePin[0] ) return;

    m_ePin[0]->stampAdmitance( m_admit );
    m_ePin[1]->stampAdmitance( m_admit );
}

void eResistor::stampCurrent( double current )
{
    if( !m_ePin[0] ) return;

    m_ePin[0]->stampCurrent( current );
    m_ePin[1]->stampCurrent( -current );
}

double eResistor::res() 
{ 
    return m_resist; 
}

void eResistor::setRes( double resist )
{
    if( resist < 1e-12 ) resist = 1e-12;
    
    m_admit = 1/resist;
    m_resist = resist;
    
    stampAdmit();
}

double eResistor::admit()
{
    return m_admit;
}

void eResistor::setAdmit( double admit )               // Admit can be 0 
{
    m_admit = admit;
    stampAdmit();
}

void eResistor::setResSafe( double resist )
{
    Simulator::self()->pauseSim();
    setRes( resist );
    Simulator::self()->resumeSim();
}

double eResistor::current()
{
    updateVI();
    return m_current;
}

void eResistor::updateVI()
{
    if( m_ePin[0]->isConnected() && m_ePin[1]->isConnected() )
    {
        double volt = m_ePin[0]->getVolt()-m_ePin[1]->getVolt();

        m_current = volt/m_resist;
    }
    else m_current = 0;
}

