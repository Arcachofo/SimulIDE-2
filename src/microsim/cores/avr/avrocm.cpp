/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "avrocm.h"
#include "mcupin.h"
#include "e_mcu.h"
#include "mcuram.h"

AvrOcm::AvrOcm( eMcu* mcu, QString name )
      : McuOcm( mcu, name )
{
}
AvrOcm::~AvrOcm(){}

void AvrOcm::setup()
{
    m_PD2.bit0 = 2;
    m_PD2.mask = 0b100;
    m_PD2.reg = m_mcuRam->getRegByName("PORTD"); /// FIXME
}

void AvrOcm::configureA()
{
    m_mode = m_PD2.getRegBitsBool();
}

void AvrOcm::OutputOcm() //Set Ocm output from OCnB1 & OCnB2
{
    if( m_mode ) m_oPin->scheduleState( m_state1 || m_state2, 0 );
    else         m_oPin->scheduleState( m_state1 && m_state2, 0 );
}
