/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef CURRSOURCE_H
#define CURRSOURCE_H

#include "varsource.h"

class Pin;

class CurrSource : public VarSource
{
    public:
        CurrSource( QString type, QString id );
        ~CurrSource();

 static listItem_t libraryItem();

        virtual void stamp() override;
        virtual void updateStep() override;

    private:
        Pin* m_outPin;
};

#endif
