/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "flipflopt.h"
#include "iopin.h"

#define tr(str) simulideTr("FlipFlopT",str)

listItem_t FlipFlopT::libraryItem(){
    return {
        tr("FlipFlop T"),
        "Memory",
        "2to2.png",
        "FlipFlopT",
        [](QString id){ return (Component*)new FlipFlopT("FlipFlopT", id ); } };
}

FlipFlopT::FlipFlopT( QString type, QString id )
         : FlipFlopBase( type, id )
{
    m_width  = 3;
    m_height = 3;
    m_dataPins = 1;

    init({         // Inputs:
            "IL01T",
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
FlipFlopT::~FlipFlopT(){}

void FlipFlopT::calcOutput()
{
    bool T = m_inPin[0]->getInpState();
    if( T ) m_nextOutVal = m_outPin[1]->getOutState()? 1:2; // !Q state = Toggle
}
