/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef BUS_H
#define BUS_H

#include "component.h"
#include "e-element.h"

class eNode;

class Bus : public Component, public eElement
{
    public:
        Bus( QString type, QString id );
        ~Bus();

 static listItem_t libraryItem();
        
        int numLines() { return m_numLines; }
        void setNumLines( int lines );

        int startBit() { return m_startBit; }
        void setStartBit( int bit );

        virtual void registerEnode( eNode* enode, int n=-1 ) override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget );
        
    protected:
        int m_height;

        int m_numLines;
        int m_startBit;
        
        Pin* m_busPin0;
        Pin* m_busPin1;
};

#endif

