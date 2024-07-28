/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef DEMUX_H
#define DEMUX_H

#include "logiccomponent.h"

class Demux : public LogicComponent
{
    public:
        Demux( QString type, QString id );
        ~Demux();

 static listItem_t libraryItem();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override{ IoComponent::runOutputs(); }

        int addrBits() { return m_addrBits; }
        void setAddrBits( int bits );
        
        virtual QPainterPath shape() const override;
        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w );

    private:
        int m_addrBits;
};

#endif

