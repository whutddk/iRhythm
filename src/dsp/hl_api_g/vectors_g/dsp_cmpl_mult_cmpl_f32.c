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

#if !defined(EXCLUDE_DSP_CMPL_MULT_CMPL_F32)

#include "dsplib_types.h"
#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_cmpl_mult_cmpl_f32")
/**
 * Computes the f32 complex multiplication of two complex numbers stored in a vector respectively
 * The return result is in f32 format.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */
#ifdef _ARCv2EM_XY

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM

void _DSPLIB_XYCC dsp_cmpl_mult_cmpl_f32(const __xy f32_t* __restrict pSrcA, const __xy f32_t* __restrict pSrcB, __xy f32_t* __restrict pDst, uint32_t nSamples)
{
    uint32_t i;
    f32_t acc;
	_DSPLIB_store_regs(3, 5, 0);
    _set_agu_ap0((uint32_t)pDst);
    _set_agu_ap1((uint32_t)pSrcA);
    _set_agu_ap2((uint32_t)pSrcB);
    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_ui32(2)); // pDst increment by 2
    _set_agu_mod1(1, AGU_DT_ui32, AGU_AD_INC_ui32(1)); // pSrcA increment
    _set_agu_mod2(2, AGU_DT_ui32, AGU_AD_INC_ui32(1)); // pSrcB  increment
    _set_agu_mod3(2, AGU_DT_ui32, AGU_AD_DEC_ui32(1)); // pSrcB  decrement
    _set_agu_mod4(2, AGU_DT_ui32, AGU_AD_INC_ui32(3)); // pSrcB  increment by 3

	// Real part
	for (i = 0; i < nSamples; i++) {
		acc  = _as_float(_agu_u1()) * _as_float(_agu_u2());  // a.re*b.re
		acc -= _as_float(_agu_u1()) * _as_float(_agu_u2());  //-a.im*b.im
		_st_agu_u0(_as_uint32(acc));
	}

    // Imaginary part
    _set_agu_ap0((uint32_t)(pDst  + 1)); // first imaginary output sample
    _set_agu_ap1((uint32_t) pSrcA     ); // first real      pSrcA  sample
    _set_agu_ap2((uint32_t)(pSrcB + 1)); // first imaginary pSrcB  sample
    for ( i =0; i<nSamples; i++)
    {
		acc  = _as_float(_agu_u1()) * _as_float(_agu_u3());  // a.re*b.im
		acc += _as_float(_agu_u1()) * _as_float(_agu_u4());  //+a.im*b.re
		_st_agu_u0(_as_uint32(acc));
	}
    _DSPLIB_restore_regs();
}

#else

void dsp_cmpl_mult_cmpl_f32(const __xy f32_t* __restrict pSrcA, const __xy f32_t* __restrict pSrcB, __xy f32_t* __restrict pDst, uint32_t nSamples)
{
    uint32_t i;
    float acc;
    __xy float* _pSrcA = (__xy float*) pSrcA;
    __xy float* _pSrcB = (__xy float*) pSrcB;
    __xy float* _pDst = (__xy float*) pDst;
    for (i=0; i < nSamples; i++)
    {
        acc = 0;
        MAC_F32(acc, *_pSrcA++,  *_pSrcB++);
        MAC_F32(acc, *_pSrcA++, -*_pSrcB++);
        *_pDst = acc; _pDst += 2;
    }
    _pSrcA = (__xy float*) pSrcA;
    _pSrcB = (__xy float*) pSrcB + 1;
    _pDst = (__xy float*) pDst + 1;
    for (i=0; i < nSamples; i++)
    {
        acc = 0;
        MAC_F32(acc, *_pSrcA++, *_pSrcB--);
        MAC_F32(acc, *_pSrcA++, *_pSrcB); _pSrcB += 3;
        *_pDst = acc; _pDst += 2;
    }
};
#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
void dsp_cmpl_mult_cmpl_f32(const f32_t* __restrict pSrcA, const f32_t* __restrict pSrcB, f32_t* __restrict pDst, uint32_t nSamples)
{
    uint32_t i;
    float acc;
    nSamples = nSamples << 1;
    for (i=0; i < nSamples; i+=2)
    {
        acc = 0;
        MAC_F32(acc, pSrcA[i], pSrcB[i]);
        MAC_F32(acc, pSrcA[i+1], -pSrcB[i+1]);
        pDst[i] = acc;
        acc = 0;
        MAC_F32(acc, pSrcA[i], pSrcB[i+1]);
        MAC_F32(acc, pSrcA[i+1], pSrcB[i]);
        pDst[i+1] = acc; 
    }
};
#endif //_ARCv2EM_XY_AGU_MEDIUM
#pragma Code()

#endif //!defined(EXCLUDE_DSP_CMPL_MULT_CMPL_F32)
