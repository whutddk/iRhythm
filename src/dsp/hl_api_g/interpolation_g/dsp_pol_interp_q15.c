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

#if !defined(EXCLUDE_DSP_POL_INTERP_Q15)

#include "dsplib_types.h"
#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_pol_interp_q15")
/**
 * q15 POLYNOMIAL EVALUATION INIT FUNCTION
 *
 * \param inst - pointer to interpolation instance structure
 * \param nCoeffs - number of coefficients. (polynom order = nCoeffs - 1)
 * \param pCoeffs - pointer to coefficients vector in direct order (c[0]*x^n + c[1]*x^(n-1)....c[n]), n = nCoeffs-1
 * \param shift - bit shift value that converts arithmetic after each multiplication with coefficient
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_pol_eval_init_q15(pol_q15_t *inst, uint16_t nCoeffs, const PTR(q15_t)pCoeffs, int32_t shift)
{
   inst->order = nCoeffs;
   inst->pCoeffs = pCoeffs;
   inst->shift = shift;

   return DSP_ERR_OK;
};

/**
 * q15 POLYNOMIAL EVALUATION PROCESSING FUNCTION
 *
 * \param inst - pointer to interpolation instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */
 
#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__ && defined _XY_HAND_OPT

void _DSPLIB_XYCC dsp_pol_eval_q15(const pol_q15_t *inst ,const PTR(q15_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples)
{
    uint32_t n,i;
    uint16_t order = inst->order;
    int32_t shift = inst->shift;
    const q15_t * coeffs = inst->pCoeffs;

    _DSPLIB_store_regs(3, 5, 1);

    n = 0;
    _set_agu_ap0(pSrc);
    _set_agu_ap1(pDst);
    _set_agu_ap2(coeffs);
    _set_agu_os0(order);    

    _set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(1));
    _set_agu_mod1(1, AGU_DT_q15, AGU_AD_INC_q15(1));
    _set_agu_mod2(2, AGU_DT_q15, AGU_AD_INC_q15(1));
    _set_agu_mod3(2, AGU_DT_q15, AGU_AD_DEC_q15_REG(0));
    _set_agu_mod4(2, AGU_DT_q15, AGU_AD_INC_q15(0));


    if (nSamples & 1) {
        q15_t x0 = (q15_t)_agu_u0();
        q15_t acc0 = (q15_t)_agu_u2();
        for (i=1; i  < order; i++)
        {
            acc0 = dspfx_mpy_q15(acc0, x0);
            acc0 = dspfx_add_q15(acc0, _agu_u2());
        }
        _add(_agu_u3(), 0);
        _st_agu_u1((q31_t)dspfx_asl_q15(acc0, shift));
        n++;
    };

    _set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2));
    _set_agu_mod2(2, AGU_DT_v2q15_replic_q15, AGU_AD_INC_q15(1));

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
        _st_agu_u1(fx_asl_q15(fx_get_v2q15(vacc, 0), shift));
        if ((n+1) < nSamples)
        	_st_agu_u1(fx_asl_q15(fx_get_v2q15(vacc, 1), shift));
    }
};


#else

void _DSPLIB_XYCC dsp_pol_eval_q15(const pol_q15_t *inst ,const PTR(q15_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples)
{
    int32_t n;
	uint32_t i;
    uint16_t order = inst->order;
    const PTR(q15_t) coeffs = (PTR(q15_t)) inst->pCoeffs;
    int32_t shift = inst->shift;
#if !defined(__FXAPI__)
    INIT_MACMODE();
    for (n=0; n<(int32_t)nSamples; n++)
    {
        q15_t x = pSrc[n];
        q15_t acc = coeffs[0];
        for (i=1; i  < order; i++)
        {
            acc = dspfx_mpy_q15(acc,x);
            acc = dspfx_add_q15(acc, coeffs[i]);
        }
        pDst[n] = dspfx_asl_q15(acc, inst->shift);
    }
#else

    n = 0;

#if defined _ARCVER_ARCv2HS_DSP && defined (__Xunaligned)
#pragma clang loop unroll(disable)
        for (; n < ((int32_t)nSamples-15); n+=16)
        {
            v2q15_t vx0 = fx_create_v2q15(pSrc[n], pSrc[n+1]);
            v2q15_t vx1 = fx_create_v2q15(pSrc[n+2], pSrc[n+3]);
            v2q15_t vx2 = fx_create_v2q15(pSrc[n+4], pSrc[n+5]);
            v2q15_t vx3 = fx_create_v2q15(pSrc[n+6], pSrc[n+7]);
            v2q15_t vx4 = fx_create_v2q15(pSrc[n+8], pSrc[n+9]);
            v2q15_t vx5 = fx_create_v2q15(pSrc[n+10], pSrc[n+11]);
            v2q15_t vx6 = fx_create_v2q15(pSrc[n+12], pSrc[n+13]);
            v2q15_t vx7 = fx_create_v2q15(pSrc[n+14], pSrc[n+15]);
            v2q15_t vacc0, vacc1, vacc2, vacc3, vacc4, vacc5, vacc6, vacc7;
            vacc0 = vacc1 = vacc2 = vacc3 = vacc4 = vacc5 = vacc6 = vacc7 =fx_create_v2q15(coeffs[0], coeffs[0]);

            for (i=1; i  < order; i++)
            {
                v2q15_t vc = fx_create_v2q15(coeffs[i], coeffs[i]);
#if defined(__GUARD_ACCUM__)
                vacc0 = fx_v2q15_cast_rnd_v2a40(fx_v2a40_mpy_v2q15(vacc0, vx0));
                vacc1 = fx_v2q15_cast_rnd_v2a40(fx_v2a40_mpy_v2q15(vacc1, vx1));
                vacc2 = fx_v2q15_cast_rnd_v2a40(fx_v2a40_mpy_v2q15(vacc2, vx2));
                vacc3 = fx_v2q15_cast_rnd_v2a40(fx_v2a40_mpy_v2q15(vacc3, vx3));
                vacc4 = fx_v2q15_cast_rnd_v2a40(fx_v2a40_mpy_v2q15(vacc4, vx4));
                vacc5 = fx_v2q15_cast_rnd_v2a40(fx_v2a40_mpy_v2q15(vacc5, vx5));
                vacc6 = fx_v2q15_cast_rnd_v2a40(fx_v2a40_mpy_v2q15(vacc6, vx6));
                vacc7 = fx_v2q15_cast_rnd_v2a40(fx_v2a40_mpy_v2q15(vacc7, vx7));
#else
                vacc0 = fx_v2q15_cast_rnd_v2a32(fx_v2a32_mpy_v2q15(vacc0, vx0));
                vacc1 = fx_v2q15_cast_rnd_v2a32(fx_v2a32_mpy_v2q15(vacc1, vx1));
                vacc2 = fx_v2q15_cast_rnd_v2a32(fx_v2a32_mpy_v2q15(vacc2, vx2));
                vacc3 = fx_v2q15_cast_rnd_v2a32(fx_v2a32_mpy_v2q15(vacc3, vx3));
                vacc4 = fx_v2q15_cast_rnd_v2a32(fx_v2a32_mpy_v2q15(vacc4, vx4));
                vacc5 = fx_v2q15_cast_rnd_v2a32(fx_v2a32_mpy_v2q15(vacc5, vx5));
                vacc6 = fx_v2q15_cast_rnd_v2a32(fx_v2a32_mpy_v2q15(vacc6, vx6));
                vacc7 = fx_v2q15_cast_rnd_v2a32(fx_v2a32_mpy_v2q15(vacc7, vx7));
#endif //__GUARD_ACCUM__
                vacc0 = fx_add_v2q15(vacc0, vc);
                vacc1 = fx_add_v2q15(vacc1, vc);
                vacc2 = fx_add_v2q15(vacc2, vc);
                vacc3 = fx_add_v2q15(vacc3, vc);
                vacc4 = fx_add_v2q15(vacc4, vc);
                vacc5 = fx_add_v2q15(vacc5, vc);
                vacc6 = fx_add_v2q15(vacc6, vc);
                vacc7 = fx_add_v2q15(vacc7, vc);
            }
            pDst[n]    = fx_asl_q15(fx_get_v2q15(vacc0, 0), shift);
            pDst[n+1]  = fx_asl_q15(fx_get_v2q15(vacc0, 1), shift);
            pDst[n+2]  = fx_asl_q15(fx_get_v2q15(vacc1, 0), shift);
            pDst[n+3]  = fx_asl_q15(fx_get_v2q15(vacc1, 1), shift);
            pDst[n+4]  = fx_asl_q15(fx_get_v2q15(vacc2, 0), shift);
            pDst[n+5]  = fx_asl_q15(fx_get_v2q15(vacc2, 1), shift);
            pDst[n+6]  = fx_asl_q15(fx_get_v2q15(vacc3, 0), shift);
            pDst[n+7]  = fx_asl_q15(fx_get_v2q15(vacc3, 1), shift);
            pDst[n+8]  = fx_asl_q15(fx_get_v2q15(vacc4, 0), shift);
            pDst[n+9]  = fx_asl_q15(fx_get_v2q15(vacc4, 1), shift);
            pDst[n+10] = fx_asl_q15(fx_get_v2q15(vacc5, 0), shift);
            pDst[n+11] = fx_asl_q15(fx_get_v2q15(vacc5, 1), shift);
            pDst[n+12] = fx_asl_q15(fx_get_v2q15(vacc6, 0), shift);
            pDst[n+13] = fx_asl_q15(fx_get_v2q15(vacc6, 1), shift);
            pDst[n+14] = fx_asl_q15(fx_get_v2q15(vacc7, 0), shift);
            pDst[n+15] = fx_asl_q15(fx_get_v2q15(vacc7, 1), shift);
        }
#endif


        if (nSamples & 1)
        {
            q15_t x = pSrc[n];
            q15_t acc = coeffs[0];
            for (i=1; i  < order; i++)
            {
                acc = dspfx_mpy_q15(acc,x);
                acc = dspfx_add_q15(acc, coeffs[i]);
            }
            pDst[n] = dspfx_asl_q15(acc, shift);
            n++;
        }

#pragma clang loop unroll(disable)
        for (; n <(int32_t)nSamples; n+=2)
        {
            v2q15_t vx = fx_create_v2q15(pSrc[n], pSrc[n+1]);
            v2q15_t vacc = fx_create_v2q15(coeffs[0], coeffs[0]);
            for (i=1; i  < order; i++)
            {
#if defined(__GUARD_ACCUM__)
                vacc = fx_v2q15_cast_rnd_v2a40(fx_v2a40_mpy_v2q15(vacc, vx));
#else
                vacc = fx_v2q15_cast_rnd_v2a32(fx_v2a32_mpy_v2q15(vacc, vx));
#endif
                vacc = fx_add_v2q15(vacc, fx_create_v2q15(coeffs[i], coeffs[i]));
            }
            pDst[n] = fx_asl_q15(fx_get_v2q15(vacc, 0), shift);
            if ((n+1) < nSamples)
                pDst[n+1] = fx_asl_q15(fx_get_v2q15(vacc, 1), shift);
        }
#endif //!defined(__FXAPI__)

};

#endif

void dsp_pol_eval_delete_q15(pol_q15_t *inst)
{
    // TODO: Place code here
};

#endif //!defined(EXCLUDE_DSP_POL_INTERP_Q15)
