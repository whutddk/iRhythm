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

#ifndef DSPLIB_PRIMITIVES_FXAPI_H
#define DSPLIB_PRIMITIVES_FXAPI_H

//#define __GUARD_ACCUM__     //Define with guard or no guard realization


//#define __ROUNDING_MODE_TRUNC__ //Define round or truncated results of MPY/MAC

#include "fxarc.h"

#undef dspfx_abs_q31
#undef dspfx_abs_q15
#undef dspfx_abs_q7
#undef dspfx_abs_v2q15
#undef dspfx_neg_q31
#undef dspfx_neg_q15
#undef dspfx_neg_q7
#undef dspfx_neg_v2q15
#undef dspfx_add_q63
#undef dspfx_add_q31
#undef dspfx_add_q15
#undef dspfx_add_q7
#undef dspfx_add_v2q15
#undef dspfx_sub_q63
#undef dspfx_sub_q31
#undef dspfx_sub_q15
#undef dspfx_sub_q7
#undef dspfx_sub_v2q15
#undef dspfx_asl_q31
#undef dspfx_asl_q15
#undef dspfx_asl_q7
#undef dspfx_asl_v2q15_n
#undef dspfx_mpy_v2q15
#undef dspfx_mpy_q31
#undef dspfx_mpy_q15
#undef dspfx_q63_mpy_q31
#undef dspfx_q31_mpy_q15
#undef dspfx_mpy_q7
#undef dspfx_mpy_rnd_q31
#undef dspfx_mpyh_rnd_q31
#undef dspfx_mpy_rnd_q15
#undef dspfx_accum64
#undef dspfx_reset_a64
#undef dspfx_accum32
#undef dspfx_reset_a32
#undef dspfx_accum16
#undef dspfx_reset_a16
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
#undef dspfx_q31_cast_q63
#undef dspfx_q15_cast_q31
#undef dspfx_accum16
#undef dspfx_reset_a16
#undef dspfx_norm_q31
#undef dspfx_norm_q15
#undef dspfx_q63_mpy_asr_q31
#undef dspfx_get_a32

#undef DSP_MULTIPLIER_PRESENT
#define DSP_MULTIPLIER_PRESENT

//ABS
#define dspfx_abs_q31(a) (fx_abs_q31(a))
#define dspfx_abs_q15(a) (fx_abs_q15(a))
#define dspfx_abs_q7(a)  ((q7_t)(fx_abs_q31(a<<24)>>24))
#define dspfx_abs_v2q15(a) (fx_q31_cast_v2q15(fx_abs_v2q15(fx_v2q15_cast_q31(a))))

//NEG
#define dspfx_neg_q31(a) (fx_neg_q31(a))
#define dspfx_neg_q15(a) (fx_neg_q15(a))
#define dspfx_neg_q7(a)  ((q7_t)(fx_asl_q31(-a,24)>>24))
#define dspfx_neg_v2q15(a) (fx_q31_cast_v2q15(fx_neg_v2q15(fx_v2q15_cast_q31(a))))

//ADD
#define dspfx_add_q63(a, b) (fx_add_q63(a,b))
#define dspfx_add_q31(a, b) (fx_add_q31(a,b))
#define dspfx_add_q15(a, b) (fx_add_q15(a,b))
#define dspfx_add_q7(a, b)  ((q7_t)(fx_asl_q31(a + b,24)>>24))
#define dspfx_add_v2q15(a, b) (fx_q31_cast_v2q15(fx_add_v2q15(fx_v2q15_cast_q31(a),fx_v2q15_cast_q31(b))))

//SUB
#define dspfx_sub_q63(a, b) (fx_sub_q63(a,b))
#define dspfx_sub_q31(a, b) (fx_sub_q31(a,b))
#define dspfx_sub_q15(a, b) (fx_sub_q15(a,b))
#define dspfx_sub_q7(a, b)  ((q7_t)(fx_asl_q31(a - b,24)>>24))
#define dspfx_sub_v2q15(a, b) (fx_q31_cast_v2q15(fx_sub_v2q15(fx_v2q15_cast_q31(a),fx_v2q15_cast_q31(b))))

//ASL
#define dspfx_asl_q31(a, b) (fx_asl_q31(a,b))
#define dspfx_asl_q15(a, b) (fx_asl_q15(a,b))
#define dspfx_asl_q7(a, b)  ((q7_t)(fx_asl_q31((q31_t)a,b+24)>>24))
#define dspfx_asl_v2q15_n(a, b) (fx_q31_cast_v2q15(fx_asl_v2q15_n(fx_v2q15_cast_q31(a),b)))

#if defined __Xdsp_postshift_mode
#define dspfx_accshift_const (0)
#else
#define dspfx_accshift_const (1)
#endif

#ifndef __GUARD_ACCUM__
#define dspfx_mpy_v2q15(a, b) fx_q31_cast_v2q15(fx_v2q15_cast_v2a32(fx_v2a32_mpy_v2q15(fx_v2q15_cast_q31(a),fx_v2q15_cast_q31(b))))
#define dspfx_mpy_rnd_asl_v2q15(a, b, c) fx_q31_cast_v2q15(fx_v2q15_cast_asl_rnd_v2a32(fx_v2a32_mpy_v2q15(fx_v2q15_cast_q31(a),fx_v2q15_cast_q31(b)),c))
#else 
#define dspfx_mpy_v2q15(a, b) fx_q31_cast_v2q15(fx_v2q15_cast_v2a40(fx_v2a40_mpy_v2q15(fx_v2q15_cast_q31(a),fx_v2q15_cast_q31(b))))
#define dspfx_mpy_rnd_asl_v2q15(a, b, c) fx_q31_cast_v2q15(fx_v2q15_cast_asl_rnd_v2a40(fx_v2a40_mpy_v2q15(fx_v2q15_cast_q31(a),fx_v2q15_cast_q31(b)),c))
#endif

//Saturation

#ifndef __GUARD_ACCUM__

#define dspfx_mpy_q31(a,b) fx_q31_cast_a64(fx_a64_mpy_q31(a,b))
#define dspfx_mpy_q15(a,b) fx_q15_cast_a32(fx_a32_mpy_q15(a,b))
#define dspfx_mpy_q7(a, b)        (q7_t) (((int16_t)((int8_t)(a)) * (int16_t)((int8_t)(b))) >> 7)

#define dspfx_q63_mpy_q31(a,b) fx_q63_cast_a64(fx_a64_mpy_q31(a,b))
#define dspfx_q31_mpy_q15(a,b) fx_q31_cast_a32(fx_a32_mpy_q15(a,b))

#define dspfx_mpy_rnd_q31(a, b)       fx_q31_cast_rnd_a64(fx_a64_mpy_q31(a,b))
#define dspfx_mpyh_rnd_q31(a, b) fx_q31_cast_rnd_a64(fx_a64_mpy_q31(a,b))
#define dspfx_mpy_rnd_q15(a, b)       fx_q15_cast_rnd_a32(fx_a32_mpy_q15(a,b))

#define dspfx_accum16() accum32_t acc32;
#define dspfx_accum32() accum32_t acc32;
#define dspfx_accum64() accum64_t acc64;

#define dspfx_reset_a16()       acc32 =fx_a32_cast_q31(0);
#define dspfx_reset_a32()       acc32 =fx_a32_cast_q31(0);
#define dspfx_reset_a64()       acc64 =fx_a64_cast_q63(0);

#ifdef __ROUNDING_MODE_TRUNC__

#define dspfx_a64mul_q31(a,b, acc) fx_q31_cast_a64(acc64=fx_a64_mpy_q31(a,b))
#define dspfx_a32mul_q15(a,b, acc) fx_q15_cast_a32(acc32=fx_a32_mpy_q15(a,b))

#define dspfx_a64mul_nf_q31(a,b, acc) fx_q31_cast_a64(acc64=fx_asr_a64(fx_a64_mpy_q31(a,b),1))
#define dspfx_a32mul_nf_q15(a,b, acc) fx_q15_cast_a32(acc32=fx_a32_mpy_nf_q15(a,b))

#define dspfx_a64mac_q31(a,b, acc) fx_q31_cast_a64(acc64=fx_add_a64(acc64,fx_a64_mpy_q31(a,b)))
#define dspfx_a64msu_q31(a,b, acc) fx_q31_cast_a64(acc64=fx_a64_msu_q31(acc64, a, b))
#define dspfx_a32mac_q15(a,b, acc) fx_q15_cast_a32(acc32=fx_add_a32(acc32,fx_a32_mpy_q15(a,b)))
#define dspfx_a32msu_q15(a,b, acc) fx_q15_cast_a32(acc32=fx_a32_msu_q15(acc32, a, b))

#define dspfx_a16mac_q7(a,b, acc)  (q7_t)fx_sat_q15(fx_q15_cast_a32(acc32=fx_add_a32(acc32,fx_a32_mpy_q15((q15_t)a,(q15_t)b))),8)

#define dspfx_a64mac_nf_q31(a,b, acc) fx_q31_cast_a64(acc64=fx_add_a64(acc64,fx_asr_a64(fx_a64_mpy_q31(a,b),1)))
#define dspfx_a32mac_nf_q15(a,b, acc) fx_q15_cast_a32(acc32=fx_add_a32(acc32,fx_a32_mpy_nf_q15(a,b)))
#define dspfx_a16mac_nf_q7(a,b, acc)  (q7_t)fx_sat_q15(fx_q15_cast_a32(acc32=fx_add_a32(acc32,fx_a32_mpy_nf_q15((q15_t)a,(q15_t)b))),8)

#define dspfx_q31_cast_a64(acc)  fx_q31_cast_a64(acc64)
#define dspfx_q15_cast_a32(acc)   fx_q15_cast_a32(acc32)
#define dspfx_q15_cast_nf_a32(acc)   fx_q15_cast_a32(acc32)
#define dspfx_q7_cast_a16(acc)    (q7_t)fx_q7_cast_asl_rnd_a32(acc32, 16)


#define dspfx_q31_cast_asl_a64(acc, shift)   fx_q31_cast_asl_rnd_a64(acc64,shift)
#define dspfx_q15_cast_asl_a32(acc, shift)   fx_q15_cast_asl_rnd_a32(acc32,shift)
#define dspfx_q31_cast_q63(a) fx_q31_cast_q63(a)
#define dspfx_q15_cast_q31(a) fx_q15_cast_q31(a)


#define dspfx_a32dmac_q15(a,b,acc) 	fx_q15_cast_a32(acc32=fx_a32_dmac_v2q15(acc32,fx_v2q15_cast_q31(a), fx_v2q15_cast_q31(b)))

#define dspfx_a32dmac_align_q15(a,b,c,acc) \
fx_q15_cast_a32(acc32=fx_a32_dmac_v2q15(acc32,fx_v2q15_cast_q31(a), fx_align_v2q15(fx_v2q15_cast_q31(b),fx_v2q15_cast_q31(c))))



#else //ROUND

#define dspfx_a64mul_q31(a,b, acc) fx_q31_cast_rnd_a64(acc64=fx_a64_mpy_q31(a,b))
#define dspfx_a32mul_q15(a,b, acc) fx_q15_cast_rnd_a32(acc32=fx_a32_mpy_q15(a,b))

#define dspfx_a64mul_nf_q31(a,b, acc) fx_q31_cast_rnd_a64(acc64=fx_asr_a64(fx_a64_mpy_q31(a,b),1))
#define dspfx_a32mul_nf_q15(a,b, acc) fx_q15_cast_rnd_a32(acc32=fx_a32_mpy_nf_q15(a,b))

#define dspfx_a64mac_q31(a,b, acc) fx_q31_cast_rnd_a64(acc64=fx_add_a64(acc64,fx_a64_mpy_q31(a,b)))
#define dspfx_a64msu_q31(a,b, acc) fx_q31_cast_rnd_a64(acc64=fx_a64_msu_q31(acc64, a,b))
#define dspfx_a32mac_q15(a,b, acc) fx_q15_cast_rnd_a32(acc32=fx_add_a32(acc32,fx_a32_mpy_q15(a,b)))
#define dspfx_a32msu_q15(a,b, acc) fx_q15_cast_rnd_a32(acc32=fx_a32_msu_q15(acc32, a, b))

#define dspfx_a16mac_q7(a,b, acc)  (q7_t)fx_sat_q15(fx_q15_cast_rnd_a32(acc32=fx_add_a32(acc32,fx_a32_mpy_q15((q15_t)a,(q15_t)b))),8)

#define dspfx_a64mac_nf_q31(a,b, acc) fx_q31_cast_rnd_a64(acc64=fx_add_a64(acc64,fx_asr_a64(fx_a64_mpy_q31(a,b),1)))
#define dspfx_a32mac_nf_q15(a,b, acc) fx_q15_cast_rnd_a32(acc32=fx_add_a32(acc32,fx_a32_mpy_nf_q15(a,b)))
#define dspfx_a16mac_nf_q7(a,b, acc)  (q7_t)fx_sat_q15(fx_q15_cast_rnd_a32(acc32=fx_add_a32(acc32,fx_a32_mpy_nf_q15((q15_t)a,(q15_t)b))),8)


#define dspfx_q31_cast_a64(acc)    fx_q31_cast_rnd_a64(acc64)
#define dspfx_q15_cast_a32(acc)    fx_q15_cast_rnd_a32(acc32)
#define dspfx_q15_cast_nf_a32(acc)    fx_q15_cast_rnd_a32(acc32)
#define dspfx_q7_cast_a16(acc)    (q7_t)fx_q7_cast_asl_rnd_a32(acc32, 16)


#define dspfx_q31_cast_asl_a64(acc, shift)   fx_q31_cast_asl_rnd_a64(acc64,shift)
#define dspfx_q15_cast_asl_a32(acc, shift)   fx_q15_cast_asl_rnd_a32(acc32,shift)

#define dspfx_q31_cast_q63(a) fx_q31_cast_rnd_q63(a)
#define dspfx_q15_cast_q31(a) fx_q15_cast_rnd_q31(a)

#define dspfx_a32dmac_q15(a,b,acc) 	fx_q15_cast_rnd_a32(acc32=fx_a32_dmac_v2q15(acc32,fx_v2q15_cast_q31(a), fx_v2q15_cast_q31(b)))

#define dspfx_get_a32(acc) fx_q31_cast_asr_rnd_a32(acc32, 1)

#define dspfx_a32dmac_align_q15(a,b,c,acc) \
fx_q15_cast_rnd_a32(acc32=fx_a32_dmac_v2q15(acc32,fx_v2q15_cast_q31(a), fx_align_v2q15(fx_v2q15_cast_q31(b),fx_v2q15_cast_q31(c))))


#endif 

#else //__GUARD_ACCUM__

#define dspfx_mpy_q31(a,b) fx_q31_cast_a72(fx_a72_mpy_q31(a,b))
#define dspfx_mpy_q15(a,b) fx_q15_cast_a40(fx_a40_mpy_q15(a,b))
#define dspfx_mpy_q7(a, b)        (q7_t) (((int16_t)((int8_t)(a)) * (int16_t)((int8_t)(b))) >> 7)

#define dspfx_q63_mpy_q31(a,b) fx_q63_cast_a72(fx_a72_mpy_q31(a,b))
#define dspfx_q31_mpy_q15(a,b) fx_q31_cast_a40(fx_a40_mpy_q15(a,b))

#define dspfx_mpy_rnd_q31(a, b)  fx_q31_cast_rnd_a72(fx_a72_mpy_q31(a,b))
#define dspfx_mpyh_rnd_q31(a, b) fx_q31_cast_rnd_a72(fx_a72_mpy_q31(a,b))
#define dspfx_mpy_rnd_q15(a, b)  fx_q15_cast_rnd_a40(fx_a40_mpy_q15(a,b))

#define dspfx_accum16() accum40_t acc40;
#define dspfx_accum32() accum40_t acc40;
#define dspfx_accum64() accum72_t acc72;

#define dspfx_reset_a16()       acc40 = fx_create_a40(0,0);
#define dspfx_reset_a32()       acc40 = fx_create_a40(0,0);
#define dspfx_reset_a64()       acc72 = fx_a72_cast_q63(0);

#ifdef __ROUNDING_MODE_TRUNC__
#define dspfx_a64mul_q31(a,b, acc) fx_q31_cast_a72(acc72=fx_a72_mpy_q31(a,b))
#define dspfx_a32mul_q15(a,b, acc) fx_q15_cast_a40(acc40=fx_a40_mpy_q15(a,b))

#define dspfx_a64mul_nf_q31(a,b, acc) fx_q31_cast_asr_rnd_a72(acc72=(fx_a72_mpy_q31(a,b)),1)
#define dspfx_a32mul_nf_q15(a,b, acc) fx_q15_cast_a40(acc40=fx_a40_mpy_nf_q15(a,b))

#define dspfx_a64mac_q31(a,b, acc) fx_q31_cast_a72(acc72=fx_add_a72(acc72,fx_a72_mpy_q31(a,b)))
#define dspfx_a64msu_q31(a,b, acc) fx_q31_cast_a72(acc72=fx_a72_msu_q31(acc72, a, b))
#define dspfx_a32mac_q15(a,b, acc) fx_q15_cast_a40(acc40=fx_add_a40(acc40,fx_a40_mpy_q15(a,b)))
#define dspfx_a32msu_q15(a,b, acc) fx_q15_cast_a40(acc40=fx_a40_msu_q15(acc40, a, b))

#define dspfx_a16mac_q7(a,b, acc)  (q7_t)fx_sat_q15(fx_q15_cast_a40(acc40=fx_add_a40(acc40,fx_a40_mpy_q15((q15_t)a,(q15_t)b))),8)

#define dspfx_a64mac_nf_q31(a,b, acc) fx_q31_cast_asr_rnd_a72(acc72=fx_a72_mac_q31(acc72,a,b), 1)
#define dspfx_a32mac_nf_q15(a,b, acc) fx_q15_cast_a40(acc40=fx_add_a40(acc40,fx_a40_mpy_nf_q15(a,b)))
#define dspfx_a16mac_nf_q7(a,b, acc)  (q7_t)fx_sat_q15(fx_q15_cast_a40(acc40=fx_add_a40(acc40,fx_a40_mpy_nf_q15((q15_t)a,(q15_t)b))),8)

#define dspfx_q31_cast_a64(acc)  fx_q31_cast_a72(acc72)
#define dspfx_q15_cast_a32(acc)   fx_q15_cast_a40(acc40)
#define dspfx_q15_cast_nf_a32(acc)   fx_q15_cast_a40(acc40)
#define dspfx_q7_cast_a16(acc)    (q7_t)fx_q7_cast_asl_rnd_a40(acc40, 16)

#define dspfx_q31_cast_asl_a64(acc, shift)   fx_q31_cast_asl_rnd_a72(acc72,shift)
#define dspfx_q15_cast_asl_a32(acc, shift)   fx_q15_cast_asl_rnd_a40(acc40,shift)

#define dspfx_q31_cast_q63(a) fx_q31_cast_q63(a)
#define dspfx_q15_cast_q31(a) fx_q15_cast_q31(a)


#define dspfx_a32dmac_q15(a,b,acc) 	fx_q15_cast_a40(acc40=fx_a40_dmac_v2q15(acc40,fx_v2q15_cast_q31(a), fx_v2q15_cast_q31(b)))

#define dspfx_a32dmac_align_q15(a,b,c,acc) \
fx_q15_cast_a40(acc40=fx_a40_dmac_v2q15(acc40,fx_v2q15_cast_q31(a), fx_align_v2q15(fx_v2q15_cast_q31(b),fx_v2q15_cast_q31(c))))

#else //ROUND

#define dspfx_a64mul_q31(a,b, acc) fx_q31_cast_rnd_a72(acc72=fx_a72_mpy_q31(a,b))
#define dspfx_a32mul_q15(a,b, acc) fx_q15_cast_rnd_a40(acc40=fx_a40_mpy_q15(a,b))

#define dspfx_a64mul_nf_q31(a,b, acc) fx_q31_cast_asr_rnd_a72(acc72=(fx_a72_mpy_q31(a,b)),1)
#define dspfx_a32mul_nf_q15(a,b, acc) fx_q15_cast_rnd_a40(acc40=fx_a40_mpy_nf_q15(a,b))

#define dspfx_a64mac_q31(a,b, acc) fx_q31_cast_rnd_a72(acc72=fx_add_a72(acc72,fx_a72_mpy_q31(a,b)))
#define dspfx_a64msu_q31(a,b, acc) fx_q31_cast_rnd_a72(acc72=fx_a72_msu_q31(acc72, a, b))

#define dspfx_a32mac_q15(a,b, acc) fx_q15_cast_rnd_a40(acc40=fx_add_a40(acc40,fx_a40_mpy_q15(a,b)))
#define dspfx_a32msu_q15(a,b, acc) fx_q15_cast_rnd_a40(acc40=fx_a40_msu_q15(acc40, a, b))

#define dspfx_a16mac_q7(a,b, acc)  (q7_t)fx_sat_q15(fx_q15_cast_rnd_a40(acc40=fx_add_a40(acc40,fx_a40_mpy_q15((q15_t)a,(q15_t)b))),8)

#define dspfx_a64mac_nf_q31(a,b, acc) fx_q31_cast_asr_rnd_a72(acc72=fx_a72_mac_q31(acc72,a,b), 1)
#define dspfx_a64msu_nf_q31(a,b, acc) fx_q31_cast_asr_rnd_a72(acc72=fx_a72_msu_q31(acc72,a,b), 1)

#define dspfx_a32mac_nf_q15(a,b, acc) fx_q15_cast_rnd_a40(acc40=fx_add_a40(acc40,fx_a40_mpy_nf_q15(a,b)))
#define dspfx_a16mac_nf_q7(a,b, acc)  (q7_t)fx_sat_q15(fx_q15_cast_rnd_a40(acc40=fx_add_a40(acc40,fx_a40_mpy_nf_q15((q15_t)a,(q15_t)b))),8)

#define dspfx_q31_cast_a64(acc)  fx_q31_cast_rnd_a72(acc72)
#define dspfx_q15_cast_a32(acc)   fx_q15_cast_rnd_a40(acc40)
#define dspfx_q15_cast_nf_a32(acc)   fx_q15_cast_rnd_a40(acc40)
#define dspfx_q7_cast_a16(acc)    (q7_t)fx_q7_cast_asl_rnd_a40(acc40, 16)


#define dspfx_get_a32(acc) fx_q31_cast_asr_rnd_a40(acc40, 1)

#define dspfx_q31_cast_asl_a64(acc, shift)   fx_q31_cast_asl_rnd_a72(acc72, shift)
#define dspfx_q15_cast_asl_a32(acc, shift)   fx_q15_cast_asl_rnd_a40(acc40, shift)

#define dspfx_q31_cast_q63(a) fx_q31_cast_rnd_q63(a)
#define dspfx_q15_cast_q31(a) fx_q15_cast_rnd_q31(a)

#define dspfx_a32dmac_q15(a,b,acc) 	fx_q15_cast_rnd_a40(acc40=fx_a40_dmac_v2q15(acc40,fx_v2q15_cast_q31(a), fx_v2q15_cast_q31(b)))

#define dspfx_a32dmac_align_q15(a,b,c,acc) \
fx_q15_cast_rnd_a40(acc40=fx_a40_dmac_v2q15(acc40,fx_v2q15_cast_q31(a), fx_align_v2q15(fx_v2q15_cast_q31(b),fx_v2q15_cast_q31(c))))

#endif 
#endif  //__GUARD_ACCUM__


#ifdef __GUARD_ACCUM__

#define dspfx_v2accum32()    v2accum40_t v2acc40;
#define dspfx_v2a32_reset()    v2acc40 = fx_v2a40_cast_v2q15(fx_create_v2q15(0, 0));
#define dspfx_v2a32mul_v2q15(a, b) v2acc40 = fx_v2a40_mpy_v2q15((a),(b))
#define dspfx_v2a32mac_v2q15(a, b) v2acc40 = fx_v2a40_mac_v2q15(v2acc40, (a),(b))
#define dspfx_q63_get_cast_a32(acc) ((q63_t)fx_q31_cast_asr_rnd_a40(acc40,1))
#define dspfx_q15_get_cast_v2a32(n) (fx_q15_cast_rnd_a40(fx_get_v2a40(v2acc40, n)))
#define dspfx_q63_mpy_asr_q31(a, b, shift) fx_asr_q63(fx_q63_cast_a72(fx_a72_mpy_q31((a), (b))), (shift)+1)

#define dspfx_v2accum40()    v2accum40_t v2acc40;
#define dspfx_v2a40mul_v2q15(a, b) v2acc40 = fx_v2a40_mpy_v2q15((a),(b))
#define dspfx_v2a40mac_v2q15(a, b) v2acc40 = fx_v2a40_mac_v2q15(v2acc40, (a),(b))
#define dspfx_q63_get_cast_v2a40(n) ((q63_t)(((int64_t)fx_extract_hi_a40(fx_get_v2a40(v2acc40, n))) << 32 | (uint64_t)fx_extract_lo_a40(fx_get_v2a40(v2acc40, n))) >> dspfx_accshift_const)

#define dspfx_cjmpy_cq15(a, b) fx_cq15_cast_rnd_v2a40(fx_v2a40_cjmpy_cq15((a), (b)))
//#define dspfx_cjmpy_asr_cq15(a, b, shift) fx_cq15_cast_asr_rnd_v2a40(fx_v2a40_cjmpy_cq15((a), (b)), (shift))
#define dspfx_cjmpy_asr_cq15(a, b, shift) fx_asr_rnd_cq15(fx_cq15_cast_rnd_v2a40(fx_v2a40_cjmpy_cq15((a), (b))), (shift))


#define dspfx_cmpy_cq15(a, b) fx_cq15_cast_rnd_v2a40(fx_v2a40_cmpy_cq15((a), (b)))

#define dspfx_v2a32cjmac_cq15(a, b) v2acc40 = fx_v2a40_cjmac_cq15(v2acc40, (a), (b))
#define dspfx_q31_get_cast_asr_v2a32(a, s) fx_q31_cast_asr_rnd_a40(fx_get_v2a40(v2acc40, (a)), (s))

#else

#define dspfx_v2accum32()    v2accum32_t v2acc32;
#define dspfx_v2a32_reset()    v2acc32 = fx_v2a32_cast_v2q15(fx_create_v2q15(0, 0));

#define dspfx_v2a32mul_v2q15(a, b) v2acc32 = fx_v2a32_mpy_v2q15((a),(b))
#define dspfx_v2a32mac_v2q15(a, b) v2acc32 = fx_v2a32_mac_v2q15(v2acc32, (a),(b))

#define dspfx_q15_get_cast_v2a32(n) (fx_q15_cast_rnd_a32(fx_get_v2a32(v2acc32, n)))
#define dspfx_q63_mpy_asr_q31(a, b, shift) fx_asr_q63(fx_q63_cast_a64(fx_a64_mpy_q31((a), (b))), (shift)+1)

#define dspfx_cjmpy_cq15(a, b) fx_cq15_cast_rnd_v2a32(fx_v2a32_cjmpy_cq15((a), (b)))
//#define dspfx_cjmpy_asr_cq15(a, b, shift) fx_cq15_cast_asr_rnd_v2a32(fx_v2a32_cjmpy_cq15((a), (b)), (shift))
#define dspfx_cjmpy_asr_cq15(a, b, shift) fx_asr_rnd_cq15(fx_cq15_cast_rnd_v2a32(fx_v2a32_cjmpy_cq15((a), (b))), (shift))

#define dspfx_cmpy_cq15(a, b) fx_cq15_cast_rnd_v2a32(fx_v2a32_cmpy_cq15((a), (b)))

#define dspfx_v2a32cjmac_cq15(a, b) v2acc32 = fx_v2a32_cjmac_cq15(v2acc32, (a), (b))
#define dspfx_q31_get_cast_asr_v2a32(a, s) fx_q31_cast_asr_rnd_a32(fx_get_v2a32(v2acc40, (a)), (s))

#endif

#define dspfx_create0_v2q15(a) fx_create_v2q15(0,(a))
#define dspfx_replic_v2q15(a) fx_replic_v2q15((a))
#define dspfx_v2q15_cast_q31(a) fx_v2q15_cast_q31(a)

// Norm
#define dspfx_norm_q31(a) (fx_norm_q31(a))
#define dspfx_norm_q15(a) (fx_norm_q15(a))

#endif //DSPLIB_PRIMITIVES_FXAPI_H
