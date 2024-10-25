/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef REGWATCHER_H
#define REGWATCHER_H

#include <QDebug>

#include "mcuram.h"
#include "mcuregister.h"

/*template <class T>                // Add callback for Register changes by address
void watchRegister( uint16_t addr, int write
                  , T* obj, void (T::*func)()
                  , McuRam* ram )
{
    if( addr == 0 ) qDebug() << "Warning: watchRegister address 0 ";

    McuRegister* reg = ram->getRegister( addr );
    if( !reg )
    {
        qDebug() << "Error: watchRegister, No Register at Address" << addr;
        return;
    }

    if( write ) reg->addWriteCallback( obj, func );
    else        reg->addReadCallback( obj, func );
}*/

/*template <class T>                // Add callback for Register changes by names
void watchRegNames( QString regNames, int write
                  , T* obj, void (T::*func)()
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
        watchRegister( addr, write, obj, func, ram );
    }
}*/

/*template <class T>              // Add callback for Register bit changes by names
void watchBitNames( QString bitNames, int write
              , T* obj, void (T::*func)()
              , McuRam* ram )
{
    if( bitNames.isEmpty() ) return;

    uint32_t regAddr = ram->bitRegAddr( bitNames );

    if( regAddr ) watchRegister( regAddr, write, obj, func, ram );
}*/
#endif
