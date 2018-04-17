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

#if !defined(EXCLUDE_DSP_CMPL_DOT_PROD_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"


#if !defined(DSP_MAC_Q31_PRESENT)
#define dspfx_accum8q24(acc) acc
#define dspfx_reset_8q24(acc) acc = 0
#endif

#pragma Code("dsplib_dsp_cmpl_dot_prod_q15")
#if defined (_ARCv2EM_XY_AGU_SMALL) && defined (__Xdsp)

#if defined _XY_HAND_OPT

static _Inline void risc_dsp_cmpl_dot_prod_q15(const q15_t* __restrict pSrcA, const q15_t* __restrict pSrcB, uint32_t nSamples, int32_t* realResult, int32_t* imagResult)
{
    int32_t i;
    _DSPLIB_store_regs(2, 2, 0);

	_set_agu_ap0((uint32_t )pSrcA);
	_set_agu_ap1((uint32_t )pSrcB);
	_set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // pSrcA increment
	_set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // pSrcB increment

    // q.31 arithmetic is converted to q8.24 outside loop
	// Reset the real and image parts of accumulator
    dspfx_v2accum32();
    dspfx_v2a32_reset();
    for (i = 0; i < (int32_t)nSamples; i++)
    {
    	dspfx_v2a32cjmac_cq15(fx_cq15_cast_q31(_agu_u0()), fx_cq15_cast_q31(_agu_u1())); // Complex conjugated MAC
    }

    *realResult = dspfx_q31_get_cast_asr_v2a32(0, 7); //fx_q31_cast_asr_rnd_a40(fx_get_v2a40(acc, 0), 7);
    *imagResult = dspfx_q31_get_cast_asr_v2a32(1, 7); //fx_q31_cast_asr_rnd_a40(fx_get_v2a40(acc, 1), 7);

    _DSPLIB_restore_regs();
}

#else

static _Inline void risc_dsp_cmpl_dot_prod_q15(const q15_t* __restrict pSrcA, const q15_t* __restrict pSrcB, uint32_t nSamples, int32_t* realResult, int32_t* imagResult)
{
    int32_t i;
    // fxapi primitives based branch for computation with complex instructions
    __xy cq15_t * __restrict pSrcAc2q15 = (__xy cq15_t*) pSrcA;
    __xy cq15_t * __restrict pSrcBc2q15 = (__xy cq15_t*) pSrcB;

    // branch that uses accumulator with guard bits
    // q.31 arithmetic is converted to q8.24 outside loop
    v2accum40_t acc = fx_v2a40_cast_v2q15(fx_create_v2q15(0, 0));
    for (i=0; i < (int32_t)nSamples; i++)
    {
        acc = fx_v2a40_cjmac_cq15(acc, *pSrcAc2q15++, *pSrcBc2q15++);
    }
    *realResult = fx_q31_cast_asr_rnd_a40(fx_get_v2a40(acc, 0), 7);
    *imagResult = fx_q31_cast_asr_rnd_a40(fx_get_v2a40(acc, 1), 7);
}

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
// primitives based version of dot product
static _Inline void risc_dsp_cmpl_dot_prod_q15(const q15_t* __restrict pSrcA, const q15_t* __restrict pSrcB, uint32_t nSamples, int32_t* realResult, int32_t* imagResult)
{
    int32_t i;
#if defined(__FXAPI__) && defined(__Xdsp_complex)
    // fxapi primitives based branch for computation with complex instructions
    cq15_t * __restrict pSrcAc2q15 = (cq15_t*) pSrcA;
    cq15_t * __restrict pSrcBc2q15 = (cq15_t*) pSrcB;


#if defined(__GUARD_ACCUM__)
    // branch that uses accumulator with guard bits
    // q.31 arithmetic is converted to q8.24 outside loop
    v2accum40_t acc = fx_v2a40_cast_v2q15(fx_create_v2q15(0, 0));
    for (i=0; i < (int32_t)nSamples; i++)
    {
        acc = fx_v2a40_cjmac_cq15(acc, *pSrcAc2q15++, *pSrcBc2q15++);
    }
    *realResult = fx_q31_cast_asr_rnd_a40(fx_get_v2a40(acc, 0), 7);
    *imagResult = fx_q31_cast_asr_rnd_a40(fx_get_v2a40(acc, 1), 7);
#else
    // branch for accumulator without guard bits.
    // q.31 arithmetic is converted to q8.24 inside loop
    q31_t acc_re = 0;
    q31_t acc_im = 0;
    for (i=0; i < (int32_t)nSamples; i++)
    {
        v2accum32_t acc =  fx_v2a32_cjmpy_cq15(*pSrcAc2q15++, *pSrcBc2q15++);
        acc_re += fx_q31_cast_asr_rnd_a32(fx_get_v2a32(acc, 0), 7);
        acc_im += fx_q31_cast_asr_rnd_a32(fx_get_v2a32(acc, 1), 7);
    }
    *realResult = acc_re;
    *imagResult = acc_im;
#endif
#else
    // dsplib primitives based branch
    const q15_t* __restrict _pSrcA = pSrcA;
    const q15_t* __restrict _pSrcB = pSrcB;
    dspfx_accum8q24(q31_t acc);

    // calculate real part of dot product
    dspfx_reset_8q24(acc);
    for (i=0; i < (int32_t)nSamples; i++)
    {
        dspfx_8q24_mac_q15(acc, *_pSrcA++, *_pSrcB++);
        dspfx_8q24_mac_q15(acc, *_pSrcA++, *_pSrcB++);
    }
    *realResult=dspfx_i32_cast_8q24(acc);

    // reinitialize input data pointers
    _pSrcA = pSrcA;
    _pSrcB = pSrcB;

    // calculate image part of dot product
    dspfx_reset_8q24(acc);
    for (i=0; i < (int32_t)nSamples; i++)
    {
        q15_t reA = *_pSrcA++;
        q15_t imA = *_pSrcA++;
        dspfx_8q24_mac_q15(acc, *_pSrcB++, imA);
        dspfx_8q24_msu_q15(acc, *_pSrcB++, reA);
    }
    *imagResult=dspfx_i32_cast_8q24(acc);
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
 * Dot product of two Q15 complex vectors. Output data format is 8.24
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nSamples
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] realResult - pointer to output result real part
 * \param[out] imagResult - pointer to output result image part
 */

void _DSPLIB_XYCC dsp_cmpl_dot_prod_q15(const PTR(q15_t) __restrict pSrcA, const PTR(q15_t) __restrict pSrcB, uint32_t nSamples, int32_t* realResult, int32_t* imagResult)
{
#if defined(_ARC_AS210_mac16)
    if (nSamples < XY_COMPL_DOT_PROD_THRESHOLD) {
#endif
        // call to primitives based version of dot product
        risc_dsp_cmpl_dot_prod_q15(pSrcA, pSrcB, nSamples, realResult, imagResult);
#if defined(_ARC_AS210_mac16) 
    } else {
        int32_t IntsRemain = nSamples;
        int32_t offset = 0;

        // initializing dot product total result variables with zero
        *realResult = 0;
        *imagResult = 0;

        // loop that splits vector processing into parts which can fit into XY memory
        // number of iterations depends on XY size and vector length
        while (IntsRemain)
        {
            int32_t realTemp, imagTemp;
            // define block size to process inside XY memory
            int32_t IntsCurrent = dspfx_min_q31(IntsRemain, XY_BLOCK_SIZE/2);

            // load XY content
            memcpy_StoX_int32(XYMAP_INPUT,               (signed long *)(void *)pSrcA + offset,   IntsCurrent);
            memcpy_StoX_int32(XYMAP_INPUT + IntsCurrent, (signed long *)(void *)pSrcB + offset,   IntsCurrent);

            // call to XY based version of dot product
            dsp_cmpl_dot_prod_q15_XX_DMAC16x16(XYMAP_INPUT, XYMAP_INPUT + IntsCurrent*2, IntsCurrent, &realTemp, &imagTemp);  

            // add intermediate summ result to total result variables
            *realResult += realTemp;
            *imagResult += imagTemp;
            offset += IntsCurrent;
            IntsRemain -= IntsCurrent;
        };
    }
#endif
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_CMPL_DOT_PROD_Q15)
