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

#include "serialport.h"
#include "mcucomponent.h"
#include "baseprocessor.h"
#include "simulator.h"
#include "circuit.h"
#include "itemlibrary.h"
#include "utils.h"

static const char* SerialPort_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","Steps"),
    QT_TRANSLATE_NOOP("App::Property","Port Name"),
    QT_TRANSLATE_NOOP("App::Property","BaudRate"),
    QT_TRANSLATE_NOOP("App::Property","DataBits"),
    QT_TRANSLATE_NOOP("App::Property","Parity"),
    QT_TRANSLATE_NOOP("App::Property","StopBits"),
    QT_TRANSLATE_NOOP("App::Property","FlowControl")
};

Component* SerialPort::construct( QObject* parent, QString type, QString id )
{
    return new SerialPort( parent, type, id );
}

LibraryItem* SerialPort::libraryItem()
{
    return new LibraryItem(
        "SerialPort",
        "",
        "SerialPort.png",
        "SerialPort",
        SerialPort::construct );
}

SerialPort::SerialPort( QObject* parent, QString type, QString id )
      : Component( parent, type, id )
      , eElement( (id+"-eElement") )
{
    Q_UNUSED( SerialPort_properties );

    m_area = QRect( -34, -26, 160, 38 );
    setLabelPos(-34,-20 );

    m_serial = new QSerialPort( this );

    m_mcuId = "";
    m_autoOpen = false;
    m_active = false;
    m_uart = 0;
    m_BaudRate    = QSerialPort::Baud9600;
    m_dataBits    = QSerialPort::Data8;
    m_parity      = QSerialPort::NoParity;
    m_stopBits    = QSerialPort::OneStop;
    m_flowControl = QSerialPort::NoFlowControl;

    m_button = new QPushButton( );
    m_button->setMaximumSize( 32,16 );
    m_button->setGeometry(-36,-16,32,16);
    m_button->setCheckable( true );
    m_button->setText( "Open" );

    m_proxy = Circuit::self()->addWidget( m_button );
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint(-32, -8) );

    connect( m_button, SIGNAL( clicked() ),
                 this, SLOT(   onbuttonclicked() ), Qt::UniqueConnection);

    connect( m_serial, SIGNAL( readyRead()),
                 this, SLOT(   readData()), Qt::UniqueConnection );

    Simulator::self()->addToUpdateList( this );

    initialize();
}

SerialPort::~SerialPort()
{
    Simulator::self()->remFromUpdateList( this );
}

void SerialPort::setMcuId( QString mcu )
{
    QString name = Circuit::self()->origId( mcu );
    if( name == "" ) name = mcu;
    m_mcuId =  name;

    Component* mcuComp = Circuit::self()->getCompById( m_mcuId );
    if( mcuComp ) m_mcuComponent = static_cast<McuComponent*>(mcuComp);

    //qDebug() << "SerialPort::setMcuId" << mcu << name;
}

void SerialPort::initialize()
{
    m_mcuComponent = 0l;

    int circVersion = Circuit::self()->circType().split(".").last().toInt();

    if( circVersion < 5 )
    {
        m_mcuComponent = McuComponent::self();
        m_mcuId = m_mcuComponent->objectName();
    }

    {
        Component* mcu = Circuit::self()->getCompById( m_mcuId );
        if( mcu )
        {
            m_mcuComponent = static_cast<McuComponent*>(mcu);
            //qDebug() << "SerialPort::resetState" << mcu->objectName();
        }
    }
    if( m_mcuComponent )
    {
        m_processor = m_mcuComponent->processor();

        connect( m_mcuComponent, SIGNAL( closeSerials()),
                           this, SLOT(   slotClose()), Qt::UniqueConnection );

        connect( m_mcuComponent, SIGNAL( openSerials()),
                           this, SLOT(   slotAutoOpen()), Qt::UniqueConnection );

        connect( m_processor, SIGNAL( uartDataOut( int, int )),
                        this, SLOT(   slotWriteData( int, int )), Qt::UniqueConnection );
    }
}

void SerialPort::updateStep()
{
    update();
}

void SerialPort::slotAutoOpen()
{
    if( m_autoOpen ) open();
}

void SerialPort::open()
{
    if( m_serial->isOpen() ) close();

    m_serial->setPortName( m_portName );
    m_serial->setBaudRate( m_BaudRate );
    m_serial->setDataBits( m_dataBits );
    m_serial->setParity(   m_parity );
    m_serial->setStopBits( m_stopBits );
    m_serial->setFlowControl( m_flowControl );

    if( m_serial->open(QIODevice::ReadWrite) )
    {
        qDebug()<<"Connected to" << m_portName;
        m_button->setText( "Close" );
    }
    else
    {
        m_button->setChecked( false );
        MessageBoxNB( "Error", tr("Cannot Open Port %1:\n%2.").arg(m_portName).arg(m_serial->errorString()) );
    }
    m_active = false;
    update();
}

void SerialPort::close()
{
    if( m_serial->isOpen() ) m_serial->close();
    m_button->setText( "Open" );
    m_active = false;
    update();
    //ui->openButton->setEnabled( true );
    //ui->closeButton->setEnabled( false );
}

void SerialPort::readData()
{
    QByteArray data = m_serial->readAll();
    m_active = !m_active;
    update();
    //qDebug()<<"SerialPort::readData" << data;

    for( int i=0; i<data.size(); i++ ) m_processor->uartIn( m_uart, data.at(i) );
}

void SerialPort::slotWriteData( int uart, int value )
{
    if( uart != m_uart ) return;

    if( m_serial->isOpen() )
    {
        QByteArray ba;
        ba.resize(1);
        ba[0] = value;
        m_serial->write( ba );
        m_active = !m_active;
        update();
        //qDebug() << "SerialPort::slotWriteData"<<value;
    }
}

void SerialPort::slotClose()
{
    close();
    Circuit::self()->removeComp( this );
}

void SerialPort::setUart( int uart )
{
    if      ( uart<1 ) uart = 1;
    else if ( uart>6 ) uart = 6;
    m_uart = uart-1;
    update();
}

void SerialPort::onbuttonclicked()
{
    if( m_button->isChecked() ) open();
    else                        close();
}

void SerialPort::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );
    
    p->setBrush( Qt::darkBlue );
    p->drawRoundedRect( m_area, 4, 4 );

    if( m_serial->isOpen() )
    {
        if( m_active ) p->setBrush( Qt::yellow );
        else           p->setBrush( Qt::red );
        m_active = false;
    }
    else p->setBrush( Qt::black );
    p->drawEllipse( 2, -6, 12, 12);
    
    p->setBrush( Qt::white );
    QPen pen = p->pen();
    pen.setWidth( 0 );
    pen.setColor( QColor( 250, 210, 150 ) );
    p->setPen(pen);
    p->drawText( 18 , 5, "Uart"+QString::number(m_uart+1)+"-"+m_portName );

    QString mcuId = m_mcuId;
    if( m_mcuComponent ) mcuId = m_mcuComponent->idLabel();
    p->drawText(-30 ,-13, mcuId );
}

#include "moc_serialport.cpp"


