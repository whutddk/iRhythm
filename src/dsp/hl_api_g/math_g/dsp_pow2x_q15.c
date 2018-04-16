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

#if !defined(EXCLUDE_DSP_POW2X_Q15)

#include "dsplib_primitives.h"

#define    coef_0  dsp_q31_cast_rnd_f32((double) 0.99999611994528903)
#define    coef_1  dsp_q31_cast_rnd_f32((double) 0.69302926646963048)
#define    coef_2  dsp_q31_cast_rnd_f32((double) 0.23938498213333428)
#define    coef_3  dsp_q31_cast_rnd_f32((double) 0.05318643307763248)
#define    coef_4  dsp_q31_cast_rnd_f32((double) 0.00683823961864134)

#pragma Code("dsplib_dsp_pow2x_q15")

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif

/**
 * Q15 base 2 power function
 *
 * \param[in] x - input argument within range [-1;0] in q15
 * \result result of power function - [1;0.5] in q15
 */

q15_t _DSPLIB_NOXYCC dsp_pow2x_q15(q15_t x)
{
    q31_t r, x_q31;

    if (x >= 0) 
        return (0x7fff); //2^0 = 1
    if (x == (int16_t)0x8000) 
        return (0x4000);//2^(-1) = 0.5

    x_q31 = x << 16;

    r = coef_4;
    r = dspfx_mpyh_rnd_q31(x_q31, r);
    r += coef_3;
    r = dspfx_mpyh_rnd_q31(x_q31, r);
    r += coef_2;
    r = dspfx_mpyh_rnd_q31(x_q31, r);
    r += coef_1;
    r = dspfx_mpyh_rnd_q31(x_q31, r);
    r += coef_0;

    return (q15_t)(r >> 16);
}

#endif //!defined(EXCLUDE_DSP_POW2X_Q15)
