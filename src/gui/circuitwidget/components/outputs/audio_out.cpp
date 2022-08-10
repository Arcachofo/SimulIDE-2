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

#include <QCoreApplication>
#include <QPainter>
#include <QtMath>

#include "audio_out.h"
#include "connector.h"
#include "itemlibrary.h"
#include "pin.h"
#include "label.h"
#include "simulator.h"

#include "doubleprop.h"
#include "boolprop.h"

Component* AudioOut::construct( QObject* parent, QString type, QString id )
{ return new AudioOut( parent, type, id ); }

LibraryItem* AudioOut::libraryItem()
{
    return new LibraryItem(
        tr( "Audio Out" ),
        tr( "Outputs" ),
        "audio_out.png",
        "AudioOut",
        AudioOut::construct);
}

AudioOut::AudioOut( QObject* parent, QString type, QString id )
        : Component( parent, type, id )
        , eResistor( id )
{
    m_area = QRect( -12, -24, 24, 40 );
    
    m_pin.resize( 2 );
    m_ePin[0] = m_pin[0] = new Pin( 180, QPoint(-16,-8), id+"-lPin", 0, this);
    m_pin[0]->setLabelColor( QColor( 0, 0, 0 ) );
    m_pin[0]->setLength( 6 );
    m_pin[0]->setLabelText( "+" );

    m_ePin[1] = m_pin[1] = new Pin( 180, QPoint(-16,0), id+"-rPin", 1, this);
    m_pin[1]->setLabelColor( QColor( 0, 0, 0 ) );
    m_pin[1]->setLength( 6 );
    m_pin[1]->setLabelText( "-" );

    setLabelPos(-20,-36, 0);
    
    m_admit = 1.0/8;
    m_buzzer = false;

    m_deviceinfo = QAudioDeviceInfo::defaultOutputDevice(); 
    if( m_deviceinfo.isNull() ) 
    {
        const auto deviceInfos = QAudioDeviceInfo::availableDevices( QAudio::AudioOutput );
        for( const QAudioDeviceInfo &deviceInfo : deviceInfos )
            qDebug() << "Device name: " << deviceInfo.deviceName();

        qDebug() <<"   Error: No defaulf Audio Output Device Found" ;
        return;
    }
    //int refreshPeriod = 10; // mS
    //int sampleRate    = 40000; // samples/S
    m_format = m_deviceinfo.preferredFormat();
    m_format.setCodec( "audio/pcm" );
    //m_format.setSampleRate( sampleRate );
    m_format.setChannelCount(1);
    m_format.setSampleSize(8);
    m_format.setSampleType( QAudioFormat::UnSignedInt );
    m_format.setByteOrder( QAudioFormat::LittleEndian );  

    if( !m_deviceinfo.isFormatSupported( m_format )) 
    {  
        qDebug() << "Warning: Default format not supported - trying to use nearest";
        m_format = m_deviceinfo.nearestFormat( m_format );  
        qDebug() << m_format.sampleRate() << m_format.channelCount()<<m_format.sampleSize();
    }  
    m_audioOutput = new QAudioOutput( m_deviceinfo, m_format );
    
    //m_dataSize = refreshPeriod*sampleRate/1000;
    //m_dataBuffer.resize( m_dataSize*10 );
    //initialize();

    //connect( m_audioOutput, SIGNAL( stateChanged( QAudio::State ) ),
    //                 this, SLOT( stateChanged( QAudio::State ) ), Qt::UniqueConnection );

    addPropGroup( { tr("Main"), {
new BoolProp<AudioOut>( "Buzzer"   , tr("Buzzer")   ,"" , this, &AudioOut::buzzer, &AudioOut::setBuzzer ),
new DoubProp<AudioOut>( "Impedance", tr("Impedance"),"Ω", this, &AudioOut::res,    &AudioOut::setResSafe )
    }} );
}
AudioOut::~AudioOut()
{
    delete m_audioOutput;
}

void AudioOut::initialize()
{
    if( m_deviceinfo.isNull() ) return;

    m_dataBuffer.clear();
    m_audioOutput->stop();
    m_audioOutput->reset();

    Simulator::self()->cancelEvents( this );
}

void AudioOut::stamp()
{
    eResistor::stamp();

    if( m_deviceinfo.isNull() ) return;

    m_audioBuffer = m_audioOutput->start();
    m_dataSize = m_audioOutput->periodSize();
    m_dataBuffer.reserve( m_dataSize );

    if( m_ePin[0]->isConnected() && m_ePin[1]->isConnected() )
            Simulator::self()->addEvent( 1, this );
}

void AudioOut::runEvent()
{
    double voltPN = m_ePin[0]->getVolt()-m_ePin[1]->getVolt();
    int outVal = 128;

    if( m_buzzer){
        if( voltPN > 2.5 )
        {
            double stepsPC = 1e12/1000;
            double time = Simulator::self()->circTime();
            time = remainder( time, stepsPC );
            time = qDegreesToRadians( time*360/stepsPC );

            outVal += sin( time )*128;
    }   }
    else outVal += voltPN*51;

    if     ( outVal > 255 ) outVal = 255;
    else if( outVal < 0 )   outVal = 0;

    m_dataBuffer.append( (char)outVal );

    if( m_dataBuffer.size() == m_dataSize )
    {
        //qDebug() <<"pushing"<<m_dataBuffer.size()<< m_audioOutput->bytesFree() ;

        m_audioBuffer->write( m_dataBuffer.data(), m_dataSize );
        m_dataBuffer.clear();
    }
    double realSpeed = Simulator::self()->realSpeed();
    if( realSpeed < 1e-6 )
    {
        realSpeed = Simulator::self()->psPerSec();
        realSpeed /= 1e8;
    }
    realSpeed *= (1e12/10000);
    uint64_t nextEvent = realSpeed/m_format.sampleRate();//realSpeed*25*1e2;//(realSpeed/10000)*25*1e6
    Simulator::self()->addEvent( nextEvent, this ); // 25 us
}

/*void AudioOut::stateChanged( QAudio::State state )
{
    qDebug() << state << m_audioOutput->bytesFree() ;
}*/

QPainterPath AudioOut::shape() const
{
    QPainterPath path;
    
    QVector<QPointF> points;
    points << QPointF(-10,-12 )
           << QPointF(-10, 4 )
           << QPointF( 0, 4 )
           << QPointF( 10, 16 )
           << QPointF( 10, -24 )
           << QPointF( 0, -12 )
           << QPointF(-10, -12 );
        
    path.addPolygon( QPolygonF(points) );
    path.closeSubpath();
    return path;
}

void AudioOut::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );

    //p->drawRect( -10.5, -12, 12, 24 );
    static const QPointF points[7] = {
        QPointF(-10,-12 ),
        QPointF(-10, 4 ),
        QPointF( 0, 4 ),
        QPointF( 10, 16 ),
        QPointF( 10, -24 ),
        QPointF( 0, -12 ),
        QPointF(-10, -12 )
        };

    p->drawPolygon(points, 7);
    
    if( m_deviceinfo.isNull() ){
        p->drawLine(0,-8, 7, 0 );
        p->drawLine( 7,-8,0, 0 );
}   }
