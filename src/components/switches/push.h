/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PUSH_H
#define PUSH_H

#include "push_base.h"

class Push : public PushBase
{
    public:
        Push( QString type, QString id );
        ~Push();

 static listItem_t libraryItem();

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;
};

#endif
