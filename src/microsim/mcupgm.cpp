/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "mcupgm.h"
#include "simulator.h"
#include "e_mcu.h"

McuPgm::McuPgm( eMcu* mcu, QString name )
         : McuModule( mcu, name )
         , MemData()
         , eElement( mcu->getId()+"-"+name )
{
    m_addressL = nullptr;
    m_addressH = nullptr;
    m_dataReg  = nullptr;
}

McuPgm::~McuPgm(){}

/*void McuPgm::initialize()
{
    m_address = 0;
}*/

void McuPgm::reset()
{
    m_address = 0;
    //if( !m_savePgm )
    //    for( uint i=0; i<m_size; ++i ) m_data[i] = -1;
}

void McuPgm::readPgm()
{
    *m_dataReg = m_data[m_address];
}

void McuPgm::writePgm()
{
    m_data[m_address] = *m_dataReg;
}

void McuPgm::addrWriteL( uint8_t val )
{
    m_address = val;
    if( m_addressH ) m_address += *m_addressH << 8;
}

void McuPgm::addrWriteH( uint8_t val )
{
    m_address = (val << 8) + *m_addressL;
}

