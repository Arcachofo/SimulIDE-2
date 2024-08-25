/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUMONITOR_H
#define MCUMONITOR_H

#include <QDialog>
#include <QTableWidget>

#include "ui_mcumonitor.h"

class eMcu;
class MemTable;
class RamTable;
class Watcher;

class MCUMonitor : public QDialog, private Ui::McuMonitor
{
    Q_OBJECT
    
    public:
        MCUMonitor( QWidget* parent=0, eMcu* mcu=0 );

        void updateStep();

        void addTable( MemTable* table, QString tittle );

    public slots:
        void tabChanged(int);

    private:
        //void createStatusPC();

        eMcu* m_eMcu;

        uint32_t* m_statusReg;  // STATUS register

        Watcher*  m_watcher;
        //RamTable* m_ramTable;

        QTableWidget m_status;
        QTableWidget m_pc;

        bool m_jumpToAddress;
};

#endif
