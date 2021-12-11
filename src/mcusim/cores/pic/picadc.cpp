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

#include "picadc.h"
#include "mcupin.h"
#include "e_mcu.h"
#include "datautils.h"
#include "regwatcher.h"

PicAdc* PicAdc::createAdc( eMcu* mcu, QString name )
{
    int type = name.right( 1 ).toInt();
    switch( type )
    {
        case 0: return new PicAdc0( mcu, name ); break;
        case 1: return new PicAdc1( mcu, name ); break;
        case 2: return new PicAdc2( mcu, name ); break;
        case 3: return new PicAdc3( mcu, name ); break;
        default: return NULL;
    }
}

PicAdc::PicAdc( eMcu* mcu, QString name )
      : McuAdc( mcu, name )
{
    m_ADON = getRegBits( "ADON", mcu );
    m_ADSC = getRegBits( "ADSC0,ADCS1", mcu );
    m_GODO = getRegBits( "GODO", mcu );
    m_ADFM = getRegBits( "ADFM", mcu );

    m_pRefPin = NULL;
    m_nRefPin = NULL;
}
PicAdc::~PicAdc(){}

void PicAdc::initialize()
{
    McuAdc::initialize();
    m_pRefPin = m_refPin.at(0);
    m_nRefPin = m_refPin.at(1);
}

void PicAdc::configureA(uint8_t newADCON0 ) // ADCON0
{
    m_enabled = getRegBitsBool( newADCON0, m_ADON );

    uint8_t prs = getRegBitsVal( newADCON0, m_ADSC );
    if( prs == 3 ) m_convTime = 4*12*1e6;
    else           m_convTime = m_mcu->simCycPI()*12*m_prescList[prs];

    m_channel = getRegBitsVal( newADCON0, m_CHS );

    bool convert = getRegBitsBool( newADCON0, m_GODO );
    if( !m_converting && convert ) startConversion();
}

void PicAdc::endConversion()
{
    if( m_leftAdjust ) m_adcValue <<= 6;
    clearRegBits( m_GODO ); // Clear GO/DONE bit
}

//------------------------------------------------------
//-- PIC ADC Type 0 ------------------------------------

PicAdc0::PicAdc0( eMcu* mcu, QString name )
       : PicAdc( mcu, name )
{
    m_CHS  = getRegBits( "CHS0,CHS1,CHS2", mcu );
    m_PCFG = getRegBits( "PCFG0,PCFG1,PCFG2,PCFG3", mcu );
}
PicAdc0::~PicAdc0(){}

void PicAdc0::configureB( uint8_t newADCON1 ) // ADCON1
{
    m_leftAdjust = !getRegBitsBool( newADCON1, m_ADFM );

    uint8_t mode = getRegBitsVal( newADCON1, m_PCFG );
    if( mode != m_mode )
    {
        m_mode = mode;
        uint8_t analog = 0;

        switch( mode ) {
            case 0:
                analog = 0b11111111;
                break;
            case 1:
                analog = 0b11110111;
                break;
            case 2:
                analog = 0b00011111;
                break;
            case 3:
                analog = 0b00010111;
                break;
            case 4:
                analog = 0b00001011;
                break;
            case 5:
                analog = 0b00000011;
                break;
            case 8:
                analog = 0b11110011;
                break;
            case 9:
                analog = 0b00111111;
                break;
            case 10:
                analog = 0b00110111;
                break;
            case 11:
                analog = 0b00110011;
                break;
            case 12:
                analog = 0b00010011;
                break;
            case 13:
                analog = 0b00000011;
                break;
            case 14:
            case 15:
                analog = 0b00000001;
        }
        for( uint i=0; i<m_adcPin.size(); ++i) if( m_adcPin[i] ) m_adcPin[i]->setAnalog( analog & (1<<i) );
    }
}

double PicAdc0::getVref()
{
    double vRef = 0;

    switch( m_mode ) {
        case 1:
        case 3:
        case 5:
            vRef = m_pRefPin->getVolt();
            break;
        case 8:
            vRef = m_pRefPin->getVolt()-m_nRefPin->getVolt();
            break;
        case 10:
            vRef = m_pRefPin->getVolt();
            break;
        case 11:
        case 12:
        case 13:
            vRef = m_pRefPin->getVolt()-m_nRefPin->getVolt();
            break;
        case 15:
            vRef = m_pRefPin->getVolt();
            break;
        default: vRef = 5;
            break;
    }
    return vRef;
}

//------------------------------------------------------
//-- PIC ADC Type 1 ------------------------------------

PicAdc1::PicAdc1( eMcu* mcu, QString name )
       : PicAdc( mcu, name )
{
    m_CHS  = getRegBits( "CHS0,CHS1,CHS2,CHS3", mcu );
    m_VCFG = getRegBits( "VCFG0,VCFG1", mcu );

    m_ANSEL  = mcu->getReg( "ANSEL" );
    m_ANSELH = mcu->getReg( "ANSELH" );
    watchRegNames( "ANSEL" , R_WRITE, this, &PicAdc1::setANSEL , mcu );
    watchRegNames( "ANSELH", R_WRITE, this, &PicAdc1::setANSELH, mcu );
}
PicAdc1::~PicAdc1(){}

void PicAdc1::configureB( uint8_t newADCON1 ) // ADCON1
{
    m_leftAdjust = !getRegBitsBool( newADCON1, m_ADFM );

    m_mode = getRegBitsVal( newADCON1, m_VCFG );
}

void PicAdc1::setANSEL( uint8_t newANSEL )
{
    *m_ANSEL = newANSEL;
    updtANSEL();
}

void PicAdc1::setANSELH( uint8_t newANSELH )
{
    *m_ANSELH = newANSELH;
    updtANSEL();
}

void PicAdc1::updtANSEL()
{
    uint16_t analog = *m_ANSEL | (*m_ANSELH << 8);
    for( uint i=0; i<m_adcPin.size(); ++i)
        if( m_adcPin[i] ) m_adcPin[i]->setAnalog( analog & (1<<i) );
}

double PicAdc1::getVref()
{
    double vRef = 0;

    switch( m_mode ) {
        case 0: vRef = 5; break;
        case 1: vRef = m_pRefPin->getVolt(); break;
        case 2: vRef = 5-m_nRefPin->getVolt(); break;
        case 3: vRef = m_pRefPin->getVolt()-m_nRefPin->getVolt(); break;
    }
    return vRef;
}

//------------------------------------------------------
//-- PIC ADC Type 2 ------------------------------------

PicAdc2::PicAdc2( eMcu* mcu, QString name )
      : PicAdc0( mcu, name )
{
}
PicAdc2::~PicAdc2(){}

double PicAdc2::getVref()
{
    double vRef = 0;

    return vRef;
}

//------------------------------------------------------
//-- PIC ADC Type 3 ------------------------------------

PicAdc3::PicAdc3( eMcu* mcu, QString name )
       : PicAdc1( mcu, name )
{
}
PicAdc3::~PicAdc3(){}

double PicAdc3::getVref()
{
    double vRef = 0;

    return vRef;
}
