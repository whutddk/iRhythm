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

#if !defined(EXCLUDE_DSP_POL_INTERP_Q31)

#include "dsplib_types.h"
#include "dsplib_primitives.h"



#pragma Code("dsplib_dsp_pol_interp_q31")
/**
 * q31 POLYNOMIAL EVALUATION INIT FUNCTION
 *
 * \param inst - pointer to interpolation instance structure
 * \param nCoeffs - number of coefficients. (polynom order = nCoeffs - 1)
 * \param pCoeffs - pointer to coefficients vector in direct order (c[0]*x^n + c[1]*x^(n-1)....c[n]), n = nCoeffs-1
 * \param shift - bit shift value that converts arithmetic after each multiplication with coefficient 
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_pol_eval_init_q31(pol_q31_t *inst, uint16_t nCoeffs, const PTR(q31_t) pCoeffs, int32_t shift)
{
   inst->order = nCoeffs;
   inst->pCoeffs = pCoeffs;
   inst->shift = shift;

   return DSP_ERR_OK;
};

/**
 * q31 POLYNOMIAL EVALUATION PROCESSING FUNCTION
 *
 * \param inst - pointer to interpolation instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

#if defined _ARCv2EM_XY && defined __FXAPI__

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM

void _DSPLIB_XYCC dsp_pol_eval_q31(const pol_q31_t *inst ,const __xy q31_t * __restrict pSrc, __xy q31_t * __restrict pDst, uint32_t nSamples)
{
    uint32_t n,i;
    uint16_t order = inst->order;
    const q31_t * coeffs = inst->pCoeffs;
    int32_t shift = inst->shift;
    _DSPLIB_store_regs(3, 5, 1);

    n = 0;

    _set_agu_ap0(pSrc);
    _set_agu_ap1(pDst);
    _set_agu_ap2(coeffs);
    _set_agu_os0(order);    

    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod1(1, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod2(2, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod3(2, AGU_DT_q31, AGU_AD_DEC_q31_REG(0));
    _set_agu_mod4(2, AGU_DT_q31, AGU_AD_INC_q31(0));


    if (nSamples & 1) {
        q31_t x0 = _agu_u0();
        q31_t acc0;
        acc0 = _agu_u2();
        for (i=1; i  < order; i++)
        {
            acc0 = dspfx_mpy_q31(acc0, x0);
            acc0 = dspfx_add_q31(acc0, _agu_u2());
        }
        _add(_agu_u3(), 0);
        _st_agu_u1(dspfx_asl_q31(acc0, shift));
        n = 1;
    };
    for (; n<nSamples; n+=2)
    {
        q31_t x0 = _agu_u0();
        q31_t x1 = _agu_u0();
        q31_t acc0, acc1;
        acc0 = acc1 = _agu_u2();
        for (i=1; i  < order; i++)
        {
            acc0 = dspfx_mpy_q31(acc0, x0);
            acc1 = dspfx_mpy_q31(acc1, x1);
            acc0 = dspfx_add_q31(acc0, _agu_u4());
            acc1 = dspfx_add_q31(acc1, _agu_u2());
        }
        _add(_agu_u3(), 0);
        _st_agu_u1(dspfx_asl_q31(acc0, shift));
        if ((n+1) < nSamples)
        	_st_agu_u1(dspfx_asl_q31(acc1, shift));
    }
};

#else

void dsp_pol_eval_q31(const pol_q31_t *inst ,const PTR(q31_t) __restrict pSrc, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
    uint32_t n,i;
    uint16_t order = inst->order;
    const PTR(q31_t) __restrict coeffs = (const PTR(q31_t))inst->pCoeffs;
    q31_t acc0, acc1;
    int32_t shift = inst->shift;
    n = 0;

    if (nSamples & 1) {
        q31_t x0 = *pSrc++;
        acc0 = coeffs[0];
        for (i=1; i  < order; i++)
        {
            acc0 = dspfx_mpy_q31(acc0, x0);
            acc0 = dspfx_add_q31(acc0, coeffs[i]);
        }
        *pDst++ = dspfx_asl_q31(acc0, shift);
        n = 1;
    };
#pragma clang loop unroll(disable)    
    for (; n<nSamples; n+=2)
    {
        q31_t x0 = *pSrc++;
        q31_t x1 = *pSrc++;
        acc0 = acc1 = coeffs[0];
        for (i=1; i  < order; i++)
        {
            acc0 = dspfx_mpy_q31(acc0, x0);
            acc1 = dspfx_mpy_q31(acc1, x1);
            acc0 = dspfx_add_q31(acc0, coeffs[i]);
            acc1 = dspfx_add_q31(acc1, coeffs[i]);
        }
        *pDst++ = dspfx_asl_q31(acc0, shift);
        if ((n+1) < nSamples)
            *pDst++ = dspfx_asl_q31(acc1, shift);
    }
};

#endif

#else

void dsp_pol_eval_q31(const pol_q31_t *inst ,const q31_t * __restrict pSrc, q31_t * __restrict pDst, uint32_t nSamples)
{
    uint32_t n,i;
    uint16_t order = inst->order;
    const q31_t * coeffs = inst->pCoeffs;
    n = 0;

    if (nSamples & 1) {
        q31_t x0 = pSrc[0];
        q31_t acc0;
        acc0 = coeffs[0];
        for (i=1; i  < order; i++)
        {
            acc0 = dspfx_mpy_q31(acc0, x0);
            acc0 = dspfx_add_q31(acc0, coeffs[i]);
        }
        pDst[0] = dspfx_asl_q31(acc0, inst->shift);
        n = 1;
    };
    for (; n<nSamples; n+=2)
    {
        q31_t x0 = pSrc[n];
        q31_t x1 = pSrc[n+1];
        q31_t acc0, acc1;
        acc0 = acc1 = coeffs[0];
        for (i=1; i  < order; i++)
        {
            acc0 = dspfx_mpy_q31(acc0, x0);
            acc1 = dspfx_mpy_q31(acc1, x1);
            acc0 = dspfx_add_q31(acc0, coeffs[i]);
            acc1 = dspfx_add_q31(acc1, coeffs[i]);
        }
        pDst[n] = dspfx_asl_q31(acc0, inst->shift);
        if ((n+1) < nSamples)
            pDst[n+1] = dspfx_asl_q31(acc1, inst->shift);
    }
};

#endif

void dsp_pol_eval_delete_q31(pol_q31_t *inst)
{
    // TODO: Place code here
};

#endif //!defined(EXCLUDE_DSP_POL_INTERP_Q31)
