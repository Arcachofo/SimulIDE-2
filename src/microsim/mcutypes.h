/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUTYPES_H
#define MCUTYPES_H

#include <inttypes.h>
#include <QString>

#include "mcusignal.h"

struct regInfo_t{
    uint32_t address;
    uint32_t resetVal;
    QString  bitNames;
};

struct regBits_t{
    uint32_t bit0=0;
    uint32_t mask=0;
    uint32_t* reg=0;
    uint32_t regAddr=0;
};

#endif
