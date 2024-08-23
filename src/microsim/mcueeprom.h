/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCUEEPROM_H
#define MCUEEPROM_H

#include <QVector>

#include "mcumodule.h"
#include "e-element.h"

class McuEeprom : public McuModule, public eElement
{
        friend class McuCreator;
        friend class Mcu;

    public:
        McuEeprom( eMcu* mcu, QString name );
        virtual ~McuEeprom();

        virtual void setup() override {;}

        virtual void initialize() override;
        virtual void reset() override;

        virtual void readEeprom();
        virtual void writeEeprom();

        virtual void addrWriteL( uint8_t val );
        virtual void addrWriteH( uint8_t val );

        QVector<int>* data() { return &m_data; }
        void setData( QVector<int>* eep );

        uint32_t size()  { return m_size; }

        uint8_t  getValue( int address ) { return m_data[address]; }
        void     setValue( int address, uint8_t value ) { m_data[address] = value; }

    protected:

        uint8_t* m_addressL; // Actual ram for counter Low address byte
        uint8_t* m_addressH; // Actual ram for counter High address byte

        uint32_t m_address;
        uint8_t* m_dataReg;

        uint32_t m_size;
        QVector<int> m_data;
        bool m_saveEepr;
};

#endif
