/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef VOLTREG_H
#define VOLTREG_H

#include "component.h"
#include "e-resistor.h"

class VoltReg : public Component, public eResistor
{
    public:
        VoltReg( QString type, QString id );
        ~VoltReg();
        
 static listItem_t libraryItem();

        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;

        double outVolt() { return m_vRef; }
        void setOutVolt( double v ) { m_vRef = v; m_changed = true; }

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w );

    protected:
        double m_vRef;
        double m_lastCurrent;

        bool m_connected;
};

#endif
