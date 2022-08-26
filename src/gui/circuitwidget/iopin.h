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

#ifndef IOPIN_H
#define IOPIN_H

#include <QColor>

#include "pin.h"
#include "e-element.h"
#include "e-node.h"

enum pinMode_t{
    undef_mode=0,
    input,
    openCo,
    output,
    source
};

class eNode;

class MAINMODULE_EXPORT IoPin : public Pin, public eElement
{
    public:
        IoPin( int angle, const QPoint pos, QString id, int index, Component* parent, pinMode_t mode=source );
        ~IoPin();

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void runEvent() override;

        virtual void sheduleState( bool state, uint64_t time );

        //pinMode_t pinMode() { return m_pinMode; }
        void setPinMode( pinMode_t mode );

        void  setInputHighV( double volt ) { m_inpHighV = volt; }
        void  setInputLowV( double volt ) { m_inpLowV = volt; }
        virtual void  setInputImp( double imp );

        double outHighV() { return m_outHighV; }
        void  setOutHighV( double v ) { m_outHighV = v; }

        double outLowV() { return m_outLowV; }
        void  setOutLowV( double v ) { m_outLowV = v; }

        void startLH();
        void startHL();

        virtual void  setOutputImp( double imp );

        //double imp() { return m_imp; }
        virtual void  setImp( double imp );

        virtual bool getInpState();
        virtual bool getOutState() { if( m_step ) return m_nextState; return m_outState; }
        virtual void setOutState( bool high );
        virtual void toggleOutState( uint64_t time=0 ) { sheduleState( !m_outState, time ); }

        inline void setOutStatFast( bool state )
        {
            m_outState = m_nextState = state;
            if( state ){
                m_outVolt = m_outHighV;
                setPinState( out_high ); // High colors
            }else{
                m_outVolt = m_outLowV;
                setPinState( out_low ); // Low colors
            }
            ePin::stampCurrent( m_outVolt*m_admit );
        }

        void setStateZ( bool z );
        virtual void setPullup( bool up );

        virtual void setInverted( bool inverted ) override;

        virtual void controlPin( bool outCtrl , bool dirCtrl ){;}

        void setRiseTime( double time ) { m_timeRis = time; }
        void setFallTime( double time ) { m_timeFal = time; }

    protected:
        void updtState();
        void stampAll();
        inline void stampVolt( double v) { ePin::stampCurrent( v*m_admit ); }
        //inline void stampOutput();

        double m_inpHighV;  // currently in eClockedDevice
        double m_inpLowV;

        double m_outHighV;
        double m_outLowV;
        double m_outVolt;

        double m_vddAdmit;  // Out stage
        double m_gndAdmit;  // Out Stage
        double m_vddAdmEx;  // Extra Source (vref out)
        double m_gndAdmEx;  // Extra Source (vref out)

        double m_inputImp;
        double m_outputImp;
        double m_openImp;
        double m_admit;

        bool m_inpState;
        bool m_outState;
        bool m_stateZ;
        bool m_nextState;

        int m_steps;
        uint64_t m_timeRis;  // Time for Output voltage to switch from 0% to 100%
        uint64_t m_timeFal;  // Time for Output voltage to switch from 100% to 0%

        pinMode_t m_pinMode;

        static eNode m_gndEnode;
};
#endif
