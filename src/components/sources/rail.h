/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef RAIL_H
#define RAIL_H

#include "component.h"
#include "e-element.h"

class IoPin;

class Rail : public Component, public eElement
{
    public:
        Rail( QString type, QString id );
        ~Rail();

 static listItem_t libraryItem();

        double volt() { return m_volt; }
        void setVolt( double v );

        virtual void stamp() override;

        virtual QPainterPath shape() const override;
        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget ) override;

    private:
        double m_volt;

        IoPin* m_out;
};

#endif
