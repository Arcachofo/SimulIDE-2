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

#include "ldr.h"
#include "itemlibrary.h"

Component* Ldr::construct( QObject* parent, QString type, QString id )
{ return new Ldr( parent, type, id ); }

LibraryItem* Ldr::libraryItem()
{
    return new LibraryItem(
            tr( "LDR" ),
            tr( "Resistive Sensors" ),
            "ldr.png",
            "LDR",
            Ldr::construct);
}

Ldr::Ldr( QObject* parent, QString type, QString id )
   : VarResBase( parent, type, id  )
{
    m_unit = "Lux";

    m_r1    = 127410;
    m_gamma = 0.8582;

    m_dial->setMinimum(1);

    Component::setValue( 1 );
    eResistor::setResSafe( m_r1 );
}
Ldr::~Ldr(){}

QList<propGroup_t> Ldr::propGroups()
{
    propGroup_t mainGroup { tr("Main") };
    //mainGroup.propList.append( {"Value_Ohm", tr("Current Value"),"main"} );
    mainGroup.propList.append( {"Lux", tr("Current Value"),"main"} );
    mainGroup.propList.append( {"Min_Lux", tr("Minimum Value"),"Lux"} );
    mainGroup.propList.append( {"Max_Lux", tr("Maximum Value"),"Lux"} );
    mainGroup.propList.append( {"Dial_Step", tr("Dial Step"),"Lux"} );
    mainGroup.propList.append( {"Gamma", tr("Gamma"),""} );
    mainGroup.propList.append( {"R1", tr("R1"),"Ω"} );
    return {mainGroup};
}

void Ldr::updateStep()
{
    if( m_changed )
    {
        //double res = double( m_maxRes-(m_maxRes-m_minRes)*val/1000 );
        double res = double(m_r1)*pow( double(m_value), -m_gamma );

        eResistor::setResSafe( res );
        m_changed = false;
        //qDebug()<<"Ldr::updateStep"<<m_resist;
    }
}

void Ldr::setR1( int r1 )
{
    m_r1 = r1;
    m_changed = true;
}

void Ldr::setGamma( double ga )
{
    m_gamma = ga;
    m_changed = true;
}

void Ldr::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    if( m_hidden ) return;

    Component::paint( p, option, widget );

    p->drawRect( -10.5, -4, 21, 8 );
    
    p->drawLine(-5,-11,-1,-7 );
    p->drawLine(-1, -7,-1,-9 );
    p->drawLine(-1, -7,-3,-7 );

    p->drawLine( 1,-11, 5,-7 );
    p->drawLine( 5, -7, 5,-9 );
    p->drawLine( 5, -7, 3,-7 );
}

#include "moc_ldr.cpp"
