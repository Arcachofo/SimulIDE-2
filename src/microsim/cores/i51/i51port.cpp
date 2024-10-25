/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "i51port.h"
#include "i51pin.h"
#include "mcu.h"
#include "e_mcu.h"
#include "mcuram.h"

I51Port::I51Port( eMcu* mcu, QString name )
       : McuPort( mcu, name )
{
}
I51Port::~I51Port(){}

void I51Port::reset()
{
    McuPort::reset();
    m_inpState = 255;
}

McuPin* I51Port::createPin( int i, QString id , Component* mcu )
{
    return new I51Pin( this, i, id, mcu );
}

