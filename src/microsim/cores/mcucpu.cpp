/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "mcucpu.h"
#include "mcuram.h"

McuCpu::McuCpu( eMcu* mcu )
      : CpuBase( mcu )
{
    m_dataMem    = m_mcuRam->rawData();
    m_dataMemEnd = m_mcuRam->size();
    if( m_dataMemEnd ) m_dataMemEnd--;

    m_progMem    = m_mcuPgm->rawData();
    m_progSize   = m_mcuPgm->size();

    m_regStart = m_mcuRam->regStart();

    if( m_regStart > 0 ) m_lowDataMemEnd = m_regStart-1;
    else                 m_lowDataMemEnd = 0;

    m_regEnd = m_mcuRam->regEnd();
    if( m_dataMemEnd > 0 )
    {
        uint16_t sregAddr = m_mcuRam->sregAddr();
        if( sregAddr ) m_STATUS = &m_dataMem[sregAddr];
    }
    if     ( m_progSize <= 0xFF )     m_progAddrSize = 1;
    else if( m_progSize <= 0xFFFF )   m_progAddrSize = 2;
    else if( m_progSize <= 0xFFFFFF ) m_progAddrSize = 3;
}
McuCpu::~McuCpu() {}

void McuCpu::CALL_ADDR( uint32_t addr ) // Used by MCU Interrupts:: All MCUs should use or override this
{
    PUSH_STACK( m_PC );
    setPC( addr );
    m_mcu->cyclesDone = m_retCycles;
}
