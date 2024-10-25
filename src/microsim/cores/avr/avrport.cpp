/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "avrport.h"
#include "avrpin.h"
#include "mcu.h"
#include "mcuram.h"
#include "e_mcu.h"

AvrPort::AvrPort( eMcu* mcu, QString name )
       : McuPort( mcu, name )
{
}
AvrPort::~AvrPort(){}

void AvrPort::pinRegChanged() // PIN register written
{
    uint8_t inReg = *m_inReg;
    *m_inReg = m_inpState;

    if( inReg == 0 ) return;

    *m_outReg ^= inReg;
    McuPort::outChanged(); // Toggle bits = 1
}

McuPin* AvrPort::createPin( int i, QString id , Component* mcu )
{
    return new AvrPin( this, i, id, mcu );
}
