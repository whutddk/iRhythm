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

#pragma Code("dsplib_dsp_cmpl_dot_prod_q31_XX_MAC32x32")

/**
 *  Dot product of two Q31 complex vectors. Output data format is 16.48
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] realResult
 * \param[out] imagResult
 */

void dsp_cmpl_dot_prod_q31_XX_MAC32x32(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t nSamples, int64_t *realResult, int64_t *imagResult)
{
    int32_t sample;        
    q63_t acc_re = 0;
    q63_t acc_im = 0;
    
    _set_ax0(offs_SrcA);
    _set_mx00(1);
    _set_mx01(AM_OFFS(-((int32_t)nSamples) * 2));
    _set_ax1(offs_SrcB);
    _set_mx10(1);
    _set_mx11(AM_OFFS(-((int32_t)nSamples) * 2));
    
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
        _mullw(_x0_nu(), _x1_nu());
        _machlw(_x0_u0(), _x1_u0());
        
        _maclw(_x0_nu(), _x1_nu());
        _machlw(_x0_u0(), _x1_u0());
        
        acc_re += (((int64_t)_get_ACC1()<<32)|_get_ACC2())>>14;
    }
    
    _add(_x0_u1(), _x1_u1());
    
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
        q31_t reA = _x0_u0();
        
        _mullw(_x1_nu(), _x0_nu());
        _machlw(_x1_u0(), _x0_u0());
        
        _maclw(dspfx_neg_q31(_x1_nu()), reA);
        _machlw(dspfx_neg_q31(_x1_u0()), reA);
        
        acc_im += (((int64_t)_get_ACC1()<<32)|_get_ACC2())>>14;
    }    
    
    *realResult = acc_re;
    *imagResult = acc_im;
}

// Produce instance with: MAC32x32; pInA - Y; 

#pragma Code("dsplib_dsp_cmpl_dot_prod_q31_YY_MAC32x32")

/**
 *  Dot product of two Q31 complex vectors. Output data format is 16.48
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] realResult
 * \param[out] imagResult
 */

void dsp_cmpl_dot_prod_q31_YY_MAC32x32(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t nSamples, int64_t *realResult, int64_t *imagResult)
{
    int32_t sample;        
    q63_t acc_re = 0;
    q63_t acc_im = 0;
    
    _set_ay0(offs_SrcA);
    _set_my00(1);
    _set_my01(AM_OFFS(-((int32_t)nSamples) * 2));
    _set_ay1(offs_SrcB);
    _set_my10(1);
    _set_my11(AM_OFFS(-((int32_t)nSamples) * 2));
    
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
        _mullw(_y0_nu(), _y1_nu());
        _machlw(_y0_u0(), _y1_u0());
        
        _maclw(_y0_nu(), _y1_nu());
        _machlw(_y0_u0(), _y1_u0());
        
        acc_re += (((int64_t)_get_ACC1()<<32)|_get_ACC2())>>14;
    }
    
    _add(_y0_u1(), _y1_u1());
    
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
        q31_t reA = _y0_u0();
        
        _mullw(_y1_nu(), _y0_nu());
        _machlw(_y1_u0(), _y0_u0());
        
        _maclw(dspfx_neg_q31(_y1_nu()), reA);
        _machlw(dspfx_neg_q31(_y1_u0()), reA);
        
        acc_im += (((int64_t)_get_ACC1()<<32)|_get_ACC2())>>14;
    }    
    
    *realResult = acc_re;
    *imagResult = acc_im;
}


#endif
