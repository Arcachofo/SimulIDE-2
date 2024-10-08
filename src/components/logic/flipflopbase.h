/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef FLIPFLOPBASE_H
#define FLIPFLOPBASE_H

#include "logiccomponent.h"

class FlipFlopBase : public LogicComponent
{
    public:
        FlipFlopBase( QString type, QString id );
        ~FlipFlopBase();

        virtual bool setPropStr( QString prop, QString val ) override;

        bool pinsRS() { return m_useRS; }
        void usePinsRS( bool rs );

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override{ IoComponent::runOutputs(); }

        bool sPinState();
        bool rPinState();

    protected:
        virtual void calcOutput(){;}

        bool m_useRS;
        bool m_Q0;

        int m_dataPins;

        IoPin* m_setPin;
        IoPin* m_rstPin;
};
#endif
