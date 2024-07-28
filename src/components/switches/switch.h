/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef SWITCH_H
#define SWITCH_H

#include "switch_base.h"

class Switch : public SwitchBase
{
    public:
        Switch( QString type, QString id );
        ~Switch();

 static listItem_t libraryItem();

        virtual void stamp() override;

        bool checked();
        void setChecked( bool c );

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

    public slots:
        virtual void keyEvent( QString key, bool pressed ) override;
};

#endif
