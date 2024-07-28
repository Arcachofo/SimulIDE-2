/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef FLIPFLOPD_H
#define FLIPFLOPD_H

#include "flipflopbase.h"

class FlipFlopD : public FlipFlopBase
{
    public:
        FlipFlopD( QString type, QString id );
        ~FlipFlopD();

 static listItem_t libraryItem();

    protected:
        void calcOutput();
};

#endif
