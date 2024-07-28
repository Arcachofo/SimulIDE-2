/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef ZENER_H
#define ZENER_H

#include "diode.h"

class Zener : public Diode
{
    public:
        Zener( QString type, QString id );
        ~Zener();

 static listItem_t libraryItem();
};

#endif
