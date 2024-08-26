/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef WATCHABLE_H
#define WATCHABLE_H

#include <QString>

class Watchable
{
public:
    Watchable(){;}

    virtual int getCpuReg( QString ){ return -1;}
    virtual QString getStrReg( QString ){ return "";}

    virtual void command( QString ){;}
};

#endif
