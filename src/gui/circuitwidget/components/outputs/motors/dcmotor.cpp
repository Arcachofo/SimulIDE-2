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

#include <QPainter>
#include <math.h>

#include "dcmotor.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "pin.h"
#include "label.h"

#include "doubleprop.h"
#include "intprop.h"

Component* DcMotor::construct( QObject* parent, QString type, QString id )
{ return new DcMotor( parent, type, id ); }

LibraryItem* DcMotor::libraryItem()
{
    return new LibraryItem(
        tr("Dc Motor"),
        tr("Motors"),
        "dcmotor.png",
        "DcMotor",
        DcMotor::construct );
}

DcMotor::DcMotor( QObject* parent, QString type, QString id )
        : Component( parent, type, id )
        , eResistor( id )
{
    m_graphical = true;
    
    m_area = QRectF( -35,-33, 70, 66 );
    m_color = QColor( 50, 50, 70 );
    m_ang  = 0;
    m_speed = 0;
    m_voltNom = 5;
    setRpm( 60 );

    m_pin.resize( 2 );
    m_ePin[0] = m_pin[0] = new Pin( 180, QPoint(-40,0), m_id+"-lPin", 0, this);
    m_pin[0]->setLength( 5 );
    m_pin[0]->setLabelText("+");

    m_ePin[1] =m_pin[1] = new Pin( 0, QPoint(40,0), m_id+"-rPin", 1, this);
    m_pin[1]->setLength( 5 );
    m_pin[1]->setLabelText("-");

    setShowId( true );
    setLabelPos(-22,-48, 0);
    setValLabelPos(-14, 36, 0);

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"), {
new IntProp <DcMotor>( "RPM_Nominal" , tr("Nominal Speed")  ,tr("_RPM"), this, &DcMotor::rpm,    &DcMotor::setRpm ),
new DoubProp<DcMotor>( "Volt_Nominal", tr("Nominal Voltage"),"V"   , this, &DcMotor::volt,   &DcMotor::setVolt ),
new DoubProp<DcMotor>( "Resistance"  , tr("Resistance")     ,"Ω"   , this, &DcMotor::getRes, &DcMotor::setResSafe )
    }} );
}
DcMotor::~DcMotor(){}

void DcMotor::initialize()
{
    m_ang = 0;
    m_speed = 0;
    m_delta = 0;
    m_lastTime = 0;
    m_updtTime = 0;
    m_LastVolt = 0;
}

void DcMotor::stamp()
{
     if( m_ePin[0]->isConnected() && m_ePin[1]->isConnected() )
     {
         m_ePin[0]->changeCallBack( this );
         m_ePin[1]->changeCallBack( this );
     }
     eResistor::stamp();
}

void DcMotor::updateStep()
{
    updatePos();

    if( m_updtTime ) m_speed = m_delta/(m_updtTime/1e12);

    m_ang += m_motStPs*m_delta;
    m_ang = remainder( m_ang, (16.0*360.0) );

    m_delta = 0;
    m_updtTime = 0;
    update();
}

void DcMotor::voltChanged() { updatePos(); }

void DcMotor::updatePos()
{
    uint64_t timePS = Simulator::self()->circTime();
    uint64_t duration = timePS-m_lastTime;
    m_updtTime += duration;
    m_lastTime = timePS;

    m_delta += (m_LastVolt/m_voltNom)*(duration/1e12);
    m_LastVolt = m_ePin[1]->getVolt() - m_ePin[0]->getVolt();
}

void DcMotor::setRpm( int rpm )
{
    if( rpm < 1 ) rpm = 1;
    m_rpm = rpm;
    m_motStPs = 16*360*rpm/60;

    update();
}

void DcMotor::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );

    p->setBrush( QColor(50, 70, 100) );
    p->drawEllipse(-32,-32, 64, 64 );

    p->setBrush( QColor(255, 0, 0) );
    p->drawRoundRect(-36,-4, 8, 8, 4, 4 );
    p->setBrush( QColor(0, 0, 0) );
    p->drawRoundRect( 28,-4, 8, 8, 4, 4 );

    p->setPen( QColor(0, 0, 0) );
    p->setBrush( QColor(255, 255, 255) );
    p->drawEllipse(-26,-26, 52, 52 );

    // rotating pointer
    p->setPen ( QColor(0, 0, 0) );
    p->setBrush( QColor(50, 70, 100) );
    p->drawPie(-24,-24, 48, 48, m_ang-120, 240 );

    p->setBrush( QColor(50, 70, 100) );
    p->drawEllipse(-20,-20, 40, 40 );

    if( m_speed != 0 ) // Speed and Direction Indicator
    {
        double speed = m_speed;
        double exedd = 0;
        if( m_speed > 1 ){
            exedd = m_speed -1;
            speed = 1;
        }
        else if( m_speed < -1 ){
            exedd = m_speed +1;
            speed = -1;
        }
        p->setPen ( QColor(50, 70, 100) );
        p->setBrush( QColor(100, 200, 70) );
        p->drawPie(-20,-20, 40, 40, 16*90, speed*16*180 );

        p->setBrush( QColor(200, 100, 70) );
        p->drawPie(-20,-20, 40, 40, -16*90, exedd*16*180 );
}   }
