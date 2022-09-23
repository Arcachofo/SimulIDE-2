/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef RTCLOCK_H
#define RTCLOCK_H

#include <QTime>

#include "e-element.h"

class IoPin;

class MAINMODULE_EXPORT RtClock : public eElement
{
    public:
        RtClock( QString id );
        ~RtClock();

        virtual void initialize() override;
        virtual void runEvent() override;

        void enable( bool en );
        void setDisOut( bool d );
        void setFreq( uint64_t freq );
        void setPin( IoPin* pin ) { m_outpin = pin; }

        QTime m_time;
        QDate m_date;

    private:
        bool m_enabled;
        bool m_disOut;

        uint64_t m_tCount;
        uint64_t m_sCount;
        uint64_t m_toggle;
        uint64_t m_halfPeriod;
        uint64_t m_freqBase;

        IoPin* m_outpin;
};

#endif
