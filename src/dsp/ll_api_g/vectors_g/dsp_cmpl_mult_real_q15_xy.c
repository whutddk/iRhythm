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

// Produce instance with: DMAC16x16; pSrcComplex - X; 

#pragma Code("dsplib_dsp_cmpl_mult_real_q15_XXtoY_DMAC16x16")

/**
 * Computes the Q15 complex multiplication of complex and real numbers stored in a vector respectively
 * The return result is in 1.31 format.
 *
 * \param[in] pSrcCmplx
 * \param[in] pSrcReal
 * \param[out] pCmplxDst
 * \param nSamples - number of samples to be processed
 */

void dsp_cmpl_mult_real_q15_XXtoY_DMAC16x16(uint32_t pSrcComplex, uint32_t pSrcReal, uint32_t offs_Dst, uint32_t nSamples)
{
    int32_t sample;

    _set_ax0(pSrcComplex>>1);
    _set_mx00(1);
    _set_ax1(pSrcReal);
    _set_mx10(AM_16BIT|AM_DUAL|1);
    _set_ay0(offs_Dst>>1);
    _set_my00(1);
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));

    for (sample = 0; sample < (int32_t)nSamples>>1; sample++)
    {   // out.re | out.im = a.re*b | a.im*b 
        _set_y0_u0(_mulrdw(_x0_u0(), _x1_u0()));
        _set_y0_u0(_mulrdw(_x0_u0(), _x1_u0()));
    }
	if (nSamples & 1) _set_y0_u0(_mulrdw(_x0_u0(), _x1_u0()));
};

// Produce instance with: DMAC16x16; pSrcComplex - Y; 

#pragma Code("dsplib_dsp_cmpl_mult_real_q15_YYtoX_DMAC16x16")

/**
 * Computes the Q15 complex multiplication of complex and real numbers stored in a vector respectively
 * The return result is in 1.31 format.
 *
 * \param[in] pSrcCmplx
 * \param[in] pSrcReal
 * \param[out] pCmplxDst
 * \param nSamples - number of samples to be processed
 */

void dsp_cmpl_mult_real_q15_YYtoX_DMAC16x16(uint32_t pSrcComplex, uint32_t pSrcReal, uint32_t offs_Dst, uint32_t nSamples)
{
    int32_t sample;

    _set_ay0(pSrcComplex>>1);
    _set_my00(1);
    _set_ay1(pSrcReal);
    _set_my10(AM_16BIT|AM_DUAL|1);
    _set_ax0(offs_Dst>>1);
    _set_mx00(1);
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));

    for (sample = 0; sample < (int32_t)nSamples>>1; sample++)
    {   // out.re | out.im = a.re*b | a.im*b 
        _set_x0_u0(_mulrdw(_y0_u0(), _y1_u0()));
        _set_x0_u0(_mulrdw(_y0_u0(), _y1_u0()));
    }
	if (nSamples & 1) _set_x0_u0(_mulrdw(_y0_u0(), _y1_u0()));
};


#endif
