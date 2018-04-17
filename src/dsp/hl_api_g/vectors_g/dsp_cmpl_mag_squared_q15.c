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

#if !defined(EXCLUDE_DSP_CMPL_MAG_SQUARED_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsplib_helper.h"

#pragma Code("dsplib_dsp_cmpl_mag_squared_q15")
#ifdef _ARCv2EM_XY_AGU_MEDIUM

#if defined _XY_HAND_OPT

static _Inline void risc_dsp_cmpl_mag_squared_q15(const q15_t* __restrict pSrc,  q15_t* __restrict pDst,  uint32_t nSamples)
{
    uint32_t i;
	_DSPLIB_store_regs(3, 3, 0);
	_set_agu_ap0((uint32_t )pDst);
	_set_agu_ap1((uint32_t )pSrc);
	_set_agu_ap2((uint32_t )pSrc);
	_set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(1)); // output increment
	_set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // input increment
	_set_agu_mod2(2, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // input increment
	for (i = 0; i < nSamples; i++)
    {
        _st_agu_u0( fx_q15_cast_asr_rnd_a40(fx_a40_dmpy_v2q15(fx_v2q15_cast_q31(_agu_u1()), fx_v2q15_cast_q31(_agu_u2())), 2));
    }
	_DSPLIB_restore_regs();
}

#else

static _Inline void risc_dsp_cmpl_mag_squared_q15(const __xy q15_t* __restrict pSrc, __xy q15_t* __restrict pDst,  uint32_t nSamples)
{
	uint32_t i;
	__xy v2q15_t* pvSrc  = (__xy v2q15_t*) pSrc;
	__xy v2q15_t* pvSrc1 = (__xy v2q15_t*) pSrc;
	for (i=0; i < nSamples; i++)
	{
		*pDst++ = fx_q15_cast_asr_rnd_a40(fx_a40_dmpy_v2q15(*pvSrc1++, *pvSrc++), 2);
	}
};

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
// primitives based version of complex magnitude
static _Inline void risc_dsp_cmpl_mag_squared_q15(const q15_t* __restrict pSrc,  q15_t* __restrict pDst,  uint32_t nSamples)
{
	uint32_t i;
	dspfx_accum32()
	INIT_MACMODE();
	for (i=0; i < nSamples; i++)
	{
		q15_t real, imag;
		real = (*pSrc++)>>1;
		imag = (*pSrc++)>>1;
		dspfx_a32mul_q15(real, real, acc);
		dspfx_a32mac_q15(imag, imag, acc);
		*pDst++ = dspfx_q15_cast_a32(acc);        
	}
};
#endif //_ARCv2EM_XY_AGU_MEDIUM
// threshold for switching between XY and primitives based versions
#define XY_COMPL_MAG_SQUARED_THRESHOLD 32

// simple XY memory map for complex magnitude function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/4)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Computes the Q15 squared magnitude of complex numbers stored in a vector                      
 * The return result is in 3.13 format.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_cmpl_mag_squared_q15(const PTR(q15_t) __restrict pSrc,  PTR(q15_t) __restrict pDst,  uint32_t nSamples)
{
#if defined(_ARC_AS210_mac16)
    if (nSamples < XY_COMPL_MAG_SQUARED_THRESHOLD) {
#endif
        // call to primitives based version of complex magnitude
        risc_dsp_cmpl_mag_squared_q15(pSrc, pDst, nSamples);
#if defined(_ARC_AS210_mac16)
    } else {
        int32_t IntsRemain = nSamples;
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
            dsp_cmpl_mag_squared_q15_XtoY_MAC16x16(XYMAP_INPUT, XYMAP_OUTPUT, IntsCurrent);

            // extract result from XY memory
            memcpy_YtoS_int16((q15_t *)(void *)pDst + offset, XYMAP_OUTPUT, IntsCurrent);
            offset += IntsCurrent;
            IntsRemain -= IntsCurrent;
        };
    }
#endif
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_CMPL_MAG_SQUARED_Q15)
