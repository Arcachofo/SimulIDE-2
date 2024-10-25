/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUREGBLOCK_H
#define MCUREGBLOCK_H

//#include <QMap>
//#include <vector>

#include "mcumemblock.h"

class McuRegister;

class McuRegBlock : public McuMemBlock
{
    public:
        McuRegBlock( uint32_t addrStart, uint32_t addrEnd );
        ~McuRegBlock();

        void addRegister( McuRegister* r );

        uint8_t  read_08( uint32_t addr ) override;
        uint16_t read_16( uint32_t addr ) override;
        uint32_t read_32( uint32_t addr ) override;

        void write_08( uint32_t addr, uint32_t val ) override;
        void write_16( uint32_t addr, uint32_t val ) override;
        void write_32( uint32_t addr, uint32_t val ) override;

        McuRegister* getRegister( QString r );
        McuRegister* getRegister( uint32_t addr ) { return m_registers[mappedAddr(addr)]; }

    private:

        std::vector<McuRegister*> m_registers;

        //QMap<QString, regInfo_t>   m_regInfo;     // Access Reg Info by  Reg name
        //QMap<QString, uint32_t>    m_bitMasks;    // Access Bit mask by bit name
        /// QMap<QString, uint32_t>    m_bitRegs;     // Access Reg. address by bit name
};

#endif
