/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef FLIPFLOPRS_H
#define FLIPFLOPRS_H

#include "flipflopbase.h"

class FlipFlopRS : public FlipFlopBase
{
    public:
        FlipFlopRS( QString type, QString id );
        ~FlipFlopRS();

 static listItem_t libraryItem();

        virtual void voltChanged() override;
};

#endif
