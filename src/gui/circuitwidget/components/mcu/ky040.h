/***************************************************************************
 *   Copyright (C) 2020 by Jan K. S.                                       *
 *                                                      *
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
/*
 *   Modified 2020 by santiago González                                    *
 *   santigoro@gmail.com                                                   *
 *                                                                         */

#ifndef KY040_H
#define KY040_H

#include "e-source.h"
#include "e-element.h"
#include "component.h"
#include "itemlibrary.h"
#include "dialwidget.h"

class MAINMODULE_EXPORT KY040 : public Component, public eElement
{
    Q_OBJECT
    Q_PROPERTY( int Detents READ detents WRITE setDetents DESIGNABLE true USER true )
    
    public:

        KY040( QObject* parent, QString type, QString id );
        ~KY040();

        static Component* construct( QObject* parent, QString type, QString id );
        static LibraryItem* libraryItem();

        virtual void initialize() override;
        virtual void updateStep() override;
        virtual void runEvent() override;
        
        void setDetents(int val);
        int detents();

        virtual void paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget );
        
    public slots:
        virtual void remove() override;
        void posChanged(int pos);
        void onbuttonpressed();
        void onbuttonreleased();
        
    private:
        int m_detents;
        int m_seqIndex;
        int m_prevDialVal;
        bool m_clockwise;
        
        bool m_changed;
        bool m_closed;
        
        DialWidget m_dialW;

        QDial* m_dial;
        QGraphicsProxyWidget* m_proxy;
        
        QToolButton* m_button;
        QGraphicsProxyWidget* m_proxy_button;
        
        Pin* m_dtpin;
        Pin* m_clkpin;
        Pin* m_swpin;
        
        eSource* m_dt;
        eSource* m_clk;
        eSource* m_sw;
};

#endif

