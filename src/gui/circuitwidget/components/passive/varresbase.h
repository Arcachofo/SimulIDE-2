/***************************************************************************
 *   Copyright (C) 2020 by santiago González                               *
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

#ifndef VARRESBASE_H
#define VARRESBASE_H

#include "e-resistor.h"
#include "component.h"
#include "dialwidget.h"

class MAINMODULE_EXPORT VarResBase : public Component, public eResistor
{
    Q_OBJECT
    Q_PROPERTY( int Dial_Step READ getStep WRITE setStep DESIGNABLE true USER true )

    public:
        VarResBase( QObject* parent, QString type, QString id );
        ~VarResBase();

        int  maxVal()  { return m_dial->maximum(); }
        void setMaxVal( int max ) { m_dial->setMaximum( max ); }

        int  minVal() { return m_dial->minimum(); }
        void setMinVal( int min ) { m_dial->setMinimum( min ); }

        int getVal();

        int  getStep() { return m_step; }
        void setStep( int step ) { m_step = step ; }

        virtual void initialize() override;

    public slots:
        void setVal( int val );

    protected:
        int m_step;

        DialWidget m_dialW;
        QDial*     m_dial;
        QGraphicsProxyWidget* m_proxy;
};

#endif
