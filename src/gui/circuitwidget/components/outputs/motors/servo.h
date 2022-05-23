/***************************************************************************
 *   Copyright (C) 2017 by santiago González                               *
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

#ifndef SERVO_H
#define SERVO_H

#include "logiccomponent.h"

class LibraryItem;

class MAINMODULE_EXPORT Servo : public LogicComponent
{
        Q_OBJECT
    public:
        Servo( QObject* parent, QString type, QString id );
        ~Servo();
        
        static Component* construct( QObject* parent, QString type, QString id );
        static LibraryItem* libraryItem();

        double speed() { return m_speed; }
        void setSpeed( double speed ) { m_speed = speed; }

        double minPulse() { return m_minPulse; }
        void setMinPulse( double w );

        double maxPulse() { return m_maxPulse; }
        void setMaxPulse( double w );

        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;

        virtual QPainterPath shape() const;
        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget );

    private:
        double m_pos;            // Actual Angular position 0-180
        double m_targetPos;      // Target Angular position 0-180
        double m_speed;               // Angular speed sec/60º
        double m_minPulse;        // Minimum pulse width,   0º
        double m_maxPulse;        // Maximum pulse width, 180º
        double m_minAngle;      // Angle to move evrey repaint

        uint64_t m_pulseStart;              // Simulation step
        uint64_t m_lastUpdate;              // Simulation step
};

#endif
