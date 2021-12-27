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

#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QMenu>
#include <QSettings>
#include <QContextMenuEvent>

#include "filewidget.h"
#include "mainwindow.h"

FileWidget* FileWidget::m_pSelf = 0l;

FileWidget::FileWidget( QWidget* parent )
          : QSplitter( parent )
{
    m_pSelf = this;
    m_blocked = false;
    
    this->setOrientation( Qt::Vertical );
    
    m_bookmarks = new QListWidget( this );
    this->addWidget( m_bookmarks );

    QWidget* pathWidget = new QWidget( this );
    QHBoxLayout* hLayout = new QHBoxLayout( pathWidget );
    hLayout->setContentsMargins(0,0,0,0);
    m_cdUpButton = new QPushButton( this );
    m_cdUpButton->setIcon( QIcon(":/cdup.png") );
    m_cdUpButton->setToolTip( tr("cd Up") );
    hLayout->addWidget( m_cdUpButton );
    m_path = new QLineEdit( this );
    hLayout->addWidget( m_path );
    pathWidget->setFixedHeight( 24 );
    this->addWidget( pathWidget );

    m_searchFiles = new QLineEdit( this );
    m_searchFiles->setPlaceholderText( tr( "Search Files (Doesn't work)" ));
    m_searchFiles->setFixedHeight( 24 );
    this->addWidget( m_searchFiles );
    
    m_fileBrowser = new FileBrowser( this );
    m_fileBrowser->setPath( QDir::rootPath() );
    this->addWidget( m_fileBrowser );

    this->setSizes( {200, 24, 24, 300} );
    
    QSettings* settings = MainWindow::self()->settings();
    QDir setDir( settings->fileName() );
    setDir.cdUp( );
    QString settingsDir = setDir.absolutePath() ;
    
    addEntry( "FileSystem", QDir::rootPath() );
    addEntry( "Home",       QDir::homePath() );
    addEntry( "Examples",   MainWindow::self()->getFilePath("examples") );
    addEntry( "Data",       MainWindow::self()->getFilePath("data") );
    addEntry( "Settings",   settingsDir );

    connect( m_bookmarks, SIGNAL( itemClicked( QListWidgetItem* )), 
             this,        SLOT(   itemClicked( QListWidgetItem* )), Qt::UniqueConnection);
             
    connect( m_searchFiles, SIGNAL( editingFinished() ),
             this,          SLOT( searchChanged() ), Qt::UniqueConnection);

    connect( m_cdUpButton,  SIGNAL(released()),
             m_fileBrowser, SLOT(  cdUp()), Qt::UniqueConnection);
             
    connect( m_path, SIGNAL( editingFinished() ),
             this,   SLOT(  pathChanged()), Qt::UniqueConnection);
             
    int size = settings->beginReadArray("bookmarks");
    
    for( int i=0; i<size; i++ ) 
    {
        settings->setArrayIndex(i);
        addBookMark( settings->value("path").toString() );
    }
    settings->endArray();
}
FileWidget::~FileWidget(){}

void FileWidget::writeSettings()
{
    QSettings* settings = MainWindow::self()->settings();
    settings->beginWriteArray("bookmarks");
    
    for( int i=0; i<m_bookmarkList.size(); i++ ) 
    {
        settings->setArrayIndex(i);
        settings->setValue("path", m_bookmarkList.at(i) );
    }
    settings->endArray(); 
    
    while( m_bookmarks->count() ) delete m_bookmarks->takeItem( 0 );
}

void FileWidget::addEntry( QString name, QString path )
{
    QListWidgetItem* item = new QListWidgetItem( name, m_bookmarks, 0 );
    item->setData( 4, path );
    
    QFont font;
    font.setPixelSize( 11*MainWindow::self()->fontScale() );
    font.setWeight(70);
    item->setFont( font );
    item->setIcon( QIcon(":/open.png") );
}

void FileWidget::addBookMark( QString path )
{
    QDir dir = QDir( path );
    QString dirPath = dir.absolutePath();
    QString name    = dir.dirName()+" ("+dirPath+")";

    if( !m_bookmarkList.contains( dirPath ) )
    {
        m_bookmarkList.append( dirPath );
        addEntry( name, dirPath );
}   }

void FileWidget::remBookMark()
{
    QListWidgetItem* item =  m_bookmarks->takeItem( m_bookmarks->currentRow() );
    m_bookmarkList.removeOne( item->data( 4 ).toString() );
    delete item;
}

void FileWidget::itemClicked( QListWidgetItem* item  )
{
    QString path = item->data( 4 ).toString();
    m_fileBrowser->setPath( path );
}

void FileWidget::searchChanged()
{
    QString filter = m_searchFiles->text();
    m_fileBrowser->searchFiles( filter );
}

void FileWidget::setPath( QString path )
{
    m_blocked = true;
    m_path->setText( path );
    m_blocked = false;
}

void FileWidget::pathChanged()
{
    if( m_blocked ) return;        // Avoid circular call with setPath()
    
    QString path = m_path->text();
    m_fileBrowser->setPath( path );
}

void FileWidget::contextMenuEvent( QContextMenuEvent* event )
{
    QListWidgetItem* item =  m_bookmarks->currentItem();
    
    if( m_bookmarkList.contains( item->data( 4 ).toString() ) )
    {
        QPoint eventPos = event->globalPos();
        QMenu menu;

        QAction* remBookMarkAction = menu.addAction(QIcon(":/remove.png"),tr("Remove Bookmark"));
        connect( remBookMarkAction, SIGNAL( triggered()), 
                 this,              SLOT(   remBookMark() ) );
                 
        menu.exec( eventPos );
}   }
