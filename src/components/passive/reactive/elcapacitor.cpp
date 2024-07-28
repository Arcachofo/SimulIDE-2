/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "elcapacitor.h"
#include "simulator.h"
#include "e-pin.h"

#define tr(str) simulideTr("elCapacitor",str)

listItem_t elCapacitor::libraryItem(){
    return {
        tr("Electrolytic Capacitor"),
        "Reactive",
        "elcapacitor.png",
        "elCapacitor",
        [](QString id){ return (Component*)new elCapacitor("elCapacitor", id ); } };
}

elCapacitor::elCapacitor( QString type, QString id )
           : CapacitorBase( type, id )
{
    Simulator::self()->addToUpdateList( this );
}
elCapacitor::~elCapacitor(){}

void elCapacitor::updateStep()
{
    double volt = m_ePin[0]->getVoltage() - m_ePin[1]->getVoltage();
    bool crashed = ( volt < -1e-6 );
    if( crashed || (crashed != m_crashed) )
    {
        m_crashed = crashed;
        update();
}   }

void elCapacitor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w);

    QPen pen = p->pen();
    pen.setWidth(3);
    p->setPen(pen);
    
    p->drawLine( 3,-7, 3, 7 );
    p->drawLine(-3,-7, 3,-7 );
    p->drawLine(-3, 7, 3, 7 );
    p->drawLine(-3,-3,-3, 3 );

    Component::paintSelected( p );
}
