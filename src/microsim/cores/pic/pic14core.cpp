/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "pic14core.h"
#include "datautils.h"
#include "regwatcher.h"

Pic14Core::Pic14Core( eMcu* mcu )
         : PicMrCore( mcu )
{
    m_stackSize = 8;

    m_Wreg = &m_WregHidden;

    m_FSR    = (uint8_t*) m_mcuRam->getReg("FSR");
    m_OPTION = (uint8_t*) m_mcuRam->getReg("OPTION");

    m_bankBits = getRegBits( "R0,R1", m_mcuRam );
    watchBitNames( "R0,R1", R_WRITE, this, &Pic14Core::setBank, m_mcuRam );
}
Pic14Core::~Pic14Core() {}

