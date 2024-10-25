/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUMMU_H
#define MCUMMU_H

#include <vector>

#include "mcumodule.h"
//#include "mcutypes.h"
#include "mcuregblock.h"

class McuMmu : public McuModule
{
    public:
        McuMmu( eMcu* mcu, QString name );
        ~McuMmu();

        virtual void setup() override;

        uint8_t  read_08( uint32_t addr );
        uint16_t read_16( uint32_t addr );
        uint32_t read_32( uint32_t addr );

        void write_08( uint32_t addr, uint32_t val );
        void write_16( uint32_t addr, uint32_t val );
        void write_32( uint32_t addr, uint32_t val );

        uint32_t getMappedAddr( uint32_t addr ){ return 0; } /// TODO

        void addMemBlock( McuMemBlock* b, uint32_t addr );

    private:
        inline McuMemBlock* getblock( uint32_t addr );

        InvalidBlock m_invalidBlock;

        std::vector<McuMemBlock*> m_memBlocks;
};
#endif
