/***************************************************************************
 *   Copyright (C) 2017 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef ADC_H
#define ADC_H

#include "logiccomponent.h"

class ADC : public LogicComponent
{
    public:
        ADC( QString type, QString id );
        ~ADC();

 static listItem_t libraryItem();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override { IoComponent::runOutputs(); }

        double maxVolt() { return m_maxVolt; }
        void setMaxVolt( double v ) { m_maxVolt = v; }

        void setNumOutputs( int pins );
        
    protected:
        double m_maxVolt;
        double m_maxValue;
};

#endif
