/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "mcuregister.h"

McuRegister::McuRegister( QString name, uint32_t addr, uint32_t mask, uint32_t rstVal, QString bits )
{
    m_name    = name;
    m_address = addr;
    m_bitMask = mask;
    m_rstVal  = rstVal;
    m_bitNames = bits;

    m_returnSlot = nullptr;
    m_readSlot   = nullptr;
    m_writeSlot  = nullptr;
    m_data8 = nullptr;
}
McuRegister::~McuRegister(){}

void McuRegister::setData( uint8_t* data )
{
    m_data8 = data;
    m_data32 = (uint32_t*)data;
}

uint8_t McuRegister::read_08( uint32_t addr )
{
    callSlots( m_readSlot );    // Slots can modify value (previous regOverride)

    if( m_returnSlot ) return m_returnSlot->read();

    uint32_t offset = addr - m_address;
    uint8_t val = m_data8[offset];

    return val;
}

uint16_t McuRegister::read_16( uint32_t addr )
{
    callSlots( m_readSlot );

    if( m_returnSlot ) return m_returnSlot->read();

    uint32_t offset = addr - m_address;
    uint16_t val = *((uint16_t*) m_data8+offset);

    return val;
}

uint32_t McuRegister::read_32()
{
    callSlots( m_readSlot );

    if( m_returnSlot ) return m_returnSlot->read();

    uint32_t val = *m_data32;

    return val;
}

void McuRegister::write_08( uint32_t addr, uint32_t val )
{
    uint32_t offset = addr - m_address;
    m_data8[offset] = val;

    callSlots( m_writeSlot );   // Slots must read value from memory pointer
}

void McuRegister::write_16( uint32_t addr, uint32_t val )
{
    uint32_t offset = addr - m_address;
    uint16_t* data = (uint16_t*)m_data8+offset;
    *data = val;

    callSlots( m_writeSlot );
}

void McuRegister::write_32( uint32_t val )
{
    *m_data32 = val;

    callSlots( m_writeSlot );
}

