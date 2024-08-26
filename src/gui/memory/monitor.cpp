/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "monitor.h"
#include "simulator.h"
#include "e_mcu.h"
#include "cpubase.h"
#include "memtable.h"
#include "mainwindow.h"
#include "utils.h"
#include "mcuram.h"
#include "watcher.h"

Monitor::Monitor( QWidget* parent, eMcu* mcu )
          : QDialog( parent )
{
    setupUi(this);
    hide();
    m_eMcu = mcu;

    McuRam* mcuRam = (McuRam*)m_eMcu->getModule("ram");
    m_watcher = mcuRam->getWatcher();
    if( m_watcher ) tabWidget->addTab( m_watcher, tr("Watch") );

    connect( tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)) );
}

void Monitor::addTable( MemTable* table, QString tittle )
{
    tabWidget->addTab( table, tittle );
}


void Monitor::tabChanged( int )
{
    if( Simulator::self()->isRunning() ) return;
    updateStep();
}

void Monitor::updateStep()
{
    if( m_watcher && m_watcher->isVisible() )
    {
        m_watcher->updateValues();
    }
}
