/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>
#include <QDebug>

#include "dram.h"
#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"
#include "iopin.h"
#include "monitor.h"
#include "memtable.h"
#include "utils.h"

#include "stringprop.h"
#include "doubleprop.h"
#include "intprop.h"

#define tr(str) simulideTr("DRAM",str)

listItem_t DRAM::libraryItem(){
    return {
        tr("Dynamic Ram"),
        "Memory",
        "2to3g.png",
        "DRAM",
        [](QString type, QString id){ return (Component*)new DRAM( type, id ); } };
}

DRAM::DRAM( QString type, QString id )
    : LogicComponent( type, id )
    , Memory()
    , Watchable()
{
    m_width  = 4;
    m_height = 11;

    m_monitor = nullptr;

    m_otherPin.resize( 4 );
    m_WePin = new IoPin( 180, QPoint( 0,0 ), m_id+"-Pin_We", 0, this, input );
    m_WePin->setLabelText("WE");
    m_WePin->setLabelColor( QColor( 0, 0, 0 ) );
    m_WePin->setInverted( true );
    m_otherPin[0] = m_WePin;
    
    m_RasPin = new IoPin(  0, QPoint( 0,0 ), m_id+"-Pin_Ras", 0, this, input );
    m_RasPin->setLabelText("RAS");
    m_RasPin->setLabelColor( QColor( 0, 0, 0 ) );
    m_RasPin->setInverted( true );
    m_otherPin[1] = m_RasPin;

    m_CasPin = new IoPin(  0, QPoint( 0,0 ), m_id+"-Pin_Cas", 0, this, input );
    m_CasPin->setLabelText("CAS");
    m_CasPin->setLabelColor( QColor( 0, 0, 0 ) );
    m_CasPin->setInverted( true );
    m_otherPin[2] = m_CasPin;

    m_OePin = new IoPin( 180, QPoint( 0,0 ), m_id+"-Pin_outEnable" , 0, this, input );
    m_OePin->setLabelText( "OE" );
    m_OePin->setLabelColor( QColor( 0, 0, 0 ) );
    m_OePin->setInverted( true );
    m_otherPin[3] = m_OePin;

    m_rowAddrBits = 0;
    m_colAddrBits = 0;
    m_addrBits = 0;
    m_wordBits = 0;
    m_refreshPeriod = 0;

    setRowAddrBits( 8 );
    setColAddrBits( 8 );
    setDataBits( 8 );

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"), {
        new IntProp <DRAM>("Row_Bits", tr("Row Address Size"),"_bits", this
                , &DRAM::rowAddrBits, &DRAM::setRowAddrBits, propNoCopy,"uint" ),

        new IntProp <DRAM>("Column_Bits", tr("Column Address Size"),"_bits", this
                , &DRAM::colAddrBits, &DRAM::setColAddrBits, propNoCopy,"uint" ),

        new IntProp <DRAM>("Data_Bits", tr("Data Size"),"_bits", this
                , &DRAM::wordBits, &DRAM::setDataBits, propNoCopy,"uint" ),

        new DoubProp<DRAM>("Refresh", tr("Refresh period"),"ps", this
                , &DRAM::refreshPeriod, &DRAM::setRefreshPeriod )
    }, groupNoCopy } );

    addPropGroup( { tr("Electric")
       ,IoComponent::inputProps()
       +IoComponent::outputProps()
       +IoComponent::outputType()
    ,0 } );

    addPropGroup( { tr("Timing")
        ,IoComponent::edgeProps()
    ,0 } );
}
DRAM::~DRAM(){}

void DRAM::stamp()                   // Called at Simulation Start
{
    m_rowAddress = 0;
    m_intRefresh = 0;
    m_refreshError = false;

    m_oe = false;
    m_ras = false;
    m_cas = false;

    for( uint i=0; i<m_data.size(); ++i )
        m_data[i] = rand() % (int)( pow( 2, m_wordBits ) );

    m_WePin->changeCallBack( this );
    m_RasPin->changeCallBack( this );
    m_CasPin->changeCallBack( this );
    m_OePin->changeCallBack( this );

    for( IoPin* pin : m_outPin ) pin->setPinMode( input );

    LogicComponent::stamp();
}

void DRAM::updateStep()
{
    if( m_memTable ) m_memTable->updateTable();
    //if ( m_refreshError )
    //    m_error = true;
}

void DRAM::voltChanged()        // Some Pin Changed State, Manage it
{
    bool RAS = m_RasPin->getInpState();
    bool CAS = m_CasPin->getInpState();
    bool WE  = m_WePin->getInpState();
    bool OE  = m_OePin->getInpState() && !WE & CAS; // Enable output buffers only if OE & CAS & RAS & Read

    if( m_oe != OE ){
        m_oe = OE;
        for( IoPin* pin : m_outPin ) pin->setPinMode( OE ? output : input ); // Enable/Disable output buffers
    }

    if( RAS != m_ras ) {
        m_ras = RAS;
        if ( RAS )                                               // falling edge at /RAS
        {
            int refreshRow;
            if ( !m_cas ) {                                      // RAS before CAS - row refresh from address bus
                m_rowAddress = 0;
                for( int i=0; i<m_rowAddrBits; ++i )             // Get Row Address
                {
                    bool state = m_inPin[i]->getInpState();
                    if( state ) m_rowAddress += pow( 2, i );
                }
                refreshRow = m_rowAddress; //qDebug() << "Row address" << m_rowAddress;
            } else {                                             // RAS after CAS - internal refresh
                refreshRow = m_intRefresh++;
                if( m_intRefresh >= pow( 2, m_rowAddrBits ) ) m_intRefresh = 0; //qDebug() << "Internal refresh row" << m_rowAddress;
            }
            if ( Simulator::self()->circTime() > ( m_rowLastRefresh[refreshRow] + m_refreshPeriod ) ) {
                m_refreshError = true; //qDebug() << "Row" << refreshRow << "of dynamic RAM " + idLabel() + " has not been refreshed for" << ( Simulator::self()->circTime() - m_rowLastRefresh[refreshRow] ) / 1e9 << "ms";
            }
            m_rowLastRefresh[refreshRow] = Simulator::self()->circTime();
        }
    }

    if( CAS != m_cas ) {
        m_cas = CAS;
        if ( CAS && RAS )     // falling edge at /CAS
        {
            m_address = m_rowAddress * pow( 2, m_rowAddrBits );
            for( int i=0; i<m_colAddrBits; ++i )                // Get Address
            {
                bool state = m_inPin[i]->getInpState();
                if( state ) m_address += pow( 2, i );
            }
            //qDebug() << "Address" << m_address;

            if ( WE ) {                                        // Write
                int value = 0;
                for( uint i=0; i<m_outPin.size(); ++i )
                {
                    bool state = m_outPin[i]->getInpState();
                    if( state ) value += pow( 2, i );
                }
                setValue( m_address, value ); //m_data[m_address] = value;
                //qDebug() << "Write" << value << "to address" << m_address;
            }else{                                            // Read
                m_nextOutVal = getValue( m_address ); //m_data[m_address];
                IoComponent::scheduleOutPuts( this );
            }
        }
    }
}

void DRAM::runEvent()
{
    IoComponent::runOutputs();
}

void DRAM::updatePins()
{
    int h = m_addrBits+1;
    if( m_wordBits > m_addrBits ) h = m_wordBits + 1;
    
    m_height = h+2;
    int origY = -(m_height/2)*8;
    
    for( int i=0; i<m_addrBits; i++ )
    {
        m_inPin[i]->setPos( QPoint(-24,origY+8+i*8 ) );
        m_inPin[i]->isMoved();
    }
    for( int i=0; i<m_wordBits; i++ )
    {
        m_outPin[i]->setPos( QPoint(24,origY+8+i*8 ) ); 
        m_outPin[i]->isMoved();
    }
    m_WePin->setPos( QPoint(-24,origY+h*8 ) );    // WE
    m_WePin->isMoved();
    
    m_RasPin->setPos( QPoint( 24,origY+h*8 ) );   // RAS
    m_RasPin->isMoved();

    m_CasPin->setPos( QPoint( 24,origY+8+h*8 ) ); // CAS
    m_CasPin->isMoved();
    
    m_OePin->setPos( QPoint(-24,origY+8+h*8 ) );  // OE
    m_OePin->isMoved();
    
    m_area   = QRect( -(m_width/2)*8, origY, m_width*8, m_height*8 );
}

void DRAM::setRowAddrBits( int rowAddrBits )
{
    if( rowAddrBits == m_rowAddrBits ) return;    
    if( rowAddrBits == 0 ) rowAddrBits = 8;
    if( ( rowAddrBits + m_colAddrBits ) > 24 ) rowAddrBits = 24 - m_colAddrBits;
    
    int bits = (rowAddrBits > m_colAddrBits) ? rowAddrBits : m_colAddrBits;
    setAddrBits( bits );

    m_rowAddrBits = rowAddrBits;
    m_rowLastRefresh.resize( pow( 2, m_rowAddrBits ) );
}

void DRAM::setColAddrBits( int colAddrBits )
{
    if( colAddrBits == m_colAddrBits ) return;
    if( colAddrBits == 0 ) colAddrBits = 8;
    if( ( m_rowAddrBits + colAddrBits ) > 24 ) colAddrBits = 24 - m_rowAddrBits;

    int bits = (m_rowAddrBits > colAddrBits) ? m_rowAddrBits : colAddrBits;
    setAddrBits( bits );

    m_colAddrBits = colAddrBits;
}

void DRAM::setAddrBits( int bits )
{
    if( bits == m_addrBits ) return;
    if( bits == 0 ) bits = 8;
    if( bits > 24 ) bits = 24;

    resize( pow( 2, bits ) );
    
    if( Simulator::self()->isRunning() ) CircuitWidget::self()->powerCircOff();
    
    if     ( bits < m_addrBits ) deleteAddrPins( m_addrBits-bits );
    else if( bits > m_addrBits ) createAddrPins( bits-m_addrBits );
    m_addrBits = bits;

    updatePins();
    Circuit::self()->update();
}

void DRAM::createAddrPins( int bits )
{
    int chans = m_addrBits + bits;
    int origY = -(m_height/2)*8;
    
    m_inPin.resize( chans );
    
    for( int i=m_addrBits; i<chans; i++ )
    {
        QString number = QString::number(i);

        m_inPin[i] = new IoPin( 180, QPoint(-24,origY+8+i*8 ), m_id+"-in"+number, i, this, input );
        m_inPin[i]->setLabelText( "A"+number );
        m_inPin[i]->setLabelColor( QColor( 0, 0, 0 ) );
        initPin( m_inPin[i] );
}   }

void DRAM::deleteAddrPins( int bits )
{ LogicComponent::deletePins( &m_inPin, bits ); }

void DRAM::setDataBits( int bits )
{
    if( Simulator::self()->isRunning() ) CircuitWidget::self()->powerCircOff();

    if( bits == m_wordBits ) return;
    if( bits == 0 ) bits = 8;
    if( bits > 32 ) bits = 32;

    if     ( bits < m_wordBits ) deleteDataPins( m_wordBits-bits );
    else if( bits > m_wordBits ) createDataPins( bits-m_wordBits );
    
    setWordBits( bits );
    updatePins();
    Circuit::self()->update();
}

void DRAM::createDataPins( int bits )
{
    int chans = m_wordBits + bits;
    int origY = -(m_height/2)*8;
    
    m_outPin.resize( chans );
    
    for( int i=m_wordBits; i<chans; i++ )
    {
        QString number = QString::number(i);
        
        m_outPin[i] = new IoPin( 0, QPoint(24,origY+8+i*8 ), m_id+"-out"+number, i, this, output );
        m_outPin[i]->setLabelText( "D"+number);
        m_outPin[i]->setLabelColor( QColor( 0, 0, 0 ) );
        initPin( m_outPin[i] );
}   }

void DRAM::deleteDataPins( int bits )
{ LogicComponent::deletePins( &m_outPin, bits ); }

void DRAM::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
{
    QAction* openMonitor = menu->addAction( QIcon(":/terminal.svg"),tr("Open Memory Monitor.") );
    QObject::connect( openMonitor, &QAction::triggered, [=](){ slotOpenMonitor(); } );

    QAction* loadAction = menu->addAction( QIcon(":/load.svg"),tr("Load data") );
    QObject::connect( loadAction, &QAction::triggered, [=](){ loadData(); } );

    QAction* saveAction = menu->addAction(QIcon(":/save.png"), tr("Save data") );
    QObject::connect( saveAction, &QAction::triggered, [=](){ saveData(); } );

    //QAction* showEepAction = menu->addAction(QIcon(":/save.png"), tr("Show Memory Table") );
    //QObject::connect( showEepAction, &QAction::triggered, [=](){ slotShowTable(); } );

    menu->addSeparator();
    Component::contextMenu( event, menu );
}

/*void DRAM::slotShowTable()
{
    showTable();
    m_memTable->setWindowTitle( "DRAM: "+idLabel() );
}*/

void DRAM::slotOpenMonitor()
{
    if( !m_monitor )
    {
        showTable();
        m_monitor = new Monitor();
        m_monitor->addTable( m_memTable, "RAM" );
    }
    m_monitor->setWindowTitle( idLabel() );
    m_monitor->show();
}
