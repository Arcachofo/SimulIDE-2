/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef DATAUTILS_H
#define DATAUTILS_H

/*#include <QStringList>

#include "mcutypes.h"

class McuRam;

uint32_t getBitMask( QStringList bitList, McuRam* mcu ); // Get mask for a group of bits in a Register

regBits_t getRegBits( QString bitNames, McuRam* mcu ); // Get a set of consecutive bits in a Register

inline uint32_t overrideBits( uint32_t val, regBits_t bits ) // Replace bits in val with current value in register bits.reg
{
    return (val & ~bits.mask) | (*(bits.reg) | bits.mask);
}

inline void replaceBits( uint32_t val, regBits_t bits ) // Replace bits in register with value
{
    *bits.reg &= ~bits.mask;
    *bits.reg |= val & bits.mask;
}

inline uint32_t getRegBits( uint32_t val, regBits_t rb )
{
    return (val & rb.mask);
}*/

/*inline uint32_t getRegBitsVal( regBits_t rb )
{
    return (*rb.reg & rb.mask)>>rb.bit0;
}

inline uint32_t getRegBitsVal( uint32_t val, regBits_t rb )
{
    return (val & rb.mask)>>rb.bit0;
}

inline uint32_t getRegBitsBool( regBits_t rb )
{
    return (*rb.reg & rb.mask) > 0;
}*/

/*inline uint32_t getRegBitsBool( uint32_t val, regBits_t rb )
{
    return (val & rb.mask) > 0;
}

inline void setRegBits( regBits_t bits )
{
    *(bits.reg) |= bits.mask;
}

inline void clearRegBits( regBits_t bits ) // Clear bits in a Register
{
    *(bits.reg) &= ~bits.mask;
}

inline void writeRegBits( regBits_t bits, bool value ) // Set/Clear bits in a Register
{
    if( value ) setRegBits( bits );
    else        clearRegBits( bits );
}

inline void setRegBitsMask( regBits_t bits, uint32_t mask ) // Set bit mask in a Register
{
    *(bits.reg) |= (bits.mask & mask);
}

inline void clearRegBitsMask( regBits_t bits, uint32_t mask ) // Clear bit mask in a Register
{
    *(bits.reg) &= ~(bits.mask & mask);
}

void writeBitsToReg( regBits_t bits, bool value, McuRam* mcu ); // Write bits in a Register calling watchers
*/
#endif
