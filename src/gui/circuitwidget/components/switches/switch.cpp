/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QGraphicsProxyWidget>
#include <QToolButton>

#include "switch.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "e-node.h"
#include "pin.h"

#include "stringprop.h"
#include "boolprop.h"
#include "intprop.h"

Component* Switch::construct( QObject* parent, QString type, QString id )
{ return new Switch( parent, type, id ); }

LibraryItem* Switch::libraryItem()
{
    return new LibraryItem(
        tr( "Switch (all)" ),
        tr( "Switches" ),
        "switch.png",
        "Switch",
        Switch::construct);
}

Switch::Switch( QObject* parent, QString type, QString id )
      : SwitchBase( parent, type, id )
{
    m_area =  QRectF( -11, -9, 22, 11 );
    m_proxy->setPos( QPoint(-8, 4) );

    SetupSwitches( 1, 1 );

    connect( m_button, SIGNAL( clicked() ),
             this,     SLOT  ( onbuttonclicked() ), Qt::UniqueConnection);

    addPropGroup( { tr("Main"), {
new BoolProp  <Switch>( "Norm_Close", tr("Normally Closed"),""      , this, &Switch::nClose, &Switch::setNClose ),
new BoolProp  <Switch>( "DT"        , tr("Double Throw")   ,""      , this, &Switch::dt,     &Switch::setDt ),
new IntProp   <Switch>( "Poles"     , tr("Poles")          ,"_Poles", this, &Switch::poles,  &Switch::setPoles, "uint" ),
new StringProp<Switch>( "Key"       , tr("Key")            ,""      , this, &Switch::key,    &Switch::setKey ),
    }} );
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

void Switch::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    if( m_hidden ) return;

    Component::paint( p, option, widget );
    MechContact::paint( p, option, widget );
}

#include "moc_switch.cpp"
