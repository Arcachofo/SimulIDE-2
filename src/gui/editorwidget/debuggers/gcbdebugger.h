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

#ifndef GCBDEBUGGER_H
#define GCBDEBUGGER_H

#include "basedebugger.h"

class GcbDebugger : public BaseDebugger
{
    public:
        GcbDebugger( CodeEditor* parent, OutPanelText* outPane );
        ~GcbDebugger();

    protected:
        virtual bool postProcess() override;
        virtual void getSubs() override;

    private:
        bool mapFlashToSource();
        void getProcType();

        int m_processorType;
};
#endif
