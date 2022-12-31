﻿/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "ledmatrix.h"
#include "itemlibrary.h"
#include "circuitwidget.h"
#include "connector.h"
#include "simulator.h"
#include "circuit.h"
#include "pin.h"

#include "doubleprop.h"
#include "stringprop.h"
#include "boolprop.h"
#include "intprop.h"

Component* LedMatrix::construct( QObject* parent, QString type, QString id )
{ return new LedMatrix( parent, type, id ); }

LibraryItem* LedMatrix::libraryItem()
{
    return new LibraryItem(
        tr( "LedMatrix" ),
        tr( "Leds" ),
        "ledmatrix.png",
        "LedMatrix",
        LedMatrix::construct);
}

LedMatrix::LedMatrix( QObject* parent, QString type, QString id )
         : Component( parent, type, id )
         , eElement( id )
{
    m_graphical = true;

    m_rows = 8;
    m_cols = 8;
    m_resist  = 0.6;
    m_maxCurr = 0.02;
    m_threshold = 2.4;
    
    m_ledColor = 0;
    m_color = QColor(0,0,0);
    m_verticalPins = false;
    createMatrix();

    addPropGroup( { tr("Main"), {
new IntProp   <LedMatrix>( "Rows"         , tr("Rows")     ,"_Leds", this, &LedMatrix::rows,     &LedMatrix::setRows, "uint" ),
new IntProp   <LedMatrix>( "Cols"         , tr("Columns")  ,"_Leds", this, &LedMatrix::cols,     &LedMatrix::setCols, "uint" ),
new StringProp<LedMatrix>( "Color"        , tr("Color")        ,"" , this, &LedMatrix::colorStr, &LedMatrix::setColorStr, "enum" ),
new BoolProp  <LedMatrix>( "Vertical_Pins", tr("Vertical Pins"),"" , this, &LedMatrix::verticalPins, &LedMatrix::setVerticalPins ),
    }} );
    addPropGroup( { tr("Electric"), {
new DoubProp<LedMatrix>( "Threshold" , tr("Forward Voltage"),"V", this, &LedMatrix::threshold,  &LedMatrix::setThreshold ),
new DoubProp<LedMatrix>( "MaxCurrent", tr("Max Current")    ,"A", this, &LedMatrix::maxCurrent, &LedMatrix::setMaxCurrent ),
new DoubProp<LedMatrix>( "Resistance", tr("Resistance")     ,"Ω", this, &LedMatrix::res,        &LedMatrix::setRes ),
    }} );
}
LedMatrix::~LedMatrix(){}

void LedMatrix::stamp()
{
    for( int row=0; row<m_rows; ++row )
    {
        eNode* rowEnode = m_rowPin[row]->getEnode();
        for( int col=0; col<m_cols; ++col )
        {
            eNode* colEnode = m_colPin[col]->getEnode();
            LedSmd* lsmd = m_led[row][col];
            lsmd->getEpin(0)->setEnode( rowEnode );
            lsmd->getEpin(1)->setEnode( colEnode );
}   }   }

void LedMatrix::setupMatrix( int rows, int cols )
{
    if( Simulator::self()->isRunning() )  CircuitWidget::self()->powerCircOff();

    deleteMatrix();
    m_rows = rows;
    m_cols = cols;
    createMatrix();

    Circuit::self()->update();
}

void LedMatrix::createMatrix()
{
    if( m_verticalPins ) m_area = QRect( -4, -8, m_cols*8, m_rows*8+8 );
    else                 m_area = QRect( -8, -8, m_cols*8+8, m_rows*8+8 );
    
    m_led.resize( m_rows, std::vector<LedSmd*>(m_cols) );
    m_rowPin.resize( m_rows );
    m_colPin.resize( m_cols );
    m_pin.resize( m_rows+m_cols );
    
    for( int row=0; row<m_rows; ++row )
    {
        QString pinId = m_id;
        pinId.append( QString("-pinRow"+QString::number(row)));
        QPoint nodpos;
        int angle;
        if( m_verticalPins ) { nodpos = QPoint( row*8, -16 ); angle = 90; }
        else                 { nodpos = QPoint(-16, row*8 ); angle = 180; }

        m_rowPin[row] = new Pin( angle, nodpos, pinId, 0, this);
        m_pin[row] = m_rowPin[row];
        
        for( int col=0; col<m_cols; ++col )
        {
            QString ledid = m_id;
            ledid.append( QString( "-led"+QString::number(row)+"_"+QString::number(col) ) );
            LedSmd* lsmd = new LedSmd( this, "LEDSMD", ledid, QRectF(-2, -2, 4, 4) );

            lsmd->setParentItem(this);
            lsmd->setPos( col*8, row*8 );
            lsmd->setRes( m_resist );
            lsmd->setMaxCurrent( m_maxCurr );
            lsmd->setThreshold( m_threshold );
            lsmd->setColorStr( colorStr() );
            lsmd->setFlag( QGraphicsItem::ItemIsSelectable, false );
            lsmd->setAcceptedMouseButtons(0);
            m_led[row][col] = lsmd;
            Circuit::self()->compList()->removeOne( lsmd );
    }   }
    for( int col=0; col<m_cols; ++col )
    {
        QString pinId = m_id;
        pinId.append( QString("-pinCol"+QString::number(col)));
        QPoint nodpos = QPoint( col*8, m_rows*8+8 );
        m_colPin[col] = new Pin( 270, nodpos, pinId, 1, this);
        m_pin[m_rows+col] = m_colPin[col];
}   }

void LedMatrix::deleteMatrix()
{
    for( int row=0; row<m_rows; ++row )
    {
        for( int col=0; col<m_cols; ++col )
        {
            LedSmd* lsmd = m_led[row][col];
            Circuit::self()->removeComp( lsmd );
        }
        m_rowPin[row]->removeConnector();
        delete m_rowPin[row];
    }
    for( int col=0; col<m_cols; col++ )
    {
        m_colPin[col]->removeConnector();
        delete m_colPin[col];
    }
    m_led.clear();
    m_rowPin.clear();
    m_colPin.clear();
    m_pin.clear();
}

QString LedMatrix::colorStr()
{
    if( m_led[0][0] ) return m_led[0][0]->colorStr();
    else              return "Yellow";
}

void LedMatrix::setColorStr( QString color )
{
    for( int row=0; row<m_rows; ++row )
        for( int col=0; col<m_cols; ++col )
            m_led[row][col]->setColorStr( color );

    if( m_showVal && (m_showProperty == "Color") )
        setValLabelText( m_enumNames.at( m_enumUids.indexOf( color ) ) );
}

QStringList LedMatrix::getEnumUids( QString ){ return m_led[0][0]->getEnumUids(""); }
QStringList LedMatrix::getEnumNames( QString ){ return m_led[0][0]->getEnumNames(""); }

void LedMatrix::setRows( int rows )
{
    if( rows == m_rows ) return;
    if( rows < 1 ) rows = 1;
    setupMatrix( rows, m_cols );
}

void LedMatrix::setCols( int cols )
{
    if( cols == m_cols ) return;
    if( cols < 1 ) cols = 1;
    setupMatrix( m_rows, cols );
}

void LedMatrix::setVerticalPins( bool v )
{
    if( v == m_verticalPins ) return;
    m_verticalPins = v;
    
    if( v ){
        for( int i=0; i<m_rows; ++i )
        {
            m_rowPin[i]->setPos( i*8, -16 );
            m_rowPin[i]->setRotation( 90 );
    }   }
    else{
        for( int i=0; i<m_rows; ++i )
        {
            m_rowPin[i]->setPos( -16, i*8 );
            m_rowPin[i]->setRotation( 0 );
    }   }
    for( int i=0; i<m_rows; ++i ) m_rowPin[i]->isMoved();
    
    if( m_verticalPins ) m_area = QRect( -4, -8, m_cols*8, m_rows*8+8 );
    else                 m_area = QRect( -8, -8, m_cols*8+8, m_rows*8+8 );
    
    update();
}

void LedMatrix::setThreshold( double threshold ) 
{ 
    if( threshold < 1e-6 ) threshold = 1e-6;
    m_threshold = threshold;
    
    for( int row=0; row<m_rows; ++row )
    {
        for( int col=0; col<m_cols; ++col )
            m_led[row][col]->setThreshold( threshold ); 
}   }

void LedMatrix::setMaxCurrent( double current ) 
{
    if( current < 1e-6 ) current = 1e-6;
    m_maxCurr = current;
    
    for( int row=0; row<m_rows; ++row )
    {
        for( int col=0; col<m_cols; ++col )
            m_led[row][col]->setMaxCurrent( current );
}   }

void LedMatrix::setRes( double resist )
{
    if( resist == 0 ) resist = 1e-14;
    m_resist = resist;
    
    for( int row=0; row<m_rows; ++row )
    {
        for( int col=0; col<m_cols; ++col )
            m_led[row][col]->setRes( resist ); 
}   }

void LedMatrix::setHidden( bool hid, bool hidArea, bool hidLabel )
{
    Component::setHidden( hid, hidArea, hidLabel );
    if( hid ) m_area = QRect(-4,-4, m_cols*8, m_rows*8 );
    else{
        if( m_verticalPins ) m_area = QRect(-4,-8, m_cols*8,   m_rows*8+8 );
        else                 m_area = QRect(-8,-8, m_cols*8+8, m_rows*8+8 );
    }
}

void LedMatrix::remove()
{
    deleteMatrix();
    Component::remove();
}

void LedMatrix::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );
    p->drawRoundRect( m_area, 4, 4 );
}
