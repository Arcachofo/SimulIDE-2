/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "mcuregblock.h"
#include "mcuregister.h"

McuRegBlock::McuRegBlock( uint32_t addrStart, uint32_t addrEnd )
           : McuMemBlock( addrStart, addrEnd )
{
    m_registers.resize( addrEnd-addrStart+1 );
}
McuRegBlock::~McuRegBlock(){}

void McuRegBlock::addRegister( McuRegister* r )
{
    uint32_t addr = r->m_address; // Local address
    if( addr < Memory::size() )
    {
        m_registers.at( addr ) = r;
        r->m_data8 = &m_data.at( addr );
    }
}

uint8_t McuRegBlock::read_08( uint32_t addr )
{
    addr = mappedAddr( addr );
    return m_registers[addr]->read_08( addr );
}

uint16_t McuRegBlock::read_16( uint32_t addr )
{
    addr = mappedAddr( addr );
    return m_registers[addr]->read_16( addr );
}

uint32_t McuRegBlock::read_32( uint32_t addr )
{
    addr = mappedAddr( addr );
    return m_registers[addr]->read_32();
}

void McuRegBlock::write_08( uint32_t addr, uint32_t val )
{
    addr = mappedAddr( addr );
    m_registers[addr]->write_08( addr, val );
}

void McuRegBlock::write_16( uint32_t addr, uint32_t val )
{
    addr = mappedAddr( addr );
    m_registers[addr]->write_16( addr, val );
}

void McuRegBlock::write_32( uint32_t addr, uint32_t val )
{
    addr = mappedAddr( addr );
    m_registers[addr]->write_32( val );
}

McuRegister* McuRegBlock::getRegister( QString r )
{
    for( McuRegister* reg : m_registers )
        if( reg && reg->m_name == r ) return reg;

    return nullptr;
}
