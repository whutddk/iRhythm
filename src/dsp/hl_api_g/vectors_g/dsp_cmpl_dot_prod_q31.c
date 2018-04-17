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
#include "dsplib_primitives.h"

#if !defined(EXCLUDE_DSP_CMPL_DOT_PROD_Q31)
#pragma Code("dsplib_dsp_cmpl_dot_prod_q31")

#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

// special complex dot product computational primitives
#define dspfx_15q48_mac_q31(acc, a, b) (acc += (dspfx_q63_mpy_asr_q31(a, b, 14)))
#define dspfx_15q48_msu_q31(acc, a, b) (acc -= (dspfx_q63_mpy_asr_q31(a, b, 14)))

#if defined (_ARCv2EM_XY_AGU_SMALL) && defined(__FXAPI__) && defined(__GUARD_ACCUM__) && defined _XY_HAND_OPT

static _Inline void risc_dsp_cmpl_dot_prod_q31(const q31_t* __restrict pSrcA, const q31_t* __restrict pSrcB, uint32_t nSamples, q63_t* realResult, q63_t* imagResult)
{
    int32_t i;

    // Branch with 2 separate loop computing real and image part of dot product result
    // in 72 bit accumulator with 8 guard bits. This branch computes dot product by blocks
    // with length no more than 128 samples in order to eliminate saturations and achieve
    // maximum performance
    accum72_t acc72;
    int64_t tempResult;
    int32_t nSamplesLast = nSamples;
    _DSPLIB_store_regs(2, 3, 0);

#define BLOCK_SIZE_WO_SATURATION 128
    // calculate real part of dot product
    tempResult = 0;
	_set_agu_ap0((uint32_t )pSrcA);
	_set_agu_ap1((uint32_t )pSrcB);
	_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1)); // pSrcA increment
	_set_agu_mod2(1, AGU_DT_q31, AGU_AD_INC_q31(1)); // pSrcB increment
    while (nSamplesLast > 0)
    {
        int32_t nBlockSamples = dspfx_min_q31(BLOCK_SIZE_WO_SATURATION, nSamplesLast);
        acc72 = fx_create_a72(0, 0);
        for (i=0; i < nBlockSamples; i++)
        {
            acc72 = fx_a72_mac_q31(acc72, _agu_u0(), _agu_u2());
            acc72 = fx_a72_mac_q31(acc72, _agu_u0(), _agu_u2());
        }
        tempResult += fx_q63_cast_a72(fx_asr_a72(acc72, 15));
        nSamplesLast -= nBlockSamples;
    }
    *realResult = tempResult;

    // reinitialize input data pointers
    _set_agu_ap0((uint32_t )(pSrcA + 1)); // points to first image element in vector A
    _set_agu_ap1((uint32_t )pSrcB);       // points to first real  element in vector B
    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_DEC_q31(1)); // pSrcA decrement to n-th real element
    _set_agu_mod1(0, AGU_DT_q31, AGU_AD_INC_q31(3)); // pSrcA increment to (n+1)-th image element
    // calculate image part of dot product
    nSamplesLast = nSamples;
    tempResult = 0;
    while (nSamplesLast > 0)
    {
        int32_t nBlockSamples = dspfx_min_q31(BLOCK_SIZE_WO_SATURATION, nSamplesLast);
        acc72 = fx_create_a72(0, 0);
        for (i=0; i < nBlockSamples; i++)
        {
            //q31_t reA = *_pSrcA++;
            //q31_t imA = *_pSrcA++;
            acc72 = fx_a72_mac_q31(acc72, _agu_u2(), _agu_u0());
            acc72 = fx_a72_msu_q31(acc72, _agu_u2(), _agu_u1());
        }
        tempResult += fx_q63_cast_a72(fx_asr_a72(acc72, 15));
        nSamplesLast -= nBlockSamples;
    }
    *imagResult = tempResult;

    _DSPLIB_restore_regs();
}

#else //_ARCv2EM_XY_AGU_MEDIUM
// primitives based version of dot product
static _Inline void risc_dsp_cmpl_dot_prod_q31(const PTR(q31_t) __restrict pSrcA, const PTR(q31_t) __restrict pSrcB, uint32_t nSamples, q63_t* realResult, q63_t* imagResult)
{
    int32_t i;
    const PTR(q31_t) __restrict _pSrcA = (const PTR(q31_t)) pSrcA;
    const PTR(q31_t) __restrict _pSrcB = (const PTR(q31_t)) pSrcB;
#if defined(__FXAPI__) && defined(__GUARD_ACCUM__)
    // Branch with 2 separate loop computing real and image part of dot product result
    // in 72 bit accumulator with 8 guard bits. This branch computes dot product by blocks
    // with length no more than 128 samples in order to eliminate saturations and achieve
    // maximum performance
    accum72_t acc72;
    int64_t tempResult;
    int32_t nSamplesLast = nSamples;

#define BLOCK_SIZE_WO_SATURATION 128
    // calculate real part of dot product
    tempResult = 0;
    while (nSamplesLast > 0)
    {
        int32_t nBlockSamples = dspfx_min_q31(BLOCK_SIZE_WO_SATURATION, nSamplesLast);
        acc72 = fx_create_a72(0, 0);
        for (i=0; i < nBlockSamples; i++)
        {
            acc72 = fx_a72_mac_q31(acc72, *_pSrcA++, *_pSrcB++);
            acc72 = fx_a72_mac_q31(acc72, *_pSrcA++, *_pSrcB++);
        }
        tempResult += fx_q63_cast_a72(fx_asr_a72(acc72, 15));
        nSamplesLast -= nBlockSamples;
    }
    *realResult = tempResult;

    // reinitialize input data pointers
    _pSrcA = (const PTR(q31_t)) pSrcA + 1; // start with image part
    _pSrcB = (const PTR(q31_t)) pSrcB;

    // calculate image part of dot product
    nSamplesLast = nSamples;
    tempResult = 0;
    while (nSamplesLast > 0)
    {
        int32_t nBlockSamples = dspfx_min_q31(BLOCK_SIZE_WO_SATURATION, nSamplesLast);
        acc72 = fx_create_a72(0, 0);
        for (i=0; i < nBlockSamples; i++)
        {
            acc72 = fx_a72_mac_q31(acc72, *_pSrcA--, *_pSrcB++);
            acc72 = fx_a72_msu_q31(acc72, *_pSrcA,   *_pSrcB++); _pSrcA += 3;
        }
        tempResult += fx_q63_cast_a72(fx_asr_a72(acc72, 15));
        nSamplesLast -= nBlockSamples;
    }
    *imagResult = tempResult;
#else
    // branch that utilizes 2 separate accumulators for output result
    q63_t acc_re = 0;
    q63_t acc_im = 0;
    for (i=0; i < (int32_t)nSamples; i++)
    {
        q31_t reA = *_pSrcA++;
        q31_t reB = *_pSrcB++;
        q31_t imA = *_pSrcA++;
        q31_t imB = *_pSrcB++;

        dspfx_15q48_mac_q31(acc_re, reA, reB);
        dspfx_15q48_mac_q31(acc_re, imA, imB);

        dspfx_15q48_mac_q31(acc_im, reB, imA);
        dspfx_15q48_msu_q31(acc_im, imB, reA);
    }
    *realResult = acc_re;
    *imagResult = acc_im;
#endif
}
#endif //_ARCv2EM_XY_AGU_MEDIUM
// threshold for switching between XY and primitives based versions
#define XY_COMPL_DOT_PROD_THRESHOLD 32

// simple XY memory map for dot product function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/4)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Dot product of two Q31 complex vectors. Output data format is 16.48
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nSamples
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] realResult - pointer to output result real part
 * \param[out] imagResult - pointer to output result image part
 */

void _DSPLIB_XYCC dsp_cmpl_dot_prod_q31(const PTR(q31_t) __restrict pSrcA, const PTR(q31_t) __restrict pSrcB, uint32_t nSamples, q63_t* realResult, q63_t* imagResult)
{
#if defined(_ARC_AS210_mac32)
    if (nSamples < XY_COMPL_DOT_PROD_THRESHOLD) {
#endif
        // call to primitives based version of dot product
        risc_dsp_cmpl_dot_prod_q31(pSrcA, pSrcB, nSamples, realResult, imagResult);
#if defined(_ARC_AS210_mac32) 
    } else {
        int32_t IntsRemain = nSamples * 2;
        int32_t offset = 0;

        // initializing dot product total result variables with zero
        *realResult = 0;
        *imagResult = 0;

        // loop that splits vector processing into parts which can fit into XY memory
        // number of iterations depends on XY size and vector length
        while (IntsRemain)
        {
            int64_t realTemp, imagTemp;

            // define block size to process inside XY memory
            int32_t IntsCurrent = dspfx_min_q31(IntsRemain, XY_BLOCK_SIZE/2);

            // load XY content
            memcpy_StoX_int32(XYMAP_INPUT,               (signed long *)(void *)pSrcA + offset,   IntsCurrent);
            memcpy_StoX_int32(XYMAP_INPUT + IntsCurrent, (signed long *)(void *)pSrcB + offset,   IntsCurrent);

            // call to XY based version of dot product
            dsp_cmpl_dot_prod_q31_XX_MAC32x32(XYMAP_INPUT, XYMAP_INPUT + IntsCurrent, IntsCurrent >>1, &realTemp, &imagTemp);        

            // add intermediate summ result to total result variables
            *realResult += realTemp;
            *imagResult += imagTemp;
            offset += IntsCurrent;
            IntsRemain -= IntsCurrent;
        };
    }
#endif
}

#endif //!defined(EXCLUDE_DSP_CMPL_DOT_PROD_Q31)
