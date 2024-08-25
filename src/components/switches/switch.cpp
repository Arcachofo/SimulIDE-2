/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QGraphicsProxyWidget>

#include "switch.h"
#include "simulator.h"
#include "e-node.h"
#include "pin.h"
#include "custombutton.h"

#include "stringprop.h"
#include "boolprop.h"
#include "intprop.h"

#define tr(str) simulideTr("Switch",str)

listItem_t Switch::libraryItem(){
    return {
        tr("Switch (all)"),
        "Switches",
        "switch.png",
        "Switch",
        [](QString type, QString id){ return (Component*)new Switch( type, id ); } };
}

Switch::Switch( QString type, QString id )
      : SwitchBase( type, id )
{
    m_area = QRectF(-11,-9, 22, 11 );
    m_proxy->setPos(-8, 4 );

    SetupSwitches( 1, 1 );

    QObject::connect( m_button, &CustomButton::clicked, [=](){ onbuttonclicked(); });

    addPropGroup( { tr("Main"), {
        new BoolProp<Switch>("Norm_Close", tr("Normally Closed"), ""
                            , this, &Switch::nClose, &Switch::setNClose ),

        new BoolProp<Switch>("DT", tr("Double Throw"), ""
                            , this, &Switch::dt, &Switch::setDt, propNoCopy ),

        new IntProp <Switch>("Poles", tr("Poles"), ""
                            , this, &Switch::poles, &Switch::setPoles, propNoCopy,"uint" ),

        new StrProp <Switch>("Key", tr("Key"), ""
                            , this, &Switch::key, &Switch::setKey ),

        new BoolProp<Switch>("Checked", "", ""
                            , this, &Switch::checked, &Switch::setChecked, propHidden ),
    },0} );
}
Switch::~Switch(){}

void Switch::stamp()
{
    MechContact::stamp();
    onbuttonclicked();
}

void Switch::keyEvent( QString key, bool pressed )
{
    if( key == m_key )
    {
        if( !pressed )
        {
            m_button->setChecked( !m_button->isChecked() );
            SwitchBase::onbuttonclicked();
}   }   }

bool Switch::checked()
{
    return m_button->isChecked();
}

void Switch::setChecked( bool c )
{
    m_button->setChecked( c );
    SwitchBase::onbuttonclicked();
}

void Switch::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    if( m_hidden ) return;

    Component::paint( p, o, w );
    MechContact::paint( p, o, w );
}
