/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PicPIN_H
#define PicPIN_H

#include "mcupin.h"

class Component;
class McuPort;
class eMcu;

class PicPin : public McuPin
{
    friend class McuPort;

    public:
        PicPin( McuPort* port, int i, QString id , Component* mcu );
        ~PicPin();

        virtual void ConfExtInt() override;

        virtual void setAnalog( bool an ) override;

    protected:

};

#endif
