/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "capacitorbase.h"

class Capacitor : public CapacitorBase
{
    public:
        Capacitor( QString type, QString id );
        ~Capacitor();

 static listItem_t libraryItem();

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;
};

#endif
