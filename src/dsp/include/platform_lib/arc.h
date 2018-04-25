/*
// CONFIDENTIAL AND PROPRIETARY INFORMATION
// 
// Copyright (c) 2015 Synopsys, Inc. All rights reserved. 
// This software and documentation contain confidential and
// proprietary information that is the property of 
// Synopsys, Inc. The software and documentation are 
// furnished under a license agreement and may be used 
// or copied only in accordance with the terms of the license 
// agreement. No part of the software and documentation 
// may be reproduced, transmitted, or translated, in any 
// form or by any means, electronic, mechanical, manual, 
// optical, or otherwise, without prior written permission 
// of Synopsys, Inc., or as expressly provided by the license agreement.
// Reverse engineering is prohibited, and reproduction, 
// disclosure or use without specific written authorization
// of Synopsys Inc. is strictly forbidden.
*/
 


#ifndef __ARC_H
#define __ARC_H

#ifndef ASSERT

#if defined(_MSC_VER) && defined(_DEBUG)
  #define ASSERT(x)   if (!(x)) __asm int 3
#else
  #undef ASSERT
  #define ASSERT(x) {}
#endif

#endif

#include <stdint.h>

#if ((_ARCVER >= 0x21) && (_ARCVER <= 0x26))
#define _ARCVER_ARC600
#endif

#if ((_ARCVER >= 0x31) && (_ARCVER <= 0x35))
#define _ARCVER_ARC700
#endif

#if ((_ARCVER == 0x40) || (_ARCVER == 0x41)) 
#define _ARCVER_ARCv2EM
#endif

#if ((_ARCVER == 0x50) || (_ARCVER == 0x51))
#define _ARCVER_ARCv2HS
#endif

// ALL ARC platforms except ARCv2EM with DSP or DSP complex extension
#if !defined(_ARCVER_ARCv2EM) && !defined(_ARCVER_ARCv2HS)
// core registers
#define REG_X0_U0 (32) //  r/w  32  XY MEMORY DSP v3.1 	Pointer x0 with update 		mode 0 
#define REG_X0_U1 (33) //  r/w  32  XY MEMORY DSP v3.1 	Pointer x0 with update 		mode 1 
#define REG_X1_U0 (34) //  r/w  32  XY MEMORY DSP v3.1 	Pointer x1 with update      mode 0 
#define REG_X1_U1 (35) //  r/w  32  XY MEMORY DSP v3.1 	Pointer x1 with update 		mode 1 
#define REG_X2_U0 (36) //  r/w  32  XY MEMORY DSP v3.1 	Pointer x2 with update 		mode 0 
#define REG_X2_U1 (37) //  r/w  32  XY MEMORY DSP v3.1 Pointer x2 with update mode 1 
#define REG_X3_U0 (38) //  r/w  32  XY MEMORY DSP v3.1 Pointer x3 with update mode 0 
#define REG_X3_U1 (39) //  r/w  32  XY MEMORY DSP v3.1 Pointer x3 with update mode 1 
#define REG_Y0_U0 (40) //  r/w  32  XY MEMORY DSP v3.1 Pointer y0 with update mode 0 
#define REG_Y0_U1 (41) //  r/w  32  XY MEMORY DSP v3.1 Pointer y0 with update mode 1 
#define REG_Y1_U0 (42) //  r/w  32  XY MEMORY DSP v3.1 Pointer y1 with update mode 0 
#define REG_Y1_U1 (43) //  r/w  32  XY MEMORY DSP v3.1 Pointer y2 with update mode 1 
#define REG_Y2_U0 (44) //  r/w  32  XY MEMORY DSP v3.1 Pointer y2 with update mode 0 
#define REG_Y2_U1 (45) //  r/w  32  XY MEMORY DSP v3.1 Pointer y3 with update mode 1 
#define REG_Y3_U0 (46) //  r/w  32  XY MEMORY DSP v3.1 Pointer y3 with update mode 0 
#define REG_Y3_U1 (47) //  r/w  32  XY MEMORY DSP v3.1 Pointer y3 with update mode 1 
#define REG_X0_NU (48) //  r/w  32  XY MEMORY DSP v3.1 Pointer x0 with no update 
#define REG_X1_NU (49) //  r/w  32  XY MEMORY DSP v3.1 Pointer x1 with no update 
#define REG_X2_NU (50) //  r/w  32  XY MEMORY DSP v3.1 Pointer x2 with no update 
#define REG_X3_NU (51) //  r/w  32  XY MEMORY DSP v3.1 Pointer x3 with no update 
#define REG_Y0_NU (52) //  r/w  32  XY MEMORY DSP v3.1 Pointer y0 with no update 
#define REG_Y1_NU (53) //  r/w  32  XY MEMORY DSP v3.1 Pointer y1 with no update
#define REG_Y2_NU (54) //  r/w  32  XY MEMORY DSP v3.1 Pointer y2 with no update 
#define REG_Y3_NU (55) //  r/w  32  XY MEMORY DSP v3.1 Pointer y3 with no update 
#define REG_ACC1  (56) //  r/w  32  XMAC DSP v3.1 XMAC: Accumulator 1  saturating 
#define REG_ACC2  (57) //  r/w  32  XMAC DSP v3.1 XMAC: Accumulator 2 =saturating

// aux registers
#define STATUS32        0xA 

#define AUX_XMAC0_24    0x2C    // r/w 32 24x24 accumulator guard register
#define AUX_XMAC1_24    0x2D    // r/w 32 24x24 accumulator MSP
#define AUX_XMAC2_24    0x2E    // r/w 32 24x24 accumulator LSP
#define AUX_FBF_STORE_16 0x2f
#define AUX_CRC_POLY    0x32
#define AUX_CRC_MODE    0x33
#define AUX_MACMODE     0x41
#define AUX_XMAC0       0x44    // r/w 32 16x16 accumulator guard register
#define AUX_XMAC1       0x45    // r/w 32 16x16 accumulator MSP
#define AUX_XMAC2       0x46    // r/w 32 16x16 accumulator LSP


#define DSPRAM_BUILD    0x79
#define AX0             0x80
#define AX1             0x81
#define AX2             0x82
#define AX3             0x83
#define AY0             0x84
#define AY1             0x85
#define AY2             0x86
#define AY3             0x87
#define MX00            0x88
#define MX01            0x89
#define MX10            0x8a
#define MX11            0x8b
#define MX20            0x8c
#define MX21            0x8d
#define MX30            0x8e
#define MX31            0x8f
#define MY00            0x90
#define MY01            0x91
#define MY10            0x92
#define MY11            0x93
#define MY20            0x94
#define MY21            0x95
#define MY30            0x96
#define MY31            0x97

#define XYLSBASEX	(0x9d)		// r/w 24-32  Load store base addr for X memory
#define XYLSBASEY	(0x9e)		// r/w 24-32  Load store base addr for Y memory
#define XYLSBASEX_REGISTER  0x9D
#define XYLSBASEY_REGISTER  0x9E
#define AUX_XMACLW_H    0x9F    // r/w 32 32x16 accumulator (high)
#define AUX_XMACLW_L    0xA0    // r/w 32 32x16 accumulator (low)


// _ARC (ARC Baseline)

// Condition codes checking
#define _condition_z     __condition_z       
#define _condition_nz    __condition_nz      
#define _condition_p     __condition_p       
#define _condition_n     __condition_n       
#define _condition_c     __condition_c       
#define _condition_cc    __condition_cc      
#define _condition_v     __condition_v       
#define _condition_nv    __condition_nv      
#define _condition_gt    __condition_gt      
#define _condition_ge    __condition_ge      
#define _condition_lt    __condition_lt      
#define _condition_le    __condition_le      
#define _condition_hi    __condition_hi      
#define _condition_ls    __condition_ls      
#define _condition_pnz   __condition_pnz   


// Arithmetic and Logical Operations
int32_t  _add    (int32_t, int32_t); // ADD  a b + c add
int32_t  _add_f  (int32_t, int32_t);
int32_t  _adc    (int32_t, int32_t); // ADC  a b + c + C add with carry
int32_t  _adc_f  (int32_t, int32_t);
int32_t  _sub    (int32_t, int32_t); // SUB  a b – c subtract
int32_t  _sub_f  (int32_t, int32_t);
int32_t  _sbc    (int32_t, int32_t); // SBC  a (b – c) - C subtract with carry
int32_t  _sbc_f  (int32_t, int32_t);
int32_t  _and    (int32_t, int32_t); // AND  a b and c logical bitwise AND
int32_t  _and_f  (int32_t, int32_t);
int32_t  _or     (int32_t, int32_t); // OR   a b or c logical bitwise OR
int32_t  _or_f   (int32_t, int32_t);
int32_t  _bic    (int32_t, int32_t); // BIC  a b and not c logical bitwise AND with invert
int32_t  _bic_f  (int32_t, int32_t);
int32_t  _xor    (int32_t, int32_t); // XOR  a b exclusive-or c logical bitwise exclusive-OR
int32_t  _xor_f  (int32_t, int32_t);
int32_t  _max    (int32_t, int32_t); // MAX  a b max c larger of 2 signed integers
int32_t  _max_f  (int32_t, int32_t);
int32_t  _min    (int32_t, int32_t); // MIN  a b min c smaller of 2 signed integers
int32_t  _min_f  (int32_t, int32_t);
int32_t  _rsub   (int32_t, int32_t); // RSUB a c - b reverse subtract
int32_t  _rsub_f (int32_t, int32_t);
int32_t  _bset   (int32_t, int32_t); // BSET a b or (1<<c) bit set
int32_t  _bset_f (int32_t, int32_t);
int32_t  _bclr   (int32_t, int32_t); // BCLR a b and not (1<<c) bit clear
int32_t  _bclr_f (int32_t, int32_t);
int32_t  _bxor   (int32_t, int32_t); // BXOR a b xor (1<<c) bit xor
int32_t  _bxor_f (int32_t, int32_t);
int32_t  _bmsk   (int32_t, int32_t); // BMSK a b and ((1<<(c+1))-1) bit mask
int32_t  _bmsk_f (int32_t, int32_t);
int32_t  _add1   (int32_t, int32_t); // ADD1 a b + (c << 1) add with left shift by 1
int32_t  _add1_f (int32_t, int32_t);
int32_t  _add2   (int32_t, int32_t); // ADD2 a b + (c << 2) add with left shift by 2
int32_t  _add2_f (int32_t, int32_t);
int32_t  _add3   (int32_t, int32_t); // ADD3 a b + (c << 3) add with left shift by 3
int32_t  _add3_f (int32_t, int32_t);
int32_t  _sub1   (int32_t, int32_t); // SUB1 a b - (c << 1) subtract with left shift by 1
int32_t  _sub1_f (int32_t, int32_t);
int32_t  _sub2   (int32_t, int32_t); // SUB2 a b - (c << 2) subtract with left shift by 2
int32_t  _sub2_f (int32_t, int32_t);
int32_t  _sub3   (int32_t, int32_t); // SUB3 a b - (c << 3) subtract with left shift by 3
int32_t  _sub3_f (int32_t, int32_t);

void _btst   (int32_t, int32_t); // BTST b and (1<<c) bit test
void _tst    (int32_t, int32_t); // TST  b and c test
void _cmp    (int32_t, int32_t); // CMP  b - c compare
void _rcmp   (int32_t, int32_t); // RCMP c - b reverse compare

int32_t _mov    (int32_t); // MOV  b c move
int32_t _mov_f  (int32_t);
int32_t _sexb   (int32_t); // SEXB Sign extend byte
int32_t _sexb_f (int32_t);
int32_t _sexw   (int32_t); // SEXB Sign extend word
int32_t _sexw_f (int32_t);
int32_t _extb   (int32_t); // EXT Zero extend byte
int32_t _extb_f (int32_t);
int32_t _extw   (int32_t); // EXT Zero extend word
int32_t _extw_f (int32_t);
int32_t _not    (int32_t); // NOT Logical NOT
int32_t _not_f  (int32_t);
int32_t _neg    (int32_t); // NEG Negate
int32_t _neg_f  (int32_t);
uint32_t _abs   (int32_t); // ABS Absolute
uint32_t _abs_f (int32_t);

int32_t _asl1   (int32_t); // ASL Arithmetic shift left by one
int32_t _asl1_f (int32_t);
int32_t _asr1   (int32_t); // ASR Arithmetic shift right by one
int32_t _asr1_f (int32_t);
int32_t _lsr1   (int32_t); // LSR Logical shift right by one
int32_t _lsr1_f (int32_t);
int32_t _ror1   (int32_t); // ROR Rotate right
int32_t _ror1_f (int32_t);
int32_t _rlc    (int32_t); // RLC Rotate left through carry
int32_t _rlc_f  (int32_t);
int32_t _rrc    (int32_t); // RRC Rotate right through carry
int32_t _rrc_f  (int32_t);

void _flag  (uint32_t); // FLAG Set flags
#ifndef _ARC
#ifndef _nop
#define _nop() {} // NOP No operation Null Instruction
#endif
#endif
//void _swi();
void _brk(); // BRK Stop and flush processor pipeline Breakpoint Instruction
void _sync(); // SYNC Synchronize with memory Wait for all data-based memory transactions to complete

#ifndef _ARC
uint32_t _get_STATUS32();
#endif // !_ARC

#ifndef _ARC
// Memory compiler intrinsics
uint32_t _core_read (uint32_t);
void    _core_write (uint32_t, uint32_t);
uint32_t _lr    (uint32_t);
void    _sr     (uint32_t, uint32_t);
#endif

// __Xbarrel_shifter
int32_t _asl    (int32_t, int32_t); // ASL  a b asl c arithmetic shift left
int32_t _asl_f  (int32_t, int32_t);
int32_t _asr    (int32_t, int32_t); // ASR  a b asr c arithmetic shift right
int32_t _asr_f  (int32_t, int32_t);
int32_t _lsr    (int32_t, int32_t); // LSR  a b lsr c logical shift right
int32_t _lsr_f  (int32_t, int32_t);
int32_t _ror    (int32_t, int32_t); // ROR  a b ror c rotate right
int32_t _ror_f  (int32_t, int32_t);

// __Xnorm
int32_t _norm   (int32_t); // NORM Normalize
int32_t _norm_f (int32_t);
int32_t _normw  (int32_t); // NORMW Normalize
int32_t _normw_f(int32_t);

// __Xswap
int32_t _swap   (int32_t); // SWAP Swap words
int32_t _swap_f (int32_t);

// __Xmpy16
uint32_t _mpyuw     (uint16_t, uint16_t); // MPYUW Unsigned 16x16 Multiply
uint32_t _mpyuw_f   (uint16_t, uint16_t);
int32_t _mpyw       (int16_t, int16_t); // MPYW Signed 16x16 Multiply
int32_t _mpyw_f     (int16_t, int16_t);

// __Xea
int32_t _adds    (int32_t, int32_t); // ADDS a sat32 (b+c) Add and saturate.
int32_t _adds_f  (int32_t, int32_t);
int32_t _subs    (int32_t, int32_t); // SUBS a sat32 (b-c) Subtract and saturate.
int32_t _subs_f  (int32_t, int32_t);
int32_t _divaw   (int32_t, int32_t); // DIVAW b_temp b<<1 if (b_temp>=c) a ((b_temp-c)+1) else a b  Division assist.
int32_t _asls    (int32_t, int32_t); // ASLS a sat32 (b<<c) Arithmetic shift left and saturate. Supports negative shift values for right shift.
int32_t _asls_f  (int32_t, int32_t);
int32_t _asrs    (int32_t, int32_t); // ASRS a sat32 (b>>c) Arithmetic shift right and saturate. Supports -ve shift values for left shift.
int32_t _asrs_f  (int32_t, int32_t);
int32_t _addsdw  (int32_t, int32_t); // ADDSDW a sat16 (b.high+c.high): sat16(b.low+c.low) Dual 16-bit add and saturate.
int32_t _subsdw  (int32_t, int32_t); // SUBSDW a sat16 (b.high-c.high): sat16(b.low-c.low) Dual16-bit subtract and saturate.

int16_t _sat16  (int32_t); // SAT16 b sat16(c) Saturate 32-bit input to 16-bits
int16_t _sat16_f(int32_t);
int16_t _rnd16  (int32_t); // RND16 b sat32(c+0x00008000)&0xffff0000 Round 32-bit input to 16-bits
int16_t _rnd16_f(int32_t);
int16_t _abssw  (int32_t); // ABSSW b sat16(abs(c.low)) Absolute value of 16-bit input
int16_t _abssw_f(int32_t);
int32_t _abss   (int32_t); // ABSS b sat32(abs(c)) Absolute value of 32-bit input
int32_t _abss_f (int32_t);
int16_t _negsw  (int32_t); // NEGSW b sat16(neg(c.low)) Negate and saturate 16-bit input
int16_t _negsw_f(int32_t);
int32_t _negs   (int32_t); // NEGS b sat32(neg(c)) Negate and saturate 32-bit input
int32_t _negs_f (int32_t);

#if (!defined(__Xmul32x16) && !defined(__Xxmac_d16))||(!defined(_ARC))
// __Xmul32x16 || __Xxmac_d16
int32_t  _get_ACC1();
uint32_t _get_ACC2();
void  _set_ACC1(uint32_t);
void  _set_ACC2(uint32_t);
#endif

#if (!defined(__Xmul32x16) && !defined(__Xxmac_d16) && !defined(__Xxmac_24))||(!defined(_ARC))
// __Xmul32x16 || __Xxmac_d16 || __Xxmac_24
uint32_t _get_MACMODE();
void     _set_MACMODE(uint32_t);
#endif

#if !defined(__Xmul32x16)||(!defined(_ARC)) // (Multiply-Accumulate with 32-bit X 16-Bit Data)
uint32_t _get_AUX_XMACLW_H();
uint32_t _get_AUX_XMACLW_L();
void _set_AUX_XMACLW_H(uint32_t);
void _set_AUX_XMACLW_L(uint32_t);
#endif

uint32_t _mululw     (uint32_t, uint32_t); // MULULW Low part 32x32 MULU
uint32_t _mululw_f   (uint32_t, uint32_t);
int32_t _mullw       (int32_t, int32_t); // MULLW Low part 32x32 MUL
int32_t _mullw_f     (int32_t, int32_t);
int32_t _mulflw      (int32_t, int32_t); // MULFLW Low part 32x32 MULF
int32_t _mulflw_f    (int32_t, int32_t);
int32_t _maclw       (int32_t, int32_t); // MACLW Low part 32x32 MAC
int32_t _maclw_f     (int32_t, int32_t);
int32_t _macflw      (int32_t, int32_t); // MACFLW Low part 32x32 MACF
int32_t _macflw_f    (int32_t, int32_t);
uint32_t _machulw    (uint32_t, uint32_t); // MACHULW High part 32x32 MULU/MACU
uint32_t _machulw_f  (uint32_t, uint32_t);
int32_t _machlw      (int32_t, int32_t); // MACHLW High part 32x32 MUL/MAC
int32_t _machlw_f    (int32_t, int32_t);
int32_t _machflw     (int32_t, int32_t); // MACHFLW High part 32x32 MULF/MACF
int32_t _machflw_f   (int32_t, int32_t);
int32_t _mulhlw      (int32_t, int32_t); // MULHLW Returns high part of 48-bit result to register
int32_t _mulhlw_f    (int32_t, int32_t);
int32_t _mulhflw     (int32_t, int32_t); // MULHFLW Returns high part of 48-bit result to register with Q shift
int32_t _mulhflw_f   (int32_t, int32_t);

#if !defined(__Xxmac_d16)||(!defined(_ARC)) // (Multiply-Accumulate with Dual-Word Data)
uint32_t _get_AUX_XMAC0();
uint32_t _get_AUX_XMAC1();
uint32_t _get_AUX_XMAC2();
void _set_AUX_XMAC0(uint32_t);
void _set_AUX_XMAC1(uint32_t);
void _set_AUX_XMAC2(uint32_t);
uint32_t _get_AUX_FBF_STORE_16();
void _set_AUX_FBF_STORE_16(uint32_t);
#endif
int32_t _muldw      (int32_t, int32_t); // MULDW Dual 16x16-bit multiplication without rounding.
int32_t _muldw_f    (int32_t, int32_t);
uint32_t _muludw    (uint32_t, uint32_t); // MULUDW Dual unsigned 16x16-bit multiplication without rounding.
uint32_t _muludw_f  (uint32_t, uint32_t);
int32_t _mulrdw     (int32_t, int32_t); // MULRDW Dual 16x16-bit multiplication with rounding. (LSP write-back mod is not a useful operation)
int32_t _mulrdw_f   (int32_t, int32_t);
int32_t _macdw      (int32_t, int32_t); // MACDW Dual 16x16-bit multiply-accumulate without rounding.
int32_t _macdw_f    (int32_t, int32_t);
uint32_t _macudw    (uint32_t, uint32_t); // MACUDW Dual unsigned 16x16-bit multiply-accumulate without rounding.
uint32_t _macudw_f  (uint32_t, uint32_t);
int32_t _macrdw     (int32_t, int32_t); // MACRDW Dual 16x16-bit multiply-accumulate with rounding. (LSP write-back mode is not a useful operation)
int32_t _macrdw_f   (int32_t, int32_t);
int32_t _msubdw     (int32_t, int32_t); // MSUBDW Dual 16x16-bit multiply-subtract without rounding.
int32_t _msubdw_f   (int32_t, int32_t);
int32_t _cmacrdw    (int32_t, int32_t); // CMACRDW Partial complex multiplication (swaps products and subtracts from A2) with rounding
int32_t _cmacrdw_f  (int32_t, int32_t);
uint32_t _fbfdw     (uint32_t); // FBFDW Dual-16-bit FFT butterfly accelerator

#if !defined(__Xxmac_24)||(!defined(_ARC)) // (Multiply-Accumulate with 24-bit Data)
uint32_t _get_AUX_XMAC0_24();
uint32_t _get_AUX_XMAC1_24();
uint32_t _get_AUX_XMAC2_24();
void _set_AUX_XMAC0_24(uint32_t);
void _set_AUX_XMAC1_24(uint32_t);
void _set_AUX_XMAC2_24(uint32_t);
#endif
int32_t _mult    (int32_t, int32_t); // MULT 24x24-bit multiply without rounding
int32_t _mult_f  (int32_t, int32_t);
uint32_t _mulut  (uint32_t, uint32_t); // MULUT Unsigned 24x24-bit multiply without rounding 
uint32_t _mulut_f(uint32_t, uint32_t);
int32_t _mulrt   (int32_t, int32_t); // MULRT 24x24-bit multiply with rounding (LSP write-back mod is not a useful operation)
int32_t _mulrt_f (int32_t, int32_t);
int32_t _mact    (int32_t, int32_t); // MACT 24x24-bit multiply-accumulate without rounding
int32_t _mact_f  (int32_t, int32_t);
int32_t _macrt   (int32_t, int32_t); // MACRT 24x24-bit multiply-accumulate with rounding (LSP write-back mod is not a useful operation)
int32_t _macrt_f (int32_t, int32_t);
int32_t _msubt   (int32_t, int32_t); // MSUBT 24x24-bit multiply-subtract without rounding
int32_t _msubt_f (int32_t, int32_t);

// __Xdmulpf // (Multiply-Accumulate with Pseudo-Floating Point Data)
int32_t _dmulpf_xy(int32_t, int32_t); // DMULPF Dual 32 x 16-bit multiplication of data from XY memory and pseudo-floating point coefficient
int32_t _dmacpf_xy(int32_t, int32_t); // DMACPF Dual 32 x 16-bit multiplication of data from XY memory and pseudo-floating point coefficient

#if !defined(__Xcrc)||(!defined(_ARC))
uint32_t    _get_AUX_CRC_POLY();
uint32_t    _get_AUX_CRC_MODE();
void        _set_AUX_CRC_POLY(uint32_t x);
void        _set_AUX_CRC_MODE(uint32_t x);
#endif
uint32_t _crc(uint32_t, uint32_t); // CRC 2-cycle variable polynomial CRC 

// __Xvbfdw // not supported
// VBFDW Dual Viterbi Butterfly

// __Xdsp_packa // not supported
// ASLDW Dual Word Arithmetic Shift Left
// ASRDW Dual Word Arithmetic Shift Right
// LSRDW Dual Word Logical Shift Left
// ASLSDW Dual Word Arithmetic Shift Left Saturating
// ASRSDW Dual Word Arithmetic Shift Right Saturating
// MAXABSSDW Dual Word Maximum Operation of Absolute Value
// MAXIDL Dual Long Word MAX with Counter
// MINIDL Dual Long WordMIN with Counter

// __Xxy

#define AM_16BIT (1<<29) // 16-bit addressing mode
#define AM_BITREV (1<<30) // reverse-carry addressing
#define AM_DUAL (1<<15) // ‘Dual’ addressing mode
#define AM_MOD(modulo) ((modulo & 0x1fff) << 16) // range for modulo addressing
#define AM_OFFS(offs) (offs & 0x3fff) // address-offset value

#if !defined(__Xxy)||(!defined(_ARC))
uint32_t _get_xbase();
uint32_t _get_ybase();

void _set_xbase(uint32_t xbase);
void _set_ybase(uint32_t ybase);

void _flushx();
void _flushy();

uint32_t _ax0();
uint32_t _ax1();
uint32_t _ax2();
uint32_t _ax3();
uint32_t _ay0();
uint32_t _ay1();
uint32_t _ay2();
uint32_t _ay3();

void _set_ax0(uint32_t);
void _set_ax1(uint32_t);
void _set_ax2(uint32_t);
void _set_ax3(uint32_t);
void _set_ay0(uint32_t);
void _set_ay1(uint32_t);
void _set_ay2(uint32_t);
void _set_ay3(uint32_t);

uint32_t _mx00();
uint32_t _mx01();
uint32_t _mx10();
uint32_t _mx11();
uint32_t _mx20();
uint32_t _mx21();
uint32_t _mx30();
uint32_t _mx31();
uint32_t _my00();
uint32_t _my01();
uint32_t _my10();
uint32_t _my11();
uint32_t _my20();
uint32_t _my21();
uint32_t _my30();
uint32_t _my31();

void _set_mx00(uint32_t);
void _set_mx01(uint32_t);
void _set_mx10(uint32_t);
void _set_mx11(uint32_t);
void _set_mx20(uint32_t);
void _set_mx21(uint32_t);
void _set_mx30(uint32_t);
void _set_mx31(uint32_t);
void _set_my00(uint32_t);
void _set_my01(uint32_t);
void _set_my10(uint32_t);
void _set_my11(uint32_t);
void _set_my20(uint32_t);
void _set_my21(uint32_t);
void _set_my30(uint32_t);
void _set_my31(uint32_t);

int32_t _x0_u0();
int32_t _x0_u1();
int32_t _x1_u0();
int32_t _x1_u1();
int32_t _x2_u0();
int32_t _x2_u1();
int32_t _x3_u0();
int32_t _x3_u1();
int32_t _y0_u0();
int32_t _y0_u1();
int32_t _y1_u0();
int32_t _y1_u1();
int32_t _y2_u0();
int32_t _y2_u1();
int32_t _y3_u0();
int32_t _y3_u1();
int32_t _x0_nu();
int32_t _x1_nu();
int32_t _x2_nu();
int32_t _x3_nu();
int32_t _y0_nu();
int32_t _y1_nu();
int32_t _y2_nu();
int32_t _y3_nu();

#ifdef _ARC
void _set_x0_u0(int32_t);
void _set_x0_u1(int32_t);
void _set_x1_u0(int32_t);
void _set_x1_u1(int32_t);
void _set_x2_u0(int32_t);
void _set_x2_u1(int32_t);
void _set_x3_u0(int32_t);
void _set_x3_u1(int32_t);
void _set_y0_u0(int32_t);
void _set_y0_u1(int32_t);
void _set_y1_u0(int32_t);
void _set_y1_u1(int32_t);
void _set_y2_u0(int32_t);
void _set_y2_u1(int32_t);
void _set_y3_u0(int32_t);
void _set_y3_u1(int32_t);
void _set_x0_nu(int32_t);
void _set_x1_nu(int32_t);
void _set_x2_nu(int32_t);
void _set_x3_nu(int32_t);
void _set_y0_nu(int32_t);
void _set_y1_nu(int32_t);
void _set_y2_nu(int32_t);
void _set_y3_nu(int32_t);
#else // 
#define _set_x0_u0(x)   __set_x0_u0(x)
#define _set_x0_u1(x)   __set_x0_u1(x)
#define _set_x1_u0(x)   __set_x1_u0(x)
#define _set_x1_u1(x)   __set_x1_u1(x)
#define _set_x2_u0(x)   __set_x2_u0(x)
#define _set_x2_u1(x)   __set_x2_u1(x)
#define _set_x3_u0(x)   __set_x3_u0(x)
#define _set_x3_u1(x)   __set_x3_u1(x)
#define _set_y0_u0(x)   __set_y0_u0(x)
#define _set_y0_u1(x)   __set_y0_u1(x)
#define _set_y1_u0(x)   __set_y1_u0(x)
#define _set_y1_u1(x)   __set_y1_u1(x)
#define _set_y2_u0(x)   __set_y2_u0(x)
#define _set_y2_u1(x)   __set_y2_u1(x)
#define _set_y3_u0(x)   __set_y3_u0(x)
#define _set_y3_u1(x)   __set_y3_u1(x)
#define _set_x0_nu(x)   __set_x0_nu(x)
#define _set_x1_nu(x)   __set_x1_nu(x)
#define _set_x2_nu(x)   __set_x2_nu(x)
#define _set_x3_nu(x)   __set_x3_nu(x)
#define _set_y0_nu(x)   __set_y0_nu(x)
#define _set_y1_nu(x)   __set_y1_nu(x)
#define _set_y2_nu(x)   __set_y2_nu(x)
#define _set_y3_nu(x)   __set_y3_nu(x)
#endif

void _CopyXMemToXMem(uint32_t dest, uint32_t src, uint32_t n);
void _CopyYMemToXMem(uint32_t dest, uint32_t src, uint32_t n);
void _CopyXMemToYMem(uint32_t dest, uint32_t src, uint32_t n);
void _CopyYMemToYMem(uint32_t dest, uint32_t src, uint32_t n);

void _BurstXMem(uint32_t dest, int32_t val, int n);
void _BurstYMem(uint32_t dest, int32_t val, int n);
void _BurstSMem(int32_t* dest, int32_t val, int n);

void _BurstSMemToXMem(unsigned dest, const int32_t* src, int n);
void _BurstSMemToYMem(unsigned dest, const int32_t* src, int n);
void _BurstXMemToSMem(int32_t* dest, const unsigned src, int n);
void _BurstYMemToSMem(int32_t* dest, const unsigned src, int n);

void _BurstSMemToXMemSafe(unsigned dest, const int32_t* src, int n);
void _BurstSMemToYMemSafe(unsigned dest, const int32_t* src, int n);
void _BurstXMemToSMemSafe(int32_t* dest, const unsigned src, int n);
void _BurstYMemToSMemSafe(int32_t* dest, const unsigned src, int n);

void _BurstSMemToXMemPk24(unsigned dest, const int32_t* src, int n);
void _BurstSMemToYMemPk24(unsigned dest, const int32_t* src, int n);
void _BurstXMemToSMemPk24(int32_t* dest, const unsigned src, int n);
void _BurstYMemToSMemPk24(int32_t* dest, const unsigned src, int n);

void _WaitTillBurstComplete();
#endif



#else

// ARCv2EM DSP and Complex DSP extension
typedef enum  {
    DSP_CTRL_TRUNC = 0,
    DSP_CTRL_ROUNDING_UP = 2,
    DSP_CTRL_CONVERGENT = 3,
} DSP_CTRL_ROUNDING_MODE;

#define DSP_CTRL_GE  (1<<2)
#define DSP_CTRL_PA  (1<<3)
#define DSP_CTRL_SAT (1<<16)

#define DSP_FFT_CTRL_SC  (1)

#if !defined(_ARC)
int _cbflyhf0r(uint32_t b, uint32_t c);
int _cbflyhf1r(uint32_t b);

void _set_DSP_CTRL(uint32_t mode);
void _set_DSP_FFT_CTRL(uint32_t mode);
#endif

#endif

#ifndef _ARC 
uint32_t _get_SP();
void * _get_stack_start();
void * _get_stack_end();
#endif

#if !defined(__Xtimer0)||(!defined(_ARC))

/** \brief  Reset and enable timer0
*
*    This function drops timer count to 0, sets timer limit to max value, enables the generation of an interrupt after the timer has reached its limit condition
*/
void _START_TIMER_0();

/** \brief  Read timer0
*
*    This function returns the content of the COUNT0 Register.
*
*    \return               COUNT0 Register value
*/
uint32_t _READ_TIMER_0();

/** \brief  Disable timer0
*
*    This function disables the generation of an interrupt (but does not stop counting).
*/
void _STOP_TIMER_0();

#endif

#if !defined(__Xtimer1)||(!defined(_ARC))

/** \brief  Reset and enable timer1
*
*    This function drops timer count to 0, sets timer limit to max value, enables the generation of an interrupt after the timer has reached its limit condition
*/
void _START_TIMER_1();

/** \brief  Read timer1
*
*    This function returns the content of the COUNT1 Register.
*
*    \return               COUNT1 Register value
*/
uint32_t _READ_TIMER_1();

/** \brief  Disable timer1
*
*    This function disables the generation of an interrupt (but does not stop counting).
*/
void _STOP_TIMER_1();
#endif

/*
    Following functions required for Platform Library:

    1) void _cache_invalidate(void* address, uint32_t size);
    used to invalidate a block of cache at the given address

    2) void _cache_flush(void* address, uint32_t size);
    used to flush a block of cache at the given address
    
*/
// editable header file with external functions required for Platform library.
#include "helper_lib_pl.h"

#include "arc_emu.h"
#include "arc_native.h"

#endif // __ARC_H
