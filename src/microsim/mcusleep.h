/***************************************************************************
 *   Copyright (C) 2022 by santiago González                               *
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

#ifndef MCUSLEEP_H
#define MCUSLEEP_H

#include "mcumodule.h"
#include "e-element.h"

class MAINMODULE_EXPORT McuSleep : public McuModule, public eElement
{
        friend class McuCreator;

    public:
        McuSleep( eMcu* mcu, QString name );
        ~McuSleep();

        uint8_t mode() { return m_sleepMode; }

        bool enabled() { return m_enabled; }
        //virtual void sleep(){;}
        virtual void callBack() override;

    protected:
        bool m_enabled;

        uint8_t m_sleepMode;
};
#endif
