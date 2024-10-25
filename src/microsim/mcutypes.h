/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUTYPES_H
#define MCUTYPES_H

#include <inttypes.h>
//#include <QString>

//#include "mcusignal.h"
#include "mcuregister.h"

struct regBits_t{
    uint32_t bit0 = 0;
    uint32_t mask = 0;
    McuRegister* reg = nullptr;

    uint32_t getRegBitsVal()  { return (reg->read() & mask)>>bit0; }
    bool     getRegBitsBool() { return (reg->read() & mask) > 0;}

    void clear_08() { *(reg->getData08()) &= ~mask; }
    void set_08()   { *(reg->getData08()) |= mask; }

    void write_08( uint8_t v ){
        uint8_t* r = reg->getData08();
        v <<= bit0;
        *r &= ~mask;
        *r |= (v & ~mask);
    }
    void setBits_08( bool set )
    {
        if( set ) set_08();
        else      clear_08();
    }
};

#endif
