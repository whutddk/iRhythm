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

#if !defined(EXCLUDE_DSP_SIN_Q15)
#pragma Code("dsplib_dsp_sin_q15")

#define sin_c0  FLT_2_INT(int32_t, (double)(3.141584759327087   ), 29)
#define sin_c1  FLT_2_INT(int32_t, (double)(-5.167248127890854    ), 28)
#define sin_c2  FLT_2_INT(int32_t, (double)(2.542875725936100   ), 28)
#define sin_c3  FLT_2_INT(int32_t, (double)(-0.557160072356705    ), 28)

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif


/**
 * Q15 sine
 *
 * \param[in] - in [-1.0, 1.0) mapped to [-pi, pi)
 * \result result of sine
 */

q15_t _DSPLIB_NOXYCC dsp_sin_q15(q15_t in)
{
    q31_t x2, out;
    q31_t arg = in << 16;

    if ((arg == 0x80000000) || (arg == 0)) // 0, -pi
        return 0;
    //if (arg == 0x7fff0000) 
    //    return 0x0003;
    if (arg == 0x40000000) //pi/2
         return  (int16_t)0x7fff;
    if (arg == 0xc0000000) //-pi/2
         return  (int16_t)0x8000; 

    if (arg < 0)
        arg = -arg;
    if (arg > dsp_q31_cast_rnd_f32((double) 0.5 ))
        arg = -(dsp_q31_cast_rnd_f32((double) -1.0 ) + arg);

    x2 = dspfx_mpy_rnd_q31(arg, arg);

    out = sin_c3;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += sin_c2;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += sin_c1;
    out = dspfx_mpy_rnd_q31(out, 2*x2);
    out += sin_c0;
    out = dspfx_mpy_rnd_q31(out, arg);

    out >>= (16 - 2);
    if (in < 0)
        out = -out;

    return (out);
}

#endif //!defined(EXCLUDE_DSP_SIN_Q15)

#if !defined(EXCLUDE_DSP_COS_Q15)
#pragma Code("dsplib_dsp_cos_q15")

//q28
#define cos_c0  FLT_2_INT(int32_t, (double)(0.999995275222959 ),  31)
#define cos_c1  FLT_2_INT(int32_t, (double)(-4.934119591583158 ), 28)
#define cos_c2  FLT_2_INT(int32_t, (double)(4.043608547964373 ),  28)
#define cos_c3  FLT_2_INT(int32_t, (double)(-1.229300165701204 ), 28)


/**
 * Q15 cosine
 *
 * \param[in] - in [-1.0, 1.0) mapped to [-pi, pi)
 * \result result of cosine
 */

q15_t _DSPLIB_NOXYCC dsp_cos_q15(q15_t in)
{
    q31_t x2, out, sign = 0;
    q31_t arg = in << 16;

    if ((arg == 0x80000000) || (arg == 0x7fff0000)) //pi,-pi
         return  (int16_t)0x8000;
    if (arg == 0) //0, pi, -pi
         return  (int16_t)0x7fff;
    if ((arg == 0x40000000) || (arg == 0xc0000000)) //pi/2,-pi/2
         return 0; 

    if (arg < 0)
        arg = -arg;
    if (arg > dsp_q31_cast_rnd_f32((double) 0.5 )) {
        arg = -(dsp_q31_cast_rnd_f32((double) -1.0 ) + arg);
        sign = 1;
    }

    //arg - q15, c[i] - q12, out - q12, return - q15
    x2 = dspfx_mpy_rnd_q31(arg, arg);

    out = cos_c3;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += cos_c2;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += cos_c1;
    out = dspfx_mpy_rnd_q31(out, 4*x2);
    out = dspfx_asl_q31(out,1);
    out += cos_c0;
    out = out >> 16;

    if (sign)
        out = -out; 

    return out;
}

#endif //!defined(EXCLUDE_DSP_COS_Q15)
