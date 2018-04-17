/*
//
//CONFIDENTIAL AND PROPRIETARY INFORMATION
//
//Copyright (c) 2015 Synopsys, Inc. All rights reserved.
//This software and documentation contain confidential and
//proprietary information that is the property of
//Synopsys, Inc. The software and documentation are
//furnished under a license agreement and may be used
//or copied only in accordance with the terms of the license
//agreement. No part of the software and documentation
//may be reproduced, transmitted, or translated, in any
//form or by any means, electronic, mechanical, manual,
//optical, or otherwise, without prior written permission
//of Synopsys, Inc., or as expressly provided by the license agreement.
//Reverse engineering is prohibited, and reproduction,
//disclosure or use without specific written authorization
//of Synopsys Inc. is strictly forbidden.
//
//
*/

#ifndef DSPLIB_PRIMITIVES_GENERAL_H
#define DSPLIB_PRIMITIVES_GENERAL_H

#include <math.h>
#include "dsplib_types.h"

// Saturate Primitives

static _Inline int64_t _saturate_62(int64_t acc)
{
    return (((acc) > MAX_VAL(62)) ? MAX_VAL(62) : (((acc) < MIN_VAL(62)) ? MIN_VAL(62) : (acc)));
}
static _Inline int32_t _saturate_30(int32_t acc)
{
    return (((acc) > MAX_VAL(30)) ? MAX_VAL(30) : (((acc) < MIN_VAL(30)) ? MIN_VAL(30) : (acc)));
}
static _Inline int16_t _saturate_14(int16_t acc)
{
    return (((acc) > MAX_VAL(14)) ? MAX_VAL(14) : (((acc) < MIN_VAL(14)) ? MIN_VAL(14) : (acc)));
}

static _Inline int64_t _saturate_63(int64_t acc)
{
    return (((acc) > MAX_VAL_Q63) ? MAX_VAL_Q63 : (((acc) < MIN_VAL_Q63) ? MIN_VAL_Q63 : (acc)));
}
static _Inline int32_t _saturate_31(int32_t acc)
{
    return (((acc) > MAX_VAL(31)) ? MAX_VAL(31) : (((acc) < MIN_VAL(31)) ? MIN_VAL(31) : (acc)));
}
static _Inline int16_t _saturate_15(int32_t acc)
{
    return (int16_t)(((acc) > MAX_VAL(15)) ? MAX_VAL(15) : (((acc) < MIN_VAL(15)) ? MIN_VAL(15) : (acc)));
}


#define SATURATE_62(acc) _saturate_62(acc)
#define SATURATE_30(acc) _saturate_30(acc)
#define SATURATE_14(acc) _saturate_14(acc)

#define SATURATE_63(acc) _saturate_63((int64_t)acc)
#define SATURATE_31(acc) _saturate_31((int32_t)acc)
#define SATURATE_15(acc) _saturate_15((int32_t)acc)

#define SATURATE(acc, bits) ((bits < 64) ? (((acc) > MAX_VAL(bits)) ? MAX_VAL(bits) : (((acc) < MIN_VAL(bits)) ? MIN_VAL(bits) : (acc))) : (acc))


static _Inline f32_t _ABS_F32(f32_t a) { return fabs(a); };

//static _Inline f32_t _ABS_F32(f32_t a) { return (a < 0 ? -a : a); };
static _Inline int32_t _ABS_Q31(int32_t a) { return (a < 0 ? -a : a); };
static _Inline int16_t _ABS_Q15(int16_t a) { return (a < 0 ? -a : a); };
static _Inline int8_t  _ABS_Q7(int8_t a)   { return (a < 0 ? -a : a); };

static _Inline int32_t _ASL_Q31(int32_t a, int32_t sh) {
    if(sh > 0) { return (a << sh); }
    sh = -sh;
    return (a >> sh);
}

static _Inline int16_t _ASL_Q15(int16_t a, int32_t sh) {
    if(sh > 0) { return (a << sh); }
    sh = -sh;
    return  (a >> sh);
}

static _Inline int8_t _ASL_Q7(int8_t a, int32_t sh) {
    if(sh > 0) { return (a << sh); }
    sh = -sh;
    return  (a >> sh);
}

_Inline static uint32_t dsp_norm_q31(q31_t op)
{
    int i;
    int32_t a = ~op>>31;

    for ( i = 0 ; i < 32 ; i++ ) {
        if ( ! (( op>>(31-i) ^ a ) & 1) )
            return (i-1) ;
    }
    return (i-1);
} 

_Inline static uint16_t dsp_norm_q15(q15_t op)
{
    int i;
    int16_t a = ~op>>15;

    for ( i = 0 ; i < 16 ; i++ ) {
        if ( ! (( op>>(15-i) ^ a ) & 1) )
            return (i-1) ;
    }
    return (i-1);
} 


_Inline static void _MEM_FILL_Q31(PTR(q31_t) Dst,q31_t val, uint32_t size) {
    if (size)
    {
        uint32_t i;
        for (i=0;i<size;i++)
            Dst[i]=val;
    }
}

_Inline static void _MEM_FILL_Q15(PTR(q15_t) Dst,q15_t val, uint32_t size) {
    if (size)
    {
        uint32_t i;
        for (i=0;i<size;i++)
            Dst[i]=val;
    }
}

_Inline static void _MEM_FILL_Q7(PTR(q7_t) Dst,q7_t val, uint32_t size) {
    if (size)
    {
        uint32_t i;
        for (i=0;i<size;i++)
            Dst[i]=val;
    }
}

_Inline static void _MEM_FILL_F32(PTR(f32_t) Dst,f32_t val, uint32_t size) {
    if (size)
    {
        uint32_t i;
        for (i=0;i<size;i++)
            Dst[i]=val;
    }
}


// Absolute value primitives

#define dspfx_abs_f32(a) _ABS_F32(a)
#define dspfx_abs_q31(a) _ABS_Q31(a)
#define dspfx_abs_q15(a) _ABS_Q15(a)  
#define dspfx_abs_q7(a)  _ABS_Q7(a)

// Negation primitives

#define NEG_F32(a) (-(a))
#define dspfx_neg_q31(a) (-(a))
#define dspfx_neg_q15(a) (-(a))
#define dspfx_neg_q7(a)  (-(a))

// Addition primitives


#define ADD_F32(a, b) ((a) + (b))
#define dspfx_add_q31(a, b) ((a) + (b))
#define dspfx_add_q15(a, b) ((a) + (b))
#define dspfx_add_q7(a, b)  ((a) + (b))
#define dspfx_add_q63(a, b) ((a) + (b))

// Substraction primitives

#define SUB_F32(a, b) ((a) - (b))
#define dspfx_sub_q31(a, b) ((a) - (b))
#define dspfx_sub_q15(a, b) ((a) - (b))
#define dspfx_sub_q7(a, b)  ((a) - (b))
#define dspfx_sub_q63(a, b) ((a) - (b))

// Shift left primitives (bi-directional)

#define dspfx_asl_q31(a, b)  _ASL_Q31(a, b) 
#define dspfx_asl_q15(a, b)  _ASL_Q15(a, b) 
#define dspfx_asl_q7(a, b)   _ASL_Q7(a, b)  

// Saturate Primitive

#define dspfx_min_q31(a, b) (((a)<=(b)?(a):(b)))
#define dspfx_max_q31(a, b) (((a)>=(b)?(a):(b)))

#define dspfx_min_q15(a, b) (((a)<=(b)?(a):(b)))
#define dspfx_max_q15(a, b) (((a)>=(b)?(a):(b)))

// Multiply primitives

#define MAC_F32(acc, a, b) (acc += (a)*(b))
#define MSU_F32(acc, a, b) (acc -= (a)*(b))

#define MPY_F32(a, b) ((a) * (b))
#define dspfx_mpy_q31(a, b)       (q31_t)(((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b))) >> 31)
#define dspfx_mpy_q15(a, b)       (q15_t)(((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b))) >> 15)
#define dspfx_mpy_q7(a, b)        (q7_t) (((int16_t)((int8_t)(a)) * (int16_t)((int8_t)(b))) >> 7)

#define dspfx_q63_mpy_q31(a, b)       (q63_t)(((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b))) << 1)
#define dspfx_q31_mpy_q15(a, b)       (q31_t)(((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b))) << 1)

#define dspfx_mpy_rnd_q31(a, b) (q31_t)(((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)) + (1<<30)) >> 31)
#define dspfx_mpyh_rnd_q31(a, b) (q31_t)(((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)) + (1<<30)) >> 31)
#define dspfx_mpy_rnd_q15(a, b) (q15_t)(((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)) + (1<<14)) >> 15)

#define dspfx_q63_mpy_asr_q31(a, b, shift) (((q63_t)(a)*(b)) >> shift)

#define dspfx_a64mul_q31(a,b, acc)   (q31_t)((acc = ((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))) >> 31)
#define dspfx_a32mul_q15(a,b, acc)   (q15_t)((acc = ((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))) >> 15)

#define dspfx_a64mul_nf_q31(a,b, acc)   (q31_t)((acc = ((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))) >> 32)
#define dspfx_a32mul_nf_q15(a,b, acc)   (q15_t)((acc = ((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))) >> 16)

#define dspfx_a64mac_q31(a,b, acc)   (q31_t)((acc = dspfx_add_q63((((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))), acc)) >> 31)
#define dspfx_a64msu_q31(a,b, acc)   (q31_t)((acc = dspfx_sub_q63(acc, (((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))))) >> 31)

#define dspfx_a32mac_q15(a,b, acc)   (q15_t)((acc = dspfx_add_q31((((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))), acc)) >> 15)
#define dspfx_a32msu_q15(a,b, acc)   (q15_t)((acc = dspfx_sub_q31(acc, (((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))))) >> 15)
#define dspfx_a16mac_q7(a,b, acc)    (q7_t) ((acc = dspfx_add_q15((((int16_t)((int8_t)(a))  * (int16_t)((int8_t) (b)))), acc)) >> 7)

#define dspfx_a64mac_nf_q31(a,b, acc)   (q31_t)((acc = dspfx_add_q63((((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))), acc)) >> 32)
#define dspfx_a32mac_nf_q15(a,b, acc)   (q15_t)((acc = dspfx_add_q31((((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))), acc)) >> 16)
#define dspfx_a16mac_nf_q7(a,b, acc)    (q7_t) ((acc = dspfx_add_q15((((int16_t)((int8_t)(a))  * (int16_t)((int8_t) (b)))), acc)) >> 8)

#define dspfx_q31_cast_a64(acc)   (q31_t)((acc) >> 31)
#define dspfx_q15_cast_a32(acc)   (q15_t)((acc) >> 15)
#define dspfx_q15_cast_nf_a32(acc)   (q15_t)((acc) >> 16)
#define dspfx_q7_cast_a16(acc)    (q7_t) ((acc) >> 7)

#define dspfx_q31_cast_q63(a) (q31_t)(a >> 32)
#define dspfx_q15_cast_q31(a) (q15_t)(a >> 16)

#define dspfx_8q24_mac_q15(acc, a, b) (acc += (((q31_t)(a)*(b))>>6))
#define dspfx_8q24_msu_q15(acc, a, b) (acc -= (((q31_t)(a)*(b))>>6))

#define dspfx_i32_cast_8q24(acc) acc

#define dspfx_q31_cast_asl_a64(acc, shift)   (q31_t)((acc) >> (31-shift))
#define dspfx_q15_cast_asl_a32(acc, shift)   (q15_t)((acc) >> (15-shift))

#define dspfx_mpy_i32(a, b)	    (int32_t)((a) * (b))

#define dspfx_norm_q31 dsp_norm_q31
#define dspfx_norm_q15 dsp_norm_q15

#define dspfx_accum64()    q63_t acc;
#define dspfx_reset_a64()     acc = 0

#define dspfx_accum32()  	q31_t acc;
#define dspfx_reset_a32()  	acc = 0

#define dspfx_get_a32(acc)  (acc)

#define dspfx_accum16()  	q15_t acc;
#define dspfx_reset_a16()  	acc = 0

#if !defined(_ARC_BE)
#define COMPLEX_CONST_UI32(re, im) ((uint32_t)re << 16 | (uint32_t) im)
#else
#define COMPLEX_CONST_UI32(re, im) ((uint32_t)im << 16 | (uint32_t) re)
#endif

#define INIT_MACMODE()
#define INIT_MACMODE_LSP()
#define INIT_MACMODE_NF()

_Inline static float _as_float(uint32_t a) 
{
	union {
		unsigned int ui;
		float d;
	} my_union;
	my_union.ui = a;
	return my_union.d;
}

_Inline static uint32_t _as_uint32(float a) 
{
	union {
		unsigned int ui;
		float d;
	} my_union;
	my_union.d = a;
	return my_union.ui;
}

#ifdef DSPLIB_CC_XYFAST
#define _DSPLIB_store_regs _agu_store_regs_fast
#define _DSPLIB_restore_regs _agu_restore_regs_fast
#else
#define _DSPLIB_store_regs _agu_store_regs
#define _DSPLIB_restore_regs _agu_restore_regs
#endif

#if defined(__CLANG__)

#if defined _ARCv2EM_XY
#define PRAGMA_UNROLL_2_FOR_XY _Pragma("unroll 2")
#else
#define PRAGMA_UNROLL_2_FOR_XY
#endif

#define PRAGMA_UNROLL_2  _Pragma("unroll 2")
#define PRAGMA_NO_UNROLL  _Pragma("clang loop unroll(disable)")
#else
#define PRAGMA_UNROLL_2_FOR_XY
#define PRAGMA_UNROLL_2
#define PRAGMA_NO_UNROLL
#define __builtin_assume(x) {;}
#endif

#if defined(DSPLIB_FORCE_HIGH_LEVEL) && defined (_XY_HAND_OPT)
#undef _XY_HAND_OPT
#endif

#if defined(DSPLIB_FORCE_LOW_LEVEL) && !defined(_XY_HAND_OPT)
#define _XY_HAND_OPT
#endif

#endif
