/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef REGSIGNAL_H
#define REGSIGNAL_H

#include <vector>
#include <inttypes.h>

class CallBackBase32
{
        friend class McuRegister;

    public:
        CallBackBase32(){;}
        virtual ~CallBackBase32(){;}

        virtual void call(){;}
        virtual uint32_t read(){ return -1; }

    protected:
        CallBackBase32* m_nextCallBack;
};

template <class T>
class RegCallBack32 : public CallBackBase32
{
        friend class McuRegister;

    public:
        RegCallBack32( T* object, void (T::*func)(), CallBackBase32* next )
        : CallBackBase32()
        {
            m_object = object;
            m_func = func;
            m_nextCallBack = next;
        }
        ~RegCallBack32(){;}

        virtual void call() override
        { (m_object->*m_func)(); }

    private:
        T* m_object;
        void (T::*m_func)();
};

template <class T>
class ReturnCallBack32 : public CallBackBase32
{
        friend class McuRegister;

    public:
        ReturnCallBack32( T* object, uint32_t (T::*func)(), CallBackBase32* next )
        : CallBackBase32()
        {
            m_object = object;
            m_func = func;
            m_nextCallBack = next;
        }
        ~ReturnCallBack32(){;}

        virtual uint32_t read() override
        { return (m_object->*m_func)(); }

    private:
        T* m_object;
        uint32_t (T::*m_func)();
};

/*
template <class Obj>
class ReadCallBack32 : public CallBackBase32
{
        friend class McuRegister;

    public:
        ReadCallBack32( Obj* object, void (Obj::*func)(uint32_t) )
        : CallBackBase32()
        {
            m_object = object;
            m_func = func;
            m_nextCallBack = nullptr;
        }
        ~ReadCallBack32() {;}

        virtual void read() override
        { (m_object->*m_func)(); }

    private:
        Obj* m_object;
        void (Obj::*m_func)();
};
*/
/*
class WriteSignal32
{
    public:
        WriteSignal32()
        {
            m_slot = nullptr;
        }
        ~WriteSignal32()
        {
            CallBackBase32* slot = m_slot;
            while( slot ) // delete slots
            {
                CallBackBase32* slotDel = slot;
                slot = slot->m_nextCallBack;
                delete slotDel;
        }   }

        template <class Obj>
        void connect( Obj* obj, void (Obj::*func)(uint32_t), uint32_t mask=-1 )
        {
            WriteCallBack32<Obj>* slot = new WriteCallBack32<Obj>( obj, func );
            slot->m_mask = mask;

            // New slots are prepended (LIFO)
            // This means Interrupt flag clearing after register write callback
            // Because Interrupts are created first
            slot->m_nextCallBack = m_slot;
            m_slot = slot;
        }

        template <class Obj>
        void disconnect( Obj* obj, void (Obj::*func)(uint32_t) )
        {
            CallBackBase32* preSlot = 0;
            CallBackBase32* posSlot = m_slot;
            while( posSlot )
            {
                WriteCallBack32<Obj>* cb = dynamic_cast<WriteCallBack32<Obj>*>(posSlot);

                if( (cb->m_object == obj) && (cb->m_func == func))
                {
                    if( preSlot ) preSlot->m_nextCallBack = posSlot->m_nextCallBack;
                    else          m_slot = posSlot->m_nextCallBack;

                    delete posSlot;
                    break;
                }
                preSlot = posSlot;
                posSlot = posSlot->m_nextCallBack;
        }   }

        void write( uint32_t val ) // Calls all connected functions with masked val.
        {
            CallBackBase32* slot = m_slot;
            while( slot ){
                slot->write( val );
                slot = slot->m_nextCallBack;
        }   }

    private:
        CallBackBase32* m_slot;
};

class ReadSignal32
{
    public:
        ReadSignal32()
        {
            m_slot = nullptr;
        }
        ~ReadSignal32()
        {
            CallBackBase32* slot = m_slot;
            while( slot ) // delete slots
            {
                CallBackBase32* slotDel = slot;
                slot = slot->m_nextCallBack;
                delete slotDel;
        }   }

        template <class Obj>
        void connect( Obj* obj, void (Obj::*func)(uint32_t), uint32_t mask=-1 )
        {
            ReadCallBack32<Obj>* slot = new ReadCallBack32<Obj>( obj, func );
            slot->m_mask = mask;

            // New slots are prepended (LIFO)
            // This means Interrupt flag clearing after register write callback
            // Because Interrupts are created first
            slot->m_nextCallBack = m_slot;
            m_slot = slot;
        }

        template <class Obj>
        void disconnect( Obj* obj, void (Obj::*func)(uint32_t) )
        {
            CallBackBase32* preSlot = 0;
            CallBackBase32* posSlot = m_slot;
            while( posSlot )
            {
                ReadCallBack32<Obj>* cb = dynamic_cast<ReadCallBack32<Obj>*>(posSlot);

                if( (cb->m_object == obj) && (cb->m_func == func))
                {
                    if( preSlot ) preSlot->m_nextCallBack = posSlot->m_nextCallBack;
                    else          m_slot = posSlot->m_nextCallBack;

                    delete posSlot;
                    break;
                }
                preSlot = posSlot;
                posSlot = posSlot->m_nextCallBack;
        }   }

        void read() // Calls all connected functions with masked val.
        {
            CallBackBase32* slot = m_slot;
            while( slot ){
                slot->read();
                slot = slot->m_nextCallBack;
        }   }

    private:
        CallBackBase32* m_slot;
};
*/
#endif
