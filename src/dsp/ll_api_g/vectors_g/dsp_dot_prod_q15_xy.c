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

#pragma Code("dsplib_dsp_dot_prod_q15_XY_MAC32x32")

/**
 * Computes the Q15 dot product of two vectors.
 * The vectors are multiplied element-by-element and then summed. 
 * The return result is in 34.30 format.
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] result
 */

void dsp_dot_prod_q15_XY_MAC32x32(int32_t offs_SrcA, int32_t offs_SrcB, uint32_t nSamples, q63_t *result) 
{
    int32_t sample;


    _set_ax0(offs_SrcA);
    _set_mx00(AM_16BIT | 1);
    _set_ay0(offs_SrcB);
    _set_my00(AM_16BIT | 1);

    _mullw(0,0);
    for (sample = 0; sample < (int32_t)nSamples; sample++){
        int32_t op0 = _x0_u0()>>16;
        int32_t op1 = _y0_u0()>>16;
        _maclw(op0,  op1);
        _machlw(op0,  op1);
    }
    *result = ((int64_t)_get_ACC1() << 32) | ((uint32_t)_get_ACC2());
}

// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_dot_prod_q15_YX_MAC32x32")

/**
 * Computes the Q15 dot product of two vectors.
 * The vectors are multiplied element-by-element and then summed. 
 * The return result is in 34.30 format.
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] result
 */

void dsp_dot_prod_q15_YX_MAC32x32(int32_t offs_SrcA, int32_t offs_SrcB, uint32_t nSamples, q63_t *result) 
{
    int32_t sample;


    _set_ay0(offs_SrcA);
    _set_my00(AM_16BIT | 1);
    _set_ax0(offs_SrcB);
    _set_mx00(AM_16BIT | 1);

    _mullw(0,0);
    for (sample = 0; sample < (int32_t)nSamples; sample++){
        int32_t op0 = _y0_u0()>>16;
        int32_t op1 = _x0_u0()>>16;
        _maclw(op0,  op1);
        _machlw(op0,  op1);
    }
    *result = ((int64_t)_get_ACC1() << 32) | ((uint32_t)_get_ACC2());
}


#endif
