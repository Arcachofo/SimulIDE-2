/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <QString>

class MemTable;
class eMcu;

class Memory
{
    friend class MemTable;

    public:
        Memory();
        ~Memory();

        uint32_t getValue( int address ) { return m_data[address]; }
        void     setValue( int address, uint32_t value ) { m_data[address] = value; }

        bool loadData( bool resize=false );
        void saveData();

        bool loadFile( QString file, bool resize, eMcu* eMcu=nullptr );
        bool loadDat( QString file, bool resize );
        bool loadDatStr( QString data, bool resize );
        bool loadHex( QString file, bool resize );
        bool loadBin( QString file, bool resize );

        QString getMem();
        void setMem( QString m );

        int wordBits() { return m_wordBits; }
        void setWordBits( int b );

        int wordBytes() { return m_wordBytes; }

        uint32_t size() { return m_data.size(); }
        void resize( int size );

        uint32_t* rawData() { return m_data.data(); }  // Get pointer to data

        void fillMemory( uint32_t v );

        virtual void showTable();

        MemTable* getTable();

    protected:
        void saveDat( int bits );
        void saveHex( int bits ); /// TODO
        void saveBin( int bits );

        int m_wordBits;
        int m_wordBytes;

        MemTable* m_memTable;
        eMcu* m_eMcu;

        std::vector<uint32_t> m_data;
};

#endif
