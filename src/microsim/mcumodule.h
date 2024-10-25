/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUMODULE_H
#define MCUMODULE_H

#include <QString>

#include "mcutypes.h"

class eMcu;
class McuRam;
class Interrupt;
class McuRegister;

class McuModule
{
        friend class McuCreator;

    public:
        McuModule( eMcu* mcu, QString name );
        virtual ~McuModule();

        virtual void setup()=0;

        virtual void reset(){;}

        virtual void configureA(){;}
        virtual void configureB(){;}
        virtual void configureC(){;}
        virtual void callBackDoub( double ){;}
        virtual void callBack() {;}
        virtual void sleep( int mode );

        QString name() { return m_name; }

        void setSleepMode( uint8_t m ) { m_sleepMode = m; }

        virtual void setInterrupt( Interrupt* i ) { m_interrupt = i; }
        Interrupt* getInterrupt() { return m_interrupt; }

        eMcu* getMcu() { return m_mcu; }

    protected:
        QString m_name;
        eMcu*   m_mcu;
        McuRam* m_mcuRam;

        uint8_t m_sleepMode;
        bool m_sleeping;

        Interrupt* m_interrupt;

        McuRegister* m_cfgRegA;
        McuRegister* m_cfgRegB;
        McuRegister* m_cfgRegC;

        regBits_t m_configBitsA;
        regBits_t m_configBitsB;
        regBits_t m_configBitsC;
};
#endif
