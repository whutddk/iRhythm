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

#if !defined(EXCLUDE_DSP_MAX_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_nomac)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsplib_helper.h"


#pragma Code("dsplib_dsp_max_q15")
// ANSI C version of max search

#if defined _ARCv2EM_XY_AGU_SMALL && defined _XY_HAND_OPT

static _Inline void risc_dsp_max_q15(const q15_t *__restrict pSrc, uint32_t nSamples, q15_t * pResult, uint32_t *pIndex)
{
    int32_t sample, sample_max = 0;
    q31_t outval = ((uint32_t)pSrc[0] << 16);
	_DSPLIB_store_regs(1, 1, 0);
	_set_agu_ap0(pSrc+1);
	_set_agu_mod0(0, AGU_DT_q31_cast_q15, AGU_AD_INC_q15(1));

    for (sample = 1; sample < nSamples; sample++) {
		q31_t tmpval = _agu_u0_volatile();
        if (outval < tmpval) {
            outval = tmpval;
            sample_max = sample;
        }
    }
    *pResult = (q15_t)((uint32_t)outval >> 16);
    *pIndex = sample_max;
    _DSPLIB_restore_regs();
}

#else

static _Inline void risc_dsp_max_q15(const PTR(q15_t)__restrict pSrc, uint32_t nSamples, q15_t * pResult, uint32_t *pIndex)
{
	uint32_t sample, sample_max = 0;
	q15_t    outval = pSrc[0];
	for (sample = 1; sample < nSamples; sample++) {
		if (outval < pSrc[sample]) {        
			outval = pSrc[sample]; 
			sample_max = sample;       
		}
	}
	*pResult = outval;
	*pIndex = sample_max;
}
#endif

// threshold for switching between XY and ANSI C version
// This is empirical define. It can be computed with memory_latency value to be more effective.
#define XY_MAX_THRESHOLD 64

// simple XY memory map for max search function
#define XY_BLOCK_SIZE ((XY_REGION_SIZE/2)-2) // half of region
#define XYMAP_INPUT (0)

/**
 * Finds the maximum element of a Q15 vector.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] pResult
 * \param[out] pIndex
 */

void _DSPLIB_XYCC dsp_max_q15(const PTR(q15_t) __restrict pSrc, uint32_t nSamples, q15_t *pResult, uint32_t *pIndex)
{
#if defined(_ARC_AS210_nomac)
    if (nSamples < XY_MAX_THRESHOLD) {
#endif
        // call to ANSI C version of max search
        risc_dsp_max_q15(pSrc, nSamples, pResult, pIndex);
#if defined(_ARC_AS210_nomac)
    } else {
        int32_t lastSize = (int32_t)nSamples;
        uint32_t offset = 0;

        // initializing total max value and index
        q15_t maxval = MIN_VAL_Q15;
        q15_t currmax;
        uint32_t sample_cur, sample_max = 0;

        // loop that splits vector processing into parts which can fit into XY memory
        // number of iterations depends on XY size and vector length
        while (lastSize) 
        {
            // define block size to process inside XY memory
            int32_t currentSize = dspfx_min_q31(XY_BLOCK_SIZE, lastSize);

            // load XY content
            int32_t pxy_input = memcpy_StoX_int16(XYMAP_INPUT, pSrc + offset, currentSize);

            // call to XY based version of max search
            dsp_max_q15_X(pxy_input, currentSize, &currmax, &sample_cur);

            // update total max value and index
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

#endif //!defined(EXCLUDE_DSP_MAX_Q15)
