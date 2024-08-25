/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>
#include <QGraphicsProxyWidget>

#include "push.h"
#include "pin.h"

#include "stringprop.h"
#include "boolprop.h"
#include "intprop.h"

#define tr(str) simulideTr("Push",str)

listItem_t Push::libraryItem(){
    return {
        tr("Push"),
        "Switches",
        "push.png",
        "Push",
        [](QString type, QString id){ return (Component*)new Push( type, id ); } };
}

Push::Push( QString type, QString id )
    : PushBase( type, id )
{
    m_area = QRectF(-11,-9, 22, 11 );
    m_proxy->setPos(-8, 4 );

    SetupSwitches( 1, 1 );

    addPropGroup( { tr("Main"), {
        new BoolProp<Push>("Norm_Close", tr("Normally Closed"), ""
                          , this, &Push::nClose, &Push::setNClose ),

        new IntProp <Push>("Poles", tr("Poles"), ""
                          , this, &Push::poles, &Push::setPoles, propNoCopy,"uint" ),

        new StrProp <Push>("Key", tr("Key"), ""
                          , this, &Push::key, &Push::setKey ),
    }, 0} );
}
Push::~Push(){}

void Push::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    if( m_hidden ) return;

    Component::paint( p, option, widget );
    
    QPen pen = p->pen();
    pen.setWidth(3);
    p->setPen(pen);

    for( int i=0; i<m_numPoles; i++ )                           // Draw Switches
    {
        int offset = 16*i;
        if( m_closed ) p->drawLine(-9,-2-offset, 9,-2-offset );
        else           p->drawLine(-9,-8-offset, 9,-8-offset );
    }
    if( m_numPoles > 1 )
    {
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
        p->setPen(pen);
        p->drawLine(-0, 4-4*m_pin0, 0,-3*m_pin0-16*m_numPoles+4 );
    }
    Component::paintSelected( p );
}
