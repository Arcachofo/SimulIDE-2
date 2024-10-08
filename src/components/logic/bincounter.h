/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef BINCOUNTER_H
#define BINCOUNTER_H

#include "logiccomponent.h"

class BinCounter : public LogicComponent
{
    public:
        BinCounter( QString type, QString id );
        ~BinCounter();

 static listItem_t libraryItem();

        virtual bool setPropStr( QString prop, QString val ) override;

        bool pinSet() { return m_pinSet; }
        void useSetPin( bool set );

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override { IoComponent::runOutputs(); }

        int maxVal() { return m_TopValue; }
        void setMaxVal( int v ) { m_TopValue = v; }

    protected:
        int m_Counter;
        int m_TopValue;

        bool m_pinSet;

        IoPin* m_setPin;
        IoPin* m_rstPin;
};

#endif
