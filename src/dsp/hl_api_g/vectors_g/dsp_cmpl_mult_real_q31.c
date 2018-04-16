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

#if !defined(EXCLUDE_DSP_CMPL_MULT_REAL_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

#pragma Code("dsplib_dsp_cmpl_mult_real_q31")
#ifdef _ARCv2EM_XY_AGU_MEDIUM

#if defined _XY_HAND_OPT

static _Inline void risc_dsp_cmpl_mult_real_q31(const q31_t*  pSrcCmplx, const q31_t*  pSrcReal, q31_t*  pCmplxDst, uint32_t nSamples)
{
    uint32_t i;
	_DSPLIB_store_regs(3, 4, 0);
    _set_agu_ap0((uint32_t)pCmplxDst);
    _set_agu_ap1((uint32_t)pSrcCmplx);
    _set_agu_ap2((uint32_t)pSrcReal);
    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1)); // pCmplxDst increment
    _set_agu_mod1(1, AGU_DT_q31, AGU_AD_INC_q31(1)); // pSrcCmplx increment
    _set_agu_mod2(2, AGU_DT_q31, AGU_AD_INC_q31(1)); // pSrcReal  increment
    _set_agu_mod3(2, AGU_DT_q31, AGU_AD_INC_q31(0)); // pSrcReal  non-increment
    for ( i =0; i<nSamples; i++)
    {
    	_st_agu_u0(dspfx_mpy_q31(_agu_u1(), (_agu_u3())));
    	_st_agu_u0(dspfx_mpy_q31(_agu_u1(), (_agu_u2())));
    }
    _DSPLIB_restore_regs();
}

#else

static _Inline void risc_dsp_cmpl_mult_real_q31(const __xy q31_t* __restrict pSrcCmplx, const __xy q31_t* __restrict pSrcReal, __xy q31_t* __restrict pCmplxDst, uint32_t nSamples)
{
    uint32_t i;
    for (i = 0; i<nSamples; i++)
    {
        *pCmplxDst++ = dspfx_mpy_q31(*pSrcCmplx++, *pSrcReal);
        *pCmplxDst++ = dspfx_mpy_q31(*pSrcCmplx++, *pSrcReal++);
    }
};

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
// primitives based version of complex multiplication with real
static _Inline void risc_dsp_cmpl_mult_real_q31(const q31_t* __restrict pSrcCmplx, const q31_t* __restrict pSrcReal, q31_t* __restrict pCmplxDst, uint32_t nSamples)
{
    uint32_t i;
    for (i = 0; i<nSamples; i++)
    {
        q31_t real = *pSrcReal++;
        *pCmplxDst++ = dspfx_mpy_q31(*pSrcCmplx++, real);
        *pCmplxDst++ = dspfx_mpy_q31(*pSrcCmplx++, real);
    }
};
#endif //_ARCv2EM_XY_AGU_MEDIUM
// threshold for switching between XY and primitives based versions
#define XY_COMPL_MULT_REAL_THRESHOLD 32

// simple XY memory map for complex multiplication function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/4)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Computes the Q31 complex multiplication of complex and real numbers stored in a vector respectively
 * The return result is in 1.31 format.
 *
 * \param[in] pSrcCmplx
 * \param[in] pSrcReal
 * \param[out] pCmplxDst
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_cmpl_mult_real_q31(const PTR(q31_t) __restrict pSrcCmplx, const PTR(q31_t) __restrict pSrcReal, PTR(q31_t) __restrict pCmplxDst, uint32_t nSamples)
{
#if defined(_ARC_AS210_mac32)
    if (nSamples < XY_COMPL_MULT_REAL_THRESHOLD) {
#endif
        // call to primitives based version of complex mult complex
        risc_dsp_cmpl_mult_real_q31(pSrcCmplx, pSrcReal, pCmplxDst, nSamples);
#if defined(_ARC_AS210_mac32) 
    } else {
        int32_t IntsRemain = nSamples;
        int32_t offset = 0;

        // loop that splits vector processing into parts which can fit into XY memory
        // number of iterations depends on XY size and vector length
        while (IntsRemain) 
        {
            // define block size to process inside XY memory
            int32_t IntsCurrent = dspfx_min_q31(IntsRemain, (XY_BLOCK_SIZE / 3));

            // load XY content
            memcpy_StoX_int32(XYMAP_INPUT,                       (signed long *)(void *)pSrcCmplx + offset * 2,      IntsCurrent * 2);
            memcpy_StoX_int32(XYMAP_INPUT + IntsCurrent * 2,     (signed long *)(void *)pSrcReal + offset,           IntsCurrent);

            // call to XY based version of complex multiplication with real
            dsp_cmpl_mult_real_q31_XXtoY_MAC32x32(XYMAP_INPUT, XYMAP_INPUT + IntsCurrent * 2, XYMAP_OUTPUT, IntsCurrent);

            // extract result from XY memory
            memcpy_YtoS_int32((signed long *)(void *)pCmplxDst + offset * 2, XYMAP_OUTPUT, IntsCurrent * 2);
        
            offset += IntsCurrent;
            IntsRemain -= IntsCurrent;
        };
    }
#endif
};
#pragma Code()

#endif //!defined(EXCLUDE_DSP_CMPL_MULT_REAL_Q31)
