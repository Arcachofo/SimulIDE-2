/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <math.h>

#include "sram.h"
#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"
#include "iopin.h"
#include "memtable.h"
#include "utils.h"

#include "stringprop.h"
#include "boolprop.h"
#include "intprop.h"

#define tr(str) simulideTr("SRAM",str)

listItem_t SRAM::libraryItem(){
    return {
        tr("Static Ram/Rom"),
        "Memory",
        "2to3g.png",
        "SRAM",
        [](QString id){ return (Component*)new SRAM("SRAM", id ); } };
}

SRAM::SRAM( QString type, QString id )
    : IoComponent( type, id )
    , eElement( id )
    , Memory()
{
    m_width  = 4;
    m_height = 11;

    m_otherPin.resize( 3 );
    m_WePin = new IoPin( 180, QPoint( 0,0 ), m_id+"-Pin_We", 0, this, input );
    m_WePin->setLabelText("WE");
    m_WePin->setLabelColor( QColor( 0, 0, 0 ) );
    m_WePin->setInverted( true );
    m_otherPin[0] = m_WePin;
    
    m_CsPin = new IoPin(  0, QPoint( 0,0 ), m_id+"-Pin_Cs", 0, this, input );
    m_CsPin->setLabelText("CS");
    m_CsPin->setLabelColor( QColor( 0, 0, 0 ) );
    m_CsPin->setInverted( true );
    m_otherPin[1] = m_CsPin;
    
    m_OePin = new IoPin( 180, QPoint( 0,0 ), m_id+"-Pin_outEnable" , 0, this, input );
    m_OePin->setLabelText("OE");
    m_OePin->setLabelColor( QColor( 0, 0, 0 ) );
    m_OePin->setInverted( true );
    m_otherPin[2] = m_OePin;

    m_asynchro = true;
    m_addrBits = 0;
    m_wordBits = 0;
    setAddrBits( 8 );
    setDataBits( 8 );

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"), {
        new IntProp <SRAM>("Address_Bits", tr("Address Size"),"_bits"
                          , this, &SRAM::addrBits, &SRAM::setAddrBits, propNoCopy,"uint" ),

        new IntProp <SRAM>("Data_Bits", tr("Data Size"),"_bits"
                          , this, &SRAM::wordBits, &SRAM::setDataBits, propNoCopy,"uint" ),

        new BoolProp<SRAM>("Persistent", tr("Persistent"),""
                          , this, &SRAM::persistent, &SRAM::setPersistent ),

        new BoolProp<SRAM>("Asynch", tr("Asynchronous"),""
                          , this, &SRAM::asynchro, &SRAM::setAsynchro )
    }, groupNoCopy} );

    addPropGroup( { tr("Electric")
        , IoComponent::inputProps()
        + IoComponent::outputProps()
        + IoComponent::outputType()
    ,0 } );

    addPropGroup( { tr("Timing")
        , IoComponent::edgeProps()
    ,0 } );

    addPropGroup( { "Hidden", {
        new StrProp<SRAM>("Mem","",""
                         , this, &SRAM::getMem, &SRAM::setMem)
    }, groupHidden} );
}
SRAM::~SRAM(){}

void SRAM::stamp()                   // Called at Simulation Start
{
    m_oe = false;
    m_cs = true;
    m_we = true;

    for( IoPin* pin : m_outPin ) pin->setPinMode( input );
    IoComponent::initState();

    if( !m_persistent ) fillMemory( 0 );

    for( uint i=0; i<m_inPin.size(); ++i )
        m_inPin[i]->changeCallBack( this, m_asynchro );

    m_WePin->changeCallBack( this );
    m_CsPin->changeCallBack( this );
    m_OePin->changeCallBack( this );
}

void SRAM::updateStep()
{
    if( m_changed )
    {
        for( IoPin* pin : m_inPin  ) pin->changeCallBack( this, m_asynchro && m_cs );
        for( IoPin* pin : m_outPin ) pin->changeCallBack( this, m_asynchro && m_cs && m_we );
        m_changed = false;
    }
    if( m_memTable ) m_memTable->updateTable();
}

void SRAM::voltChanged()        // Some Pin Changed State, Manage it
{
    bool cs = m_CsPin->getInpState();
    bool we = m_WePin->getInpState();
    bool oe = cs && !we && m_OePin->getInpState(); // Enable output buffers only if OE & CS & Read
    if( m_oe != oe || m_cs != cs || m_we != we )
    {
        m_oe = oe;
        for( IoPin* pin : m_outPin )         // Enable/Disable output buffers
        {
            pin->setPinMode( oe ? output : input );
            if( m_asynchro ) pin->changeCallBack( this, cs && we );
        }
    }
    // Operate only if Asynchronous or change in CS or WE
    bool operate = m_asynchro || m_cs != cs || m_we != we;
    m_cs = cs;
    m_we = we;

    if( !operate || !m_cs ) return;          // Nothing to do

    m_address = 0;
    for( int i=0; i<m_addrBits; ++i )        // Get Address
    {
        bool state = m_inPin[i]->getInpState();
        if( state ) m_address += pow( 2, i );
    }
    if( m_we ){                              // Write
        int value = 0;
        for( uint i=0; i<m_outPin.size(); ++i )
        {
            bool state = m_outPin[i]->getInpState();
            if( state ) value += pow( 2, i );
        }
        m_data[m_address] = value;
    }
    else{                                    // Read
        m_nextOutVal = m_data[m_address];
        IoComponent::scheduleOutPuts( this );
}   }

void SRAM::setAsynchro( bool a )
{
    m_asynchro = a;
    m_changed = true;
}

void SRAM::updatePins()
{
    int h = m_addrBits+1;
    if( m_wordBits > h ) h = m_wordBits;
    
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
    m_WePin->setPos( QPoint(-24,origY+h*8 ) );   // WE
    m_WePin->isMoved();
    
    m_CsPin->setPos( QPoint( 24,origY+8+h*8 ) ); // CS
    m_CsPin->isMoved();
    
    m_OePin->setPos( QPoint(-24,origY+8+h*8 ) ); // OE
    m_OePin->isMoved();
    
    m_area = QRect(-(m_width/2)*8, origY, m_width*8, m_height*8 );
}

void SRAM::setAddrBits( int bits )
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

void SRAM::createAddrPins( int bits )
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

void SRAM::deleteAddrPins( int bits )
{ IoComponent::deletePins( &m_inPin, bits ); }

void SRAM::setDataBits( int bits )
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

void SRAM::createDataPins( int bits )
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

void SRAM::deleteDataPins( int bits )
{ IoComponent::deletePins( &m_outPin, bits ); }

void SRAM::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
{
    QAction* loadAction = menu->addAction( QIcon(":/load.svg"),tr("Load data") );
    QObject::connect( loadAction, &QAction::triggered, [=](){ loadData(); } );

    QAction* saveAction = menu->addAction(QIcon(":/save.png"), tr("Save data") );
    QObject::connect( saveAction, &QAction::triggered, [=](){ saveData(); } );

    QAction* showEepAction = menu->addAction(QIcon(":/save.png"), tr("Show SRAM Table") );
    QObject::connect( showEepAction, &QAction::triggered, [=](){ slotShowTable(); } );

    menu->addSeparator();
    Component::contextMenu( event, menu );
}

void SRAM::slotShowTable()
{
    Memory::showTable();
    if( m_persistent ) m_memTable->setWindowTitle( "ROM: "+idLabel() );
    else               m_memTable->setWindowTitle( "RAM: "+idLabel() );
}
