/***************************************************************************
 *   Copyright (C) 2010 by santiago González                               *
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
 
#include "circuitwidget.h"
#include "mainwindow.h"
#include "simulator.h"
#include "circuit.h"
#include "filebrowser.h"
#include "utils.h"

CircuitWidget*  CircuitWidget::m_pSelf = 0l;

CircuitWidget::CircuitWidget( QWidget *parent  )
             : QWidget( parent )
             , m_verticalLayout( this )
             , m_horizontLayout()
             , m_circView( this )
             , m_circToolBar( this )
             , m_fileMenu( this )
             , m_infoMenu( this )
{
    setObjectName( "CircuitWidget" );
    m_pSelf = this;

    m_verticalLayout.setObjectName( "verticalLayout" );
    m_verticalLayout.setContentsMargins(0, 0, 0, 0);
    m_verticalLayout.setSpacing(0);

    m_verticalLayout.addWidget( &m_circToolBar );
    m_verticalLayout.addWidget( &m_circView );
    
    m_verticalLayout.addLayout( &m_horizontLayout );
    
    m_rateLabel = new QLabel( this );
    QFont font( "Arial", 10, QFont::Normal );
    double fontScale = MainWindow::self()->fontScale();
    font.setPixelSize( int(10*fontScale) );
    m_rateLabel->setFont( font );

    m_errorLabel  = new QLabel( this );
    m_errorLabel->setFont( font );
    m_errorLabel->setMaximumSize( 200, 15 );
    //m_errorLabel->setStyleSheet("QLabel { background-color: lightgreen; color: blue; }");

    createActions();
    updateRecentFileActions();
    createToolBars();
    
    QString appPath = QCoreApplication::applicationDirPath();
    
    m_lastCircDir = MainWindow::self()->settings()->value("lastCircDir").toByteArray();
    if( m_lastCircDir.isEmpty() )  m_lastCircDir = appPath + "..share/simulide/examples";
    
    newCircuit();
    setRate( 0, 0 );
}
CircuitWidget::~CircuitWidget() { }

void CircuitWidget::clear()
{
    m_circView.clear();
    m_circView.setCircTime( 0 );
}

void CircuitWidget::createActions()
{
    for( int i=0; i<MaxRecentFiles; i++ )
    {
        recentFileActs[i] = new QAction( this );
        recentFileActs[i]->setVisible( false );
        connect( recentFileActs[i], SIGNAL( triggered() ),
                 this,              SLOT( openRecentFile() ), Qt::UniqueConnection );
    }

    newCircAct = new QAction( QIcon(":/newcirc.png"), tr("New C&ircuit\tCtrl+N"), this);
    newCircAct->setStatusTip( tr("Create a new Circuit"));
    connect( newCircAct, SIGNAL( triggered()),
                   this, SLOT( newCircuit()), Qt::UniqueConnection );

    openCircAct = new QAction( QIcon(":/opencirc.png"), tr("&Open Circuit\tCtrl+O"), this);
    openCircAct->setStatusTip( tr("Open an existing Circuit"));
    connect( openCircAct, SIGNAL( triggered()),
                    this, SLOT(openCirc()), Qt::UniqueConnection );

    saveCircAct = new QAction( QIcon(":/savecirc.png"), tr("&Save Circuit\tCtrl+S"), this);
    saveCircAct->setStatusTip( tr("Save the Circuit to disk"));
    connect( saveCircAct, SIGNAL( triggered()),
                    this, SLOT(saveCirc()), Qt::UniqueConnection );

    saveCircAsAct = new QAction( QIcon(":/savecircas.png"),tr("Save Circuit &As...\tCtrl+Shift+S"), this);
    saveCircAsAct->setStatusTip( tr("Save the Circuit under a new name"));
    connect( saveCircAsAct, SIGNAL( triggered()),
                      this, SLOT(saveCircAs()), Qt::UniqueConnection );

    powerCircAct = new QAction( QIcon(":/poweroff.png"),tr("Power Circuit"), this);
    powerCircAct->setStatusTip(tr("Power the Circuit"));
    powerCircAct->setIconText("Off");
    connect( powerCircAct, SIGNAL( triggered()),
                     this, SLOT(powerCirc()), Qt::UniqueConnection );

    pauseSimAct = new QAction( QIcon(":/pausesim.png"),tr("Pause Simulation"), this);
    pauseSimAct->setStatusTip(tr("Pause Simulation"));
    connect( pauseSimAct, SIGNAL( triggered()),
             this, SLOT(pauseSim()), Qt::UniqueConnection );
    
    infoAct = new QAction( QIcon(":/help.png"),tr("Online Help"), this);
    infoAct->setStatusTip(tr("Online Help"));
    connect( infoAct, SIGNAL( triggered()),
                this, SLOT(openInfo()), Qt::UniqueConnection );
    
    aboutAct = new QAction( QIcon(":/about.png"),tr("About SimulIDE"), this);
    aboutAct->setStatusTip(tr("About SimulIDE"));
    connect( aboutAct, SIGNAL( triggered()),
                 this, SLOT(about()), Qt::UniqueConnection );
    
    aboutQtAct = new QAction( QIcon(":/about.png"),tr("About Qt"), this);
    aboutQtAct->setStatusTip(tr("About Qt"));
    connect( aboutQtAct, SIGNAL(triggered()),
                   qApp, SLOT(aboutQt()), Qt::UniqueConnection );
}

void CircuitWidget::createToolBars()
{
    for( int i=0; i<MaxRecentFiles; i++ ) m_fileMenu.addAction( recentFileActs[i] );
    QToolButton* fileButton = new QToolButton( this );
    fileButton->setToolTip( tr("Last Circuits") );
    fileButton->setMenu( &m_fileMenu );
    fileButton->setIcon( QIcon(":/lastfiles.png") );
    fileButton->setPopupMode( QToolButton::InstantPopup );
    m_circToolBar.addWidget( fileButton );

    m_circToolBar.setObjectName( "m_circToolBar" );
    m_circToolBar.addAction( newCircAct );

    m_circToolBar.addAction( openCircAct );
    m_circToolBar.addAction( saveCircAct );
    m_circToolBar.addAction( saveCircAsAct );
    m_circToolBar.addSeparator();//..........................
    m_circToolBar.addAction( powerCircAct );
    m_circToolBar.addAction( pauseSimAct );
    m_circToolBar.addSeparator();//..........................
    m_circToolBar.addWidget( m_rateLabel );
    //m_circToolBar.addSeparator();
    m_circToolBar.addWidget( m_errorLabel );

    QWidget *spacerWidget = new QWidget( this );
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget->setVisible( true );
    m_circToolBar.addWidget( spacerWidget );

    m_infoMenu.addAction( infoAct );
    m_infoMenu.addAction( aboutAct );
    m_infoMenu.addAction( aboutQtAct );

    QToolButton* toolButton = new QToolButton( this );
    toolButton->setStatusTip( tr("Info") );
    toolButton->setMenu( &m_infoMenu );
    toolButton->setIcon( QIcon(":/help.png") );
    toolButton->setPopupMode( QToolButton::InstantPopup );
    m_circToolBar.addWidget( toolButton );
    
    m_circToolBar.addSeparator();//..........................
}

bool CircuitWidget::newCircuit()
{
    if( Simulator::self() ) powerCircOff();
    
    if( MainWindow::self()->windowTitle().endsWith('*') )
    {
        const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, "MainWindow::closeEvent",
                               tr("\nCircuit has been modified.\n"
                                  "Do you want to save your changes?\n"),
          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                               
        if     ( ret == QMessageBox::Save )   saveCirc();
        else if( ret == QMessageBox::Cancel ) return false;
    }
    clear();

    Circuit::self()->setAutoBck( MainWindow::self()->autoBck() );

    m_curCirc = "";

    MainWindow::self()->setTitle( tr("New Circuit"));
    MainWindow::self()->settings()->setValue( "lastCircDir", m_lastCircDir );
    
    return true;
}

void CircuitWidget::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    if( action ) loadCirc( action->data().toString() );
}

void CircuitWidget::openCirc()
{
    const QString dir = m_lastCircDir;
    QString fileName = QFileDialog::getOpenFileName( 0l, tr("Load Circuit"), dir,
                                        tr("Circuits (*.simu);;All files (*.*)"));
    loadCirc( fileName );
}

void CircuitWidget::loadCirc( QString path )
{
    if( !path.isEmpty() && path.endsWith(".simu") )
    {
        newCircuit();
        Circuit::self()->loadCircuit( path );
   
        m_curCirc = path;
        m_lastCircDir = path;
        MainWindow::self()->setTitle(path.split("/").last());

        QSettings* settings = MainWindow::self()->settings();
        settings->setValue( "lastCircDir", m_lastCircDir );

        QStringList files = settings->value("recentCircList").toStringList();
        files.removeAll( m_curCirc );
        files.prepend( m_curCirc );
        while( files.size() > MaxRecentFiles ) files.removeLast();

        settings->setValue("recentCircList", files );
        updateRecentFileActions();

        m_circView.setCircTime( 0 );
    }
}

void CircuitWidget::saveCirc()
{
    bool saved = false;
    emit saving();

    if( m_curCirc.isEmpty() ) saved =  saveCircAs();
    else                      saved =  Circuit::self()->saveCircuit( m_curCirc );
    
    if( saved ) 
    {
        QString fileName = m_curCirc;
        MainWindow::self()->setTitle(fileName.split("/").last());
    }
}

bool CircuitWidget::saveCircAs()
{
    const QString dir = m_lastCircDir;
    QString fileName = QFileDialog::getSaveFileName( this, tr("Save Circuit"), dir,
                                                     tr("Circuits (*.simu);;All files (*.*)"));
    if (fileName.isEmpty()) return false;

    m_curCirc = fileName;
    m_lastCircDir = fileName;
    
    bool saved = Circuit::self()->saveCircuit(fileName);
    if( saved ) 
    {
        QString fileName = m_curCirc;
        MainWindow::self()->setTitle(fileName.split("/").last());
        MainWindow::self()->settings()->setValue( "lastCircDir", m_lastCircDir );
    }
    return saved;
}

void CircuitWidget::powerCirc()
{
    if     ( powerCircAct->iconText() == "Off" ) powerCircOn();
    else if( powerCircAct->iconText() == "On" )  powerCircOff();
}

void CircuitWidget::powerCircOn()
{
    powerCircAct->setIcon(QIcon(":/poweron.png"));
    powerCircAct->setEnabled( true );
    powerCircAct->setIconText("On");
    pauseSimAct->setEnabled( true );
    Simulator::self()->startSim();
}

void CircuitWidget::powerCircOff()
{
    if( Simulator::self()->isPaused() ) Simulator::self()->resumeSim();
    if( Simulator::self()->isRunning() ) Simulator::self()->stopSim();

    powerCircAct->setIcon(QIcon(":/poweroff.png"));
    powerCircAct->setIconText("Off");
    powerCircAct->setEnabled( true );
    pauseSimAct->setEnabled( false );
}

void CircuitWidget::powerCircDebug( bool run )
{
    powerCircAct->setIcon(QIcon(":/powerdeb.png"));
    powerCircAct->setIconText("Debug");
    powerCircAct->setEnabled( true );

    Simulator::self()->debug( run );
    m_rateLabel->setText( tr("    Real Speed: Debugger") );
}

void CircuitWidget::pauseSim()
{
    if( Simulator::self()->isRunning() )
    {
        Simulator::self()->pauseSim();
        powerCircAct->setEnabled( false );
    }
    else if( Simulator::self()->isPaused() )
    {
        Simulator::self()->resumeSim();
        powerCircAct->setEnabled( true );
    }
}

void CircuitWidget::openInfo()
{
    QDesktopServices::openUrl(QUrl("http://simulide.blogspot.com"));
}

void CircuitWidget::about()
{
    QString date = QDate::currentDate().toString( "dd-MM-yyyy" );
    date.append( " (dd-MM-yyyy)" );

    QString r = "&nbsp;";
    QString s = "&nbsp;&nbsp;&nbsp;";
    QString t = "&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;";
    QMessageBox::about( this, tr("About SimulIDE"),
                        "<br><br>"
    "<b>Version:</b>&nbsp; &nbsp; &nbsp;"+QString( APP_VERSION )+"<br><b>Compiled:</b> "+date+"<br><br><br>"
    "<b>Web site:</b> <a href=\"https://simulide.com/\"> https://simulide.com/ </a><br><br>"
    "<b>Become a Patron:</b> <a href=\"https://www.patreon.com/simulide\"> https://www.patreon.com/simulide </a><br><br>"
    "<br><br>"
               "<b>Creator:</b> Santiago Gonzalez. <br>"
               "<br>"
               "<b>Developers:</b> <br>"
               +t+"Santiago Gonzalez. <br>"
               +t+"Popov Alexey <br>"
               "<br>"
               "<b>Contributors:</b> <br>"
               +t+"Chris Roper <br>"
               +t+"Sergey Chiyanov <br>"
               +t+"Sergey Roenko <br>"
               +t+"Gabor Nagy <br>"
               +t+"Tim Fischer <br>"
               +t+"Benoit ZERR <br>"
               +t+"Jan K. S. <br>"
               "<br>"
               "<b>Translations:</b> <br>"
               +t+"Spanish:"+r+"Santiago Gonzalez. <br>"
               +t+s+s+s+s+r+r+"   Kike Gl. <br>"
               +t+"Russian:"+r+"Sergey Chiyanov <br>"
               +t+s+s+s+s+r+r+"   Sergey Roenko <br>"
               +t+"French:"+s+"Pascal Cotret <br>"
               +t+"German:    Tim Fischer <br>"
               +t+"Brasilian: Maico Smaniotto <br>"
               +t+"Italian:"+s+r+"@rkanoid <br>"
               );
}

void CircuitWidget::setRate( int rate, int load )
{
    if( rate < 0 ) m_rateLabel->setText( tr("Circuit ERROR!!!") );
    else
    {
        if( (load > 150) || (load < 0) ) load = 0;
        double speed = (double)rate/100;
        QString Srate = QString::number( speed,'f', 2 );
        if( speed < 100 ) Srate = "0"+Srate;
        if( speed < 10 )  Srate = "0"+Srate;
        QString Sload = QString::number( load );
        if( load < 100 ) Sload = "0"+Sload;
        if( load < 10 )  Sload = "0"+Sload;

        m_rateLabel->setText( tr("    Real Speed: ")+Srate+" %"
                            + tr("    Load: "      )+Sload+" %    ");
        m_errorLabel->setStyleSheet("QLabel { background-color: lightgreen; color: green; }");
        m_errorLabel->setText( "   Ok   ");
    }
}

void CircuitWidget::setError( QString error )
{
    m_errorLabel->setStyleSheet("QLabel { background-color: yellow; color: red; }");
    m_errorLabel->setText( "   "+error );
    setRate( -1, 0 );
}

void CircuitWidget::updateRecentFileActions()
{
    QSettings* settings = MainWindow::self()->settings();
    QStringList files = settings->value("recentCircList").toStringList();

    int numRecentFiles = qMin( files.size(), (int)MaxRecentFiles );

    for( int i=0; i<numRecentFiles; i++ )
    {
        QString text = tr("&%1 %2").arg(i + 1).arg( strippedName( files[i] ) );
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData( files[i] );
        recentFileActs[i]->setVisible( true );
    }
    for( int i=numRecentFiles; i<MaxRecentFiles; i++ ) recentFileActs[i]->setVisible(false);
}


#include "moc_circuitwidget.cpp"
