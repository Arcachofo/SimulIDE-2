/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
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

#include "editorwindow.h"
#include "mainwindow.h"
#include "simulator.h"
#include "filebrowser.h"
#include "compiler.h"
#include "utils.h"

EditorWindow*  EditorWindow::m_pSelf = 0l;

EditorWindow::EditorWindow( QWidget* parent )
            : QWidget( parent )
            , m_fileMenu( this )
            , m_outPane( this )
            , m_compiler( this, &m_outPane )
{
    m_pSelf = this;

    setAcceptDrops( true );
    
    createWidgets();
    createActions();
    updateRecentFileActions();
    createToolBars();
    readSettings();

    /// m_compiler.loadCompiler( "/home/user/gcbcompiler.xml");
}
EditorWindow::~EditorWindow(){}

bool EditorWindow::close()
{    
    writeSettings();

    for( int i=0; i<m_docWidget->count(); i++ )
    {
        closeTab( m_docWidget->currentIndex() );
    }
    return maybeSave();
}

void EditorWindow::keyPressEvent( QKeyEvent* event )
{
    if( event->key() == Qt::Key_N && (event->modifiers() & Qt::ControlModifier))
    {
        newFile();
    }
    else if( event->key() == Qt::Key_S && (event->modifiers() & Qt::ControlModifier))
    {
        if( event->modifiers() & Qt::ShiftModifier) saveAs();
        else                                        save();
    }
    else if( event->key() == Qt::Key_O && (event->modifiers() & Qt::ControlModifier))
    {
        open();
    }
}

void EditorWindow::dragEnterEvent( QDragEnterEvent* event)
{
    event->accept();
}

void EditorWindow::dropEvent( QDropEvent* event )
{
    QString id = event->mimeData()->text();

    QString file = "file://";
    if( id.startsWith( file ) )
    {
        id.replace( file, "" ).replace("\r\n", "" );
#ifdef _WIN32
        if( id.startsWith( "/" )) id.remove( 0, 1 );
#endif
        loadFile( id );
    }
}

void EditorWindow::newFile()
{
    CodeEditor* codeEditor = new CodeEditor( this, &m_outPane );

    m_docWidget->addTab( codeEditor, "New" );
    m_docWidget->setCurrentWidget( codeEditor );
    
    connect( codeEditor->document(), SIGNAL( contentsChanged()),
             this,                   SLOT(   documentWasModified()), Qt::UniqueConnection);
            
    m_fileList << "New";
    enableFileActs( true ); 
    enableDebugActs( true );
}

void EditorWindow::open()
{
    const QString dir = m_lastDir;
    QString fileName = QFileDialog::getOpenFileName( this, tr("Load File"), dir,
                       tr("All files")+" (*);;Arduino (*.ino);;Asm (*.asm);;GcBasic (*.gcb)" );

    if( !fileName.isEmpty() ) loadFile( fileName );
}

void EditorWindow::loadFile( const QString &fileName )
{
    if( m_fileList.contains( fileName ) )
    {
        m_docWidget->setCurrentIndex( m_fileList.indexOf( fileName ) );
        //return;
    }
    else newFile();
    QApplication::setOverrideCursor( Qt::WaitCursor );
    
    CodeEditor* ce = getCodeEditor();
    ce->setPlainText( fileToString( fileName, "EditorWindow" ) );
    ce->setFile( fileName );
    
    m_lastDir = fileName;
    int index = m_docWidget->currentIndex();
    m_fileList.replace( index, fileName );
    m_docWidget->setTabText( index, strippedName(fileName) );
    enableFileActs( true );   // enable file actions
    //if( ce->hasDebugger() )
    enableDebugActs( true );

    QSettings* settings = MainWindow::self()->settings();
    QStringList files = settings->value("recentFileList").toStringList();
    files.removeAll( fileName );
    files.prepend( fileName );
    while( files.size() > MaxRecentFiles ) files.removeLast();
    settings->setValue("recentFileList", files );
    updateRecentFileActions();

    QApplication::restoreOverrideCursor();
}

void EditorWindow::reload()
{
    QString fileName = m_fileList.at( m_docWidget->currentIndex() );
    loadFile( fileName );
}

bool EditorWindow::save()
{
    QString file = getCodeEditor()->getFilePath();
    if( file.isEmpty() ) return saveAs();
    else                 return saveFile( file );
}

bool EditorWindow::saveAs()
{
    CodeEditor* ce = getCodeEditor();

    QFileInfo fi = QFileInfo( ce->getFilePath() );
    QString ext  = fi.suffix();
    QString path = fi.absolutePath();
    if( path == "" ) path = m_lastDir;

    QString extensions = "";
    if( ext == "" ) extensions = tr("All files")+" (*);;Arduino (*.ino);;Asm (*.asm);;GcBasic (*.gcb)";
    else            extensions = "."+ext+"(*."+ext+");;"+tr("All files")+" (*.*)";

    QString fileName = QFileDialog::getSaveFileName( this, tr("Save Document As"), path, extensions );
    if( fileName.isEmpty() ) return false;

    m_fileList.replace( m_docWidget->currentIndex(), fileName );

    return saveFile( fileName );
}

bool EditorWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if( !file.open(QFile::WriteOnly | QFile::Text) )
    {
        QMessageBox::warning(this, "EditorWindow::saveFile",
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    QApplication::setOverrideCursor(Qt::WaitCursor);
    CodeEditor* ce = getCodeEditor();
    out << ce->toPlainText();
    ce->setFile( fileName );
    QApplication::restoreOverrideCursor();
    
    ce->document()->setModified( false );
    documentWasModified();

    m_docWidget->setTabText( m_docWidget->currentIndex(), strippedName(fileName) );
    return true;
}

bool EditorWindow::maybeSave()
{
    if( m_fileList.isEmpty() ) return true;
    if( getCodeEditor()->document()->isModified() )
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning( this, "EditorWindow::saveFile",
              tr("\nThe Document has been modified.\nDo you want to save your changes?\n"),
              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
              
        if     ( ret == QMessageBox::Save )   return save();
        else if( ret == QMessageBox::Cancel ) return false;
    }
    return true;
}

void EditorWindow::documentWasModified()
{
    CodeEditor* ce = getCodeEditor();
    QTextDocument *doc = ce->document();

    bool    modified = doc->isModified();
    int     index    = m_docWidget->currentIndex();
    QString tabText  = m_docWidget->tabText( index );

    if     ( modified && !tabText.endsWith("*") ) tabText.append("*");
    else if( !modified && tabText.endsWith("*") ) tabText.remove("*");

    m_docWidget->setTabText( index, tabText );

    redoAct->setEnabled( false );
    undoAct->setEnabled( false );
    if( doc->isRedoAvailable() ) redoAct->setEnabled( true );
    if( doc->isUndoAvailable() ) undoAct->setEnabled( true );
    
    ce->setCompiled( false );
}

void EditorWindow::enableFileActs( bool enable )
{
    saveAct->setEnabled( enable );
    saveAsAct->setEnabled( enable );
    cutAct->setEnabled( enable );
    copyAct->setEnabled( enable );
    pasteAct->setEnabled( enable );
    undoAct->setEnabled( enable );
    redoAct->setEnabled( enable );
    findQtAct->setEnabled( enable );
}

void EditorWindow::enableDebugActs( bool enable )
{
    debugAct->setEnabled( enable );
    runAct->setEnabled( enable );
    stepAct->setEnabled( enable );
    stepOverAct->setEnabled( enable );
    pauseAct->setEnabled( enable );
    resetAct->setEnabled( enable );
    stopAct->setEnabled( enable );
    compileAct->setEnabled( enable );
    loadAct->setEnabled( enable );
}

void EditorWindow::tabContextMenu( const QPoint &eventpoint )
{
    CodeEditor* ce = getCodeEditor();
    if( !ce ) return;
    
    QMenu* menu = new QMenu();
    QAction* setCompilerAction = menu->addAction(QIcon(":/copy.png"),tr("Set Compiler Path"));
    connect( setCompilerAction, SIGNAL( triggered()), this, SLOT(setCompiler()), Qt::UniqueConnection );

    QAction* reloadAction = menu->addAction(QIcon(":/reload.png"),tr("Reload"));
    connect( reloadAction, SIGNAL( triggered()), this, SLOT(reload()), Qt::UniqueConnection );

    menu->exec( mapToGlobal(eventpoint) );
    menu->deleteLater();
}

void EditorWindow::tabChanged( int tab )
{
    //qDebug() << "EditorWindow::tabChanged" << m_docWidget->currentIndex() << tab;
}

void EditorWindow::setCompiler()
{
    CodeEditor* ce = getCodeEditor();
    if( ce ) ce->setCompilerPath();
}

void EditorWindow::createWidgets()
{
    baseWidgetLayout = new QGridLayout( this );
    baseWidgetLayout->setSpacing(0);
    baseWidgetLayout->setContentsMargins(0, 0, 0, 0);
    baseWidgetLayout->setObjectName("gridLayout");

    m_editorToolBar = new QToolBar( this );
    baseWidgetLayout->addWidget( m_editorToolBar );
    
    m_debuggerToolBar = new QToolBar( this );
    m_debuggerToolBar->setVisible( false );
    baseWidgetLayout->addWidget( m_debuggerToolBar );

    QSplitter* splitter0 = new QSplitter( this );
    splitter0->setObjectName("splitter0");
    splitter0->setOrientation( Qt::Vertical );
    baseWidgetLayout->addWidget( splitter0 );
    
    m_docWidget = new QTabWidget( this );
    m_docWidget->setObjectName("docWidget");
    m_docWidget->setTabPosition( QTabWidget::North );
    m_docWidget->setTabsClosable ( true );
    m_docWidget->setContextMenuPolicy( Qt::CustomContextMenu );
    double fontScale = MainWindow::self()->fontScale();
    QString fontSize = QString::number( int(10*fontScale) );
    m_docWidget->tabBar()->setStyleSheet("QTabBar { font-size:"+fontSize+"px; }");
    //m_docWidget->setMovable( true );
    ///baseWidgetLayout->addWidget( m_docWidget );
    splitter0->addWidget( m_docWidget );

    splitter0->addWidget( &m_outPane );
    splitter0->setSizes( {300, 100} );
    
    connect( m_docWidget, SIGNAL( tabCloseRequested(int)), 
             this,        SLOT(   closeTab(int)), Qt::UniqueConnection);
             
    connect( m_docWidget, SIGNAL( customContextMenuRequested(const QPoint &)), 
             this,        SLOT(   tabContextMenu(const QPoint &)), Qt::UniqueConnection);
                        
    connect( m_docWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)), Qt::UniqueConnection);
    
    setLayout( baseWidgetLayout );
    
    findRepDiaWidget = new FindReplaceDialog(this);
    findRepDiaWidget->setModal(false);
}

void EditorWindow::createActions()
{
    for( int i=0; i<MaxRecentFiles; i++ )
    {
        recentFileActs[i] = new QAction( this );
        recentFileActs[i]->setVisible( false );
        connect( recentFileActs[i], SIGNAL( triggered() ),
                 this,              SLOT( openRecentFile() ), Qt::UniqueConnection);
    }

    newAct = new QAction(QIcon(":/new.png"), tr("&New\tCtrl+N"), this);
    newAct->setStatusTip(tr("Create a new file"));
    connect( newAct, SIGNAL(triggered()), this, SLOT(newFile()), Qt::UniqueConnection);

    openAct = new QAction(QIcon(":/open.png"), tr("&Open...\tCtrl+O"), this);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()), Qt::UniqueConnection);

    saveAct = new QAction(QIcon(":/save.png"), tr("&Save\tCtrl+S"), this);
    saveAct->setStatusTip(tr("Save the document to disk"));
    saveAct->setEnabled(false);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()), Qt::UniqueConnection);

    saveAsAct = new QAction(QIcon(":/saveas.png"),tr("Save &As...\tCtrl+Shift+S"), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    saveAsAct->setEnabled(false);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()), Qt::UniqueConnection);

    exitAct = new QAction(QIcon(":/exit.png"),tr("E&xit"), this);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()), Qt::UniqueConnection);

    cutAct = new QAction(QIcon(":/cut.png"), tr("Cu&t\tCtrl+X"), this);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    cutAct->setEnabled(false);
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()), Qt::UniqueConnection);

    copyAct = new QAction(QIcon(":/copy.png"), tr("&Copy\tCtrl+C"), this);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    copyAct->setEnabled(false);
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()), Qt::UniqueConnection);

    pasteAct = new QAction(QIcon(":/paste.png"), tr("&Paste\tCtrl+V"), this);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    pasteAct->setEnabled(false);
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()), Qt::UniqueConnection);

    undoAct = new QAction(QIcon(":/undo.png"), tr("Undo\tCtrl+Z"), this);
    undoAct->setStatusTip(tr("Undo the last action"));
    undoAct->setEnabled(false);
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()), Qt::UniqueConnection);

    redoAct = new QAction(QIcon(":/redo.png"), tr("Redo\tCtrl+Shift+Z"), this);
    redoAct->setStatusTip(tr("Redo the last action"));
    redoAct->setEnabled(false);
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

    runAct =  new QAction(QIcon(":/runtobk.png"),tr("Run To Breakpoint"), this);
    runAct->setStatusTip(tr("Run to next breakpoint"));
    runAct->setEnabled(false);
    connect(runAct, SIGNAL(triggered()), this, SLOT(run()), Qt::UniqueConnection);

    stepAct = new QAction(QIcon(":/step.png"),tr("Step"), this);
    stepAct->setStatusTip(tr("Step debugger"));
    stepAct->setEnabled(false);
    connect( stepAct, SIGNAL(triggered()), this, SLOT(step()), Qt::UniqueConnection );

    stepOverAct = new QAction(QIcon(":/rotateCW.png"),tr("StepOver"), this);
    stepOverAct->setStatusTip(tr("Step Over"));
    stepOverAct->setEnabled(false);
    stepOverAct->setVisible(false);
    connect( stepOverAct, SIGNAL(triggered()), this, SLOT(stepOver()), Qt::UniqueConnection );

    pauseAct = new QAction(QIcon(":/pause.png"),tr("Pause"), this);
    pauseAct->setStatusTip(tr("Pause debugger"));
    pauseAct->setEnabled(false);
    connect( pauseAct, SIGNAL(triggered()), this, SLOT(pause()), Qt::UniqueConnection );

    resetAct = new QAction(QIcon(":/reset.png"),tr("Reset"), this);
    resetAct->setStatusTip(tr("Reset debugger"));
    resetAct->setEnabled(false);
    connect( resetAct, SIGNAL(triggered()), this, SLOT(reset()), Qt::UniqueConnection );

    stopAct = new QAction(QIcon(":/stop.png"),tr("Stop Debugger"), this);
    stopAct->setStatusTip(tr("Stop debugger"));
    stopAct->setEnabled(false);
    connect( stopAct, SIGNAL(triggered()), this, SLOT(stop()), Qt::UniqueConnection );

    compileAct = new QAction(QIcon(":/compile.png"),tr("Compile"), this);
    compileAct->setStatusTip(tr("Compile Source"));
    compileAct->setEnabled(false);
    connect( compileAct, SIGNAL(triggered()), this, SLOT(compile()), Qt::UniqueConnection );
    
    loadAct = new QAction(QIcon(":/load.png"),tr("UpLoad"), this);
    loadAct->setStatusTip(tr("Load Firmware"));
    loadAct->setEnabled(false);
    connect( loadAct, SIGNAL(triggered()), this, SLOT(upload()), Qt::UniqueConnection );

    findQtAct = new QAction(QIcon(":/find.png"),tr("Find Replace"), this);
    findQtAct->setStatusTip(tr("Find Replace"));
    findQtAct->setEnabled(false);
    connect(findQtAct, SIGNAL(triggered()), this, SLOT(findReplaceDialog()), Qt::UniqueConnection);
    
    debugAct =  new QAction(QIcon(":/play.png"),tr("Debug"), this);
    debugAct->setStatusTip(tr("Start Debugger"));
    debugAct->setEnabled(false);
    connect(debugAct, SIGNAL(triggered()), this, SLOT(debug()), Qt::UniqueConnection);
}

void EditorWindow::enableStepOver( bool en )
{
    stepOverAct->setVisible( en );
}

CodeEditor* EditorWindow::getCodeEditor()
{
    return (CodeEditor*)m_docWidget->currentWidget();
}

void EditorWindow::closeTab( int index )
{
    m_docWidget->setCurrentIndex( index );
    if( !maybeSave() ) return;

    m_fileList.removeAt(index);

    if( m_fileList.isEmpty() )  // disable file actions
    {
        enableFileActs( false );
        enableDebugActs( false );
    }
    if( m_debuggerToolBar->isVisible() ) stop();

    CodeEditor* doc = (CodeEditor*)m_docWidget->currentWidget();
    m_docWidget->removeTab( index );
    delete doc;

    int last = m_docWidget->count()-1;
    if( index > last ) m_docWidget->setCurrentIndex( last );
    else               m_docWidget->setCurrentIndex( index );
}

void EditorWindow::cut()     { getCodeEditor()->cut(); }
void EditorWindow::copy()    { getCodeEditor()->copy(); }
void EditorWindow::paste()   { getCodeEditor()->paste(); }
void EditorWindow::undo()    { getCodeEditor()->undo(); }
void EditorWindow::redo()    { getCodeEditor()->redo(); }

void EditorWindow::debug()    
{ 
    CodeEditor* ce = getCodeEditor();

    if( ce->initDebbuger() )
    {
        m_editorToolBar->setVisible( false);
        m_debuggerToolBar->setVisible( true );

        runAct->setEnabled( true );
        stepAct->setEnabled( true );
        stepOverAct->setEnabled( true );
        resetAct->setEnabled( true );
        pauseAct->setEnabled( false );

        Simulator::self()->addToUpdateList( &m_outPane );
    }
}

void EditorWindow::run()
{ 
    setStepActs();
    QTimer::singleShot( 10, getCodeEditor(), SLOT( runToBreak() ) );
}

void EditorWindow::step()    
{ 
    setStepActs();
    QTimer::singleShot( 10, getCodeEditor(), SLOT( step()) );
}

void EditorWindow::stepOver()
{
    setStepActs();
    QTimer::singleShot( 10, getCodeEditor(), SLOT( stepOver()) );
}

void EditorWindow::setStepActs()
{ 
    runAct->setEnabled( false );
    stepAct->setEnabled( false );
    stepOverAct->setEnabled( false );
    resetAct->setEnabled( false );
    pauseAct->setEnabled( true );
}

void EditorWindow::pause()   
{
    getCodeEditor()->pause();
    runAct->setEnabled( true );
    stepAct->setEnabled( true );
    stepOverAct->setEnabled( true );
    resetAct->setEnabled( true );
    pauseAct->setEnabled( false );
}
void EditorWindow::reset()   
{ 
    getCodeEditor()->reset();
}

void EditorWindow::stop()    
{ 
    getCodeEditor()->stopDebbuger();
    m_debuggerToolBar->setVisible( false );
    m_editorToolBar->setVisible( true);

    Simulator::self()->remFromUpdateList( &m_outPane );
}

void EditorWindow::compile() 
{ 
    getCodeEditor()->compile();
    /// m_compiler.compile( getCodeEditor()->getFilePath() );
}

void EditorWindow::upload()  
{ 
    getCodeEditor()->upload();
}

void EditorWindow::findReplaceDialog() 
{
    CodeEditor* ce = getCodeEditor();
    
    findRepDiaWidget->setTextEdit( ce );
    
    QString text =ce->textCursor().selectedText();
    if( text != "" ) findRepDiaWidget->setTextToFind( text );

    findRepDiaWidget->show(); 
}

void EditorWindow::createToolBars()
{
    for( int i=0; i<MaxRecentFiles; i++ ) m_fileMenu.addAction( recentFileActs[i] );
    QToolButton* fileButton = new QToolButton( this );
    fileButton->setStatusTip( tr("Last Circuits") );
    fileButton->setMenu( &m_fileMenu );
    fileButton->setIcon( QIcon(":/lastfiles.png") );
    fileButton->setPopupMode( QToolButton::InstantPopup );
    m_editorToolBar->addWidget( fileButton );

    m_editorToolBar->addAction(newAct);
    m_editorToolBar->addAction(openAct);
    m_editorToolBar->addAction(saveAct);
    m_editorToolBar->addAction(saveAsAct);
    m_editorToolBar->addSeparator();

    m_editorToolBar->addAction(findQtAct);
    m_editorToolBar->addSeparator();
    
    m_editorToolBar->addAction(compileAct);
    m_editorToolBar->addAction(loadAct);
    m_editorToolBar->addSeparator();
    
    m_editorToolBar->addAction(debugAct);
    
    m_debuggerToolBar->addAction(stepAct);
    m_debuggerToolBar->addAction(stepOverAct);
    m_debuggerToolBar->addAction(runAct);
    m_debuggerToolBar->addAction(pauseAct);
    m_debuggerToolBar->addAction(resetAct);
    m_debuggerToolBar->addSeparator();
    m_debuggerToolBar->addAction(stopAct);
}

void EditorWindow::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    if( action ) loadFile( action->data().toString() );
}

void EditorWindow::updateRecentFileActions()
{
    QSettings* settings = MainWindow::self()->settings();
    QStringList files = settings->value("recentFileList").toStringList();

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

void EditorWindow::readSettings()
{
    QSettings* settings = MainWindow::self()->settings();
    restoreGeometry( settings->value("geometry").toByteArray() );
    m_docWidget->restoreGeometry( settings->value("docWidget/geometry").toByteArray() );
    m_lastDir = settings->value("lastDir").toString();
}

void EditorWindow::writeSettings()
{
    QSettings* settings = MainWindow::self()->settings();
    settings->setValue( "geometry", saveGeometry() );
    settings->setValue( "docWidget/geometry", m_docWidget->saveGeometry() );
    settings->setValue( "lastDir", m_lastDir );
}

QString EditorWindow::strippedName( const QString &fullFileName )
{
    return QFileInfo( fullFileName ).fileName();
}

#include  "moc_editorwindow.cpp"
