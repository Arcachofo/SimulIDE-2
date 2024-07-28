/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MAGNITUDECOMP_H
#define MAGNITUDECOMP_H

#include "iocomponent.h"
#include "e-element.h"

class MagnitudeComp : public IoComponent, public eElement
{
    public:
        MagnitudeComp( QString type, QString id );
        ~MagnitudeComp();

 static listItem_t libraryItem();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override { IoComponent::runOutputs(); }

        int bits() { return m_bits; }
        void setBits( int b );

    private:

        int m_bits;
};

#endif
