/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef PICCOMPARATOR_H
#define PICCOMPARATOR_H

#include "mcucomparator.h"

class PicComp : public McuComp
{
    public:
        PicComp( eMcu* mcu, QString name );
        ~PicComp();

        virtual void setup() override;
        virtual void initialize() override;
        virtual void voltChanged() override;

 static PicComp* createComparator( eMcu* mcu, QString name, int type );

    protected:
        void connect( McuPin* pinN, McuPin* pinP=NULL, McuPin* pinOut=NULL );

        bool m_inv;

        regBits_t m_COUT;
        regBits_t m_CINV;
};

class PicComp0 : public PicComp
{
    public:
        PicComp0( eMcu* mcu, QString name );
        ~PicComp0();

        virtual void configureA() override;

    protected:
        bool m_cis;

        regBits_t m_CM;
        regBits_t m_CIS;
};

class PicComp01 : public PicComp0
{
    public:
        PicComp01( eMcu* mcu, QString name );
        ~PicComp01();

    protected:
        virtual void setMode( uint8_t mode );
};

class PicComp02 : public PicComp0
{
    public:
        PicComp02( eMcu* mcu, QString name );
        ~PicComp02();

    protected:
        virtual void setMode( uint8_t mode );
};

class PicComp03 : public PicComp0
{
    public:
        PicComp03( eMcu* mcu, QString name );
        ~PicComp03();

    protected:
        virtual void setMode( uint8_t mode );
};


class PicComp1 : public PicComp
{
    public:
        PicComp1( eMcu* mcu, QString name );
        ~PicComp1();
        virtual void configureA() override;

    protected:
        uint8_t* m_CMxCON0;

        regBits_t m_CxON;
        regBits_t m_CxOE;
        regBits_t m_CxR;
        regBits_t m_CxCH;
};

class PicComp11 : public PicComp1
{
    public:
        PicComp11( eMcu* mcu, QString name );
        ~PicComp11();

        virtual void configureC() override;

    protected:
};

class PicComp12 : public PicComp1
{
    public:
        PicComp12( eMcu* mcu, QString name );
        ~PicComp12();

        virtual void configureB() override;
        virtual void configureC() override;

    protected:
};

class PicComp20 : public PicComp
{
    public:
        PicComp20( eMcu* mcu, QString name );
        ~PicComp20();

        virtual void configureA() override;
        virtual void configureB() override;

    protected:
};
#endif
