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

#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QtGui>
#include "findreplacedialog.h"
#include "compiler.h"

class EditorWindow : public QWidget
{
    Q_OBJECT

    public:
        EditorWindow( QWidget *parent );
        ~EditorWindow();
        
 static EditorWindow* self() { return m_pSelf; }

        bool close();
        
        void enableStepOver( bool en );

        CodeEditor* getCodeEditor();

    public slots:
        void loadFile(const QString &fileName);
        void pause();
        void stop();
        void run();
        bool save();
        
    private slots:
        void openRecentFile();
        void newFile();
        void open();
        bool saveAs();
        void closeTab(int);
        void documentWasModified();
        void tabContextMenu( const QPoint & eventpoint );
        void tabChanged( int tab );
        void setCompiler();
        void reload();

        void cut();
        void copy();
        void paste();
        void undo();
        void redo();
        
        void debug();
        void step();
        void stepOver();
        void reset();
        void compile();
        void upload();
        void findReplaceDialog();

    protected:
        void dropEvent( QDropEvent* event );
        void dragEnterEvent( QDragEnterEvent* event);

    private:
 static EditorWindow*  m_pSelf;

        enum { MaxRecentFiles = 10 };
        void updateRecentFileActions();
        void createWidgets();
        void createActions();
        void createToolBars();
        void readSettings();
        void writeSettings();
        void enableFileActs( bool enable );
        void enableDebugActs( bool enable );
        void setStepActs();
        void keyPressEvent( QKeyEvent* event );

        bool maybeSave();
        bool saveFile( const QString &fileName );
        
        QString strippedName( const QString &fullFileName );
        
        QGridLayout* baseWidgetLayout;
        QTabWidget*  m_docWidget;

        QMenu        m_fileMenu;
        OutPanelText m_outPane;
        Compiler     m_compiler;

        
        FindReplaceDialog* findRepDiaWidget;
        
        QString     m_lastDir;
        QStringList m_fileList;

        QToolBar* m_editorToolBar;
        QToolBar* m_debuggerToolBar;

        QAction* recentFileActs[MaxRecentFiles];
        QAction* newAct;
        QAction* openAct;
        QAction* saveAct;
        QAction* saveAsAct;
        QAction* exitAct;
        QAction* aboutAct;
        QAction* aboutQtAct;
        QAction* undoAct;
        QAction* redoAct;

        QAction* cutAct;
        QAction* copyAct;
        QAction* pasteAct;
        
        QAction* debugAct;
        
        QAction* stepAct;
        QAction* stepOverAct;
        QAction* runAct;
        QAction* pauseAct;
        QAction* resetAct;
        QAction* stopAct;
        QAction* compileAct;
        QAction* loadAct;
        QAction* findQtAct;
};

#endif
