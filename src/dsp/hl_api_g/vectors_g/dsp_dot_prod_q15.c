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

#if !defined(EXCLUDE_DSP_DOT_PROD_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32) 
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsplib_helper.h"

#pragma Code("dsplib_dsp_dot_prod_q15")


#if defined _ARCv2EM_XY && defined __FXAPI__

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM 

void _DSPLIB_XYCC dsp_dot_prod_q15(const __xy q15_t* __restrict pSrcA, const __xy q15_t* __restrict pSrcB, uint32_t nSamples, q63_t* result)
{
    int32_t i;
    q63_t tempResult = 0;
	_DSPLIB_store_regs(2, 2, 0);

    _set_agu_ap0(pSrcA); // Set AP registers for pSrcA
    _set_agu_ap1(pSrcB); // Set AP registers for pSrcB

    _set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2));
    _set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_INC_q15(2));

    while ((int32_t)nSamples > 0) 
    {
#define BLOCK_SIZE_WO_SATURATION 128
        int32_t nBlockSamples = dspfx_min_q31(BLOCK_SIZE_WO_SATURATION, nSamples);
        int32_t nBlockSamplePairs = nBlockSamples >> 1;
        accum40_t acc40 = fx_a40_cast_q31(0);
        for (i = 0; i < nBlockSamplePairs; i++) {
            acc40 = fx_a40_dmac_v2q15(acc40, fx_v2q15_cast_q31(_agu_u0()), fx_v2q15_cast_q31(_agu_u1()));
        }
        // Next block could be accessed on last frame only
        if (nBlockSamples & 1) {
            _set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(1));
            _set_agu_mod1(1, AGU_DT_q15, AGU_AD_INC_q15(1));
            acc40 = fx_a40_mac_q15(acc40, _agu_u0(), _agu_u1());
        }
#if defined __Xdsp_postshift_mode
        tempResult += ((q63_t)fx_extract_hi_a40(acc40) << 32) | 
            ((q63_t)fx_extract_lo_a40(acc40));
#else
        //preshift mode
        tempResult += ((q63_t)fx_extract_hi_a40(acc40) << 31) | 
                      ((q63_t)fx_extract_lo_a40(acc40)>>1);
#endif
        nSamples -= nBlockSamples;
    }
    *result = tempResult;

	_DSPLIB_restore_regs();
}

#else

void _DSPLIB_XYCC dsp_dot_prod_q15(const __xy q15_t* __restrict pSrcA, const __xy q15_t* __restrict pSrcB, uint32_t nSamples, q63_t* result)
{
    int32_t i;
    q63_t tempResult = 0;
    __xy v2q15_t *pvSrcA = (__xy v2q15_t*)pSrcA;
    __xy v2q15_t *pvSrcB = (__xy v2q15_t*)pSrcB;

    while ((int32_t)nSamples > 0)
    {
#define BLOCK_SIZE_WO_SATURATION 128
        int32_t nBlockSamples = dspfx_min_q31(BLOCK_SIZE_WO_SATURATION, nSamples);
        int32_t nBlockSamplePairs = nBlockSamples >> 1;
        accum40_t acc40 = fx_a40_cast_q31(0);
        for (i = 0; i < nBlockSamplePairs; i++) {
            acc40 = fx_a40_dmac_v2q15(acc40, *pvSrcA++, *pvSrcB++);
        }
        // Next block could be accessed on last frame only
        if (nBlockSamples & 1) {
            acc40 = fx_a40_mac_q15(acc40, *((__xy q15_t*) pvSrcA), *((__xy q15_t*) pvSrcB));
        }
#if defined __Xdsp_postshift_mode
        tempResult += ((q63_t)fx_extract_hi_a40(acc40) << 32) | 
            ((q63_t)fx_extract_lo_a40(acc40));
#else
        //preshift mode
        tempResult += ((q63_t)fx_extract_hi_a40(acc40) << 31) | 
            ((q63_t)fx_extract_lo_a40(acc40)>>1);
#endif
        nSamples -= nBlockSamples;
    }
    *result = tempResult;
}

#endif

#else //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

// primitives based version of dot product
static _Inline void risc_dsp_dot_prod_q15(const q15_t* __restrict pSrcA, const q15_t* __restrict pSrcB, uint32_t nSamples, q63_t* result)
{
    uint32_t sample;
#if defined(dspfx_v2a40mac_v2q15)
    // branch that calculates dot product with vector instruction
    // using vector instruction is effective in only case when both input and
    // output data pointers are equally aligned or unaligned within 4 byte boundary
    //
    // example 1: pSrcA = 0x00001000 pSrcB = 0x00002000 => vectorization is effective
    // example 2: pSrcA = 0x00001002 pSrcB = 0x00002002 => vectorization is effective
    // example 3: pSrcA = 0x00001000 pSrcB = 0x00002002 => vectorization is not effective

    int32_t  FirstSampleOffset = (((uint32_t)pSrcA) & 0x3) >> 1;

#if !defined(__Xunaligned)
    // check for equal (un)alignment
    int32_t  FirstSampleOffsetB = (((uint32_t)pSrcB) & 0x3) >> 1;
	INIT_MACMODE();
    if (FirstSampleOffset != FirstSampleOffsetB) {
        // branch for input and output are not 'equally' (un)aligned

        dspfx_accum32()
        dspfx_a32mul_q15(0, 0, acc);

        for (sample = 0; sample < nSamples; sample++) {
            //dspfx_v2a40mac_v2q15(dspfx_create0_v2q15(*pSrcA++),dspfx_create0_v2q15(*pSrcB++));
            dspfx_a32mac_q15(*pSrcA++, *pSrcB++, acc);
        }
        *result = dspfx_q63_get_cast_a32(acc);
    }
    else
#endif
    {
		dspfx_v2accum40()
        //branch that uses vector instruction
        q31_t* __restrict pSrcA_32;
        q31_t* __restrict pSrcB_32;
        uint32_t N;

        if (FirstSampleOffset) {
            // process first sample if it is needed
            dspfx_v2a40mul_v2q15(dspfx_create0_v2q15(*pSrcA),dspfx_create0_v2q15(*pSrcB));
        } else {
            // zero accumulator otherwise
            dspfx_v2a40mul_v2q15(dspfx_v2q15_cast_q31(0), dspfx_v2q15_cast_q31(0));
        }

        pSrcA_32 = (int32_t*)(void*)(&pSrcA[FirstSampleOffset]);
        pSrcB_32 = (int32_t*)(void*)(&pSrcB[FirstSampleOffset]);

        // main vector instruction loop
        N = (nSamples - FirstSampleOffset) >> 1;
        for (sample = 0; sample < N; sample++) 
            dspfx_v2a40mac_v2q15(dspfx_v2q15_cast_q31(*pSrcA_32++), dspfx_v2q15_cast_q31(*pSrcB_32++));

        // process last sample if it is needed
        sample = (N << 1) + FirstSampleOffset;
        if (sample < nSamples) //'tail'
            dspfx_v2a40mac_v2q15(dspfx_create0_v2q15(pSrcA[nSamples-1]),dspfx_create0_v2q15(pSrcB[nSamples-1]));

        // extract accumulated results
        *result = dspfx_add_q63(dspfx_q63_get_cast_v2a40(0), dspfx_q63_get_cast_v2a40(1));
    }

#else
    // branch that calculates abs with plain loop
#if defined(__Xmul32x16)
    // ARC600 specific version that utilizes 64bit HW accumulator with intrisics
    _mullw(0,0);
    for (sample = 0; sample < nSamples; sample++){
        int32_t op0 = (int32_t)*pSrcA++;
        int32_t op1 = (int32_t)*pSrcB++;
        _maclw(op0, op1);
        _machlw(op0, op1);
    }
    *result = ((int64_t)_get_ACC1() << 32) | ((uint32_t)_get_ACC2());
#else
    // ANSI C version that utilizes 64bit software accumulator
    q63_t    acc = 0;
    for (sample = 0; sample < nSamples; sample++){
        acc += *pSrcA++ * *pSrcB++;
    }
    *result = acc;
#endif
#endif

}

// threshold for switching between XY and primitives based versions
#define XY_DOT_PROD_THRESHOLD 64

// simple XY memory map for dot product function
#define XY_BLOCK_SIZE ((XY_REGION_SIZE/2)-2) // half of region
#define XYMAP_INPUT0 (0)
#define XYMAP_INPUT1 (0)

/**
 * Computes the Q15 dot product of two vectors.
 * The vectors are multiplied element-by-element and then summed. 
 * The return result is in 34.30 format.
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nSamples
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] result - pointer to output result
 */

void dsp_dot_prod_q15(const q15_t* __restrict pSrcA, const q15_t* __restrict pSrcB, uint32_t nSamples, q63_t* result) 
{
#if defined(_ARC_AS210_mac32) 
   if (nSamples < XY_DOT_PROD_THRESHOLD) {
#endif
       // call to primitives based version of dot product
       risc_dsp_dot_prod_q15(pSrcA, pSrcB, nSamples, result);
#if defined(_ARC_AS210_mac32) 
   } else {
       int32_t lastSize = nSamples;
       int32_t offset = 0;

       // initializing dot product total result variables with zero
       *result = 0;

       // loop that splits vector processing into parts which can fit into XY memory
       // number of iterations depends on XY size and vector length
       while (lastSize) 
       {
           int64_t local_result;
           // define block size to process inside XY memory
           int32_t currentSize = dspfx_min_q31(XY_BLOCK_SIZE, lastSize);

           // load XY content
           int32_t pxy_input0 = memcpy_StoX_int16(XYMAP_INPUT0, pSrcA + offset, currentSize);
           int32_t pxy_input1 = memcpy_StoY_int16(XYMAP_INPUT1, pSrcB + offset, currentSize);

           // call to XY based version of dot product
           dsp_dot_prod_q15_XY_MAC32x32(pxy_input0, pxy_input1, currentSize, &local_result);

           // add intermediate summ result to total result variables
           *result += local_result;
           offset += currentSize;
           lastSize -= currentSize;
       };
   }
#endif
}

#endif //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

#endif //!defined(EXCLUDE_DSP_DOT_PROD_Q15)
