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

#ifndef MCUCOMPONENT_H
#define MCUCOMPONENT_H

#include <QtWidgets>

#include "chip.h"
#include "memdata.h"
#include <QtSerialPort/QSerialPort>

class BaseProcessor;
class McuComponentPin;

class MAINMODULE_EXPORT McuComponent : public Chip, public MemData
{
    Q_OBJECT
    Q_PROPERTY( QStringList varList  READ varList  WRITE setVarList )
    Q_PROPERTY( QVector<int> eeprom  READ eeprom   WRITE setEeprom )
    Q_PROPERTY( double   Mhz         READ freq     WRITE setFreq     DESIGNABLE true  USER true )
    Q_PROPERTY( QString  Program     READ program  WRITE setProgram  DESIGNABLE true  USER true )
    Q_PROPERTY( bool     Auto_Load   READ autoLoad WRITE setAutoLoad DESIGNABLE true  USER true )

    public:

        McuComponent( QObject* parent, QString type, QString id );
        ~McuComponent();
        
 static McuComponent* self() { return m_pSelf; }

        virtual QList<propGroup_t> propGroups() override;

        virtual void initialize() override;
        virtual void updateStep() override;
        virtual void attach() override;
        virtual void remove() override;

        virtual void initChip();

        QStringList varList();
        void setVarList( QStringList vl ) { m_varList = vl; }

        QString program()   const { return m_symbolFile; }
        void setProgram( QString pro );

        double freq(){ return m_freq; }
        virtual void setFreq( double freq );

        bool autoLoad() { return m_autoLoad; }
        void setAutoLoad( bool al ) { m_autoLoad = al; }
        
        QString device() { return m_device; }

        void setEeprom(QVector<int> eep );
        QVector<int> eeprom();
        
        QList<McuComponentPin*> getPinList() { return m_pinList; }

        virtual void terminate();
        virtual void reset();
        virtual void load( QString fileName );
        virtual void setSubcDir( QString dir );
        virtual void crash(){;}

        BaseProcessor* processor() { return m_processor; }

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget );

    signals:
        void closeSerials();
        void openSerials();
  

    public slots:
        void slotmain();
        void slotLoad();
        void slotReload();
        void slotOpenTerm();
        void slotOpenSerial();
        void slotOpenRamTable();

        void loadData();
        void saveData();
        void showTable();

    protected:
 static McuComponent* m_pSelf;

        virtual void contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu );
        virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

        virtual void addPin( QString id, QString type, QString label,
                             int pos, int xpos, int ypos, int angle, int length=8 )=0;

        virtual void updatePin( QString id, QString type, QString label,
                                int pos, int xpos, int ypos, int angle );

        virtual void attachPins()=0;

        void createRamTable();

        BaseProcessor* m_processor;

        double m_freq;           // Clock Frequency Mhz
        int m_cpi;
        
        bool m_attached;
        bool m_autoLoad;
        bool m_crashed;

        int m_warning;

        QString m_device;       // Name of device
        QString m_symbolFile;   // firmware file loaded
        QString m_lastFirmDir;  // Last firmware folder used
        QString m_subcDir;      // Subcircuit Path

        QList<McuComponentPin*> m_pinList;
        QStringList m_varList;

        QWidget*     m_ramTabWidget;
        QVBoxLayout* m_ramTabWidgetLayout;
};
#endif
