/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "datautils.h"
#include "mcuram.h"

/*uint32_t getBitMask( QStringList bitList, McuRam* ram ) // Get mask for a group of bits in a Register
{
    uint32_t bitMask = 0;
    for( QString bitName : bitList ) bitMask |= ram->bitMask( bitName.remove(" ") );
    return bitMask;
}*/

/*regBits_t getRegBits( QString bitNames, McuRam* ram ) // Get a set of bits in a Register
{
    regBits_t regBits;
    QStringList bitList = bitNames.split(",");

    regBits.mask = getBitMask( bitList, ram );
    regBits.reg = ram->getRegByBits( bitNames );

    return regBits;
}*/

/*void writeBitsToReg( regBits_t bits, bool value, McuRam* ram ) // Set/Clear bits in a Register
{
    if( value ) ram->write_08( bits.regAddr, *bits.reg |  bits.mask);
    else        ram->write_08( bits.regAddr, *bits.reg & ~bits.mask);
}*/
