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

#if !defined(EXCLUDE_DSP_ASIN_Q31)

#pragma Code("dsplib_dsp_asin_q31")

#define ONEdivPI  FLT_2_INT(int32_t,(double)1/(double)M_PI,31) 
#define HALF_PI  FLT_2_INT(int32_t,(double)M_PI/2.0,30) 

#define coeff_0_0 FLT_2_INT(int32_t,(double) 0.9999999979604067,30);
#define coeff_0_1 FLT_2_INT(int32_t,(double) 0.1666669571933762,30);
#define coeff_0_2 FLT_2_INT(int32_t,(double) 0.0749880652734475,30);
#define coeff_0_3 FLT_2_INT(int32_t,(double) 0.0448601226761400,30);
#define coeff_0_4 FLT_2_INT(int32_t,(double) 0.0283203293253514,30);
#define coeff_0_5 FLT_2_INT(int32_t,(double) 0.0331802850285967,30);
#define coeff_0_6 FLT_2_INT(int32_t,(double)-0.0133796007430255,30);
#define coeff_0_7 FLT_2_INT(int32_t,(double) 0.0550214071621586,30);

#define coeff_1_0 FLT_2_INT(int32_t,(double) 1.57079632340505e+000,30);
#define coeff_1_1 FLT_2_INT(int32_t,(double)-2.14601487179448e-001,31);
#define coeff_1_2 FLT_2_INT(int32_t,(double) 8.90396680554920e-002,31);
#define coeff_1_3 FLT_2_INT(int32_t,(double)-5.06931239288404e-002,31);
#define coeff_1_4 FLT_2_INT(int32_t,(double) 3.30808215852795e-002,31);
#define coeff_1_5 FLT_2_INT(int32_t,(double)-2.21799086268193e-002,31);
#define coeff_1_6 FLT_2_INT(int32_t,(double) 1.34475715144360e-002,31);
#define coeff_1_7 FLT_2_INT(int32_t,(double)-6.31641159083450e-003,31);
#define coeff_1_8 FLT_2_INT(int32_t,(double) 1.90448697575936e-003,31);
#define coeff_1_9 FLT_2_INT(int32_t,(double)-2.64367144793196e-004,31);

/**
 * Q31 arcsine function
 *
 * \param[input] - input [-1.0, 1.0)
 *
 * \result result of arcsine function [-0.5, 0.5) mapped to [-pi/2, pi/2)
 */

q31_t _DSPLIB_NOXYCC dsp_asin_q31(q31_t input)
{
    q31_t res, x2;

	if (input == dsp_q31_cast_rnd_f32(-1.0)) return dsp_q31_cast_rnd_f32(-0.5);
	//if (input == dsp_q31_cast_rnd_f32( 1.0)) return dsp_q31_cast_rnd_f32( 0.5);
    if (dspfx_abs_q31(input) < dsp_q31_cast_rnd_f32(0.59))
	{
	    x2 = dspfx_mpy_rnd_q31(input, input);
		res = coeff_0_7;
		res = dspfx_mpy_rnd_q31(res, x2);
		res += coeff_0_6;
		res = dspfx_mpy_rnd_q31(res, x2);
		res += coeff_0_5;
		res = dspfx_mpy_rnd_q31(res, x2);
		res += coeff_0_4;
		res = dspfx_mpy_rnd_q31(res, x2);
		res += coeff_0_3;
		res = dspfx_mpy_rnd_q31(res, x2);
		res += coeff_0_2;
		res = dspfx_mpy_rnd_q31(res, x2);
		res += coeff_0_1;
		res = dspfx_mpy_rnd_q31(res, x2);
		res += coeff_0_0;
		res = dspfx_mpy_rnd_q31(res, input);
		
		res = dspfx_mpy_rnd_q31(res, ONEdivPI);
		return res << 1;
	} else {
	    q31_t temp;
	    q31_t sign = (input < 0) ? 1 : 0;
		input = dspfx_abs_q31(input);
		res = coeff_1_9;
		res = dspfx_mpy_rnd_q31(res, input);
		res += coeff_1_8;
		res = dspfx_mpy_rnd_q31(res, input);
		res += coeff_1_7;
		res = dspfx_mpy_rnd_q31(res, input);
		res += coeff_1_6;
		res = dspfx_mpy_rnd_q31(res, input);
		res += coeff_1_5;
		res = dspfx_mpy_rnd_q31(res, input);
		res += coeff_1_4;
		res = dspfx_mpy_rnd_q31(res, input);
		res += coeff_1_3;
		res = dspfx_mpy_rnd_q31(res, input);
		res += coeff_1_2;
		res = dspfx_mpy_rnd_q31(res, input);
		res += coeff_1_1;
		res = dspfx_mpy_rnd_q31(res, input);
		res >>= 1;		
		res += coeff_1_0;
		
		temp = dspfx_add_q31(input, dsp_q31_cast_rnd_f32(-1.0));
		temp = dsp_sqrt_q31(temp);
		res = dspfx_mpy_rnd_q31(res, temp);
		
		res = dspfx_sub_q31(HALF_PI, res);
		
		res = dspfx_mpy_rnd_q31(res, ONEdivPI);
		if (sign) res = -res;
		return res << 1;		
	}
	
}

#endif //!defined(EXCLUDE_DSP_ASIN_Q31)
