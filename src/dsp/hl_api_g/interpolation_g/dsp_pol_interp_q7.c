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

#if !defined(EXCLUDE_DSP_POL_INTERP_Q7)

#include "dsplib_types.h"
#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_pol_interp_q7")
/**
 * q7 POLYNOMIAL EVALUATION INIT FUNCTION
 *
 * \param inst - pointer to interpolation instance structure
 * \param nCoeffs - number of coefficients. (polynom order = nCoeffs - 1)
 * \param pCoeffs - pointer to coefficients vector in direct order (c[0]*x^n + c[1]*x^(n-1)....c[n]), n = nCoeffs-1
 * \param shift - bit shift value that converts arithmetic after each multiplication with coefficient 
 * \result dsp_status
 */

 
dsp_status _DSPLIB_XYCC dsp_pol_eval_init_q7(pol_q7_t *inst, uint16_t nCoeffs, const PTR(q7_t) pCoeffs, int32_t shift)
{
   inst->order = nCoeffs;
   inst->pCoeffs = pCoeffs;
   inst->shift = shift;

   return DSP_ERR_OK;
};
 
/**
 * q7 POLYNOMIAL EVALUATION PROCESSING FUNCTION
 *
 * \param inst - pointer to interpolation instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

void _DSPLIB_XYCC dsp_pol_eval_q7(const pol_q7_t *inst ,const PTR(q7_t) __restrict pSrc, PTR(q7_t) __restrict pDst, uint32_t nSamples)
{
    uint32_t n,i;
    uint16_t order = inst->order;
    int32_t shift = inst->shift;
    const q7_t * coeffs = inst->pCoeffs;
    _DSPLIB_store_regs(3, 5, 1);

    n = 0;
    _set_agu_ap0(pSrc);
    _set_agu_ap1(pDst);
    _set_agu_ap2(coeffs);
    _set_agu_os0(order);    

    _set_agu_mod0(0, AGU_DT_q15_cast_q7, AGU_AD_INC_q7(1));
    _set_agu_mod1(1, AGU_DT_q7, AGU_AD_INC_q7(1));
    _set_agu_mod2(2, AGU_DT_q15_cast_q7, AGU_AD_INC_q7(1));
    _set_agu_mod3(2, AGU_DT_q15_cast_q7, AGU_AD_DEC_q7_REG(0));
    _set_agu_mod4(2, AGU_DT_q15_cast_q7, AGU_AD_INC_q7(0));

    if (nSamples & 1) {
        q15_t x0 = (q15_t)_agu_u0();
        q15_t acc0 = (q15_t)_agu_u2();
        for (i=1; i  < order; i++)
        {
            acc0 = dspfx_mpy_q15(acc0, x0);
            acc0 = dspfx_add_q15(acc0, _agu_u2());
        }
        _add(_agu_u3(), 0);
        _st_agu_u1(_vpack2hblf((q31_t)dspfx_asl_q15(acc0, shift)));
        n++;
    };

    _set_agu_mod0(0, AGU_DT_v2q15_cast_v2q7, AGU_AD_INC_q7(2));
    _set_agu_mod2(2, AGU_DT_v2q15_cast_v2q7 | __agu_mod_repl, AGU_AD_INC_q7(1));

    for (; n < nSamples; n+=2)
    {
        v2q15_t vx = fx_v2q15_cast_q31(_agu_u0());
        v2q15_t vacc = fx_v2q15_cast_q31(_agu_u2());
        for (i=1; i  < order; i++)
        {
            vacc = fx_v2q15_cast_rnd_v2a40(fx_v2a40_mpy_v2q15(vacc, vx));
            vacc = fx_add_v2q15(vacc, fx_v2q15_cast_q31(_agu_u2()));
        }
        _add(_agu_u3(), 0);
        _st_agu_u1(_vpack2hblf(fx_asl_q15(fx_get_v2q15(vacc, 0), shift)));
        if ((n+1) < nSamples)
        	_st_agu_u1(_vpack2hblf(fx_asl_q15(fx_get_v2q15(vacc, 1), shift)));
    }
};

#else

void _DSPLIB_XYCC dsp_pol_eval_q7(const pol_q7_t *inst ,const PTR(q7_t) __restrict pSrc, PTR(q7_t) __restrict pDst, uint32_t nSamples)
{
    uint32_t n,i;
    uint16_t order = inst->order;
    const PTR(q7_t) coeffs = (PTR(q7_t))inst->pCoeffs;
    int32_t shift = inst->shift;

    INIT_MACMODE_LSP();
    for (n=0; n<nSamples; n++)
    {
        q7_t x = pSrc[n];
        q7_t acc = coeffs[0];
        for (i=1; i  < order; i++)
        {
            acc = dspfx_mpy_q7(acc,x);
            acc = dspfx_add_q7(acc, coeffs[i]);
        }
        pDst[n] = dspfx_asl_q7(acc, shift);
    }
};

#endif

void dsp_pol_eval_delete_q7(pol_q7_t *inst)
{
 // TODO: Place code here
};

#endif //!defined(EXCLUDE_DSP_POL_INTERP_Q7)
