/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef AVRTWI_H
#define AVRTWI_H

#include "mcutwi.h"
//#include "mcutypes.h"

class eMcu;
//class McuPin;

class AvrTwi : public McuTwi
{
    public:
        AvrTwi( eMcu* mcu, QString name );
        ~AvrTwi();

        virtual void setup() override;
        virtual void initialize() override;

        virtual void configureA() override;
        virtual void configureB() override;

        virtual void writeAddrReg() override;
        virtual void writeStatus() override;
        virtual void writeTwiReg() override;

        virtual void writeByte() override;

        virtual void I2Cstop() override;

    protected:
        virtual void setTwiState( twiState_t state ) override;
        uint8_t getStaus() { return *m_statReg &= 0b11111000; }
        virtual void updateFreq() override;

        uint8_t m_bitRate;

        bool m_start;
        bool m_stop;

        //uint8_t* m_TWCR;
        uint8_t* m_TWBR;

        regBits_t m_TWEN;
        regBits_t m_TWWC;
        regBits_t m_TWSTO;
        regBits_t m_TWSTA;
        regBits_t m_TWEA;
        regBits_t m_TWINT;

};

#endif
