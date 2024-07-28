/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef XORGATE_H
#define XORGATE_H

#include "gate.h"
#include "component.h"

class XorGate : public Gate
{
    public:
        XorGate( QString type, QString id );
        ~XorGate();

 static listItem_t libraryItem();

        virtual QPainterPath shape() const override;
        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    protected:
        virtual bool calcOutput( int inputs ) override;
};
#endif
