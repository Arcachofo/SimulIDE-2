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

#ifndef COMPILER_H
#define COMPILER_H

#include <QtGui>
#include "outpaneltext.h"
#include "mcucomponent.h"

class CodeEditor;

class Compiler : public QObject    // Base Class for all debuggers
{
        friend class BaseProcessor;

    Q_OBJECT
    Q_PROPERTY( bool    Drive_Circuit READ driveCirc    WRITE setDriveCirc    DESIGNABLE true USER true )
    Q_PROPERTY( QString Compiler_Path READ compilerPath WRITE setCompilerPath DESIGNABLE true USER true )
    
    public:
        Compiler( CodeEditor* parent=0, OutPanelText* outPane=0, QString filePath=0 );
        ~Compiler();
        
        bool driveCirc();
        void setDriveCirc( bool drive );

        QString compilerPath();
        virtual void getCompilerPath();
        virtual void setCompilerPath( QString path );

        virtual bool loadFirmware();
        virtual void upload();
        virtual void stop();

        virtual int  compile()=0;
        virtual void mapFlashToSource()=0;

        virtual int getValidLine( int pc );

        virtual void readSettings();
        virtual QString getVarType( QString var );
        virtual QStringList getVarList();
        virtual QList<int> getSubLines() { return m_subLines; }
        
        int type;
        
    public slots:
        void ProcRead();

    protected:
        void toolChainNotFound();
        virtual void getSubs(){;}
    
        OutPanelText*  m_outPane;

        CodeEditor* m_editor;

 static bool m_loadStatus;                          // Is symbol file loaded?

        int m_processorType;
        int m_lastLine;
        
        //QString m_device;
        QString m_appPath;
        QString m_firmware;
        QString m_file;
        QString m_fileDir;
        QString m_fileName;
        QString m_fileExt;
        QString m_compilerPath;
        QString m_compSetting;
        
        QStringList m_varNames;
        QStringList m_subs;
        QList<int>  m_subLines;
        
        QHash<QString, QString> m_typesList;
        QHash<QString, QString> m_varList;
        QHash<int, int> m_flashToSource;            // Map flash adress to Source code line
        QHash<int, int> m_sourceToFlash;            // Map .asm code line to flash adress
        
        QProcess m_compProcess;
};

#endif

