/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef REGWATCHER_H
#define REGWATCHER_H

#include <QDebug>

#include "mcuram.h"

template <class T>                // Add callback for Register changes by address
void watchRegister( uint16_t addr, int write
                  , T* inst, void (T::*func)(uint8_t)
                  , McuRam* ram, uint8_t mask=0xFF )
{
    if( addr == 0 ) qDebug() << "Warning: watchRegister address 0 ";

    if( write )
    {
        McuSignal* regSignal = ram->writeSignals()->value( addr );
        if( !regSignal )
        {
            regSignal = new McuSignal;
            ram->writeSignals()->insert( addr, regSignal );
        }
        regSignal->connect( inst, func, mask );
    }else{
        McuSignal* regSignal = ram->readSignals()->value( addr );
        if( !regSignal )
        {
            regSignal = new McuSignal;
            ram->readSignals()->insert( addr, regSignal );
        }
        regSignal->connect( inst, func, mask  );
    }
}

template <class T>                // Add callback for Register changes by names
void watchRegNames( QString regNames, int write
                  , T* inst, void (T::*func)(uint8_t)
                  , McuRam* ram)
{
    if( regNames.isEmpty() ) return;

    QStringList regs = regNames.split(",");
    for( QString reg : regs )
    {
        if( !ram->regInfo()->contains( reg ) )
        {
            qDebug() << "ERROR: Register not found: " << reg;
            continue;
        }
        uint16_t addr = ram->regInfo()->value( reg ).address;
        watchRegister( addr, write, inst, func, ram );
    }
}

template <class T>              // Add callback for Register bit changes by names
void watchBitNames( QString bitNames, int write
              , T* inst, void (T::*func)(uint8_t)
              , McuRam* ram )
{
    if( bitNames.isEmpty() ) return;

    uint16_t regAddr = 0;
    QStringList bitList = bitNames.split(",");
    uint8_t     bitMask = getBitMask( bitList, ram );

    regAddr = ram->bitRegs()->value( bitList.first() );

    if( regAddr )
        watchRegister( regAddr, write, inst, func, ram, bitMask );
}
#endif
