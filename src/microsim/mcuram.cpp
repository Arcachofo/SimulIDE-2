/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "mcuram.h"
#include "e_mcu.h"
#include "datautils.h"
#include "mcuregister.h"
#include "utils.h"

McuRam::McuRam( eMcu* mcu, QString name )
      : McuMmu( mcu, name )
{
    m_sregAddr = 0;
}

McuRam::~McuRam(){}

void McuRam::initialize()
{
    m_isCpuRead = true;   // RAM read is cpu read by default

    /*for( uint i=0; i<m_data.size(); i++ ) writeReg( i, 0, false );

    for( QString regName : m_regInfo.keys() )  // Set Registers Reset Values
    {
        regInfo_t regInfo = m_regInfo.value(regName);
        if( regInfo.resetVal != 0 )
        {
            writeReg( regInfo.address, regInfo.resetVal, false );
            m_data[regInfo.address] = regInfo.resetVal;
        }
    }*/
}

void McuRam::addRegBlock( McuRegBlock* b, uint32_t addr )
{
    addMemBlock( b, addr );
    m_regBlocks.emplace_back( b );
}

McuRegister* McuRam::getRegByName( QString regName )
{
    for( McuRegBlock* block : m_regBlocks )
    {
        McuRegister* reg = block->getRegister( regName ) ;
        if( reg ) return reg;
    }
    return nullptr;
}

McuRegister* McuRam::getRegByAddr( uint32_t addr )
{
    for( McuRegBlock* block : m_regBlocks )
        if( block->contains( addr ) )
            return block->getRegister( addr );

    return nullptr;
}

McuRegister* McuRam::getRegByBits( QString bits )
{
    QStringList bitList = bits.split(",");
    return m_bitRegs.value( bitList.first() );
}

/*uint32_t McuRam::getRegAddress( QString reg )// Get Reg address by name
{
    uint32_t addr = 65535;
    if( m_regInfo.contains( reg ) ) addr = m_regInfo.value( reg ).address;
    else if( m_regInfo.contains( reg.toUpper() ) ) addr = m_regInfo.value( reg ).address;

    return addr;
}*/

uint8_t* McuRam::getReg( QString reg )      // Get pointer to Reg data by name
{
    McuRegister* r = getRegByName( reg );
    if( r ) return r->getData08();

    return nullptr;
}

regBits_t McuRam::getRegBits( QString bits) // Get a set of bits in a Register
{
    regBits_t regBits;

    regBits.mask = getBitMask( bits );
    regBits.reg  = getRegByBits( bits );

    return regBits;
}

uint32_t McuRam::getBitMask( QString bits )
{
    uint32_t mask = 0;
    QStringList bitList = bits.split(",");
    for( QString bitName : bitList ) mask |= m_bitMasks.value( bitName.remove(" ") );
    return mask;
}
