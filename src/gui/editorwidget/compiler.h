/***************************************************************************
 *   Copyright (C) 2021 by santiago González                               *
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

#include <QString>
#include <QProcess>

class OutPanelText;
class CodeEditor;
class CompilerProp;

class Compiler : public QObject
{
    Q_OBJECT

    public:
        Compiler( CodeEditor* parent, OutPanelText* outPane );
        ~Compiler();

        void clearCompiler();
        void loadCompiler( QString file );
        virtual int compile( bool debug );

        QString compName() { return m_compName; }
        QString fileName() { return m_fileName; }
        QString buildPath() { return m_buildPath; }

        virtual QString toolPath() { return m_toolPath; }
        virtual void setToolPath( QString path );
        void getToolPath();

        QString includePath() { return m_inclPath; }
        void setIncludePath( QString path );
        void getIncludePath();

        QString family() { return m_family; }
        void setFamily( QString f ) { m_family = f; }
        bool useFamily() { return m_useFamily; }

        QString device() { return m_device; }
        void setDevice( QString d ) { m_device = d; }
        bool useDevice() { return m_useDevice; }

        void compProps();

        bool isProjectFile( QString file ) { return m_fileList.contains( file ); }

        void readSettings();

        virtual void getInfoInFile( QString line ){;}

        bool checkCommand( QString c );

        OutPanelText* outPane() { return m_outPane; }

    protected:
        virtual void preProcess(){;}
        virtual bool postProcess(){return false;}

        int getError( QString txt );

        int runBuildStep( QString fullCommand );
        QString replaceData( QString str );
        QString getPath( QString msg );
        void toolChainNotFound();

        CompilerProp* m_compDialog;
        CodeEditor* m_document;

        bool m_useFamily;
        bool m_useDevice;
        bool m_uploadHex;

        QString m_compName;
        QString m_toolPath;
        QString m_inclPath;
        QString m_buildPath;
        QStringList m_command;
        QStringList m_arguments;
        QStringList m_argsDebug;
        QStringList m_fileList;

        QString m_type;
        QString m_family;
        QString m_device;
        QString m_board;
        QString m_firmware;
        QString m_file;
        QString m_fileDir;
        QString m_fileName;
        QString m_fileExt;

        QProcess m_compProcess;

        OutPanelText* m_outPane;
};

#endif
