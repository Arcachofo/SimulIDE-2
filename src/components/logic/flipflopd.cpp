/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "flipflopd.h"
#include "iopin.h"

#define tr(str) simulideTr("FlipFlopD",str)

listItem_t FlipFlopD::libraryItem(){
    return {
        tr("FlipFlopD"),
        "Memory",
        "2to2.png",
        "FlipFlopD",
        [](QString type, QString id){ return (Component*)new FlipFlopD( type, id ); } };
}

FlipFlopD::FlipFlopD( QString type, QString id )
         : FlipFlopBase( type, id )
{
    m_width  = 3;
    m_height = 3;
    m_dataPins = 1;

    init({         // Inputs:
            "IL01D",
            "IU01S",
            "ID02R",
            "IL02>",
                   // Outputs:
            "OR01Q",
            "OR02!Q"
        });

    m_setPin = m_inPin[1];
    m_rstPin = m_inPin[2];
    m_clkPin = m_inPin[3];

    m_setPin->setInverted( true );
    m_rstPin->setInverted( true );

    setTrigger( Clock );
}
FlipFlopD::~FlipFlopD(){}

void FlipFlopD::calcOutput()
{
    m_nextOutVal = m_inPin[0]->getInpState()? 1:2; // D state
}
