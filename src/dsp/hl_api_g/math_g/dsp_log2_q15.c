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

#if !defined(EXCLUDE_DSP_LOG2_Q15)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_log2_q15")

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif

#define    coef_1  dsp_q31_cast_rnd_f32((double) 0.721872562571630 )
#define    coef_2  dsp_q31_cast_rnd_f32((double) -0.347500017928492)
#define    coef_3  dsp_q31_cast_rnd_f32((double) 0.348999752749768 )
#define    coef_4  dsp_q31_cast_rnd_f32((double) 0.196408043328879 )
#define    coef_5  dsp_q31_cast_rnd_f32((double) 0.666283329905208 )

#define minus_one   0x80000000

/**
 * Q15 base 2 logarithmic function
 *
 * \param  x - (0.5;1) in q15
 * \result result of log2 function - (-1; 0) in q15
 */

q15_t _DSPLIB_NOXYCC dsp_log2_q15(q15_t x)
{
    q31_t r;
    if (x > 0x4000) 
    {
        q31_t x_q31 = x << 16;

        x_q31 += minus_one;

        r = coef_5;
        r = dspfx_mpyh_rnd_q31(r, x_q31);
        r += coef_4;
        r = dspfx_mpyh_rnd_q31(r, x_q31);
        r += coef_3;
        r = dspfx_mpyh_rnd_q31(r, x_q31);
        r += coef_2;
        r = dspfx_mpyh_rnd_q31(r, x_q31);
        r += coef_1;
        r = dspfx_mpyh_rnd_q31(r, x_q31);

        r >>= (16-1);
    } else {
        r = 0x8000;
    }
    return (q15_t)r;
}

#endif //!defined(EXCLUDE_DSP_LOG2_Q15)
