/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUPORT_H
#define MCUPORT_H

#include <vector>

#include "mcumodule.h"
#include "mcupin.h"

class Mcu;
class eMcu;
class McuPin;
class Component;

class McuPort : public McuModule
{
        friend class McuCreator;
        friend class McuPorts;

    public:
        McuPort( eMcu* mcu, QString name );
        ~McuPort();

        virtual void setup() override{;}
        virtual void reset() override;

        virtual void pinChanged( uint8_t pinMask, uint8_t val );

        virtual void puRegChanged() {;}
        virtual void puBitsChanged() {;}
        virtual void puBitsInvChanged() {;}

        void setPullups( uint32_t puMask );

        McuPin* getPinN( uint8_t i );
        McuPin* getPin( QString pinName );

        QString name() { return m_name; }

        virtual void outChanged();
        virtual void dirChanged();

        void intChanged();
        void setIntMask( uint8_t val) { m_intMask = val; }
        void rstIntMask( bool rst) { m_rstIntMask = rst; }

        void readPort();
        virtual uint32_t getInpState();

        void controlPort( bool outCtrl, bool dirCtrl ); // Direct control
        void setDirection( uint val );       // Direct control over pins
        void setOutState( uint val );        // Direct control over pins
        void setPinMode( pinMode_t mode );   // Direct control over pins

 static QStringList registerScript( asIScriptEngine* engine );

    protected:
        void createPins( Mcu* mcuComp, QString pins, uint32_t pinMask );
        virtual McuPin* createPin( int i, QString id , Component* mcu );

        QString m_shortName;

        std::vector<McuPin*> m_pins;
        uint8_t m_numPins;

        regBits_t m_puBits;
        uint8_t*  m_puReg;

        regBits_t m_intBits;
        uint8_t   m_intMask;

        bool m_dirInv;
        bool m_rstIntMask;

        uint8_t m_inpState;
        uint8_t m_outState;
        uint8_t m_dirState;

        uint8_t* m_outReg; // Pointer to PORT Register
        uint8_t* m_inReg;  // Pointer to PIN  Register
        uint8_t* m_dirReg; // Pointer to DIR  Register
};

#endif
