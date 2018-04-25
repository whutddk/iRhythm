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

#if !defined(EXCLUDE_DSP_OFFSET_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_nomac)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

#pragma Code("dsplib_dsp_offset_q31")
// primitives based version of vector offset
static _Inline void _DSPLIB_XYCC risc_dsp_offset_q31(const PTR(q31_t) __restrict pSrc, q31_t offset, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
    int32_t sample;

    for (sample = 0; sample < (int32_t)nSamples; sample++){
        *pDst++ = dspfx_add_q31(*pSrc++, offset);
    }
}

// This is empirical define. It can be computed with memory_latency value to be more effective.
#define XY_OFFSET_THRESHOLD 32

// simple XY memory map for offset function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/4)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Adds a constant offset to each element of a vector.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param offset - additive constant
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_offset_q31(const PTR(q31_t) __restrict pSrc, q31_t offset, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
#if defined(_ARC_AS210_nomac)
    if (nSamples < XY_OFFSET_THRESHOLD) {
#endif
        // call to primitives based version of vector offset
        risc_dsp_offset_q31(pSrc, offset, pDst, nSamples);
#if defined(_ARC_AS210_nomac)
    } else {
        int32_t currentSize;
        int32_t lastSize = nSamples;
        int32_t data_offset = 0;

        // loop that splits vector processing into parts which can fit into XY memory
        // number of iterations depends on XY size and vector length
        while (lastSize) 
        {
            // define block size to process inside XY memory
            currentSize = dspfx_min_q31(XY_BLOCK_SIZE, lastSize);

            // load XY content
            memcpy_StoX_int32(XYMAP_INPUT, pSrc + data_offset, currentSize);

            // call to XY based version of offset
            dsp_offset_q31_XtoY(XYMAP_INPUT, offset, XYMAP_OUTPUT, currentSize);

            // extract result from XY memory
            memcpy_YtoS_int32(pDst + data_offset, XYMAP_OUTPUT, currentSize);
            data_offset += currentSize;
            lastSize -= currentSize;
        };
    }
#endif
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_OFFSET_Q31)
