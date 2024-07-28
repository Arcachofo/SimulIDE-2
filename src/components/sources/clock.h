/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef CLOCK_H
#define CLOCK_H

#include "clock-base.h"

class Clock : public ClockBase
{
    public:
        Clock( QString type, QString id );
        ~Clock();

 static listItem_t libraryItem();

        virtual void runEvent() override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;
};

#endif
