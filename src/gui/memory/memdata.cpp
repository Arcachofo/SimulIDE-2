/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QTranslator>
#include <QFileDialog>
#include <QMessageBox>

#include "memdata.h"
#include "memtable.h"
#include "simulator.h"
#include "circuitwidget.h"
#include "circuit.h"
#include "mcuconfigword.h"
#include "e_mcu.h"
#include "utils.h"


MemData::MemData()
{
    m_memTable = nullptr;
    m_eMcu = nullptr;
}
MemData::~MemData()
{
    if( !m_memTable ) return;

    m_memTable->setParent( nullptr );
    m_memTable->deleteLater();
}

void MemData::showTable( int dataSize, int wordBytes )
{
    if( !m_memTable )
    {
        m_memTable = new MemTable( CircuitWidget::self(), dataSize, wordBytes );
        m_memTable->setWindowFlags( Qt::Window | Qt::WindowTitleHint | Qt::Tool
                                  | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint );
    }
    m_memTable->show();
}

bool MemData::loadData( bool resize )
{
    Simulator::self()->pauseSim();

    QString dir = changeExt( Circuit::self()->getFilePath(), ".data" );
    QString fileName = QFileDialog::getOpenFileName( nullptr,
                                                    "MemData::loadData", dir,
                       simulideTr( "MemData", "All files (*.*);;.data (*.data);;.bin (*.bin)"));

    if( fileName.isEmpty() ) return false; // User cancels loading

    bool ok = loadFile( fileName, resize );
    Simulator::self()->resumeSim();

    return ok;
}

bool MemData::loadFile( QString file, bool resize, eMcu* eMcu )
{
    m_eMcu = eMcu;
    if( eMcu ) return loadHex( file, resize ); // MCUs file must be hex format

    if( resize ) m_data.resize( 1 );

    QString ext = getFileExt( file ).toLower();

    bool ok = false;
    if     ( ext == ".data" ) ok = loadDat( file, resize );
    else if( ext == ".hex"
          || ext == ".ihx" )  ok = loadHex( file, resize ); // Intel Hex Format
    else                      ok = loadBin( file, resize ); // Binary Format

    m_eMcu = nullptr;
    return ok;
}

bool MemData::loadDat( QString file, bool resize )
{
    QString data = fileToString( file, "MemData::loadTxt" );

    data.replace("\n", ",");
    return loadDatStr( data, resize );
}

bool MemData::loadDatStr( QString data, bool resize )
{
    int addr = 0;
    int dataEnd = m_data.size()-1;

    QStringList words = data.split(",");

    for( QString sdata : words )
    {
        bool ok = false;
        int data = sdata.toInt( &ok, 10 );
        if( !ok ) continue;

        if( addr > dataEnd )
        {
            if( resize ){
                dataEnd++;
                m_data.resize( dataEnd+1 );
            }else{
                qDebug() << "\nMemData::loadDat: Data doesn't fit in Memory"<<dataEnd<<"\n";
                break;
            }
        }
        m_data.at( addr ) = data ;
        addr++;
    }
    return true;
}

bool MemData::loadHex( QString file, bool resize )
{
    qDebug() <<"Loading hex file:\n"<<file<<"\n";
    QStringList lineList = fileToStringList( file, "MemData::loadHex" );

    int nLine = 0;
    int addr;
    int addrBase = 0;
    int nBytes;
    int type;
    int checksum;
    int hiByte;
    uint16_t data;
    int dataEnd = m_data.size()-1;
    int bytes = (m_bits+7)/8;

    bool ok;

    for( QString line : lineList )
    {
        checksum = 0;
        line = line.remove( " " );
        if( line.isEmpty() ) continue;

        if( !(line.left(1) == ":") ){
            qDebug() << "    Error: Wrong Start code at line "+QString::number(nLine);
            return false;
        }
        line = line.remove( 0, 1 );

        nBytes = line.left( 2 ).toInt( &ok, 16 );
        int lineSize = 2+4+2+nBytes*2+2;
        if( line.size() != lineSize ){
            qDebug() << "    Error: Wrong line size at line "+QString::number(nLine);
            return false;
        }
        checksum += nBytes;

        addr = addrBase+line.mid( 2, 4 ).toInt( &ok, 16 );
        addr /= bytes;
        checksum += line.mid( 2, 2 ).toInt( &ok, 16 );
        checksum += line.mid( 4, 2 ).toInt( &ok, 16 );

        type = line.mid( 6, 2 ).toInt( &ok, 16 );
        if     ( type == 1 ) return true; // Reached End Of File
        else if( type == 4 );             // Extended Linear Address
        else if( type != 0 )
        {
            qDebug() <<"    Warning: Not supported Record type:"<<type<<"at line"<<QString::number(nLine);
            continue;
        }
        checksum += type;
        int i;
        for( i=8; i<8+nBytes*2; i+=2*bytes )
        {
            data = line.mid( i, 2 ).toInt( &ok, 16 );
            checksum += data;

            if( bytes == 2 ){
                hiByte = line.mid( i+2, 2 ).toInt( &ok, 16 );
                data += (hiByte<<8);
                checksum += hiByte;
            }
            if( type == 4 )
            {
                addrBase = (line.mid( 8, 4 ).toInt( &ok, 16 ))<<16;
                qDebug() <<"    Extended Linear Address:"<< "0x"+QString::number( addrBase, 16 ).toUpper();
                continue;
            }
            if( resize ){
                dataEnd++;
                m_data.resize( dataEnd+1 );
            }
            //qDebug()<< "MemData::loadHex"<<addrBase/bytes<< addr <<data;
            if( addr > dataEnd ){
                bool ok = false;
                if( m_eMcu )
                {
                    ConfigWord* cfgWord = (ConfigWord*)m_eMcu->getModule("cfgword");
                    if( cfgWord ) ok = cfgWord->setCfgWord( addr, data );
                }
                if( !ok )
                {
                    qDebug() << "    Warning: PGM End reached at Line"<<QString::number( nLine )
                             <<"\n    Address:"<<"0x"+QString::number( addr, 16 )<<"> PMG End:"<<"0x"+QString::number( dataEnd, 16 ).toUpper()
                             <<"\n    TODO: Config word ??"<<"\n";
                    return true;
                }
            }
            else m_data.at( addr ) = data;
            addr++;
        }
        checksum += line.mid( i, 2 ).toInt( &ok, 16 );
        if( checksum & 0xFF ){
            qDebug() << "    Error: CheckSum Error at line "+QString::number(nLine)+1;
            return false;
        }
        nLine++;
    }
    qDebug() << "    Error: No End Of File reached";
    return false;
}

bool MemData::loadBin( QString file, bool resize )
{
    int bytes = (m_bits+7)/8;
    int dataEnd = m_data.size()-1;

    QByteArray ba = fileToByteArray( file, "MemData::loadData" );
    int memSize = ba.size()/bytes;

    if( resize )
    {
        dataEnd = memSize-1;
        m_data.resize( memSize );
    }
    for( int i=0; i<memSize; i++ )
    {
        if( i > dataEnd ){
            qDebug() << "\nMemData::loadBin: Data doesn't fit in Memory"<<dataEnd<<"\n";
            return false;
        }
        int data = 0;
        for( int by=0; by<bytes; by++ )  // Join bytes little-endian
        {
            int val = ba.at( i*bytes+by );
            if( by>0 ) val <<= 8*by;
            data += val;
        }
        m_data.at(i) = data;
    }
    return true;
}

void MemData::saveData()
{
     Simulator::self()->pauseSim();

    QString dir = changeExt( Circuit::self()->getFilePath(), ".data" );

    QString fileName = QFileDialog::getSaveFileName( nullptr,
                       simulideTr( "MemData", "Save Data" ), dir,
                       simulideTr( "MemData", "All files (*.*);;.data (*.data);;.bin (*.bin)"));

    if( fileName.isEmpty() ) return; // User cancels saving

    QFile outFile( fileName );
    int bytes = (m_bits+7)/8;
    int i = 0;

    if( fileName.endsWith(".data") )
    {
        QString output = "";
        for( uint val : m_data )
        {
            QString sval = QString::number( val );
            while( sval.length() < 4) sval.prepend( " " );
            output += sval;

            if( i == 15 ) { output += "\n"; i = 0; }
            else          { output += ",";  i++;  }
        }
        if( !outFile.open( QFile::WriteOnly | QFile::Text ) )
        {
             QMessageBox::warning(nullptr, "MemData::saveData",
             simulideTr( "MemData", "Cannot write file %1:\n%2.").arg(fileName).arg(outFile.errorString()));
        }else{
            QTextStream toFile( &outFile );
            toFile << output;
            outFile.close();
    }   }
    else                          // Binary Format
    {
        if( !outFile.open( QFile::WriteOnly ) )
        {
              QMessageBox::warning(nullptr, "MemData::saveData",
              simulideTr( "MemData", "Cannot write file %1:\n%2.").arg(fileName).arg(outFile.errorString()));
        }else{
            for( uint val : m_data ){
                for( int by=0; by<bytes; by++ ) // Separate bytes little-endian
                {
                    char byte = val & 0xFF;
                    val >>= 8;
                    outFile.write( &byte, 1 );
            }   }
            outFile.close();
    }   }
    Simulator::self()->resumeSim();
}

void MemData::saveDat( int bits )
{

}

void MemData::saveHex( int bits ) /// TODO
{

}

void MemData::saveBin( int bits )
{

}

QString MemData::getMem()
{
    QString m;
    uint size = m_data.size();
    if( size > 0 )
    {
        bool empty = true;
        for( int i=size-1; i>=0; --i )
        {
            int val = m_data.at( i );
            if( val != 0 ) empty = false;
            if( empty ) continue;
            m.prepend( QString::number( val )+"," );
    }   }
    return m;
}

void MemData::setMem( QString m )
{
    if( m.isEmpty() ) return;

    QStringList list = m.split(",");
    uint i = 0;
    uint size = m_data.size();
    for( QString val : list )
    {
        if( i >= size ) break;
        m_data.at(i) = val.toInt();
        i++;
}   }
