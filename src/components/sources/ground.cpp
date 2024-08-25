/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "ground.h"
#include "iopin.h"

#define tr(str) simulideTr("Ground",str)

listItem_t Ground::libraryItem(){
    return {
        tr("Ground (0 V)"),
        "Sources",
        "ground.png",
        "Ground",
        [](QString type, QString id){ return (Component*)new Ground( type, id ); } };
}

Ground::Ground( QString type, QString id )
      : Component( type, id )
{
    m_area = QRect(-8,-10, 16, 12 );

    m_pin.resize(1);
    IoPin* pin = new IoPin( 90, QPoint( 0,-16 ), id+"-Gnd", 0, this, source );
    pin->setOutState( false );
    m_pin[0] = pin;
    
    setLabelPos(-16, 8, 0);
}
Ground::~Ground() {}

void Ground::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Component::paint( p, option, widget );

    QPen pen = p->pen();
    pen.setWidthF( 2.5 );
    p->setPen(pen);

    p->drawLine(QLineF(-6.6,-8, 6.6,-8) );
    p->drawLine(QLineF(-4.3,-4, 4.3,-4) );
    p->drawLine(QLineF(-1.9, 0, 1.9, 0) );

    Component::paintSelected( p );
}
