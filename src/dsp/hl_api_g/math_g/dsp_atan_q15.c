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

#include "dsplib_primitives.h"

#if !defined(EXCLUDE_DSP_ATAN_Q15)

#pragma Code("dsplib_dsp_atan_q15")

#define ONEdivPI  FLT_2_INT(int32_t,(double)1/(double)M_PI,31) 

#define coeff4 dsp_q31_cast_rnd_f32((double)2.18413238587152e-02)
#define coeff3 dsp_q31_cast_rnd_f32((double)-8.71243495676754e-02)
#define coeff2 dsp_q31_cast_rnd_f32((double)1.81421019970979e-01)
#define coeff1 dsp_q31_cast_rnd_f32((double)-3.30592800364426e-01)
#define coeff0 dsp_q31_cast_rnd_f32((double)9.99882984368714e-01)

/**
 * Q15 arctangent function
 *
 * \param[in] - in [-1.0, 1.0)
 *
 * \result result of arctangent function [-0.25, 0.25) mapped to [-pi/4, pi/4)
 */

q15_t _DSPLIB_NOXYCC dsp_atan_q15(q15_t in)
{
    q31_t res,x2;
    q31_t in31 = (q31_t)in << 16;

    x2 = dspfx_mpyh_rnd_q31(in31, in31);

    res = coeff4;
    res = dspfx_mpyh_rnd_q31(res, x2);
    res += coeff3;
    res = dspfx_mpyh_rnd_q31(res, x2);
    res += coeff2;
    res = dspfx_mpyh_rnd_q31(res, x2);
    res += coeff1;
    res = dspfx_mpyh_rnd_q31(res, x2);
    res += coeff0;
    res = dspfx_mpyh_rnd_q31(res, in31);

    res = dspfx_mpy_rnd_q31(res, ONEdivPI);	
    return (q15_t)(res >> 16);
}

#endif //!defined(EXCLUDE_DSP_ATAN_Q15)
