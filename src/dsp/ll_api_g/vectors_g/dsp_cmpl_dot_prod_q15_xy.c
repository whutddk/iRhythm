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

#pragma Code("dsplib_dsp_cmpl_dot_prod_q15_XX_DMAC16x16")

/**
 *  Dot product of two Q15 complex vectors. Output data format is 8.24
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] realResult
 * \param[out] imagResult
 */

void dsp_cmpl_dot_prod_q15_XX_DMAC16x16(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t nSamples, int32_t *realResult, int32_t *imagResult)
{
    int32_t sample;
    q31_t acc_re = 0;
    q31_t acc_im = 0;
    
    _set_ax0(offs_SrcA>>1);
    _set_mx00(1);
    _set_ax1(offs_SrcB);
    _set_mx10(AM_16BIT|AM_DUAL|1);
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    _mulrdw(0,0);
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {        
        int32_t imAreA = _ror(_x0_u0(), 16);
         _macrdw(imAreA, _x1_u0());
        _cmacrdw(imAreA, _x1_u0());
    }

    {
        int32_t xmac0 = _lr(AUX_XMAC0);
        uint32_t xmac1 = _lr(AUX_XMAC1);
        uint32_t xmac2 = _lr(AUX_XMAC2);
        *realResult =  (((xmac0 >> 16) & 0x00ff) << 25) | (xmac1 >> 7);
        *imagResult =  (( xmac0 & 0x00ff) << 25) | (xmac2 >> 7);
    }
}

// Produce instance with: DMAC16x16; pInA - Y; 

#pragma Code("dsplib_dsp_cmpl_dot_prod_q15_YY_DMAC16x16")

/**
 *  Dot product of two Q15 complex vectors. Output data format is 8.24
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] realResult
 * \param[out] imagResult
 */

void dsp_cmpl_dot_prod_q15_YY_DMAC16x16(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t nSamples, int32_t *realResult, int32_t *imagResult)
{
    int32_t sample;
    q31_t acc_re = 0;
    q31_t acc_im = 0;
    
    _set_ay0(offs_SrcA>>1);
    _set_my00(1);
    _set_ay1(offs_SrcB);
    _set_my10(AM_16BIT|AM_DUAL|1);
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    _mulrdw(0,0);
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {        
        int32_t imAreA = _ror(_y0_u0(), 16);
         _macrdw(imAreA, _y1_u0());
        _cmacrdw(imAreA, _y1_u0());
    }

    {
        int32_t xmac0 = _lr(AUX_XMAC0);
        uint32_t xmac1 = _lr(AUX_XMAC1);
        uint32_t xmac2 = _lr(AUX_XMAC2);
        *realResult =  (((xmac0 >> 16) & 0x00ff) << 25) | (xmac1 >> 7);
        *imagResult =  (( xmac0 & 0x00ff) << 25) | (xmac2 >> 7);
    }
}


#endif
