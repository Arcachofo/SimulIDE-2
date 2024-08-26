/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef DRAM_H
#define DRAM_H

#include "logiccomponent.h"
#include "memory.h"

class DRAM : public LogicComponent, public Memory
{
    public:
        DRAM( QString type, QString id );
        ~DRAM();

 static listItem_t libraryItem();

        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;
        virtual void runEvent() override;

        int rowAddrBits() { return m_rowAddrBits; }
        int colAddrBits() { return m_colAddrBits; }
        void setRowAddrBits( int bits );
        void setColAddrBits( int bits );
        void setAddrBits( int bits );
        void deleteAddrPins( int bits );
        void createAddrPins( int bits );

        void setDataBits( int bits );
        void deleteDataPins( int bits );
        void createDataPins( int bits );

        double refreshPeriod() { return m_refreshPeriod*1e-12; }
        void setRefreshPeriod( double rp ) { m_refreshPeriod = rp*1e12; }

        void updatePins();

        //void slotShowTable();
        void slotOpenMonitor();

    protected:
        virtual void contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu ) override;

    private:
        int m_refreshPeriod;
        int m_rowAddrBits;
        int m_colAddrBits;
        int m_addrBits;
        int m_intRefresh;
        int m_rowAddress;
        int m_address;
        bool m_refreshError;

        QVector<uint64_t> m_rowLastRefresh;

        bool m_oe;
        bool m_ras;
        bool m_cas;

        IoPin* m_RasPin;
        IoPin* m_CasPin;
        IoPin* m_WePin;
        IoPin* m_OePin;
};

#endif
