/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "avreeprom.h"
#include "datautils.h"
#include "e_mcu.h"
#include "simulator.h"
#include "mcuram.h"

AvrEeprom::AvrEeprom( eMcu* mcu, QString name )
         : McuRom( mcu, name )
{
}
AvrEeprom::~AvrEeprom(){}

void AvrEeprom::setup()
{
    m_EECR  = (uint8_t*) m_mcuRam->getReg("EECR");
    m_EEPM  = m_mcuRam->getRegBits("EEPM0, EEPM1");
    m_EEMPE = m_mcuRam->getRegBits("EEMPE" );
    m_EEPE  = m_mcuRam->getRegBits("EEPE" );
    m_EERE  = m_mcuRam->getRegBits("EERE" );
}

void AvrEeprom::initialize()
{
    m_mode = 0;
    m_oldEempe = 0;
    m_oldEepe = 0;
    McuRom::initialize();
}

void AvrEeprom::runEvent() // Write cycle end reached
{
    bool eempe = m_EEMPE.getRegBitsBool();

    if( eempe ) m_EEMPE.clear_08(); // No read operation took place: clear EEMPE
    else        m_EEPE.clear_08();  // Read operation took place: clear EEPE
}

void AvrEeprom::configureA() // EECR is being written
{
    bool eempe = m_EEMPE.getRegBitsBool();
    bool eepe  = m_EEPE.getRegBitsBool();

    if( !eepe )
    {
        m_mode = m_EEPM.getRegBitsVal(); // EEPROM Programming Mode Bits

        if( m_EERE.getRegBitsBool() ) // Read triggered
        {
            m_EERE.clear_08(); // Clear EERE: it happens after 4 cycles, but cpu is halted for these cycles
            m_mcu->cyclesDone += 4;
            readEeprom(); // Should we return here?
        }
    }
    if( eempe )
    {
        if( !m_oldEepe && eepe ) // Write triggered
        {
            m_EEMPE.clear_08();                      // Clear EEMPE: it happens after 4 cycles but we need to cancel it now
            Simulator::self()->cancelEvents( this ); // Cancel EEMPE clear event
            m_mcu->cyclesDone += 2;
            writeEeprom();
            return;
        }
        if( !m_oldEempe ) // Shedule EEMPE clear: 4 cycles
            Simulator::self()->addEvent( m_mcu->psInst()*4, this );
    }
    m_oldEempe = eempe;
    m_oldEepe  = eepe;
}

void AvrEeprom::writeEeprom()
{
    uint8_t data = *m_dataReg;
    uint64_t time;

    if( m_EEPM.mask ) // Has EEPM bits: atmega328 and friends
    {
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
    }
    else            // No EEPM bits: atmega8 8.5 ms
    {
        time = 8500*1e6; // picoseconds
    }
    setValue( m_address, data );
    Simulator::self()->addEvent( time, this ); // Shedule Write cycle end
}

