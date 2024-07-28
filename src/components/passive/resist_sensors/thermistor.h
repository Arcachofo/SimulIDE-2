/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef Thermistor_H
#define Thermistor_H

#include "thermistorbase.h"

class Thermistor : public ThermistorBase
{
    public:
        Thermistor( QString type, QString id );
        ~Thermistor();

 static listItem_t libraryItem();

        int bVal() { return m_bVal; }
        void setBval( int bval );

        int r25() { return m_r25; }
        void setR25( int r25 );

        virtual void updateStep() override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget *w ) override;

    private:
        int m_bVal;
        int m_r25;
};

#endif
