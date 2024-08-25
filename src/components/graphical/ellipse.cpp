/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "ellipse.h"

#define tr(str) simulideTr("Ellipse",str)

listItem_t Ellipse::libraryItem(){
    return {
        tr("Ellipse"),
        "Graphical",
        "ellipse.png",
        "Ellipse",
        [](QString type, QString id){ return (Component*)new Ellipse( type, id ); } };
}

Ellipse::Ellipse( QString type, QString id )
       : Shape( type, id )
{
}
Ellipse::~Ellipse(){}

QPainterPath Ellipse::shape() const
{
    QPainterPath path;
    path.addEllipse( Shape::boundingRect() );
    return path;
}

void Ellipse::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    QPen pen(Qt::black, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p->setPen( pen );
    p->setBrush( m_color );

    double opacity = p->opacity();
    p->setOpacity( opacity*m_opac );
    p->drawEllipse( m_area );
    p->setOpacity( opacity );

    if( m_border > 0 )
    {
        QPen pen(Qt::black, m_border, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p->setPen( pen );
        p->setBrush( Qt::transparent );
        p->drawEllipse( m_area );
    }
    Component::paintSelected( p );
}
