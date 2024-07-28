/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef FLIPFLOPT_H
#define FLIPFLOPT_H

#include "flipflopbase.h"

class FlipFlopT : public FlipFlopBase
{
    public:
        FlipFlopT( QString type, QString id );
        ~FlipFlopT();

 static listItem_t libraryItem();

    protected:
        void calcOutput();
};

#endif
