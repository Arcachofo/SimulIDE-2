/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "mcummu.h"
//#include "mcumemblock.h"

McuMmu::McuMmu( eMcu* mcu, QString name )
      : McuModule( mcu, name )
{

}
McuMmu::~McuMmu(){}

void McuMmu::setup()
{

}

uint8_t McuMmu::read_08( uint32_t addr )
{
    return getblock( addr )->read_08( addr );
}

uint16_t McuMmu::read_16( uint32_t addr )
{
    return getblock( addr )->read_16( addr );
}

uint32_t McuMmu::read_32( uint32_t addr )
{
    return getblock( addr )->read_32( addr );
}

void McuMmu::write_08( uint32_t addr, uint32_t val )
{
    getblock( addr )->write_08( addr, val );
}

void McuMmu::write_16( uint32_t addr, uint32_t val )
{
    getblock( addr )->write_16( addr, val );
}

void McuMmu::write_32( uint32_t addr, uint32_t val )
{
    getblock( addr )->write_32( addr, val );
}

McuMemBlock* McuMmu::getblock( uint32_t addr )
{
    for( McuMemBlock* block : m_memBlocks )
        if( block->contains( addr ) ) return block;

    return &m_invalidBlock;
}

void McuMmu::addMemBlock( McuMemBlock* b, uint32_t addr )
{
    m_memBlocks.emplace_back( b );
}

