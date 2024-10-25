/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "picvref.h"
#include "mcuram.h"
#include "e_mcu.h"
#include "mcupin.h"

PicVref::PicVref( eMcu* mcu, QString name )
       : McuVref( mcu, name )
{
}
PicVref::~PicVref(){}

void PicVref::setup()
{
    m_VREN = m_mcuRam->getRegBits("VREN");
    m_VROE = m_mcuRam->getRegBits("VROE");
    m_VRR  = m_mcuRam->getRegBits("VRR");
    m_VR   = m_mcuRam->getRegBits("VR0,VR1,VR2,VR3");
}

void PicVref::initialize()
{
    m_vref = m_mcu->vdd()/4; //setMode( 0 );
}

void PicVref::configureA()
{
    m_enabled = m_VREN.getRegBitsBool();

    bool     vrr = m_VRR.getRegBitsBool();
    uint8_t mode = m_VR.getRegBitsVal();
    if( mode != m_mode || vrr != m_vrr )
    {
        m_mode = mode;
        m_vrr = vrr;

        if( m_enabled ){
            double vdd = m_mcu->vdd();
            if( vrr ) m_vref = vdd*mode/24; /// TODO: get Vdd or use VrefP
            else      m_vref = vdd/4+vdd*mode/32;
        }
        else m_vref = 0;
    }
    bool vroe = m_VROE.getRegBitsBool();
    if( vroe != m_vroe )    // VDD-┬-8R-R-..16 Stages..-R-8R-┬-GND
    {                       // VrP-┘                     -VRR┴-VrN
        m_vroe = vroe;      /// TODO: Add VrefP/VrefN option to ladder
        if( m_pinOut ){
            double vddAdmit = 0;
            double gndAdmit = 0;
            if( vroe && m_enabled )
            {
                vddAdmit = 8+16-m_mode;
                if( !vrr ) gndAdmit = 8;
                gndAdmit += m_mode;
            }
            m_pinOut->setExtraSource( vddAdmit, gndAdmit );
        }
    }
    if( !m_callBacks.isEmpty() )
    { for( McuModule* mod : m_callBacks ) mod->callBackDoub( m_vref ); }
}

/*void PicVref::setMode( uint8_t mode )
{
    m_mode = mode;
}*/

//-------------------------------------------------------------
// Type 0: 16f1826 FVR  ---------------------------------------

PicVrefE::PicVrefE( eMcu* mcu, QString name )
        : McuVref( mcu, name )
{
}
PicVrefE::~PicVrefE(){}

void PicVrefE::setup()
{
    m_FVREN  = m_mcuRam->getRegBits("FVREN");
    m_ADFVR  = m_mcuRam->getRegBits("ADFVR0,ADFVR1");   // ADC Vref
    m_CDAFVR = m_mcuRam->getRegBits("CDAFVR0,CDAFVR1"); // DAC Vref
}

void PicVrefE::configureA() // FVRCON
{
    m_enabled = m_FVREN.getRegBitsVal();

    double vdd = m_mcu->vdd();
    uint8_t adfvr = m_ADFVR.getRegBitsVal();
    switch( adfvr ) {
        case 0: m_adcVref = 0.000; break;
        case 1: m_adcVref = 1.024; break;
        case 2: m_adcVref = 2.048; break;
        case 3: m_adcVref = 4.096; break;
    }
    if( m_adcVref > vdd ) m_adcVref = vdd;

    uint8_t cdafvr = m_CDAFVR.getRegBitsVal();
    switch( cdafvr ) {
        case 0: m_dacVref = 0.000; break;
        case 1: m_dacVref = 1.024; break;
        case 2: m_dacVref = 2.048; break;
        case 3: m_dacVref = 4.096; break;
    }
    if( m_dacVref > vdd ) m_dacVref = vdd;

    if( !m_callBacks.isEmpty() )
    { for( McuModule* mod : m_callBacks ) mod->callBack(); }
}

double PicVrefE::getAdcVref()
{
    if( m_enabled ) return m_adcVref;
    else            return 0;
}

double PicVrefE::getDacVref()
{
    if( m_enabled ) return m_dacVref;
    else            return 0;
}
