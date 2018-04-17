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

#include "dsplib_types.h"
#include "dsplib_primitives.h"


#if !defined(EXCLUDE_DSP_COS_F32)
#pragma Code("dsplib_dsp_cos_f32")

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif

/**
 * f32 sine
 *
 * \param[in]  in
 * \result f32_t result of sine
 */

static q31_t _Inline dsp_cast_f32_to_q31_1_pi(void *f) {
	q31_t sign, m, e;
	q31_t *pf = (q31_t*) f;
	dspfx_accum64();
	sign = (uint32_t) *pf >> 31;
	m = ((uint32_t)(*pf << 9)>>3)|( 1 << 29); // 3q29
	m = sign ? -m:m;
	dspfx_a64mul_q31(m, 683565275, acc); //multiply by 1/pi
	e = ((*pf >> 23) & 0xff) - 125;
	m = dspfx_q31_cast_asl_a64(acc, e);
	return m;
};

static void _Inline dsp_cast_q31_to_f32(q31_t in, void* out){
	q31_t sign, m, e;
	q31_t *result = (q31_t*)out;
	sign = (uint32_t) in & 0x80000000;
	m = dspfx_abs_q31 (in);
	e = dspfx_norm_q31(m);
	m = (m << e) >> 7;
	m &= 0xFF7FFFFF ;
	*result = sign | m | ((uint32_t)(126- e) << 23);
};

f32_t _DSPLIB_NOXYCC dsp_cos_f32(f32_t in)
{
#if !defined __Xfpu_mac
	q31_t res, arg;
	f32_t out;
#else
	f32_t x2, out;

	const f32_t c0 =  1.0f;
	const f32_t c1 =  -0.500000000f;         //-1/2!
	const f32_t c2 =0.041666666666f;      //1/4!
	const f32_t c3 =-0.00138888888f;       //-1/6!
	const f32_t c4 =2.480158730158e-05f;  //1/8!
	const f32_t c5 =-2.7557319223985e-07f; //1/10!
	const f32_t c6 =2.087675698786e-09f;  //1/12!
	const f32_t c7 =-1.147074559772e-011f; //1/14!
	const f32_t c8 =4.779477332387e-014f; //1/16!
	const f32_t c9 =-1.5619206968586e-016f;  //1/18!
#endif

	if (dspfx_abs_f32(in) > (f32_t)M_PI) {
	   f32_t c = in > 0 ? M_PI : -M_PI;
 	   int32_t sign=((int32_t)((in + c)*(f32_t)M_1_2PI));
       in = in - (sign*(f32_t)M_2PI);
	}
#if defined(__Xfpu_mac)
    x2  = (in * in);
    out = (x2  * c9);
    out+= c8;
    out = (out * x2);
    out+= c7;
    out = (out * x2);
    out+= c6;
    out = (out * x2);
    out+= c5;
    out = (out * x2);
    out+= c4;
    out = (out * x2);
    out+= c3;
    out = (out * x2);
    out+= c2;
    out = (out * x2);
    out+= c1;
    out = (out * x2);
    out+= c0;
    return (out);
#else
	arg = dsp_cast_f32_to_q31_1_pi((void*) &in);
	res = dsp_cos_q31(arg);
	//out = (float)res/2147483648.0;
	dsp_cast_q31_to_f32(res, &out);
	return out;
#endif
};

#endif //!defined(EXCLUDE_DSP_COS_F32)

#if !defined(EXCLUDE_DSP_SIN_F32)
#pragma Code("dsplib_dsp_sin_f32")

/**
 * f32 cosine
 *
 * \param  in
 * \result f32_t result of cosine
 */

f32_t _DSPLIB_NOXYCC dsp_sin_f32(f32_t in)
{
#if defined(__Xfpu_mac)
	f32_t x2, out;

	const f32_t c0 =1.0f;                   // 1
	const f32_t c1 =-0.16666666666f;       //-1/3!
	const f32_t c2 =0.00833333333f;       // 1/5!
	const f32_t c3 =-1.98412698412e-04f;   //-1/7!
	const f32_t c4 =2.75573192239e-06f;   // 1/9!
	const f32_t c5 =-2.50521083854e-08f;   //-1/11!
	const f32_t c6 =1.60590438368e-010f;  //-1/13!
	const f32_t c7 =-7.64716373181e-013f;  //-1/15!
	const f32_t c8 =2.81145725434e-015f;   // 1/17!
#else
	q31_t res, arg;
	f32_t out;
#endif

	if (dspfx_abs_f32(in) > (f32_t)M_PI) {
	   f32_t c = in > 0 ? M_PI : -M_PI;
 	   int32_t sign=((int32_t)((in + c)*(f32_t)M_1_2PI));
       in = in - (sign*(f32_t)M_2PI);
	}
#if defined(__Xfpu_mac)


    x2 = (in * in);
    out =(c8* x2);
    out+=c7;
    out =(out * x2);
    out+=c6;
    out =(out * x2);
    out+=c5;
    out =(out * x2);
    out+=c4;
    out =(out * x2);
    out+=c3;
    out =(out * x2);
    out+=c2;
    out =(out * x2);
    out+=c1;
    out =(out * x2);
    out+=c0;
    out =(out * in);
    return (out);
#else
	arg = dsp_cast_f32_to_q31_1_pi((void*) &in);
	res = dsp_sin_q31(arg);
	//out = (float)res/2147483648.0;
	dsp_cast_q31_to_f32(res, &out);
	return out;
#endif
};


#endif //!defined(EXCLUDE_DSP_SIN_F32)
