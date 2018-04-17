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

#if !defined(EXCLUDE_DSP_SCALE_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

#pragma Code("dsplib_dsp_scale_q31")
// primitives based version of vector scale
static _Inline void _DSPLIB_XYCC risc_dsp_scale_q31(const PTR(q31_t) __restrict pSrc, q31_t   scaleFract, int8_t  shift, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
    int32_t sample;
    q31_t acc;
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
        acc = dspfx_mpy_q31(*pSrc++, scaleFract);
        *pDst++ = dspfx_asl_q31(acc, shift);
    }
}

// This is empirical define. It can be computed with memory_latency value to be more effective.
#define XY_SCALE_THRESHOLD 32

// simple XY memory map for scale function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/4)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Q31 multiply a vector by a scalar value.
 * The algorithm used is:
 * pDst[n] = (pSrc[n] * scaleFract) << shift,   0 <= n < nSamples. 
 * The overall scale factor applied to the fixed-point data is:
 * scale = scaleFract * 2^shift.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param scaleFract
 * \param shift
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_scale_q31(const PTR(q31_t) __restrict pSrc, q31_t   scaleFract, int8_t  shift, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
#if defined(_ARC_AS210_mac32)
    if (nSamples < XY_SCALE_THRESHOLD) {
#endif
        // call to primitives based version of vector scale
        risc_dsp_scale_q31(pSrc, scaleFract, shift, pDst, nSamples);
#if defined(_ARC_AS210_mac32)
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

            // call to XY based version of scale
            dsp_scale_q31_XtoY_MAC32x32(XYMAP_INPUT, scaleFract, shift, XYMAP_OUTPUT, currentSize);

            // extract result from XY memory
            memcpy_YtoS_int32(pDst + offset, XYMAP_OUTPUT, currentSize);
            offset += currentSize;
            lastSize -= currentSize;
        };
    }
#endif
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_SCALE_Q31)
