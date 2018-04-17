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

#if !defined(EXCLUDE_DSP_TAN_Q31)

#if 0
#define c0 FLT_2_INT(int32_t, (double)(9.99999988146511e-001), 30)
#define c1 FLT_2_INT(int32_t, (double)(3.33334242291399e-001), 30)
#define c2 FLT_2_INT(int32_t, (double)(1.33313218946920e-001), 30)
#define c3 FLT_2_INT(int32_t, (double)(5.41658231075713e-002), 30)
#define c4 FLT_2_INT(int32_t, (double)(2.08550570648094e-002), 30)
#define c5 FLT_2_INT(int32_t, (double)(1.17546013647218e-002), 30)
#define c6 FLT_2_INT(int32_t, (double)(-9.13684040890507e-004), 30)
#define c7 FLT_2_INT(int32_t, (double)(4.81332807665819e-003), 30)

#define c0 FLT_2_INT(int32_t, (double)( 0.785398163397448),31)
#define c1 FLT_2_INT(int32_t, (double)( 0.161490937186395),31)
#define c2 FLT_2_INT(int32_t, (double)( 0.039848157043813),31)
#define c3 FLT_2_INT(int32_t, (double)( 0.009934279569313),31)
#define c4 FLT_2_INT(int32_t, (double)( 0.002543743465495),31)
#define c5 FLT_2_INT(int32_t, (double)( 0.000495981115322),31)
#define c6 FLT_2_INT(int32_t, (double)( 0.000315921911991),31)
#define c7 FLT_2_INT(int32_t, (double)(-0.000075159017272),31)
#define c8 FLT_2_INT(int32_t, (double)( 0.000047975519192),31)
#endif

#define c0 FLT_2_INT(int32_t, (double)(0.785398167260568),31)
#define c1 FLT_2_INT(int32_t, (double)(0.161490831102301),31)
#define c2 FLT_2_INT(int32_t, (double)(0.039849160763466),31)
#define c3 FLT_2_INT(int32_t, (double)(0.009929683492111),31)
#define c4 FLT_2_INT(int32_t, (double)(0.002555336572206),31)
#define c5 FLT_2_INT(int32_t, (double)(0.000479091960888),31)
#define c6 FLT_2_INT(int32_t, (double)(0.000329994231137),31)
#define c7 FLT_2_INT(int32_t, (double)(-0.000081332625514),31)
#define c8 FLT_2_INT(int32_t, (double)(0.000049067417381),31)




#pragma Code("dsplib_dsp_tan_q31")

#define DSP_PI_Q29 FLT_2_INT(int32_t, (double)(3.14159265359), 29)

/**
 * Q31 tangent function
 *
 * \param[in] - in [-1.0, 1.0) mapped to [-pi, pi)
 *
 *               Data range       |   Real value mapping   |   Output value
 *            --------------------+------------------------+-----------------
 *               [-1.0 .. -0.75)  |    [-pi .. -3pi/4)     |      TAN
 *               [-0.75 .. -0.5)  |    [-3pi/4 .. -pi/2)   |     +1.0
 *               [-0.5 .. -0.25)  |    [-pi/2 .. -pi/4)    |     -1.0
 *               [-0.25 .. 0.25)  |    [-pi/4 .. pi/4)     |      TAN
 *               [0.25 .. 0.5]    |    [pi/4 .. pi/2]      |     +1.0
 *               (0.5 .. 0.75)    |    (pi/2 .. 3pi/4)     |     -1.0
 *               [0.75 .. 1.0)    |    [3pi/4 .. pi)       |      TAN
 *
 * \result result of tangent function
 */

q31_t _DSPLIB_NOXYCC dsp_tan_q31(q31_t input)
{
    q31_t x2, out;
    q31_t arg;

    if (dspfx_abs_q31(input) >= 0x40000000) 
	    input = (q31_t)((uint32_t)input + (uint32_t)0x80000000);
    if (input >= (int32_t) 0x20000000) 
	    return 0x7fffffff;
    if (input <= (int32_t)-0x20000000) 
	    return 0x80000000;
    
    arg = dspfx_asl_q31(input,2);
    x2 = dspfx_mpy_rnd_q31(arg, arg);
    out = c8;
    out = dspfx_mpy_rnd_q31(out, x2); 
    out += c7;
    out = dspfx_mpy_rnd_q31(out, x2); 
    out += c6;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += c5;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += c4;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += c3;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += c2;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += c1;
    out = dspfx_mpy_rnd_q31(out, x2);
    out += c0;
    out = dspfx_mpy_rnd_q31(out, arg);
    return out;

}

#endif //!defined(EXCLUDE_DSP_TAN_Q31)
