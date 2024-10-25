/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "i51usart.h"
#include "usarttx.h"
#include "usartrx.h"
#include "mcutimer.h"
#include "e_mcu.h"
#include "datautils.h"
#include "mcuram.h"

#define SCON *m_scon

I51Usart::I51Usart( eMcu* mcu, QString name, int number )
        : McuUsart( mcu, name, number )
{
}
I51Usart::~I51Usart(){}

void I51Usart::setup()
{
    m_receiver->setFifoSize( 1 );
    m_stopBits = 1;
    m_dataMask = 0xFF;
    m_parity   = parNONE;

    m_timer1 = m_mcu->getTimer( "TIMER1" );

    m_scon = (uint8_t*) m_mcuRam->getReg( "SCON" );

    m_SM     = m_mcuRam->getRegBits("SM1,SM0");
    m_bit9Tx = m_mcuRam->getRegBits("TB8");
    m_bit9Rx = m_mcuRam->getRegBits("RB8");
    m_SM2    = m_mcuRam->getRegBits("SM2");

    m_SMOD = m_mcuRam->getRegBits("SMOD");
}

void I51Usart::reset()
{
    m_sender->enable( true ); // Sender always enabled
    m_mode = 0xFF;
    m_smodDiv = false;
    m_smodVal = 0;
    m_counter = 0;
}

void I51Usart::configureA() //SCON
{
    uint8_t mode = m_SM.getRegBitsVal();
    bool     sm2 = m_SM2.getRegBitsBool();

    if( mode == m_mode ){
        if( mode == 2 || mode == 3 ) m_receiver->ignoreData( sm2 );
        return;
    }
    m_mode = mode;

    setSynchronous( mode == 0 );

    bool useTimer = false;

    switch( mode )
    {
        case 0:             // Synchronous 8 bit
            m_uartSync->setPeriod( m_mcu->psInst() );// Fixed baudrate 32 or 64
            m_uartSync->setClkOffset( 200*1e3 );     // 200 ns first clock edge
            sm2 = 0;
            break;
        case 1:             // Asynchronous Timer1 8 bits
            useTimer = true;
            m_dataBits = 8;
            sm2 = 0;
            break;
        case 2:             // Asynchronous MCU Clock 9 bits
            setPeriod( m_mcu->psInst() );// Fixed baudrate 32 or 64
            m_dataBits = 9;
            break;
        case 3:             // Asynchronous Timer1 9 bits
            useTimer = true;
            m_dataBits = 9;
            break;
    }
    m_receiver->ignoreData( sm2 );

    m_timer1->getInterrupt()->callBack( this, useTimer );
    if( useTimer ) setPeriod( 0 );
}

void I51Usart::configureB() // PCON
{
    m_smodVal = m_SMOD.getRegBitsVal();
}

void I51Usart::dataRegChanged() // Buffer is being written
{
    if( m_mcu->state() == mcuStopped ) return;

    if( m_synchronous ) m_uartSync->sendSyncData( *m_txReg );
    else                m_sender->processData( *m_txReg );
}

void I51Usart::callBack()
{
    if( ++m_counter != 16 ) return;

    m_counter = 0;
    if( (m_smodVal & 1) == 0 ) // half speed
    {
        m_smodDiv = !m_smodDiv;
        if( m_smodDiv ) return;
    }
    m_sender->runEvent();
    m_receiver->runEvent();
}

uint32_t I51Usart::getData() // Reading SBUF
{
    if( m_mcuRam->isCpuRead() )
    {
        m_stopBitError = false;
        uint8_t data = m_receiver->getData(); // This calls I51Usart::setRxFlags()

        if( m_mode == 1 && m_stopBitError ) return -1; /// Fixme // Ignore frame
        //m_mcuRam->m_regOverride = data;
        return data;
    }
    return -1;/// Fixme
}

void I51Usart::setRxFlags( uint16_t frame )
{
    m_stopBitError = (frame & frameError) != 0;

    //writeRegBits( m_FE , frame & frameError );  // frameError
    //writeRegBits( m_DOR, frame & dataOverrun ); // overrun error
    //writeRegBits( m_UPE, frame & parityError ); // parityError
}
