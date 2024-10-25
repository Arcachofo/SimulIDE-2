/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef AVROCM_H
#define AVROCM_H

#include "mcuocm.h"

class AvrOcm : public McuOcm
{
    friend class McuCreator;

    public:
        AvrOcm( eMcu* mcu, QString name);
        ~AvrOcm();

        virtual void setup() override;

        virtual void configureA() override;

    protected:
        virtual void OutputOcm() override;

        regBits_t m_PD2;
};

#endif
