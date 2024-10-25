/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUMEMBLOCK_H
#define MCUMEMBLOCK_H

#include "memory.h"
//#include "mcutypes.h"

class McuMemBlock : public Memory
{
    public:
        McuMemBlock( uint32_t addrStart, uint32_t addrEnd );
        ~McuMemBlock();

        virtual uint8_t  read_08( uint32_t addr ) { return Memory::read_08( mappedAddr( addr ) ); }
        virtual uint16_t read_16( uint32_t addr ) { return Memory::read_16( mappedAddr( addr ) ); }
        virtual uint32_t read_32( uint32_t addr ) { return Memory::read_32( mappedAddr( addr ) ); }

        virtual void write_08( uint32_t addr, uint32_t val ) { Memory::write_08( mappedAddr( addr ), val ); }
        virtual void write_16( uint32_t addr, uint32_t val ) { Memory::write_16( mappedAddr( addr ), val ); }
        virtual void write_32( uint32_t addr, uint32_t val ) { Memory::write_32( mappedAddr( addr ), val ); }

        bool contains( uint32_t addr ) { return addr >= m_addrStart && addr <= m_addrEnd; }

        uint32_t mappedAddr( uint32_t addr ) { return addr-m_addrStart; }

    protected:

        uint32_t m_addrStart;
        uint32_t m_addrEnd;
        //uint32_t m_size;
};

class InvalidBlock : public McuMemBlock
{
    public:
         InvalidBlock() : McuMemBlock( 0, 0 ){;}
         ~InvalidBlock(){;}

        virtual uint8_t  read_08( uint32_t ) override { return 0; }
        virtual uint16_t read_16( uint32_t ) override { return 0; }
        virtual uint32_t read_32( uint32_t ) override { return 0; }

        virtual void write_08( uint32_t, uint32_t ) override { ; }
        virtual void write_16( uint32_t, uint32_t ) override { ; }
        virtual void write_32( uint32_t, uint32_t ) override { ; }
};
#endif
