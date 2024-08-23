/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUDATASPACE_H
#define MCUDATASPACE_H

#include <QHash>
#include <vector>

#include "mcutypes.h"
#include "ramtable.h"

class RamTable;

class DataSpace
{
    public:
        DataSpace();
        ~DataSpace();

        void initialize();

        uint32_t ramSize()  { return m_ramSize; }
        uint8_t  getRamValue( int address );
        void     setRamValue( int address, uint8_t value );
        uint8_t* getRam() { return m_dataMem.data(); }  // Get pointer to Ram data
        uint16_t getMapperAddr( uint16_t addr ) { return m_addrMap[addr]; } // Get mapped addresses in Data space

        uint16_t getRegAddress( QString reg );  // Get Reg address by name
        uint8_t* getReg( QString reg );            // Get pointer to Reg data by name
        bool     regExist( QString reg ) { return m_regInfo.contains( reg ); }
        uint8_t  readReg( uint16_t addr );         // Read Register (call watchers)
        void     writeReg(uint16_t addr, uint8_t v, bool masked=true);// Write Register (call watchers)

        RamTable* getRamTable() { return m_ramTable; }

        QMap<QString, uint8_t>*     bitMasks() { return &m_bitMasks; }
        QMap<QString, uint16_t>*    bitRegs() { return &m_bitRegs; }
        QMap<QString, regInfo_t>*   regInfo()  { return &m_regInfo; }
        QMap<uint16_t, McuSignal*>* readSignals() { return &m_readSignals; }
        QMap<uint16_t, McuSignal*>* writeSignals() { return &m_writeSignals; }

        void setStatusBits( QStringList bits ) { m_statusBits = bits; }
        QStringList getStatusBits() { return m_statusBits; }

        bool isCpuRead() { return m_isCpuRead; }

        int m_regOverride;                        // Register value is overriden at write time

    protected:
        uint16_t m_regStart;                      // First address of SFR section
        uint16_t m_regEnd;                        // Last  address of SFR Section

        bool m_isCpuRead;
        uint32_t m_ramSize;
        std::vector<uint8_t>  m_dataMem;          // Whole Ram space including Registers
        std::vector<uint16_t> m_addrMap;          // Maps addresses in Data space
        std::vector<uint8_t>  m_regMask;          // Registers Write mask

        QMap<QString, regInfo_t>   m_regInfo;     // Access Reg Info by  Reg name
        QMap<uint16_t, McuSignal*> m_readSignals; // Access read Reg Signals by Reg address
        QMap<uint16_t, McuSignal*> m_writeSignals;// Access write Reg Signals by Reg address
        QMap<QString, uint8_t>     m_bitMasks;    // Access Bit mask by bit name
        QMap<QString, uint16_t>    m_bitRegs;     // Access Reg. address by bit name

        uint16_t m_sregAddr;                       // STATUS Reg Address
        QStringList m_statusBits;

        RamTable* m_ramTable;
};

#endif
