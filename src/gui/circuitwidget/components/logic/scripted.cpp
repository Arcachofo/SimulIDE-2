/***************************************************************************
 *   Copyright (C) 2018 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#include <QDebug>
#include <QMenu>
#include <QFileDialog>

#include "scripted.h"
#include "connector.h"
#include "circuit.h"
#include "itemlibrary.h"
#include "iopin.h"
#include "e-node.h"
#include "utils.h"

#include "stringprop.h"
#include "boolprop.h"
#include "intprop.h"

Component* Scripted::construct( QObject* parent, QString type, QString id )
{ return new Scripted( parent, type, id ); }

LibraryItem* Scripted::libraryItem()
{
    return new LibraryItem(
        tr( "Scripted" ),
        tr( "Logic/Arithmetic" ),
        "subc.png",
        "Scripted",
        Scripted::construct );
}

Scripted::Scripted( QObject* parent, QString type, QString id )
        : LogicComponent( parent, type, id )
{
    m_lastDir = Circuit::self()->getFilePath();
    
    m_width = 4;
    setNumInps( 2 );                           // Create Input Pins
    setNumOuts( 1 );

    m_thisObject = m_engine.newQObject( this );
    m_engine.globalObject().setProperty( "component", m_thisObject );

    addPropGroup( { tr("Main"), {
//new IntProp <Scripted>( "Num_Inputs" , tr("Input Size")  ,"_Pins", this, &Scripted::numInps,   &Scripted::setNumInps, "uint" ),
//new IntProp <Scripted>( "Num_Outputs", tr("Output Size") ,"_Pins", this, &Scripted::numOuts,   &Scripted::setNumOuts, "uint" ),
new BoolProp<Scripted>( "Inverted", tr("Invert Outputs") ,""     , this, &Scripted::invertOuts,&Scripted::setInvertOuts )
    }} );
    addPropGroup( { tr("Script"), {
new StringProp<Scripted>(  "Script"  , tr("Script Editor")  ,"", this, &Scripted::getScript, &Scripted::setScript, "textSave" ),
new ComProperty(           "Evaluate", tr("Evaluate Script"),"", "textEvaluate" ),
    }} );
    addPropGroup( { tr("Electric"), IoComponent::inputProps()+IoComponent::outputProps() } );
}
Scripted::~Scripted(){}

void Scripted::stamp()
{
    m_canRun = false;
    LogicComponent::stamp();

    QString excep = evaluate( m_script );
    if( excep != "" ) return;
    m_canRun = true;

    for( uint i=0; i<m_inPin.size(); ++i )
    {
        eNode* enode = m_inPin[i]->getEnode();
        if( enode ) enode->voltChangedCallback( this );
}   }

void Scripted::voltChanged()
{
    if( !m_canRun ) return;

    m_voltChanged.call( m_thisObject );
    //if( getExceptions() ) return;

    ///Simulator::self()->addEvent( m_propDelay, this );
}

QString Scripted::evaluate( QString script )
{
    if( !m_engine.canEvaluate( script ) ) return "Error canEvaluate returned false!";

    m_program = QScriptProgram( script );

    m_engine.evaluate( m_program );
    QString excep = getExceptions();
    if( excep != "" ) return excep;

    m_voltChanged = m_engine.evaluate( "voltChanged" );
    excep = getExceptions();
    if( excep != "" ) return excep;

    if (!m_voltChanged.isFunction()) return "voltChanged: Not a function:\n";

    m_voltChanged.call( m_thisObject );
    excep = getExceptions();
    if( excep != "" ) return excep;

    return "";
}

QString Scripted::getExceptions()
{
    if( m_engine.hasUncaughtException() )
    {
        QScriptValue exception = m_engine.uncaughtException();
        return exception.toString();
    }
    return "";
}

void Scripted::runEvent()
{
}

void Scripted::displayMsg( QString msg ) { qDebug() << msg; }

void Scripted::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
    if( !acceptedMouseButtons() ) event->ignore();
    else
    {
        event->accept();
        QMenu* menu = new QMenu();
        contextMenu( event, menu );
        Component::contextMenu( event, menu );
        menu->deleteLater();
}   }

void Scripted::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
{
    menu->addSeparator();
    QAction* loadDaAction = menu->addAction( QIcon(":/load.png"),tr("Load script") );
    connect( loadDaAction, SIGNAL(triggered()),
                     this, SLOT(loadScript()), Qt::UniqueConnection );

    QAction* saveDaAction = menu->addAction(QIcon(":/save.png"), tr("Save script") );
    connect( saveDaAction, SIGNAL(triggered()),
                     this, SLOT(saveScript()), Qt::UniqueConnection );
    menu->addSeparator();
}

void Scripted::loadScript()
{
    QString fileName = QFileDialog::getOpenFileName( 0l, "Scripted::loadData", m_lastDir, "" );

    if( fileName.isEmpty() ) return; // User cancels loading
    m_lastDir = fileName;
    m_script = fileToString( fileName, "Scripted::loadData" );
}

void Scripted::saveScript()
{
    QString fileName = QFileDialog::getSaveFileName( 0l, "Scripted::saveData", m_lastDir, "" );

    if( fileName.isEmpty() ) return; // User cancels saving
    m_lastDir = fileName;
    QFile outFile( fileName );

    if( !outFile.open( QFile::WriteOnly | QFile::Text ) )
    {
          MessageBoxNB( "Scripted::saveData"
                       , tr("Cannot write file %1:\n%2.").arg(fileName).arg( outFile.errorString() ));
    }else {
        QTextStream toFile( &outFile );
        toFile << m_script;
        outFile.close();
}   }

#include "moc_scripted.cpp"
