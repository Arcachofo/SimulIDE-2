/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "fixedvolt.h"
#include "circuit.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "iopin.h"
#include "custombutton.h"

#include "doubleprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("FixedVolt",str)

Component* FixedVolt::construct( QString type, QString id )
{ return new FixedVolt( type, id ); }

LibraryItem* FixedVolt::libraryItem()
{
    return new LibraryItem(
        tr("Fixed Voltage"),
        "Sources",
        "voltage.png",
        "Fixed Voltage",
        FixedVolt::construct );
}

FixedVolt::FixedVolt( QString type, QString id )
          : Component( type, id )
          , eElement( id )
{
    m_graphical = true;
    m_changed   = false;

    m_pin.resize(1);
    m_pin[0] = m_outpin = new IoPin( 0, QPoint(16,0), id+"-outnod", 0, this, source );

    setValLabelPos(-16, 8 , 0 ); // x, y, rot 
    setLabelPos(-64,-24 );

    m_button = new CustomButton();
    m_button->setCheckable( true );

    m_proxy = Circuit::self()->addWidget( m_button );
    m_proxy->setParentItem( this );

    setSmall( false );

    setVolt( 5.0 );
    
    Simulator::self()->addToUpdateList( this );

    QObject::connect( m_button, &CustomButton::clicked, [=](){ onbuttonclicked(); } );

    addPropGroup( { tr("Main"), {
        new DoubProp<FixedVolt>("Voltage", tr("Voltage"), "V"
                               , this, &FixedVolt::volt, &FixedVolt::setVolt ),

        new BoolProp<FixedVolt>("Small", tr("Small size"), ""
                           , this, &FixedVolt::isSmall, &FixedVolt::setSmall )
    }, 0} );

    addPropGroup( { "Hidden", {
        new BoolProp<FixedVolt>( "Out", "",""
                        , this, &FixedVolt::out, &FixedVolt::setOut ),
    }, groupHidden} );
}
FixedVolt::~FixedVolt(){}

void FixedVolt::stamp()
{
    if( !Simulator::self()->isPaused() ) m_changed = true;
}

void FixedVolt::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;
    m_outpin->setOutState( m_button->isChecked() );
    update();
}

void FixedVolt::onbuttonclicked()
{
    if( Simulator::self()->isRunning() ) m_changed = true;
    else{
        m_outpin->setOutState( m_button->isChecked() );
        update();
    }
}

void FixedVolt::updateOutput()
{
    m_outpin->setOutHighV( m_voltage );
    m_changed = true;
}

bool FixedVolt::out() { return m_button->isChecked(); }

void FixedVolt::setOut( bool out )
{
    m_button->setChecked( out );
    onbuttonclicked();
}

void FixedVolt::setVolt( double v )
{
    m_voltage = v;
    m_outpin->setOutHighV( m_voltage );
    m_changed = true;
}

void FixedVolt::setSmall( bool s )
{
    m_small = s;

    if( s ){
        m_button->setMaximumSize( 9, 9 );
        m_button->setGeometry(-5,-5, 9, 9);
        m_proxy->setPos( QPointF(-8,-4.5) );
        m_area = QRect( 4, -4, 8, 8 );
    }else{
        m_button->setMaximumSize( 16, 16 );
        m_button->setGeometry(-20,-16, 16, 16);
        m_proxy->setPos( QPoint(-32,-8) );
        m_area = QRect(-10,-10, 20, 20 );
    }
    Circuit::self()->update();
}

void FixedVolt::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    if( m_hidden ) return;

    Component::paint( p, o, w );

    if( m_button->isChecked() ) p->setBrush( QColor( 255, 166, 0 ) );
    else                        p->setBrush( QColor( 230, 230, 255 ) );

    if( m_small ) p->drawRoundedRect( QRectF( 4,-4,  8,  8 ), 2, 2);
    else          p->drawRoundedRect( QRectF(-8,-8, 16, 16 ), 2, 2);

    Component::paintSelected( p );
}
