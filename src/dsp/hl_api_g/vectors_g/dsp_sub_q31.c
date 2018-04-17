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

#if !defined(EXCLUDE_DSP_SUB_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_nomac) 
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

#pragma Code("dsplib_dsp_sub_q31")
// primitives based version of vector subtraction
static _Inline void _DSPLIB_XYCC risc_dsp_sub_q31(const PTR(q31_t) __restrict pSrcA, const PTR(q31_t) __restrict pSrcB, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
    int32_t sample;
    for (sample = 0; sample < (int32_t)nSamples; sample++)
    {
        *pDst++ = dspfx_sub_q31(*pSrcA++, *pSrcB++);
    }
}

// threshold for switching between XY and primitives based versions
#define XY_SUB_THRESHOLD 32

// simple XY memory map for sub function
#define XY_BLOCK_SIZE ((XY_REGION_SIZE/4)/2)
#define XYMAP_INPUT0 (0)
#define XYMAP_INPUT1 ((XY_REGION_SIZE/4)/2)
#define XYMAP_OUTPUT (0)

/**
 * Q31 element-by-element subtraction of two vectors.
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nSamples
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_sub_q31(const PTR(q31_t) __restrict pSrcA, const PTR(q31_t) __restrict pSrcB, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
#if defined(_ARC_AS210_nomac) 
    if (nSamples < XY_SUB_THRESHOLD) {
#endif
        // call to primitives based version of sub
        risc_dsp_sub_q31(pSrcA, pSrcB, pDst, nSamples);
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
            memcpy_StoX_int32(XYMAP_INPUT0, pSrcA + offset, currentSize);
            memcpy_StoX_int32(XYMAP_INPUT1, pSrcB + offset, currentSize);

            // call to XY based version of sub
            dsp_sub_q31_XXtoY(XYMAP_INPUT0, XYMAP_INPUT1, XYMAP_OUTPUT, currentSize);

            // extract result from XY memory
            memcpy_YtoS_int32(pDst + offset, XYMAP_OUTPUT, currentSize);
            offset += currentSize;
            lastSize -= currentSize;
        };
    }
#endif
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_SUB_Q31)
