/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef SRAM_H
#define SRAM_H

#include "iocomponent.h"
#include "e-element.h"
#include "memory.h"

class SRAM : public IoComponent, public eElement, public Memory
{
    public:
        SRAM( QString type, QString id );
        ~SRAM();

 static listItem_t libraryItem();

        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void voltChanged() override;
        virtual void runEvent() override { IoComponent::runOutputs(); }

        int addrBits() { return m_addrBits; }
        void setAddrBits( int bits );
        void deleteAddrPins( int bits );
        void createAddrPins( int bits );

        void setDataBits( int bits );
        void deleteDataPins( int bits );
        void createDataPins( int bits );

        bool persistent() { return m_persistent; }
        void setPersistent( bool p ) { m_persistent = p; }

        bool asynchro() { return m_asynchro; }
        void setAsynchro( bool a );

        void updatePins();

    public slots:
        void slotShowTable();

    protected:
        virtual void contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu ) override;
        
    private:
        int m_addrBits;
        int m_address;

        bool m_oe;
        bool m_we;
        bool m_cs;
        bool m_persistent;
        bool m_asynchro;

        IoPin* m_CsPin;
        IoPin* m_WePin;
        IoPin* m_OePin;
};

#endif
