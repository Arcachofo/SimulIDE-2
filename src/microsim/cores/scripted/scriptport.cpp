/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "scriptport.h"
#include "e_mcu.h"
#include "mcupin.h"

ScriptPort::ScriptPort( eMcu* mcu, QString name )
          : ScriptBase( mcu->getId()+"-"+name )
          , McuPort( mcu, name )
{
}
ScriptPort::~ScriptPort(){}

void ScriptPort::configureA()
{
    //callFunction( &m_configureA, {QScriptValue( (int)newVal )} );
}

void ScriptPort::configureB()
{
    //callFunction( &m_configureB, {QScriptValue( (int)newVal )} );
}

void ScriptPort::configureC()
{
    //m_configureC.call( m_thisObject, newVal );
}

void ScriptPort::reset()
{
    //callFunction( &m_reset );
}

void ScriptPort::setExtIntTrig( int pinNumber, int trig )
{
    m_pins[pinNumber]->setExtIntTrig( trig );
}

void ScriptPort::setScript( QString script )
{
    ScriptBase::setScript( script );

    /*m_configureA = evalFunc("configureA");
    m_configureB = evalFunc("configureB");
    m_configureC = evalFunc("configureC");*/
}
