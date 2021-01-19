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

#include <QWidget>

#include "ui_boolval.h"

#ifndef BOOLVAL_H
#define BOOLVAL_H

class Component;

class BoolVal : public QWidget, private Ui::BoolVal
{
    Q_OBJECT
    
    public:
        BoolVal( QWidget* parent=0 );
        
        void setup( Component* comp );
        void setPropName( QString name, QString caption );

    public slots:
        void on_trueVal_toggled( bool checked );

    private:
        Component* m_component;
        QString m_propName;
};

#endif
