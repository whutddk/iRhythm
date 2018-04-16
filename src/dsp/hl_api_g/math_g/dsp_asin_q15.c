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

#if !defined(EXCLUDE_DSP_ASIN_Q15)

#pragma Code("dsplib_dsp_asin_q15")

#define ONEdivPI  FLT_2_INT(int32_t,(double)1/(double)M_PI,31) 
#define HALF_PI  FLT_2_INT(int32_t,(double)M_PI/2.0,30) 

#define c0 FLT_2_INT(int32_t, (double)(1.00000000000),30)
#define c1 FLT_2_INT(int32_t, (double)(0.16748604186),30)
#define c2 FLT_2_INT(int32_t, (double)(0.06493314456),30)
#define c3 FLT_2_INT(int32_t, (double)(0.08101438546),30)

#define coeff_1_0 FLT_2_INT(int32_t,(double) 1.57078437096914669,30);
#define coeff_1_1 FLT_2_INT(int32_t,(double)-0.21415642056339812,31);
#define coeff_1_2 FLT_2_INT(int32_t,(double) 0.08512224785715758,31);
#define coeff_1_3 FLT_2_INT(int32_t,(double)-0.03687045406455980,31);
#define coeff_1_4 FLT_2_INT(int32_t,(double) 0.00938647752676486,31);

/**
 * Q31 arcsine function
 *
 * \param[input] - input [-1.0, 1.0)
 *
 * \result result of arcsine function [-0.5, 0.5) mapped to [-pi/2, pi/2)
 */

q15_t _DSPLIB_NOXYCC dsp_asin_q15(q15_t input)
{
    q31_t res, x2;
	q31_t q31input = dspfx_asl_q31((q31_t)input, 16);

	if (input == dsp_q15_cast_rnd_f32(-1.0)) return dsp_q15_cast_rnd_f32(-0.5);
	//if (input == dsp_q15_cast_rnd_f32( 1.0)) return dsp_q15_cast_rnd_f32( 0.5);
    if (dspfx_abs_q31(q31input) < dsp_q31_cast_rnd_f32(0.59))
	{
	    x2 = dspfx_mpy_rnd_q31(q31input, q31input);
		res = c3;
		res = dspfx_mpy_rnd_q31(res, x2);
		res += c2;
		res = dspfx_mpy_rnd_q31(res, x2);
		res += c1;
		res = dspfx_mpy_rnd_q31(res, x2);
		res += c0;
		res = dspfx_mpy_rnd_q31(res, q31input);
	} else {
        q31_t temp;
	    q31_t sign = (q31input < 0) ? 1 : 0;
		q31input = dspfx_abs_q31(q31input);
		res = coeff_1_4;
		res = dspfx_mpy_rnd_q31(res, q31input);
		res += coeff_1_3;
		res = dspfx_mpy_rnd_q31(res, q31input);
		res += coeff_1_2;
		res = dspfx_mpy_rnd_q31(res, q31input);
		res += coeff_1_1;
		res = dspfx_mpy_rnd_q31(res, q31input);
		res >>= 1;		
		res += coeff_1_0;
		
		temp = dspfx_add_q31(q31input, dsp_q31_cast_rnd_f32(-1.0));
		temp = dsp_sqrt_q31(temp);
		res = dspfx_mpy_rnd_q31(res, temp);

		res = dspfx_sub_q31(HALF_PI, res);
		if (sign) res = -res;
	}
	res = dspfx_mpy_rnd_q31(res, ONEdivPI);
	return (q15_t)(res >> 15);
}

#endif //!defined(EXCLUDE_DSP_ASIN_Q31)
