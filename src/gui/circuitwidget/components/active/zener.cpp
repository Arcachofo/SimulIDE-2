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

#include "zener.h"
#include "itemlibrary.h"

Component* Zener::construct( QObject* parent, QString type, QString id )
{ return new Zener( parent, type, id ); }

LibraryItem* Zener::libraryItem()
{
    return new LibraryItem(
            tr( "Zener Diode" ),
            tr( "Rectifiers" ),
            "zener.png",
            "Zener",
            Zener::construct);
}

Zener::Zener( QObject* parent, QString type, QString id )
     : Diode( parent, type, id, true )
{
    m_enumUids = m_enumNames = m_zeners.keys();
}
Zener::~Zener(){}
