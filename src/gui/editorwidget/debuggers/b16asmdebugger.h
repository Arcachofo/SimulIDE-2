/***************************************************************************
 *   Copyright (C) 2019 by santiago González                               *
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

#ifndef B16ASMDEBUGGER_H
#define B16ASMDEBUGGER_H

#include "basedebugger.h"

class B16AsmDebugger : public BaseDebugger
{
    Q_OBJECT

    public:
        B16AsmDebugger( CodeEditor* parent, OutPanelText* outPane) ;
        ~B16AsmDebugger();
        
        virtual int compile( bool ) override;

    private:
        virtual bool postProcess() override;
        void loadCodes();

        int m_codeBits;

        QHash<QString, int> m_opcodes;
};

#endif
