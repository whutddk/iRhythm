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
#include "dsplib_helper.h"

#if defined __Xdsp
#include <arc/arc_intrinsics.h>
#endif

#if !defined(EXCLUDE_DSP_DIV_Q31)

const int32_t mul8[] = {
    0x71c70000,
    0x66660000,
    0x5d170000,
    0x55550000,
    0x4ec40000,
    0x49240000,
    0x44440000,
    0x40000000, 
};

#pragma Code("dsplib_dsp_div_q31")

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif

/**
 * Q31 division for numerator < denumerator
 *
 * \param num
 * \param den
 * \result q31_t result of division
 */

q31_t _DSPLIB_NOXYCC dsp_div_q31(q31_t num, q31_t den)
#if defined(_ARC) && defined(__Xdsp) && __Xdsp_version == 1
{
	q31_t Lm = _abss(num);
	q31_t L_denom;
	q31_t sign;
	q31_t var_out;
	// initialize low accumulator to numerator
	// set high accumulator to 0
	_setacc(Lm, 0x1);
	L_denom = _abss(den);
	sign = (num ^ den);
	if (Lm >= L_denom) return sign < 0 ? 0x80000000 : 0x7fffffff;
	for (int i = 0; i < 31; i++)
	{
		_divacc(L_denom);
	}
	var_out = _getacc(0x0);
	if ((sign ^ var_out) < 0) var_out = _negs(var_out);
	return var_out;
}
#elif defined(_ARC) && defined(__Xdsp) && (__Xdsp_version > 1) && (defined(__Xdsp_divsqrt_radix2) || defined(__Xdsp_divsqrt_radix4))
{
	return _divf(num, den);
}
#else
{
    q31_t m, e;
    const q31_t one = 0x7fffffff;
    q31_t sign = (num ^ den); 

    num = dspfx_abs_q31(num);
    den = dspfx_abs_q31(den);
    ASSERT(num <= den);
    num = dspfx_min_q31(num,den);

    /* normalize the fraction by the denominator */
    e = dspfx_norm_q31(den);

    den <<= e; 
    num <<= e; 

    m = mul8[(den >> 27) & 0x7] ; /* 1Q30 */

    den = dspfx_mpy_q31(den, m);
    num = dspfx_mpy_rnd_q31(num, m);

    /* 5 correct bits */
    m = one - den; /* 1Q30 */
    den<<=1;num<<=1;

    den = dspfx_mpy_rnd_q31(den, m );
    num = dspfx_mpy_rnd_q31(num, m );

    /* 10 correct bits */
    m = one - den; /* 1Q30 */
    den<<=1;num<<=1;

    den = dspfx_mpy_rnd_q31(den, m );
    num = dspfx_mpy_rnd_q31(num, m );

    /* 20 correct bits */
    m = one - den;/* 1Q30 */
    den<<=1;num<<=1;

    den = dspfx_mpy_rnd_q31(den, m );
    num = dspfx_mpy_rnd_q31(num, m );

    /* ~27 correct bits*/
    m = one - den;/* 1Q30 */
    num<<=1;

    num = dspfx_mpy_rnd_q31(num, m );

    num<<=1;

    if ((sign ^ num) < 0)
        num = dspfx_neg_q31(num);

    return num;
}
#endif

#endif //!defined(EXCLUDE_DSP_DIV_Q31)

#if !defined(EXCLUDE_DSP_DIV_Q31_Q15)

/**
 * Q31 division for numerator (q31) < denumerator (q15). 
 * Both arguments are fixed point fractional data!!!
 *
 * \param num
 * \param den
 * \result q31_t result of division
 */

#pragma Code("dsplib_dsp_div_q31_q15")

q31_t _DSPLIB_NOXYCC dsp_div_q31_q15(q31_t num, q15_t den)
{
#if defined __Xea
    q31_t var_out;
    int32_t i;
    q31_t Lm, L_denom;
    q31_t sign;

    Lm = num;
    L_denom = (q31_t)den<<16;
    sign = (Lm ^ L_denom);

    Lm = dspfx_abs_q31(Lm);
    L_denom = dspfx_abs_q31(L_denom);
    Lm = dspfx_min_q31(Lm,L_denom);

    Lm = _divaw(Lm,L_denom);
    for(i=0;i<7;i++)
    {
        Lm = _divaw(Lm,L_denom);
        Lm = _divaw(Lm,L_denom);
    } // remainder in MSW of Lm quotient in SW of Lm
    var_out = (Lm & 0x7fff) << 16;
    Lm &= 0xffff8000;
    for(i=0;i<8;i++)
    {
        Lm = _divaw(Lm,L_denom);
        Lm = _divaw(Lm,L_denom);
    } // remainder in MSW of Lm quotient in SW of Lm
    var_out |= Lm & 0xffff;

    if ((sign ^ var_out) < 0)
        var_out = dspfx_neg_q31(var_out);

    return(var_out);
#elif defined(_ARC) && defined(__Xdsp) && (__Xdsp_version > 1) && (defined(__Xdsp_divsqrt_radix2) || defined(__Xdsp_divsqrt_radix4))
{
	return _divf(num, (q31_t)den << 16);
}
#else
    q31_t m, e;
    const q31_t one = 0x7fffffff;
    q31_t sign = (num ^ (den << 16)); 
    q31_t den31;

    num = dspfx_abs_q31(num);
    den31 = dspfx_abs_q31(den << 16);
    //ASSERT(num <= den31);
    num = dspfx_min_q31(num,den31);

    /* normalize the fraction by the denominator */
    e = dspfx_norm_q31(den31);

    den31 <<= e; 
    num <<= e; 

    m = mul8[(den31 >> 27) & 0x7] ; /* 1Q30 */

    den31 = dspfx_mpy_rnd_q31(den31, m);
    num = dspfx_mpy_rnd_q31(num, m);

    /* 5 correct bits */
    m = one - den31; /* 1Q30 */
    den31<<=1;num<<=1;

    den31 = dspfx_mpy_rnd_q31(den31, m );
    num = dspfx_mpy_rnd_q31(num, m );

    /* 10 correct bits */
    m = one - den31; /* 1Q30 */
    den31<<=1;num<<=1;

    den31 = dspfx_mpy_rnd_q31(den31, m );
    num = dspfx_mpy_rnd_q31(num, m );

    /* 20 correct bits */
    m = one - den31;/* 1Q30 */
    den31<<=1;num<<=1;

    den31 = dspfx_mpy_rnd_q31(den31, m );
    num = dspfx_mpy_rnd_q31(num, m );

    /* ~27 correct bits*/
    m = one - den31;/* 1Q30 */
    num<<=1;

    num = dspfx_mpy_rnd_q31(num, m );

    num<<=1;

    if ((sign ^ num) < 0)
        num = dspfx_neg_q31(num);

    return num;
#endif
}

#endif //!defined(EXCLUDE_DSP_DIV_Q31_Q15)
