/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "mcuuart.h"
#include "serialmon.h"
#include "usartrx.h"
#include "usarttx.h"
#include "e_mcu.h"
#include "mcuinterrupts.h"
#include "datautils.h"

McuUsart::McuUsart( eMcu* mcu, QString name, int number )
        : McuModule( mcu, name )
        , UsartModule( mcu, mcu->getId()+"-"+name )
{
    m_number = number;
}
McuUsart::~McuUsart( ){}

void McuUsart::bufferEmpty()
{
    if( m_interrupt ) m_interrupt->raise(); // USART Data Register Empty Interrupt
}

void McuUsart::frameSent( uint8_t data )
{
    if( m_monitor ) m_monitor->printOut( data );
    m_sender->raiseInt();
}

uint32_t McuUsart::getData() // Reading Data register
{
    return m_receiver->getData();
}

uint8_t McuUsart::getBit9Tx()
{
    return m_bit9Tx.getRegBitsVal();
}

void McuUsart::setBit9Rx( uint8_t bit )
{
    if( bit ) m_bit9Rx.set_08();
    else      m_bit9Rx.clear_08();
}
