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

#if !defined(EXCLUDE_DSP_SCALE_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsplib_helper.h"

#pragma Code("dsplib_dsp_scale_q15")

#if defined _ARCv2EM_XY && defined __FXAPI__

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_SMALL

void _DSPLIB_XYCC dsp_scale_q15(const __xy q15_t* __restrict pSrc, q15_t   scaleFract, int8_t  shift, __xy q15_t* __restrict pDst, uint32_t nSamples)
{
    int32_t i, N  = nSamples >> 1;
    v2q15_t v2q15_scaleFract = fx_create_v2q15(scaleFract, scaleFract);
    //v2i16_t v2i16_shift = fx_create_v2i16(shift, shift);
	_DSPLIB_store_regs(2, 2, 0);

    _set_agu_ap0(pSrc); // Set AP registers for pSrcA
    _set_agu_ap1(pDst); // Set AP registers for pSrcB

    _set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2));
    _set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_INC_q15(2));

    for (i = 0; i < N; i++) {
        _st_agu_u1(fx_q31_cast_v2q15(fx_v2q15_cast_asl_rnd_v2a40(fx_v2a40_mpy_v2q15(fx_v2q15_cast_q31(_agu_u0()), v2q15_scaleFract), shift)));
    }
    if (nSamples & 1) {
        _set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(1));
        _set_agu_mod1(1, AGU_DT_q15, AGU_AD_INC_q15(1));
        _st_agu_u1(fx_q31_cast_v2q15(fx_v2q15_cast_asl_rnd_v2a40(fx_v2a40_mpy_v2q15(fx_v2q15_cast_q31(_agu_u0()), v2q15_scaleFract), shift)));
    }

	_DSPLIB_restore_regs();
}

#else

void _DSPLIB_XYCC dsp_scale_q15(const __xy q15_t* __restrict pSrc, q15_t   scaleFract, int8_t  shift, __xy q15_t* __restrict pDst, uint32_t nSamples)
{
    int32_t i;
    __xy v2q15_t *pvSrc, *pvDst;
    v2q15_t v2q15_scaleFract = fx_create_v2q15(scaleFract, scaleFract);

    if (nSamples & 1) {
    	*pDst++ = fx_q15_cast_asl_rnd_a40(fx_a40_mpy_q15(*pSrc++,scaleFract), shift);
    }
    pvSrc = (__xy v2q15_t*) pSrc;
    pvDst = (__xy v2q15_t*) pDst;
    for (i = 0; i < (nSamples >> 1); i++) {
        *pvDst++ = fx_v2q15_cast_asl_rnd_v2a40(fx_v2a40_mpy_v2q15(*pvSrc++, v2q15_scaleFract), shift);
    }
}

#endif

#else //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

// primitives based version of vector scale
static _Inline void risc_dsp_scale_q15(const q15_t* __restrict pSrc, q15_t   scaleFract, int8_t  shift, q15_t* __restrict pDst, uint32_t nSamples)
{
    int32_t sample;
#if !defined(dspfx_mpy_rnd_asl_v2q15)
    // branch that calculates vector scale with plain loop
    INIT_MACMODE();
    for (sample = 0; sample < (int32_t)nSamples; sample++){
        dspfx_accum32();
        dspfx_a32mul_q15(*pSrc++, scaleFract, acc);
        *pDst++ = dspfx_q15_cast_asl_a32(acc, shift);
    }
#else
    // branch that calculates vector scale with vector instruction
    // using vector instruction is effective in only case when both input and
    // output data pointers are equally aligned or unaligned within 4 byte boundary
    //
    // example 1: pSrc = 0x00001000 pDst = 0x00002000 => vectorization is effective
    // example 2: pSrc = 0x00001002 pDst = 0x00002002 => vectorization is effective
    // example 3: pSrc = 0x00001000 pDst = 0x00002002 => vectorization is not effective

    int32_t  startSample = (((uint32_t)pSrc) & 0x3) >> 1;
    INIT_MACMODE();
    if (nSamples == 0) return;
#if !defined(__Xunaligned)
    // check for equal (un)alignment
    if (startSample != (((uint32_t)pDst & 0x3)>>1))
    {
        // branch for input and output are not 'equally' (un)aligned
        for (sample = 0; sample < (int32_t)nSamples; sample++) 
        {
            dspfx_accum32();
            dspfx_a32mul_q15(*pSrc++, scaleFract, acc);
            *pDst++ = dspfx_q15_cast_asl_a32(acc, shift);
        }
    }
    else 
#endif
    {
        //branch that uses vector instruction
        q31_t scaleFract_32;
        q31_t* __restrict pDst_32;
        q31_t* __restrict pSrc_32;
        int32_t N;
        
        // process first sample if it is needed
        if (startSample) 
        {
            dspfx_accum32();
            dspfx_a32mul_q15(pSrc[0], scaleFract, acc);
            pDst[0] = dspfx_q15_cast_asl_a32(acc, shift);
        }
        
        pDst_32 = (int32_t*)(void*)(&pDst[startSample]);
        pSrc_32 = (int32_t*)(void*)(&pSrc[startSample]);
        
        scaleFract_32 = ((int32_t)scaleFract & 0xffff) + (((int32_t)scaleFract<<16) & 0xffff0000);
        
        // main vector instruction loop
        N = (nSamples - startSample) >> 1;
        for (sample = 0; sample < N; sample++) {
            *pDst_32++ = dspfx_mpy_rnd_asl_v2q15(*pSrc_32++, scaleFract_32, shift);
        }
        
        // process last sample if it is needed
        sample = (N << 1) + startSample;
        if (sample < (int32_t)nSamples){
            dspfx_accum32();
            dspfx_a32mul_q15(pSrc[nSamples - 1], scaleFract, acc);
            pDst[nSamples - 1] = dspfx_q15_cast_asl_a32(acc, shift);
        }
        
    }
#endif
}

// This is empirical define. It can be computed with memory_latency value to be more effective.
#define XY_SCALE_THRESHOLD 32

// simple XY memory map for abs function
#define XY_BLOCK_SIZE ((XY_REGION_SIZE/2)-2)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Q15 multiply a vector by a scalar value.
 * The algorithm used is:
 * pDst[n] = (pSrc[n] * scaleFract) << shift,   0 <= n < nSamples. 
 * The overall scale factor applied to the fixed-point data is:
 * scale = scaleFract * 2^shift.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param scaleFract
 * \param shift
 * \param nSamples - number of samples to be processed
 */

void dsp_scale_q15(const q15_t* __restrict pSrc, q15_t   scaleFract, int8_t  shift, q15_t* __restrict pDst, uint32_t nSamples)
{
#if defined(_ARC_AS210_mac16)
    if (nSamples < XY_SCALE_THRESHOLD) {
#endif
        // call to primitives based version of scale
        risc_dsp_scale_q15(pSrc, scaleFract, shift, pDst, nSamples);
#if defined(_ARC_AS210_mac16)
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
            int32_t pxy_input = memcpy_StoX_int16(XYMAP_INPUT, pSrc + offset, currentSize);

            // call to XY based version of scale
            dsp_scale_q15_XtoY_MAC16x16(pxy_input, scaleFract, shift, XYMAP_OUTPUT, currentSize);

            // extract result from XY memory
            memcpy_YtoS_int16(pDst + offset, XYMAP_OUTPUT, currentSize);
            offset += currentSize;
            lastSize -= currentSize;
        };
    }
#endif
}

#endif //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

#endif //!defined(EXCLUDE_DSP_SCALE_Q15)
