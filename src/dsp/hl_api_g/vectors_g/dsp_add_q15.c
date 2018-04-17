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

#if !defined(EXCLUDE_DSP_ADD_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_nomac) 
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsplib_helper.h"

#pragma Code("dsplib_dsp_add_q15")

#if defined _ARCv2EM_XY && defined __FXAPI__

#if defined _XY_HAND_OPT &&  defined _ARCv2EM_XY_AGU_MEDIUM

void _DSPLIB_XYCC dsp_add_q15(const __xy q15_t* __restrict pSrcA, const __xy q15_t* __restrict pSrcB, __xy q15_t* __restrict pDst, uint32_t nSamples)
{
    int32_t i, N  = nSamples >> 1;
	_DSPLIB_store_regs(3, 3, 0);

    _set_agu_ap0(pSrcA); // Set AP registers for pSrcA
    _set_agu_ap1(pSrcB); // Set AP registers for pSrcA
    _set_agu_ap2(pDst); // Set AP registers for pSrcB

    _set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2));
    _set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_INC_q15(2));
    _set_agu_mod2(2, AGU_DT_v2q15, AGU_AD_INC_q15(2));

    for (i = 0; i < N; i++) {
        _st_agu_u2(fx_q31_cast_v2q15(fx_add_v2q15(fx_v2q15_cast_q31(_agu_u0()), fx_v2q15_cast_q31(_agu_u1()))));
    }
    if (nSamples & 1) {
        _set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(1));
        _set_agu_mod1(1, AGU_DT_q15, AGU_AD_INC_q15(1));
        _set_agu_mod2(2, AGU_DT_q15, AGU_AD_INC_q15(1));
        _st_agu_u2(fx_q31_cast_v2q15(fx_add_v2q15(fx_v2q15_cast_q31(_agu_u0()), fx_v2q15_cast_q31(_agu_u1()))));
    }

	_DSPLIB_restore_regs();
}

#else


#if defined _CCAC_AUTOVECTORIZATION

void _DSPLIB_XYCC dsp_add_q15(const __xy q15_t* __restrict pSrcA, const __xy q15_t* __restrict pSrcB, __xy q15_t* __restrict pDst, uint32_t nSamples)
{
    int32_t i;
#pragma clang loop vectorize(enable) interleave_count(1)
    for (i = 0; i < nSamples; i++) {
        *pDst++ = fx_add_q15(*pSrcA++, *pSrcB++);
    }
}

#else

void _DSPLIB_XYCC dsp_add_q15(const __xy q15_t* __restrict pSrcA, const __xy q15_t* __restrict pSrcB, __xy q15_t* __restrict pDst, uint32_t nSamples)
{
    int32_t i;
    __xy v2q15_t *pvSrcA, *pvSrcB, *pvDst;

    if (nSamples & 1) {
    	*pDst++ = fx_add_q15(*pSrcA++,*pSrcB++);
    }
    pvSrcA = (__xy v2q15_t*) pSrcA;
    pvSrcB = (__xy v2q15_t*) pSrcB;
    pvDst = (__xy v2q15_t*) pDst;

    for (i = 0; i < (nSamples >> 1); i++) {
        *pvDst++ = fx_add_v2q15(*pvSrcA++, *pvSrcB++);
    }
}

#endif

#endif


#else //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

// primitives based version of vector addition
static _Inline void risc_dsp_add_q15(const q15_t* __restrict pSrcA, const q15_t* __restrict pSrcB, q15_t* pDst, uint32_t nSamples)
{
    int32_t sample;
#if !defined(dspfx_add_v2q15)
    // branch that calculates abs with plain loop
    for (sample = 0; sample < (int32_t)nSamples; sample++){
        *pDst++ = dspfx_add_q15(*pSrcA++, *pSrcB++);
    }
#else
    // branch that calculates addition with vector instruction
    // using vector instruction is effective in only case when both inputs and
    // output data pointers are equally aligned or unaligned within 4 byte boundary
    //
    // example 1: pSrcA = 0x00001000 pSrcB = 0x00003000 pDst = 0x00002000 => vectorization is effective
    // example 2: pSrcA = 0x00001002 pSrcB = 0x00003002 pDst = 0x00002002 => vectorization is effective
    // example 3: pSrcA = 0x00001004 pSrcB = 0x00003008 pDst = 0x00002006 => vectorization is not effective

    int32_t  startSample = (((uint32_t)pSrcA) & 0x3)>>1;
    if (nSamples == 0) return;
#if !defined(__Xunaligned)
    // check for equal (un)alignment
    if ((startSample != (((uint32_t)pDst & 0x3)>>1)) ||
        (startSample != (((uint32_t)pSrcB & 0x3)>>1)))
    {
        // branch for inputs and output are not 'equally' (un)aligned
        for (sample = 0; sample < (int32_t)nSamples; sample++) {
            *pDst++ = dspfx_add_q15(*pSrcA++, *pSrcB++);
        }
    }
    else
#endif
    {
        //branch that uses vector instruction
        q31_t* __restrict pDst_32;
        q31_t* __restrict pSrcA_32;
        q31_t* __restrict pSrcB_32;
        int32_t N;

        // process first sample if it is needed
        if (startSample) *pDst = dspfx_add_q15(*pSrcA,*pSrcB);

        pDst_32  = (int32_t*)(void*)(&pDst[startSample]);
        pSrcA_32 = (int32_t*)(void*)(&pSrcA[startSample]);
        pSrcB_32 = (int32_t*)(void*)(&pSrcB[startSample]);

        // main vector instruction loop
        N = (nSamples - startSample) >> 1;
        for (sample = 0; sample < N; sample++)
            *pDst_32++ = dspfx_add_v2q15(*pSrcA_32++, *pSrcB_32++);

        // process last sample if it is needed
        sample = (N << 1) + startSample;
        if (sample < (int32_t)nSamples)
            pDst[nSamples-1] = dspfx_add_q15(pSrcA[nSamples-1],pSrcB[nSamples-1]);
    }
#endif
}

// threshold for switching between XY and primitives based versions
#define XY_ADD_THRESHOLD 32

// simple XY memory map for add function
#define XY_BLOCK_SIZE (((XY_REGION_SIZE/2)/2)-2) // half of region
#define XYMAP_INPUT0 (0)
#define XYMAP_INPUT1 ((XY_REGION_SIZE/2)/2)
#define XYMAP_OUTPUT (0)

/**
 * Q15 element-by-element addition of two vectors.
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nSamples
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_add_q15(const q15_t* __restrict pSrcA, const q15_t* __restrict pSrcB, q15_t* __restrict pDst, uint32_t nSamples)
{
#if defined(_ARC_AS210_nomac) 
    if (nSamples < XY_ADD_THRESHOLD) {
#endif
        // call to primitives based version of add
        risc_dsp_add_q15(pSrcA, pSrcB, pDst, nSamples);
#if defined(_ARC_AS210_nomac) 
    } else {
        int32_t lastSize = nSamples;
        int32_t offset = 0;

        // loop that splits vector processing into parts which can fit into XY memory
        // number of iterations depends on XY size and vector length
        while (lastSize) 
        {
            // define block size to process inside XY memory
            int32_t currentSize = dspfx_min_q31(XY_BLOCK_SIZE, lastSize);

            // load XY content
            int32_t pxy_input0 = memcpy_StoX_int16(XYMAP_INPUT0, pSrcA + offset, currentSize);
            int32_t pxy_input1 = memcpy_StoX_int16(XYMAP_INPUT1, pSrcB + offset, currentSize);

            // call to XY based version of add
            dsp_add_q15_XXtoY(pxy_input0, pxy_input1, XYMAP_OUTPUT, currentSize);

            // extract result from XY memory
            memcpy_YtoS_int16(pDst + offset, XYMAP_OUTPUT, currentSize);
            offset += currentSize;
            lastSize -= currentSize;
        };
    }
#endif
}

#endif //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

#endif //!defined(EXCLUDE_DSP_ADD_Q15)
