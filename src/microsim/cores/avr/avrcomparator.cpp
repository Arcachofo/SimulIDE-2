/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "avrcomparator.h"
#include "datautils.h"
#include "regwatcher.h"
#include "e_mcu.h"
#include "mcupin.h"

AvrComp::AvrComp( eMcu* mcu, QString name )
       : McuComp( mcu, name )
{
}
AvrComp::~AvrComp(){}

void AvrComp::setup()
{
    m_ACD  = m_mcuRam->getRegBits("ACD");
    m_ACBG = m_mcuRam->getRegBits("ACBG");
    m_ACO  = m_mcuRam->getRegBits("ACO" );
    m_ACOE = m_mcuRam->getRegBits("ACOE");
    m_ACI  = m_mcuRam->getRegBits("ACI" );
    m_ACIE = m_mcuRam->getRegBits("ACIE");
    m_ACIC = m_mcuRam->getRegBits("ACIC");
    m_ACIS = m_mcuRam->getRegBits("ACIS0,ACIS1");

    m_AIN0D = m_mcuRam->getRegBits("AIN0D");
    m_AIN1D = m_mcuRam->getRegBits("AIN1D");

    m_mcuRam->watchRegName("ACSR", R_READ, this, &AvrComp::readACO ); // Trigger a compare when ACO or ACI is read (ACSR)
}

void AvrComp::initialize()
{
    m_acie = false;
    m_acoe = false;
    m_compOut = false;

    m_pinP = m_pins[0];
    m_pinN = m_pins[1];
}

void AvrComp::voltChanged()
{
    compare();
}

void AvrComp::configureA() // ACSR is being written
{
    m_enabled = !m_ACD.getRegBitsBool();

    m_acie = m_ACIE.getRegBitsBool( );  // Enable interrupt
    changeCallbacks();

    m_fixVref = m_ACBG.getRegBitsVal();

    /// TODO: ACIC: Analog Comparator Input Capture Enable

    m_mode = m_ACIS.getRegBitsVal();

    if( !m_enabled ) m_ACO.clear_08(); // Clear ACO
}

void AvrComp::configureB() // AIN0D,AIN1D being written
{
    /// TODO: Disable Digital Input buffer.
    /// The corresponding PIN Register bit will always read as zero when this bit is set
}

void AvrComp::configureC() // ACOE mega328PB
{
    if( m_pins.size() < 3 ) return;

    m_acoe = m_ACOE.getRegBitsBool();

    if( m_acoe ) m_pinOut = m_pins[2];
    else         m_pinOut = nullptr;
    m_pins[2]->controlPin( m_acoe, m_acoe );

    changeCallbacks();
}

void AvrComp::readACO()
{
    if( !m_enabled ) return;
    compare();
    /// m_mcuRam->m_regOverride = *m_ACO.reg; // Clear ACO
}

void AvrComp::compare( uint8_t ) //
{
    if( !m_enabled ) return;

    double vRef = m_fixVref ? 1.1 : m_pinP->getVoltage();
    bool compOut = vRef > m_pinN->getVoltage() ;
    bool rising = !m_compOut && compOut;

    if( m_compOut != compOut )
    {
        if( compOut ) m_ACO.set_08();
        else          m_ACO.clear_08();

        switch( m_mode ){
            case 0: m_interrupt->raise();               break; // Comparator Interrupt on Output Toggle.
            case 1:                                     break; // Reserved
            case 2: if( !rising ) m_interrupt->raise(); break; // Comparator Interrupt on Falling Output Edge.
            case 3: if(  rising ) m_interrupt->raise();        // Comparator Interrupt on Rising  Output Edge.
        }
        m_compOut = compOut;
        if( m_pinOut ) m_pinOut->scheduleState( compOut, 0 );
    }
}

void AvrComp::setPinN( McuPin* pin )
{
    if( !pin ) pin = m_pins[1];

    m_pinN->changeCallBack( this, false );
    m_pinN = pin;

    changeCallbacks();
    compare();
}

void AvrComp::changeCallbacks()
{
    m_pinP->changeCallBack( this, m_enabled && (m_acie || m_acoe) );
    m_pinN->changeCallBack( this, m_enabled && (m_acie || m_acoe) );
}
