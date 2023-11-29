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

void McuUsart::readByte( uint8_t )
{
    if( m_mcu->isCpuRead() ) m_mcu->m_regOverride = m_receiver->getData();
}

uint8_t McuUsart::getBit9Tx()
{
    return getRegBitsVal( m_bit9Tx );
}

void McuUsart::setBit9Rx( uint8_t bit )
{
    writeRegBits( m_bit9Rx, bit );
}
