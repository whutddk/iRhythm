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
#if defined(_ARC_AS210_mac16)

// Produce instance with: MAC16x16; pIn0 - X; 

#pragma Code("dsplib_dsp_imdct_postprocess_q15_XtoY")

/**
 *  iMDCT postprocessing function for q15 data type
 *
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param length - transform size
 */

void dsp_imdct_postprocess_q15_XtoY(uint32_t offs_input, uint32_t offs_output, uint32_t length)  
{
    int32_t i;
    int32_t L = length;
    int32_t N = length >> 1;

    _set_ax0(offs_input + N);
    _set_mx00(AM_16BIT|1);
    _set_ax1(offs_input);
    _set_mx10(AM_16BIT|1);
    _set_ay0(offs_output);
    _set_my00(AM_16BIT|1);
    _set_ay1(offs_output + L + N);
    _set_my10(AM_16BIT|1);

    for (i = 0; i < N; i++)
    {
        _set_y0_u0( _x0_u0());
        _set_y1_u0( dspfx_neg_q31(_x1_u0()));
    }    


    _set_ay0(offs_output);
    _set_my00(AM_16BIT|1);
    _set_ay1(offs_output + L + N);
    _set_my10(AM_16BIT|1);
    _set_ay2(offs_output + L - 1);
    _set_my20(AM_16BIT|AM_OFFS(-1));
    _set_ay3(offs_output + L + N - 1);
    _set_my30(AM_16BIT|AM_OFFS(-1));

    for (i = 0; i < N; i++)
    {
        _set_y2_u0( dspfx_neg_q31(_y0_u0()));
        _set_y3_u0( _y1_u0());
    }    
}

// Produce instance with: MAC16x16; pIn0 - Y; 

#pragma Code("dsplib_dsp_imdct_postprocess_q15_YtoX")

/**
 *  iMDCT postprocessing function for q15 data type
 *
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param length - transform size
 */

void dsp_imdct_postprocess_q15_YtoX(uint32_t offs_input, uint32_t offs_output, uint32_t length)  
{
    int32_t i;
    int32_t L = length;
    int32_t N = length >> 1;

    _set_ay0(offs_input + N);
    _set_my00(AM_16BIT|1);
    _set_ay1(offs_input);
    _set_my10(AM_16BIT|1);
    _set_ax0(offs_output);
    _set_mx00(AM_16BIT|1);
    _set_ax1(offs_output + L + N);
    _set_mx10(AM_16BIT|1);

    for (i = 0; i < N; i++)
    {
        _set_x0_u0( _y0_u0());
        _set_x1_u0( dspfx_neg_q31(_y1_u0()));
    }    


    _set_ax0(offs_output);
    _set_mx00(AM_16BIT|1);
    _set_ax1(offs_output + L + N);
    _set_mx10(AM_16BIT|1);
    _set_ax2(offs_output + L - 1);
    _set_mx20(AM_16BIT|AM_OFFS(-1));
    _set_ax3(offs_output + L + N - 1);
    _set_mx30(AM_16BIT|AM_OFFS(-1));

    for (i = 0; i < N; i++)
    {
        _set_x2_u0( dspfx_neg_q31(_x0_u0()));
        _set_x3_u0( _x1_u0());
    }    
}


#endif
