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

#if !defined(EXCLUDE_DSP_CMPL_MAG_SQUARED_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

#pragma Code("dsplib_dsp_cmpl_mag_squared_q31")
#ifdef _ARCv2EM_XY_AGU_MEDIUM

#if defined _XY_HAND_OPT

static _Inline void risc_dsp_cmpl_mag_squared_q31(const q31_t*  pSrc,  q31_t*  pDst,  uint32_t nSamples)
{
    uint32_t i;
    dspfx_accum64()
	_DSPLIB_store_regs(3, 3, 0);
    _set_agu_ap0((uint32_t)pDst);
    _set_agu_ap1((uint32_t)pSrc);
    _set_agu_ap2((uint32_t)pSrc);
    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1)); // output increment
    _set_agu_mod1(1, AGU_DT_q31, AGU_AD_INC_q31(1)); // input increment
    _set_agu_mod2(2, AGU_DT_q31, AGU_AD_INC_q31(1)); // input increment
    for (i=0; i < nSamples; i++)
    {
        dspfx_a64mul_nf_q31(_agu_u1(), _agu_u2(), acc);
        dspfx_a64mac_nf_q31(_agu_u1(), _agu_u2(), acc);
        _st_agu_u0(dspfx_q31_cast_asl_a64(acc, -2));
    }
    _DSPLIB_restore_regs();
};

#else

static _Inline void risc_dsp_cmpl_mag_squared_q31(const __xy q31_t* __restrict pSrc, __xy q31_t* __restrict pDst,  uint32_t nSamples)
{
    uint32_t i;
    for (i=0; i < nSamples; i++)
    {
        accum72_t acc72;
        q31_t real, imag;
        real = *pSrc++;
        imag = *pSrc++;
        acc72 = fx_a72_mpy_q31(real, real);
        acc72 = fx_a72_mac_q31(acc72, imag, imag);
        *pDst++ = fx_q31_cast_asr_rnd_a72(acc72, 2);
    }
}

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
// primitives based version of complex magnitude
static _Inline void risc_dsp_cmpl_mag_squared_q31(const q31_t* __restrict pSrc,  q31_t* __restrict pDst,  uint32_t nSamples)
{
#if defined(__FXAPI__) && defined(__GUARD_ACCUM__)
    uint32_t i;
    for (i=0; i < nSamples; i++)
    {
        accum72_t acc72;
        q31_t real, imag;
        real = *pSrc++;
        imag = *pSrc++;
        acc72 = fx_a72_mpy_q31(real, real);
        acc72 = fx_a72_mac_q31(acc72, imag, imag);
        *pDst++ = fx_q31_cast_asr_rnd_a72(acc72, 2);
    }
#else
	uint32_t i;
	dspfx_accum64()
	for (i=0; i < nSamples; i++)
	{
		q31_t real, imag;
		real = (*pSrc++)>>1;
		imag = (*pSrc++)>>1;
		dspfx_a64mul_q31(real, real, acc);
		*pDst++ = dspfx_a64mac_q31(imag, imag, acc);
	}
#endif
};
#endif //_ARCv2EM_XY_AGU_MEDIUM
// threshold for switching between XY and primitives based versions
#define XY_COMPL_MAG_SQUARED_THRESHOLD 32

// simple XY memory map for complex magnitude function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/4)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Computes the Q31 squared magnitude of complex numbers stored in a vector                      
 * The return result is in 3.29 format.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_cmpl_mag_squared_q31(const PTR(q31_t) __restrict pSrc,  PTR(q31_t) __restrict pDst,  uint32_t nSamples)
{
#if defined(_ARC_AS210_mac32)
    if (nSamples < XY_COMPL_MAG_SQUARED_THRESHOLD) {
#endif
        // call to primitives based version of complex magnitude
        risc_dsp_cmpl_mag_squared_q31(pSrc, pDst, nSamples);
#if defined(_ARC_AS210_mac32) 
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

            // call to XY based version of complex magnitude
            dsp_cmpl_mag_squared_q31_XtoY_MAC32x32(XYMAP_INPUT, XYMAP_OUTPUT, IntsCurrent>>1);

            // extract result from XY memory
            memcpy_YtoS_int32((signed long *)(void *)pDst + (offset>>1), XYMAP_OUTPUT, IntsCurrent>>1);
            offset += IntsCurrent;
            IntsRemain -= IntsCurrent;
        };
    }
#endif
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_CMPL_MAG_SQUARED_Q31)
