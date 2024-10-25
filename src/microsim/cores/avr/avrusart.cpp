/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "avrusart.h"
#include "usarttx.h"
#include "usartrx.h"
#include "e_mcu.h"
#include "iopin.h"
#include "serialmon.h"
#include "mcuram.h"
#include "regwatcher.h"

AvrUsart::AvrUsart( eMcu* mcu,  QString name, int number )
        : McuUsart( mcu, name, number )
{
}
AvrUsart::~AvrUsart(){}

void AvrUsart::setup()
{
    QString n = m_name.right(1);
    bool ok = false;
    n.toInt( &ok );
    if( !ok ) n = "";
    m_UCSRnA = (uint8_t*) m_mcuRam->getReg("UCSR"+n+"A");
    m_UCSRnC = (uint8_t*) m_mcuRam->getReg("UCSR"+n+"C");
    m_u2xn   = m_mcuRam->getRegBits("U2X"+n);

    m_bit9Tx = m_mcuRam->getRegBits("TXB8"+n);
    m_bit9Rx = m_mcuRam->getRegBits("RXB8"+n );

    m_txEn = m_mcuRam->getRegBits("TXEN"+n );
    m_rxEn = m_mcuRam->getRegBits("RXEN"+n );

    if( n == "" ) m_modeRB = m_mcuRam->getRegBits("UMSEL"); // atmega8
    else          m_modeRB = m_mcuRam->getRegBits("UMSEL"+n+"0,UMSEL"+n+"1");

    m_pariRB = m_mcuRam->getRegBits("UPM"+n+"0,UPM"+n+"1");
    m_stopRB = m_mcuRam->getRegBits("USBS"+n );
    m_UCSZ01 = m_mcuRam->getRegBits("UCSZ"+n+"0,UCSZ"+n+"1");
    m_UCSZ2  = m_mcuRam->getRegBits("UCSZ"+n+"2" );

    m_UBRRnL = (uint8_t*) m_mcuRam->getReg("UBRR"+n+"L");
    m_mcuRam->watchRegName("UBRR"+n+"L", R_WRITE, this, &AvrUsart::setUBRRnL );

    if( n == "" ) m_UBRRnH = nullptr; // atmega8
    else{         m_UBRRnH = (uint8_t*) m_mcuRam->getReg( "UBRR"+n+"H" );
        m_mcuRam->watchRegName("UBRR"+n+"H", R_WRITE, this, &AvrUsart::setUBRRnH );
    }
    m_UDRIE = m_mcuRam->getRegBits("UDRIE"+n );
    m_UDRE  = m_mcuRam->getRegBits("UDRE"+n );
    m_TXC   = m_mcuRam->getRegBits("TXC"+n );
    m_RXC   = m_mcuRam->getRegBits("RXC"+n );
    m_FE    = m_mcuRam->getRegBits("FE"+n );
    m_DOR   = m_mcuRam->getRegBits("DOR"+n );
    m_MPCM  = m_mcuRam->getRegBits("MPCM"+n );

    if( n == "" ) m_UPE = m_mcuRam->getRegBits( "PE" );
    else          m_UPE = m_mcuRam->getRegBits( "UPE"+n );
}

void AvrUsart::configureA() // UCSRnA
{
    bool mpcm = m_MPCM.getRegBitsBool();
    m_receiver->ignoreData( mpcm );

    bool speedx2 = m_u2xn.getRegBitsBool(); // Double Speed?
    if( speedx2 == m_speedx2 ) return;
    m_speedx2 = speedx2;
    setBaurrate();
}

void AvrUsart::configureB() // UCSRnB changed
{
    m_ucsz2 =  m_UCSZ2.getRegBitsVal() <<2;
    m_dataBits = m_ucsz01+m_ucsz2+5;

    uint8_t txEn = m_txEn.getRegBitsVal();
    if( txEn != m_sender->isEnabled() )
    {
        if( txEn ){
            m_sender->getPin()->controlPin( true, true );
            m_sender->getPin()->setPinMode( output );
        }
        else m_sender->getPin()->controlPin( false, false );
        m_sender->enable( txEn );
    }

    uint8_t rxEn = m_rxEn.getRegBitsVal();
    if( rxEn != m_receiver->isEnabled() )
    {
        if( rxEn )
        {
            m_receiver->getPin()->controlPin( true, true );
            m_receiver->getPin()->setPinMode( input );
        }
        else m_receiver->getPin()->controlPin( false, false );
        m_receiver->enable( rxEn );
    }
    if( m_UDRIE.getRegBitsBool() ) // Buffer empty Interrupt enabled?
    {
        if( m_UDRE.getRegBitsBool() )  // Buffer is empty?
            bufferEmpty();             // Trigger Buffer empty Interrupt
    }
}

void AvrUsart::configureC() // UCSRnC changed
{
    if( !m_UBRRnH && !(*m_UCSRnC & (1<<7)) ) // atmega8 Writting to UBBRH
    {
        *m_UBRRnH = *m_UCSRnC & 0x0F;

        return;
    }
    // clockPol = getRegBitsVal( val, UCPOLn );

    m_mode     = m_modeRB.getRegBitsVal();    // UMSELn1, UMSELn0
    m_stopBits = m_stopRB.getRegBitsVal()+1;  // UPMn1, UPMno
    m_ucsz01   = m_UCSZ01.getRegBitsVal();
    m_dataBits = m_ucsz01+m_ucsz2+5;

    uint8_t par = m_pariRB.getRegBitsVal();
    if( par > 0 ) m_parity = (parity_t)(par-1);
    else          m_parity = parNONE;

    /*if( sm0 )  // modes 2 and 3
    {
        if( !sm0 ) // Mode 2
        {
            /// setPeriod(  m_mcu->psInst() );// Fixed baudrate 32 or 64
        }
    }*/
}

void AvrUsart::setUBRRnL()
{
    //if( *m_UBRRnL == v ) return;
    //*m_UBRRnL = v;
    setBaurrate();
}

void AvrUsart::setUBRRnH()
{
    //if( m_UBRRHval == v ) return;
    //m_UBRRHval = v;
    setBaurrate();
}

void AvrUsart::setBaurrate( uint8_t )
{
    uint16_t ubrr = *m_UBRRnL | (m_UBRRHval & 0x0F)<<8 ;

    uint64_t period = 16*(ubrr+1)*m_mcu->psInst();
    if( m_speedx2 ) period /= 2;

    setPeriod( period );
}

void AvrUsart::dataRegChanged() // Buffer is being written
{
    if( !m_sender->isEnabled() ) return;

    if( m_UDRE.getRegBitsBool() )  // Buffer is empty?
    {
        m_interrupt->clearFlag();//clearRegBits( m_UDRE ); // Transmit buffer now full: Clear UDREn bit
        m_sender->processData( *m_txReg );
}   }

void AvrUsart::frameSent( uint8_t data )
{
    if( m_monitor ) m_monitor->printOut( data );

    if( m_UDRE.getRegBitsBool()  ) // Frame sent & Buffer is empty
        m_sender->raiseInt();                 // Raise USART Transmit Complete
    else
        m_sender->startTransmission();        // Buffer contains data, send it
}

void AvrUsart::setRxFlags( uint16_t frame )
{
    if( m_dataBits == 9 ) setBit9Rx( ( frame & (1<<8) ) ? 1 : 0 );

    m_FE.setBits_08( frame & frameError );   // frameError
    m_DOR.setBits_08( frame & dataOverrun ); // overrun error
    m_UPE.setBits_08( frame & parityError ); // parityError
}
