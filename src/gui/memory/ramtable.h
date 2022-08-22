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

#include "ui_ramtable.h"

#ifndef RAMTABLE_H
#define RAMTABLE_H

class eMcu;
class BaseDebugger;
class QStandardItemModel;
class QSplitter;

class RamTable : public QWidget, private Ui::RamTable
{
    Q_OBJECT

    public:
        RamTable( QWidget* parent=0, eMcu* processor=NULL, bool cpuMonitor=false );

        void updateValues();

        void setItemValue( int col, QString v ) { table->item( m_currentRow, col )->setData( 0, v ); }
        void setItemValue( int col, int32_t v ) { table->item( m_currentRow, col )->setData( 0, v ); }
        void setItemValue( int col, float v ) { table->item( m_currentRow, col )->setData( 0, v ); }

        void setRegisters( QStringList regs );
        void addRegister( QString name, QString type );

        void setDebugger( BaseDebugger* deb ) { m_debugger = deb; }
        void remDebugger( BaseDebugger* deb ) { if( m_debugger == deb ) m_debugger = NULL; }

        void setVariables( QStringList vars );
        void addVariable( QString name, int address, QString type );
        void addVariable( QString name, QString type );

        void loadVarSet( QStringList varSet );
        QStringList getVarSet();
        uint16_t getCurrentAddr();

        void updateItems();

        QSplitter* getSplitter() { return splitter; }

    public slots:
        void RegDoubleClick( const QModelIndex& index );
        void VarDoubleClick( const QModelIndex& index );
        void clearSelected();
        void clearTable();
        void loadVarSet();
        void saveVarSet();

    private slots:
        void addToWatch( QTableWidgetItem* );
        void slotContextMenu( const QPoint& );

    private:
        void updateRamValue( QString name );

        eMcu* m_processor;
        BaseDebugger*  m_debugger;

        QStandardItemModel* m_registerModel;
        QStandardItemModel* m_variableModel;

        QHash<int, QString> watchList;

        QHash<QString, QString> m_typeTable;
        QHash<QString, uint16_t> m_varsTable;
        QStringList m_varNames;
        QStringList m_regNames;

        bool m_loadingVars;
        bool m_cpuMonitor;

        int m_numRegs;
        int m_currentRow;
};
#endif // RAMTABLE_H
