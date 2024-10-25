/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef AVREEPROM_H
#define AVREEPROM_H

#include "mcurom.h"

class AvrEeprom : public McuRom
{
        friend class McuCreator;

    public:
        AvrEeprom( eMcu* mcu, QString name );
        ~AvrEeprom();

        virtual void setup() override;

        virtual void initialize() override;
        virtual void runEvent() override;

        virtual void configureA() override;

        virtual void writeEeprom() override;

    private:
        uint64_t m_nextCycle;

        uint8_t m_mode;

        bool m_oldEempe;
        bool m_oldEepe;

        uint8_t* m_EECR;

        regBits_t m_EEPM;
        regBits_t m_EEMPE;
        regBits_t m_EEPE;
        regBits_t m_EERE;
};

#endif
