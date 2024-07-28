/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef FLIPFLOPJK_H
#define FLIPFLOPJK_H

#include "flipflopbase.h"

class FlipFlopJK : public FlipFlopBase
{
    public:
        FlipFlopJK( QString type, QString id );
        ~FlipFlopJK();

 static listItem_t libraryItem();

    protected:
        void calcOutput();
};

#endif
