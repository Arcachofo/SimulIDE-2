/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "datautils.h"
#include "mcuram.h"

uint32_t getBitMask( QStringList bitList, McuRam* ram ) // Get mask for a group of bits in a Register
{
    uint32_t bitMask = 0;
    for( QString bitName : bitList ) bitMask |= ram->bitMasks()->value( bitName.remove(" ") );
    return bitMask;
}

regBits_t getRegBits( QString bitNames, McuRam* ram ) // Get a set of bits in a Register
{
    regBits_t regBits;
    QStringList bitList = bitNames.split(",");

    uint32_t mask = getBitMask( bitList, ram );
    regBits.mask = mask;

    for( regBits.bit0=0; regBits.bit0<8; ++regBits.bit0 ) // Rotate mask to get initial bit
    {
        if( mask & 1 ) break;
        mask >>= 1;
    }
    regBits.regAddr = ram->bitRegs()->value( bitList.first() );
    uint32_t* ramData = ram->rawData();
    regBits.reg = ramData + regBits.regAddr;

    return regBits;
}

void writeBitsToReg( regBits_t bits, bool value, McuRam* ram ) // Set/Clear bits in a Register
{
    if( value ) ram->writeReg( bits.regAddr, *bits.reg |  bits.mask);
    else        ram->writeReg( bits.regAddr, *bits.reg & ~bits.mask);
}
