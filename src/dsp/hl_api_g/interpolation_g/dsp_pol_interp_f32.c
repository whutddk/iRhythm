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

#if !defined(EXCLUDE_DSP_POL_INTERP_F32)

#include "dsplib_types.h"
#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_pol_eval_f32")

/**
 * f32 POLYNOMIAL EVALUATION INIT FUNCTION
 *
 * \param inst - pointer to interpolation instance structure
 * \param nCoeffs - number of coefficients. (polynom order = nCoeffs - 1)
 * \param pCoeffs - pointer to coefficients vector in direct order (c[0]*x^n + c[1]*x^(n-1)....c[n]), n = nCoeffs-1
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_pol_eval_init_f32(pol_f32_t *inst, uint16_t nCoeffs, const PTR(f32_t) pCoeffs)
{
    inst->order = nCoeffs;
    inst->pCoeffs = pCoeffs;

    return DSP_ERR_OK;
};

/**
 * f32 POLYNOMIAL EVALUATION PROCESSING FUNCTION
 *
 * \param inst - pointer to interpolation instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

static void __dsp_pol_eval_f32(const pol_f32_t *inst ,const f32_t * __restrict pSrc, f32_t * __restrict pDst, uint32_t nSamples)
{
    int32_t n,i;
    uint16_t order = inst->order;
    const f32_t * coeffs = (f32_t*)inst->pCoeffs;

    if (nSamples & 1) {
        f32_t x = *pSrc++;
        f32_t res;

        res = coeffs[0];
        for (i=1; i < order; i++)
            res = res*x + coeffs[i];

        *pDst++=res;
        n = 1;
    }

    for (n=0; n < (int32_t)(nSamples>>1); n++)
    {
         f32_t x0 = *pSrc++;
         f32_t x1 = *pSrc++;
         f32_t res0, res1;
     
         res0 = res1 = coeffs[0];
         for (i=1; i < order; i++) {
             res0 = res0*x0 + coeffs[i];
             res1 = res1*x1 + coeffs[i];
         }
    
         *pDst++ = res0;
         *pDst++ = res1;
    }

    return;
};


#ifdef _ARCv2EM_XY
#pragma Off_inline(__dsp_pol_eval_f32)
#endif

void _DSPLIB_XYCC dsp_pol_eval_f32(const pol_f32_t *inst ,const PTR(f32_t) __restrict pSrc, PTR(f32_t) __restrict pDst, uint32_t nSamples)
{
	__dsp_pol_eval_f32(inst, pSrc, pDst, nSamples);
}

void dsp_pol_eval_delete_f32(pol_f32_t *inst)
{
    // TODO: Place code here
};

#endif //!defined(EXCLUDE_DSP_POL_INTERP_F32)
