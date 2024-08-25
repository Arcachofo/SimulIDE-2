/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "resistor.h"
#include "pin.h"

#include "doubleprop.h"

#define tr(str) simulideTr("Resistor",str)

listItem_t Resistor::libraryItem(){
    return {
        tr("Resistor"),
        "Resistors",
        "resistor.png",
        "Resistor",
        [](QString type, QString id){ return (Component*)new Resistor( type, id ); } };
}

Resistor::Resistor( QString type, QString id )
        : Comp2Pin( type, id )
        , eResistor( id )
{
    m_ePin[0] = m_pin[0];
    m_ePin[1] = m_pin[1];

    m_pin[0]->setLength( 5 );
    m_pin[1]->setLength( 5 );

    addPropGroup( { tr("Main"), {
        new DoubProp<Resistor>("Resistance", tr("Resistance"), "Ω"
                              , this, &Resistor::resistance, &Resistor::setResistance )
    }, 0 } );

    setShowProp("Resistance");
    setPropStr("Resistance", "100");
}
Resistor::~Resistor(){}

void Resistor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );
    p->drawRect( m_area );

    Component::paintSelected( p );
}
