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
 
#ifndef __ARC_PLATFORM_EMU_H
#define __ARC_PLATFORM_EMU_H

#if defined(EMU__Xbarrel_shifter) && defined(__Xbarrel_shifter)
#undef EMU__Xbarrel_shifter
#endif
#if defined(EMU__Xnorm) && defined(__Xnorm)
#undef EMU__Xnorm
#endif
#if defined(EMU__Xswap) && defined(__Xswap)
#undef EMU__Xswap
#endif
#if defined(EMU__Xmpy16) && defined(__Xmpy16)
#undef EMU__Xmpy16
#endif
#if defined(EMU__Xea) && defined(__Xea)
#undef EMU__Xea
#endif
#if defined(EMU__Xmul32x16) && defined(__Xmul32x16)
#undef EMU__Xmul32x16
#endif
#if defined(EMU__Xxmac_d16) && defined(__Xxmac_d16)
#undef EMU__Xxmac_d16
#endif
#if defined(EMU__Xxmac_24) && defined(__Xxmac_24)
#undef EMU__Xxmac_24
#endif
#if defined(EMU__Xdmulpf) && defined(__Xdmulpf)
#undef EMU__Xdmulpf
#endif
#if defined(EMU__Xcrc) && defined(__Xcrc)
#undef EMU__Xcrc
#endif
#if defined(EMU__Xxy) && defined(__Xxy)
#undef EMU__Xxy
#endif
#if defined(EMU__Xdsp) && defined(__Xdsp)
#undef EMU__Xdsp
#endif
#if defined(EMU__Xdsp_complex) && defined(__Xdsp_complex)
#undef EMU__Xdsp_complex
#endif


#ifndef _ARC

#define EMU__Xbarrel_shifter
#define EMU__Xnorm
#define EMU__Xswap
#define EMU__Xmpy16
#define EMU__Xea
#define EMU__Xmul32x16
#define EMU__Xxmac_d16
#define EMU__Xxmac_24
#define EMU__Xdmulpf
#define EMU__Xcrc
#define EMU__Xxy
#define EMU__Xdsp // ARCv2EM Xdsp extension
#define EMU__Xdsp_complex // ARCv2EM Xdsp_complex extension
#endif // !_ARC


#ifndef _ARC

/* dummy defines for PC compilation */
#define _Uncached
#define _Rarely
#define _Usually

#define  _Inline __inline
#pragma warning(disable: 4068)

static int32_t saturate32(int64_t L_var1);

extern int32_t _Z;
extern int32_t _N;
extern int32_t _C;
extern int32_t _V;

#define __condition_z        (_Z)    // Zero
#define __condition_nz       (!_Z)   // Non-Zero
#define __condition_p        (!_N)   // Positive
#define __condition_n        (_N)    // Negative
#define __condition_c        (_C)    // Carry set, lower than (unsigned)
#define __condition_cc       (!_C)   // Carry clear, higher or same (unsigned)
#define __condition_v        (_V)    // Over-flow set
#define __condition_nv       (!_V)   // Over-flow clear
#define __condition_gt       ((_N && _V && !_Z) || (!_N && !_V && !_Z))  // Greater than (signed)
#define __condition_ge       ((_N && _V) || (!_N && !_V))                // Greater than or equal to (signed)
#define __condition_lt       ((_N && !_V) || (!_N && _V))                // Less than (signed)
#define __condition_le       (_Z || (_N && !_V) || (!_N && _V))          // Less than or equal to (signed)
#define __condition_hi       (!_C && !_Z)                                // Higher than (unsigned)
#define __condition_ls       (_C || _Z)                                  // Lower than or same (unsigned)
#define __condition_pnz      (!_N && !_Z)                                // Positive non-zero

#endif // !_ARC


#ifdef EMU__Xxy
/*
    XY memory addressing
*/

///////////////////////////////////////////////////////////////////////////////
// XY-memory simulation
//
// Use the following global variables in debugger to see xy-memory parameters:
//
//-----------------------------------------------------------------------------
// XMEM, YMEM
// XY-Memory data buffers for x bank and y bank respectively
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// AX0..AX3, AY0..AY3
// Address (offset) registers
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// MX00,MX01..MX30,MX31, MY00,MY01..MY30,MY31
// Modifiers for the respective address registers
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// MODX00,MODX01..MODX30,MODX31, MODY00,MODY01..MODY30,MODY31
// Modulo extracted from the respective modifier
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// INCX00,INCX01..INCX30,INCX31, INCY00,INCY01..INCY30,INCY31
// Increment extracted from the respective modifier
//-----------------------------------------------------------------------------
//
///////////////////////////////////////////////////////////////////////////////
#define XYMEM_XY_NUM            (2)

#define DSPRAM_VERSION  (0x3) // Current version (0x03 = Supports 24-bit data packing and unpacking)
#define DSPRAM_MEMSIZE  (0x4) // Memory size of X and Y per single bank (0100b 8 KB)
#define DSPRAM_NBANKS   (0x1) // Memory size of X and Y (0001b 1 bank)
#define DSPRAM_RA       (0x2) // RAM Architecture Type (0010b Single port RAM architecture)
#define DSPRAM_EP       (0x1) // Extended XY Memory Packing and Unpacking
#define DSPRAM_MOD      (0x1) // XY Memory Modulo Addressing Mode

#define XYMEM_MAX_OFFSET8       (512*(1<<DSPRAM_MEMSIZE))
#define XYMEM_MAX_OFFSET16      (XYMEM_MAX_OFFSET8/2)
#define XYMEM_MAX_OFFSET32      (XYMEM_MAX_OFFSET16/2)

#define XYMEM_XY_SIZE           (XYMEM_MAX_OFFSET32)



typedef int32_t XYBank[XYMEM_XY_SIZE];

extern XYBank* XMEM;
extern XYBank* YMEM;

extern uint32_t _AX0;
extern uint32_t _AX1;
extern uint32_t _AX2;
extern uint32_t _AX3;

extern uint32_t _AY0;
extern uint32_t _AY1;
extern uint32_t _AY2;
extern uint32_t _AY3;

extern uint32_t AX0_PREV;
extern uint32_t AX1_PREV;
extern uint32_t AX2_PREV;
extern uint32_t AX3_PREV;

extern uint32_t AY0_PREV;
extern uint32_t AY1_PREV;
extern uint32_t AY2_PREV;
extern uint32_t AY3_PREV;

extern uint32_t _MX00;
extern uint32_t _MX01;
extern uint32_t _MX10;
extern uint32_t _MX11;
extern uint32_t _MX20;
extern uint32_t _MX21;
extern uint32_t _MX30;
extern uint32_t _MX31;

extern uint32_t _MY00;
extern uint32_t _MY01;
extern uint32_t _MY10;
extern uint32_t _MY11;
extern uint32_t _MY20;
extern uint32_t _MY21;
extern uint32_t _MY30;
extern uint32_t _MY31;

extern uint32_t MODX00;
extern uint32_t MODX01;
extern uint32_t MODX10;
extern uint32_t MODX11;
extern uint32_t MODX20;
extern uint32_t MODX21;
extern uint32_t MODX30;
extern uint32_t MODX31;

extern uint32_t MODY00;
extern uint32_t MODY01;
extern uint32_t MODY10;
extern uint32_t MODY11;
extern uint32_t MODY20;
extern uint32_t MODY21;
extern uint32_t MODY30;
extern uint32_t MODY31;

extern int32_t INCX00;
extern int32_t INCX01;
extern int32_t INCX10;
extern int32_t INCX11;
extern int32_t INCX20;
extern int32_t INCX21;
extern int32_t INCX30;
extern int32_t INCX31;

extern int32_t INCY00;
extern int32_t INCY01;
extern int32_t INCY10;
extern int32_t INCY11;
extern int32_t INCY20;
extern int32_t INCY21;
extern int32_t INCY30;
extern int32_t INCY31;

extern int32_t last_func_pf_flag;
extern int32_t pf_implicit_dst2;

extern XYBank m_arXYMemory[XYMEM_XY_NUM];

extern uint32_t BASEX00;
extern uint32_t BASEX01;
extern uint32_t BASEX10;
extern uint32_t BASEX11;
extern uint32_t BASEX20;
extern uint32_t BASEX21;
extern uint32_t BASEX30;
extern uint32_t BASEX31;
 
extern uint32_t BASEY00;
extern uint32_t BASEY01;
extern uint32_t BASEY10;
extern uint32_t BASEY11;
extern uint32_t BASEY20;
extern uint32_t BASEY21;
extern uint32_t BASEY30;
extern uint32_t BASEY31;

void set_xy(
    uint32_t ModifierRG, 
    uint32_t *AddressRG, 
    int32_t *XYBankBase,
    int32_t v);

void update_a(
    uint32_t ModifierRG, 
    uint32_t *AddressRG,
    int32_t base_address
    );

#define __set_x0_u0(a) \
{   \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a); \
    if (last_func_pf_flag) {\
    AY0_PREV = _AY0;\
    set_xy(_MY00,&_AY0, m_arXYMemory[1], pf_implicit_dst2);\
    update_a(_MY00, &_AY0, BASEY00);\
    } \
    AX0_PREV = _AX0;\
    set_xy(_MX00,&_AX0, m_arXYMemory[0], r_tmp);\
    update_a(_MX00, &_AX0, BASEX00);\
}

#define __set_x0_u1(a) \
{   \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AY0_PREV = _AY0;\
    set_xy(_MY01,&_AY0, m_arXYMemory[1], pf_implicit_dst2);\
    update_a(_MY01, &_AY0, BASEY01);\
    } \
    AX0_PREV = _AX0;\
    set_xy(_MX01,&_AX0, m_arXYMemory[0], r_tmp);\
    update_a(_MX01, &_AX0, BASEX01);\
} 

#define __set_x0_nu(a) \
{   \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AY0_PREV = _AY0;\
    set_xy(_MY00,&_AY0, m_arXYMemory[1], pf_implicit_dst2);\
    } \
    AX0_PREV = _AX0;\
    set_xy(_MX00,&_AX0, m_arXYMemory[0], r_tmp);\
}

#define __set_x1_u0(a) \
{   \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AY1_PREV = _AY1;\
    set_xy(_MY10,&_AY1, m_arXYMemory[1], pf_implicit_dst2);\
    update_a(_MY10, &_AY1, BASEY10);\
    } \
    AX1_PREV = _AX1;\
    set_xy(_MX10,&_AX1, m_arXYMemory[0], r_tmp);\
    update_a(_MX10, &_AX1, BASEX10);\
}

#define __set_x1_u1(a) \
{   \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AY1_PREV = _AY1;\
    set_xy(_MY11,&_AY1, m_arXYMemory[1], pf_implicit_dst2);\
    update_a(_MY11, &_AY1, BASEY11);\
    } \
    AX1_PREV = _AX1;\
    set_xy(_MX11,&_AX1, m_arXYMemory[0], r_tmp);\
    update_a(_MX11, &_AX1, BASEX11);\
} 

#define __set_x1_nu(a) \
{   \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AY1_PREV = _AY1;\
    set_xy(_MY10,&_AY1, m_arXYMemory[1], pf_implicit_dst2);\
    } \
    AX1_PREV = _AX1;\
    set_xy(_MX10,&_AX1, m_arXYMemory[0], r_tmp);\
}

#define __set_x2_u0(a) \
{   \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AY2_PREV = _AY2;\
    set_xy(_MY20,&_AY2, m_arXYMemory[1], pf_implicit_dst2);\
    update_a(_MY20, &_AY2, BASEY20);\
    } \
    AX2_PREV = _AX2;\
    set_xy(_MX20,&_AX2, m_arXYMemory[0], r_tmp);\
    update_a(_MX20, &_AX2, BASEX20);\
}

#define __set_x2_u1(a) \
{   \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AY2_PREV = _AY2;\
    set_xy(_MY21,&_AY2, m_arXYMemory[1], pf_implicit_dst2);\
    update_a(_MY21, &_AY2, BASEY21);\
    } \
    AX2_PREV = _AX2;\
    set_xy(_MX21,&_AX2, m_arXYMemory[0], r_tmp);\
    update_a(_MX21, &_AX2, BASEX21);\
} 

#define __set_x2_nu(a) \
{   \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AY2_PREV = _AY2;\
    set_xy(_MY20,&_AY2, m_arXYMemory[1], pf_implicit_dst2);\
    } \
    AX2_PREV = _AX2;\
    set_xy(_MX20,&_AX2, m_arXYMemory[0], r_tmp);\
}

#define __set_x3_u0(a) \
{   \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AY3_PREV = _AY3;\
    set_xy(_MY30,&_AY3, m_arXYMemory[1], pf_implicit_dst2);\
    update_a(_MY30, &_AY3, BASEY30);\
    } \
    AX3_PREV = _AX3;\
    set_xy(_MX30,&_AX3, m_arXYMemory[0], r_tmp);\
    update_a(_MX30, &_AX3, BASEX30);\
}

#define __set_x3_u1(a) \
{   \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AY3_PREV = _AY3;\
    set_xy(_MY31,&_AY3, m_arXYMemory[1], pf_implicit_dst2);\
    update_a(_MY31, &_AY3, BASEY31);\
    } \
    AX3_PREV = _AX3;\
    set_xy(_MX31,&_AX3, m_arXYMemory[0], r_tmp);\
    update_a(_MX31, &_AX3, BASEX31);\
} 

#define __set_x3_nu(a) \
{   \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AY3_PREV = _AY3;\
    set_xy(_MY30,&_AY3, m_arXYMemory[1], pf_implicit_dst2);\
    } \
    AX3_PREV = _AX3;\
    set_xy(_MX30,&_AX3, m_arXYMemory[0], r_tmp);\
}
//
#define __set_y0_u0(a) \
{    \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AX0_PREV = _AX0;\
    set_xy(_MX00,&_AX0, m_arXYMemory[0], pf_implicit_dst2);\
    update_a(_MX00, &_AX0, BASEX00);\
    } \
    AY0_PREV = _AY0;\
    set_xy(_MY00,&_AY0, m_arXYMemory[1], r_tmp);\
    update_a(_MY00, &_AY0, BASEY00);\
} 

#define __set_y0_u1(a) \
{    \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AX0_PREV = _AX0;\
    set_xy(_MX01,&_AX0, m_arXYMemory[0], pf_implicit_dst2);\
    update_a(_MX01, &_AX0, BASEX01);\
    } \
    AY0_PREV = _AY0;\
    set_xy(_MY01,&_AY0, m_arXYMemory[1], r_tmp);\
    update_a(_MY01, &_AY0, BASEY01);\
}
    
#define __set_y0_nu(a) \
{    \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    set_xy(_MX00,&_AX0, m_arXYMemory[0], pf_implicit_dst2);\
    } \
    set_xy(_MY00,&_AY0, m_arXYMemory[1], r_tmp);\
}

#define __set_y1_u0(a) \
{    \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AX1_PREV = _AX1;\
    set_xy(_MX10,&_AX1, m_arXYMemory[0], pf_implicit_dst2);\
    update_a(_MX10, &_AX1, BASEX10);\
    } \
    AY1_PREV = _AY1;\
    set_xy(_MY10,&_AY1, m_arXYMemory[1], r_tmp);\
    update_a(_MY10, &_AY1, BASEY10);\
} 

#define __set_y1_u1(a) \
{    \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AX1_PREV = _AX1;\
    set_xy(_MX11,&_AX1, m_arXYMemory[0], pf_implicit_dst2);\
    update_a(_MX11, &_AX1, BASEX11);\
    } \
    AY1_PREV = _AY1;\
    set_xy(_MY11,&_AY1, m_arXYMemory[1], r_tmp);\
    update_a(_MY11, &_AY1, BASEY11);\
}

#define __set_y1_nu(a) \
{    \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    set_xy(_MX10,&_AX1, m_arXYMemory[0], pf_implicit_dst2);\
    } \
    set_xy(_MY10,&_AY1, m_arXYMemory[1], r_tmp);\
}

#define __set_y2_u0(a) \
{    \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AX2_PREV = _AX2;\
    set_xy(_MX20,&_AX2, m_arXYMemory[0], pf_implicit_dst2);\
    update_a(_MX20, &_AX2, BASEX20);\
    } \
    AY2_PREV = _AY2;\
    set_xy(_MY20,&_AY2, m_arXYMemory[1], r_tmp);\
    update_a(_MY20, &_AY2, BASEY20);\
} 

#define __set_y2_u1(a) \
{    \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AX2_PREV = _AX2;\
    set_xy(_MX21,&_AX2, m_arXYMemory[0], pf_implicit_dst2);\
    update_a(_MX21, &_AX2, BASEX21);\
    } \
    AY2_PREV = _AY2;\
    set_xy(_MY21,&_AY2, m_arXYMemory[1], r_tmp);\
    update_a(_MY21, &_AY2, BASEY21);\
}

#define __set_y2_nu(a) \
{    \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    set_xy(_MX20,&_AX2, m_arXYMemory[0], pf_implicit_dst2);\
    } \
    set_xy(_MY20,&_AY2, m_arXYMemory[1], r_tmp);\
}

#define __set_y3_u0(a) \
{    \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AX3_PREV = _AX3;\
    set_xy(_MX30,&_AX3, m_arXYMemory[0], pf_implicit_dst2);\
    update_a(_MX30, &_AX3, BASEX30);\
    } \
    AY3_PREV = _AY3;\
    set_xy(_MY30,&_AY3, m_arXYMemory[1], r_tmp);\
    update_a(_MY30, &_AY3, BASEY30);\
} 

#define __set_y3_u1(a) \
{    \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    AX3_PREV = _AX3;\
    set_xy(_MX31,&_AX3, m_arXYMemory[0], pf_implicit_dst2);\
    update_a(_MX31, &_AX3, BASEX31);\
    } \
    AY3_PREV = _AY3;\
    set_xy(_MY31,&_AY3, m_arXYMemory[1], r_tmp);\
    update_a(_MY31, &_AY3, BASEY31);\
}

#define __set_y3_nu(a) \
{    \
    int32_t r_tmp;\
    last_func_pf_flag = 0; \
    r_tmp = (a);\
    if (last_func_pf_flag) {\
    set_xy(_MX30,&_AX3, m_arXYMemory[0], pf_implicit_dst2);\
    } \
    set_xy(_MY30,&_AY3, m_arXYMemory[1], r_tmp);\
}

#endif // EMU__Xxy

#endif // __ARC_PLATFORM_EMU_H
