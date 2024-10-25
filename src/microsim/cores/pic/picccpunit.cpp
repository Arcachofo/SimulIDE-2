/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "picccpunit.h"
#include "picicunit.h"
#include "picocunit.h"
#include "datautils.h"
#include "mcuram.h"
#include "mcupin.h"
#include "e_mcu.h"
#include "simulator.h"

PicCcpUnit::PicCcpUnit( eMcu* mcu, QString name, int type )
          : McuModule( mcu, name )
          , eElement( name )
{
    QString n = name.right(1); // name="CCP1" => n="1"
    QString e = name.contains("+") ? "+" : "";

    m_capUnit = new PicIcUnit( m_mcu, "IC"+n );
    m_comUnit = new PicOcUnit( m_mcu, "OC"+e+n );
    m_pwmUnit = PicOcUnit::createPwmUnit( m_mcu, "PWM"+e+n, type  );
}
PicCcpUnit::~PicCcpUnit()
{
    delete m_capUnit;
}

void PicCcpUnit::setup()
{
    McuTimer* timer1 = m_mcu->getTimer( "TIMER1" );
    McuTimer* timer2 = m_mcu->getTimer( "TIMER2" );

    m_capUnit->m_timer = timer1;

    m_comUnit->m_timer = timer1;
    if( timer1 ) timer1->addOcUnit( m_comUnit );

    m_pwmUnit->m_timer = timer2;
    if( timer2 ) timer2->addOcUnit( m_pwmUnit );

    QString n = m_name.right(1);
    m_CCPxM = m_mcuRam->getRegBits("CCP"+n+"M0,CCP"+n+"M1,CCP"+n+"M2,CCP"+n+"M3");

    m_CCPxCON = m_cfgRegA->getData08();

    m_mode = 0;
}

void PicCcpUnit::initialize()
{
    m_mode = 0;
    m_lastRegH = 0;
    m_ccpMode = ccpOFF;
}

void PicCcpUnit::ccprWriteL()
{
    m_pwmUnit->ocrWriteL();
    m_comUnit->ocrWriteL();
}

void PicCcpUnit::ccprWriteH()
{
    if( m_ccpMode == ccpPWM )
    {
        *m_ccpRegH = m_lastRegH; // Read only
    }else{
        m_lastRegH = *m_ccpRegH;
        m_comUnit->ocrWriteH();
    }
}

void PicCcpUnit::configureA() // CCPxCON
{
    if( m_mode == *m_CCPxCON ) return;
    m_mode = *m_CCPxCON;

    uint8_t CCPxM = m_CCPxM.getRegBitsVal();

    m_ccpMode = ccpOFF;
    m_capUnit->initialize();
    m_comUnit->initialize();
    m_pwmUnit->initialize();

    if     ( CCPxM == 0 ) return;
    if     ( CCPxM < 4  ) { m_ccpMode = ccpCOM; m_comUnit->configure( CCPxM ); } // Compare Mode (Enhanced):
    else if( CCPxM < 8  ) { m_ccpMode = ccpCAP; m_capUnit->configure( CCPxM ); } // Capture Mode:
    else if( CCPxM < 12 ) { m_ccpMode = ccpCOM; m_comUnit->configure( CCPxM ); } // Compare Mode:
    else                  { m_ccpMode = ccpPWM; m_pwmUnit->configure( *m_CCPxCON );}// PWM Mode
}

void PicCcpUnit::setInterrupt( Interrupt* i )
{
    m_capUnit->m_interrupt = i;
    m_comUnit->m_interrupt = i;
    m_pwmUnit->m_interrupt = i;
}

void PicCcpUnit::setPin( McuPin* pin )
{
    m_capUnit->m_icPin = pin;
    m_comUnit->m_ocPin = pin;
    m_pwmUnit->m_ocPin = pin;
}
