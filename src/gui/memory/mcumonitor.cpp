﻿/***************************************************************************
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

#include "mcumonitor.h"
#include "simulator.h"
#include "e_mcu.h"
#include "cpubase.h"
#include "memtable.h"
#include "mainwindow.h"
#include "utils.h"

MCUMonitor::MCUMonitor( QWidget* parent, eMcu* mcu )
          : QDialog( parent )
          , m_status( 1, 8 )
          , m_pc( 1, 2 )
{
    setupUi(this);
    hide();
    m_processor = mcu;

    m_jumpToAddress = false;

    m_statusReg    = NULL;
    m_ramTable     = NULL;
    m_ramMonitor   = NULL;
    m_flashMonitor = NULL;
    m_romMonitor   = NULL;

    createStatusPC();

    horizontalLayout->setStretchFactor( byteButton, 20 );

    if( mcu->ramSize() )
    {
        m_ramMonitor   = new MemTable( tabWidget, m_processor->ramSize() );
        m_ramTable = m_processor->getRamTable();
        m_ramTable->getSplitter()->insertWidget( 2, m_ramMonitor );
        m_ramTable->getSplitter()->setSizes( {50,320,50} );
        connect( m_ramMonitor,   SIGNAL(dataChanged(int, int)), this, SLOT(ramDataChanged(int, int)) );
        tabWidget->addTab( m_ramTable, "RAM" );
    }
    if( mcu->flashSize() )
    {
        m_flashMonitor = new MemTable( tabWidget, m_processor->flashSize(), m_processor->wordSize() );
        tabWidget->addTab( m_flashMonitor, "Flash");
        connect( m_flashMonitor, SIGNAL(dataChanged(int, int)), this, SLOT(flashDataChanged(int, int)) );
    }
    if( mcu->romSize() )
    {
        m_romMonitor   = new MemTable( tabWidget, m_processor->romSize() );
        tabWidget->addTab( m_romMonitor, "EEPROM");
        connect( m_romMonitor,   SIGNAL(dataChanged(int, int)), this, SLOT(eepromDataChanged(int, int)) );
    }
    m_cpuTable = m_processor->getCpuTable();
    if( m_cpuTable )
    {
        m_cpuTable->getSplitter()->setSizes( {50,320} );
        tabWidget->addTab( m_cpuTable, "CPU" );
    }
    connect( tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)) );
}

void MCUMonitor::ramDataChanged( int address, int val )
{ m_processor->setRamValue( address, val ); }

void MCUMonitor::flashDataChanged( int address, int val )
{ m_processor->setFlashValue( address, val ); }

void MCUMonitor::eepromDataChanged( int address, int val )
{ m_processor->setRomValue( address, val ); }

void MCUMonitor::tabChanged( int )
{
    if( Simulator::self()->isRunning() ) return;
    /// if( tabWidget->count() < 3 ) return;
    updateStep();
}

void MCUMonitor::on_byteButton_toggled( bool byte )
{
    if( m_flashMonitor )
    {
        int bytes = byte ? 1 : m_processor->wordSize();
        m_flashMonitor->setCellBytes( bytes );
    }
    updateStep();
}

void  MCUMonitor::on_jumpButton_toggled( bool jump )
{
    m_jumpToAddress = jump;
}

void MCUMonitor::updateStep()
{
    int pc = m_processor->cpu->getPC();

    if( m_statusReg )
    {
        int status = *m_statusReg; //m_processor->cpu->getStatus();
        for( int i=0; i<8; i++ )
        {
            int bit = status & 1;
            if( bit ) m_status.item( 0, 7-i )->setBackground( QColor( 255, 150, 00 ) );
            else      m_status.item( 0, 7-i )->setBackground( QColor( 120, 230, 255 ) );
            status >>= 1;
        }

        if( byteButton->isChecked() ) pc *= m_processor->wordSize();
        m_pc.item( 0, 0 )->setData( 0, pc );
        m_pc.item( 0, 1 )->setText("0x"+val2hex(pc) );
    }

    if( m_cpuTable && m_cpuTable->isVisible() )
    {
        m_cpuTable->updateValues();
    }
    else if( m_ramTable && m_ramTable->isVisible() )
    {
        if( m_ramTable->getSplitter()->sizes().at(1) > 150 ) // RAM Watcher visible
            m_ramTable->updateValues();
        if( m_ramTable->getSplitter()->sizes().at(2) > 100 ) // RAM MemTable visible
        {
            for( uint32_t i=0; i<m_processor->ramSize(); ++i )
                m_ramMonitor->setValue( i, m_processor->getRamValue(i));

            if(  Simulator::self()->simState() == SIM_RUNNING )
                m_ramMonitor->setAddrSelected( m_ramTable->getCurrentAddr(), m_jumpToAddress );
    }   }
    else if( m_romMonitor && m_romMonitor->isVisible() )
    {
        m_romMonitor->setData( m_processor->eeprom() );
    }
    else if( m_flashMonitor && m_flashMonitor->isVisible() )
    {
        for( uint32_t i=0; i<m_processor->flashSize(); ++i )
            m_flashMonitor->setValue( i, m_processor->getFlashValue(i));

        if( Simulator::self()->simState() == SIM_RUNNING )
            m_flashMonitor->setAddrSelected( pc, m_jumpToAddress );
    }
}

void MCUMonitor::updateRamTable()
{
    if( m_ramTable ) m_ramTable->updateItems();
}

void MCUMonitor::createStatusPC()
{
    m_statusReg = m_processor->cpu->getStatus();
    if( !m_statusReg ) return;

    float scale = MainWindow::self()->fontScale();
    int row_heigh = round( 22*scale );
    int font_size = round(14*scale);
    int numberColor = 0x202090;

    QTableWidgetItem* it;
    QFont font;
    font.setFamily("Ubuntu Mono");
    font.setBold( true );
    font.setPixelSize( font_size );
    m_status.setFont( font );
    m_pc.setFont( font );

    m_status.setVerticalHeaderLabels( QStringList()<<" STATUS " );
    m_status.verticalHeader()->setFixedWidth( round(60*scale) );
    m_status.horizontalHeader()->setMaximumSectionSize(row_heigh);
    m_status.horizontalHeader()->hide();
    m_status.setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_status.setRowHeight( 0, row_heigh );
    m_status.setFixedHeight( row_heigh );
    font.setPixelSize( round(12*scale) );
    for( int i=0; i<8; i++ )
    {
        it = new QTableWidgetItem(0);
        it->setFlags( Qt::ItemIsEnabled );
        it->setTextAlignment( Qt::AlignCenter );
        it->setFont( font );
        m_status.setItem( 0, i, it );
        m_status.setColumnWidth( i, row_heigh );
    }
    int wi = row_heigh*8 + round(60*scale);
    m_status.setMinimumWidth( wi );
    m_status.setMaximumWidth( wi );
    QStringList statusBits = m_processor->getStatusBits();
    if( statusBits.size() )
        for( int i=7; i>=0; --i ) m_status.item( 0, i )->setText( statusBits.takeFirst() );

    horizontalLayout->insertWidget( 0, &m_status);
    horizontalLayout->setStretchFactor( &m_status, 65 );

    m_pc.setVerticalHeaderLabels( QStringList()<<" PC ");
    m_pc.verticalHeader()->setFixedWidth( round(31*scale) );
    m_pc.horizontalHeader()->hide();
    m_pc.setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_pc.setRowHeight( 0, row_heigh );
    m_pc.setFixedHeight( row_heigh  );

    font.setPixelSize( font_size );
    it = new QTableWidgetItem(0);
    it->setFlags( Qt::ItemIsEnabled );
    it->setFont( font );
    it->setTextColor( QColor( numberColor ) );
    m_pc.setItem( 0, 0, it );
    m_pc.setColumnWidth( 0, round(45*scale) );

    it = new QTableWidgetItem(0);
    it->setFlags( Qt::ItemIsEnabled );
    it->setFont( font );
    it->setTextColor( QColor( 0x3030B8 ) );
    m_pc.setItem( 0, 1, it );
    m_pc.setColumnWidth( 1, round(60*scale) );
    wi = round(135*scale);
    m_pc.setMinimumWidth( wi );
    m_pc.setMaximumWidth( wi );

    horizontalLayout->insertWidget( 0, &m_pc);
    horizontalLayout->setStretchFactor( &m_pc, 30 );
}
