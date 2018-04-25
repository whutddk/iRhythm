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

#ifndef DSPLIB_PRIMITIVES_GENERAL_SAT_H
#define DSPLIB_PRIMITIVES_GENERAL_SAT_H

#undef dspfx_abs_q31
#undef dspfx_abs_q15
#undef dspfx_abs_q7
#undef dspfx_neg_q31
#undef dspfx_neg_q15
#undef dspfx_neg_q7
#undef dspfx_add_q63
#undef dspfx_add_q31
#undef dspfx_add_q15
#undef dspfx_add_q7
#undef dspfx_sub_q63
#undef dspfx_sub_q31
#undef dspfx_sub_q15
#undef dspfx_sub_q7
#undef dspfx_asl_q31
#undef dspfx_asl_q15
#undef dspfx_asl_q7

#undef dspfx_mpy_q31
#undef dspfx_mpy_q15
#undef dspfx_q63_mpy_q31
#undef dspfx_q31_mpy_q15
#undef dspfx_mpy_q7
#undef dspfx_mpy_rnd_q31
#undef dspfx_mpyh_rnd_q31
#undef dspfx_mpy_rnd_q15
#undef dspfx_a64mul_q31
#undef dspfx_a32mul_q15
#undef dspfx_a64mac_q31
#undef dspfx_a64msu_q31
#undef dspfx_a32mac_q15
#undef dspfx_a32msu_q15
#undef dspfx_a16mac_q7
#undef dspfx_a64mul_nf_q31
#undef dspfx_a32mul_nf_q15
#undef dspfx_a64mac_nf_q31
#undef dspfx_a32mac_nf_q15
#undef dspfx_a16mac_nf_q7
#undef dspfx_q31_cast_a64
#undef dspfx_q15_cast_a32
#undef dspfx_q15_cast_nf_a32
#undef dspfx_q7_cast_a16
#undef dspfx_q31_cast_asl_a64
#undef dspfx_q15_cast_asl_a32

// use this define to emulate multiplier with zero guard bits (otherwise scheme with single guard bit is used)
//#define ZERO_GUARDBITS

#if defined(TRAP_OVF)
#define TRAP() primitives_trap();
_Inline void primitives_trap()
{
    int brk = 1;
}
#else
#define TRAP()
#endif


static _Inline int32_t _ABSS_Q31(int32_t a) { return (a == MIN_VAL_Q31 ? MAX_VAL_Q31 : (a < 0 ? -a : a)); };
static _Inline int16_t _ABSS_Q15(int16_t a) { return (a == MIN_VAL_Q15 ? MAX_VAL_Q15 : (a < 0 ? -a : a)); };
static _Inline int8_t  _ABSS_Q7(int8_t a)   { return (a == MIN_VAL_Q7 ? MAX_VAL_Q7 : (a < 0 ? -a : a)); };

static _Inline int32_t _NEGS_Q31(int32_t a) { return ((a) == MIN_VAL_Q31 ? MAX_VAL_Q31 : (-(a))); };
static _Inline int16_t _NEGS_Q15(int16_t a) { return ((a) == MIN_VAL_Q15 ? MAX_VAL_Q15 : (-(a))); };
static _Inline int8_t  _NEGS_Q7(int8_t a)   { return ((a) == MIN_VAL_Q7  ? MAX_VAL_Q7  : (-(a))); };



static _Inline int64_t _ADDS_Q63(int64_t a, int64_t b) {
    int64_t c = a + b;
    if ((( a ^ c ) & ( c ^ b )) >= 0) return c;
    TRAP()
    return ((c >= 0) ? MIN_VAL_Q63 : MAX_VAL_Q63);
}

static _Inline int64_t _SUBS_Q63(int64_t a, int64_t b) {
    int64_t c = a - b;
    if ((( a ^ c ) & ( c ^ (~b) )) >= 0) return c;
    TRAP()
    return ((c >= 0) ? MIN_VAL_Q63 : MAX_VAL_Q63);
}

static _Inline int32_t _ADDS_Q31(int32_t a, int32_t b) {
    int32_t c = a + b;
    if ((( a ^ c ) & ( c ^ b )) >= 0) return c;
    TRAP()
    return ((c >= 0) ? MIN_VAL_Q31 : MAX_VAL_Q31);
}

static _Inline int16_t _ADDS_Q15(int16_t a, int16_t b) {
    int16_t c = a + b;
    if ((( a ^ c ) & ( c ^ b )) >= 0) return c;
    TRAP()
    return ((c >= 0) ? MIN_VAL_Q15 : MAX_VAL_Q15);
}

static _Inline int8_t _ADDS_Q7(int8_t a, int8_t b) {
    int8_t c = a + b;
    if ((( a ^ c ) & ( c ^ b )) >= 0) return c;
    TRAP()
    return ((c >= 0) ? MIN_VAL_Q7 : MAX_VAL_Q7);
}


static _Inline int32_t _SUBS_Q31(int32_t a, int32_t b) {
    int32_t c = a - b;
    if ((( a ^ c ) & ( c ^ (~b) )) >= 0) return c;
    TRAP()
    return ((c >= 0) ? MIN_VAL_Q31 : MAX_VAL_Q31);
}

static _Inline int16_t _SUBS_Q15(int16_t a, int16_t b) {
    int16_t c = a - b;
    if ((( a ^ c ) & ( c ^ (~b) )) >= 0) return c;
    TRAP()
    return ((c >= 0) ? MIN_VAL_Q15 : MAX_VAL_Q15);
}

static _Inline int8_t _SUBS_Q7(int8_t a, int8_t b) {
    int8_t c = a - b;
    if ((( a ^ c ) & ( c ^ (~b) )) >= 0) return c;
    TRAP()
    return ((c >= 0) ? MIN_VAL_Q7 : MAX_VAL_Q7);
}


static _Inline int32_t _ASLS_Q31(int32_t a, int32_t sh) {
    if(sh > 0) {
        int32_t sg = (a >= 0) ? 0 : ~0;
#if defined(__Xbarrel_shifter) 
        if (sg == (a >> (FRACT_BITS_Q31 - sh))) return ( a << sh );
#else
        if (sg == (a >> ((FRACT_BITS_Q31 - sh) & 0x1f))) return ( a << sh );
#endif
        TRAP()
        return ((sg == 0) ? MAX_VAL_Q31 : MIN_VAL_Q31);
    }
    sh = -sh;
#if !defined(__Xbarrel_shifter) 
    sh &= 0x1f;
#endif //!defined(__Xbarrel_shifter)
    return (a >> sh);
}

static _Inline int16_t _ASLS_Q15(int16_t a, int32_t sh) {
    if(sh > 0) {
        int16_t sg = (a >= 0) ? 0 : ~0;
#if defined(__Xbarrel_shifter) 
        if (sg == (a >> (FRACT_BITS_Q15 - sh))) return ( a << sh );
#else
        if (sg == (a >> ((FRACT_BITS_Q15 - sh) & 0x1f))) return ( a << sh );
#endif
        TRAP()
        return ((sg == 0) ? MAX_VAL_Q15 : MIN_VAL_Q15);
    }
    sh = -sh;
#if !defined(__Xbarrel_shifter) 
    sh &= 0x1f;
#endif //!defined(__Xbarrel_shifter)
    return  (a >> sh);
}

static _Inline int8_t _ASLS_Q7(int8_t a, int32_t sh) {
    if(sh > 0) {
        int8_t sg = (a >= 0) ? 0 : ~0;
#if defined(__Xbarrel_shifter) 
        if (sg == (a >> (FRACT_BITS_Q7 - sh))) return ( a << sh );
#else
        if (sg == (a >> ((FRACT_BITS_Q7 - sh) & 0x1f))) return ( a << sh );
#endif
        TRAP()
        return ((sg == 0) ? MAX_VAL_Q7 : MIN_VAL_Q7);
    }
    sh = -sh;
#if !defined(__Xbarrel_shifter) 
    sh &= 0x1f;
#endif //!defined(__Xbarrel_shifter)
    return  (a >> sh);
}

// Absolute value primitives

#define dspfx_abs_q31(a)     _ABSS_Q31(a)
#define dspfx_abs_q15(a)     _ABSS_Q15(a)
#define dspfx_abs_q7(a)      _ABSS_Q7(a)

// Negation primitives

#define dspfx_neg_q31(a)     _NEGS_Q31(a)
#define dspfx_neg_q15(a)     _NEGS_Q15(a)
#define dspfx_neg_q7(a)      _NEGS_Q7(a)

// Addition primitives

#define dspfx_add_q63(a, b)  _ADDS_Q63(a, b)
#define dspfx_add_q31(a, b)  _ADDS_Q31(a, b)
#define dspfx_add_q15(a, b)  _ADDS_Q15(a, b)
#define dspfx_add_q7(a, b)   _ADDS_Q7(a, b)

// Substraction primitives

#define dspfx_sub_q63(a, b)  _SUBS_Q63(a, b)
#define dspfx_sub_q31(a, b)  _SUBS_Q31(a, b)
#define dspfx_sub_q15(a, b)  _SUBS_Q15(a, b)
#define dspfx_sub_q7(a, b)   _SUBS_Q7(a, b)

// Shift left primitives (bi-directional)

#define dspfx_asl_q31(a, b)  _ASLS_Q31(a, b) 
#define dspfx_asl_q15(a, b)  _ASLS_Q15(a, b) 
#define dspfx_asl_q7(a, b)   _ASLS_Q7(a, b)  

// Multiply primitives

#define dspfx_mpy_q31(a, b)       (q31_t)(SATURATE_62((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b))) >> 31)
#define dspfx_mpy_q15(a, b)       (q15_t)(SATURATE_30((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b))) >> 15)
#define dspfx_mpy_q7(a, b)        (q7_t) (SATURATE_14((int16_t)((int8_t)(a)) * (int16_t)((int8_t)(b))) >> 7)

#define dspfx_q63_mpy_q31(a, b)       (q63_t)(SATURATE_62((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b))) << 1)
#define dspfx_q31_mpy_q15(a, b)       (q31_t)(SATURATE_30((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b))) << 1)

#define dspfx_mpy_rnd_q31(a, b)       (q31_t)(SATURATE_62((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)) + (1<<30)) >> 31)
#define dspfx_mpyh_rnd_q31(a, b)      (q31_t)(SATURATE_62((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)) + (1<<30)) >> 31)
#define dspfx_mpy_rnd_q15(a, b)       (q15_t)(SATURATE_30((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)) + (1<<14)) >> 15)

#if defined(ZERO_GUARDBITS)
#define dspfx_a64mul_q31(a,b, acc)   (q31_t)((acc = SATURATE_62((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))) >> 31)
#define dspfx_a32mul_q15(a,b, acc)   (q15_t)((acc = SATURATE_30((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))) >> 15)

#define dspfx_a64mul_nf_q31(a,b, acc)   (q31_t)((acc = SATURATE_63((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))) >> 32)
#define dspfx_a32mul_nf_q15(a,b, acc)   (q15_t)((acc = SATURATE_31((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))) >> 16)

#define dspfx_a64mac_q31(a,b, acc)   (q31_t)((acc = SATURATE_62(dspfx_add_q63((((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))), acc))) >> 31)
#define dspfx_a32mac_q15(a,b, acc)   (q15_t)((acc = SATURATE_30(dspfx_add_q31((((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))), acc))) >> 15)
#define dspfx_a16mac_q7(a,b, acc)    (q7_t) ((acc = SATURATE_14(dspfx_add_q15((((int16_t)((int8_t)(a))  * (int16_t)((int8_t) (b)))), acc))) >> 7)

#define dspfx_a64mac_nf_q31(a,b, acc)   (q31_t)(SATURATE_31(((q63_t)(acc = dspfx_add_q63((((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))), acc)) >> 32)))
#define dspfx_a32mac_nf_q15(a,b, acc)   (q15_t)(SATURATE_15((acc = dspfx_add_q31((((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))), acc)) >> 16))
#define dspfx_a16mac_nf_q7(a,b, acc)    (q7_t) (SATURATE_15((acc = dspfx_add_q15((((int16_t)((int8_t)(a))  * (int16_t)((int8_t) (b)))), acc)) >> 8))
#else
#define dspfx_a64mul_q31(a,b, acc)   (q31_t)((acc = ((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))) >> 31)
#define dspfx_a32mul_q15(a,b, acc)   (q15_t)((acc = ((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))) >> 15)

#define dspfx_a64mul_nf_q31(a,b, acc)   (q31_t)((acc = ((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))) >> 32)
#define dspfx_a32mul_nf_q15(a,b, acc)   (q15_t)((acc = ((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))) >> 16)

#define dspfx_a64mac_q31(a,b, acc)   (q31_t)(SATURATE_62(acc = dspfx_add_q63((((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))), acc)) >> 31)
#define dspfx_a64msu_q31(a,b, acc)   (q31_t)(SATURATE_62(acc = dspfx_sub_q63(acc, (((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))))) >> 31)

#define dspfx_a32mac_q15(a,b, acc)   (q15_t)(SATURATE_30(acc = dspfx_add_q31((((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))), acc)) >> 15)
#define dspfx_a32msu_q15(a,b, acc)   (q15_t)(SATURATE_30(acc = dspfx_sub_q31(acc, (((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))))) >> 15)
#define dspfx_a16mac_q7(a,b, acc)    (q7_t) (SATURATE_14(acc = dspfx_add_q15((((int16_t)((int8_t)(a))  * (int16_t)((int8_t) (b)))), acc)) >> 7)

#define dspfx_a64mac_nf_q31(a,b, acc)   (q31_t)(SATURATE_31(((q63_t)(acc = dspfx_add_q63((((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)))), acc)) >> 32)))
#define dspfx_a32mac_nf_q15(a,b, acc)   (q15_t)(SATURATE_15((acc = dspfx_add_q31((((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)))), acc)) >> 16))
#define dspfx_a16mac_nf_q7(a,b, acc)    (q7_t) (SATURATE_15((acc = dspfx_add_q15((((int16_t)((int8_t)(a))  * (int16_t)((int8_t) (b)))), acc)) >> 8))
#endif

#define dspfx_q31_cast_a64(acc)   (q31_t)(SATURATE_62(acc) >> 31)
#define dspfx_q15_cast_a32(acc)   (q15_t)(SATURATE_30(acc) >> 15)
#define dspfx_q15_cast_nf_a32(acc)   (q15_t)(SATURATE_31(acc) >> 16)
#define dspfx_q7_cast_a16(acc)    (q7_t) (SATURATE_14(acc) >> 7)

#if defined(__Xbarrel_shifter) 
#define dspfx_q31_cast_asl_a64(acc, shift)   (q31_t)(SATURATE(acc, (62-shift)) >> (31-shift))
#define dspfx_q15_cast_asl_a32(acc, shift)   (q15_t)(SATURATE(acc, (30-shift)) >> (15-shift))
#else
#define dspfx_q31_cast_asl_a64(acc, shift)   (q31_t)(SATURATE(acc, (62-shift)) >> ((31-shift) & 0x3f))
#define dspfx_q15_cast_asl_a32(acc, shift)   (q15_t)(SATURATE(acc, (30-shift)) >> ((15-shift) & 0x1f))
#endif


#endif
