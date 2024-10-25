/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PICEEPROM_H
#define PICEEPROM_H

#include "mcurom.h"

class PicEeprom : public McuRom
{
        friend class McuCreator;

    public:
        PicEeprom( eMcu* mcu, QString name );
        ~PicEeprom();

        virtual void setup() override;
        virtual void initialize() override;
        virtual void runEvent() override;

        virtual void configureA() override;
        virtual void configureB() override;

    private:
        uint64_t m_nextCycle;
        bool m_writeEnable;

        uint8_t m_wrMask;

        uint8_t* m_EECON2;

        regBits_t m_WRERR;
        regBits_t m_WREN;
        regBits_t m_WR;
        regBits_t m_RD;
};

#endif
