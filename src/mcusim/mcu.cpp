/***************************************************************************
 *   Copyright (C) 2020 by santiago González                               *
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

#include <QDomDocument>
#include <QFileInfo>

#include "mcu.h"
#include "mcuport.h"
#include "mcupin.h"
#include "mcucreator.h"

#include "circuit.h"
#include "simulator.h"
#include "itemlibrary.h"
#include "circuitwidget.h"
#include "mainwindow.h"
#include "componentselector.h"
#include "simuapi_apppath.h"
#include "utils.h"


LibraryItem* Mcu::libraryItem()
{
    return new LibraryItem(
        "NEW_MCU",
        tr(""),
        "ic2.png",
        "MCU",
    Mcu::construct );
}

Component* Mcu::construct( QObject* parent, QString type, QString id )
{
    Mcu* mcu = new Mcu( parent, type,  id );
    if( !m_error) m_error = McuCreator::createMcu( mcu, id );
    if( !m_error) mcu->setLogicSymbol( false );

    if( m_error > 0 )
    {
        //Circuit::self()->removeComp( mcu );
        mcu = 0l;
        m_error = 0;
        //m_pSelf = 0l;
    }
    return mcu;
}

Mcu::Mcu( QObject* parent, QString type, QString id )
   : Chip( parent, type, id )
   , m_eMcu( id )
{
    QString compName = m_id.split("-").first(); // for example: "atmega328-1" to: "atmega328"

    m_icColor = QColor( 20, 30, 60 );

    QFont f = QFontDatabase::systemFont( QFontDatabase::FixedFont );
    f.setFamily("Monospace");
    f.setPixelSize(5);
    f.setLetterSpacing( QFont::PercentageSpacing, 120 );
    m_valLabel->setFont( f );
    m_valLabel->setPlainText( compName );
    m_valLabel->setDefaultTextColor( QColor( 110, 110, 110 ) );
    m_valLabel->setAcceptedMouseButtons( 0 );
    setShowVal( true );

    QString xmlFile = ComponentSelector::self()->getXmlFile( compName );
    QFile file( xmlFile );

    if(( xmlFile == "" ) || ( !file.exists() ))
    {
        m_error = 1;
        return;
    }
    QDomDocument domDoc = fileToDomDoc( xmlFile, "Mcu::Mcu" );
    if( domDoc.isNull() ) { m_error = 1; return; }

    QDomElement root  = domDoc.documentElement();
    QDomNode    rNode = root.firstChild();

    while( !rNode.isNull() )
    {
        QDomElement element = rNode.toElement();
        QDomNode    node    = element.firstChild();

        while( !node.isNull() )
        {
            QDomElement element = node.toElement();
            if( element.attribute("name")==compName )
            {
                // Get package file
                QDir dataDir( xmlFile );
                dataDir.cdUp();             // Indeed it doesn't cd, just take out file name
                m_pkgeFile = dataDir.filePath( element.attribute( "package" ) )+".package";

                // Get data file
                m_dataFile = dataDir.filePath( element.attribute( "data" ) )+".mcu";
                //create( dataFile );

                break;
            }
            node = node.nextSibling();
        }
        rNode = rNode.nextSibling();
    }
    /*if( m_device == "" ) //return;//Chip::initChip();
    {
        m_error = 1;
        qDebug() << compName << "ERROR!! Mcu::Mcu Chip not Found: " << compName;
        return;
    }*/

    QSettings* settings = MainWindow::self()->settings();
    m_lastFirmDir = settings->value("lastFirmDir").toString();

    if( m_lastFirmDir.isEmpty() )
        m_lastFirmDir = QCoreApplication::applicationDirPath();

    m_subcDir = "";

}
Mcu::~Mcu(){}

void Mcu::setLogicSymbol( bool ls )
{
    Chip::setLogicSymbol( ls );

    //QFontMetrics fm( m_valLabel->font() );
    //fm.width( m_valLabel->text() );

    setValLabelX( m_area.width()/2-3 );
    setValLabelY( m_area.height()/2+m_valLabel->textWidth()/2 );
    setValLabRot(-90 );
    setValLabelPos();
}

void Mcu::setProgram( QString pro )
{
    if( pro == "" ) return;
    m_firmware = pro;

    QDir circuitDir;
    if( m_subcDir != "" ) circuitDir.setPath( m_subcDir );
    else circuitDir = QFileInfo( Circuit::self()->getFileName() ).absoluteDir();
    QString fileNameAbs = circuitDir.absoluteFilePath( m_firmware );

    if( QFileInfo::exists( fileNameAbs ) ) // Load firmware at circuit load
    // && !m_processor->getLoadStatus() )
    {
        load( m_firmware );
    }
}

void Mcu::initialize()
{
}

void Mcu::remove()
{
    //emit closeSerials();

    //Simulator::self()->remFromUpdateList( this );
    for( Pin* pin : m_pinList )
    {
        if( pin->connector() ) pin->connector()->remove();
    }
    //terminate();
    m_pinList.clear();

    Component::remove();
}

void Mcu::slotLoad()
{
    const QString dir = m_lastFirmDir;
    QString fileName = QFileDialog::getOpenFileName( 0l, tr("Load Firmware"), dir,
                       tr("All files (*.*);;ELF Files (*.elf);;Hex Files (*.hex)"));

    if( fileName.isEmpty() ) return; // User cancels loading

    load( fileName );
}

void Mcu::load( QString fileName )
{
    QDir circuitDir;
    if( m_subcDir != "" ) circuitDir.setPath( m_subcDir );
    else circuitDir = QFileInfo( Circuit::self()->getFileName() ).absoluteDir();
    QString fileNameAbs = circuitDir.absoluteFilePath( fileName );
    QString cleanPathAbs = circuitDir.cleanPath( fileNameAbs );

    if( Simulator::self()->isRunning() )  CircuitWidget::self()->powerCircOff();

    QString msg = loadHex( cleanPathAbs, m_eMcu.m_wordSize );
    if( msg.isEmpty() )
    {
        msg ="hex file succesfully loaded";

        //if( !m_varList.isEmpty() ) m_processor->getRamTable()->loadVarSet( m_varList );

        m_firmware = circuitDir.relativeFilePath( fileName );
        m_lastFirmDir = cleanPathAbs;

        QSettings* settings = MainWindow::self()->settings();
        settings->setValue( "lastFirmDir", circuitDir.relativeFilePath( fileName ) );
    }
    qDebug() << msg << "\n";
}

void Mcu::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
    if( !acceptedMouseButtons() ) event->ignore();
    else
    {
        event->accept();
        QMenu* menu = new QMenu();
        contextMenu( event, menu );
        Component::contextMenu( event, menu );
        menu->deleteLater();
    }
}

void Mcu::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
{
    /*QAction* mainAction = menu->addAction( QIcon(":/subc.png"),tr("Main Mcu") );
    connect( mainAction, SIGNAL(triggered()),
                   this, SLOT(slotmain()), Qt::UniqueConnection );*/

    QAction* loadAction = menu->addAction( QIcon(":/load.png"),tr("Load firmware") );
    connect( loadAction, SIGNAL(triggered()),
                   this, SLOT(slotLoad()), Qt::UniqueConnection );

    /*QAction* reloadAction = menu->addAction( QIcon(":/reload.png"),tr("Reload firmware") );
    connect( reloadAction, SIGNAL(triggered()),
                     this, SLOT(slotReload()), Qt::UniqueConnection );

    QAction* loadDaAction = menu->addAction( QIcon(":/load.png"),tr("Load EEPROM data") );
    connect( loadDaAction, SIGNAL(triggered()),
                     this, SLOT(loadData()), Qt::UniqueConnection );

    QAction* saveDaAction = menu->addAction(QIcon(":/save.png"), tr("Save EEPROM data") );
    connect( saveDaAction, SIGNAL(triggered()),
                     this, SLOT(saveData()), Qt::UniqueConnection );

    menu->addSeparator();
    QAction* openRamTab = menu->addAction( QIcon(":/terminal.png"),tr("Open RamTable.") );
    connect( openRamTab, SIGNAL(triggered()),
                   this, SLOT(slotOpenRamTable()), Qt::UniqueConnection );

    QAction* openTerminal = menu->addAction( QIcon(":/terminal.png"),tr("Open Serial Monitor.") );
    connect( openTerminal, SIGNAL(triggered()),
                     this, SLOT(slotOpenTerm()), Qt::UniqueConnection );

    QAction* openSerial = menu->addAction( QIcon(":/terminal.png"),tr("Open Serial Port.") );
    connect( openSerial, SIGNAL(triggered()),
                   this, SLOT(slotOpenSerial()), Qt::UniqueConnection );*/

    menu->addSeparator();
}

void Mcu::addPin( QString id, QString type, QString label,
                     int pos, int xpos, int ypos, int angle, int length )
{
    bool portPin = false;
    if( type == "IO" )
    {
        int pinNum = id.right(1).toInt( &portPin );
        if( portPin )
        {
            QString portName = "PORT"+id.mid(1,1);
            McuPort* port = m_eMcu.m_ports.getPort( portName );
            Pin* pin = port->getPin( pinNum )->pin();

            pin->setPos( QPoint( xpos, ypos ) );
            pin->setPinAngle( angle );
            pin->setLength( length );
            pin->setLabelText( label );
            pin->setFlag( QGraphicsItem::ItemStacksBehindParent, false );
            m_pinList.append( pin );
        }
    }
    if( !portPin ) Chip::addPin( id,  type, label, pos, xpos, ypos, angle, length );
}

QString Mcu::loadHex( QString file, int WordSize )
{
    qDebug() << m_device << " Loading hex file: \n" << file;

    QStringList lineList = fileToStringList( file, "Mcu::loadHex" );

    int nLine = 0;
    int addr;
    int nBytes;
    int type;
    int checksum;
    int hiByte;
    uint16_t data;
    bool ok;

    for( QString line : lineList )
    {
        checksum = 0;
        line = line.remove( " " );
        if( line.isEmpty() ) continue;

        if( !(line.left(1) == ":") ) return "Error: Wrong Start code at line "+QString::number(nLine);
        line = line.remove( 0, 1 );

        nBytes = line.left( 2 ).toInt( &ok, 16 );
        int lineSize = 2+4+2+nBytes*2+2;
        if( line.size() != lineSize ) return "Error: Wrong line size at line "+QString::number(nLine);
        checksum += nBytes;

        addr = line.mid( 2, 4 ).toInt( &ok, 16 );
        addr /= WordSize;
        checksum += line.mid( 2, 2 ).toInt( &ok, 16 );
        checksum += line.mid( 4, 2 ).toInt( &ok, 16 );

        type = line.mid( 6, 2 ).toInt( &ok, 16 );
        if     ( type == 1 ) return ""; // Reached End Of File
        else if( type != 0 ) continue;  //return "Error: Not valid Record type at line "+QString::number(nLine);
        checksum += type;

        int i;
        for( i=8; i<8+nBytes*2; i+=2*WordSize )
        {
            data = line.mid( i, 2 ).toInt( &ok, 16 );
            checksum += data;

            if( WordSize == 2 )
            {
                hiByte = line.mid( i+2, 2 ).toInt( &ok, 16 );
                data += (hiByte<<8);
                checksum += hiByte;
            }
            m_eMcu.m_progMem[addr] = data;
            addr++;
        }
        checksum += line.mid( i, 2 ).toInt( &ok, 16 );
        if( checksum & 0xFF ) return "Error: CheckSum Error at line "+QString::number(nLine);
        nLine++;
    }
    return "Error: No End Of File reached";
}

#include "moc_mcu.cpp"
