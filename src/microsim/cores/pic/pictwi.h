/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PICTWI_H
#define PICTWI_H

#include "mcutwi.h"
//#include "mcutypes.h"

class eMcu;
//class McuPin;

class PicTwi : public McuTwi
{
    public:
        PicTwi( eMcu* mcu, QString name );
        ~PicTwi();

        virtual void setup() override;
        virtual void initialize() override;

        virtual void configureA() override;
        virtual void configureB() override;

        virtual void writeAddrReg() override;
        virtual void writeStatus() override;
        virtual void writeTwiReg() override;
        virtual void readTwiReg() override;

        virtual void setMode( twiMode_t mode ) override;
        virtual void writeByte() override;
        virtual void bufferEmpty() override;
        virtual void readByte() override;

        virtual void sleep( int mode ) override;

    protected:
        virtual void setTwiState( twiState_t state ) override;
        uint8_t getStaus() { return *m_statReg &= 0b11111000; }

        uint8_t m_bitRate;

        // SSPCON
        regBits_t m_WCOL;

        // SSPCON2
        regBits_t m_CGEN;
        regBits_t m_ACKSTAT;
        regBits_t m_ACKDT;
        regBits_t m_ACKEN;
        regBits_t m_RCEN;
        regBits_t m_PEN;
        regBits_t m_RSEN;
        regBits_t m_SEN;

        //SSPSTAT
        regBits_t m_P;
        regBits_t m_S;
        regBits_t m_RW;
        regBits_t m_BF;
};

#endif
