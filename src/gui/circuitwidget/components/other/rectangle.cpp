/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
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

#include "rectangle.h"

Component* Rectangle::construct( QObject* parent, QString type, QString id )
{ return new Rectangle( parent, type, id ); }

LibraryItem* Rectangle::libraryItem()
{
    return new LibraryItem(
        QObject::tr( "Rectangle" ),
        QObject::tr( "Graphical" ),
        "rectangle.png",
        "Rectangle",
        Rectangle::construct);
}

Rectangle::Rectangle( QObject* parent, QString type, QString id )
         : Shape( parent, type, id )
{
}
Rectangle::~Rectangle(){}

void Rectangle::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Component::paint( p, option, widget );

    QPen pen(Qt::black, m_border, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p->setPen( pen );
    
    p->setOpacity( m_opac );
    p->fillRect( m_area, m_color );
    p->setOpacity( 1 );

    if( m_border > 0 )
    {
        p->setBrush( Qt::transparent );
        p->drawRect( m_area );
    }
}
