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

// Produce instance with: MAC16x16; pIn - X; 

#pragma Code("dsplib_dsp_cmpl_mag_q15_XtoY_MAC16x16")

/**
 * Computes the Q15 magnitude of complex numbers stored in a vector                      
 * The return result is in 1.14 format.
 *
 * \param[in] offs_Src - XY memory offset to input vector. Vector length: nSamples
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_cmpl_mag_q15_XtoY_MAC16x16(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples)
{
    int32_t sample;    
    
    _set_ax0(offs_Src);
    _set_mx00(AM_16BIT | 1);
    _set_ay0(offs_Dst);
    _set_my00(AM_16BIT | 1);
    _set_MACMODE((1UL<<15)+(1UL<<3));
    
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {        
        q31_t temp, root;
               _mulrdw(_x0_u0(), _x0_nu());                
        temp = _macrdw(_x0_u0(), _x0_nu());
        root = dsp_sqrt_q31(temp>>1);
        _set_y0_u0(root);
    }
};

// Produce instance with: MAC16x16; pIn - Y; 

#pragma Code("dsplib_dsp_cmpl_mag_q15_YtoX_MAC16x16")

/**
 * Computes the Q15 magnitude of complex numbers stored in a vector                      
 * The return result is in 1.14 format.
 *
 * \param[in] offs_Src - XY memory offset to input vector. Vector length: nSamples
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_cmpl_mag_q15_YtoX_MAC16x16(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples)
{
    int32_t sample;    
    
    _set_ay0(offs_Src);
    _set_my00(AM_16BIT | 1);
    _set_ax0(offs_Dst);
    _set_mx00(AM_16BIT | 1);
    _set_MACMODE((1UL<<15)+(1UL<<3));
    
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {        
        q31_t temp, root;
               _mulrdw(_y0_u0(), _y0_nu());                
        temp = _macrdw(_y0_u0(), _y0_nu());
        root = dsp_sqrt_q31(temp>>1);
        _set_x0_u0(root);
    }
};


#endif
