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

#ifndef MCUCOMPARATOR_H
#define MCUCOMPARATOR_H

#include "mcumodule.h"
#include "e-element.h"

class McuPin;

class MAINMODULE_EXPORT McuComp : public McuModule, public eElement
{
        friend class McuCreator;

    public:
        McuComp( eMcu* mcu, QString name );
        ~McuComp();

        virtual void initialize() override;

        virtual void callBackDoub( double vref ) override;

    protected:
        virtual void setMode( uint8_t mode ) { m_mode = mode; }

        bool m_fixVref;
        bool m_enabled;
        bool m_compOut;

        double m_vref;

        uint8_t m_mode;

        std::vector<McuPin*> m_pins;
        McuPin* m_pinP;
        McuPin* m_pinN;
        McuPin* m_pinOut;
};

#endif
