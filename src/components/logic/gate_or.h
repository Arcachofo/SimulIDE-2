/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef ORGATE_H
#define ORGATE_H

#include "gate.h"
#include "component.h"

class OrGate : public Gate
{
    public:
        OrGate( QString type, QString id );
        ~OrGate();

 static listItem_t libraryItem();

        virtual QPainterPath shape() const override;
        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;
        
    protected:
        virtual bool calcOutput( int inputs ) override;
};

#endif
