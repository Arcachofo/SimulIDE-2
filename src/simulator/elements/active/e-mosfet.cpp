/***************************************************************************
 *   Copyright (C) 2016 by santiago González                               *
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

#include <math.h>   // fabs(x,y)
//#include <QDebug>

#include "e-mosfet.h"
#include "simulator.h"
#include "e-node.h"


eMosfet::eMosfet( QString id )
       : eResistor( id )
{
    m_Pchannel  = false;
    m_depletion = false;
    
    m_gateV     = 0;
    m_resist    = 1;
    m_RDSon     = 1;
    m_threshold = 3;

    m_ePin.resize(3);
}
eMosfet::~eMosfet(){}

void eMosfet::initialize()
{
    eResistor::setRes( m_RDSon );
    
    m_accuracy = Simulator::self()->NLaccuracy();

    m_gateV = 0;
    m_lastCurrent = 0;
    m_Vs = 0;
    m_Sfollow = false;
    m_converged = true;
    m_firtStage = true;
    
    m_kRDSon = m_RDSon*(10-m_threshold);
    m_Gth    = m_threshold-m_threshold/4;
    //if( m_depletion ) m_Gth = -m_Gth;
}

void eMosfet::stamp()
{
    if( (m_ePin[0]->isConnected())
      &&(m_ePin[1]->isConnected())
      &&(m_ePin[2]->isConnected()) )
    {
        m_ePin[0]->getEnode()->addToNoLinList(this);
        m_ePin[1]->getEnode()->addToNoLinList(this);
        m_ePin[2]->getEnode()->addToNoLinList(this);
    }
    eResistor::stamp();
}

void eMosfet::voltChanged()
{
    double Vd = m_ePin[0]->getVolt();
    double Vs = m_ePin[1]->getVolt();
    double Vg = m_ePin[2]->getVolt();
    double Vgs = Vg-Vs;
    double Vds = Vd-Vs;
    
    if(( m_Sfollow == false)&( fabs(Vs) > 1e-3 ))
    {
        if(( fabs(m_Vs) > 1e-3 )&( m_Vs != Vs )) m_Sfollow = true;
        m_Vs = Vs;
    }
    if( m_Pchannel )
    {
        Vgs = -Vgs;
        Vds = -Vds;
    }
    if( m_depletion ) Vgs = -Vgs;
    
    double gateV = Vgs - m_Gth;
    
    if( gateV < 0 ) gateV = 0;
    //if( m_Sfollow ) gateV = (m_gateV*4 + gateV)/5;

    double admit = cero_doub;
    double current = 0;
    double maxCurrDS = Vds/m_resist;
    double DScurrent = m_accuracy*2;
    
    if( gateV > 0 )
    {
        admit = 1/m_RDSon;
        
        /*if( m_firtStage )
        {
            m_firtStage = false;

            if( (gateV-m_gateV) < 0 ) DScurrent = -DScurrent;
            current = m_lastCurrent+DScurrent;
        }
        else*/
        {
            double satK = 1+Vds/100;
            //double maxCurrDS = Vds/m_resist;

            if( Vds > gateV ) Vds =gateV;

            DScurrent = (gateV*Vds-pow(Vds,2)/2)*satK/m_kRDSon;

            //if( m_Sfollow ) DScurrent /= 2;
            if( DScurrent > maxCurrDS ) DScurrent = maxCurrDS;
            current = maxCurrDS-DScurrent;
        }
    }
    if( m_Pchannel ) current = -current;
    
    if( admit != m_admit )
    {
        eResistor::setAdmit( admit );
        eResistor::stamp();
    }
    m_gateV = gateV;

    if( fabs(current-m_lastCurrent)<m_accuracy )            // Converged
    {
        //m_converged = true;
        m_firtStage = true;
        return;
    }
    m_lastCurrent = current;
    m_ePin[0]->stampCurrent( current );
    m_ePin[1]->stampCurrent(-current );
}

bool eMosfet::pChannel()
{ 
    return m_Pchannel; 
}

void eMosfet::setPchannel( bool pc )
{
    m_Pchannel = pc;
}

bool eMosfet::depletion()
{
    return m_depletion;
}

void eMosfet::setDepletion( bool dep )
{
    m_depletion = dep;
}
        
double eMosfet::RDSon()
{ 
    return m_RDSon;
}
        
void eMosfet::setRDSon( double rdson )
{
    if( rdson < cero_doub ) rdson = cero_doub;
    if( rdson > 1000 ) rdson = 1000;
    m_RDSon = rdson;
}

double eMosfet::threshold()
{ 
    return m_threshold; 
}

void eMosfet::setThreshold( double th )
{ 
    m_threshold = th; 
    m_kRDSon = m_RDSon*(10-m_threshold);
    m_Gth = m_threshold-m_threshold/4;
}

