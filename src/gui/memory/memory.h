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
    public:
        Memory();
        ~Memory();

        bool loadData( bool resize=false );
        void saveData();

        bool loadFile( QString file, bool resize, eMcu* eMcu=nullptr );
        bool loadDat( QString file, bool resize );
        bool loadDatStr( QString data, bool resize );
        bool loadHex( QString file, bool resize );
        bool loadBin( QString file, bool resize );

        QString getMem();
        void setMem( QString m );

        void setBits( int b );

        virtual void showTable();

    protected:
        void saveDat( int bits );
        void saveHex( int bits ); /// TODO
        void saveBin( int bits );

        int m_wordBits;
        int m_wordBytes;

        MemTable* m_memTable;
        eMcu* m_eMcu;

        std::vector<uint64_t> m_data;
};

#endif
