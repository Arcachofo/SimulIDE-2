/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef BATTERY_H
#define BATTERY_H

#include "comp2pin.h"
#include "e-resistor.h"

class Battery : public Comp2Pin, public eResistor
{
    public:
        Battery( QString type, QString id );
        ~Battery();

 static listItem_t libraryItem();

        double voltage() { return m_volt; }
        void setVoltage( double volt );

        virtual void setResistance( double resist ) override;

        virtual void stamp() override;
        virtual void updateStep() override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget ) override;

    private:
        double m_volt;

        bool m_connected;
};

#endif
