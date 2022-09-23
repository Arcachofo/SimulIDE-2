/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef MCS65CPU_H
#define MCS65CPU_H

#include "cpubase.h"
#include "e-element.h"
#include "iopin.h"

#define CONSTANT  0x20
#define BREAK     0x10

#define SET_NEGATIVE(x)  write_S_Bit( N, x & 0x80 ) //(x ? (m_STATUS |= NEGATIVE) : (m_STATUS &= (~NEGATIVE)) )
#define SET_OVERFLOW(x)  write_S_Bit( V, x ) //(x ? (m_STATUS |= OVERFLOW) : (m_STATUS &= (~OVERFLOW)) )
#define SET_DECIMAL(x)   write_S_Bit( D, x ) //(x ? (m_STATUS |= DECIMAL) : (m_STATUS &= (~DECIMAL)) )
#define SET_INTERRUPT(x) write_S_Bit( I, x ) //(x ? (m_STATUS |= INTERRUPT) : (m_STATUS &= (~INTERRUPT)) )
#define SET_ZERO(x)      write_S_Bit( Z, x ) //(x ? (m_STATUS |= ZERO) : (m_STATUS &= (~ZERO)) )
#define SET_CARRY(x)     write_S_Bit( C, x ) //(x ? (m_STATUS |= CARRY) : (m_STATUS &= (~CARRY)) )

class IoPort;
class IoPin;

class MAINMODULE_EXPORT Mcs65Cpu : public CpuBase, public eElement
{
    public:
        Mcs65Cpu( eMcu* mcu );
        ~Mcs65Cpu();

        virtual void stamp() override;
        virtual void runEvent() override;

        virtual void reset() override;
        virtual void runStep() override;
        virtual void extClock( bool clkState ) override;

        virtual uint getPC() override { return m_debugPC; }

        enum { C=0,Z,I,D,B,O,V,N }; // STATUS bits

        enum cpuState_t{
            cRESET=0,
            cFETCH,
            cDECODE,
            cREAD,
            cEXEC,
            cWRITE
        };
        enum addrMode_t{
            aNONE=0,
            aACCU,
            aIMME,
            aRELA,
            aABSO,
            aINDI
        };
        enum addrIndx_t{
            iX=1<<0,
            iY=1<<1,
            iC=1<<2,
            iI=1<<3,
            iZ=1<<4,
        };

    protected:
        inline void clkRisingEdge();
        inline void clkFallingEdge();
        inline void resetSeq();
        inline void decode();

        inline void readPGM();
        inline void Read();
        inline void readMem( uint16_t addr );
        inline uint8_t readDataBus();

        inline void writeMem( uint16_t addr );

        uint32_t m_debugPC; // PC for dDebugger

        bool m_nextClock;

        //uint64_t m_psStep;
        int m_cycle;

        // Buses
        IoPort* m_dataBus;
        IoPort* m_addrBus;
        IoPin* m_rwPin;

        // Control Pins
        IoPort* m_ctrlPort;
        IoPin* m_phi0Pin;
        IoPin* m_phi1Pin;
        IoPin* m_phi2Pin;
        IoPin* m_syncPin;

        // Interrupt Pins
        IoPin* m_irqPin;
        IoPin* m_nmiPin;

        // User Pins
        IoPin* m_rdyPin;
        IoPin* m_soPin;
        IoPin* m_dbePin;

        uint8_t m_P;   // status
        uint8_t m_SP;
        uint8_t m_Ac;
        uint8_t m_IR;
        uint8_t m_rX;
        uint8_t m_rY;
        //uint8_t* m_rI;

        cpuState_t m_state;
        cpuState_t m_nextState;

        uint8_t m_IsrH;
        uint8_t m_IsrL;

        addrMode_t m_aMode;
        uint8_t m_aIndx;

        uint8_t m_tmp0;
        uint8_t m_tmp1;
        uint8_t m_op0;
        uint16_t m_opAddr;

        uint16_t m_busAddr;
        pinMode_t m_dataMode;

        // Timing
        uint64_t m_tHR = 10+5;     // 10 Read Data Hold Time: Time to release Data Bus (set Data Bus is inmediate)
        uint64_t m_tHA = 15+5;     // 15 Address Hold Time:   Time to set Address Bus
        uint64_t m_tHW = 30+5;     // 30 RW Hold Time:        Time to set RW PinC;

        typedef void (Mcs65Cpu::* funcPtr_t)();
        funcPtr_t m_EXEC;   // Pointer to function execute instruction

        inline void setNZ( uint8_t val );
        inline void writeWflags( bool c );

        inline void pushStack8( uint8_t byte );
        inline void popStack8();

        inline void ADC();
        inline void AND();
        inline void ASL();
        inline void BIT();
        inline void BRK();
        inline void CLC();
        inline void CLD();
        inline void CLI();
        inline void CLV();
        inline void CMP();
        inline void CPX();
        inline void CPY();
        inline void DEC();
        inline void DEX();
        inline void DEY();
        inline void EOR();
        inline void INC();
        inline void INX();
        inline void INY();
        inline void JMP();
        inline void JSR();
        inline void LDA();
        inline void LDX();
        inline void LDY();
        inline void LSR();
        inline void NOP();
        inline void ORA();
        inline void PHA();
        inline void PHP();
        inline void PLA();
        inline void PLP();
        inline void ROL();
        inline void ROR();
        inline void RTI();
        inline void RTS();
        inline void SBC();
        inline void SEC();
        inline void SED();
        inline void SEI();
        inline void STA();
        inline void STX();
        inline void STY();
        inline void TAX();
        inline void TAY();
        inline void TSX();
        inline void TXA();
        inline void TXS();
        inline void TYA();
        inline void BXX();
};
#endif
