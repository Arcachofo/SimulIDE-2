/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PICPORT_H
#define PICPORT_H

#include "mcuport.h"

class Mcu;
class eMcu;
class McuPin;
class Component;

class PicPort : public McuPort
{
    public:
        PicPort( eMcu* mcu, QString name );
        ~PicPort();

        virtual void setup() override{;}

        virtual void configureA() override;

        virtual void outChanged() override;
        virtual uint32_t getInpState() override;

        virtual void puRegChanged() override;
        virtual void puBitsChanged() override;
        virtual void puBitsInvChanged() override;

    protected:
        virtual McuPin* createPin( int i, QString id , Component* mcu ) override;

        uint8_t* m_ANSEL;
};

#endif
