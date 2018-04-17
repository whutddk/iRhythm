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

// Produce instance with: DMAC16x16; pInA - X; 

#pragma Code("dsplib_dsp_cmpl_mult_cmpl_q15_XXtoY_DMAC16x16")

/**
 * Computes the Q15 complex multiplication of two complex numbers stored in a vector respectively
 * The return result is in 1.15 format.
 *
 * \param[in] offs_Src - XY memory offset to input vector. Vector length: nSamples
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_cmpl_mult_cmpl_q15_XXtoY_DMAC16x16(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t offs_Dst, uint32_t nSamples)
{
    int32_t sample;    
    
    _set_ax0(offs_SrcA>>1);
    _set_mx00(1);
    _set_ax1(offs_SrcB);
    _set_mx10(AM_16BIT|AM_DUAL|AM_OFFS(1));
    _set_ay0(offs_Dst>>1);
    _set_my00(1);
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
#ifdef _ARC_BE
        uint32_t C;
        uint32_t A = _ror(_x0_u0(), 16);
        // a.re*b.re | a.im*b*re
        _mulrdw(A, _x1_u0()); 
		//-a.im*b.im | a.re*b.im
        C = _cmacrdw(A, _x1_u0());
		_set_y0_u0(_ror(C, 16));
#else
        // a.re*b.re | a.im*b*re
        _mulrdw(_x0_nu(), _x1_u0()); 
		//-a.im*b.im | a.re*b.im
        _set_y0_u0(_cmacrdw(_x0_u0(), _x1_u0()));
#endif
    }
};

// Produce instance with: DMAC16x16; pInA - Y; 

#pragma Code("dsplib_dsp_cmpl_mult_cmpl_q15_YYtoX_DMAC16x16")

/**
 * Computes the Q15 complex multiplication of two complex numbers stored in a vector respectively
 * The return result is in 1.15 format.
 *
 * \param[in] offs_Src - XY memory offset to input vector. Vector length: nSamples
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_cmpl_mult_cmpl_q15_YYtoX_DMAC16x16(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t offs_Dst, uint32_t nSamples)
{
    int32_t sample;    
    
    _set_ay0(offs_SrcA>>1);
    _set_my00(1);
    _set_ay1(offs_SrcB);
    _set_my10(AM_16BIT|AM_DUAL|AM_OFFS(1));
    _set_ax0(offs_Dst>>1);
    _set_mx00(1);
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
#ifdef _ARC_BE
        uint32_t C;
        uint32_t A = _ror(_y0_u0(), 16);
        // a.re*b.re | a.im*b*re
        _mulrdw(A, _y1_u0()); 
		//-a.im*b.im | a.re*b.im
        C = _cmacrdw(A, _y1_u0());
		_set_x0_u0(_ror(C, 16));
#else
        // a.re*b.re | a.im*b*re
        _mulrdw(_y0_nu(), _y1_u0()); 
		//-a.im*b.im | a.re*b.im
        _set_x0_u0(_cmacrdw(_y0_u0(), _y1_u0()));
#endif
    }
};

#endif
