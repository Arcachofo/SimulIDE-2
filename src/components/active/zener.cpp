/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "zener.h"

#define tr(str) simulideTr("Zener",str)

listItem_t Zener::libraryItem(){
    return {
        tr("Zener Diode"),
        "Rectifiers",
        "zener.png",
        "Zener",
        [](QString type, QString id){ return (Component*)new Zener( type, id ); } };
}

Zener::Zener( QString type, QString id )
     : Diode( type, id, true )
{
    m_enumUids = m_enumNames = m_zeners.keys();
}
Zener::~Zener(){}
