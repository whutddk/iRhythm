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

#pragma Code("dsplib_dsp_mult_q31_XXtoY_MAC32x32")

/**
 *  Q31 element-by-element multiplication of two vectors.
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_mult_q31_XXtoY_MAC32x32(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples) 
{
    int32_t sample;

    _set_ax0(offs_SrcA);
    _set_mx00(1);
    _set_ax1(offs_SrcB);
    _set_mx10(1);
    _set_ay0(offs_Dst);
    _set_my00(1);
    for (sample = 0; sample < (int32_t)nSamples>>1; sample++)
    {
                    _mulflw(_x0_nu(), _x1_nu());
        _set_y0_u0( _machflw(_x0_u0(), _x1_u0()));
                    _mulflw(_x0_nu(), _x1_nu());
        _set_y0_u0( _machflw(_x0_u0(), _x1_u0()));
    }
    if (nSamples & 1)
                    _mulflw(_x0_nu(), _x1_nu());
        _set_y0_u0( _machflw(_x0_u0(), _x1_u0()));
}
// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_mult_q31_YYtoX_MAC32x32")

/**
 *  Q31 element-by-element multiplication of two vectors.
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_mult_q31_YYtoX_MAC32x32(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples) 
{
    int32_t sample;

    _set_ay0(offs_SrcA);
    _set_my00(1);
    _set_ay1(offs_SrcB);
    _set_my10(1);
    _set_ax0(offs_Dst);
    _set_mx00(1);
    for (sample = 0; sample < (int32_t)nSamples>>1; sample++)
    {
                    _mulflw(_y0_nu(), _y1_nu());
        _set_x0_u0( _machflw(_y0_u0(), _y1_u0()));
                    _mulflw(_y0_nu(), _y1_nu());
        _set_x0_u0( _machflw(_y0_u0(), _y1_u0()));
    }
    if (nSamples & 1)
                    _mulflw(_y0_nu(), _y1_nu());
        _set_x0_u0( _machflw(_y0_u0(), _y1_u0()));
}

#endif
