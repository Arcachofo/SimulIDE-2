/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef COMPARATOR_H
#define COMPARATOR_H

#include "iocomponent.h"
#include "e-element.h"


class Comparator : public IoComponent, public eElement
{
    public:
        Comparator( QString type, QString id );
        ~Comparator();

 static listItem_t libraryItem();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override { IoComponent::runOutputs(); }

        virtual QPainterPath shape() const override;
        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;
};

#endif
