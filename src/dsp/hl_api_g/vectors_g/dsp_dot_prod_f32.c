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

#if !defined(EXCLUDE_DSP_DOT_PROD_F32)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_dot_prod_f32")

/**
 * Computes the float-point dot product of two vectors.
 * The vectors are multiplied element-by-element and then summed.
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nSamples
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] result - pointer to output result
 */

void _DSPLIB_XYCC dsp_dot_prod_f32(const PTR(f32_t) __restrict pSrcA, const PTR(f32_t) __restrict pSrcB, uint32_t nSamples, f32_t* result)
{
    uint32_t sample;
    float acc = 0.0;
    for (sample = 0; sample < nSamples; sample++){
         acc += pSrcA[sample] * pSrcB[sample];
    }
    *result = acc;
}

#endif //!defined(EXCLUDE_DSP_DOT_PROD_F32)
