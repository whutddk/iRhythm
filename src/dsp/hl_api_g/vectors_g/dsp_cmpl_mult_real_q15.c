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

#if !defined(EXCLUDE_DSP_CMPL_MULT_REAL_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsplib_helper.h"

// complex multiplication with real local primitives definition
#if defined(__GUARD_ACCUM__) 
#define dspfx_cmpy_cq15q15(a, b) fx_q31_cast_cq15(fx_cq15_cast_rnd_v2a40(fx_v2a40_cmpy_cq15(fx_cq15_cast_q31(a), fx_create_cq15((b), 0))))
#else
#define dspfx_cmpy_cq15q15(a, b) fx_q31_cast_cq15(fx_cq15_cast_rnd_v2a32(fx_v2a32_cmpy_cq15(fx_cq15_cast_q31(a), fx_create_cq15((b), 0))))
#endif

#pragma Code("dsplib_dsp_cmpl_mult_real_q15")
#ifdef _ARCv2EM_XY_AGU_MEDIUM

#if defined _XY_HAND_OPT

static _Inline void risc_dsp_cmpl_mult_real_q15(const q15_t*  pSrcCmplx, const q15_t*  pSrcReal, q15_t*  pCmplxDst, uint32_t nSamples)
{
    uint32_t i;
	_DSPLIB_store_regs(3, 3, 0);
    _set_agu_ap0((uint32_t)pCmplxDst);
    _set_agu_ap1((uint32_t)pSrcCmplx);
    _set_agu_ap2((uint32_t)pSrcReal);
	_set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // pCmplxDst increment
	_set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // pSrcCmplx increment
	_set_agu_mod2(2, AGU_DT_v2q15_replic_q15, AGU_AD_INC_q15(1));   // pSrcReal  increment

	for (i = 0; i < (int32_t) nSamples; i++) {
		_st_agu_u0(fx_q31_cast_v2q15(fx_v2q15_cast_v2a40(fx_v2a40_mpy_v2q15(fx_v2q15_cast_q31(_agu_u1()), fx_v2q15_cast_q31(_agu_u2())))));
	}
	_DSPLIB_restore_regs();
}

#else

static _Inline void risc_dsp_cmpl_mult_real_q15(const q15_t* __restrict pSrcCmplx, const PTR(q15_t) __restrict pSrcReal, PTR(q15_t) __restrict pCmplxDst, uint32_t nSamples)
{
    uint32_t i;
    // fxapi primitives based branch for computation with complex instructions
    PTR(v2q15_t) __restrict pSrcCmplxv2q15 = (PTR(v2q15_t))pSrcCmplx;
    PTR(v2q15_t) __restrict pDstv2q15 = (PTR(v2q15_t))pCmplxDst;
    for (i=0; i < (int32_t)nSamples; i++)
    {
    	*pDstv2q15++ = fx_v2q15_cast_v2a40(fx_v2a40_mpy_v2q15(*pSrcCmplxv2q15++, fx_replic_v2q15(*pSrcReal++)));
    }
}

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
// primitives based version of complex multiplication with real
static _Inline void risc_dsp_cmpl_mult_real_q15(const q15_t* __restrict pSrcCmplx, const q15_t* __restrict pSrcReal, q15_t* __restrict pCmplxDst, uint32_t nSamples)
{
    uint32_t i; 
#if defined(__FXAPI__) && defined(__Xdsp_complex)
    // fxapi primitives based branch for computation with complex instructions
    uint32_t * __restrict pSrcCmplxc2q15 = (uint32_t *)pSrcCmplx;
    q15_t * __restrict pSrcRealcq15 = (q15_t *)pSrcReal;
    uint32_t * __restrict pDstc2q15 = (uint32_t *)pCmplxDst;
    for (i=0; i < (int32_t)nSamples; i++) {
        *pDstc2q15++ = dspfx_cmpy_cq15q15(*pSrcCmplxc2q15++, *pSrcRealcq15++);

    }
#else
    // dsplib primitives based branch
    INIT_MACMODE();
    for (i = 0; i<nSamples; i++)
    {
        q15_t real = *pSrcReal++;
        // compute real part of result
        *pCmplxDst++ = dspfx_mpy_q15(*pSrcCmplx++, real);
        // compute image part of result
        *pCmplxDst++ = dspfx_mpy_q15(*pSrcCmplx++, real);
    }
#endif
};
#endif //_ARCv2EM_XY_AGU_MEDIUM
// threshold for switching between XY and primitives based versions
#define XY_COMPL_MULT_REAL_THRESHOLD 32

// simple XY memory map for complex multiplication function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/2)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Computes the Q15 complex multiplication of complex and real numbers stored in a vector respectively
 * The return result is in 1.15 format.
 *
 * \param[in] pSrcCmplx
 * \param[in] pSrcReal
 * \param[out] pCmplxDst
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_cmpl_mult_real_q15(const PTR(q15_t) __restrict pSrcCmplx, const PTR(q15_t) __restrict pSrcReal, PTR(q15_t) __restrict pCmplxDst, uint32_t nSamples)
{
#if defined(_ARC_AS210_mac16)
    if (nSamples < XY_COMPL_MULT_REAL_THRESHOLD) {
#endif
        // call to primitives based version of complex mult complex
        risc_dsp_cmpl_mult_real_q15(pSrcCmplx, pSrcReal, pCmplxDst, nSamples);
#if defined(_ARC_AS210_mac16)
    } else {
        int32_t ShortsRemain = nSamples;
        int32_t offset = 0;

        // loop that splits vector processing into parts which can fit into XY memory
        // number of iterations depends on XY size and vector length
        while (ShortsRemain) 
        {
            // define block size to process inside XY memory
            int32_t ShortsCurrent = dspfx_min_q31(ShortsRemain, (XY_BLOCK_SIZE / 3));

            // load XY content
            uint32_t NewRealOffset = memcpy_StoX_int16(XYMAP_INPUT + ShortsCurrent*2, pSrcReal + offset, ShortsCurrent);
            memcpy_StoX_int32(XYMAP_INPUT, (signed long *)(void *)pSrcCmplx  + offset, ShortsCurrent);                    

            // call to XY based version of complex multiplication with real
            dsp_cmpl_mult_real_q15_XXtoY_DMAC16x16(XYMAP_INPUT, XYMAP_INPUT + NewRealOffset, XYMAP_OUTPUT, ShortsCurrent);

            // extract result from XY memory
            memcpy_YtoS_int32((signed long *)(void *)pCmplxDst + offset, XYMAP_OUTPUT, ShortsCurrent);

            offset += ShortsCurrent;
            ShortsRemain -= ShortsCurrent;
        };    
    }
#endif
};
#pragma Code()

#endif //!defined(EXCLUDE_DSP_CMPL_MULT_REAL_Q15)
