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

#include <QMenu>

#include "i2cram.h"
#include "memtable.h"
#include "simulator.h"
#include "itemlibrary.h"
#include "iopin.h"
#include "label.h"

#include "stringprop.h"
#include "doubleprop.h"
#include "boolprop.h"
#include "intprop.h"

Component* I2CRam::construct( QObject* parent, QString type, QString id )
{ return new I2CRam( parent, type, id ); }

LibraryItem* I2CRam::libraryItem()
{
    return new LibraryItem(
        tr( "I2C Ram" ),
        tr( "Logic/Memory" ),
        "2to3.png",
        "I2CRam",
        I2CRam::construct );
}

I2CRam::I2CRam( QObject* parent, QString type, QString id )
      : IoComponent( parent, type, id )
      , TwiModule( id )
      , MemData()
{
    m_width  = 4;
    m_height = 4;
    
    init({         // Inputs:
            "IL01SDA", //type: Input, side: Left, pos: 01, label: "SDA"
            "IL03SCL",
            "IR01A0",
            "IR02A1",
            "IR03A2"
        });

    m_inPin[0]->setPinMode( openCo );
    TwiModule::setSdaPin( m_inPin[0] );

    m_inPin[1]->setPinMode( openCo );
    TwiModule::setSclPin( m_inPin[1] );

    m_persistent = false;
    m_cCode = 0b01010000; // 0x50, 80
    m_size  = 65536;
    m_ram.resize( m_size );

    IoComponent::initState();

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"), {
new IntProp <I2CRam>( "Size_bytes"  , tr("Size")         ,"_Bytes", this, &I2CRam::rSize,      &I2CRam::setRSize, "uint" ),
new IntProp <I2CRam>( "Control_Code", tr("Control_Code") ,""      , this, &I2CRam::cCode,      &I2CRam::setCcode, "uint" ),
new DoubProp<I2CRam>( "Frequency"   , tr("I2C Frequency"),"_KHz"  , this, &I2CRam::freqKHz,    &I2CRam::setFreqKHz ),
new BoolProp<I2CRam>( "Persistent"  , tr("Persistent")   ,""      , this, &I2CRam::persistent, &I2CRam::setPersistent ),
    }} );
    addPropGroup( {"Hidden", {
new StringProp<I2CRam>( "Mem", "","", this, &I2CRam::getMem, &I2CRam::setMem )
    }} );
}
I2CRam::~I2CRam(){}

void I2CRam::initialize()
{
    TwiModule::initialize();

    m_addrPtr = 0;
    m_phase = 3;
}

void I2CRam::stamp()           // Called at Simulation Start
{
    TwiModule::setMode( TWI_SLAVE );

    for( int i=2; i<5; i++ )     // Initialize address pins
        m_inPin[i]->changeCallBack( this );
}

void I2CRam::updateStep()
{
    IoComponent::updateStep();
    if( m_memTable ) m_memTable->updateTable( &m_ram );
}

void I2CRam::voltChanged()             // Some Pin Changed State, Manage it
{
    m_address = m_cCode;

    if( m_inPin[2]->getInpState() ) m_address += 1;
    if( m_inPin[3]->getInpState() ) m_address += 2;
    if( m_inPin[4]->getInpState() ) m_address += 4;

    TwiModule::voltChanged();        // Run I2C Engine

    if( m_i2cState == I2C_STOP ) m_phase = 3;
}

void I2CRam::readByte() // Write to RAM
{
    if( m_phase == 0 )
    {
        m_phase++;
        m_addrPtr = m_rxReg<<8;
    }
    else if( m_phase == 1 )
    {
        m_phase++;
        if( m_size > 256 ) m_addrPtr += m_rxReg;
        else               m_addrPtr  = m_rxReg;
    }else{
        while( m_addrPtr >= m_size ) m_addrPtr -= m_size;
        m_ram[ m_addrPtr ] = m_rxReg;
        m_addrPtr++;
        
        if( m_addrPtr >= m_size ) m_addrPtr = 0;
    }
   TwiModule::readByte();
}

void I2CRam::startWrite()
{
    if( m_size > 256 ) m_phase = 0;
    else               m_phase = 1;
}

void I2CRam::writeByte() // Read from RAM
{
    while( m_addrPtr >= m_size ) m_addrPtr -= m_size;

    m_txReg = m_ram[ m_addrPtr ];
    if( ++m_addrPtr >= m_size ) m_addrPtr = 0;

    TwiModule::writeByte();
}

void I2CRam::setMem( QString m )
{
    if( m.isEmpty() ) return;
    MemData::setMem( &m_ram, m );
}

QString I2CRam::getMem()
{
    QString m;
    if( !m_persistent ) return m;

    return MemData::getMem( &m_ram );
}

void I2CRam::setRSize( int size )
{
    if( size > 65536 ) size = 65536;
    if( size < 1 ) size = 1;
    m_size = size;
    m_ram.resize( size );

    if( m_memTable ) m_memTable->setData( &m_ram );
}

void I2CRam::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
    if( !acceptedMouseButtons() ) event->ignore();
    else
    {
        event->accept();
        QMenu* menu = new QMenu();
        contextMenu( event, menu );
        Component::contextMenu( event, menu );
        menu->deleteLater();
}   }

void I2CRam::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
{
    QAction* loadAction = menu->addAction( QIcon(":/load.png"),tr("Load data") );
    connect( loadAction, SIGNAL(triggered()),
                   this, SLOT(loadData()), Qt::UniqueConnection );

    QAction* saveAction = menu->addAction(QIcon(":/save.png"), tr("Save data") );
    connect( saveAction, SIGNAL(triggered()),
                   this, SLOT(saveData()), Qt::UniqueConnection );

    QAction* showEepAction = menu->addAction(QIcon(":/save.png"), tr("Show Memory Table") );
    connect( showEepAction, SIGNAL(triggered()),
                      this, SLOT(showTable()), Qt::UniqueConnection );

    menu->addSeparator();
}

void I2CRam::showTable()
{
    MemData::showTable( m_size, 1 );
    if( m_persistent ) m_memTable->setWindowTitle( "I2C ROM: "+m_idLabel->toPlainText());
    else               m_memTable->setWindowTitle( "I2C RAM: "+m_idLabel->toPlainText() );
    m_memTable->setData( &m_ram );
}

#include "moc_i2cram.cpp"
