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
 
#ifndef __ARC_PLATFORM_NATIVE_H
#define __ARC_PLATFORM_NATIVE_H

// Using the MetaWare C/C++ compiler for any ARC target 
#if defined(_ARC) 

// Define HW intrinsics and registers for any ARC target except ARCv2EM DSP
#if !defined(_ARCVER_ARCv2EM) && !defined(_ARCVER_ARCv2HS)

#if (((_ARCVER < 0x21) || (_ARCVER > 0x26)) && ((_ARCVER < 0x31) || (_ARCVER > 0x35)) && ((_ARCVER < 0x40) || (_ARCVER > 0x42)))
#print warning: Target Processor not supported
#endif

#define XYCONFIG    (0x98)
#define BURSTSYS	(0x99)		// Burst system address
#define BURSTXYM	(0x9a)		// Burst X or Y memory address
#define BURSTSZ     (0x9b)		// Burst size
#define BURSTVAL	(0x9c)		// Burst fill value

// BURSTSZ bit values
#define BURSTSZ_BD(x)	(((x)&1) << 30) // Burst direction
#define BURSTSZ_MEM2XY	BURSTSZ_BD(1)
#define BURSTSZ_XY2MEM	BURSTSZ_BD(0)

#define BURSTSZ_RS(x)	(((x)&1) << 29) // Region select
#define BURSTSZ_XMEM	BURSTSZ_RS(0)
#define BURSTSZ_YMEM	BURSTSZ_RS(1)

#define BURSTSZ_F(x)	(((x)&1) << 28) // Burst fill
#define BURSTSZ_FILL	BURSTSZ_F(1)
#define BURSTSZ_XFER	BURSTSZ_F(0)

#define BURSTSZ_BS(x)	(((x)&3) << 24) // Bank select

#define BURSTSZ_SIZE(x)	(((x)&255))	// Burst transfer/fill size

// XYCONFIG bit values
#define XYCONFIG_BS(x)	((x)&3)
#define XYCONFIG_BP(x)	(((x)&1)<<4)
#define XYCONFIG_BU(x)	(((x)&1)<<5)
#define XYCONFIG_DP(x)	(((x)&1)<<6)
#define XYCONFIG_DU(x)	(((x)&1)<<7)
#define XYCONFIG_LO(x)	(((x)&1)<<8)
#define XYCONFIG_BI(x)	(((x)&1)<<9)
#define XYCONFIG_BURST_IN_PROGRESS		XYCONFIG_BP(1)
#define XYCONFIG_DMI_IN_PROGRESS		XYCONFIG_DP(1)
#define XYCONFIG_LOCK_OUT			XYCONFIG_LO(1)
#define XYCONFIG_BURST_INTERRUPT_DISABLE	XYCONFIG_BI(0)
#define XYCONFIG_BURST_INTERRUPT_ENABLE		XYCONFIG_BI(1)

// BURSTSZ bit values
#define BURSTSZ_BD(x)	(((x)&1) << 30) // Burst direction
#define BURSTSZ_MEM2XY	BURSTSZ_BD(1)
#define BURSTSZ_XY2MEM	BURSTSZ_BD(0)

#define BURSTSZ_RS(x)	(((x)&1) << 29) // Region select
#define BURSTSZ_XMEM	BURSTSZ_RS(0)
#define BURSTSZ_YMEM	BURSTSZ_RS(1)

#define BURSTSZ_F(x)	(((x)&1) << 28) // Burst fill
#define BURSTSZ_FILL	BURSTSZ_F(1)
#define BURSTSZ_XFER	BURSTSZ_F(0)

#define BURSTSZ_BS(x)	(((x)&3) << 24) // Bank select

#define BURSTSZ_SIZE(x)	(((x)&255))	// Burst transfer/fill size


#define REG_DC_IVDC      (0x47) // w  0  DATA CACHE  Invalidate Data Cache 
#define REG_DC_CTRL      (0x48) // rw  8  DATA CACHE  Control register  
#define REG_DC_CTRL_DC_BIT (1)  // DC[0]  Disable Cache: - Enables/Disables the cache ( 0 = Enable Cache , 1 = Disable Cache ) R/W 
#define REG_DC_CTRL_EB_BIT  (1 << 1) //  EB[1]  Bypass: - For ARC 700 the bypass is always enabled 1 =  Bypass Enabled  R (ARC700) R/W earlier cores 
#define REG_DC_CTRL_SB_BIT  (1 << 2) //  SB[2]  Success Bit: - Success of last cache operation (0 = failed, 1=success) R/W 
#define REG_DC_CTRL_RA_BIT  (3 << 3) //  RA[4]  Replacement Algorithm: - ARC 700 uses random replacement only (0 = Random Replacement, 1= round robin) ARC700 R, earlier cores R/W
#define REG_DC_CTRL_AT_BIT  (1 << 5) //   AT[5]  Address Debug Type: - Used for debug purposes when accessing cache RAMs (0 = Direct Cache RAM Access , 1 = Cache Controlled RAM Access ) R/W
#define REG_DC_CTRL_IM_BIT  (1 << 6) //  IM[6]  Invalidate Mode: - Selects the invalidate type ( 0 = invalidate only, 1= invalidate and flush) R/W
#define REG_DC_CTRL_M_BIT  (1 << 7)  //  LM[7] Lock Mode: - Selects the effect of a flush command on a locked entry (0=enable flush, 1 = disable flush) R/W
#define REG_DC_CTRL_FS_BIT  (1 << 8) //  FS[8]  Flush Status:- Status of the data cache flush mechanism (0 = idle, 1 = flush in progress) R
// Reserved [31:9] 
#define REG_DC_LDL       (0x49) // w  32  DATA CACHE  Lock data line 
#define REG_DC_IVDL      (0x4A) // w  32  DATA CACHE  Invalidate data line 
#define REG_DC_FLSH      (0x4B) // w  0  DATA CACHE  Flush data cache 
#define REG_DC_FLDL      (0x4C) // w  32  DATA CACHE  Flush data line 

#define REG_P_BASE_ADDR       (0x69) //  r  Peripheral base address

// Condition codes checking
#define __condition_z        _condition(0x01) // Zero _Z
#define __condition_nz       _condition(0x02) // Non-Zero
#define __condition_p        _condition(0x03) // Positive
#define __condition_n        _condition(0x04) // Negative
#define __condition_c        _condition(0x05) // Carry set, lower than (unsigned)
#define __condition_cc       _condition(0x06) // Carry clear, higher or same (unsigned)
#define __condition_v        _condition(0x07) // Over-flow set
#define __condition_nv       _condition(0x08) // Over-flow clear
#define __condition_gt       _condition(0x09) // Greater than (signed) (_N and _V and /_Z) or (/_N and /_V and /_Z)
#define __condition_ge       _condition(0x0A) // Greater than or equal to (signed) (_N and _V) or (/_N and /_V)
#define __condition_lt       _condition(0x0B) // Less than (signed) (_N and /_V) or (/_N and _V)
#define __condition_le       _condition(0x0C) // Less than or equal to (signed) _Z or (_N and /_V) or (/_N and _V)
#define __condition_hi       _condition(0x0D) // Higher than (unsigned) /_C and /_Z
#define __condition_ls       _condition(0x0E) // Lower than or same (unsigned) _C or _Z
#define __condition_pnz      _condition(0x0F) // Positive non-zero /_N and /_Z

#define REG_DSPRAM_BUILD         0x0079 //   r  Scratchpad and XY Memory Build Configuration Register

// Arithmetic and Logical Operations

#ifdef __CCAC__

#define _add(x, y) ((int32_t)(x) + (y))
#define _sub(x, y) ((int32_t)(x) - (y))
#define _and(x, y) ((int32_t)(x) & (y))
#define _or(x, y) ((int32_t)(x) | (y))
#define _xor(x, y) ((int32_t)(x) ^ (y))

#else

#pragma intrinsic(_add, name => "add");
#pragma intrinsic(_sub, name => "sub");
#pragma intrinsic(_and, name => "and");
#pragma intrinsic(_or, name => "or");
#pragma intrinsic(_xor, name => "xor");

#endif
//
#pragma intrinsic(_add_f, name => "add", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_adc, name => "adc", effects => "STATUS32:is_read");
//
#pragma intrinsic(_adc_f, name => "adc", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written:is_read");
//
#pragma intrinsic(_sub_f, name => "sub", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_sbc, name => "sbc", effects => "STATUS32:is_read");
#pragma intrinsic(_sbc_f, name => "sbc", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written:is_read");
//
#pragma intrinsic(_and_f, name => "and", set_flags => 1, flags => "zn");
// or
#pragma intrinsic(_or_f, name => "or", set_flags => 1, flags => "zn");
#pragma intrinsic(_bic, name => "bic");
#pragma intrinsic(_bic_f, name => "bic", set_flags => 1);
// xor
#pragma intrinsic(_xor_f, name => "xor", set_flags => 1, flags => "zn");
#pragma intrinsic(_max, name => "max");
#pragma intrinsic(_max_f, name => "max", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_min, name => "min");
#pragma intrinsic(_min_f, name => "min", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_rsub, name => "rsub");
#pragma intrinsic(_rsub_f, name => "rsub", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_bset, name => "bset");
#pragma intrinsic(_bset_f, name => "bset", set_flags => 1, flags => "zn");
#pragma intrinsic(_bclr, name => "bclr");
#pragma intrinsic(_bclr_f, name => "bclr", set_flags => 1, flags => "zn");
#pragma intrinsic(_bxor, name => "bxor");
#pragma intrinsic(_bxor_f, name => "bxor", set_flags => 1, flags => "zn");
#pragma intrinsic(_bmsk, name => "bmsk");
#pragma intrinsic(_bmsk_f, name => "bmsk", set_flags => 1, flags => "zn");
#pragma intrinsic(_add1, name => "add1");
#pragma intrinsic(_add1_f, name => "add1", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_add2, name => "add2");
#pragma intrinsic(_add2_f, name => "add2", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_add3, name => "add3");
#pragma intrinsic(_add3_f, name => "add3", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_sub1, name => "sub1");
#pragma intrinsic(_sub1_f, name => "sub1", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_sub2, name => "sub2");
#pragma intrinsic(_sub2_f, name => "sub2", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_sub3, name => "sub3");
#pragma intrinsic(_sub3_f, name => "sub3", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");

// no result instructions
#pragma intrinsic(_btst, opcode => 0x04, sub_opcode => 0x11, set_flags => 1, flags => "zn");
#pragma intrinsic(_tst, opcode => 0x04, sub_opcode => 0x0b, set_flags => 1, flags => "zn");
#pragma intrinsic(_cmp, opcode => 0x04, sub_opcode => 0x0c, set_flags => 1, flags => "zcnv");
#pragma intrinsic(_rcmp, opcode => 0x04, sub_opcode => 0x0d, set_flags => 1, flags => "zcnv");

// Single Operand Instructions
#pragma intrinsic(_mov, name => "mov");
#pragma intrinsic(_mov_f, name => "mov", set_flags => 1, flags => "zn", effects => "STATUS32:is_written");
#pragma intrinsic(_sexb, name => "sexb");
#pragma intrinsic(_sexb_f, name => "sexb", set_flags => 1, flags => "zn", effects => "STATUS32:is_written");
#pragma intrinsic(_sexw, name => "sexw");
#pragma intrinsic(_sexw_f, name => "sexw", set_flags => 1, flags => "zn", effects => "STATUS32:is_written");
#pragma intrinsic(_extb, name => "extb");
#pragma intrinsic(_extb_f, name => "extb", set_flags => 1, flags => "zn", effects => "STATUS32:is_written");
#pragma intrinsic(_extw, name => "extw");
#pragma intrinsic(_extw_f, name => "extw", set_flags => 1, flags => "zn", effects => "STATUS32:is_written");
#pragma intrinsic(_not, name => "not");
#pragma intrinsic(_not_f, name => "not", set_flags => 1, flags => "zn", effects => "STATUS32:is_written");
#pragma intrinsic(_neg, name => "neg");
#pragma intrinsic(_neg_f, name => "neg", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_abs, name => "abs");
#pragma intrinsic(_abs_f, name => "abs", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#ifdef __CCAC__

#else
#pragma intrinsic(_asl1, name => "asl");
#pragma intrinsic(_asl1_f, name => "asl", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_asr1, name => "asr");
#pragma intrinsic(_asr1_f, name => "asr", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_lsr1, name => "lsr");
#pragma intrinsic(_lsr1_f, name => "lsr", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_ror1, name => "ror");
#pragma intrinsic(_ror1_f, name => "ror", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#endif
#pragma intrinsic(_rlc, name => "rlc", effects => "STATUS32:is_read");
#pragma intrinsic(_rlc_f, name => "rlc", set_flags => 1, flags => "zcn", effects => "STATUS32:is_written:is_read");
#pragma intrinsic(_rrc, name => "rrc", effects => "STATUS32:is_read");
#pragma intrinsic(_rrc_f, name => "rrc", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written:is_read");

#ifdef __CCAC__
#pragma intrinsic(_flag, name => "flag");
#else
#pragma intrinsic(_flag, name => "flag", effects => "STATUS32:is_written");
#endif

// _nop() 
#pragma intrinsic(_brk, name => "brk", assume_volatile => 1); 
#pragma intrinsic(_sync, name => "sync", assume_volatile => 1); 

#ifndef __CCAC__
int32_t __mov_volatile(int32_t);
#pragma intrinsic(__mov_volatile, name => "mov", assume_volatile => 1);
#endif

static _Inline uint32_t _get_STATUS32()
{
#ifdef __CCAC__
    return _lr(STATUS32);
#else
    return __mov_volatile(_lr(STATUS32));
#endif
}

#ifdef __Xbarrel_shifter
#pragma intrinsic(_asl, name => "asl");
#pragma intrinsic(_asl_f, name => "asl", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_asr, name => "asr");
#pragma intrinsic(_asr_f, name => "asr", set_flags => 1, flags => "zcn", effects => "STATUS32:is_written");
#pragma intrinsic(_lsr, name => "lsr");
#pragma intrinsic(_lsr_f, name => "lsr", set_flags => 1, flags => "zcn", effects => "STATUS32:is_written");
#pragma intrinsic(_ror, name => "ror");
#pragma intrinsic(_ror_f, name => "ror", set_flags => 1, flags => "zcn", effects => "STATUS32:is_written");
#endif // __Xbarrel_shifter

#ifdef __Xnorm
#pragma intrinsic(_norm, name => "norm");
#pragma intrinsic(_norm_f, name => "norm", set_flags => 1, flags => "zn", effects => "STATUS32:is_written");
#pragma intrinsic(_normw, name => "normw");
#pragma intrinsic(_normw_f, name => "normw", set_flags => 1, flags => "zn", effects => "STATUS32:is_written");
#endif // __Xnorm

#ifdef __Xswap
#pragma intrinsic(_swap, name => "swap");
#pragma intrinsic(_swap_f, name => "swap", set_flags => 1, flags => "zn", effects => "STATUS32:is_written");
#endif // __Xswap

#ifdef __Xmpy16
#pragma intrinsic(_mpyuw, name => "mpyuw");
#pragma intrinsic(_mpyuw_f, name => "mpyuw",set_flags => 1, flags => "znc", effects => "STATUS32:is_written");
#pragma intrinsic(_mpyw, name => "mpyw");
#pragma intrinsic(_mpyw_f, name => "mpyw",set_flags => 1, flags => "znc", effects => "STATUS32:is_written");
#endif

#if defined(__Xea) && !defined(EMU__Xea)  // Extended Arithmetic Library
#pragma intrinsic(_adds, name => "adds");
#pragma intrinsic(_adds_f, name => "adds", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_subs, name => "subs");
#pragma intrinsic(_subs_f, name => "subs", set_flags => 1, flags => "zcnv", effects => "STATUS32:is_written");
#pragma intrinsic(_divaw, name => "divaw");
#pragma intrinsic(_asls, name => "asls");
#pragma intrinsic(_asls_f, name => "asls", set_flags => 1, flags => "znv");
#pragma intrinsic(_asrs, name => "asrs");
#pragma intrinsic(_asrs_f, name => "asrs", set_flags => 1, flags => "znv");
#pragma intrinsic(_addsdw, name => "addsdw");
#pragma intrinsic(_subsdw, name => "subsdw");

#pragma intrinsic(_sat16, name => "sat16");
#pragma intrinsic(_sat16_f, name => "sat16", set_flags => 1, flags => "znv");
#pragma intrinsic(_rnd16, name => "rnd16");
#pragma intrinsic(_rnd16_f, name => "rnd16", set_flags => 1, flags => "znv");
#pragma intrinsic(_abssw, name => "abssw");
#pragma intrinsic(_abssw_f, name => "abssw", set_flags => 1, flags => "znv");
#pragma intrinsic(_abss, name => "abss");
#pragma intrinsic(_abss_f, name => "abss", set_flags => 1, flags => "znv");
#pragma intrinsic(_negsw, name => "negsw");
#pragma intrinsic(_negsw_f, name => "negsw", set_flags => 1, flags => "znv");
#pragma intrinsic(_negs, name => "negs");
#pragma intrinsic(_negs_f, name => "negs", set_flags => 1, flags => "znv");
#endif // __Xea

#if defined(__Xmul32x16) || defined(__Xxmac_d16)

#define ACC1    56
#define ACC2    57

static _Inline int32_t  _get_ACC1() { return _core_read(ACC1); }
static _Inline uint32_t _get_ACC2() { return _core_read(ACC2); }

#ifdef __CCAC__
static _Inline void  _set_ACC1(uint32_t x) { _core_write(x,ACC1); }
static _Inline void  _set_ACC2(uint32_t x) { _core_write(x,ACC2); }
#else
static _Inline void  _set_ACC1(uint32_t x) { _core_write(__mov_volatile(x),ACC1); }
static _Inline void  _set_ACC2(uint32_t x) { _core_write(__mov_volatile(x),ACC2); }
#endif

#endif

#if defined(__Xmul32x16) || defined(__Xxmac_d16) || defined(__Xxmac_24)
#pragma aux_register(AUX_MACMODE, name => "aux_macmode")
static _Inline uint32_t _get_MACMODE() { return _lr(AUX_MACMODE); }
static _Inline void     _set_MACMODE(uint32_t mode) { _sr(mode,AUX_MACMODE); }
#endif

#ifdef __Xmul32x16 // Multiply-Accumulate with 32-bit X 16-Bit Data
#pragma aux_register(AUX_XMACLW_H, name => "aux_xmac1632h")
#pragma aux_register(AUX_XMACLW_L, name => "aux_xmac1632l")
static _Inline uint32_t _get_AUX_XMACLW_H() { return _lr(AUX_XMACLW_H); }
static _Inline uint32_t _get_AUX_XMACLW_L() { return _lr(AUX_XMACLW_L); }
static _Inline void _set_AUX_XMACLW_H(uint32_t val) { _sr(val, AUX_XMACLW_H); }
static _Inline void _set_AUX_XMACLW_L(uint32_t val) { _sr(val, AUX_XMACLW_L); }
#pragma intrinsic(_mululw, name => "mululw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_mululw_f, name => "mululw", latency_cycles => 2, set_flags => 1, flags => "znv", effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_mullw, name => "mullw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_mullw_f, name => "mullw", latency_cycles => 2, set_flags => 1, flags => "znv", effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_mulflw, name => "mulflw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_mulflw_f, name => "mulflw", latency_cycles => 2, set_flags => 1, flags => "znv", effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_maclw, name => "maclw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_maclw_f, name => "maclw", set_flags => 1, latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_macflw, name => "macflw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_macflw_f, name => "macflw", set_flags => 1, latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_machulw, name => "machulw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_machulw_f, name => "machulw", latency_cycles => 2, set_flags => 1, flags => "znv", effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_machlw, name => "machlw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_machlw_f, name => "machlw", latency_cycles => 2, set_flags => 1, flags => "znv", effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_machflw, name => "machflw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_machflw_f, name => "machflw", latency_cycles => 2, set_flags => 1, flags => "znv", effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_mulhlw, name => "mulhlw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_mulhlw_f, name => "mulhlw", latency_cycles => 2, set_flags => 1, flags => "znv", effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_mulhflw, name => "mulhflw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#pragma intrinsic(_mulhflw_f, name => "mulhflw", latency_cycles => 2, set_flags => 1, flags => "znv", effects => "reg=56:is_written;reg=57:is_written;auxreg=159:is_written:is_read;auxreg=160:is_written:is_read");
#endif // __Xmul32x16

#ifdef __Xxmac_d16 // Multiply-Accumulate with Dual-Word Data
#pragma aux_register(AUX_FBF_STORE_16, name => "aux_fbf_store_16");
#pragma aux_register(AUX_XMAC0, name => "aux_xmac0")
#pragma aux_register(AUX_XMAC1, name => "aux_xmac1")
#pragma aux_register(AUX_XMAC2, name => "aux_xmac2")
static _Inline uint32_t _get_AUX_XMAC0() { return _lr(AUX_XMAC0); }
static _Inline uint32_t _get_AUX_XMAC1() { return _lr(AUX_XMAC1); }
static _Inline uint32_t _get_AUX_XMAC2() { return _lr(AUX_XMAC2); }
static _Inline void _set_AUX_XMAC0(uint32_t val) { _sr(val, AUX_XMAC0); }
static _Inline void _set_AUX_XMAC1(uint32_t val) { _sr(val, AUX_XMAC1); }
static _Inline void _set_AUX_XMAC2(uint32_t val) { _sr(val, AUX_XMAC2); }
static _Inline uint32_t _get_AUX_FBF_STORE_16() { return _lr(AUX_FBF_STORE_16); }
static _Inline void _set_AUX_FBF_STORE_16(uint32_t val) { _sr(val, AUX_FBF_STORE_16); }

#pragma intrinsic(_muldw, name => "muldw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_muldw_f, name => "muldw", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_muludw, name => "muludw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_muludw_f, name => "muludw", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_mulrdw, name => "mulrdw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_mulrdw_f, name => "mulrdw", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_macdw, name => "macdw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_macdw_f, name => "macdw", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_macudw, name => "macudw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_macudw_f, name => "macudw", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_macrdw, name => "macrdw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_macrdw_f, name => "macrdw", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_msubdw, name => "msubdw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_msubdw_f, name => "msubdw", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_cmacrdw, name => "cmacrdw", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_cmacrdw_f, name => "cmacrdw", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read");
#pragma intrinsic(_fbfdw, name => "fbfdw", assume_volatile => 1);
#endif // __Xxmac_d16

#ifdef __Xxmac_24 // Multiply-Accumulate with 24-bit Data
#pragma aux_register(AUX_XMAC0_24, name => "aux_xmac0_24")
#pragma aux_register(AUX_XMAC1_24, name => "aux_xmac1_24")
#pragma aux_register(AUX_XMAC2_24, name => "aux_xmac2_24")
static _Inline uint32_t _get_AUX_XMAC0_24() { return _lr(AUX_XMAC0_24); }
static _Inline uint32_t _get_AUX_XMAC1_24() { return _lr(AUX_XMAC1_24); }
static _Inline uint32_t _get_AUX_XMAC2_24() { return _lr(AUX_XMAC2_24); }
static _Inline void _set_AUX_XMAC0_24(uint32_t val) { _sr(val, AUX_XMAC0_24); }
static _Inline void _set_AUX_XMAC1_24(uint32_t val) { _sr(val, AUX_XMAC1_24); }
static _Inline void _set_AUX_XMAC2_24(uint32_t val) { _sr(val, AUX_XMAC2_24); }
#pragma intrinsic(_mult, name => "mult", latency_cycles => 2, effects => "aux_xmac0_24:is_written;aux_xmac1_24:is_written;aux_xmac2_24:is_written;");
#pragma intrinsic(_mult_f, name => "mult", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "aux_xmac0_24:is_written;aux_xmac1_24:is_written;aux_xmac2_24:is_written;");
#pragma intrinsic(_mulut, name => "mulut", latency_cycles => 2, effects => "aux_xmac0_24:is_written:is_read;aux_xmac1_24:is_written:is_read;aux_xmac2_24:is_written:is_read;");
#pragma intrinsic(_mulut_f, name => "mulut", set_flags => 1, flags => "znv", latency_cycles => 2,  effects => "aux_xmac0_24:is_written:is_read;aux_xmac1_24:is_written:is_read;aux_xmac2_24:is_written:is_read;");
#pragma intrinsic(_mulrt, name => "mulrt", latency_cycles => 2, effects => "aux_xmac0_24:is_written:is_read;aux_xmac1_24:is_written:is_read;aux_xmac2_24:is_written:is_read;");
#pragma intrinsic(_mulrt_f, name => "mulrt", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "aux_xmac0_24:is_written:is_read;aux_xmac1_24:is_written:is_read;aux_xmac2_24:is_written:is_read;");
#pragma intrinsic(_mact, name => "mact", latency_cycles => 2, effects => "aux_xmac0_24:is_written:is_read;aux_xmac1_24:is_written:is_read;aux_xmac2_24:is_written:is_read;");
#pragma intrinsic(_mact_f, name => "mact", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "aux_xmac0_24:is_written:is_read;aux_xmac1_24:is_written:is_read;aux_xmac2_24:is_written:is_read;");
#pragma intrinsic(_macrt, name => "macrt", latency_cycles => 2, effects => "aux_xmac0_24:is_written:is_read;aux_xmac1_24:is_written:is_read;aux_xmac2_24:is_written:is_read;");
#pragma intrinsic(_macrt_f, name => "macrt", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "aux_xmac0_24:is_written:is_read;aux_xmac1_24:is_written:is_read;aux_xmac2_24:is_written:is_read;");
#pragma intrinsic(_msubt, name => "msubt", latency_cycles => 2, effects => "aux_xmac0_24:is_written:is_read;aux_xmac1_24:is_written:is_read;aux_xmac2_24:is_written:is_read;");
#pragma intrinsic(_msubt_f, name => "msubt", set_flags => 1, flags => "znv", latency_cycles => 2, effects => "aux_xmac0_24:is_written:is_read;aux_xmac1_24:is_written:is_read;aux_xmac2_24:is_written:is_read;");
#endif // __Xxmac_24

#ifdef __Xdmulpf // Multiply-Accumulate withPseudo-Floating Point Data
#pragma intrinsic(_dmulpf_xy, dmulpf => 1, opcode => 0x05, sub_opcode => 0x3A, latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read;auxreg=0x800:is_written:is_read;auxreg=0x801:is_written:is_read;auxreg=0x802:is_written:is_read;auxreg=0x803:is_written:is_read", assume_volatile => 1);
#pragma intrinsic(_dmacpf_xy, dmulpf => 1, opcode => 0x05, sub_opcode => 0x3B, latency_cycles => 2, effects => "reg=56:is_written;reg=57:is_written;aux_xmac0:is_written:is_read;aux_xmac1:is_written:is_read;aux_xmac2:is_written:is_read;auxreg=0x800:is_written:is_read;auxreg=0x801:is_written:is_read;auxreg=0x802:is_written:is_read;auxreg=0x803:is_written:is_read", assume_volatile => 1);
#endif // __Xdmulpf

#ifdef __Xcrc // Variable Polynomial CRC
#pragma aux_register(AUX_CRC_POLY, name => "aux_crc_poly")
#pragma aux_register(AUX_CRC_MODE, name => "aux_crc_mode")
static _Inline uint32_t    _get_AUX_CRC_POLY() { return _lr(AUX_CRC_POLY); }
static _Inline uint32_t    _get_AUX_CRC_MODE() { return _lr(AUX_CRC_MODE); }
static _Inline void        _set_AUX_CRC_POLY(uint32_t x) { _sr(x, AUX_CRC_POLY); }
static _Inline void        _set_AUX_CRC_MODE(uint32_t x) { _sr(x, AUX_CRC_MODE); }
#pragma intrinsic(_crc, opcode => 0x05, sub_opcode => 0x2C, latency_cycles => 2);
#endif // __Xcrc

#ifdef __Xxy // XY memory addressing

#define XYCONFIG_REGISTER (0x98)

/* Symbolic names for important bits of XYCONFIG_REGISTER */
#define XYCONFIG_REGISTER_BP (1<<4)
#define XYCONFIG_REGISTER_BI (1<<9)
#define XYCONFIG_REGISTER_PX (1<<10)
#define XYCONFIG_REGISTER_PY (1<<11)
#define XYCONFIG_REGISTER_FX (1<<12)
#define XYCONFIG_REGISTER_FY (1<<13)

static _Inline uint32_t _get_xbase() { return _lr(XYLSBASEX); }
static _Inline uint32_t _get_ybase() { return _lr(XYLSBASEY); }

static _Inline void _set_xbase(uint32_t xbase) { _sr((unsigned int)xbase, XYLSBASEX); }
static _Inline void _set_ybase(uint32_t ybase) { _sr((unsigned int)ybase, XYLSBASEY); }

static _Inline void _flushx() { _sr(XYCONFIG_REGISTER_FX, XYCONFIG_REGISTER); }
static _Inline void _flushy() { _sr(XYCONFIG_REGISTER_FY, XYCONFIG_REGISTER); }

#define XMEM _lr(XYLSBASEX)
#define YMEM _lr(XYLSBASEY)

static _Inline uint32_t _ax0() { return _lr(AX0); }
static _Inline uint32_t _ax1() { return _lr(AX1); }
static _Inline uint32_t _ax2() { return _lr(AX2); }
static _Inline uint32_t _ax3() { return _lr(AX3); }
static _Inline uint32_t _ay0() { return _lr(AY0); }
static _Inline uint32_t _ay1() { return _lr(AY1); }
static _Inline uint32_t _ay2() { return _lr(AY2); }
static _Inline uint32_t _ay3() { return _lr(AY3); }

static _Inline void _set_ax0(uint32_t a) { _sr(a, AX0); }
static _Inline void _set_ax1(uint32_t a) { _sr(a, AX1); }
static _Inline void _set_ax2(uint32_t a) { _sr(a, AX2); }
static _Inline void _set_ax3(uint32_t a) { _sr(a, AX3); }
static _Inline void _set_ay0(uint32_t a) { _sr(a, AY0); }
static _Inline void _set_ay1(uint32_t a) { _sr(a, AY1); }
static _Inline void _set_ay2(uint32_t a) { _sr(a, AY2); }
static _Inline void _set_ay3(uint32_t a) { _sr(a, AY3); }

static _Inline uint32_t _mx00() { return _lr(MX00); }
static _Inline uint32_t _mx01() { return _lr(MX01); }
static _Inline uint32_t _mx10() { return _lr(MX10); }
static _Inline uint32_t _mx11() { return _lr(MX11); }
static _Inline uint32_t _mx20() { return _lr(MX20); }
static _Inline uint32_t _mx21() { return _lr(MX21); }
static _Inline uint32_t _mx30() { return _lr(MX30); }
static _Inline uint32_t _mx31() { return _lr(MX31); }
static _Inline uint32_t _my00() { return _lr(MY00); }
static _Inline uint32_t _my01() { return _lr(MY01); }
static _Inline uint32_t _my10() { return _lr(MY10); }
static _Inline uint32_t _my11() { return _lr(MY11); }
static _Inline uint32_t _my20() { return _lr(MY20); }
static _Inline uint32_t _my21() { return _lr(MY21); }
static _Inline uint32_t _my30() { return _lr(MY30); }
static _Inline uint32_t _my31() { return _lr(MY31); }

static _Inline void _set_mx00(uint32_t m) { _sr(m, MX00); }
static _Inline void _set_mx01(uint32_t m) { _sr(m, MX01); }
static _Inline void _set_mx10(uint32_t m) { _sr(m, MX10); }
static _Inline void _set_mx11(uint32_t m) { _sr(m, MX11); }
static _Inline void _set_mx20(uint32_t m) { _sr(m, MX20); }
static _Inline void _set_mx21(uint32_t m) { _sr(m, MX21); }
static _Inline void _set_mx30(uint32_t m) { _sr(m, MX30); }
static _Inline void _set_mx31(uint32_t m) { _sr(m, MX31); }
static _Inline void _set_my00(uint32_t m) { _sr(m, MY00); }
static _Inline void _set_my01(uint32_t m) { _sr(m, MY01); }
static _Inline void _set_my10(uint32_t m) { _sr(m, MY10); }
static _Inline void _set_my11(uint32_t m) { _sr(m, MY11); }
static _Inline void _set_my20(uint32_t m) { _sr(m, MY20); }
static _Inline void _set_my21(uint32_t m) { _sr(m, MY21); }
static _Inline void _set_my30(uint32_t m) { _sr(m, MY30); }
static _Inline void _set_my31(uint32_t m) { _sr(m, MY31); }

#pragma core_register(REG_X0_U0, side_effects => "rw")
#pragma core_register(REG_X0_U1, side_effects => "rw")
#pragma core_register(REG_X1_U0, side_effects => "rw")
#pragma core_register(REG_X1_U1, side_effects => "rw")
#pragma core_register(REG_X2_U0, side_effects => "rw")
#pragma core_register(REG_X2_U1, side_effects => "rw")
#pragma core_register(REG_X3_U0, side_effects => "rw")
#pragma core_register(REG_X3_U1, side_effects => "rw")

#pragma core_register(REG_Y0_U0, side_effects => "rw")
#pragma core_register(REG_Y0_U1, side_effects => "rw")
#pragma core_register(REG_Y1_U0, side_effects => "rw")
#pragma core_register(REG_Y1_U1, side_effects => "rw")
#pragma core_register(REG_Y2_U0, side_effects => "rw")
#pragma core_register(REG_Y2_U1, side_effects => "rw")
#pragma core_register(REG_Y3_U0, side_effects => "rw")
#pragma core_register(REG_Y3_U1, side_effects => "rw")

static _Inline int32_t _x0_u0() { return (int32_t)_core_read(REG_X0_U0); }
static _Inline int32_t _x0_u1() { return (int32_t)_core_read(REG_X0_U1); }
static _Inline int32_t _x1_u0() { return (int32_t)_core_read(REG_X1_U0); }
static _Inline int32_t _x1_u1() { return (int32_t)_core_read(REG_X1_U1); }
static _Inline int32_t _x2_u0() { return (int32_t)_core_read(REG_X2_U0); }
static _Inline int32_t _x2_u1() { return (int32_t)_core_read(REG_X2_U1); }
static _Inline int32_t _x3_u0() { return (int32_t)_core_read(REG_X3_U0); }
static _Inline int32_t _x3_u1() { return (int32_t)_core_read(REG_X3_U1); }
static _Inline int32_t _y0_u0() { return (int32_t)_core_read(REG_Y0_U0); }
static _Inline int32_t _y0_u1() { return (int32_t)_core_read(REG_Y0_U1); }
static _Inline int32_t _y1_u0() { return (int32_t)_core_read(REG_Y1_U0); }
static _Inline int32_t _y1_u1() { return (int32_t)_core_read(REG_Y1_U1); }
static _Inline int32_t _y2_u0() { return (int32_t)_core_read(REG_Y2_U0); }
static _Inline int32_t _y2_u1() { return (int32_t)_core_read(REG_Y2_U1); }
static _Inline int32_t _y3_u0() { return (int32_t)_core_read(REG_Y3_U0); }
static _Inline int32_t _y3_u1() { return (int32_t)_core_read(REG_Y3_U1); }
static _Inline int32_t _x0_nu() { return (int32_t)_core_read(REG_X0_NU); }
static _Inline int32_t _x1_nu() { return (int32_t)_core_read(REG_X1_NU); }
static _Inline int32_t _x2_nu() { return (int32_t)_core_read(REG_X2_NU); }
static _Inline int32_t _x3_nu() { return (int32_t)_core_read(REG_X3_NU); }
static _Inline int32_t _y0_nu() { return (int32_t)_core_read(REG_Y0_NU); }
static _Inline int32_t _y1_nu() { return (int32_t)_core_read(REG_Y1_NU); }
static _Inline int32_t _y2_nu() { return (int32_t)_core_read(REG_Y2_NU); }
static _Inline int32_t _y3_nu() { return (int32_t)_core_read(REG_Y3_NU); }

static _Inline void _set_x0_u0(int32_t x) { _core_write(x, REG_X0_U0); }
static _Inline void _set_x0_u1(int32_t x) { _core_write(x, REG_X0_U1); }
static _Inline void _set_x1_u0(int32_t x) { _core_write(x, REG_X1_U0); }
static _Inline void _set_x1_u1(int32_t x) { _core_write(x, REG_X1_U1); }
static _Inline void _set_x2_u0(int32_t x) { _core_write(x, REG_X2_U0); }
static _Inline void _set_x2_u1(int32_t x) { _core_write(x, REG_X2_U1); }
static _Inline void _set_x3_u0(int32_t x) { _core_write(x, REG_X3_U0); }
static _Inline void _set_x3_u1(int32_t x) { _core_write(x, REG_X3_U1); }
static _Inline void _set_y0_u0(int32_t x) { _core_write(x, REG_Y0_U0); }
static _Inline void _set_y0_u1(int32_t x) { _core_write(x, REG_Y0_U1); }
static _Inline void _set_y1_u0(int32_t x) { _core_write(x, REG_Y1_U0); }
static _Inline void _set_y1_u1(int32_t x) { _core_write(x, REG_Y1_U1); }
static _Inline void _set_y2_u0(int32_t x) { _core_write(x, REG_Y2_U0); }
static _Inline void _set_y2_u1(int32_t x) { _core_write(x, REG_Y2_U1); }
static _Inline void _set_y3_u0(int32_t x) { _core_write(x, REG_Y3_U0); }
static _Inline void _set_y3_u1(int32_t x) { _core_write(x, REG_Y3_U1); }
static _Inline void _set_x0_nu(int32_t x) { _core_write(x, REG_X0_NU); }
static _Inline void _set_x1_nu(int32_t x) { _core_write(x, REG_X1_NU); }
static _Inline void _set_x2_nu(int32_t x) { _core_write(x, REG_X2_NU); }
static _Inline void _set_x3_nu(int32_t x) { _core_write(x, REG_X3_NU); }
static _Inline void _set_y0_nu(int32_t x) { _core_write(x, REG_Y0_NU); }
static _Inline void _set_y1_nu(int32_t x) { _core_write(x, REG_Y1_NU); }
static _Inline void _set_y2_nu(int32_t x) { _core_write(x, REG_Y2_NU); }
static _Inline void _set_y3_nu(int32_t x) { _core_write(x, REG_Y3_NU); }

/*
    Data Transfer routines
*/
static _Inline void _CopyXMemToXMem(uint32_t dest, uint32_t src, uint32_t n)
{
    uint32_t i;
    int ax0_tmp = _ax0();
    int ax1_tmp = _ax1();
    int mx00_tmp = _mx00();
    int mx10_tmp = _mx10();

    _set_mx00(1);
    _set_ax0(dest);
    _set_mx10(1);
    _set_ax1(src);
    _nop();
    _nop();
    for (i = 0; i < n; i++) {
        _set_x0_u0(_x1_u0());
    }

    _set_ax0(ax0_tmp);
    _set_mx00(mx00_tmp);
    _set_ax1(ax1_tmp);
    _set_mx10(mx10_tmp);
}

static _Inline void _CopyYMemToXMem(uint32_t dest, uint32_t src, uint32_t n)
{
    uint32_t i;
    int ax0_tmp = _ax0();
    int ay0_tmp = _ay0();
    int mx00_tmp = _mx00();
    int my00_tmp = _my00();

    _set_mx00(1);
    _set_ax0(dest);
    _set_my00(1);
    _set_ay0(src);
    _nop();
    _nop();
    for (i = 0; i < n; i++) {
        _set_x0_u0(_y0_u0());
    }

    _set_ax0(ax0_tmp);
    _set_mx00(mx00_tmp);
    _set_ay0(ay0_tmp);
    _set_my00(my00_tmp);
}

static _Inline void _CopyXMemToYMem(uint32_t dest, uint32_t src, uint32_t n)
{
    uint32_t i;
    int ay0_tmp = _ay0();
    int ax0_tmp = _ax0();
    int my00_tmp = _my00();
    int mx00_tmp = _mx00();

    _set_my00(1);
    _set_ay0(dest);
    _set_mx00(1);
    _set_ax0(src);
    _nop();
    _nop();
    for (i = 0; i < n; i++) {
        _set_y0_u0(_x0_u0());
    }

    _set_ay0(ay0_tmp);
    _set_my00(my00_tmp);
    _set_ax0(ax0_tmp);
    _set_mx00(mx00_tmp);
}

static _Inline void _CopyYMemToYMem(uint32_t dest, uint32_t src, uint32_t n)
{
    uint32_t i;
    int ay0_tmp = _ay0();
    int ay1_tmp = _ay1();
    int my00_tmp = _my00();
    int my10_tmp = _my10();

    _set_my00(1);
    _set_ay0(dest);
    _set_my10(1);
    _set_ay1(src);
    _nop();
    _nop();
    for (i = 0; i < n; i++) {
        _set_y0_u0(_y1_u0());
    }

    _set_ay0(ay0_tmp);
    _set_my00(my00_tmp);
    _set_ay1(ay1_tmp);
    _set_my10(my10_tmp);
}


static _Inline void _BurstXMem(uint32_t dest, int32_t val, int n) 
{
    // Initiate bursting
    _sr(dest,                      0x9a);
    _sr(val,                       0x9c);
    _sr(0x100,                     0x98);
    _sr((((n<<2)-1) | 0x50000000), 0x9b);
    // Wait till burst complete
    while (_lr(0x98) & 0x10);
} /* _BurstXMem */

static _Inline void _BurstYMem(uint32_t dest, int32_t val, int n) 
{
    // Initiate bursting
    _sr(dest,                      0x9a);
    _sr(val,                       0x9c);
    _sr(0x100,                     0x98);
    _sr((((n<<2)-1) | 0x70000000), 0x9b);
    // Wait till burst complete
    while (_lr(0x98) & 0x10);
} /* _BurstYMem */

static _Inline void _BurstSMem(int32_t* dest, int32_t val, int n)
{
    // Initiate bursting
    _sr((uint32_t)dest,            0x99);
    _sr(val,                       0x9c);
    _sr(0x100,                     0x98);
    _sr((((n<<2)-1) | 0x10000000), 0x9b);
    // Wait till burst complete
    while (_lr(0x98) & 0x10);
} /* _BurstSMem */

static _Inline void _BurstSMemToXMem(unsigned dest, const int32_t* src, int n) 
{
    // Initiate bursting
    _sr(dest,                      0x9a);
    _sr((uint32_t)src,             0x99);
    _sr(0x100,                     0x98);
    _sr((((n<<2)-1) | 0x40000000), 0x9b);
    // Wait till burst complete
    while (_lr(0x98) & 0x10);
} /* _BurstSMemToXMem */

static _Inline void _BurstSMemToYMem(unsigned dest, const int32_t* src, int n) 
{
    // Initiate bursting
    _sr(dest,                      0x9a);
    _sr((uint32_t)src,             0x99);
    _sr(0x100,                     0x98);
    _sr((((n<<2)-1) | 0x60000000), 0x9b);
    // Wait till burst complete
    while (_lr(0x98) & 0x10);
} /* _BurstSMemToYMem */

static _Inline void _BurstXMemToSMem(int32_t* dest, const unsigned src, int n) 
{
    // Initiate bursting
    _sr(src,                       0x9a);
    _sr((uint32_t)dest,            0x99);
    _sr(0x100,                     0x98);
    _sr((((n<<2)-1) | 0x00000000), 0x9b);
    // Wait till burst complete
    while (_lr(0x98) & 0x10);
} /* _BurstXMemToSMem */

static _Inline void _BurstYMemToSMem(int32_t* dest, const unsigned src, int n) 
{
    // Initiate bursting
    _sr(src,                       0x9a);
    _sr((uint32_t)dest,            0x99);
    _sr(0x100,                     0x98);
    _sr((((n<<2)-1) | 0x20000000), 0x9b);

    // Wait till burst complete
    while (_lr(0x98) & 0x10);
} /* _BurstYMemToSMem */

static _Inline void _BurstSMemToXMemPk24(unsigned dest, const int32_t* src, int n)
{
    // Initiate bursting
    _sr(dest,                       0x9a);
    _sr((uint32_t)src,              0x99);
    _sr(100,                        0x98);
    _sr((((n<<2)-1) | 0x40080000),  0x9b);
    // Wait till burst complete
    while (_lr(0x98) & 0x10);
}

static _Inline void _BurstSMemToYMemPk24(unsigned dest, const int32_t* src, int n)
{
    // Initiate bursting
    _sr(dest,                       0x9a);
    _sr((uint32_t)src,              0x99);
    _sr(100,                        0x98);
    _sr((((n<<2)-1) | 0x60080000),  0x9b);
    // Wait till burst complete
    while (_lr(0x98) & 0x10);
}

static _Inline void _BurstXMemToSMemPk24(int32_t* dest, const unsigned src, int n)
{
    // Initiate bursting
    _sr(src,                        0x9a);
    _sr((uint32_t)dest,             0x99);
    _sr(100,                        0x98);
    _sr((((n<<2)-1) | 0x00080000),  0x9b);
    // Wait till burst complete
    while (_lr(0x98) & 0x10);
}

static _Inline void _BurstYMemToSMemPk24(int32_t* dest, const unsigned src, int n)
{
    // Initiate bursting
    _sr(src,                        0x9a);
    _sr((uint32_t)dest,             0x99);
    _sr(100,                        0x98);
    _sr((((n<<2)-1) | 0x20080000),  0x9b);
    // Wait till burst complete
    while (_lr(0x98) & 0x10);
}

static _Inline void _WaitTillBurstComplete() 
{
    // Wait till burst complete
    while (_lr(0x98) & 0x10);
} /* _WaitTillBurstComplete */

#endif // __Xxy

#else // !(defined(_ARCVER_ARCv2EM) && (defined(_Xdsp) || defined(_Xdsp_complex)))

/*
#ifdef __Xdsp // ARCv2EM DSP Extensions

#pragma core_register(58, name => "accl")
#pragma core_register(59, name => "acch")

#pragma intrinsic(_abss, name => "abss")

#pragma aux_register(ACC0_LO, name => "acc0_lo")
#pragma aux_register(ACC0_GLO, name => "acc0_glo")
#pragma aux_register(ACC0_HI, name => "acc0_hi")
#pragma aux_register(ACC0_GHI, name => "acc0_ghi")

#pragma aux_register(DSP_CTRL, name => "dsp_ctrl")

#endif // __Xdsp
*/

#if defined(__Xdsp) || defined(__Xfpus_div)
#include <arc/arc_intrinsics.h>
#endif

#ifdef __Xdsp_complex // ARCv2EM DSP Complex Extensions
/*
#pragma aux_register(DSP_BFLY, name => "dsp_bfly")
#pragma aux_register(DSP_FFT_CTRL, name => "dsp_fft_ctrl")
*/
static _Inline void _set_DSP_CTRL(uint32_t val) { _sr(val, DSP_CTRL); }
static _Inline void _set_DSP_FFT_CTRL(uint32_t val) { _sr(val, DSP_FFT_CTRL); }
/*
#pragma intrinsic(_cbflyhf0r, name => "cbflyhf0r", effects => "dsp_bfly:is_read;dsp_fft_ctrl:is_read;dsp_ctrl:is_written;acc0_lo:is_written;acc0_glo:is_written;acc0_hi:is_written;acc0_ghi:is_written;accl:is_written;acch:is_written");
#pragma intrinsic(_cbflyhf1r, name => "cbflyhf1r", effects => "dsp_bfly:is_read:is_written;dsp_fft_ctrl:is_read;dsp_ctrl:is_written;acc0_lo:is_read:is_written;acc0_glo:is_read:is_written;acc0_hi:is_read:is_written;acc0_ghi:is_read:is_written");
*/
#endif // __Xdsp_complex

#endif // !(defined(_ARCVER_ARCv2EM) && defined(_Xdsp))

/*
    Stack
*/
#pragma Push_small_data(0)
extern uint32_t _fstack;
extern uint32_t _estack;
#pragma Pop_small_data()

static _Inline uint32_t _get_SP() 
{
    return( _core_read(28) );
}

static _Inline void * _get_stack_start()
{
    return ((void *) &_fstack);
}

static _Inline void * _get_stack_end()
{
    return ((void *) &_estack);
}

#ifdef __Xtimer0
static _Inline void _START_TIMER_0()
{
    _sr (0 , 0x22); // Disable interrupts
    _sr(0xffffffff, 0x23); // Write the limit value
    _sr(3, 0x22); // Set up the control flags
    _sr(0, 0x21); // Write the count value
}

static _Inline uint32_t _READ_TIMER_0()
{
    return (_lr(0x21)); // Read the count value
}

static _Inline void _STOP_TIMER_0()
{
    _sr (0 , 0x22); // Disable interrupts
}
#endif

#ifdef __Xtimer1
static _Inline void _START_TIMER_1()
{
    _sr (0 , 0x101);
    _sr(0xffffffff, 0x102);
    _sr(3, 0x101);
    _sr(0, 0x100);
}

static _Inline uint32_t _READ_TIMER_1()
{
    return( _lr(0x100) );
}

static _Inline void _STOP_TIMER_1()
{
    _sr (0 , 0x101);
}
#endif



#endif // _ARC

#endif // __ARC_PLATFORM_NATIVE_H
