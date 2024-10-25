/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef AVRSPI_H
#define AVRSPI_H

#include "mcuspi.h"

class eMcu;

class AvrSpi : public McuSpi
{
    public:
        AvrSpi( eMcu* mcu, QString name );
        ~AvrSpi();

        virtual void setup() override;
        virtual void initialize() override;

        virtual void setMode( spiMode_t mode ) override;
        virtual void configureA() override;
        virtual void writeStatus() override;
        virtual void writeSpiReg() override;
        virtual void endTransaction() override;

    protected:
        void updateSpeed();

        bool m_speed2x;

        uint8_t m_status;

        regBits_t m_SPR;
        regBits_t m_SPE;
        regBits_t m_DODR;
        regBits_t m_MSTR;
        regBits_t m_CPOL;
        regBits_t m_CPHA;
        regBits_t m_SPI2X;
};

#endif
