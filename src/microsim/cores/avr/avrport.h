/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef AVRPORT_H
#define AVRPORT_H

#include "mcuport.h"


class AvrPort : public McuPort
{
    public:
        AvrPort( eMcu* mcu, QString name );
        ~AvrPort();

        void pinRegChanged();

    protected:
        virtual McuPin* createPin( int i, QString id , Component* mcu ) override;
};

#endif
