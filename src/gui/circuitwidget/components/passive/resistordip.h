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

#ifndef RESISTORDIP_H
#define RESISTORDIP_H

#include "component.h"
#include "e-resistordip.h"

class LibraryItem;

class MAINMODULE_EXPORT ResistorDip : public Component, public eResistorDip
{
    Q_OBJECT
    Q_PROPERTY( int    Size       READ size     WRITE setSize    DESIGNABLE true USER true )
    Q_PROPERTY( double Resistance READ resist   WRITE setResist  DESIGNABLE true USER true )

    public:
        ResistorDip( QObject* parent, QString type, QString id );
        ~ResistorDip();

        static Component* construct( QObject* parent, QString type, QString id );
        static LibraryItem *libraryItem();
        
        virtual QList<propGroup_t> propGroups() override;

        int  size();
        void setSize( int size );
        
        double resist();
        void setResist( double r );
        
        void setUnit( QString un );
        
        void createResistors( int c );
        void deleteResistors( int d );

        virtual void remove() override;
        
        virtual void paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget );

    private:
        std::vector<Pin*> m_pin;
};

#endif
