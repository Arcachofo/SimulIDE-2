/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef BCDTO7S_H
#define BCDTO7S_H

#include "bcdbase.h"

class BcdTo7S : public BcdBase
{
    public:
        BcdTo7S( QString type, QString id );
        ~BcdTo7S();

 static listItem_t libraryItem();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override { IoComponent::runOutputs(); }
};

#endif
