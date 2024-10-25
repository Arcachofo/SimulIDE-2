/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "mcumemblock.h"

McuMemBlock::McuMemBlock( uint32_t addrStart, uint32_t addrEnd )
           : Memory()
{
    m_addrStart = addrStart;
    m_addrEnd   = addrEnd;
    //m_size = m_addrEnd - m_addrStart + 1;
    Memory::resize( m_addrEnd-m_addrStart+1 );
}
McuMemBlock::~McuMemBlock(){}
