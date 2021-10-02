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

#ifndef JOYSTICKWIDGET_H
#define JOYSTICKWIDGET_H

#include <QWidget>

class JoystickWidget : public QWidget
{
    Q_OBJECT
    
    public:
        JoystickWidget();
        ~JoystickWidget();

        void setupWidget();
        
        int getXValue() { return m_xValue; }
        int getYValue() { return m_yValue; }
        
        void mousePressEvent( QMouseEvent* event );
        void mouseMoveEvent( QMouseEvent* event );
        void mouseReleaseEvent( QMouseEvent* event );
        void paintEvent( QPaintEvent* event );
        
    signals:
        void valueChanged(int newXValue, int newYValue);
        
    private:
        int m_xValue;
        int m_yValue;
        
        QPointF m_movingOffset;
        bool m_grabCenter;
        
        QRectF centerEllipse();
        QPointF center();
        void updateOutputValues();
};
#endif

