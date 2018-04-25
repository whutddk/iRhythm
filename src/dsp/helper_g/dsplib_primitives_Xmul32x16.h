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

#ifndef DSPLIB_PRIMITIVES_MUL32x16_H
#define DSPLIB_PRIMITIVES_MUL32x16_H

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
#undef dspfx_accum16
#undef dspfx_reset_a16
#undef dspfx_get_a32
#undef DSP_MULTIPLIER_PRESENT

#define DSP_MULTIPLIER_PRESENT				// flag that indicates presence of any multiplier unit on platform

_Inline static q31_t _mulf_q31(q31_t a, q31_t b) {
    _mulflw(a, b);
    return _machflw(a, b);
}
_Inline static q31_t _macf_q31(q31_t a, q31_t b) {
    _macflw(a, b);
    return _machflw(a, b);
}
_Inline static q31_t _mulnf_q31(q31_t a, q31_t b) {
    _mullw(a, b);
    return _machlw(a, b);
}
_Inline static q31_t _macnf_q31(q31_t a, q31_t b) {
    _maclw(a, b);
    return _machlw(a, b);
}
// q63 and q31 result
_Inline static q63_t _mulf_q63_q31(q31_t a, q31_t b) {
    _mulflw(a, b);
    _machflw(a, b);
    return ((int64_t)_get_ACC1() << 32) | (int64_t)(_get_ACC2());
}
_Inline static q31_t _mulf_q31_q15(q15_t a, q15_t b) {
    _mulhflw((q31_t)a << 16, (q31_t)b << 16);
    return _get_ACC1();
}

// q31 multiplication with round
_Inline static q31_t _mpy_round_q31(q31_t a, q31_t b) {
    int32_t round_b = (1 << 15);
    _mulflw(round_b, round_b);
    _macflw(a, b);
    return _machflw(a, b);
}
_Inline static q31_t _mpy_round_q31_short(q31_t a, q31_t b) {
    int32_t round_b = (1 << 23);
    _mulhflw(round_b, round_b);
    return _machflw(a, b);
}
_Inline static q15_t _mpy_round_q15(q15_t a, q15_t b) {
    int32_t round_b = (1 << 15);
    _mulflw(round_b, round_b);
    return (q15_t)_machflw(a, b << 16);
}

#define dspfx_mpy_q31(a, b)       (q31_t)(_mulf_q31(a, b))
#define dspfx_mpy_q15(a, b)       (q15_t)((q31_t)_mulhflw(dspfx_asl_q31(a, 16),dspfx_asl_q31(b,16)) >> 16)
#define dspfx_mpy_q7(a, b)        (q7_t)((q31_t)_mulhflw(dspfx_asl_q31(a, 24),dspfx_asl_q31(b, 24)) >> 24)

#define dspfx_q63_mpy_q31(a,b) ((q63_t)_mulf_q63_q31(a,b))
#define dspfx_q31_mpy_q15(a,b) ((q31_t)_mulf_q31_q15(a,b))

#define dspfx_mpy_rnd_q31(a, b) (q31_t)(_mpy_round_q31(a, b))
#define dspfx_mpyh_rnd_q31(a, b) (q31_t)(_mpy_round_q31_short(a, b))
#define dspfx_mpy_rnd_q15(a, b) (q15_t)(_mpy_round_q15(a, b))

#define DEFINE_ACC(acc)        
#define dspfx_reset_a64()         _mulflw(0, 0);

#define dspfx_a64mul_q31(a, b, acc)   (q31_t)(_mulf_q31(a, b))
#define dspfx_a32mul_q15(a, b, acc)   (q15_t)(_mulhflw(dspfx_asl_q31(a, 16), dspfx_asl_q31(b, 16)) >> 16)
#define dspfx_a32msu_q15(a, b, acc)   (q15_t)(_mulhflw(dspfx_asl_q31(a, 16), dspfx_neg_q31(dspfx_asl_q31(b, 16))) >> 16)

#define dspfx_a64mul_nf_q31(a, b, acc)   (q31_t)(_mulnf_q31(a, b))
#define dspfx_a32mul_nf_q15(a, b, acc)   (q15_t)(_mulhlw(dspfx_asl_q31(a, 16), dspfx_asl_q31(b, 16)) >> 16)

#define dspfx_a64mac_q31(a, b, acc)   (q31_t)(_macf_q31(a, b))

#define dspfx_a64msu_q31(a, b, acc)   (q31_t)(_macf_q31(a, dspfx_neg_q31(b)))

#define dspfx_a32mac_q15(a, b, acc)   (q15_t)(_machflw(dspfx_asl_q31(a, 16), dspfx_asl_q31(b, 16)) >> 16)
#define dspfx_a16mac_q7(a,b, acc)    (q7_t) (_machflw(dspfx_asl_q31(a, 24), dspfx_asl_q31(b, 24)) >> 24)

#define dspfx_a64mac_nf_q31(a, b, acc)   (q31_t)(_macnf_q31(a, b))
#define dspfx_a32mac_nf_q15(a, b, acc)   (q15_t)(_machlw(dspfx_asl_q31(a, 16), dspfx_asl_q31(b, 16)) >> 16)
#define dspfx_a16mac_nf_q7(a,b, acc)     (q7_t) (_machlw(dspfx_asl_q31(a, 24), dspfx_asl_q31(b, 24)) >> 24)

#define dspfx_q31_cast_a64(acc)   (q31_t)(_get_ACC1())
#define dspfx_q15_cast_a32(acc)   (q15_t)(_get_ACC1() >> 16)
#define dspfx_q15_cast_nf_a32(acc)   (q15_t)(_get_ACC1() >> 16)
#define dspfx_q7_cast_a16(acc)    (q7_t) (_get_ACC1() >> 24)

_Inline static q31_t _q31_cast_asl_a64(int32_t shift) {
    int32_t res = dspfx_asl_q31(_get_ACC1(), shift);
    if (shift > 0) res |= ((uint32_t)_get_ACC2()) >> (32-shift);
    return res;
}

#define dspfx_q31_cast_asl_a64(acc, shift)   _q31_cast_asl_a64(shift)
#define dspfx_q15_cast_asl_a32(acc, shift)   (q15_t)SATURATE_15((int32_t)_get_ACC1() >> (16-shift))

#define dspfx_accum64()  	
#define dspfx_reset_a64()  	_mulflw(0, 0);
#define dspfx_accum32()  	
#define dspfx_reset_a32()  	_mulflw(0, 0);
#define dspfx_accum16()  	
#define dspfx_reset_a16()  	_mulflw(0, 0);

#define dspfx_get_a32(acc)     (_get_ACC1() >> 1)

#endif
