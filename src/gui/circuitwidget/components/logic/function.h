/***************************************************************************
 *   Copyright (C) 2018 by santiago González                               *
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

#ifndef FUNCTION_H
#define FUNCTION_H

#include <QScriptEngine>

#include "logiccomponent.h"

class LibraryItem;
class QPushButton;
class QGraphicsProxyWidget;

class MAINMODULE_EXPORT Function : public LogicComponent
{
    Q_OBJECT
    public:
        Function( QObject* parent, QString type, QString id );
        ~Function();
        
        static Component* construct( QObject* parent, QString type, QString id );
        static LibraryItem* libraryItem();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override { IoComponent::runOutputs(); }

        QString functions() { return m_functions; }
        void setFunctions( QString f );

        virtual void remove() override;
        
        void setNumInps( int inputs );
        void setNumOuts( int outs );
        
    public slots:
        void onbuttonclicked();
        void loadData();
        void saveData();
        
    protected:
        virtual void contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu );
        virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent* event);

    private:
        void updateArea( uint ins, uint outs );

        QScriptEngine m_engine;
        QList<QScriptProgram> m_program;

        QString m_functions;
        QStringList m_funcList;

        QList<QPushButton*> m_buttons;
        QList<QGraphicsProxyWidget*> m_proxys;

        QString m_lastDir;
};

#endif
