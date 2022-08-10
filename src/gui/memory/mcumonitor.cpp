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

#include "mcumonitor.h"
#include "simulator.h"
#include "e_mcu.h"
#include "memtable.h"
#include "utils.h"

#define BYTES_PER_LINE 16

MCUMonitor::MCUMonitor( QWidget* parent, eMcu* mcu )
          : QDialog( parent )
{
    setupUi(this);
    hide();
    m_processor = mcu;

    m_jumpToAddress = false;

    m_ramTable = m_processor->getRamTable();
    m_status = &m_ramTable->m_status;
    m_pc     = &m_ramTable->m_pc;

    horizontalLayout->insertWidget( 0, m_status);
    horizontalLayout->insertWidget( 0, m_pc);
    horizontalLayout->setStretchFactor( m_pc, 30 );
    horizontalLayout->setStretchFactor( m_status, 65 );
    horizontalLayout->setStretchFactor( byteButton, 20 );

    m_ramMonitor   = new MemTable( tabWidget, m_processor->ramSize() );
    m_flashMonitor = new MemTable( tabWidget, m_processor->flashSize(), m_processor->wordSize() );
    m_romMonitor   = new MemTable( tabWidget, m_processor->romSize() );

    m_ramTable->getSplitter()->insertWidget( 2, m_ramMonitor );
    m_ramTable->getSplitter()->setSizes( {50,320,50} );

    tabWidget->addTab( m_ramTable, "RAM" );
    //tabWidget->addTab( m_ramMonitor, "RAM");
    tabWidget->addTab( m_romMonitor, "EEPROM");
    tabWidget->addTab( m_flashMonitor, "Flash");

    connect( tabWidget,      SIGNAL(currentChanged(int)),   this, SLOT(tabChanged(int)) );
    connect( m_ramMonitor,   SIGNAL(dataChanged(int, int)), this, SLOT(ramDataChanged(int, int)) );
    connect( m_flashMonitor, SIGNAL(dataChanged(int, int)), this, SLOT(flashDataChanged(int, int)) );
    connect( m_romMonitor,   SIGNAL(dataChanged(int, int)), this, SLOT(eepromDataChanged(int, int)) );
}

void MCUMonitor::ramDataChanged( int address, int val )
{ m_processor->setRamValue( address, val ); }

void MCUMonitor::flashDataChanged( int address, int val )
{ m_processor->setFlashValue( address, val ); }

void MCUMonitor::eepromDataChanged(  int address, int val )
{ m_processor->setRomValue( address, val ); }

void MCUMonitor::tabChanged( int )
{
    if( Simulator::self()->isRunning() ) return;
    if( tabWidget->count() < 3 ) return;
    updateStep();
}

void MCUMonitor::on_byteButton_toggled( bool byte )
{
    int bytes = byte ? 1 : m_processor->wordSize();
    m_flashMonitor->setCellBytes( bytes );
    updateStep();
}

void  MCUMonitor::on_jumpButton_toggled( bool jump )
{
    m_jumpToAddress = jump;
}

void MCUMonitor::updateStep()
{
    int status = m_processor->status();
    for( int i=0; i<8; i++ )
    {
        int bit = status & 1;
        if( bit ) m_status->item( 0, 7-i )->setBackground( QColor( 255, 150, 00 ) );
        else      m_status->item( 0, 7-i )->setBackground( QColor( 120, 230, 255 ) );
        status >>= 1;
    }

    int pc = m_processor->pc();
    if( byteButton->isChecked() ) pc *= m_processor->wordSize();
    m_pc->item( 0, 0 )->setData( 0, pc );
    m_pc->item( 0, 1 )->setText("0x"+val2hex(pc) );

    if( m_ramTable->isVisible() )
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
    else if( m_romMonitor->isVisible() )
    {
        m_romMonitor->setData( m_processor->eeprom() );
    }
    else if( m_flashMonitor->isVisible() )
    {
        for( uint32_t i=0; i<m_processor->flashSize(); ++i )
            m_flashMonitor->setValue( i, m_processor->getFlashValue(i));

        if( Simulator::self()->simState() == SIM_RUNNING )
            m_flashMonitor->setAddrSelected( pc, m_jumpToAddress );
    }
}

void MCUMonitor::updateRamTable()
{
    m_ramTable->updateItems();
}
