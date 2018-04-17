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

#if !defined(EXCLUDE_DSP_TAN_Q15)

#define c0 FLT_2_INT(int32_t, (double)(1.00000000000),30)
#define c1 FLT_2_INT(int32_t, (double)(0.33303543911),30)
#define c2 FLT_2_INT(int32_t, (double)(0.13709191056),30)
#define c3 FLT_2_INT(int32_t, (double)(0.03873082549),30)
#define c4 FLT_2_INT(int32_t, (double)(0.04523295575),30)

#pragma Code("dsplib_dsp_tan_q15")

#define DSP_PI_Q29 FLT_2_INT(int32_t, (double)(3.14159265359), 29)

/**
 * Q15 tangent function
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

q15_t _DSPLIB_NOXYCC dsp_tan_q15(q15_t input)
{
    q31_t x2, out;
    q31_t arg = dspfx_asl_q31(input, 16);

    if (dspfx_abs_q31(arg) >= 0x40000000) 
	    arg = (q31_t)((uint32_t)arg + (uint32_t)0x80000000);
    if (arg >= (int32_t) 0x20000000) 
	    return (q15_t)(uint16_t)0x7fff;
    if (arg <= (int32_t)-0x20000000) 
	    return (q15_t)(uint16_t)0x8000;
    
    arg = dspfx_mpy_rnd_q31(dspfx_asl_q31(arg,2), DSP_PI_Q29);
    x2 = dspfx_mpy_rnd_q31(arg, arg);
    out = c4;
    out = dspfx_mpy_rnd_q31(out, x2);
    out = dspfx_add_q31(out,c3);
    out = dspfx_mpy_rnd_q31(out, x2);
    out = dspfx_add_q31(out,c2);
    out = dspfx_mpy_rnd_q31(out, x2);
    out = dspfx_add_q31(out,c1);
    out = dspfx_mpy_rnd_q31(out, x2);
    out = dspfx_add_q31(out,c0);
    out = dspfx_mpy_rnd_q31(out, arg);
    return dspfx_asl_q31(out, -15);
}

#endif //!defined(EXCLUDE_DSP_TAN_Q15)
