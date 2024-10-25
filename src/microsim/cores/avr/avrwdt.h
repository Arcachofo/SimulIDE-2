/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef AVRWDT_H
#define AVRWDT_H

#include "mcuwdt.h"

class eMcu;

class AvrWdt : public McuWdt
{
    public:
        AvrWdt( eMcu* mcu, QString name );
        ~AvrWdt();

 static AvrWdt* createWdt( eMcu* mcu, QString name );

        virtual void setup() override;
        virtual void initialize() override;
        virtual void runEvent() override;

        virtual void configureA() override;

        virtual void reset() override;

        virtual void callBack() override;

    protected:
        void wdtEnable();
        virtual void updtPrescaler(){;}

        bool m_allowChanges;
        bool m_disabled;

        //uint8_t*  m_WDTCSR;

        regBits_t m_WDCE;
        regBits_t m_WDE;
        regBits_t m_WDP02;

        regBits_t m_WDRF;
};

class AvrWdt00 : public AvrWdt
{
    public:
        AvrWdt00( eMcu* mcu, QString name );
        ~AvrWdt00();

        virtual void configureA() override;

    private:
        virtual void updtPrescaler() override;

        regBits_t m_WDIF;
        regBits_t m_WDIE;
        regBits_t m_WDP3;
};

class AvrWdt01 : public AvrWdt
{
    public:
        AvrWdt01( eMcu* mcu, QString name );
        ~AvrWdt01();

        virtual void configureA() override;

    private:
        virtual void updtPrescaler() override;
};
#endif
