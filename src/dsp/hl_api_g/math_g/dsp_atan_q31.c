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

#if !defined(EXCLUDE_DSP_ATAN_Q31)

#pragma Code("dsplib_dsp_atan_q31")

#define ONEdivPI  FLT_2_INT(int32_t,(double)1/(double)M_PI,31) 

#define coeff10 dsp_q31_cast_rnd_f32((double)9.82002350456186e-04);
#define coeff9  dsp_q31_cast_rnd_f32((double)-6.61577374176831e-03);
#define coeff8  dsp_q31_cast_rnd_f32((double)2.09368008367067e-02);
#define coeff7  dsp_q31_cast_rnd_f32((double)-4.26422713060363e-02);
#define coeff6  dsp_q31_cast_rnd_f32((double)6.58129299558346e-02);
#define coeff5  dsp_q31_cast_rnd_f32((double)-8.73188780539057e-02);
#define coeff4  dsp_q31_cast_rnd_f32((double)1.10337119197102e-01);
#define coeff3  dsp_q31_cast_rnd_f32((double)-1.42752774360155e-01);
#define coeff2  dsp_q31_cast_rnd_f32((double)1.99992064172173e-01);
#define coeff1  dsp_q31_cast_rnd_f32((double)-3.33333052218497e-01);
#define coeff0  dsp_q31_cast_rnd_f32((double)9.99999997041358e-01);

/**
 * Q31 arctangent function
 *
 * \param[in] - in [-1.0, 1.0) mapped to [-pi, pi)
 *
 * \result result of tangent function
 */

q31_t _DSPLIB_NOXYCC dsp_atan_q31(q31_t in)
{
    q31_t res, x2;

    x2 = dspfx_mpy_rnd_q31(in, in);
    res = coeff10;
    res = dspfx_mpy_rnd_q31(res, x2);
    res += coeff9;
    res = dspfx_mpy_rnd_q31(res, x2);
    res += coeff8;
    res = dspfx_mpy_rnd_q31(res, x2);
    res += coeff7;
    res = dspfx_mpy_rnd_q31(res, x2);
    res += coeff6;
    res = dspfx_mpy_rnd_q31(res, x2);
    res += coeff5;
    res = dspfx_mpy_rnd_q31(res, x2);
    res += coeff4;
    res = dspfx_mpy_rnd_q31(res, x2);
    res += coeff3;
    res = dspfx_mpy_rnd_q31(res, x2);
    res += coeff2;
    res = dspfx_mpy_rnd_q31(res, x2);
    res += coeff1;
    res = dspfx_mpy_rnd_q31(res, x2);
    res += coeff0;
    res = dspfx_mpy_rnd_q31(res, in);
	
    res = dspfx_mpy_rnd_q31(res, ONEdivPI);
    return res;
}

#endif //!defined(EXCLUDE_DSP_ATAN_Q31)
