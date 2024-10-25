/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "avrtwi.h"
#include "mcupin.h"
#include "e_mcu.h"
#include "mcuinterrupts.h"
#include "datautils.h"
#include "mcuram.h"

AvrTwi::AvrTwi( eMcu* mcu, QString name )
      : McuTwi( mcu, name )
{

}
AvrTwi::~AvrTwi(){}

void AvrTwi::setup()
{
    QString n = m_name.right(1);
    bool ok = false;
    n.toInt( &ok );
    if( !ok ) n = "";

    //m_TWCR = (uint8_t*) m_mcuRam->getReg("TWCR"+n );
    m_TWBR = (uint8_t*) m_mcuRam->getReg("TWBR"+n);

    m_TWEN  = m_mcuRam->getRegBits("TWEN"+n  );
    m_TWWC  = m_mcuRam->getRegBits("TWWC"+n  );
    m_TWSTO = m_mcuRam->getRegBits("TWSTO"+n );
    m_TWSTA = m_mcuRam->getRegBits("TWSTA"+n );
    m_TWEA  = m_mcuRam->getRegBits("TWEA"+n  );
    m_TWINT = m_mcuRam->getRegBits("TWINT"+n );
}

void AvrTwi::initialize()
{
    McuTwi::initialize();
    m_bitRate = 0;
    m_enabled = false;
    m_start = false;
    m_stop  = false;
}

void AvrTwi::configureA() // TWCR is being written
{
    //bool oldEn  = getRegBitsBool( *m_TWCR, m_TWEN );
    bool enable = m_TWEN.getRegBitsBool();

    if( m_enabled != enable )
    {
        if( enable )   /// Enable TWI if it was disabled
        {
            m_sda->setPinMode( openCo );
            m_sda->controlPin( true, true ); // Get control of MCU PIns
            m_scl->setPinMode( openCo );
            m_scl->controlPin( true, true );
        }
        else          /// Disable TWI
        {
            setMode( TWI_OFF );
            m_sda->controlPin( false, false ); // Release control of MCU PIns
            m_scl->controlPin( false, false );
        }
        m_enabled = enable;
    }
    if( !enable ) return;                  // Not enabled, do nothing

    bool newStart = m_TWSTA.getRegBitsBool();
    if( m_start != newStart )
    {
        if( newStart && !m_start )            /// Generate Start Condition
        {
            if( m_mode != TWI_MASTER ) setMode( TWI_MASTER );
            masterStart();
        }
        m_start = newStart;
    }
    bool newStop = m_TWSTO.getRegBitsBool();
    if( m_stop != newStop )
    {
        if( newStop && !m_stop )              /// Generate Stop Condition
        {
            if( m_mode == TWI_MASTER ) // Master: Stop if I2C was started
            {/// DONE if Stop and Start at same time, then Start Condition should be sheduled
                if( !newStart && getStaus() < TWI_NO_STATE ) masterStop();//I2Cstop();
            }
            else setMode( TWI_SLAVE ); // Slave: Stop Cond restarts Slave mode (can be used to recover from an error condition)
        }
        m_stop = newStop;
    }

    bool clearTwint = m_TWINT.getRegBitsBool();
    bool twea = m_TWEA.getRegBitsBool();
    if( !newStop && !newStart && !clearTwint )
    {
        if( m_mode != TWI_SLAVE ) setMode( TWI_SLAVE );
        /// FIXME: m_enabled = twea;  // Check this
    }

    bool data = clearTwint && !newStop && !newStart; // No start or stop and TWINT cleared, receive data
    if( !data ) return;

    if( m_mode == TWI_MASTER )
    {
        if( m_twiState == TWI_MRX_ADR_ACK    // We sent Slave Address + R and received ACK
         || m_twiState == TWI_MRX_DATA_ACK ) // We sent data and received ACK
            masterRead( twea );              // Read a byte and send ACK/NACK
    }
    else if( m_mode == TWI_SLAVE )
    {
        if( m_twiState == TWI_STX_ADR_ACK )   // Own Slave Address + R received and ACK sent
            slaveWrite();                     // Start Slave transmission
    }
}

void AvrTwi::configureB()       // TWBR is being written
{
    if( m_bitRate == *m_TWBR ) return;
    m_bitRate = *m_TWBR;
    updateFreq();
}

void AvrTwi::writeAddrReg() // TWAR is being written
{
    m_genCall = *m_addrReg & 1;
    m_address = *m_addrReg >> 1;
}

void AvrTwi::writeStatus() // TWSR Status Register is being written
{
    //newTWSR &= 0b00000011;
    uint8_t prIndex = *m_statReg & 0b00000011;
    uint8_t prescaler = m_prescList[prIndex];
    if( m_prescaler != prescaler ) { m_prescaler = prescaler; updateFreq(); }
    /// Done by masking //m_mcu->m_regOverride = newTWSR | (*m_statReg & 0b11111100); // Preserve Status bits
}

void AvrTwi::writeTwiReg() // TWDR is being written
{
    uint8_t TWDR = *m_dataReg;

    if( m_mode == TWI_SLAVE ) m_txReg = TWDR;
    if( m_mode != TWI_MASTER ) return;

    bool twint = m_TWINT.getRegBitsBool(); // Check if TWINT is set
    if( twint )                  // If set clear TWWC
    {
        m_TWWC.clear_08();  // Clear Write Collision bit TWWC
    }
    else                         // If not, the access will be discarded
    {
        m_TWWC.set_08();    // set Write Collision bit TWWC
        return;
    }
    bool write = false;
    bool isAddr = (getStaus() == TWI_START
                || getStaus() == TWI_REP_START); // We just sent Start, so this must be slave address

    if( isAddr ) write = ((TWDR & 1) == 0);   // Sending address for Read or Write?

    masterWrite( TWDR, isAddr, write );       /// Write data or address to Slave
}

void AvrTwi::setTwiState( twiState_t state )     // Set new AVR Status value
{
    TwiModule::setTwiState( state );

    *m_statReg &= 0b00000111;      // Clear old state
    *m_statReg |= state;           // Write new state

    if( state == TWI_NO_STATE && m_i2cState == I2C_STOP ) // Stop Condition sent
    {
        m_TWSTO.clear_08();   // Clear TWSTO bit
    }
    else{
        m_interrupt->raise();

        if( m_mode == TWI_MASTER )
        {
            if( state == TWI_MRX_DATA_ACK || state == TWI_MRX_DATA_NACK ) // Data received
                *m_dataReg = m_rxReg; // Save data received into TWDR
        }
        else{ // Slave
            if( state == TWI_SRX_ADR_DATA_ACK || state == TWI_SRX_ADR_DATA_NACK
             || state == TWI_SRX_GEN_DATA_ACK || state == TWI_SRX_GEN_DATA_NACK )
                *m_dataReg = m_rxReg; // Save data received into TWDR
}   }   }

void AvrTwi::updateFreq()
{
    double freq = m_mcu->freq()/(16+2*m_bitRate*m_prescaler);
    setFreqKHz( freq/1e3 );
}


void AvrTwi::writeByte() // Read from Data Register
{
    if( m_mode == TWI_SLAVE ) m_txReg = *m_dataReg;
    TwiModule::writeByte();
}

void AvrTwi::I2Cstop()
{
    TwiModule::I2Cstop();
    if( m_mode != TWI_SLAVE ) return;
    if (m_addrMatch) setTwiState( TWI_SRX_STOP_RESTART );
}
