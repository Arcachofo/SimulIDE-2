/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "pic14einterrupt.h"
#include "e_mcu.h"
#include "datautils.h"
#include "mcuram.h"

Pic14eInterrupt::Pic14eInterrupt( QString name, uint16_t vector, eMcu* mcu )
               : PicInterrupt( name, vector, mcu )
{
    McuRam* mcuRam = (McuRam*)m_mcu->getModule("ram");

    m_wReg   = (uint8_t*) mcuRam->getReg("WREG");
    m_status = (uint8_t*) mcuRam->getReg("STATUS");
    m_bsr    = (uint8_t*) mcuRam->getReg("BSR");
    m_pclath = (uint8_t*) mcuRam->getReg("PCLATH");
}
Pic14eInterrupt::~Pic14eInterrupt(){}

void Pic14eInterrupt::execute() // Save context
{
    m_wRegSaved   = *m_wReg;
    m_statusSaved = *m_status;
    m_bsrSaved    = *m_bsr;
    m_pclathSaved = *m_pclath;

    Interrupt::execute();
}

void Pic14eInterrupt::exitInt() // Restore context
{
    *m_wReg   = m_wRegSaved;
    *m_status = m_statusSaved;
    *m_bsr    = m_bsrSaved;
    *m_pclath = m_pclathSaved;

    Interrupt::exitInt();
}

