/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "gate_or.h"

#include "intprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("OrGate",str)

listItem_t OrGate::libraryItem(){
    return {
        tr("Or Gate"),
        "Gates",
        "orgate.png",
        "OrGate",
        [](QString type, QString id){ return (Component*)new OrGate( type, id ); } };
}

OrGate::OrGate( QString type, QString id )
      : Gate( type, id, 2 )
{
    addPropGroup( { tr("Electric"),
        IoComponent::inputProps()
        +QList<ComProperty*>({
        new BoolProp<OrGate>("Invert_Inputs", tr("Invert Inputs"),""
                            , this, &OrGate::invertInps, &OrGate::setInvertInps, propNoCopy ),

        new IntProp <OrGate>("Num_Inputs", tr("Input Size"),"_Inputs"
                            , this, &OrGate::numInps, &OrGate::setNumInputs, propNoCopy,"uint" )
                    })
        + Gate::outputProps()
      //+ IoComponent::outputType()
        +QList<ComProperty*>({
        new BoolProp<OrGate>("Open_Collector", tr("Open Drain"), ""
                                 , this, &IoComponent::openCol, &IoComponent::setOpenCol, propNoCopy )
                    })
                    ,0 } );

    addPropGroup( { tr("Timing"), IoComponent::edgeProps(),0 } );
}
OrGate::~OrGate(){}

bool OrGate::calcOutput( int inputs ) { return (inputs > 0); }

QPainterPath OrGate::shape() const
{
    QPainterPath path;
    QVector<QPointF> points;
    
    int size = (int)m_inPin.size()*4;
    
    points << QPointF(-14,-size-1 )
           << QPointF(-10,-size+4 )
           << QPointF( -9, 0 )
           << QPointF(-10, size-4  )
           << QPointF(-14, size+1 )
           << QPointF(  0, size )
           << QPointF( 10, 8  )
           << QPointF( 10,-8  )
           << QPointF(  0,-size );
        
    path.addPolygon( QPolygonF(points) );
    path.closeSubpath();
    return path;
}

void OrGate::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    int y_orig = m_area.y();
    int height = m_area.height();

    // Paint white background of gate
    Component::paint( p, o, w );
    QPen pen = p->pen();
    p->setPen( Qt::NoPen );

    QPainterPath path;
    path.moveTo( -8, 0 );
    path.arcTo( -33, y_orig, 42, height, -90, 180 );
    path.moveTo( -8, 0 );
    path.arcTo( -16, y_orig, 8, height, -90, 180 );
    p->drawPath( path );

    // Draw curves
    p->setPen( pen );
    p->setBrush( Qt::NoBrush );
    // Output side arc
    p->drawArc( -28, y_orig, 37, height, -1520/*-16*95*/, 3040/*16*190*/ );
    // Input side arc
    p->drawArc( -16, y_orig, 8, height, -1440/*-16*90*/, 2880/*16*180*/ );

    /*QPen pen2( Qt::red, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen2 );
    p->drawPath( shape() );

    QPen pen3( Qt::blue, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen3 );
    p->drawRect( m_area );*/

    Component::paintSelected( p );
}
