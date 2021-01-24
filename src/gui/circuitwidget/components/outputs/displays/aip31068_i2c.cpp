/***************************************************************************
 *   Copyright (C) 2016 by santiago González                               *
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


#include "simulator.h"
#include "aip31068_i2c.h"
#include "utils.h"

static const char* Aip31068_i2c_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","Control Code")
};

Component* Aip31068_i2c::construct( QObject* parent, QString type, QString id )
{
    return new Aip31068_i2c( parent, type, id );
}

LibraryItem* Aip31068_i2c::libraryItem()
{
    return new LibraryItem(
        tr( "Aip31068_i2c" ),
        tr( "Displays" ),
        "aip31068.png",
        "Aip31068_i2c",
        Aip31068_i2c::construct );
}

Aip31068_i2c::Aip31068_i2c( QObject* parent, QString type, QString id )
            : Hd44780_Base( parent, type, id )
            , eI2CSlave( id )
{
    Q_UNUSED( Aip31068_i2c_properties );

    m_address = 0x3E;

    m_pin.resize( 2 );

    m_pinSDA = new Pin( 270, QPoint(16, 8), id+"PinSDA", 0, this );
    m_pinSCL = new Pin( 270, QPoint(24, 8), id+"PinSCL", 0, this );
    m_pinSDA->setLabelText( " SDA" );
    m_pinSCL->setLabelText( " SCL" );

    m_pin[0] = m_pinSDA;
    m_pin[1] = m_pinSCL;

    eLogicDevice::createInput( m_pinSDA );
    eLogicDevice::createClockPin( m_pinSCL );

    Simulator::self()->addToUpdateList( this );
    
    initialize();
}
Aip31068_i2c::~Aip31068_i2c(){}

QList<propGroup_t> Aip31068_i2c::propGroups()
{
    propGroup_t mainGroup { tr("Main") };
    mainGroup.propList.append( {"Rows", tr("Rows"),""} );
    mainGroup.propList.append( {"Cols", tr("Columns"),""} );
    mainGroup.propList.append( {"Control_Code", tr("I2C Address"),""} );
    mainGroup.propList.append( {"Frequency", tr("I2C Frequency"),"KHz"} );
    return {mainGroup};
}

void Aip31068_i2c::updateStep() { update(); }

void Aip31068_i2c::initialize()
{
    m_enabled = true;
    eI2CSlave::initialize();

    m_controlByte = 0;
    m_phase = 3;

    Hd44780_Base::init();
}

void Aip31068_i2c::voltChanged()             // Called when clock Pin changes
{
    eI2CSlave::voltChanged();

    if( m_state == I2C_STOPPED ) m_phase = 3;
}

void Aip31068_i2c::startWrite()
{
    m_phase = 0;
}

void Aip31068_i2c::readByte()
{
    if( m_phase == 0 )
    {
        m_phase++;
        m_controlByte = m_rxReg;
    }
    else if( m_phase == 1 )
    {
        m_phase++;
        int data = eI2CSlave::byteReceived();
        int rs = m_controlByte & 0x40;

        if( rs ) writeData(data);
        else     proccessCommand(data);
    }
    eI2CSlave::readByte();
}

int Aip31068_i2c::cCode()
{
    return m_address;
}

void Aip31068_i2c::setCcode( int code )
{
    m_address = code;
}

void Aip31068_i2c::showPins( bool show )
{
    m_pinSDA->setVisible( show );
    m_pinSCL->setVisible( show );
}

void Aip31068_i2c::remove()
{
    Simulator::self()->remFromUpdateList( this );

    Component::remove();
}
#include "moc_aip31068_i2c.cpp"

