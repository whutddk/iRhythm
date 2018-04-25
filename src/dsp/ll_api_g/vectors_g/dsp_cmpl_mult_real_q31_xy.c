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

// Produce instance with: MAC32x32; pSrcComplex - X; 

#pragma Code("dsplib_dsp_cmpl_mult_real_q31_XXtoY_MAC32x32")

/**
 * Computes the Q31 complex multiplication of complex and real numbers stored in a vector respectively
 * The return result is in 1.31 format.
 *
 * \param[in] pSrcCmplx
 * \param[in] pSrcReal
 * \param[out] pCmplxDst
 * \param nSamples - number of samples to be processed
 */

void dsp_cmpl_mult_real_q31_XXtoY_MAC32x32(uint32_t pSrcComplex, uint32_t pSrcReal, uint32_t offs_Dst, uint32_t nSamples)
{
    int32_t sample;

    _set_ax0(pSrcComplex);
    _set_mx00(1);
    _set_ax1(pSrcReal);
    _set_mx10(1);
    _set_ay0(offs_Dst);
    _set_my00(1);

    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {        
                    _mulflw(_x0_nu(), _x1_nu());
        _set_y0_u0(_machflw(_x0_u0(), _x1_nu()));
                    _mulflw(_x0_nu(), _x1_nu());
        _set_y0_u0(_machflw(_x0_u0(), _x1_u0()));
    }
};

// Produce instance with: MAC32x32; pSrcComplex - Y; 

#pragma Code("dsplib_dsp_cmpl_mult_real_q31_YYtoX_MAC32x32")

/**
 * Computes the Q31 complex multiplication of complex and real numbers stored in a vector respectively
 * The return result is in 1.31 format.
 *
 * \param[in] pSrcCmplx
 * \param[in] pSrcReal
 * \param[out] pCmplxDst
 * \param nSamples - number of samples to be processed
 */

void dsp_cmpl_mult_real_q31_YYtoX_MAC32x32(uint32_t pSrcComplex, uint32_t pSrcReal, uint32_t offs_Dst, uint32_t nSamples)
{
    int32_t sample;

    _set_ay0(pSrcComplex);
    _set_my00(1);
    _set_ay1(pSrcReal);
    _set_my10(1);
    _set_ax0(offs_Dst);
    _set_mx00(1);

    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {        
                    _mulflw(_y0_nu(), _y1_nu());
        _set_x0_u0(_machflw(_y0_u0(), _y1_nu()));
                    _mulflw(_y0_nu(), _y1_nu());
        _set_x0_u0(_machflw(_y0_u0(), _y1_u0()));
    }
};


#endif
