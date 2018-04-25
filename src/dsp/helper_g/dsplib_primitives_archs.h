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

#ifndef DSPLIB_PRIMITIVES_ARC_HS_H
#define DSPLIB_PRIMITIVES_ARC_HS_H

#undef dspfx_mpy_rnd_q31
#undef dspfx_mpyh_rnd_q31
#undef dspfx_mpy_rnd_q15
#undef DSP_MULTIPLIER_PRESENT

#define DSP_MULTIPLIER_PRESENT				// flag that indicates presence of any multiplier unit on platform

// q31 multiplication with round
_Inline static q31_t _mpy_round_q31(q31_t a, q31_t b) {
    int32_t round_b = (1 << 15);
    int64_t mpy_tmp = (int64_t)round_b*round_b;
    mpy_tmp += (int64_t)a * b;
    return (q31_t)((mpy_tmp) >> 31);
}

_Inline static q15_t _mpy_round_q15(q15_t a, q15_t b) {
    int16_t round_b = (1 << 7);
    int32_t mpy_tmp = (int32_t)round_b*round_b;
    mpy_tmp += (int32_t)a * b;
    return (q15_t)((mpy_tmp) >> 15);
}

//#define dspfx_mpy_rnd_q31(a, b) (q31_t)(_mpy_round_q31(a, b))
//#define dspfx_mpyh_rnd_q31(a, b) (q31_t)(_mpy_round_q31(a, b))
//#define dspfx_mpy_rnd_q15(a, b) (q15_t)(_mpy_round_q15(a, b))

//#define dspfx_mpy_rnd_q31(a, b)  (q31_t)(((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b))) >> 31)
//#define dspfx_mpyh_rnd_q31(a, b) (q31_t)(((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b))) >> 31)
//#define dspfx_mpy_rnd_q15(a, b)  (q15_t)(((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b))) >> 15)

#define dspfx_mpy_rnd_q31(a, b)       (q31_t)(SATURATE_62((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)) + (1<<30)) >> 31)
#define dspfx_mpyh_rnd_q31(a, b)      (q31_t)(SATURATE_62((int64_t)((int32_t)(a)) * (int64_t)((int32_t)(b)) + (1<<30)) >> 31)
#define dspfx_mpy_rnd_q15(a, b)       (q15_t)(SATURATE_30((int32_t)((int16_t)(a)) * (int32_t)((int16_t)(b)) + (1<<14)) >> 15)


#endif
