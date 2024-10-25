/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "picadc.h"
#include "picvref.h"
#include "mcupin.h"
#include "e_mcu.h"
#include "datautils.h"
#include "regwatcher.h"
#include "simulator.h"

PicAdc* PicAdc::createAdc( eMcu* mcu, QString name, int type )
{
    switch( type ){
        case 00: return new PicAdc00( mcu, name ); break;
        case 10: return new PicAdc10( mcu, name ); break;
        case 11: return new PicAdc11( mcu, name ); break;
        case 20: return new PicAdc20( mcu, name ); break;
        default: return nullptr;
}   }

PicAdc::PicAdc( eMcu* mcu, QString name )
      : McuAdc( mcu, name )
{
}
PicAdc::~PicAdc(){}

void PicAdc::setup()
{
    m_sleepMode = 0xFF;

    m_ADON = m_mcuRam->getRegBits("ADON" );
    m_GODO = m_mcuRam->getRegBits("GO/DONE" );
    m_ADFM = m_mcuRam->getRegBits("ADFM" );

    m_pRefPin = nullptr;
    m_nRefPin = nullptr;
}

void PicAdc::initialize()
{
    McuAdc::initialize();
    m_pRefPin = m_refPin.at(0);
    if( m_refPin.size() > 1 ) m_nRefPin = m_refPin.at(1);
}

void PicAdc::configureA() // ADCON0
{
    m_enabled = m_ADON.getRegBitsBool();

    uint8_t prs = m_ADSC.getRegBitsVal();
    setAdcClock( prs );

    m_channel = m_CHS.getRegBitsVal();

    bool convert = m_GODO.getRegBitsBool();
    if( !m_converting && convert ) startConversion();
}

void PicAdc::endConversion()
{
    if( m_leftAdjust ) m_adcValue <<= 6;
    m_GODO.clear_08(); // Clear GO/DONE bit
}

void PicAdc::setAdcClock( uint8_t prs )
{
    m_adcClock = (prs == 3);
    if( m_adcClock ) m_convTime = 4*12*1e6;
    else             m_convTime = m_mcu->psInst()*12*m_prescList[prs];
}

void PicAdc::sleep( int mode )
{
    McuModule::sleep( mode );
    m_sleeping = m_sleeping && !m_adcClock; // Only sleep if running on system clock
    if( m_sleeping ) pauseEvents();
    else             resumeEvents();
}

//------------------------------------------------------
//-- PIC ADC Type 0 ------------------------------------

PicAdc00::PicAdc00( eMcu* mcu, QString name )
        : PicAdc( mcu, name )
{
}
PicAdc00::~PicAdc00(){}

void PicAdc00::setup()
{
    PicAdc::setup();

    m_ADSC = m_mcuRam->getRegBits("ADSC0,ADCS1" );
    m_CHS  = m_mcuRam->getRegBits("CHS0,CHS1,CHS2" );
    m_PCFG = m_mcuRam->getRegBits("PCFG0,PCFG1,PCFG2,PCFG3" );
}

void PicAdc00::configureB() // ADCON1
{
    m_leftAdjust = !m_ADFM.getRegBitsBool();

    uint8_t mode = m_PCFG.getRegBitsVal();
    if( mode != m_mode )
    {
        m_mode = mode;
        uint8_t analog = 0;

        switch( mode ) {
            case 0:  analog = 0b11111111; break;
            case 1:  analog = 0b11110111; break;
            case 2:  analog = 0b00011111; break;
            case 3:  analog = 0b00010111; break;
            case 4:  analog = 0b00001011; break;
            case 5:  analog = 0b00000011; break;
            case 8:  analog = 0b11110011; break;
            case 9:  analog = 0b00111111; break;
            case 10: analog = 0b00110111; break;
            case 11: analog = 0b00110011; break;
            case 12: analog = 0b00010011; break;
            case 13: analog = 0b00000011; break;
            case 14:
            case 15: analog = 0b00000001;
        }
        for( uint i=0; i<m_adcPin.size(); ++i)
            if( m_adcPin[i] ) m_adcPin[i]->setAnalog( analog & (1<<i) );
}   }

void PicAdc00::updtVref()
{
    m_vRefP = m_mcu->vdd();
    m_vRefN = 0;

    switch( m_mode ){
        case 1:
        case 3:
        case 5:
        case 10: m_vRefP = m_pRefPin->getVoltage(); break;
        case 8:
        case 11:
        case 12:
        case 13:
        case 15: m_vRefP = m_pRefPin->getVoltage();
                 m_vRefN = m_nRefPin->getVoltage();
}   }

//------------------------------------------------------
//-- PIC ADC Type 1 ------------------------------------

PicAdc1::PicAdc1( eMcu* mcu, QString name )
       : PicAdc( mcu, name )
{

}
PicAdc1::~PicAdc1(){}

void PicAdc1::setup()
{
    PicAdc::setup();

    m_ANSELH = nullptr;
    m_ANSEL  = (uint8_t*) m_mcuRam->getReg( "ANSEL" );
    m_mcuRam->watchRegName( "ANSEL" , R_WRITE, this, &PicAdc1::updtANSEL );
}

void PicAdc1::updtANSEL()
{
    uint16_t analog = *m_ANSEL;
    if( m_ANSELH ) analog |= (*m_ANSELH << 8);
    for( uint i=0; i<m_adcPin.size(); ++i)
        if( m_adcPin[i] ) m_adcPin[i]->setAnalog( (analog>>i) & 1 );
}

void PicAdc1::updtVref()
{
    m_vRefP = (m_mode & 1) ? m_pRefPin->getVoltage() : m_mcu->vdd();
    if( m_nRefPin ) m_vRefN = (m_mode & 0b00000010) ? m_nRefPin->getVoltage() : 0;
}

//------------------------------------------------------
//-- PIC ADC Type 10 p16F88x ---------------------------

PicAdc10::PicAdc10( eMcu* mcu, QString name )
        : PicAdc1( mcu, name )
{
}
PicAdc10::~PicAdc10(){}

void PicAdc10::setup()
{
    PicAdc1::setup();

    m_ADSC = m_mcuRam->getRegBits("ADSC0,ADCS1" );
    m_CHS  = m_mcuRam->getRegBits("CHS0,CHS1,CHS2,CHS3" );
    m_VCFG = m_mcuRam->getRegBits("VCFG0,VCFG1" );

    m_ANSELH = (uint8_t*) m_mcuRam->getReg( "ANSELH" );
    m_mcuRam->watchRegName( "ANSELH", R_WRITE, this, &PicAdc10::updtANSEL );
}

void PicAdc10::configureB() // ADCON1
{
    m_leftAdjust = !m_ADFM.getRegBitsBool();
    m_mode       =  m_VCFG.getRegBitsVal();
}

//------------------------------------------------------
//-- PIC ADC Type 11 p12F675 ---------------------------

PicAdc11::PicAdc11( eMcu* mcu, QString name )
        : PicAdc1( mcu, name )
{
}
PicAdc11::~PicAdc11(){}

void PicAdc11::setup()
{
    PicAdc1::setup();

    m_ADSC = m_mcuRam->getRegBits("ADSC0,ADCS1,ADCS2");
    m_CHS  = m_mcuRam->getRegBits("CHS0,CHS1");
    m_VCFG = m_mcuRam->getRegBits("VCFG");
}

void PicAdc11::configureA() // ADCON0
{
    m_leftAdjust = !m_ADFM.getRegBitsBool();
    m_mode       = m_VCFG.getRegBitsVal();
    m_enabled    = m_ADON.getRegBitsBool();
    m_channel    = m_CHS.getRegBitsVal();

    bool convert = m_GODO.getRegBitsBool();
    if( !m_converting && convert ) startConversion();
}

void PicAdc11::updtANSEL()
{
    uint8_t prs = m_ADSC.getRegBitsVal();
    setAdcClock( prs );
    PicAdc1::updtANSEL();
}

//------------------------------------------------------
//-- PIC ADC Type 20 p16f1826 --------------------------

PicAdc20::PicAdc20( eMcu* mcu, QString name )
        : PicAdc( mcu, name )
{
}
PicAdc20::~PicAdc20(){}

void PicAdc20::setup()
{
    PicAdc::setup();

    m_ADSC   = m_mcuRam->getRegBits("ADCS0,ADCS1,ADCS2");
    m_CHS    = m_mcuRam->getRegBits("CHS0,CHS1,CHS2,CHS3,CHS4");
    m_ADXREF = m_mcuRam->getRegBits("ADPREF0,ADPREF1,ADNREF");

    m_fvr = (PicVrefE*)m_mcu->getModule("Vref");
}

void PicAdc20::configureA() // ADCON0
{
    m_enabled    = m_ADON.getRegBitsBool();
    m_channel    = m_CHS.getRegBitsVal();
    bool convert = m_GODO.getRegBitsBool();
    if( !m_converting && convert ) startConversion();
}

void PicAdc20::configureB() // ADCON1
{
    m_leftAdjust = !m_ADFM.getRegBitsBool();

    m_mode = m_ADXREF.getRegBitsVal();

    uint8_t prs = m_ADSC.getRegBitsVal();
    setAdcClock( prs );
}

void PicAdc20::updtVref()
{
    m_vRefP = m_mcu->vdd();  // VREF+ is connected to VDD
    switch ( m_mode ) {
    case 2: m_vRefP = m_pRefPin->getVoltage(); break; // VREF+ is connected to external VREF+ pin
    case 3: m_vRefP = m_fvr->getDacVref();     break; // VREF+ is connected to internal Fixed Voltage Reference (FVR) module
    }
    m_vRefN = (m_mode & 0b00000100) ? m_nRefPin->getVoltage() : 0;
}

//------------------------------------------------------
//-- PIC ADC Type 3 ------------------------------------

PicAdc3::PicAdc3( eMcu* mcu, QString name )
       : PicAdc( mcu, name )
{
}
PicAdc3::~PicAdc3(){}
