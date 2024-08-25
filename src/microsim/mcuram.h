/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCURAM_H
#define MCURAM_H

#include <QMap>
#include <vector>

#include "mcumodule.h"
#include "memory.h"
#include "mcutypes.h"
//#include "ramtable.h"

class Watcher;

class McuRam : public McuModule, public Memory
{
    friend class McuCreator;

    public:
        McuRam( eMcu* mcu, QString name );
        ~McuRam();

        virtual void setup() override {;}
        void initialize();

        uint32_t getRamValue( uint32_t address );
        void     setRamValue( uint32_t address, uint32_t value );

        uint32_t getMapperAddr( uint32_t addr ) { return m_addrMap[addr]; } // Get mapped addresses in Data space
        uint32_t getRegAddress( QString reg );                              // Get Reg address by name
        uint32_t*  getReg( QString reg );                                   // Get pointer to Reg data by name
        bool     regExist( QString reg ) { return m_regInfo.contains( reg ); }
        uint32_t  readReg( uint32_t addr );                                 // Read Register (call watchers)
        void     writeReg( uint32_t addr, uint32_t v, bool masked=true );   // Write Register (call watchers)

        Watcher* getWatcher() { return m_watcher; }
        void createWatcher();

        QStringList registerList() { return m_regInfo.keys(); }
        QMap<QString, uint32_t>*  bitMasks()  { return &m_bitMasks; }
        QMap<QString, uint32_t>*  bitRegs()   { return &m_bitRegs; }
        QMap<QString, regInfo_t>* regInfo()   { return &m_regInfo; }

        QMap<uint32_t, McuSignal*>* readSignals() { return &m_readSignals; }
        QMap<uint32_t, McuSignal*>* writeSignals(){ return &m_writeSignals; }

        uint32_t regStart() { return m_regStart; }
        uint32_t regEnd()   { return m_regEnd; }
        uint32_t sregAddr() { return m_sregAddr; }

        void setStatusBits( QStringList bits ) { m_statusBits = bits; }
        QStringList getStatusBits() { return m_statusBits; }

        bool isCpuRead() { return m_isCpuRead; }

        uint32_t m_regOverride;                   // Register value is overriden at write time

    protected:
        uint32_t m_regStart;                      // First address of SFR section
        uint32_t m_regEnd;                        // Last  address of SFR Section

        bool m_isCpuRead;

        //std::vector<uint8_t>  m_dataMem;        // Whole Ram space including Registers
        std::vector<uint32_t> m_addrMap;          // Maps addresses in Data space
        std::vector<uint32_t> m_regMask;          // Registers Write mask

        QMap<QString, regInfo_t>   m_regInfo;     // Access Reg Info by  Reg name
        QMap<uint32_t, McuSignal*> m_readSignals; // Access read Reg Signals by Reg address
        QMap<uint32_t, McuSignal*> m_writeSignals;// Access write Reg Signals by Reg address
        QMap<QString, uint32_t>    m_bitMasks;    // Access Bit mask by bit name
        QMap<QString, uint32_t>    m_bitRegs;     // Access Reg. address by bit name

        uint32_t m_sregAddr;                      // STATUS Reg Address
        QStringList m_statusBits;

        Watcher* m_watcher;
};

#endif
