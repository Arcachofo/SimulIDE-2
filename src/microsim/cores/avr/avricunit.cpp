/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "avricunit.h"
#include "mcuram.h"
#include "mcupin.h"
#include "mcutimer.h"
#include "mcuinterrupts.h"

AvrIcUnit::AvrIcUnit( eMcu* mcu, QString name )
         : McuIcUnit( mcu, name )
{

}
AvrIcUnit::~AvrIcUnit( ){}

void AvrIcUnit::setup()
{
    QString n = m_name.right(1);
    m_ICES = m_mcuRam->getRegBits("ICES"+n);
    m_ICNC = m_mcuRam->getRegBits("ICNC"+n);
}

void AvrIcUnit::configureA() // ICES,ICNC
{
    m_fallingEdge = m_ICES.getRegBitsBool();
    /// TODO INCN
}
