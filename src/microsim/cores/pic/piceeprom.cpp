/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "piceeprom.h"
#include "datautils.h"
#include "mcuram.h"
#include "e_mcu.h"
#include "simulator.h"

PicEeprom::PicEeprom( eMcu* mcu, QString name )
         : McuRom( mcu, name )
{
}
PicEeprom::~PicEeprom(){}

void PicEeprom::setup()
{
    m_EECON2 = (uint8_t*)m_mcuRam->getReg("EECON0");
    //m_EECR  = m_mcu->getReg( "EECR" );
    m_WRERR = m_mcuRam->getRegBits("WRERR");
    m_WREN  = m_mcuRam->getRegBits("WREN");
    m_WR    = m_mcuRam->getRegBits("WR");
    m_RD    = m_mcuRam->getRegBits("RD");
}

void PicEeprom::initialize()
{
    m_writeEnable = false;
    m_nextCycle = 0;
    m_wrMask = 0;
    McuRom::initialize();
}

void PicEeprom::runEvent() // Write cycle end reached
{
    writeEeprom();
    m_WR.clear_08();
    m_wrMask = 0;
}

void PicEeprom::configureA() // EECON1 (EECR ?) is being written
{
    if( m_writeEnable ) // Write enabled
    {
        m_writeEnable = false;

        if( m_mcu->cycle() == m_nextCycle ) // Must happen in next cycle
        {
            bool wren  = m_WREN.getRegBitsBool();
            bool write = wren && m_WR.getRegBitsBool();
            if( write )
            {
                m_wrMask = m_WR.mask; // Don't clear WR until write finished
                Simulator::self()->addEvent( 5e9, this ); // Write time = 5 ms
            }
        }
        m_nextCycle = 0;
    }
    else if( m_RD.getRegBitsBool() ) // Read enable
    {
        readEeprom();
    }
    m_WR.clear_08();
    //newEECON1 &= ~(m_WR.mask);                                    // Clear WR if not in write cycle
    //m_mcuRam->m_regOverride = (newEECON1 | m_wrMask) & ~(m_RD.mask); // Clear RD, set WR if in write cycle
}

void PicEeprom::configureB() // EECON2
{
    if     ( *m_EECON2 == 0x55 ) m_nextCycle = m_mcu->cycle()+2;
    else if( *m_EECON2 == 0xAA )
    {
        if( m_mcu->cycle() == m_nextCycle )
        {
            m_nextCycle = m_mcu->cycle()+1;
            m_writeEnable = true;
        }
    }
    /// FIXME: m_mcuRam->m_regOverride = 0; // Don't write value (this is not a physical register).
}

/*void PicEeprom::writeEeprom()
{
    uint8_t data = *m_dataReg;
    uint64_t time;

    switch( m_mode )
    {
        case 0:     // 3.4 ms - Erase and Write in one operation (Atomic Operation)
            time = 3400*1e6; // picoseconds
            break;
        case 1:     // 1.8 ms - Erase Only
            data = 0xFF;
            // fallthrough
        case 2:     // 1.8 ms - Write Only
            time = 1800*1e6; // picoseconds
            break;
    }
    m_mcu->setRomValue( m_address, data );
    m_mcu->cyclesDone += 2;

    Simulator::self()->addEvent( time, this ); // Shedule Write cycle end
}*/

