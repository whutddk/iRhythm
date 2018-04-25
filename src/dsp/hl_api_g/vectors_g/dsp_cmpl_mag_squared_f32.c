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

#if !defined(EXCLUDE_DSP_CMPL_MAG_SQUARED_F32)

#include "dsplib_types.h"
#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_cmpl_mag_squared_f32")
/**
 * Computes the F32 squared magnitude of complex numbers stored in a vector                      
 * The return result is in f32 format.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */
#ifdef _ARCv2EM_XY

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_SMALL
void _DSPLIB_XYCC dsp_cmpl_mag_squared_f32(const __xy f32_t* __restrict pSrc,  __xy f32_t* __restrict pDst,  uint32_t nSamples)
{
    uint32_t i;
    f32_t acc;
	_DSPLIB_store_regs(3, 3, 0);
    _set_agu_ap0((uint32_t)pDst);
    _set_agu_ap1((uint32_t)pSrc);
    _set_agu_ap2((uint32_t)pSrc);
    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_ui32(1)); // output increment
    _set_agu_mod1(1, AGU_DT_ui32, AGU_AD_INC_ui32(1)); // input increment
    _set_agu_mod2(2, AGU_DT_ui32, AGU_AD_INC_ui32(1)); // input increment
    for (i=0; i < nSamples; i++)
    {
        acc = 0;
        acc += (_as_float(_agu_u1())) * (_as_float(_agu_u2()));
        acc += (_as_float(_agu_u1())) * (_as_float(_agu_u2()));
        _st_agu_u0(_as_uint32(acc));
    }
    _DSPLIB_restore_regs();
};

#else

void _DSPLIB_XYCC dsp_cmpl_mag_squared_f32(const __xy f32_t* __restrict pSrc,  __xy f32_t* __restrict pDst,  uint32_t nSamples)
{
    uint32_t i;
    for (i=0; i < nSamples; i++)
    {
        f32_t re = *pSrc++;
        f32_t im = *pSrc++;
        *pDst++ = re*re + im*im;
    }
};

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
void dsp_cmpl_mag_squared_f32(const f32_t* __restrict pSrc,  f32_t* __restrict pDst,  uint32_t nSamples)
{
    uint32_t i;
    for (i=0; i < nSamples; i++)
    {
        f32_t re = *pSrc++;
        f32_t im = *pSrc++;
        *pDst++ = re*re + im*im;
    }
};
#endif //_ARCv2EM_XY_AGU_MEDIUM
#pragma Code()

#endif //!defined(EXCLUDE_DSP_CMPL_MAG_SQUARED_F32)
