/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef VOLTIMETER_H
#define VOLTIMETER_H

#include "meter.h"

class Voltimeter : public Meter
{
    public:
        Voltimeter( QString type, QString id );
        ~Voltimeter();

 static listItem_t libraryItem();

        virtual void updateStep() override;
};

#endif
