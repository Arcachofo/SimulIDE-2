/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef SCRIPTPORT_H
#define SCRIPTPORT_H

#include "scriptbase.h"
#include "mcuport.h"

class ScriptPort : public ScriptBase, public McuPort
{
    public:
        ScriptPort( eMcu* mcu, QString name );
        ~ScriptPort();

        virtual void configureA() override;
        virtual void configureB() override;
        virtual void configureC() override;

        virtual void reset() override;

        virtual void setScript( QString script ) override;

        void setExtIntTrig( int pinNumber, int trig );

    protected:


};
#endif
