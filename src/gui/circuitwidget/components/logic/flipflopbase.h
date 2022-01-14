/***************************************************************************
 *   Copyright (C) 2016 by santiago González                               *
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

#ifndef FLIPFLOPBASE_H
#define FLIPFLOPBASE_H

#include "logiccomponent.h"

class MAINMODULE_EXPORT FlipFlopBase : public LogicComponent
{
        Q_OBJECT
    public:
        FlipFlopBase( QObject* parent, QString type, QString id );
        ~FlipFlopBase();

        bool srInv() { return m_srInv; }
        void setSrInv( bool inv );

        bool pinsRS() { return m_useRS; }
        void usePinsRS( bool rs );

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override{ IoComponent::runOutputs(); }

        bool sPinState();
        bool rPinState();

    protected:
        virtual void calcOutput(){;}
        bool m_useRS;
        bool m_srInv;
        bool m_Q0;

        int m_dataPins;

        IoPin* m_setPin;
        IoPin* m_resetPin;
};

#endif
