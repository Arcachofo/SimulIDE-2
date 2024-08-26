/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef COREBASE_H
#define COREBASE_H

#include <QStringList>
#include <QMap>

#include "watchable.h"

class Display;

class CoreBase : public Watchable
{
    public:
        CoreBase();
        virtual ~CoreBase();

        virtual void reset(){;}
        virtual void runStep(){;}
        virtual void extClock( bool clkState ){;}
        virtual void updateStep(){;}

        virtual int getCpuReg( QString reg ) override;

        virtual QStringList getEnumUids( QString ) { return m_enumUids;}    // For enum properties
        virtual QStringList getEnumNames( QString ) { return m_enumNames; } // For enum properties

    protected:
        QMap<QString, uint8_t*> m_cpuRegs;

        QStringList m_enumUids;  // For enum properties
        QStringList m_enumNames; // For enum properties

        Display* m_display; // Find a place for this
};

#endif
