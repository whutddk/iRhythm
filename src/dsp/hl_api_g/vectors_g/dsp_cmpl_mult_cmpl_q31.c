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

#if !defined(EXCLUDE_DSP_CMPL_MULT_CMPL_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

#pragma Code("dsplib_dsp_cmpl_mult_cmpl_q31")
#if defined _ARCv2EM_XY_AGU_MEDIUM  && defined _XY_HAND_OPT
static _Inline void risc_dsp_cmpl_mult_cmpl_q31(const q31_t*  pSrcA, const q31_t*  pSrcB, q31_t*  pDst, uint32_t nSamples)
{
    uint32_t i;
    dspfx_accum64()
	_DSPLIB_store_regs(3, 5, 0);
    _set_agu_ap0((uint32_t)pDst);

    _set_agu_ap1((uint32_t)pSrcA);
    _set_agu_ap2((uint32_t)pSrcB);
    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1)); // pDst increment by 2

    _set_agu_mod1(1, AGU_DT_q31, AGU_AD_INC_q31(1)); // pSrcA increment
    _set_agu_mod2(1, AGU_DT_q31, AGU_AD_DEC_q31(1)); // pSrcA decrement

    _set_agu_mod3(2, AGU_DT_q31, AGU_AD_INC_q31(1)); // pSrcB  increment
    _set_agu_mod4(2, AGU_DT_q31, AGU_AD_DEC_q31(1)); // pSrcB  decrement
    _set_agu_mod5(2, AGU_DT_q31, AGU_AD_INC_q31(0)); // pSrcB  no update
    _set_agu_mod6(2, AGU_DT_q31, AGU_AD_INC_q31(2)); // pSrcB  increment by 2

    // Real/Image part
    for ( i =0; i<nSamples; i++)
    {
    	           dspfx_a64mul_q31(_agu_u1(), _agu_u3(), acc);  // a.re*b.re
    	_st_agu_u0(dspfx_a64msu_q31(_agu_u2(), _agu_u5(), acc)); //-a.im*b.im
                   dspfx_a64mul_q31(_agu_u1(), _agu_u4(), acc);  // a.re*b.im
        _st_agu_u0(dspfx_a64mac_q31(_agu_u1(), _agu_u6(), acc)); //+a.im*b.re
    }

    _DSPLIB_restore_regs();
}
#else //_ARCv2EM_XY_AGU_MEDIUM
// primitives based version of complex multiplication
static _Inline void risc_dsp_cmpl_mult_cmpl_q31(const PTR(q31_t) __restrict pSrcA, const PTR(q31_t) __restrict pSrcB, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
    uint32_t i;
    dspfx_accum64()
    for (i=0; i < nSamples; i++)
    {
        //q31_t aRe =*pSrcA++;
        //q31_t aIm =*pSrcA++;
        //q31_t bRe =*pSrcB++;
        //q31_t bIm =*pSrcB++;
                  dspfx_a64mul_q31(pSrcA[0], pSrcB[0]/*aRe, bRe*/, acc);
        *pDst++ = dspfx_a64msu_q31(pSrcA[1], pSrcB[1]/*aIm, bIm*/, acc);
                  dspfx_a64mul_q31(pSrcA[0], pSrcB[1]/*aRe, bIm*/, acc);
        *pDst++ = dspfx_a64mac_q31(pSrcA[1], pSrcB[0]/*bRe, aIm*/, acc);
        pSrcA+=2; pSrcB+=2;
    }
};

#endif //_ARCv2EM_XY_AGU_MEDIUM
// threshold for switching between XY and primitives based versions
#define XY_COMPL_MULT_COMPL_THRESHOLD 32

// simple XY memory map for dot product function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/4)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Computes the Q31 complex multiplication of two complex numbers stored in a vector respectively
 * The return result is in 1.31 format.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_cmpl_mult_cmpl_q31(const PTR(q31_t) __restrict pSrcA, const PTR(q31_t) __restrict pSrcB, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
#if defined(_ARC_AS210_mac32)
    if (nSamples < XY_COMPL_MULT_COMPL_THRESHOLD) {
#endif
        // call to primitives based version of complex mult complex
        risc_dsp_cmpl_mult_cmpl_q31(pSrcA, pSrcB, pDst, nSamples);
#if defined(_ARC_AS210_mac32) 
    } else {
        int32_t IntsRemain = nSamples * 2;
        int32_t offset = 0;

        // loop that splits vector processing into parts which can fit into XY memory
        // number of iterations depends on XY size and vector length
        while (IntsRemain)
        {
            // define block size to process inside XY memory
            int32_t IntsCurrent = dspfx_min_q31(IntsRemain, XY_BLOCK_SIZE/2);

            // load XY content
            memcpy_StoX_int32(XYMAP_INPUT,                   (signed long *)(void *)pSrcA + offset,    IntsCurrent);
            memcpy_StoX_int32(XYMAP_INPUT + IntsCurrent,     (signed long *)(void *)pSrcB + offset,    IntsCurrent);

            // call to XY based version of complex multiplication
            dsp_cmpl_mult_cmpl_q31_XXtoY_MAC32x32(XYMAP_INPUT, XYMAP_INPUT + IntsCurrent, XYMAP_OUTPUT, IntsCurrent>>1);

            // extract result from XY memory
            memcpy_YtoS_int32((signed long *)(void *)pDst + offset, XYMAP_OUTPUT, IntsCurrent);

            offset += IntsCurrent;
            IntsRemain -= IntsCurrent;        
        };
    }
#endif
};

#endif //!defined(EXCLUDE_DSP_CMPL_MULT_CMPL_Q31)
