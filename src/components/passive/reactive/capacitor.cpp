/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "capacitor.h"

#define tr(str) simulideTr("Capacitor",str)

listItem_t Capacitor::libraryItem(){
    return {
        tr("Capacitor"),
        "Reactive",
        "capacitor.png",
        "Capacitor",
        [](QString id){ return (Component*)new Capacitor("Capacitor", id ); } };
}

Capacitor::Capacitor( QString type, QString id )
         : CapacitorBase( type, id )
{}
Capacitor::~Capacitor(){}

void Capacitor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    QPen pen = p->pen();
    pen.setWidth(3);
    p->setPen(pen);

    p->drawLine(-3,-6,-3, 6 );
    p->drawLine( 3,-6, 3, 6 );

    Component::paintSelected( p );
}
