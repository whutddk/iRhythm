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

// Produce instance with: MAC32x32; pInA - X; 

#pragma Code("dsplib_dsp_cmpl_mult_cmpl_q31_XXtoY_MAC32x32")

/**
 * Computes the Q31 complex multiplication of two complex numbers stored in a vector respectively
 * The return result is in 1.31 format.
 *
 * \param[in] offs_Src - XY memory offset to input vector. Vector length: nSamples
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_cmpl_mult_cmpl_q31_XXtoY_MAC32x32(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t offs_Dst, uint32_t nSamples)
{
    int32_t sample;    
    
    _set_ax0(offs_SrcA);
    _set_mx00(1);
    _set_mx01(AM_OFFS(-1));
    _set_ax1(offs_SrcB);
    _set_mx10(1);
    _set_ay0(offs_Dst);
    _set_my00(1);
    
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
        q31_t bRe = _x1_u0();
        
         _mulflw(_x0_nu(), bRe);
        _machflw(_x0_u0(), bRe);        
                    _macflw(_x0_nu(),dspfx_neg_q31(_x1_nu()));
        _set_y0_u0(_machflw(_x0_u1(),dspfx_neg_q31(_x1_nu())));
        
         _mulflw(_x0_nu(), _x1_nu());
        _machflw(_x0_u0(), _x1_u0());
                    _macflw(bRe, _x0_nu());
        _set_y0_u0(_machflw(bRe, _x0_u0()));
    }
};

// Produce instance with: MAC32x32; pInA - Y; 

#pragma Code("dsplib_dsp_cmpl_mult_cmpl_q31_YYtoX_MAC32x32")

/**
 * Computes the Q31 complex multiplication of two complex numbers stored in a vector respectively
 * The return result is in 1.31 format.
 *
 * \param[in] offs_Src - XY memory offset to input vector. Vector length: nSamples
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_cmpl_mult_cmpl_q31_YYtoX_MAC32x32(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t offs_Dst, uint32_t nSamples)
{
    int32_t sample;    
    
    _set_ay0(offs_SrcA);
    _set_my00(1);
    _set_my01(AM_OFFS(-1));
    _set_ay1(offs_SrcB);
    _set_my10(1);
    _set_ax0(offs_Dst);
    _set_mx00(1);
    
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
        q31_t bRe = _y1_u0();
        
         _mulflw(_y0_nu(), bRe);
        _machflw(_y0_u0(), bRe);        
                    _macflw(_y0_nu(),dspfx_neg_q31(_y1_nu()));
        _set_x0_u0(_machflw(_y0_u1(),dspfx_neg_q31(_y1_nu())));
        
         _mulflw(_y0_nu(), _y1_nu());
        _machflw(_y0_u0(), _y1_u0());
                    _macflw(bRe, _y0_nu());
        _set_x0_u0(_machflw(bRe, _y0_u0()));
    }
};


#endif
