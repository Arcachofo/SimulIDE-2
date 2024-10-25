/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "pic14core.h"
#include "datautils.h"
#include "regwatcher.h"
#include "mcuregister.h"

Pic14Core::Pic14Core( eMcu* mcu )
         : PicMrCore( mcu )
{
    m_stackSize = 8;

    m_Wreg = &m_WregHidden;

    McuRegister* fsr = m_mcuRam->getRegByName("FSR");
    if( fsr ) m_FSR = fsr->getData08();

    /// Option used only for 1 instruction
    m_OPTION = (uint8_t*) m_mcuRam->getReg("OPTION");

    m_bankBits = m_mcuRam->getRegBits("R0,R1");
    m_mcuRam->watchBitNames( "R0,R1", R_WRITE, this, &Pic14Core::bankChanged );
}
Pic14Core::~Pic14Core() {}

