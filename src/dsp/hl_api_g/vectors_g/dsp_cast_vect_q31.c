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

#if !defined(EXCLUDE_DSP_CAST_VECT_Q31)

#include "dsplib.h"
#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_cast_vec_q31")
/**
 * Convert the elements of a Q31 input vector to other data types.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */
void _DSPLIB_XYCC dsp_q15_cast_vect_q31(PTR(q31_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples)
{
    uint32_t sample;
    for (sample = 0; sample < nSamples; sample++)
    {
        *pDst++ = dsp_q15_cast_q31(*pSrc++);
    }
}
void _DSPLIB_XYCC dsp_q7_cast_vect_q31 (PTR(q31_t) __restrict pSrc, PTR(q7_t)  __restrict pDst, uint32_t nSamples)
{
    uint32_t sample;
    for (sample = 0; sample < nSamples; sample++)
    {
        *pDst++ = dsp_q7_cast_q31(*pSrc++);
    }
}
void _DSPLIB_XYCC dsp_f32_cast_vect_q31(PTR(q31_t) __restrict pSrc, PTR(f32_t) __restrict pDst, uint32_t nSamples)
{
    uint32_t sample;
    for (sample = 0; sample < nSamples; sample++)
    {
        *pDst++ = dsp_f32_cast_q31(*pSrc++);
    }
}


#pragma Code()

#endif //!defined(EXCLUDE_DSP_CAST_VECT_Q31)
