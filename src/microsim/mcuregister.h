/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUREGISTER_H
#define MCUREGISTER_H

#include <QString>
#include <QDebug>

//#include "mcutypes.h"
#include "regsignal.h"

class McuRegister
{
    friend class McuRegBlock;

    public:
        McuRegister( QString name, uint32_t addr, uint32_t mask, uint32_t rstVal, QString bits );
        ~McuRegister();

        void setData( uint8_t* data );

        uint8_t  read_08( uint32_t addr );
        uint16_t read_16( uint32_t addr );
        uint32_t read_32();
        uint32_t read() { return *m_data32; }

        void write_08( uint32_t addr, uint32_t val );
        void write_16( uint32_t addr, uint32_t val );
        void write_32( uint32_t val );

        uint8_t* getData08()  { return m_data8; }
        uint32_t getAddress() { return m_address; } /// Fixme: Offset

        template <class T>
        void setReturnCallback( T* obj, uint32_t (T::*func)() )
        {
            if( m_returnSlot ){
                qDebug() << "Warning: McuRegister returnSlot already exist";
                return;
            }
            ReturnCallBack32<T>* slot = new ReturnCallBack32<T>( obj, func, m_returnSlot );
            m_returnSlot = slot;
        }

        template <class T>
        void addReadCallback( T* obj, void (T::*func)() )
        {
            RegCallBack32<T>* slot = new RegCallBack32<T>( obj, func, m_readSlot );
            m_readSlot = slot;
        }

        template <class T>
        void addWriteCallback( T* obj, void (T::*func)() )
        {
            RegCallBack32<T>* slot = new RegCallBack32<T>( obj, func, m_writeSlot );

            // New slots are prepended (LIFO)
            // This means Interrupt flag clearing after register write callback
            // Because Interrupts are created first
            m_writeSlot = slot;
        }

    protected:
        inline void callSlots( CallBackBase32* slot )
        {
            while( slot ){
                slot->call();
                slot = slot->m_nextCallBack;
            }
        }

        QString m_name;
        QString m_bitNames;

        uint32_t m_address;
        uint32_t m_bitMask;
        uint32_t m_rstVal;

        uint8_t*  m_data8;
        uint32_t* m_data32;

        CallBackBase32* m_returnSlot;
        CallBackBase32* m_readSlot;
        CallBackBase32* m_writeSlot;
};

#endif
