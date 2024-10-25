/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "avradc.h"
#include "avrtimer.h"
#include "mcuocunit.h"
#include "mcuicunit.h"
#include "mcucomparator.h"
#include "mcupin.h"
#include "e_mcu.h"
#include "mcuregister.h"
#include "mcuram.h"

AvrAdc* AvrAdc::createAdc( eMcu* mcu, QString name, int type )
{
    switch ( type ){
        case 00: return new AvrAdc00( mcu, name ); break;
        case 02: return new AvrAdc02( mcu, name ); break;
        case 03: return new AvrAdc03( mcu, name ); break;
        case 04: return new AvrAdc04( mcu, name ); break;
        case 10: return new AvrAdc10( mcu, name ); break;
        case 11: return new AvrAdc11( mcu, name ); break;
        case 20: return new AvrAdc20( mcu, name ); break;
        default: return nullptr;
}   }

AvrAdc::AvrAdc( eMcu* mcu, QString name )
      : McuAdc( mcu, name )
{
}
AvrAdc::~AvrAdc(){}

void AvrAdc::setup()
{
    m_ADEN  = m_mcuRam->getRegBits("ADEN");
    m_ADSC  = m_mcuRam->getRegBits("ADSC");
    m_ADATE = m_mcuRam->getRegBits("ADATE");
    m_ADIF  = m_mcuRam->getRegBits("ADIF");
    m_ADPS  = m_mcuRam->getRegBits("ADPS0,ADPS1,ADPS2");

    m_ADTS  = m_mcuRam->getRegBits("ADTS0,ADTS1,ADTS2");
    m_ACME  = m_mcuRam->getRegBits("ACME");

    m_ADLAR = m_mcuRam->getRegBits("ADLAR");
    m_REFS  = m_mcuRam->getRegBits("REFS0,REFS1");

    if( m_mcu->getMcuPort("PORTV") )
    {
        m_aVccPin = m_mcu->getMcuPin("PORTV0");
        m_aRefPin = m_mcu->getMcuPin("PORTV1");
        if( m_aRefPin ) m_aRefPin->setInputImp( 32000 ); // Internal 32K resistor on the AREF pin
    }

    m_int0Ovf = nullptr;
    m_int0OCA = nullptr;
    m_intxOCB = nullptr;
    m_int1Cap = nullptr;

    m_aComp = (McuComp*)m_mcu->getModule("COMP");
    if( m_aComp ) m_compInt = m_aComp->getInterrupt();

    m_intExt0 = m_mcu->interrupts()->getInterrupt("INT0");

    m_vRefN = 0;
}

void AvrAdc::initialize()
{
    m_autoTrigger = false;
    m_freeRunning = false;
    m_acme = false;

    m_trigger = 0;
    m_refSelect = 0;
    m_initCycles = 12;

    if( m_refPin.size() > 0 ) m_pRefPin = m_refPin.at(0);

    McuAdc::initialize();
}

void AvrAdc::configureA() // ADCSRA
{
    bool enabled = m_ADEN.getRegBitsBool();
    if( m_enabled != enabled )
    {
        if( !m_enabled && enabled ) m_initCycles = 12; // First conversion after enabling ADC
        m_enabled = enabled;
        toAdcMux();
    }

    uint8_t prs = m_ADPS.getRegBitsVal();
    m_convTime = m_mcu->psInst()*(13+m_initCycles)*m_prescList[prs];

    m_autoTrigger = m_ADATE.getRegBitsBool();
    if( m_autoTrigger ) autotriggerConf();

    bool convert = m_ADSC.getRegBitsBool();
    if( !m_converting && convert )
    {
        startConversion();

        if( m_initCycles ) // Recalculate m_convTime in case of Free Running
        {
            m_initCycles = 0;
            m_convTime = m_mcu->psInst()*13*m_prescList[prs];
        }
    }
}

void AvrAdc::configureB() // ADCSRB / SFIOR(Atmega32)
{
    m_trigger = m_ADTS.getRegBitsVal();
    if( m_autoTrigger ) autotriggerConf();

    updateAcme();
}

void AvrAdc::updateAcme()
{
    bool acme = m_ACME.getRegBitsBool();
    if( m_acme != acme )
    {
        m_acme = acme;
        toAdcMux();
    }
}

void AvrAdc::toAdcMux() // Connect Comparator with ADC multiplexer
{
    if( !m_aComp ) return;

    bool connect = m_acme && !m_enabled;
    if( connect ) m_aComp->setPinN( m_adcPin[m_channel] );
    else          m_aComp->setPinN( nullptr );
}

void AvrAdc::setChannel() // ADMUX
{
    m_channel    = m_MUX.getRegBitsVal();
    m_leftAdjust = m_ADLAR.getRegBitsBool();
    m_refSelect  = m_REFS.getRegBitsVal();

    updtVref();

    if( !m_aComp ) return;
    if( m_acme && !m_enabled ) m_aComp->setPinN( m_adcPin[m_channel] );
}

void AvrAdc::endConversion()
{
    m_ADSC.clear_08(); // Clear ADSC bit
    if( m_autoTrigger && m_freeRunning ) startConversion();
}

//------------------------------------------------------
//-- AVR ADC Type 0 ------------------------------------

AvrAdc00::AvrAdc00( eMcu* mcu, QString name )
        : AvrAdc( mcu, name )
{
}
AvrAdc00::~AvrAdc00(){}

void AvrAdc00::setup()
{
    AvrAdc::setup();

    m_MUX = m_mcuRam->getRegBits("MUX0,MUX1,MUX2,MUX3");

    McuTimer* timer1 = m_mcu->getTimer("TIMER1");
    m_int1Ovf = timer1->getInterrupt();
    m_intxOCB = timer1->getOcUnit("OCB")->getInterrupt();
    m_int1Cap = timer1->getIcUnit()->getInterrupt();

    McuTimer* timer0 = m_mcu->getTimer("TIMER0");
    m_int0Ovf = timer0->getInterrupt();
    m_int0OCA = timer0->getOcUnit("OCA")->getInterrupt();

    m_fixedVref = 1.1;
}

void AvrAdc00::autotriggerConf()
{
    uint8_t trigger = m_trigger;
    if( !m_autoTrigger ) trigger = 255;

    m_freeRunning = trigger == 0;
    m_compInt->callBack( this, trigger == 1 ); // Analog Comparator
    m_intExt0->callBack( this, trigger == 2 ); // External Interrupt Request 0
    m_int0OCA->callBack( this, trigger == 3 ); // Timer/Counter0 Compare Match A
    m_int0Ovf->callBack( this, trigger == 4 ); // Timer/Counter0 Overflow
    m_intxOCB->callBack( this, trigger == 5 ); // Timer/Counter1 Compare Match B
    m_int1Ovf->callBack( this, trigger == 6 ); // Timer/Counter1 Overflow
    m_int1Cap->callBack( this, trigger == 7 ); // Timer/Counter1 Capture Event
}

void AvrAdc00::updtVref()
{
    double vRefP = 0;

    if( m_refSelect & 1 ) // Connect aRefPin to multiplexer
    {
        if( m_refSelect & 2 ) vRefP = m_fixedVref;
        else                  vRefP = m_aVccPin->getVoltage();
    }
    m_aRefPin->setVoltage( vRefP );

    m_vRefP = m_aRefPin->getVoltage();
}

//------------------------------------------------------
//-- AVR ADC Type 02 -----------------------------------

AvrAdc02::AvrAdc02( eMcu* mcu, QString name )
        : AvrAdc00( mcu, name )
{
}
AvrAdc02::~AvrAdc02(){}

void AvrAdc02::setup()
{
    AvrAdc00::setup();
    m_MUX = m_mcuRam->getRegBits("MUX0,MUX1,MUX2,MUX3,MUX4,MUX5");
}

void AvrAdc02::updtVref()
{
    m_vRefP = m_mcu->vdd();
    switch( m_refSelect ){
        case 1: m_vRefP = m_pRefPin->getVoltage();break; // External voltage reference at PA0 (AREF)
        case 2: m_vRefP = 1.1;                    break; // Internal Vref. 1.1 Volt
}   }

//------------------------------------------------------
//-- AVR ADC Type 03 -----------------------------------

AvrAdc03::AvrAdc03( eMcu* mcu, QString name )
        : AvrAdc00( mcu, name )
{
}
AvrAdc03::~AvrAdc03(){}

void AvrAdc03::setup()
{
    AvrAdc00::setup();
    m_MUX = m_mcuRam->getRegBits("MUX0,MUX1,MUX2,MUX3,MUX4");
    m_fixedVref = 2.6;
}

void AvrAdc03::startConversion()
{
    if( !m_enabled ) return;
    if( m_channel >= 8 /*m_adcPin.size()*/ ) specialConv();
    else                                     McuAdc::startConversion();
}

void AvrAdc03::specialConv()
{
    if     ( m_channel == 30) m_adcValue = 1.22*512/m_vRefP;
    else if( m_channel == 31) m_adcValue = 0;
    else{
        updtVref();

        int chP,chN;
        int gain = 1;

        if( m_channel < 16 )
        {
            switch( m_channel ) {
                case  8: chP = 0; chN = 0; gain = 10;  break;
                case  9: chP = 1; chN = 0; gain = 10;  break;
                case 10: chP = 0; chN = 0; gain = 200; break;
                case 11: chP = 1; chN = 0; gain = 200; break;
                case 12: chP = 2; chN = 2; gain = 10;  break;
                case 13: chP = 3; chN = 2; gain = 10;  break;
                case 14: chP = 2; chN = 2; gain = 200; break;
                case 15: chP = 3; chN = 2; gain = 200; break;
            }
        }else{
            chP = m_channel & 3;
            if( m_channel < 24 ) chN = 1;
            else                 chN = 2;
        }
        double voltP = m_adcPin[chP+m_chOffset]->getVoltage();
        double voltN = m_adcPin[chN+m_chOffset]->getVoltage();
        if( voltP < 0 ) voltP = 0;
        if( voltN < 0 ) voltN = 0;
        m_adcValue = (voltP-voltN)*gain*512/m_vRefP;
    }
}

//------------------------------------------------------
//-- AVR ADC Type 04 -----------------------------------

AvrAdc04::AvrAdc04( eMcu* mcu, QString name )
        : AvrAdc03( mcu, name )
{
    m_MUX5 = m_mcuRam->getRegBits("MUX5");
}
AvrAdc04::~AvrAdc04(){}

void AvrAdc04::setup()
{
    AvrAdc00::setup();
    m_MUX5 = m_mcuRam->getRegBits("MUX5");
    m_fixedVref = 2.6;
}

void AvrAdc04::configureB() // ADCSRB
{
    AvrAdc::configureB();

    bool mux5 = m_MUX5.getRegBitsBool();
    if( mux5 ) m_chOffset = 8;
    else       m_chOffset = 0;
}

//------------------------------------------------------
//-- AVR ADC Type 10 -----------------------------------

AvrAdc10::AvrAdc10( eMcu* mcu, QString name )
        : AvrAdc( mcu, name )
{
}
AvrAdc10::~AvrAdc10(){}

void AvrAdc10::setup()
{
    AvrAdc::setup();

    m_MUX = m_mcuRam->getRegBits("MUX0,MUX1");

    McuTimer* timer0 = m_mcu->getTimer("TIMER0");
    m_int0Ovf = timer0->getInterrupt();
    m_int0OCA = timer0->getOcUnit("OCA")->getInterrupt();
    m_intxOCB = timer0->getOcUnit("OCB")->getInterrupt();

    m_intPinC = m_mcu->interrupts()->getInterrupt("PCINT");
}

void AvrAdc10::autotriggerConf()
{
    uint8_t trigger = m_trigger;
    if( !m_autoTrigger ) trigger = 255;

    m_freeRunning = trigger == 0;
    m_compInt->callBack( this, trigger == 1 ); // Analog Comparator
    m_intExt0->callBack( this, trigger == 2 ); // External Interrupt Request 0
    m_int0OCA->callBack( this, trigger == 3 ); // Timer/Counter0 Compare Match A
    m_int0Ovf->callBack( this, trigger == 4 ); // Timer/Counter0 Overflow
    m_intxOCB->callBack( this, trigger == 5 ); // Timer/Counter0 Compare Match B
    m_intPinC->callBack( this, trigger == 6 ); // Pin Change Interrupt Request
}

void AvrAdc10::updtVref()
{
    if( m_refSelect == 1 ) m_vRefP = 1.1; ///TODO // Internal Vref. = ??? 1.1 Volt
    else                   m_vRefP = m_mcu->vdd();
}

//------------------------------------------------------
//-- AVR ADC Type 11 -----------------------------------

AvrAdc11::AvrAdc11( eMcu* mcu, QString name )
        : AvrAdc10( mcu, name )
{
}
AvrAdc11::~AvrAdc11(){}

void AvrAdc11::setup()
{
    AvrAdc10::setup();

    m_MUX = m_mcuRam->getRegBits("MUX0,MUX1,MUX2,MUX3");
}

void AvrAdc11::updtVref()
{
    m_vRefP = m_mcu->vdd();
    switch( m_refSelect ){
        case 1: m_vRefP = m_pRefPin->getVoltage(); break; // External voltage reference at PB0 (AREF)
        case 2: m_vRefP = 1.1;  break;  // Internal Vref. 1.1 Volt
        case 4:                         // Internal 2.56V Voltage Reference without external capacitor
        case 5: m_vRefP = 2.56; break;  // Internal 2.56V Voltage Reference with external capacitor
}   }

//------------------------------------------------------
//-- AVR ADC Type 20 -----------------------------------

AvrAdc20::AvrAdc20( eMcu* mcu, QString name )
        : AvrAdc( mcu, name )
{
}
AvrAdc20::~AvrAdc20(){}

void AvrAdc20::setup()
{
    AvrAdc::setup();

    m_MUX   = m_mcuRam->getRegBits("MUX0,MUX1,MUX2,MUX3");
    m_ADATE = m_mcuRam->getRegBits("ADFR"); // Same bit, different name: Autotrigger

    m_fixedVref = 2.56;
}

void AvrAdc20::configureB() // SFIOR(Atmega8)
{
    updateAcme();
}

void AvrAdc20::autotriggerConf()
{
    m_freeRunning = m_autoTrigger;
}

void AvrAdc20::updtVref()
{
    double vRefP = 0;

    if( m_refSelect & 1 ) // Connect aRefPin to multiplexer
    {
        if( m_refSelect & 2 ) vRefP = m_fixedVref;
        else                  vRefP = m_aVccPin->getVoltage();
    }
    m_aRefPin->setVoltage( vRefP );

    m_vRefP = m_aRefPin->getVoltage();
}
