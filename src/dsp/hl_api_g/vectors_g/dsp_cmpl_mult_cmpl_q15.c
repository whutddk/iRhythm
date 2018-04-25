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

#if !defined(EXCLUDE_DSP_CMPL_MULT_CMPL_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

// complex multiplication local primitives definition
//#define dspfx_cmpy_cq15(a, b) fx_q31_cast_cq15(fx_cq15_cast_rnd_v2a40(fx_v2a40_cmpy_cq15(fx_cq15_cast_q31(a), fx_cq15_cast_q31(b))))

#pragma Code("dsplib_dsp_cmpl_mult_cmpl_q15")
#ifdef _ARCv2EM_XY_AGU_MEDIUM

#if defined _XY_HAND_OPT

static _Inline void risc_dsp_cmpl_mult_cmpl_q15(const q15_t*  pSrcA, const q15_t*  pSrcB, q15_t* pDst, uint32_t nSamples)
{
    uint32_t i;
	_DSPLIB_store_regs(3, 3, 0);
	// fxapi primitives based branch for computation with complex instructions
	_set_agu_ap0((uint32_t )pDst);
	_set_agu_ap1((uint32_t )pSrcA);
	_set_agu_ap2((uint32_t )pSrcB);
	// non-swapping mode is used for AGU pointers ui32 instead of v2q15
	_set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_q15(2)); // pDst increment
	_set_agu_mod1(1, AGU_DT_ui32, AGU_AD_INC_q15(2)); // pSrcA increment
	_set_agu_mod2(2, AGU_DT_ui32, AGU_AD_INC_q15(2)); // pSrcB  increment
	// non-swapping code is used here, because software swapping of FXAPI is interferred with HW swapping of cmpyhfr for Big-Endian
	for (i = 0; i < (int32_t) nSamples; i++) {
		_st_agu_u0(_cmpyhfr(_agu_u1(),  _agu_u2()));
	}
	_DSPLIB_restore_regs();
}

#else

static _Inline void risc_dsp_cmpl_mult_cmpl_q15(const q15_t* __restrict pSrcA, const q15_t* __restrict pSrcB, q15_t* pDst, uint32_t nSamples)
{
    int32_t i;
    // fxapi primitives based branch for computation with complex instructions
    __xy cq15_t * __restrict pSrcAc2q15 = (__xy cq15_t*) pSrcA;
    __xy cq15_t * __restrict pSrcBc2q15 = (__xy cq15_t*) pSrcB;
    __xy cq15_t * __restrict pDstc2q15 = (__xy cq15_t*) pDst;
    for (i=0; i < (int32_t)nSamples; i++) {
    	// non-swapping code is used here, because software swapping of FXAPI interferes with HW swapping of cmpyhfr for Big-Endian
        //*pDstc2q15++ = _cmpyhfr(*pSrcAc2q15++, *pSrcBc2q15++);
        *pDstc2q15++ = dspfx_cmpy_cq15(*pSrcAc2q15++, *pSrcBc2q15++);
    }
}

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
// primitives based version of complex multiplication
static _Inline void risc_dsp_cmpl_mult_cmpl_q15(const q15_t* __restrict pSrcA, const q15_t* __restrict pSrcB, q15_t* pDst, uint32_t nSamples)
{
    int32_t i;
#if defined(__Xdsp_complex)
    // fxapi primitives based branch for computation with complex instructions
    cq15_t * __restrict pSrcAc2q15 = (cq15_t*) pSrcA;
    cq15_t * __restrict pSrcBc2q15 = (cq15_t*) pSrcB;
    cq15_t * __restrict pDstc2q15 = (cq15_t*) pDst;
    for (i=0; i < (int32_t)nSamples; i++) {
    	// non-swapping code is used here, because software swapping of FXAPI interferes with HW swapping of cmpyhfr for Big-Endian
        //*pDstc2q15++ = _cmpyhfr(*pSrcAc2q15++, *pSrcBc2q15++);
        *pDstc2q15++ = dspfx_cmpy_cq15(*pSrcAc2q15++, *pSrcBc2q15++);
    }
#else
    // dsplib primitives based branch
    dspfx_accum32()
    INIT_MACMODE();
    for (i=0; i < (int32_t)nSamples; i++)
    {
        q15_t aRe =*pSrcA++;
        q15_t aIm =*pSrcA++;
        q15_t bRe =*pSrcB++;
        q15_t bIm =*pSrcB++;
        // compute real part of result
        dspfx_a32mul_q15(aRe, bRe, acc);
        dspfx_a32mac_q15(aIm, dspfx_neg_q15(bIm), acc);
        *pDst++=dspfx_q15_cast_a32(acc);

        // compute image part of result
        dspfx_a32mul_q15(aRe, bIm, acc);
        dspfx_a32mac_q15(bRe, aIm, acc);
        *pDst++=dspfx_q15_cast_a32(acc);
    }
#endif
};
#endif //_ARCv2EM_XY_AGU_MEDIUM
// threshold for switching between XY and primitives based versions
#define XY_COMPL_MULT_COMPL_THRESHOLD 32

// simple XY memory map for complex multiplication function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/4)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Computes the Q15 complex multiplication of two complex numbers stored in a vector respectively
 * The return result is in 1.15 format.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_cmpl_mult_cmpl_q15(const PTR(q15_t) __restrict pSrcA, const PTR(q15_t) __restrict pSrcB, PTR(q15_t) __restrict pDst, uint32_t nSamples)
{   
#if defined(_ARC_AS210_mac16)
    if (nSamples < XY_COMPL_MULT_COMPL_THRESHOLD) {
#endif
        // call to primitives based version of complex mult complex
        risc_dsp_cmpl_mult_cmpl_q15(pSrcA, pSrcB, pDst, nSamples);
#if defined(_ARC_AS210_mac16)
    } else {
        int32_t IntsRemain = nSamples;
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
            dsp_cmpl_mult_cmpl_q15_XXtoY_DMAC16x16(XYMAP_INPUT, XYMAP_INPUT+IntsCurrent*2, XYMAP_OUTPUT, IntsCurrent);

            // extract result from XY memory
            memcpy_YtoS_int32((signed long *)(void *)pDst + offset, XYMAP_OUTPUT, IntsCurrent);

            offset += IntsCurrent;
            IntsRemain -= IntsCurrent;
        };
    }
#endif
};
#pragma Code()

#endif //!defined(EXCLUDE_DSP_CMPL_MULT_CMPL_Q15)
