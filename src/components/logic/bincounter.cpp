/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "bincounter.h"
#include "connector.h"
#include "iopin.h"

#include "intprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("BinCounter",str)

listItem_t BinCounter::libraryItem(){
    return {
        tr("Binary Counter"),
        "Arithmetic",
        "2to1.png",
        "BinCounter",
        [](QString id){ return (Component*)new BinCounter("BinCounter", id ); } };
}

BinCounter::BinCounter( QString type, QString id)
          : LogicComponent( type, id )
{
    m_TopValue = 1;
    m_width  = 3;
    m_height = 3;

    init({         // Inputs:
            "IL01>",
            "IL02R",
            "IU01S",
                   // Outputs:
            "OR01Q"
        });
    
    m_clkPin = m_inPin[0];     // eClockedDevice
    m_rstPin = m_inPin[1];
    m_setPin = m_inPin[2];

    m_setPin->setInverted( true );
    m_rstPin->setInverted( true );

    useSetPin( false );          // Don't use Set Pin

    addPropGroup( { tr("Main"), {
        new BoolProp<BinCounter>("Pin_SET", tr("Use Set Pin"),""
                                , this, &BinCounter::pinSet,&BinCounter::useSetPin, propNoCopy ),

        new IntProp <BinCounter>("Max_Value", tr("Count to"),""
                                , this, &BinCounter::maxVal, &BinCounter::setMaxVal,0,"uint" ),
    },groupNoCopy} );

    addPropGroup( { tr("Electric"), IoComponent::inputProps()+IoComponent::outputProps(),0 } );
    addPropGroup( { tr("Timing")  , IoComponent::edgeProps(),0 } );
}
BinCounter::~BinCounter(){}

bool BinCounter::setPropStr( QString prop, QString val )
{
    if( prop =="Clock_Inverted" ) // Old circuits
    {
        m_clkPin->setInverted( val == "true" );
    }
    else if( prop =="Reset_Inverted" ) // Old circuits
    {
        bool invert = (val == "true");
        m_setPin->setInverted( invert );
        m_rstPin->setInverted( invert );
    }
    else return Component::setPropStr( prop, val );
    return true;
}

void BinCounter::stamp()
{
    m_Counter = 0;
    m_rstPin->changeCallBack( this );
    m_setPin->changeCallBack( this );
    LogicComponent::stamp();
}

void BinCounter::voltChanged()
{
    updateClock();
    bool clkRising = ( m_clkState == Clock_Rising );

    if( m_rstPin->getInpState() ) // Reset
    {
       m_Counter = 0;
       m_nextOutVal = 0;
    }
    else if( m_pinSet && m_setPin->getInpState() ) // Set
    {
       m_Counter = m_TopValue;
       m_nextOutVal = 1;
    }
    else if( clkRising )
    {
        m_Counter++;

        if(      m_Counter == m_TopValue ) m_nextOutVal = 1;
        else if( m_Counter >  m_TopValue )
        {
            m_Counter = 0;
            m_nextOutVal = 0;
    }   }
    IoComponent::scheduleOutPuts( this );
}

void BinCounter::useSetPin( bool set )
{
    m_pinSet = set;
    if( !set ) m_setPin->removeConnector();
    m_setPin->setVisible( set );
}
