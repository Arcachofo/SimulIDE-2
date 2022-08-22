/***************************************************************************
 *   Copyright (C) 2021 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#ifndef PICOCUNIT_H
#define PICOCUNIT_H

#include "mcuocunit.h"

class PicPwmUnit;

class MAINMODULE_EXPORT PicOcUnit : public McuOcUnit
{
        friend class PicCcpUnit;

    public:
        PicOcUnit( eMcu* mcu, QString name );
        ~PicOcUnit();

static PicPwmUnit* createPwmUnit( eMcu* mcu, QString name, int type );

        //virtual void initialize() override;
        virtual void runEvent();

        virtual void configure( uint8_t CCPxM ) override;

    protected:
        bool m_enhanced;
        bool m_specEvent;
        bool m_resetTimer;

        regBits_t m_GODO;
};

class MAINMODULE_EXPORT PicPwmUnit : public McuOcUnit
{
        friend class PicCcpUnit;

    public:
        PicPwmUnit( eMcu* mcu, QString name );
        ~PicPwmUnit();

        //virtual void initialize() override;
        virtual void runEvent();

        virtual void configure( uint8_t newCCPxCON ) override;

        virtual void sheduleEvents( uint32_t ovf, uint32_t countVal, int rot=0 ) override;

        virtual void ocrWriteL( uint8_t val ) override;

    protected:
        bool m_enhanced;

        uint8_t m_cLow;
        uint8_t m_CCPRxL;

        regBits_t m_PxM;
        regBits_t m_DCxB;
};

class MAINMODULE_EXPORT PicPwmUnit00 : public PicPwmUnit
{
    public:
        PicPwmUnit00( eMcu* mcu, QString name );
        ~PicPwmUnit00();
};

class MAINMODULE_EXPORT PicPwmUnit01 : public PicPwmUnit
{
    public:
        PicPwmUnit01( eMcu* mcu, QString name );
        ~PicPwmUnit01();
};

#endif
