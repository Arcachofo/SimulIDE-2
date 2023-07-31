/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <QWidget>

#include "updatable.h"
#include "e-element.h"


class Display : public QWidget, public Updatable, public eElement
{
    public:
        Display( int width, int height, QString name, QWidget* parent );
        ~Display();

        virtual void initialize() override;
        virtual void updateStep() override;

        void setWidth( int w );
        void setHeight( int h );
        void setSize( int w, int h );
        void setBackground( int b );
        void setPixel( int x, int y, int color );

        void setMonitorScale( double scale );

        QImage* getImage() { return &m_image; }

    protected:
        virtual void paintEvent( QPaintEvent* e ) override;

        void updtImageSize();

        QString m_name;

        bool m_changed;

        int m_width;
        int m_height;

        double m_scale;

        int m_background;

        QRectF  m_area;

        QImage m_image;    //Visual representation
};

#endif
