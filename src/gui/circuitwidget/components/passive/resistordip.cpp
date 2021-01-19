/***************************************************************************
 *   Copyright (C) 2016 by santiago González                               *
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

#include "resistordip.h"
#include "circuitwidget.h"
#include "itemlibrary.h"
#include "connector.h"
#include "simulator.h"
#include "circuit.h"
#include "pin.h"


Component* ResistorDip::construct( QObject* parent, QString type, QString id )
{ return new ResistorDip( parent, type, id ); }

LibraryItem* ResistorDip::libraryItem()
{
    return new LibraryItem(
            tr( "ResistorDip" ),
            tr( "Resistors" ),
            "resistordip.png",
            "ResistorDip",
            ResistorDip::construct);
}

ResistorDip::ResistorDip( QObject* parent, QString type, QString id )
           : Component( parent, type, id )
           , eResistorDip( id )
{
    m_size = 0;
    setSize( 8 );
    
    m_unit = "Ω";
    setResist( 100 );
    setValLabelX( 5 );
    setValLabelY(-26 );
    setValLabRot( 90 );
    setValLabelPos();
    //m_valLabel->setEnabled( false );
    m_valLabel->setAcceptedMouseButtons( 0 );
    setShowVal( true );
    
    setLabelPos(-24,-40, 0);
}
ResistorDip::~ResistorDip(){}

QList<propGroup_t> ResistorDip::propGroups()
{
    propGroup_t mainGroup { tr("Main") };
    mainGroup.propList.append( {"Resistance", tr("Resistance"),"main"} );
    mainGroup.propList.append( {"Size", tr("Size"),"Resistors"} );
    return {mainGroup};
}

void ResistorDip::createResistors( int c )
{
    int start = m_size;
    m_size = m_size+c;
    m_resistor.resize( m_size );
    m_pin.resize( m_size*2 );

    for( int i=start; i<m_size; i++ )
    {
        int index = i*2;
        QString reid = m_id;
        reid.append(QString("-resistor"+QString::number(i)));
        m_resistor[i] = new eResistor( reid );
        
        QPoint pinpos = QPoint(-16,-32+8+i*8 );
        Pin* pin = new Pin( 180, pinpos, reid+"-ePin"+QString::number(index), 0, this);
        m_resistor[i]->setEpin( 0, pin );
        m_pin[index] = pin;
        
        pinpos = QPoint( 16,-32+8+i*8 );
        pin = new Pin( 0, pinpos, reid+"-ePin"+QString::number(index+1), 0, this);
        m_resistor[i]->setEpin( 1, pin );
        m_pin[index+1] = pin;
    }
    //update();
}

void ResistorDip::deleteResistors( int d )
{
    if( d > m_size ) d = m_size;
    int start = m_size-d;

    for( int i=start*2; i<m_size*2; i++ )
    {
        Pin* pin = m_pin[i];
        if( pin->isConnected() ) pin->connector()->remove();
        
        delete pin;
    }
    for( int i=start; i<m_size; i++ ) delete m_resistor[i];
    m_size = m_size-d;
    m_resistor.resize( m_size );
    m_pin.resize( m_size*2 );
    //Circuit::self()->update();
}

int ResistorDip::size()
{
    return m_size;
}

void ResistorDip::setSize( int size )
{
    if( Simulator::self()->isRunning() )  CircuitWidget::self()->powerCircOff();
    
    if( size == 0 ) size = 8;
    
    if     ( size < m_size ) deleteResistors( m_size-size );
    else if( size > m_size ) createResistors( size-m_size );
    
    m_area = QRect( -10, -30, 20, m_size*8+4 );
    setValLabelY(-26 );

    Circuit::self()->update();
}

double ResistorDip::resist() { return m_value; }

void ResistorDip::setResist( double r )
{
    bool pauseSim = Simulator::self()->isRunning();
    if( pauseSim )  Simulator::self()->pauseSim();
    
    if( r == 0 ) r = 0.001;
    Component::setValue( r );       // Takes care about units multiplier
    
    setRes( m_value*m_unitMult );
    
    if( pauseSim ) Simulator::self()->resumeSim();
}

void ResistorDip::setUnit( QString un ) 
{
    bool pauseSim = Simulator::self()->isRunning();
    if( pauseSim )  Simulator::self()->pauseSim();
    
    Component::setUnit( un );
    setRes( m_value*m_unitMult );
    
    if( pauseSim ) Simulator::self()->resumeSim();
}

void ResistorDip::remove()
{
    deleteResistors( m_size );

    Component::remove();
}
void ResistorDip::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );
    
    //p->setBrush( QColor( 80, 80, 80) );

    p->drawRoundRect( QRect( -9, -28, 18, m_size*8 ), 2, 2 );
}

#include "moc_resistordip.cpp"

