/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "picusart.h"
#include "usarttx.h"
#include "usartrx.h"
#include "e_mcu.h"
#include "iopin.h"
#include "serialmon.h"
#include "mcuregister.h"
#include "mcuram.h"
#include "regwatcher.h"

PicUsart::PicUsart( eMcu* mcu,  QString name, int number )
        : McuUsart( mcu, name, number )
{
}
PicUsart::~PicUsart(){}

void PicUsart::setup()
{
    m_sleepMode = 0xFF;
    m_sender->setSleepMode( 0xFF );
    m_receiver->setSleepMode( 0xFF );

    m_enabled = false;
    m_SPBRG  = nullptr;
    m_SPBRGL = nullptr;
    m_SPBRGH = nullptr;

    m_PIR1  = (uint8_t*) m_mcuRam->getReg("PIR1");
    m_TXSTA = (uint8_t*) m_mcuRam->getReg("TXSTA");
    m_RCSTA = (uint8_t*) m_mcuRam->getReg("RCSTA");

    McuRegister* reg = m_mcuRam->getRegByName("SPBRG");
    if( reg ){
        m_SPBRGL = reg->getData08();
        m_mcuRam->watchRegName("SPBRG",  R_WRITE, this, &PicUsart::setBaurrate );
    }
    reg = m_mcuRam->getRegByName("SPBRGL");
    if( reg ){
        m_SPBRGL = reg->getData08();
        m_mcuRam->watchRegName("SPBRGL", R_WRITE, this, &PicUsart::setBaurrate );
    }
    reg = m_mcuRam->getRegByName("SPBRGH");
    if( reg ){
        m_SPBRGH = reg->getData08();
        m_mcuRam->watchRegName("SPBRGH", R_WRITE, this, &PicUsart::setBaurrate );
    }

    m_SPEN = m_mcuRam->getRegBits("SPEN");
    m_BRGH = m_mcuRam->getRegBits("BRGH");

    m_bit9Tx = m_mcuRam->getRegBits("TX9D");
    m_bit9Rx = m_mcuRam->getRegBits("RX9D");

    m_txEn = m_mcuRam->getRegBits("TXEN");
    m_rxEn = m_mcuRam->getRegBits("CREN");
    m_TX9  = m_mcuRam->getRegBits("TX9");
    m_RX9  = m_mcuRam->getRegBits("RX9");
    m_ADDEN = m_mcuRam->getRegBits("ADDEN");

    m_TXIF = m_mcuRam->getRegBits("TXIF");
    m_TRMT = m_mcuRam->getRegBits("TMRT");
    m_RCIF = m_mcuRam->getRegBits("RCIF");
    m_FERR = m_mcuRam->getRegBits("FERR");
    m_OERR = m_mcuRam->getRegBits("OERR");
    m_SYNC = m_mcuRam->getRegBits("SYNC");
}

void PicUsart::configureA() // TXSTA changed
{
    // clockPol = getRegBitsVal( val, CSRC );

    m_synchronous = m_SYNC.getRegBitsBool();

    bool txEn = m_txEn.getRegBitsBool();
    if( m_synchronous );
    else{
        if( txEn != m_sender->isEnabled() )
        {
            m_TXIF.set_08();
            m_TRMT.set_08();
            if( txEn ){
                m_sender->getPin()->controlPin( true, true );
                m_sender->getPin()->setPinMode( output );
            }
            else m_sender->getPin()->controlPin( false, false );
            m_sender->enable( txEn );
        }
    }
    m_dataBits = m_TX9.getRegBitsVal()+8;

    m_speedx2 = m_BRGH.getRegBitsBool(); // Double Speed?
    setBaurrate();
}

void PicUsart::configureB() // RCSTA changed
{
    m_enabled = m_SPEN.getRegBitsVal();

    bool rxEn = m_rxEn.getRegBitsVal();
    if( rxEn != m_receiver->isEnabled() ) m_receiver->enable( rxEn );

    bool adden = m_ADDEN.getRegBitsBool();
    m_receiver->ignoreData( adden );
}

void PicUsart::setBaurrate()
{
    uint64_t mult = 16;
    if( m_speedx2 ) mult = 4;
    uint16_t SPBRG = *m_SPBRGL;
    if( m_SPBRGH ) SPBRG |= (uint16_t)*m_SPBRGH << 8;
    setPeriod( mult*(SPBRG+1)*m_mcu->psInst() ); // period in picoseconds
}

void PicUsart::dataRegChanged() // Buffer is being written
{
    if( m_TXIF.getRegBitsBool() )  // TXREG is empty
    {
        m_interrupt->clearFlag();            // TXREG full: Clear TXIF bit
        m_sender->processData( *m_txReg );
}   }

void PicUsart::frameSent( uint8_t data ) // TSR is empty
{
    if( m_monitor ) m_monitor->printOut( data );

    if( m_TXIF.getRegBitsBool() )       // TXREG is empty, no data to load
        m_TRMT.clear_08();              // Clear TMRT bit
    else
        m_sender->startTransmission();  // TXREG contains data, send it
}

void PicUsart::bufferEmpty()
{
    m_interrupt->raise(); // USART Data Register Empty Interrupt
    m_TRMT.set_08(); // Set TMRT bit
}

void PicUsart::setRxFlags( uint16_t frame )
{
    if( m_dataBits == 9 ) setBit9Rx( ( frame & (1<<8) ) ? 1 : 0 );

    m_FERR.setBits_08( frame & frameError );  // frameError
    m_OERR.setBits_08( frame & dataOverrun ); // overrun error
    //setBits_08(     , frame & parityError);  // parityError
}

void PicUsart::sleep( int mode )
{
    McuModule::sleep( mode );
    if( m_sleeping ) m_sender->pauseEvents();
    else             m_sender->resumeEvents();
}
