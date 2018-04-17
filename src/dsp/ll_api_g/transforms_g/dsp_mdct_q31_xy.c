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

#if defined(_ARC_AS210_mac32)

// Produce instance with: MAC32x32; pIn0 - X; 

#pragma Code("dsplib_dsp_mdct_preprocess_q31_XtoY")

/**
 *  MDCT preprocessing function for q31 data type
 *
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param length - transform size
 */

void dsp_mdct_preprocess_q31_XtoY(uint32_t offs_input, uint32_t offs_output, uint32_t length)  
{
    int32_t i;
    int32_t N = length >> 2;

    _set_ax0(offs_input + 3*N);
    _set_mx00(AM_OFFS(-3*N));
    _set_mx01(3*N + 1);
    _set_ax1(offs_input + 3*N-1);
    _set_mx10(AM_OFFS(-N));
    _set_mx11(N - 1);
    _set_ay0(offs_output);
    _set_my00(N);
    _set_my01(AM_OFFS(1-N));

    for (i = 0; i < N; i++)
    {
        _set_y0_u0( dspfx_neg_q31(dspfx_add_q31(_x0_u0(), _x1_u0())));
        _set_y0_u1(         dspfx_sub_q31(_x0_u1(), _x1_u1()));
    }    
}

// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_mdct_preprocess_q31_YtoX")

/**
 *  MDCT preprocessing function for q31 data type
 *
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param length - transform size
 */

void dsp_mdct_preprocess_q31_YtoX(uint32_t offs_input, uint32_t offs_output, uint32_t length)  
{
    int32_t i;
    int32_t N = length >> 2;

    _set_ay0(offs_input + 3*N);
    _set_my00(AM_OFFS(-3*N));
    _set_my01(3*N + 1);
    _set_ay1(offs_input + 3*N-1);
    _set_my10(AM_OFFS(-N));
    _set_my11(N - 1);
    _set_ax0(offs_output);
    _set_mx00(N);
    _set_mx01(AM_OFFS(1-N));

    for (i = 0; i < N; i++)
    {
        _set_x0_u0( dspfx_neg_q31(dspfx_add_q31(_y0_u0(), _y1_u0())));
        _set_x0_u1(         dspfx_sub_q31(_y0_u1(), _y1_u1()));
    }    
}


#endif
