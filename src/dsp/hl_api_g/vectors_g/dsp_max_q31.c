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

#if !defined(EXCLUDE_DSP_MAX_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_nomac)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

#pragma Code("dsplib_dsp_max_q31")
// ANSI C version of max search
static _Inline void _DSPLIB_XYCC risc_dsp_max_q31(const PTR(q31_t) __restrict pSrc, uint32_t nSamples, q31_t * pResult, uint32_t *pIndex)
{
    uint32_t sample, sample_max = 0;
    q31_t    outval = pSrc[0];
    for (sample = 1; sample < nSamples; sample++) {
        if (outval < pSrc[sample]) {        
            outval = pSrc[sample]; 
            sample_max = sample;       
        }
    }
    *pResult = outval;
    *pIndex = sample_max;
}

// threshold for switching between XY and ANSI C version
// This is empirical define. It can be computed with memory_latency value to be more effective.
#define XY_MAX_THRESHOLD 32

// simple XY memory map for max search function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/4) // half of region
#define XYMAP_INPUT (0)

/**
 * Finds the maximum element of a Q31 vector.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] pResult
 * \param[out] pIndex
 */

void _DSPLIB_XYCC dsp_max_q31(const PTR(q31_t) __restrict pSrc, uint32_t nSamples, q31_t *pResult, uint32_t *pIndex)
{
#if defined(_ARC_AS210_nomac)
    if (nSamples < XY_MAX_THRESHOLD) {
#endif
        // call to ANSI C version of max search
        risc_dsp_max_q31(pSrc, nSamples, pResult, pIndex);
#if defined(_ARC_AS210_nomac)
    } else {
        int32_t currentSize;
        int32_t lastSize = (int32_t)nSamples;
        uint32_t offset = 0;

        // initializing total max value and index
        q31_t maxval = MIN_VAL_Q31;
        q31_t currmax;
        uint32_t sample_cur, sample_max = 0;

        // loop that splits vector processing into parts which can fit into XY memory
        // number of iterations depends on XY size and vector length
        while (lastSize) 
        {
            // define block size to process inside XY memory
            currentSize = dspfx_min_q31(XY_BLOCK_SIZE, lastSize);

            // load XY content
            memcpy_StoX_int32(XYMAP_INPUT, pSrc + offset, currentSize);

            // call to XY based version of max search
            dsp_max_q31_X(XYMAP_INPUT, currentSize, &currmax, &sample_cur);

            // update result total max value and index
            if(maxval < currmax) {
                maxval = currmax;
                sample_max = offset + sample_cur;
            }
            offset += currentSize;
            lastSize -= currentSize;
        };
        *pResult = maxval;
        *pIndex = sample_max;
    }
#endif
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_MAX_Q31)
