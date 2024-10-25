/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "avrspi.h"
#include "datautils.h"
#include "mcuram.h"
#include "iopin.h"
#include "e_mcu.h"
#include "mcuinterrupts.h"

AvrSpi::AvrSpi( eMcu* mcu, QString name )
      : McuSpi( mcu, name )
{
}
AvrSpi::~AvrSpi(){}

void AvrSpi::setup()
{
    QString n = m_name.right(1);
    bool ok = false;
    n.toInt( &ok );
    if( !ok ) n = "";

    m_SPR   = m_mcuRam->getRegBits( "SPR"+n+"0,SPR"+n+"1" );
    m_SPE   = m_mcuRam->getRegBits( "SPE"+n );
    m_DODR  = m_mcuRam->getRegBits( "DODR"+n );
    m_MSTR  = m_mcuRam->getRegBits( "MSTR"+n );
    m_CPOL  = m_mcuRam->getRegBits( "CPOL"+n );
    m_CPHA  = m_mcuRam->getRegBits( "CPHA"+n );
    m_SPI2X = m_mcuRam->getRegBits( "SPI2X"+n );
}

void AvrSpi::initialize()
{
    McuSpi::initialize();
    m_status = 0;
}

void AvrSpi::setMode( spiMode_t mode )
{
    if     ( mode == m_mode ) return;
    else if( mode == SPI_OFF )
    {
        m_MOSI->controlPin( false, false );
        m_MISO->controlPin( false, false );
        m_clkPin->controlPin( false, false );
        m_SS->controlPin( false, false );
    }
    else if( mode == SPI_MASTER )
    {
        m_MOSI->controlPin( true, false );
        m_MISO->setPinMode( input );
        m_MISO->controlPin( true, true );
        m_clkPin->controlPin( true, false );
        //m_SS->controlPin( true, false );
    }
    else if( mode == SPI_SLAVE )
    {
        m_MOSI->setPinMode( input );
        m_MOSI->controlPin( true, true );
        m_clkPin->setPinMode( input );
        m_clkPin->controlPin( true, true );
        m_SS->setPinMode( input );
        m_SS->controlPin( true, true );
        m_MISO->controlPin( true, false );
    }
    SpiModule::setMode( mode );
}

void AvrSpi::configureA() // SPCR is being written
{
    bool enable = m_SPE.getRegBitsBool();
    if( !enable )                 /// Disable SPI
    { setMode( SPI_OFF ); return; }

    bool master = m_MSTR.getRegBitsBool();
    spiMode_t mode = master ? SPI_MASTER : SPI_SLAVE;
    setMode( mode );

    m_lsbFirst  = m_DODR.getRegBitsBool(); // Data order

    bool clkPol = m_CPOL.getRegBitsBool(); // Clock polarity
    m_leadEdge = clkPol ? Clock_Falling : Clock_Rising;
    m_tailEdge = clkPol ? Clock_Rising  : Clock_Falling;
    m_clkPin->setOutState( clkPol );
    updateClock();

    bool clkPha = m_CPHA.getRegBitsBool(); // Clock phase
    m_sampleEdge = ( clkPol == clkPha ) ? Clock_Rising : Clock_Falling; // This shows up in the truth table

    uint8_t spr = m_SPR.getRegBitsVal();
    m_prescaler = m_prescList[spr];
    updateSpeed();
}

void AvrSpi::writeStatus() // SPSR is being written
{
    m_speed2x = m_SPI2X.getRegBitsBool();
    updateSpeed();

    // m_mcuRam->m_regOverride = (*m_statReg & ~m_SPI2X.mask) | spi2x; // Preserve Status bits
    *m_statReg = m_status & ~m_SPI2X.mask;
    if( m_speed2x ) *m_statReg |= m_SPI2X.mask;
    m_status = *m_statReg ;
}

void AvrSpi::writeSpiReg() // SPDR is being written
{
    m_srReg = *m_dataReg;

    /// SPIF is cleared by first reading the SPI Status Register with SPIF set,
    /// then accessing the SPI Data Register (SPDR).
    m_interrupt->clearFlag();          // Clear Iterrupt flag

    if( m_mode == SPI_MASTER ) StartTransaction();
}

void AvrSpi::endTransaction()
{
    SpiModule::endTransaction();
    *m_dataReg = m_srReg;
    m_interrupt->raise();
}

void AvrSpi::updateSpeed()
{
    uint64_t div = m_speed2x ? 4 : 2;
    m_clockPeriod = m_mcu->psInst()*m_prescaler/div;
}
