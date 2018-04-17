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

#if !defined(EXCLUDE_DSP_CMPL_CONJ_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_nomac)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

#pragma Code("dsplib_dsp_cmpl_conj_q31")
#ifdef _ARCv2EM_XY_AGU_MEDIUM

#if defined _XY_HAND_OPT

static _Inline void risc_dsp_cmpl_conj_q31(const q31_t *  pSrc, q31_t *  pDst, uint32_t nSamples)
{
    uint32_t i;
	_DSPLIB_store_regs(2, 2, 0);
    _set_agu_ap0((uint32_t)pDst);
    _set_agu_ap1((uint32_t)pSrc);
    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1)); // input increment
    _set_agu_mod1(1, AGU_DT_q31, AGU_AD_INC_q31(1)); // output increment
    for ( i =0; i<nSamples; i++)
    {
    	_st_agu_u0(_agu_u1());
    	_st_agu_u0(dspfx_neg_q31(_agu_u1()));
    }
    _DSPLIB_restore_regs();
}

#else

static _Inline void risc_dsp_cmpl_conj_q31(const __xy q31_t * __restrict pSrc, __xy q31_t * __restrict pDst, uint32_t nSamples)
{
    uint32_t i;
    for ( i =0; i<nSamples; i++)
    {
        *pDst++ = *pSrc++;
        *pDst++ = dspfx_neg_q31(*pSrc++);
    }
}

#endif


#else //_ARCv2EM_XY_AGU_MEDIUM
// primitives based version of complex conjugate
static _Inline void risc_dsp_cmpl_conj_q31(const q31_t * __restrict pSrc, q31_t * __restrict pDst, uint32_t nSamples)
{
    uint32_t i;
    for ( i =0; i<nSamples; i++)
    {
        *pDst++ = *pSrc++;
        *pDst++ = dspfx_neg_q31(*pSrc++);
    }
}
#endif //_ARCv2EM_XY_AGU_MEDIUM

// threshold for switching between XY and primitives based versions
#define XY_COMPL_CONJ_THRESHOLD 32

// simple XY memory map for dot product function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/4)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Q31 complex conjugate function
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_cmpl_conj_q31(const PTR(q31_t) __restrict pSrc, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
#if defined(_ARC_AS210_nomac)
    if (nSamples < XY_COMPL_CONJ_THRESHOLD) {
#endif
        // call to primitives based version of complex conjugate
        risc_dsp_cmpl_conj_q31(pSrc, pDst, nSamples);
#if defined(_ARC_AS210_nomac) 
    } else {
        int32_t IntsRemain = nSamples * 2;
        int32_t offset = 0;

        // loop that splits vector processing into parts which can fit into XY memory
        // number of iterations depends on XY size and vector length
        while (IntsRemain)
        {
            // define block size to process inside XY memory
            int32_t IntsCurrent = dspfx_min_q31(IntsRemain, XY_BLOCK_SIZE);

            // load XY content
            memcpy_StoX_int32(XYMAP_INPUT, (signed long *)(void *)pSrc + offset, IntsCurrent);

            // call to XY based version of complex conjugate
            dsp_cmpl_conj_q31_XtoY(XYMAP_INPUT, XYMAP_OUTPUT, IntsCurrent>>1);

            // extract result from XY memory
            memcpy_YtoS_int32((signed long *)(void *)pDst + offset, XYMAP_OUTPUT, IntsCurrent);
            offset += IntsCurrent;
            IntsRemain -= IntsCurrent;
        };  
    }
#endif
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_CMPL_CONJ_Q31)
