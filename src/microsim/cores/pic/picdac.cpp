/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "picdac.h"
#include "e_mcu.h"
#include "mcupin.h"
#include "picvref.h"
#include "mcuram.h"

PicDac::PicDac( eMcu* mcu, QString name )
      : McuDac( mcu, name )
{
}
PicDac::~PicDac(){}

void PicDac::setup()
{
    m_dacReg = nullptr;
    m_outPin = nullptr;

    m_DACEN  = m_mcuRam->getRegBits("DACEN");
    m_DACLPS = m_mcuRam->getRegBits("DACLPS");
    m_DACOE  = m_mcuRam->getRegBits("DACOE");
    m_DACPSS = m_mcuRam->getRegBits("DACPSS0,DACPSS1");
    m_DACNSS = m_mcuRam->getRegBits("DACNSS");

    m_DACR = m_mcuRam->getRegBits("DACR0,DACR1,DACR2,DACR3,DACR4");

    m_fvr = (PicVrefE*)m_mcu->getModule("Vref");
}

void PicDac::initialize()
{
    McuDac::initialize();

    if( m_pins.size() > 0 ) m_pRefPin = m_pins.at(0);
    else qDebug() << "PicDac::configureA: Error: NULL VREF+ Pin";

    if( m_pins.size() > 1 ) m_nRefPin = m_pins.at(1);
    else qDebug() << "PicDac::configureA: Error: NULL VREF- Pin";

    if( m_pins.size() > 2 ) m_outPin = m_pins.at(2);
    else qDebug() << "PicDac::configureA: Error: NULL Out Pin";
}

void PicDac::voltChanged()
{
    if( m_usePinP && m_pRefPin ) m_vRefP = m_pRefPin->getVoltage();
    if( m_usePinN && m_nRefPin ) m_vRefN = m_nRefPin->getVoltage();
    updtOutVolt();
}

void PicDac::configureA() // DACCON0
{
    m_enabled = m_DACEN.getRegBitsBool();
    m_daclps  = m_DACLPS.getRegBitsBool();

    m_outVoltEn = m_DACOE.getRegBitsBool();
    if( m_outPin )
    {
        if( m_outVoltEn ) m_outPin->setPinMode( output );
        m_outPin->controlPin( m_outVoltEn, m_outVoltEn );
    }

    m_useFVR  = false;
    m_usePinP = false;
    m_vRefP = 0;
    uint8_t dacPss = m_DACPSS.getRegBitsVal();
    switch( dacPss ) {
        case 0: m_vRefP = m_mcu->vdd(); break; // VDD
        case 1: m_usePinP = true;       break; // VREF+
        case 2: m_useFVR  = true;       break; // FVR Buffer2 output
        case 3:                         break; // Reserved, do not use
    }
    m_usePinN = false;
    m_vRefN = 0;
    uint8_t dacNss = m_DACNSS.getRegBitsVal();
    switch( dacNss ) {
        case 0:                    break; // VSS
        case 1: m_usePinN = true;  break; // VREF-
    }
    if( m_pRefPin ) m_pRefPin->changeCallBack( this, m_usePinP );
    if( m_nRefPin ) m_nRefPin->changeCallBack( this, m_usePinN );

    voltChanged();
}

void PicDac::outRegChanged() // DACON1 is written
{
    m_outVal = m_DACR.getRegBitsVal();
    updtOutVolt();
}

void PicDac::updtOutVolt()
{
    if( m_useFVR ) m_vRefP = m_fvr->getDacVref();

    if( m_enabled ) m_outVolt = ( m_vRefP-m_vRefN )*(double)m_outVal/32 + m_vRefN;
    else            m_outVolt = m_daclps ? m_vRefP : m_vRefN;

    if( m_outPin && m_outVoltEn ) m_outPin->setVoltage( m_outVolt );
}

void PicDac::callBack() // Called by FVR
{
    updtOutVolt();
}
