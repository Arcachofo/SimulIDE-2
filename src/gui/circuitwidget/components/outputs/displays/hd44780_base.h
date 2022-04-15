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

#ifndef HD44780BASE_H
#define HD44780BASE_H

#include "component.h"
#include "e-element.h"
#include "pin.h"

class MAINMODULE_EXPORT Hd44780_Base : public Component
{
    public:
        Hd44780_Base( QObject* parent, QString type, QString id );
        ~Hd44780_Base();

        int cols() { return m_cols; }
        void setCols( int cols );
        
        int rows() { return m_rows; }
        void setRows( int rows );

        void init();

        void showPins( bool show );
        
        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget );
        
    protected:
        void clearDDRAM();
        void clearLcd();
        void writeData( int data );
        void readData(){;}
        void readBusy(){;}
        void proccessCommand( int command );
        void functionSet( int data );
        void C_D_Shift( int data );
        void dispControl( int data );
        void entryMode( int data );
        void cursorHome();
        void setDDaddr( int addr );
        void setCGaddr( int addr );
        
        QImage m_fontImg;                  //Characters image

        int m_DDram[80];                   //80 DDRAM
        int m_CGram[64];                   //64 CGRAM
        
        int m_rows;
        int m_cols;
        int m_imgWidth;
        int m_imgHeight;
        
        int m_cursPos;
        int m_shiftPos;
        int m_direction;
        int m_shiftDisp;
        int m_dispOn;
        int m_cursorOn;
        int m_cursorBlink;
        int m_dataLength;
        //int m_dispLines;
        int m_lineLength;
        int m_DDaddr;
        int m_CGaddr;
        int m_nibble;
        int m_input;
        
        bool m_lastClock;
        bool m_writeDDRAM;
        bool m_blinking;

        uint64_t m_lastCircTime;

        //Inputs
        Pin* m_pinRS;
        Pin* m_pinRW;
        Pin* m_pinEn;
        std::vector<Pin*> m_dataPin;
};

#endif
