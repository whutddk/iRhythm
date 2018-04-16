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

#if !defined(EXCLUDE_DSP_CMPL_MAG_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"


#if defined(__Xdsp) && defined(__FXAPI__) && defined(_ARC) && __Xdsp_version == 1

#include "fxarc.h"
#include <arc/arc_intrinsics.h>

#undef dsp_sqrt_31

#pragma Code("dsplib_dsp_cmpl_mag_q31")
static q31_t _Inline dsp_sqrt_q31(q31_t op)
{
    if (op == 0) return 0;
    op = _abss(op);
    _setacc(op<<1, 0x1);
    // 15 msbs
    _sqrtacc(1);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    // 8 more bits
    _aslacc(0x0e030e);
    _sr(0x7, 0x583);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    // 4 more bits
    _aslacc(0x080308);
    _sr(0x3, 0x583);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    // 2 more bits
    _aslacc(0x040304);
    _sr(0x1, 0x583);
    _sqrtacc(0);
    _sqrtacc(0);

    return _getacc(0x02);
}

#endif //defined(__Xdsp) && defined(__FXAPI__) && defined(_ARC) 

#ifdef _ARCv2EM_XY_AGU_MEDIUM

#if defined _XY_HAND_OPT

static _Inline void risc_dsp_cmpl_mag_q31(const q31_t* __restrict pSrc,  q31_t* __restrict pDst,  uint32_t nSamples)
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
        q31_t root;
        dspfx_a64mul_nf_q31(_agu_u1(), _agu_u2(), acc);
        root = dspfx_a64mac_nf_q31(_agu_u1(), _agu_u2(), acc);
        _st_agu_u0(dsp_sqrt_q31(root>>1));
    }
    _DSPLIB_restore_regs();
};

#else

static _Inline void risc_dsp_cmpl_mag_q31(const __xy q31_t* __restrict pSrc, __xy q31_t* __restrict pDst,  uint32_t nSamples)
{
    uint32_t i;
    dspfx_accum64()
    for (i=0; i < nSamples; i++)
    {
        q31_t root, real, imag;
        real = (*pSrc++);
        imag = (*pSrc++);
        dspfx_a64mul_nf_q31(real, real, acc);
        root = dspfx_a64mac_nf_q31(imag, imag, acc);
        *pDst++ = dsp_sqrt_q31(root>>1);
    }
};

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
// primitives based version of complex magnitude
static _Inline void risc_dsp_cmpl_mag_q31(const q31_t* __restrict pSrc,  q31_t* __restrict pDst,  uint32_t nSamples)
{
    uint32_t i;
    dspfx_accum64()
    for (i=0; i < nSamples; i++)
    {
        q31_t root, real, imag;
        real = (*pSrc++);
        imag = (*pSrc++);
        dspfx_a64mul_nf_q31(real, real, acc);
        root = dspfx_a64mac_nf_q31(imag, imag, acc);
        *pDst++ = dsp_sqrt_q31(root>>1);    
    }
};
#endif //_ARCv2EM_XY_AGU_MEDIUM
// threshold for switching between XY and primitives based versions
#define XY_COMPL_MAG_THRESHOLD 32

// simple XY memory map for complex magnitude function
#define XY_BLOCK_SIZE (XY_REGION_SIZE/4)
#define XYMAP_INPUT (0)
#define XYMAP_OUTPUT (0)

/**
 * Computes the Q31 magnitude of complex numbers stored in a vector                      
 * The return result is in 2.30 format.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_cmpl_mag_q31(const PTR(q31_t) __restrict pSrc,  PTR(q31_t) __restrict pDst,  uint32_t nSamples)
{
#if defined(_ARC_AS210_mac32)
    if (nSamples < XY_COMPL_MAG_THRESHOLD) {
#endif
        // call to primitives based version of complex magnitude
        risc_dsp_cmpl_mag_q31(pSrc, pDst, nSamples);
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
            dsp_cmpl_mag_q31_XtoY_MAC32x32(XYMAP_INPUT, XYMAP_OUTPUT, IntsCurrent>>1);

            // extract result from XY memory
            memcpy_YtoS_int32((signed long *)(void *)pDst + (offset>>1), XYMAP_OUTPUT, IntsCurrent>>1);
            offset += IntsCurrent;
            IntsRemain -= IntsCurrent;
        };
    }
#endif
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_CMPL_MAG_Q31)
