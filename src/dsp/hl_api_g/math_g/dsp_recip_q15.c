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

#if !defined(EXCLUDE_DSP_RECIP_Q15)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_recip_q15")

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif

/**
 * Q15 reciprocal function
 *
 * \param[in]  x in int16
 * \result result of reciprocal in q15
 */

q15_t _DSPLIB_NOXYCC dsp_recip_q15(int16_t x)
{
    q31_t A,B,C,D,E,F;
    int i;
    if (!x || (x == 1))
        x = 0x7fff;
    else {
        B = dspfx_norm_q15(x) + 16;
        F = x << B;          
        E = B;                
        B = (F >> 3)<<1;

        for (i = 0; i < 6; i++)
        {
            C = B << 1;     
            A = dspfx_mpyh_rnd_q31(C,  C);
            D = dspfx_mpyh_rnd_q31(A,  F); 
            B = C - D;
        }
        E = (16 + 16 + 14 - E -1);

        x = B >> E;
    }
    return x;
}

#endif //!defined(EXCLUDE_DSP_RECIP_Q15)
