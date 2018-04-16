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

#if !defined(EXCLUDE_DSP_DOT_PROD_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32) 
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

#define dspfx_15q48_mac_q31(acc, a, b) (acc += (dspfx_q63_mpy_asr_q31(a, b, 14)))

#pragma Code("dsplib_dsp_dot_prod_q31")

#if defined _ARCv2EM_XY && defined __FXAPI__

/**
 * Computes the Q31 dot product of two vectors.
 * The vectors are multiplied element-by-element and then summed. 
 * The return result is in 16.48 format.
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nSamples
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] result - pointer to output result
 */

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM

void _DSPLIB_XYCC dsp_dot_prod_q31(const __xy q31_t* __restrict pSrcA, const __xy q31_t* __restrict pSrcB, uint32_t nSamples, q63_t* result)
{
    int32_t i;
    q63_t tempResult = 0;
	_DSPLIB_store_regs(2, 2, 0);

    _set_agu_ap0(pSrcA); // Set AP registers for pSrcA
    _set_agu_ap1(pSrcB); // Set AP registers for pSrcB

    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod1(1, AGU_DT_q31, AGU_AD_INC_q31(1));

    while ((int32_t)nSamples > 0) 
    {
#define BLOCK_SIZE_WO_SATURATION 256
        int32_t nBlockSamples = dspfx_min_q31(BLOCK_SIZE_WO_SATURATION, nSamples);
        accum72_t acc72 = fx_a72_cast_q31(0);
        for (i = 0; i < nBlockSamples; i++)
        {
            acc72 = fx_a72_mac_q31(acc72, _agu_u0(), _agu_u1());
        }
        tempResult += fx_q63_cast_a72(fx_asr_a72(acc72, 15));
        nSamples -= nBlockSamples;
    }
    *result = tempResult;
	_DSPLIB_restore_regs();
}

#else

// primitives based version of dot product
void _DSPLIB_XYCC dsp_dot_prod_q31(const PTR(q31_t) __restrict pSrcA, const PTR(q31_t) __restrict pSrcB, uint32_t nSamples, q63_t* result)
{
    int32_t i;
    // fxapi primitives version
#define BLOCK_SIZE_WO_SATURATION 256
    q63_t tempResult = 0;
    while ((int32_t)nSamples > 0)
    {
        int32_t nBlockSamples = dspfx_min_q31(BLOCK_SIZE_WO_SATURATION, nSamples);
        accum72_t acc72 = fx_a72_cast_q31(0);
        for (i = 0; i < nBlockSamples; i++)
        {
            acc72 = fx_a72_mac_q31(acc72, *pSrcA++, *pSrcB++);
        }
        tempResult += fx_q63_cast_a72(fx_asr_a72(acc72, 15));
        nSamples -= nBlockSamples;
    }
    *result = tempResult;
}

#endif

#else

// primitives based version of dot product
static _Inline void risc_dsp_dot_prod_q31(const q31_t* __restrict pSrcA, const q31_t* __restrict pSrcB, uint32_t nSamples, q63_t* result)
{
    int32_t i;
#if defined(__FXAPI__) && defined(__GUARD_ACCUM__)
    // fxapi primitives version
#define BLOCK_SIZE_WO_SATURATION 256
    q63_t tempResult = 0;
    while ((int32_t)nSamples > 0) 
    {
        int32_t nBlockSamples = dspfx_min_q31(BLOCK_SIZE_WO_SATURATION, nSamples);
        accum72_t acc72 = fx_a72_cast_q31(0);
        for (i = 0; i < nBlockSamples; i++)
        {
            acc72 = fx_a72_mac_q31(acc72, *pSrcA++, *pSrcB++);
        }
        tempResult += fx_q63_cast_a72(fx_asr_a72(acc72, 15));
        nSamples -= nBlockSamples;
    }
    *result = tempResult;
#else
    // dsplib primitives version
    q63_t acc = 0;
    for (i = 0; i < (int32_t)nSamples; i++){
        dspfx_15q48_mac_q31(acc, *pSrcA++, *pSrcB++);
    }
    *result = acc;
#endif
}

// threshold for switching between XY and primitives based versions
#define XY_DOT_PROD_THRESHOLD 32

// simple XY memory map for dot product function
#define XY_BLOCK_SIZE ((XY_REGION_SIZE/4)) // full region
#define XYMAP_INPUT0 (0)
#define XYMAP_INPUT1 (0)

/**
 * Computes the Q31 dot product of two vectors.
 * The vectors are multiplied element-by-element and then summed. 
 * The return result is in 16.48 format.
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nSamples
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] result - pointer to output result
 */

void dsp_dot_prod_q31(const q31_t* __restrict pSrcA, const q31_t* __restrict pSrcB, uint32_t nSamples, q63_t* result)
{
#if defined(_ARC_AS210_mac32) 
   if (nSamples < XY_DOT_PROD_THRESHOLD) {
#endif
       // call to primitives based version of dot product
       risc_dsp_dot_prod_q31(pSrcA, pSrcB, nSamples, result);
#if defined(_ARC_AS210_mac32) 
   } else {
       int32_t currentSize;
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
           currentSize = dspfx_min_q31(XY_BLOCK_SIZE, lastSize);

           // load XY content
           memcpy_StoX_int32(XYMAP_INPUT0, pSrcA + offset, currentSize);
           memcpy_StoY_int32(XYMAP_INPUT1, pSrcB + offset, currentSize);

           // call to XY based version of dot product
           dsp_dot_prod_q31_XY_MAC32x32(XYMAP_INPUT0, XYMAP_INPUT1, currentSize, &local_result );

           // add intermediate summ result to total result variables
           *result += local_result;
           offset += currentSize;
           lastSize -= currentSize;
       };
   }
#endif
}
#endif

#endif //!defined(EXCLUDE_DSP_DOT_PROD_Q31)
