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

#ifndef WS2812_H
#define WS2812_H

#include "e-clocked_device.h"
#include "component.h"

class LibraryItem;

class MAINMODULE_EXPORT WS2812 : public Component, public eClockedDevice
{
        Q_OBJECT
    public:
        WS2812( QObject* parent, QString type, QString id );
        ~WS2812();

        static Component* construct( QObject* parent, QString type, QString id );
        static LibraryItem *libraryItem();

        int  rows() { return m_rows; }
        void setRows( int rows );

        int  cols() { return m_cols; }
        void setCols( int cols );

        virtual void initialize() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;

        virtual void paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget );

    private:
        void setOut( bool state );
        void saveBit( bool bit );
        void updateLeds();

        int m_state;
        int m_word;
        int m_byte;
        int m_bit;
        int m_data;

        int m_rows;
        int m_cols;
        int m_leds;

        double m_stepsPerus;

        bool m_lastHstate;
        bool m_newWord;

        uint64_t m_lastTime;

        std::vector<int> m_rgb;
        std::vector<QColor> m_led;

        IoPin* m_output;
};

#endif

