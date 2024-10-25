/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "picport.h"
#include "picpin.h"
#include "mcu.h"
#include "e_mcu.h"
#include "mcuram.h"

PicPort::PicPort( eMcu* mcu, QString name )
       : McuPort( mcu, name )
{
    m_sleepMode = 0xFF;

    m_ANSEL = m_mcuRam->getReg("ANSEL");
}
PicPort::~PicPort(){}

void PicPort::configureA() // ANSEL Analog pins
{
    for( int i=0; i<m_numPins; ++i )
        m_pins[i]->setAnalog( *m_ANSEL & 1<<i );
}

void PicPort::outChanged()
{
    McuPort::outChanged();
    if( m_interrupt ) m_interrupt->clearFlag(); // Clear interrupt fag
}

uint32_t PicPort::getInpState()
{
    if( m_interrupt ) m_interrupt->clearFlag(); // Clear interrupt fag
    return McuPort::getInpState();
}

McuPin* PicPort::createPin( int i, QString id , Component* mcu )
{
    return new PicPin( this, i, id, mcu );
}

void PicPort::puRegChanged()
{
    setPullups( *m_puReg );
}

void PicPort::puBitsChanged()
{
    uint8_t puMask = m_puBits.getRegBitsBool() ? 255 : 0;
    setPullups( puMask );
}

void PicPort::puBitsInvChanged()
{
    uint8_t puMask = m_puBits.getRegBitsBool() ? 0 : 255;
    setPullups( puMask );
}
