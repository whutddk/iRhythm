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

#pragma Code("dsplib_dsp_scale_q31_XtoY_MAC32x32")

/**
 * Q31 multiply a vector by a scalar value.
 * The algorithm used is:
 * offs_Dst[n] = (offs_Src[n] * scaleFract) << shift,   0 <= n < nSamples. 
 * The overall scale factor applied to the fixed-point data is:
 * scale = scaleFract * 2^shift.
 *
 * \param[in] offs_Src - XY memory offset to input vector. Vector length: nSamples
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nSamples
 * \param scaleFract
 * \param shift
 * \param nSamples - number of samples to be processed
 */

void dsp_scale_q31_XtoY_MAC32x32(int32_t offs_Src, q31_t scaleFract, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples) 
{
    int32_t sample;

    _set_ax0(offs_Src);
    _set_mx00(1);
    _set_ay0(offs_Dst);
    _set_my00(1);
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
        q31_t tmp;
               _mulflw(_x0_nu(), scaleFract);
        tmp = _machflw(_x0_u0(), scaleFract);
        _set_y0_u0( dspfx_asl_q31(tmp, shiftBits));
    }
}
// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_scale_q31_YtoX_MAC32x32")

/**
 * Q31 multiply a vector by a scalar value.
 * The algorithm used is:
 * offs_Dst[n] = (offs_Src[n] * scaleFract) << shift,   0 <= n < nSamples. 
 * The overall scale factor applied to the fixed-point data is:
 * scale = scaleFract * 2^shift.
 *
 * \param[in] offs_Src - XY memory offset to input vector. Vector length: nSamples
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nSamples
 * \param scaleFract
 * \param shift
 * \param nSamples - number of samples to be processed
 */

void dsp_scale_q31_YtoX_MAC32x32(int32_t offs_Src, q31_t scaleFract, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples) 
{
    int32_t sample;

    _set_ay0(offs_Src);
    _set_my00(1);
    _set_ax0(offs_Dst);
    _set_mx00(1);
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
        q31_t tmp;
               _mulflw(_y0_nu(), scaleFract);
        tmp = _machflw(_y0_u0(), scaleFract);
        _set_x0_u0( dspfx_asl_q31(tmp, shiftBits));
    }
}

#endif
