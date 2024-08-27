/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "mcuram.h"
#include "e_mcu.h"
#include "datautils.h"
#include "utils.h"

McuRam::McuRam( eMcu* mcu, QString name )
      : McuModule( mcu, name )
      , Memory()
{
    m_sregAddr = 0;
    m_regStart = 0xFFFF;
    m_regEnd   = 0;
}

McuRam::~McuRam()
{
    for( uint32_t addr : m_readSignals.keys() )
        delete m_readSignals.value( addr );

    for( uint32_t addr : m_writeSignals.keys() )
        delete m_writeSignals.value( addr );

    m_readSignals.clear();
    m_writeSignals.clear();
    //m_dataMem.clear();
}

void McuRam::initialize()
{
    m_isCpuRead = true;   // RAM read is cpu read by default

    for( uint i=0; i<m_data.size(); i++ ) writeReg( i, 0, false );

    for( QString regName : m_regInfo.keys() )  // Set Registers Reset Values
    {
        regInfo_t regInfo = m_regInfo.value(regName);
        if( regInfo.resetVal != 0 )
        {
            writeReg( regInfo.address, regInfo.resetVal, false );
            m_data[regInfo.address] = regInfo.resetVal;
        }
    }
}

uint32_t McuRam::readReg( uint32_t addr )
{
    uint32_t v = m_data[addr];
    McuSignal* regSignal = m_readSignals.value( addr );
    if( regSignal )
    {
        m_regOverride = -1;
        regSignal->emitValue( v );
        if( m_regOverride >= 0 ) v = (uint32_t)m_regOverride; // Value overriden in callback
        else                     v = m_data[addr];        // Timers update their counters in callback
    }
    return v;
}

void McuRam::writeReg( uint32_t addr, uint32_t v, bool masked )
{
    uint32_t mask = 255;
    if( masked ) // Protect Read Only bits from being written
    {
        if( addr < m_regMask.size() ) mask = m_regMask[addr];
        if( mask != 0xFF && mask != 0x00 ) v = (m_data[addr] & ~mask) | (v & mask);
    }
    McuSignal* regSignal = m_writeSignals.value( addr );
    if( regSignal )
    {
        m_regOverride = -1;
        regSignal->emitValue( v );
        if( m_regOverride >= 0 ) v = (uint32_t)m_regOverride; // Value overriden in callback
    }
    if( mask != 0x00 ) m_data[addr] = v;
}

uint32_t McuRam::getRegAddress( QString reg )// Get Reg address by name
{
    uint32_t addr = 65535;
    if( m_regInfo.contains( reg ) ) addr = m_regInfo.value( reg ).address;
    else if( m_regInfo.contains( reg.toUpper() ) ) addr = m_regInfo.value( reg ).address;

    return addr;
}

uint32_t* McuRam::getReg( QString reg )      // Get pointer to Reg data by name
{
    if( !m_regInfo.contains( reg ) )
    {
        qDebug() << "Warning: Register not found:" << reg <<"\n";
        return NULL;
    }
    return &m_data[m_regInfo.value( reg ).address];
}

uint32_t McuRam::getRamValue( uint32_t address ) // Read RAM from Mcu Monitor
{
    m_isCpuRead = false;
    uint32_t value = readReg( getMapperAddr(address) );
    m_isCpuRead = true;
    return value;
}

void McuRam::setRamValue( uint32_t address, uint32_t value ) // Setting RAM from external source (McuMonitor)
{ writeReg( getMapperAddr(address), value ); }

