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

#if !defined(EXCLUDE_DSP_CMPL_MULT_REAL_F32)

#include "dsplib_types.h"
#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_cmpl_mult_real_f32")
/**
 * Computes the f32 complex multiplication of complex and real numbers stored in a vector respectively
 * The return result is in f32 format.
 *
 * \param[in] pSrcCmplx
 * \param[in] pSrcReal
 * \param[out] pCmplxDst
 * \param nSamples - number of samples to be processed
 */
#ifdef _ARCv2EM_XY

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM

void _DSPLIB_XYCC dsp_cmpl_mult_real_f32(const __xy f32_t* __restrict pSrcCmplx, const __xy f32_t* __restrict pSrcReal, __xy f32_t* __restrict pCmplxDst, uint32_t nSamples)
{
    uint32_t i;
	_DSPLIB_store_regs(3, 4, 0);
    _set_agu_ap0((uint32_t)pCmplxDst);
    _set_agu_ap1((uint32_t)pSrcCmplx);
    _set_agu_ap2((uint32_t)pSrcReal);
    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1)); // pCmplxDst increment
    _set_agu_mod1(1, AGU_DT_q31, AGU_AD_INC_q31(1)); // pSrcCmplx increment
    _set_agu_mod2(2, AGU_DT_q31, AGU_AD_INC_q31(1)); // pSrcReal  increment
    _set_agu_mod3(2, AGU_DT_q31, AGU_AD_INC_q31(0)); // pSrcReal  non-increment
    for ( i =0; i<nSamples; i++)
    {
    	_st_agu_u0(_as_uint32(_as_float(_agu_u1()) * _as_float(_agu_u3())));
    	_st_agu_u0(_as_uint32(_as_float(_agu_u1()) * _as_float(_agu_u2())));
    }
    _DSPLIB_restore_regs();
}

#else

void _DSPLIB_XYCC dsp_cmpl_mult_real_f32(const __xy f32_t* __restrict pSrcCmplx, const __xy f32_t* __restrict pSrcReal, __xy f32_t* __restrict pCmplxDst, uint32_t nSamples)
{
    uint32_t i;
    for (i = 0; i<nSamples; i++)
    {
        *pCmplxDst++ = *pSrcCmplx++*(*pSrcReal);
        *pCmplxDst++ = *pSrcCmplx++*(*pSrcReal);
        pSrcReal++;
    }
};

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
void dsp_cmpl_mult_real_f32(const f32_t* __restrict pSrcCmplx, const f32_t* __restrict pSrcReal, f32_t* __restrict pCmplxDst, uint32_t nSamples)
{
    uint32_t i;
    nSamples = nSamples << 1;
    for (i = 0; i<nSamples; i+=2)
    {
        pCmplxDst[i] = pSrcCmplx[i]*(*pSrcReal);
        pCmplxDst[i+1] = pSrcCmplx[i+1]*(*pSrcReal);
        pSrcReal++;
    }
};
#endif //_ARCv2EM_XY_AGU_MEDIUM
#pragma Code()

#endif //!defined(EXCLUDE_DSP_CMPL_MULT_REAL_F32)
