/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MOSFET_H
#define MOSFET_H

#include "e-mosfet.h"
#include "component.h"

class LibraryItem;

class Mosfet : public Component, public eMosfet
{
    public:
        Mosfet( QString type, QString id );
        ~Mosfet();

 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        virtual void updateStep() override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget );
};

#endif
