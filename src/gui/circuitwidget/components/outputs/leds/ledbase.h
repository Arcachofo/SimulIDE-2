/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
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

#ifndef LEDBASE_H
#define LEDBASE_H

#include "e-led.h"
#include "component.h"
#include "e-node.h"

class eNode;

class MAINMODULE_EXPORT LedBase : public Component, public eLed
{
    public:
        LedBase( QObject* parent, QString type, QString id );
        ~LedBase();
        
        enum LedColor {
            yellow = 0,
            red,
            green,
            blue,
            orange,
            purple,
            white
        };

        QString colorStr() { return m_enumUids.at((int)m_ledColor ); }
        void setColorStr( QString color );

        bool grounded() { return m_grounded; }
        void setGrounded( bool grounded );

        virtual void initialize() override;
        virtual void updateStep() override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget );

    protected:
        virtual void drawBackground( QPainter* p )=0;
        virtual void drawForeground( QPainter* p )=0;
        
        bool   m_grounded;
        
        LedColor m_ledColor;

        static eNode m_gndEnode;
};

#endif
