/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef LED_H
#define LED_H

#include "ledbase.h"
#include "linker.h"

class Led : public LedBase, public Linker
{
    public:
        Led( QString type, QString id );
        ~Led();

 static listItem_t libraryItem();

        virtual void voltChanged() override;

        void slotLinkComp() { Linker::startLinking(); }

    protected:
        virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent* event ) override;

        void drawBackground( QPainter* p ) override;
        void drawForeground( QPainter* p ) override;
};

#endif
