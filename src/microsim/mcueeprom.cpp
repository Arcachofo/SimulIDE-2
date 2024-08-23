/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "mcueeprom.h"
#include "simulator.h"
#include "e_mcu.h"

McuEeprom::McuEeprom( eMcu* mcu, QString name )
         : McuModule( mcu, name )
         , eElement( mcu->getId()+"-"+name )
{
    m_addressL = nullptr;
    m_addressH = nullptr;
    m_dataReg  = nullptr;
}

McuEeprom::~McuEeprom(){}

void McuEeprom::initialize()
{
    m_address = 0;
}

void McuEeprom::reset()
{
    if( !m_saveEepr ) m_data.fill(-1);
}

void McuEeprom::readEeprom()
{
    *m_dataReg = m_data[m_address];
}

void McuEeprom::writeEeprom()
{
    m_data[m_address] = *m_dataReg;
}

void McuEeprom::addrWriteL( uint8_t val )
{
    m_address = val;
    if( m_addressH ) m_address += *m_addressH << 8;
}

void McuEeprom::addrWriteH( uint8_t val )
{
    m_address = (val << 8) + *m_addressL;
}

void McuEeprom::setData( QVector<int>* eep )
{
    int size = m_size;
    if( eep->size() < size ) size = eep->size();

    for( int i=0; i<size; ++i ) m_data[i] = eep->at(i);
}


