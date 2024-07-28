/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef RESISTOR_H
#define RESISTOR_H

#include "e-resistor.h"
#include "comp2pin.h"

class Resistor : public Comp2Pin, public eResistor
{
    public:
        Resistor( QString type, QString id );
        ~Resistor();

 static listItem_t libraryItem();

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;
};

#endif
