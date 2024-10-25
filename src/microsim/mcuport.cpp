/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QDebug>

#include "mcuport.h"
//#include "mcupin.h"
#include "mcu.h"
#include "e_mcu.h"
#include "mcuinterrupts.h"
#include "datautils.h"

McuPort::McuPort( eMcu* mcu, QString name )
       : McuModule( mcu, name )
{
    m_numPins = 0;

    m_shortName = "P"+ name.right(1);

    m_outReg = nullptr;
    m_dirReg = nullptr;
    m_inReg  = nullptr;
    m_puReg  = nullptr;

    m_intMask = 0;
    m_dirInv  = false;
    m_rstIntMask = true;
}
McuPort::~McuPort(){}

void McuPort::reset()
{
    //m_pinState   = 0;
    m_inpState = 0;
    m_outState = 0;
    m_dirState = 0;
    if( m_rstIntMask ) m_intMask = 0;
    /// for( McuPin* pin : m_pins ) pin->reset();
}

void McuPort::pinChanged( uint8_t pinMask, uint8_t val ) // Pin number in pinMask
{
    uint8_t pinState = (m_inpState & ~pinMask) | (val & pinMask);

    if( pinState == m_inpState ) return;
    m_inpState = pinState;

    if( (m_intMask & pinMask) && m_interrupt ) m_interrupt->raise(); // Pin change interrupt

    if( m_sleeping ) return;
    if( m_inReg ) *m_inReg = m_inpState; //m_mcu->writeReg( m_inAddr, m_pinState, false ); // Write to RAM
}

void McuPort::outChanged() // PORT register written
{
    uint8_t changed = *m_outReg ^ m_outState; // See which Pins have actually changed
    if( changed == 0 ) return;
    m_outState = *m_outReg;

    for( int i=0; i<m_numPins; ++i ){
        if( changed & (1<<i) ) m_pins[i]->setPortState( m_outState & (1<<i) ); // Pin changed
}   }

void McuPort::dirChanged() // DIR register written
{
    uint8_t changed = *m_dirReg ^ m_dirState;  // See which Pins have actually changed
    if( changed == 0 ) return;
    m_dirState = *m_dirReg;

    uint8_t val = m_dirInv ? ~m_dirState : m_dirState;   // defaul: 1 for outputs, inverted: 0 for outputs (PICs)

    for( int i=0; i<m_numPins; ++i ){
        if( changed & 1<<i) m_pins[i]->setDirection( val & (1<<i) ); // Pin changed
}   }

void McuPort::readPort()
{
    if( m_inReg ) *m_inReg = getInpState();
}

void McuPort::intChanged()
{
    m_intMask = m_intBits.getRegBitsVal();
}
// Direct Control---------------------------------------------------

void McuPort::controlPort( bool outCtrl, bool dirCtrl )
{
    for( int i=0; i<m_numPins; ++i )
        m_pins[i]->controlPin( outCtrl, dirCtrl );
}

void McuPort::setDirection( uint val )
{
    for( int i=0; i<m_numPins; ++i )
        m_pins[i]->setDirection( val & (1<<i) );
}

void McuPort::setOutState( uint val )
{
    for( int i=0; i<m_numPins; ++i )
    {
        bool state = (val & 1<<i) > 0;
        if( state != m_pins[i]->getOutState() )
            m_pins[i]->setOutState( state );
    }
}

uint32_t McuPort::getInpState()
{
    m_inpState = 0;
    for( int i=0; i<m_numPins; ++i )
        if( m_pins[i]->getInpState() ) m_inpState |= (1 << i);
    return m_inpState;
}

void McuPort::setPinMode( pinMode_t mode )
{
    for( int i=0; i<m_numPins; ++i ) m_pins[i]->setPinMode( mode );
}
//-------------------------------------------------------------------

void McuPort::setPullups( uint32_t puMask )
{
    for( int i=0; i<m_numPins; ++i )
        m_pins[i]->setPullup( puMask & 1<<i );
}

void McuPort::createPins( Mcu* mcuComp, QString pins, uint32_t pinMask )
{
    m_numPins = pins.toUInt(0,0);
    if( m_numPins )
    {
        m_pins.resize( m_numPins );

        for( int i=0; i<m_numPins; ++i )
        {
            if( pinMask & 1<<i )
                m_pins[i] = createPin( i, mcuComp->getUid()+"-"+m_name+QString::number(i) , mcuComp );//new McuPin( this, i, m_name+QString::number(i), mcuComp );
        }
    }else{
        QStringList pinList = pins.split(",");
        pinList.removeAll("");
        for( QString pinName : pinList )
        {
            McuPin* pin = createPin( m_numPins, mcuComp->getUid()+"-"+m_name+pinName , mcuComp );//new McuPin( this, i, m_name+pinName, mcuComp );
            m_pins.emplace_back( pin );
            m_numPins++;
        }
    }
}

McuPin* McuPort::createPin( int i, QString id , Component* mcu )
{
    return new McuPin( this, i, id, mcu );
}

McuPin* McuPort::getPinN( uint8_t i )
{
    if( i >= m_pins.size() ) return nullptr;
    return m_pins[i];
}

McuPin* McuPort::getPin( QString pinName )
{
    McuPin* pin = nullptr;

    if( pinName.startsWith( m_name ) || pinName.startsWith( m_shortName ) )
    {
        QString pinId = pinName.remove( m_name ).remove( m_shortName );
        int pinNumber = pinId.toInt();
        pin = getPinN( pinNumber );
    }else{
        for( McuPin* mcuPin : m_pins )
        {
            if( !mcuPin ) continue;
            QString pid = mcuPin->pinId();
            pid = pid.split("-").last().remove( m_name );
            if( pid == pinName ) return mcuPin;
        }
    }
    //if( !pin )
    //    qDebug() << "ERROR: McuPort::getPin nullptr Pin:"<< pinName;
    return pin;
}

// ---- Script Engine -------------------
#include "angelscript.h"
QStringList McuPort::registerScript( asIScriptEngine* engine )
{
    QStringList memberList;
    engine->RegisterObjectType("McuPort", 0, asOBJ_REF | asOBJ_NOCOUNT );

    memberList << "controlPort( bool outCtrl, bool dirCtrl )";
    engine->RegisterObjectMethod("McuPort", "void controlPort( bool o, bool d )"
                                   , asMETHODPR( McuPort, controlPort, (bool, bool), void)
                                   , asCALL_THISCALL );

    memberList << "setDirection( uint direction )";
    engine->RegisterObjectMethod("McuPort", "void setDirection( uint d )"
                                   , asMETHODPR( McuPort, setDirection, (uint), void)
                                   , asCALL_THISCALL );

    memberList << "getInpState()";
    engine->RegisterObjectMethod("McuPort", "uint getInpState()"
                                   , asMETHODPR( McuPort, getInpState, (), uint)
                                   , asCALL_THISCALL );

    memberList << "setOutState( uint state )";
    engine->RegisterObjectMethod("McuPort", "void setOutState(uint s)"
                                   , asMETHODPR( McuPort, setOutState, (uint), void)
                                   , asCALL_THISCALL );

    return memberList;
}
