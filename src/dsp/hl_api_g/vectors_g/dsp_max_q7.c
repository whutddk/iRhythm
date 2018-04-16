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

#if !defined(EXCLUDE_DSP_MAX_Q7)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_max_q7")

/**
 * Finds the maximum element of a Q7 vector.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] pResult
 * \param[out] pIndex
 */

#if defined _ARCv2EM_XY_AGU_SMALL

void _DSPLIB_XYCC dsp_max_q7(const __xy q7_t* __restrict pSrc, uint32_t nSamples, q7_t *pResult, uint32_t *pIndex)
{
	int32_t sample, sample_max = 0;
	q31_t outval = ((uint32_t)pSrc[0] << 24);

	_set_agu_ap0(pSrc+1);
	_set_agu_mod0(0, AGU_DT_q31_cast_q7, AGU_AD_INC_q7(1));

	for (sample = 1; sample < nSamples; sample++) {
		q31_t tmpval = _agu_u0_volatile();
		if (outval < tmpval) {
			outval = tmpval;
			sample_max = sample;       
		}
	}
	*pResult = (q7_t)((uint32_t)outval >> 24);
	*pIndex = sample_max;
}

#else

void dsp_max_q7(const q7_t* __restrict pSrc, uint32_t nSamples, q7_t *pResult, uint32_t *pIndex)
{
    uint32_t sample, sample_max = 0;
    q7_t    outval = pSrc[0];
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

#endif //!defined(EXCLUDE_DSP_MAX_Q7)
