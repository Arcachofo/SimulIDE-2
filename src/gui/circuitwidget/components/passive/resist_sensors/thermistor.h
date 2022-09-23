/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef Thermistor_H
#define Thermistor_H

#include "thermistorbase.h"

class LibraryItem;

class MAINMODULE_EXPORT Thermistor : public ThermistorBase
{
    public:
        Thermistor( QObject* parent, QString type, QString id );
        ~Thermistor();

        static Component* construct( QObject* parent, QString type, QString id );
        static LibraryItem* libraryItem();

        //virtual QList<propGroup_t> propGroups() override;

        int bVal() { return m_bVal; }
        void setBval( int bval );

        int r25() { return m_r25; }
        void setR25( int r25 );

        virtual void updateStep() override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget *widget );

    private:
        int m_bVal;
        int m_r25;
};

#endif
