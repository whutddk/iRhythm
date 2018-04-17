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

#if !defined(EXCLUDE_DSP_OFFSET_F32)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_offset_f32")
/**
 * Adds a constant offset to each element of a vector.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param offset - additive constant
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_offset_f32(const PTR(f32_t) __restrict pSrc, f32_t offset, PTR(f32_t) __restrict pDst, uint32_t nSamples)
{
    uint32_t sample;
    for (sample = 0; sample < nSamples; sample++)
    {
        *pDst++ = ADD_F32(*pSrc++, offset);
    }
}

#endif //!defined(EXCLUDE_DSP_OFFSET_F32)
