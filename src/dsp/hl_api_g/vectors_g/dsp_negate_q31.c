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

#if !defined(EXCLUDE_DSP_NEGATE_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_nomac)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

#pragma Code("dsplib_dsp_negate_q31")
// primitives based version of vector negate
static _Inline void _DSPLIB_XYCC risc_dsp_negate_q31(const PTR(q31_t) __restrict pSrc,  PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
    int32_t sample;
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
        *pDst++ = dspfx_neg_q31(*pSrc++);
    }
}

// This is empirical define. It can be computed with memory_latency value to be more effective.
#define XY_NEGATE_THRESHOLD 32

// simple XY memory map for vector negate function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/4)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Q31 negate of the elements of a vector.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_negate_q31(const PTR(q31_t) __restrict pSrc, PTR(q31_t)  __restrict pDst, uint32_t nSamples)
{
#if defined(_ARC_AS210_nomac)
    if (nSamples < XY_NEGATE_THRESHOLD) {
#endif
        // call to primitives based version of vector negate
        risc_dsp_negate_q31(pSrc, pDst, nSamples);
#if defined(_ARC_AS210_nomac)
    } else {
        int32_t currentSize;
        int32_t lastSize = nSamples;
        int32_t offset = 0;

        // loop that splits vector processing into parts which can fit into XY memory
        // number of iterations depends on XY size and vector length
        while (lastSize) 
        {
            // define block size to process inside XY memory
            currentSize = dspfx_min_q31(XY_BLOCK_SIZE, lastSize);

            // load XY content
            memcpy_StoX_int32(XYMAP_INPUT, pSrc + offset, currentSize);

            // call to XY based version of vector negate
            dsp_negate_q31_XtoY(XYMAP_INPUT, XYMAP_OUTPUT, currentSize);

            // extract result from XY memory
            memcpy_YtoS_int32(pDst + offset, XYMAP_OUTPUT, currentSize);
            offset += currentSize;
            lastSize -= currentSize;
        };
    }
#endif
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_NEGATE_Q31)
