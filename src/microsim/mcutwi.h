/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUTWI_H
#define MCUTWI_H

#include "mcuprescaled.h"
#include "twimodule.h"

class eMcu;
class McuPin;

class McuTwi : public McuPrescaled, public TwiModule
{
        friend class McuCreator;

    public:
        McuTwi( eMcu* mcu, QString name );
        ~McuTwi();

        virtual void initialize() override;

        virtual void writeAddrReg(){ m_address = *m_addrReg; }
        virtual void writeStatus(){;}
        virtual void writeTwiReg(){;}
        virtual void readTwiReg(){;}

    protected:
        virtual void updateFreq() {;}

        uint8_t* m_addrReg;
        uint8_t* m_dataReg;
        uint8_t* m_statReg;
};

#endif
