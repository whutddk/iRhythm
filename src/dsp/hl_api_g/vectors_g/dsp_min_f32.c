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

#if !defined(EXCLUDE_DSP_MIN_F32)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_min_f32")
/**
 * Finds the minimum element of a float-point vector.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] pResult
 * \param[out] pIndex
 */

void _DSPLIB_XYCC dsp_min_f32(const PTR(f32_t) __restrict pSrc, uint32_t nSamples, f32_t *pResult, uint32_t *pIndex)
{
    uint32_t sample, sample_min = 0;
    float    outval = pSrc[0];
    
    for (sample = 1; sample < nSamples; sample++) {
        if(outval > pSrc[sample]) {
            outval = pSrc[sample];
            sample_min = sample;
        }
    }
    *pResult = outval;
    *pIndex = sample_min;
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_MIN_F32)
