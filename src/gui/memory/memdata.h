/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MEMDATA_H
#define MEMDATA_H

#include <QVector>

class MemTable;
class eMcu;

class MemData
{
    public:
        MemData();
        ~MemData();

        bool loadData( bool resize=false );
        void saveData();

        bool loadFile( QString file, bool resize, eMcu* eMcu=nullptr );
        bool loadDat( QString file, bool resize );
        bool loadDatStr( QString data, bool resize );
        bool loadHex( QString file, bool resize );
        bool loadBin( QString file, bool resize );

        QString getMem();
        void setMem( QString m );

        void setBits( int b ) { m_bits = b; }

        virtual void showTable( int dataSize=256, int wordBytes=1 );

    protected:
        void saveDat( int bits );
        void saveHex( int bits ); /// TODO
        void saveBin( int bits );

        int m_bits;

        MemTable* m_memTable;
        eMcu* m_eMcu;

        std::vector<uint64_t> m_data;
};

#endif


