/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef VARRESISTOR_H
#define VARRESISTOR_H

#include "varresbase.h"

class VarResistor : public VarResBase
{
    public:
        VarResistor( QString type, QString id );
        ~VarResistor();

 static listItem_t libraryItem();

        virtual void updateStep() override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;
};

#endif
