/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "rectangle.h"

#define tr(str) simulideTr("Rectangle",str)

listItem_t Rectangle::libraryItem(){
    return {
        tr("Rectangle"),
        "Graphical",
        "rectangle.png",
        "Rectangle",
        [](QString id){ return (Component*)new Rectangle("Rectangle", id ); } };
}

Rectangle::Rectangle( QString type, QString id )
         : Shape( type, id )
{
}
Rectangle::~Rectangle(){}

void Rectangle::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    QPen pen(Qt::black, m_border, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p->setPen( pen );
    
    double opacity = p->opacity();
    p->setOpacity( opacity*m_opac );
    p->fillRect( m_area, m_color );
    p->setOpacity( opacity );

    if( m_border > 0 )
    {
        p->setBrush( Qt::transparent );
        p->drawRect( m_area );
    }
    Component::paintSelected( p );
}
