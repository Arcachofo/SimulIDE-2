/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "voltmeter.h"
#include "e-pin.h"

#define tr(str) simulideTr("Voltimeter",str)

listItem_t Voltimeter::libraryItem(){
    return {
        tr("Voltmeter"),
        "Meters",
        "voltimeter.png",
        "Voltimeter",
        [](QString id){ return (Component*)new Voltimeter("Voltimeter", id ); } };
}

Voltimeter::Voltimeter( QString type, QString id )
          : Meter( type, id )
{
    m_unit = "V";
    setResistance( high_imp );
    m_display.setText( " 0.000\n V");
}
Voltimeter::~Voltimeter(){}

void Voltimeter::updateStep()
{
    double volt = m_ePin[0]->getVoltage()-m_ePin[1]->getVoltage();
    
    if( volt != m_dispValue )
    {
        m_dispValue = volt;
        Meter::updateStep();
}   }
