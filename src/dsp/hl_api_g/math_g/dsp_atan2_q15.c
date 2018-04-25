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

#if !defined(EXCLUDE_DSP_ATAN2_Q15)
#pragma Code("dsplib_dsp_atan2_q15")

#define PI_div4         FLT_2_INT(int16_t,(double)  1.0/4.0,15) //pi/4
#define negPI_div4      FLT_2_INT(int16_t,(double) -1.0/4.0,15) //-pi/4
#define PI_div2         FLT_2_INT(int16_t,(double)  1.0/2.0,15) //pi/2
#define negPI_div2      FLT_2_INT(int16_t,(double) -1.0/2.0,15) //-pi/2
#define PI_mul3div4     FLT_2_INT(int16_t,(double)  3.0/4.0,15) //3pi/4
#define PI              FLT_2_INT(int16_t,(double)  1.0,    15) //pi
#define negPI_mul3div4  FLT_2_INT(int16_t,(double) -3.0/4.0,15) //-3pi/4
#define negPI           FLT_2_INT(int16_t,(double) -1.0,   15) //pi

#define ONEdivPI  FLT_2_INT(int16_t,(double)1/(double)M_PI,15) 

/**
 * Q15 arctangent of 2 arguments. 
 *
 * \param y - cos part (-1,1)
 * \param x - sin part (-1;1)
 * \result value between [-pi;pi) mapped to [-1;1) 
 */

q15_t _DSPLIB_NOXYCC dsp_atan2_q15(q15_t y, q15_t x)
{
    q15_t res, arg;
 
    // checks for boundary cases
    if (x == 0)
    {
        if (y>0)
            return (PI_div2);
        if (y<0)
            return (negPI_div2);

        return (q15_t)0xffff;//undefined
    }

    if (dspfx_abs_q15(x)==dspfx_abs_q15(y))
    {
        if ((y>0)&&(x>0))
            return (PI_div4);
        if ((y<0)&&(x>0))
            return (negPI_div4);
        if ((y>0)&&(x<0))
            return (PI_mul3div4);
        if ((y<0)&&(x<0))
            return (negPI_mul3div4);
    }

    INIT_MACMODE();
    if (dspfx_abs_q15(y) < dspfx_abs_q15(x)) 
    {
        // |y|<|x|, x>0 -> atan(y/x) -> ([-pi/4;pi/4]) /4
        // |y|<|x|, x<0 -> atan(y/x) -> (pi + [-pi/4;pi/4]) /4
        arg = dsp_div_q15(y,x);
        res = dsp_atan_q15(arg);

        if (x<0) {
            if (y>=0) // [3pi/4; pi]
                res = res + PI; 
            else // [-3pi/4;-pi]
                res = res - PI; 
        }
    }
    else // (dspfx_abs_q15(y) > dspfx_abs_q15(x))
    {
        // |y|>|x|, y>0 -> arcctg(x/y) -> pi/2-atan(x/y) -> ([pi/4; 3pi/4])/4
        // |y|>|x|, y<0 -> arcctg(x/y) -> pi/2-atan(x/y)-pi -> ([-pi/4; -3pi/4])/4
        arg = dsp_div_q15(x,y);
        res = dsp_atan_q15(arg);

        if (y>0)                                 // [pi/4; 3pi/4]
            res = PI_div2 - res;

        if (y<0)                                 // [-pi/4;-3pi/4]
            res = negPI_div2 -res;
    }

    return res;
}

#endif //!defined(EXCLUDE_DSP_ATAN2_Q15)
