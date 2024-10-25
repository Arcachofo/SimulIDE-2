/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef I51PORT_H
#define I51PORT_H

#include "mcuport.h"


class I51Port : public McuPort
{
    public:
        I51Port( eMcu* mcu, QString name );
        ~I51Port();

        virtual void setup() override{;}

        virtual void reset() override;

    protected:
        virtual McuPin* createPin( int i, QString id , Component* mcu ) override;
};

#endif
