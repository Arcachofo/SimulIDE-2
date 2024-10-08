/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCU_H
#define MCU_H

//#include <QHash>

#include "e_mcu.h"
#include "chip.h"
#include "linker.h"


/*enum deviceType_t{
    typeNONE=0,
    typeIOU,
    typeMPU,
    typeMCU
};*/

class ScriptCpu;
class McuRam;
class McuRom;
class McuPgm;
class McuIntOsc;
class Monitor;

class Mcu : public Chip, public Linker
{
        friend class McuCreator;

    public:
        Mcu( QString type, QString id );
        ~Mcu();

 static Mcu* self() { return m_pSelf; }
 static Component* construct( QString type, QString id );
 static listItem_t libraryItem();

        void setupMcu();

        virtual bool setPropStr( QString prop, QString val ) override;

        bool mainMcu() { return m_pSelf == this; }
        void setMainMcu( bool m ) { if( m ) slotmain(); }

        QString program() { return m_firmware; }
        void setProgram( QString file );

        bool autoLoad() { return m_autoLoad; }
        void setAutoLoad( bool al ) { m_autoLoad = al; }

        bool savePGM() { return m_savePGM; }
        void setSavePGM( bool s ) { m_savePGM = s; }

        double extFreq() { return m_extFreq; }
        void setExtFreq( double freq ) { m_extFreq = freq; setFreq( freq ); }

        double freq() { return m_eMcu.m_freq; }
        void setFreq( double freq ) { m_eMcu.setFreq( freq ); }

        bool rstPinEnabled();
        void enableRstPin( bool en );

        bool extOscEnabled();
        void enableExtOsc( bool en );

        bool wdtEnabled();
        void enableWdt( bool en );

        bool clockOut();
        void setClockOut( bool clkOut );

        QString varList();
        void setVarList( QString vl );

        QString cpuRegs();
        void setCpuRegs( QString vl );

        void setEeprom( QString eep );
        QString getEeprom();

        bool saveEepr();
        void setSaveEepr( bool s );

        int serialMon();
        void setSerialMon( int s );

        virtual void setIdLabel( QString id ) override;

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;

        QString device() { return m_device; }
        bool isScripted() { return m_scripted; }
        CpuBase* cpu() { return m_eMcu.cpu(); }

        void reset() { m_eMcu.hardReset( true ); }
        void crash( bool c) { m_crashed = c; update(); }

        bool load( QString fileName );

        virtual Pin* addPin( QString id, QString type, QString label,
                             int pos, int xpos, int ypos, int angle , int length=8, int space=0 ) override;

        void setScriptLinker( ScriptCpu* l) { m_scriptLink = l; } // Called from script

        virtual void setLinkedValue( double v, int i=0  ) override;
        virtual void setLinkedString( QString str, int i=0 ) override;

        virtual QStringList getEnumUids( QString prop ) override;
        virtual QStringList getEnumNames( QString prop ) override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

        void slotmain();
        void slotLoad();
        void slotReload();
        void slotOpenTerm( int num );
        void slotOpenMonitor();
        void slotLinkComp();

        void loadEEPROM();
        void saveEEPROM();

    protected:
 static Mcu* m_pSelf;

        QString getPGM();
        void setPGM( QString pgm );

        virtual void contextMenu( QGraphicsSceneContextMenuEvent* e, QMenu* m ) override;

        QString findIdLabel();

        //deviceType_t m_deviceType;
        //bool m_isTQFP;

        bool m_autoLoad;
        bool m_savePGM;
        bool m_scripted;
        bool m_resetPol;

        double m_extFreq;

        int m_serialMon;

        QString m_firmware;     // firmware file loaded
        QString m_lastFirmDir;  // Last firmware folder used
        QString m_device;       // Name of device

        eMcu m_eMcu;

        McuIntOsc* m_intOsc;
        McuRom* m_rom;
        McuPgm* m_pgm;
        McuRam* m_ram;

        IoPin*  m_resetPin;
        McuPin* m_portRstPin;

        ScriptCpu* m_scriptLink;

        Monitor* m_monitor;
};

#endif
