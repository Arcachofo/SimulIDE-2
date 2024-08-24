/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "mcurom.h"
#include "simulator.h"
#include "e_mcu.h"

McuRom::McuRom( eMcu* mcu, QString name )
         : McuModule( mcu, name )
         , Memory()
         , eElement( mcu->getId()+"-"+name )
{
    m_addressL = nullptr;
    m_addressH = nullptr;
    m_dataReg  = nullptr;
}

McuRom::~McuRom(){}

/*void McuRom::initialize()
{
    m_address = 0;
}*/

void McuRom::reset()
{
    m_address = 0;
    if( !m_saveEepr )
        for( uint i=0; i<m_size; ++i ) m_data[i] = -1;
}

void McuRom::readEeprom()
{
    *m_dataReg = m_data[m_address];
}

void McuRom::writeEeprom()
{
    m_data[m_address] = *m_dataReg;
}

void McuRom::addrWriteL( uint8_t val )
{
    m_address = val;
    if( m_addressH ) m_address += *m_addressH << 8;
}

void McuRom::addrWriteH( uint8_t val )
{
    m_address = (val << 8) + *m_addressL;
}

