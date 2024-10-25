/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "piccomparator.h"
#include "mcuram.h"
#include "e_mcu.h"
#include "mcupin.h"
#include "mcuvref.h"

PicComp* PicComp::createComparator( eMcu* mcu, QString name, int type ) // Static
{
    switch( type ){
        case 01: return new PicComp01( mcu, name );
        case 02: return new PicComp02( mcu, name );
        case 03: return new PicComp03( mcu, name );
        case 11: return new PicComp11( mcu, name );
        case 12: return new PicComp12( mcu, name );
        default: return nullptr;
    }
}

PicComp::PicComp( eMcu* mcu, QString name )
       : McuComp( mcu, name )
{
}
PicComp::~PicComp(){}

void PicComp::setup()
{
    McuVref* vref = (McuVref*)m_mcu->getModule("Vref");
    if( vref ) vref->callBack( this, true ); // Vref Module will update vref if changed.
}

void PicComp::initialize()
{
    McuComp::initialize();
    m_pinP = nullptr;
    m_pinN = nullptr;
    m_pinOut = nullptr;
    m_enabled = false;
}

void PicComp::voltChanged()
{
    if( !m_enabled ) return;

    double vRef = m_fixVref ? m_vref : m_pinP->getVoltage();
    bool compOut = vRef > m_pinN->getVoltage() ;
    if( m_inv ) compOut = !compOut;

    if( m_compOut != compOut )
    {
        m_compOut = compOut;
        m_interrupt->raise();
        if( m_pinOut ) m_pinOut->setOutState( m_compOut );

        m_COUT.setBits_08( m_compOut );
    }
}

void PicComp::connect( McuPin* pinN, McuPin* pinP, McuPin* pinOut )
{
    if( pinN != m_pinN )
    {
        if( m_pinN ) m_pinN->changeCallBack( this, false );
        if( pinN )   pinN->changeCallBack( this, true );
        m_pinN = pinN;
    }
    if( pinP != m_pinP )
    {
        if( m_pinP ) m_pinP->changeCallBack( this, false );
        if( pinP )   pinP->changeCallBack( this, true );
        m_pinP = pinP;
    }
    if( pinOut != m_pinOut )
    {
        if( m_pinOut ) m_pinOut->controlPin( false, false );
        if( pinOut )   pinOut->controlPin( true, false );

        m_pinOut = pinOut;
    }
    if( m_pinN ) voltChanged(); // Update Comparator state
}

//-------------------------------------------------------------
// Type 0: 16f627 comparators  --------------------------------

PicComp0::PicComp0( eMcu* mcu, QString name )
        : PicComp( mcu, name )
{
    m_CM  = m_mcuRam->getRegBits("CM0,CM1,CM2");
    m_CIS = m_mcuRam->getRegBits("CIS");
}
PicComp0::~PicComp0(){}

void PicComp0::configureA() // CMCON
{
    m_cis = m_CIS.getRegBitsBool();
    m_inv = m_CINV.getRegBitsBool();

    uint8_t mode = m_CM.getRegBitsVal();
    if( mode != m_mode )
    {
        m_mode = mode;
        m_fixVref = false;
        m_enabled = true;
        setMode( mode );
    }
    voltChanged();
}

//-------------------------------------------------------------
// Type 01: 16f627 comparator 1 -------------------------------

PicComp01::PicComp01( eMcu* mcu, QString name )
         : PicComp0( mcu, name )
{
    m_CINV = m_mcuRam->getRegBits("C1INV");
    m_COUT = m_mcuRam->getRegBits("C1OUT");
}
PicComp01::~PicComp01(){}

void PicComp01::setMode( uint8_t mode )
{
    McuPin* pinN = m_cis ? m_pins[1] : m_pins[0];

    switch ( mode ) {
        case 0: m_enabled = false; connect( m_pins[0], m_pins[1], nullptr      ); break;
        case 1:                    connect( pinN     , m_pins[2], nullptr      ); break;
        case 2: m_fixVref = true;  connect( pinN     , nullptr     , nullptr      ); break;
        case 3:                    connect( m_pins[0], m_pins[2], nullptr      ); break;
        case 4:                    connect( m_pins[0], m_pins[1], nullptr      ); break;
        case 5: m_enabled = false; connect( nullptr     , nullptr     , nullptr      ); break;
        case 6:                    connect( m_pins[0], nullptr     , m_pins[1] ); break;
        case 7: m_enabled = false; connect( nullptr     , nullptr     , nullptr      ); break;
    }
}

//-------------------------------------------------------------
// Type 02: 16f627 comparator 2 -------------------------------

PicComp02::PicComp02( eMcu* mcu, QString name )
         : PicComp0( mcu, name )
{
    m_CINV = m_mcuRam->getRegBits("C2INV");
    m_COUT = m_mcuRam->getRegBits("C2OUT");
}
PicComp02::~PicComp02(){}

void PicComp02::setMode( uint8_t mode )
{
    McuPin* pinN = m_cis ? m_pins[1] : m_pins[0];

    switch ( mode ) {
        case 0: m_enabled = false;
        case 1:                    connect( m_pins[0], m_pins[1], nullptr      ); break;
        case 2: m_fixVref = true;  connect( pinN     , nullptr     , nullptr      ); break;
        case 3:                                                                // Fallthrough
        case 4:                                                                // Fallthrough
        case 5:                    connect( m_pins[0], m_pins[1], m_pins[2] ); break;
        case 6:                    connect( m_pins[0], m_pins[1], nullptr      ); break;
        case 7: m_enabled = false; connect( nullptr     , nullptr     , nullptr      ); break;
    }
}

//-------------------------------------------------------------
// Type 01: 16f629 comparator ---------------------------------

PicComp03::PicComp03( eMcu* mcu, QString name )
         : PicComp0( mcu, name )
{
    m_CINV = m_mcuRam->getRegBits("CINV");
    m_COUT = m_mcuRam->getRegBits("COUT");
}
PicComp03::~PicComp03(){}

void PicComp03::setMode( uint8_t mode )
{
    McuPin* pinO = nullptr;
    McuPin* pinN = m_cis ? m_pins[1] : m_pins[0];

    switch ( mode ) {
        case 0: m_enabled = false; connect( m_pins[0], m_pins[1], nullptr      ); break;
        case 1:                    connect( m_pins[0], m_pins[1], m_pins[2] ); break;
        case 2:                    connect( m_pins[0], m_pins[1], nullptr      ); break;
        case 3: m_fixVref = true;  connect( m_pins[0], nullptr     , m_pins[2] ); break;
        case 4: m_fixVref = true;  connect( m_pins[0], nullptr     , nullptr      ); break;
        case 5:                                             pinO = m_pins[2];  // Fallthrough
        case 6: m_fixVref = true;  connect( pinN     , nullptr     , pinO      ); break;
        case 7: m_enabled = false; connect( nullptr     , nullptr     , nullptr      ); break;
    }
}

//-------------------------------------------------------------
// Type 1: 16f88x comparators  --------------------------------

PicComp1::PicComp1( eMcu* mcu, QString name )
        : PicComp( mcu, name )
{
    QString n = name.right(1); // name="Comp01" => n="1"

    m_CMxCON0  = (uint8_t*) m_mcuRam->getReg("CM"+n+"CON0");

    m_CxON = m_mcuRam->getRegBits("C"+n+"ON");
    m_COUT = m_mcuRam->getRegBits("C"+n+"OUT");
    m_CxOE = m_mcuRam->getRegBits("C"+n+"OE");
    m_CINV = m_mcuRam->getRegBits("C"+n+"POL");
    m_CxR  = m_mcuRam->getRegBits("C"+n+"R");
    m_CxCH = m_mcuRam->getRegBits("C"+n+"CH0,C"+n+"CH1");
}
PicComp1::~PicComp1(){}

void PicComp1::configureA() // CMxCON0
{
    if( *m_CMxCON0 == m_mode ) return;
    m_mode = *m_CMxCON0;

    m_enabled = m_CxON.getRegBitsBool();
    if( !m_enabled )
    {
        connect( nullptr, nullptr, nullptr );
        return;
    }
    bool OE = m_CxOE.getRegBitsBool();
    McuPin* pinOut = OE ? m_pins[5] : nullptr;

    m_inv     = m_CINV.getRegBitsBool();
    m_fixVref = m_CxR.getRegBitsBool();

    uint8_t channel = m_CxCH.getRegBitsVal();
    connect( m_pins[channel], m_pins[4], pinOut );
}

//-------------------------------------------------------------
// Type 11: 16f88x comparator 1 -------------------------------

PicComp11::PicComp11( eMcu* mcu, QString name )
         : PicComp1( mcu, name )
{

}
PicComp11::~PicComp11(){}

void PicComp11::configureC() // SRCON
{
    /// TODO
}

//-------------------------------------------------------------
// Type 12: 16f88x comparator 2 -------------------------------

PicComp12::PicComp12( eMcu* mcu, QString name )
         : PicComp1( mcu, name )
{

}
PicComp12::~PicComp12(){}

void PicComp12::configureB() // CM2CON1
{
    /// TODO
}

void PicComp12::configureC() // SRCON
{
    /// TODO
}

//-------------------------------------------------------------
// Type 20: 16f1826 comparators  ------------------------------

PicComp20::PicComp20( eMcu* mcu, QString name )
         : PicComp( mcu, name )
{

}
PicComp20::~PicComp20(){}

void PicComp20::configureA() // CMxCON0
{
    /// TODO
}

void PicComp20::configureB() // CMxCON1
{
    /// TODO
}
