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

#include "dsplib_primitives.h"

#if !defined(EXCLUDE_DSP_BIQUAD_CASCADE_DF1_F32)
#pragma Code("dsplib_dsp_biquad_cascade_df1_f32")

/**
 * initialization for f32 IIR filtering
 *
 * \param inst - pointer to instance structure
 * \param numStages
 * \param pCoeffs - pointer to coefficients vector. Vector length: numStages * 5
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_biquad_cascade_df1_getsize_f32" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_biquad_cascade_df1_init_f32(iir_biquad_f32_t *inst, uint8_t numStages, const PTR(f32_t) pCoeffs, PTR(f32_t) pState)
{
    inst->filter.numTaps = numStages;
    inst->filter.pCoeffs = (f32_t*) pCoeffs;
    inst->filter.pData = pState;

#if defined (_ARCv2EM_XY_AGU_MEDIUM)
    inst->filter.pData = (f32_t*)ADDR_ALIGN(pState, 8);
#else
    inst->filter.pData = pState;
#endif
    _MEM_FILL_F32((PTR(f32_t))inst->filter.pData, 0.0f, numStages*4);
    return DSP_ERR_OK;
};

/**
 * f32 DF1 type Biquad-Cascade (IIR) filter
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

#if defined(_ARCv2EM_XY_AGU_MEDIUM) && defined _XY_HAND_OPT

void _DSPLIB_XYCC dsp_biquad_cascade_df1_f32(const iir_biquad_f32_t *inst, const __xy f32_t * __restrict pSrc, __xy f32_t * __restrict pDst, uint32_t nSamples)
{
    int32_t stage; // Stage counter
    uint32_t i; // Sample counter
    const f32_t* coeffs = inst->filter.pCoeffs;
    f32_t* states = inst->filter.pData;
    int32_t order  = inst->filter.numTaps;

    _DSPLIB_store_regs(4, 5, 0);

    _set_agu_ap0(pSrc);
    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_ui32(0)); // input increment
    _set_agu_mod1(0, AGU_DT_ui32, AGU_AD_INC_ui32(1)); // input increment
    _set_agu_mod2(1, AGU_DT_ui32, AGU_AD_INC_ui32(1)); // output increment
    _set_agu_mod3(2, AGU_DT_ui32, AGU_AD_WRAP_INC_II(sizeof(f32_t), AGU_WRAP_IMM_8)); // output increment
    _set_agu_mod4(3, AGU_DT_ui32, AGU_AD_WRAP_INC_II(sizeof(f32_t), AGU_WRAP_IMM_8)); // output increment

#pragma  clang loop unroll(disable)
    for (stage = 0; stage < order; stage++, states += 4)
    {
        f32_t b0, b1, b2, a1, a2;

        b0 =   *coeffs++;
        b1 =   *coeffs++;
        b2 =   *coeffs++;
        a1 =   *coeffs++;
        a2 =   *coeffs++;
        _set_agu_ap1(pDst);
        _set_agu_ap2(states); // output states
        _set_agu_ap3(states+2); // input states

        for (i = 0; i < nSamples; i++)
        {
        	f32_t acc = 0;
			MAC_F32(acc, _as_float(_agu_u4()), b2);
			MAC_F32(acc, _as_float(_agu_u4()), b1);
			MAC_F32(acc, _as_float(_agu_u0()), b0);
            _st_agu_u4(_agu_u1());
			MAC_F32(acc, _as_float(_agu_u3()), a2);
			MAC_F32(acc, _as_float(_agu_u3()), a1);
            _st_agu_u2(_as_uint32(acc));
            _st_agu_u3(_as_uint32(acc));
        }

        _set_agu_ap0(pDst);
    }
    _DSPLIB_restore_regs();
}

#else //_ARCv2EM_XY_AGU_MEDIUM

void dsp_biquad_cascade_df1_f32(const iir_biquad_f32_t *inst, const PTR(f32_t) __restrict pSrc, PTR(f32_t) __restrict pDst, uint32_t nSamples)
{
    int32_t stage; // Stage counter
    uint32_t i; // Sample counter 
    PTR(f32_t) src = (PTR(f32_t)) pSrc;
    PTR(f32_t) dst = (PTR(f32_t)) pDst;
    const PTR(f32_t) coeffs = (PTR(f32_t))inst->filter.pCoeffs;
    PTR(f32_t) states = (PTR(f32_t))inst->filter.pData;
    int32_t order  = inst->filter.numTaps;

    PRAGMA_NO_UNROLL
    for (stage = 0; stage < order; stage++, states += 4)
    {
        f32_t b0, b1, b2, a1, a2;
        f32_t y0, y1, y2, x0, x1, x2;

        b0 =   *coeffs++;
        b1 =   *coeffs++;
        b2 =   *coeffs++;
        a1 =   *coeffs++;
        a2 =   *coeffs++;

        y1 = states[0];
        x1 = states[2];
        x2 = states[3];
#if defined _BIQUAD_UNROLL
        y0 = states[1]; /*y2*/
        for (i = 0; i < nSamples >> 1; i++)
        {
            x0 = *src++;
            *dst++ = y0 = x0*b0 + x1*b1 + x2*b2 + y1*a1 + y0*a2;
            x2 = *src++;
            *dst++ = y1 = x2*b0 + x0*b1 + x1*b2 + y0*a1 + y1*a2;
            x1 = x2;
            x2 = x0;
        }
        if (nSamples & 1) {
        	f32_t tmp;
            x0 = *src++;
            tmp = x0*b0 + x1*b1 + x2*b2 + y1*a1 + y0*a2;
            y0 = y1;
            *dst++ = y1 = tmp;
            x2 = x1;
            x1 = x0;
        }

        states[0] = y1;
        states[1] = y0;
        states[2] = x1;
        states[3] = x2;
#else
        y2 = states[1];

        for (i = 0; i < nSamples; i++) 
        {
            x0 = *src++;

            *dst++ = y0 = x0*b0 + x1*b1 + x2*b2 + y1*a1 + y2*a2;

            y2 = y1;
            y1 = y0;
            x2 = x1;
            x1 = x0;
        }

        states[0] = y1;
        states[1] = y2;
        states[2] = x1;
        states[3] = x2;
#endif
        src = pDst;
        dst = pDst;
    }
};

#endif //_ARCv2EM_XY_AGU_MEDIUM

void dsp_biquad_cascade_df1_delete_f32(iir_biquad_f32_t* inst)
{
};

#endif //!defined(EXCLUDE_DSP_BIQUAD_CASCADE_DF1_F32)
