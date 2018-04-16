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

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif

#if !defined(EXCLUDE_DSP_COS_Q31)
#pragma Code("dsplib_dsp_cos_q31")

//q28
#define cos_c0  FLT_2_INT(int32_t, (double)(0.9999999995343387126922607421875), 31)
#define cos_c1  FLT_2_INT(int32_t, (double)(-4.9348021253518768 ),   28)
#define cos_c2  FLT_2_INT(int32_t, (double)(4.0587090644128612),   28)
#define cos_c3  FLT_2_INT(int32_t, (double)(-1.3352125899095002 ),   28)
#define cos_c4  FLT_2_INT(int32_t, (double)(0.2349452319812032),   28)
#define cos_c5  FLT_2_INT(int32_t, (double)(-0.0244156210001610 ),   28)

/**
 * Q31 sine
 *
 * \param[in] - in [-1.0, 1.0) mapped to [-pi, pi)
 * \result result of sine
 */


q31_t _DSPLIB_NOXYCC dsp_cos_q31(q31_t in)
{
    q31_t x2, out, sign = 0;
    q31_t arg = in;

    if (arg == (int32_t)0x80000000) //pi,-pi
        return 0x80000000;
    if (arg == 0) //0, pi, -pi
        return 0x7fffffff;
    if ((arg == (int32_t)0x40000000) || (arg == (int32_t)0xc0000000)) //pi/2,-pi/2
        return 0; 

    if (arg < 0)
        arg = -arg;
    if (arg > dsp_q31_cast_rnd_f32((double) 0.5 )) {
        arg = -(dsp_q31_cast_rnd_f32((double) -1.0 ) + arg);
        sign = 1;
    }
    //arg - q31, c[i] - q28, out - q28, return - q31
    x2 = dspfx_mpy_rnd_q31(arg, arg);

    out = cos_c5;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += cos_c4;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += cos_c3;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += cos_c2;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += cos_c1;
    out = dspfx_mpy_rnd_q31(out, 4*x2);
    out = dspfx_asl_q31(out,1);
    out += cos_c0;

    if (sign)
        out = -out; 

    return out;
}

#endif //!defined(EXCLUDE_DSP_COS_Q31)

#if !defined(EXCLUDE_DSP_SIN_Q31)
#pragma Code("dsplib_dsp_sin_q31")
//q28
#define sin_c0  FLT_2_INT(int32_t, (double)(3.14159265404833970  ), 29)
#define sin_c1  FLT_2_INT(int32_t, (double)(-5.16771281253917180   ), 28)
#define sin_c2  FLT_2_INT(int32_t, (double)(2.55016452658883970  ), 28)
#define sin_c3  FLT_2_INT(int32_t, (double)(-0.59926434897149095   ), 28)
#define sin_c4  FLT_2_INT(int32_t, (double)(0.08210311455977050  ), 28)
#define sin_c5  FLT_2_INT(int32_t, (double)(-0.00711066868715629   ), 28)


/**
 * Q31 cosine
 *
 * \param[in] - in [-1.0, 1.0) mapped to [-pi, pi)
 * \result result of cosine
 */

q31_t _DSPLIB_NOXYCC dsp_sin_q31(q31_t in)
{
    q31_t x2, out;
    q31_t arg = in;

    if ((arg == (int32_t)0x80000000) || (arg == (int32_t)0x7fffffff) || (arg == 0)) //0, pi, -pi
        return 0;
    if (arg == (int32_t)0x40000000) //pi/2
        return 0x7fffffff;
    if (arg == (int32_t)0xc0000000)//-pi/2
        return 0x80000000; 

    if (arg < 0)
        arg = -arg;
    if (arg > dsp_q31_cast_rnd_f32((double) 0.5 ))
        arg = -(dsp_q31_cast_rnd_f32((double) -1.0 ) + arg);

    x2 = dspfx_mpy_rnd_q31(arg, arg);

    out = sin_c5;
    out = dspfx_mpy_rnd_q31(out, x2); 
    out += sin_c4;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += sin_c3;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += sin_c2;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += sin_c1;
    out = dspfx_mpy_rnd_q31(out, 2*x2);
    out += sin_c0;
    out = dspfx_mpy_rnd_q31(out, 2*arg);
    out = dspfx_asl_q31(out,1);

    if (in < 0)
        out = -out;

    return out;
}

#endif //!defined(EXCLUDE_DSP_SIN_Q31)
