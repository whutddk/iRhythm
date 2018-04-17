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

#if !defined(EXCLUDE_DSP_FIR_Q15)
#pragma Code("dsplib_dsp_fir_q15")

#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif

#if defined(_ARCv2EM_XY_AGU_MEDIUM) && defined(_XY_HAND_OPT)
static _Inline void risc_dsp_fir_q15(fir_q15_t* inst, const q15_t* __restrict pSrc, q15_t* __restrict pDst, uint32_t nSamples)
{
    int32_t i, j, offs;
    int32_t nTaps = inst->numTaps;
    const q15_t *pCoeff = (const q15_t *)inst->pCoeffs;
    q15_t *pState = inst->pData;
    int32_t N = dspfx_min_q31(nTaps, (int32_t)nSamples) & ~1;
    v2accum40_t vacc;
    _DSPLIB_store_regs(4, 6, 1);
    if (nTaps <= 0) return;
    __builtin_assume(nTaps > 0);
    __builtin_assume(nSamples > 0);
    // Initial part for FIR filter: samples are given from INPUT and STATE buffers
    // 2-samples output loop

    _set_agu_ap3(pDst);
    _set_agu_mod0(0, AGU_DT_v2q15_replic_q15, AGU_AD_INC_q15(1));
    _set_agu_mod1(1, AGU_DT_v2q15_replic_q15, AGU_AD_INC_q15(1));
    _set_agu_mod2(2, AGU_DT_v2q15, AGU_AD_INC_q15(2));
    _set_agu_mod3(3, AGU_DT_v2q15, AGU_AD_INC_q15(2));
    _set_agu_mod4(3, AGU_DT_q15, AGU_AD_INC_q15(1));
    for (offs = 0; offs < N; offs += 2) {
        vacc = fx_replic_v2a40(fx_create_a40(0,0));
        _set_agu_ap0(pState + offs + 1); // pState;
        _set_agu_ap1(pSrc); // pSrc
        _set_agu_ap2(pCoeff); //pCoeff
        for (j = offs+1; j < nTaps; j++) {
            vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u2()), fx_v2q15_cast_q31(_agu_u0()));
        }
        for (j = 0; j < offs + 2; j++) {
            vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u2()), fx_v2q15_cast_q31(_agu_u1()));
        }
        _st_agu_u4(fx_q15_cast_rnd_a40(fx_get_v2a40(vacc, 1)));
        _st_agu_u4(fx_q15_cast_rnd_a40(fx_get_v2a40(vacc, 0)));
    }

    // 1-sample output branch
    if (dspfx_min_q31(nTaps, (int32_t)nSamples) & 1) {
        vacc = fx_replic_v2a40(fx_create_a40(0,0));
        _set_agu_ap0(pState + offs + 1); // pState;
        _set_agu_ap1(pSrc); // pSrc
        _set_agu_ap2(pCoeff); //pCoeff
        for (j = offs+1; j < nTaps; j++) {
            vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u2()), fx_v2q15_cast_q31(_agu_u0()));
        }
        for (j = 0; j < offs + 2; j++) {
            vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u2()), fx_v2q15_cast_q31(_agu_u1()));
        }
        _st_agu_u4(fx_q15_cast_rnd_a40(fx_get_v2a40(vacc, 1)));
    }
    // Main part for FIR filter: samples are given from INPUT
    // 2-samples output loop
    _set_agu_ap1(pSrc + 1); // pSrc
    _set_agu_ap2(pCoeff); //pCoeff
    _set_agu_os0(nTaps-2);

    _set_agu_mod0(1, AGU_DT_v2q15_replic_q15, AGU_AD_DEC_q15_REG(0));
    _set_agu_mod5(2, AGU_DT_v2q15, AGU_AD_DEC_q15(2*nTaps));

    for (i = 1; i < ((int32_t)(nSamples - nTaps) & ~1); i += 2) {
        vacc = fx_replic_v2a40(fx_create_a40(0,0));
#if 0
        for (j = 0; j < nTaps; j++) {
            vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u2()), fx_v2q15_cast_q31(_agu_u1()));
        }
#else // split loop into 2 branches to eliminate zd loop initialization stall
        if (nTaps & 1)
        vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u2()), fx_v2q15_cast_q31(_agu_u1()));
        for (j = 0; j < (nTaps>>1); j++) {
            vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u2()), fx_v2q15_cast_q31(_agu_u1()));
            vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u2()), fx_v2q15_cast_q31(_agu_u1()));
        }
#endif
        vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u5()), fx_v2q15_cast_q31(_agu_u0()));
        _st_agu_u4(fx_q15_cast_rnd_a40(fx_get_v2a40(vacc, 1)));
        _st_agu_u4(fx_q15_cast_rnd_a40(fx_get_v2a40(vacc, 0)));
    }
    // 1-sample output branch
    if (((int32_t)(nSamples - nTaps) & 1) && (((int32_t)nSamples - nTaps) > 0)) {
        vacc = fx_replic_v2a40(fx_create_a40(0,0));
        for (j = 0; j < nTaps + 1; j++) {
            vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u2()), fx_v2q15_cast_q31(_agu_u1()));
        }
        _st_agu_u4(fx_q15_cast_rnd_a40(fx_get_v2a40(vacc, 1)));
    }

    // STATE buffer re-initialization with last samples from INPUT buffer
    _set_agu_ap0(pState); // pState init
    _set_agu_ap1(pState + nSamples);
    _set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(1));
    _set_agu_mod1(1, AGU_DT_q15, AGU_AD_INC_q15(1));
    for (i = 0; i < nTaps-(int32_t)nSamples; i++)
    {
    	_st_agu_u0(_agu_u1());
    }
    _set_agu_ap1(pSrc + i - (nTaps-nSamples));
    for (; i < nTaps; i++)
    {
    	_st_agu_u0(_agu_u1());
    }
    _DSPLIB_restore_regs();
}

#else //_ARCv2EM_XY_AGU_MEDIUM

#if defined(__FXAPI__)

#if defined DSPLIB_PRECOMPILE_FIR_Q15_1_TAPS  || defined DSPLIB_PRECOMPILE_FIR_Q15_2_TAPS  || \
    defined DSPLIB_PRECOMPILE_FIR_Q15_3_TAPS  || defined DSPLIB_PRECOMPILE_FIR_Q15_4_TAPS  || \
    defined DSPLIB_PRECOMPILE_FIR_Q15_5_TAPS  || defined DSPLIB_PRECOMPILE_FIR_Q15_6_TAPS  || \
    defined DSPLIB_PRECOMPILE_FIR_Q15_7_TAPS  || defined DSPLIB_PRECOMPILE_FIR_Q15_8_TAPS  || \
    defined DSPLIB_PRECOMPILE_FIR_Q15_9_TAPS  || defined DSPLIB_PRECOMPILE_FIR_Q15_10_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q15_11_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q15_12_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q15_13_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q15_14_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q15_15_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q15_16_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q15_17_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q15_18_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q15_19_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q15_20_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q15_21_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q15_22_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q15_23_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q15_24_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q15_25_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q15_26_TAPS
#define __DSPLIB_PRECOMPILE_FIR_Q15
#endif
 
#if defined __DSPLIB_PRECOMPILE_FIR_Q15

// Special branch with precompiled versions which should be installed
 
#define __DSP_FIR_Q15(nTaps) \
static void risc_dsp_fir_q15_## nTaps ## _taps(fir_q15_t* inst, const q15_t* __restrict pSrc, q15_t* __restrict pDst, uint32_t nSamples) \
{ \
    int32_t i, j, offs; \
    q15_t* __restrict _pDst = pDst; \
    q15_t* __restrict pState = (q15_t*) inst->pData; \
    int32_t N = dspfx_min_q31(nTaps, (int32_t)nSamples) & ~1; \
    dspfx_v2accum32(); \
    if (nTaps <= 0) return; \
    for (offs = 0; offs < N; offs += 2) { \
        const v2q15_t* pCoeff = (const v2q15_t*)inst->pCoeffs; \
        dspfx_v2a32_reset(); \
        __builtin_assume(nTaps > 0); \
        for (j = offs+1; j < nTaps; j++) { \
            dspfx_v2a32mac_v2q15(*pCoeff++, dspfx_replic_v2q15(pState[j])); \
        } \
        for (j = 0; j < offs + 2; j++) { \
            dspfx_v2a32mac_v2q15(*pCoeff++, dspfx_replic_v2q15(pSrc[j])); \
        } \
        *_pDst++ = dspfx_q15_get_cast_v2a32(1); \
        *_pDst++ = dspfx_q15_get_cast_v2a32(0); \
    } \
    if (dspfx_min_q31(nTaps, (int32_t)nSamples) & 1) { \
        const v2q15_t* pCoeff = (const v2q15_t*)inst->pCoeffs; \
        dspfx_v2a32_reset(); \
        __builtin_assume(nTaps > 0); \
        for (j = offs+1; j < nTaps; j++) { \
            dspfx_v2a32mac_v2q15(*pCoeff++, dspfx_replic_v2q15(pState[j])); \
        } \
        for (j = 0; j < offs + 2; j++) { \
            dspfx_v2a32mac_v2q15(*pCoeff++, dspfx_replic_v2q15(pSrc[j])); \
        } \
        *_pDst++ = dspfx_q15_get_cast_v2a32(1); \
    } \
    for (i = 1; i < ((int32_t)(nSamples - nTaps) & ~1); i += 2) { \
        const v2q15_t* pCoeff = (const v2q15_t*)inst->pCoeffs; \
        dspfx_v2a32_reset(); \
        __builtin_assume(nTaps+1 > 1); \
        _Pragma("unroll 26") \
        for (j = 0; j < nTaps+1; j++) { \
            dspfx_v2a32mac_v2q15(*pCoeff++, dspfx_replic_v2q15(pSrc[j+i])); \
        } \
        *_pDst++ = dspfx_q15_get_cast_v2a32(1); \
        *_pDst++ = dspfx_q15_get_cast_v2a32(0); \
    } \
    if (((int32_t)(nSamples - nTaps) & 1) && (((int32_t)nSamples - nTaps) > 0)) { \
        const v2q15_t* pCoeff = (const v2q15_t*)inst->pCoeffs; \
        dspfx_v2a32_reset(); \
        __builtin_assume(nTaps+1 > 1); \
        for (j = 0; j < nTaps+1; j++) { \
            dspfx_v2a32mac_v2q15(*pCoeff++, dspfx_replic_v2q15(pSrc[j+i])); \
        } \
        *_pDst++ = dspfx_q15_get_cast_v2a32(1); \
    } \
    for (i = 0; i < nTaps-(int32_t)nSamples; i++)  \
    { \
        pState[i] = pState[i+nSamples]; \
    } \
    __builtin_assume(nTaps > 0); \
    for (; i < nTaps; i++)  \
    { \
        pState[i] = pSrc[i-(nTaps-nSamples)]; \
    } \
}

__DSP_FIR_Q15(1)
__DSP_FIR_Q15(2);
__DSP_FIR_Q15(3);
__DSP_FIR_Q15(4);
__DSP_FIR_Q15(5);
__DSP_FIR_Q15(6);
__DSP_FIR_Q15(7);
__DSP_FIR_Q15(8);
__DSP_FIR_Q15(9);
__DSP_FIR_Q15(10);
__DSP_FIR_Q15(11);
__DSP_FIR_Q15(12);
__DSP_FIR_Q15(13);
__DSP_FIR_Q15(14);
__DSP_FIR_Q15(15);
__DSP_FIR_Q15(16);
__DSP_FIR_Q15(17);
__DSP_FIR_Q15(18);
__DSP_FIR_Q15(19);
__DSP_FIR_Q15(20);
__DSP_FIR_Q15(21);
__DSP_FIR_Q15(22);
__DSP_FIR_Q15(23);
__DSP_FIR_Q15(24);
__DSP_FIR_Q15(25);
__DSP_FIR_Q15(26);

typedef void (*fir_func_q15_t)(fir_q15_t* inst, const q15_t* __restrict pSrc, q15_t* __restrict pDst, uint32_t nSamples);

#define NULL 0

static fir_func_q15_t precompiled_fir_list[26] = {
#if defined DSPLIB_PRECOMPILE_FIR_Q15_1_TAPS
    risc_dsp_fir_q15_1_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_2_TAPS
    risc_dsp_fir_q15_2_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_3_TAPS
    risc_dsp_fir_q15_3_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_4_TAPS
    risc_dsp_fir_q15_4_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_5_TAPS
    risc_dsp_fir_q15_5_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_6_TAPS
    risc_dsp_fir_q15_6_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_7_TAPS
    risc_dsp_fir_q15_7_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_8_TAPS
    risc_dsp_fir_q15_8_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_9_TAPS
    risc_dsp_fir_q15_9_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_10_TAPS
    risc_dsp_fir_q15_10_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_11_TAPS
    risc_dsp_fir_q15_11_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_12_TAPS
    risc_dsp_fir_q15_12_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_13_TAPS
    risc_dsp_fir_q15_13_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_14_TAPS
    risc_dsp_fir_q15_14_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_15_TAPS
    risc_dsp_fir_q15_15_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_16_TAPS
    risc_dsp_fir_q15_16_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_17_TAPS
    risc_dsp_fir_q15_17_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_18_TAPS
    risc_dsp_fir_q15_18_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_19_TAPS
    risc_dsp_fir_q15_19_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_20_TAPS
    risc_dsp_fir_q15_20_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_21_TAPS
    risc_dsp_fir_q15_21_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_22_TAPS
    risc_dsp_fir_q15_22_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_23_TAPS
    risc_dsp_fir_q15_23_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_24_TAPS
    risc_dsp_fir_q15_24_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_25_TAPS
    risc_dsp_fir_q15_25_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q15_26_TAPS
    risc_dsp_fir_q15_26_taps,
#else
    NULL,
#endif
};

#endif

static _Inline void risc_dsp_fir_q15(fir_q15_t* inst, const PTR(q15_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples)
{
    int32_t i, j, offs;
    int32_t nTaps = inst->numTaps;
    PTR(q15_t) __restrict _pDst = pDst;
    PTR(q15_t) __restrict pState = (PTR(q15_t)) inst->pData;
    int32_t N = dspfx_min_q31(nTaps, (int32_t)nSamples) & ~1;
    dspfx_v2accum32();
    if (nTaps <= 0) return;
    // Initial part for FIR filter: samples are given from INPUT and STATE buffers
    // 2-samples output loop
    PRAGMA_NO_UNROLL
    for (offs = 0; offs < N; offs += 2) {
        const PTR(v2q15_t) pCoeff = (const PTR(v2q15_t))inst->pCoeffs;
        dspfx_v2a32_reset();
        __builtin_assume(nTaps > 0);
        for (j = offs+1; j < nTaps; j++) {
            dspfx_v2a32mac_v2q15(*pCoeff++, dspfx_replic_v2q15(pState[j]));
        }
        for (j = 0; j < offs + 2; j++) {
            dspfx_v2a32mac_v2q15(*pCoeff++, dspfx_replic_v2q15(pSrc[j]));
        }
        *_pDst++ = dspfx_q15_get_cast_v2a32(1);
        *_pDst++ = dspfx_q15_get_cast_v2a32(0);
    }
    // 1-sample output branch
    if (dspfx_min_q31(nTaps, (int32_t)nSamples) & 1) {
        const PTR(v2q15_t) pCoeff = (const PTR(v2q15_t))inst->pCoeffs;
        dspfx_v2a32_reset();
        __builtin_assume(nTaps > 0);
        for (j = offs+1; j < nTaps; j++) {
            dspfx_v2a32mac_v2q15(*pCoeff++, dspfx_replic_v2q15(pState[j]));
        }
        for (j = 0; j < offs + 2; j++) {
            dspfx_v2a32mac_v2q15(*pCoeff++, dspfx_replic_v2q15(pSrc[j]));
        }
        *_pDst++ = dspfx_q15_get_cast_v2a32(1);
    }
    // Main part for FIR filter: samples are given from INPUT
    // 2-samples output loop
    PRAGMA_NO_UNROLL
    for (i = 1; i < ((int32_t)(nSamples - nTaps) & ~1); i += 2) {
        const PTR(v2q15_t) pCoeff = (const PTR(v2q15_t))inst->pCoeffs;
        dspfx_v2a32_reset();
        __builtin_assume(nTaps+1 > 1);
        PRAGMA_UNROLL_2_FOR_XY
        for (j = 0; j < nTaps+1; j++) {
            dspfx_v2a32mac_v2q15(*pCoeff++, dspfx_replic_v2q15(pSrc[j+i]));
        }
        *_pDst++ = dspfx_q15_get_cast_v2a32(1);
        *_pDst++ = dspfx_q15_get_cast_v2a32(0);
    }
    // 1-sample output branch
    if (((int32_t)(nSamples - nTaps) & 1) && (((int32_t)nSamples - nTaps) > 0)) {
        const PTR(v2q15_t) pCoeff = (const PTR(v2q15_t))inst->pCoeffs;
        dspfx_v2a32_reset();
        __builtin_assume(nTaps+1 > 1);
        for (j = 0; j < nTaps+1; j++) {
            dspfx_v2a32mac_v2q15(*pCoeff++, dspfx_replic_v2q15(pSrc[j+i]));
        }
        *_pDst++ = dspfx_q15_get_cast_v2a32(1);
    }
    // STATE buffer re-initialization with last samples from INPUT buffer
    for (i = 0; i < nTaps-(int32_t)nSamples; i++) 
    {
        pState[i] = pState[i+nSamples];
    }
    __builtin_assume(nTaps > 0);
    for (; i < nTaps; i++) 
    {
        pState[i] = pSrc[i-(nTaps-nSamples)];
    }
}

#else
static _Inline void risc_dsp_fir_q15(fir_q15_t* inst, const q15_t* __restrict pSrc, q15_t* __restrict pDst, uint32_t nSamples)
{
    int32_t j;
    uint32_t i,k;
    const q15_t* pCoeff;
    uint32_t order = inst->numTaps-1 ;
    q15_t* pDataNew = inst->pData + inst->shift;
    q15_t* pBufLast = inst->pData + order;
    q15_t* pBufFirst = inst->pData;
    uint32_t shift= inst->shift;
    q15_t* p;    
    dspfx_accum32()
    INIT_MACMODE();
    PRAGMA_NO_UNROLL
    for ( i = 0; i < nSamples; i++ )
    {
        dspfx_reset_a32();
        *pDataNew = *pSrc++;
        p = pDataNew;
        pCoeff = inst->pCoeffs;
        for ( j = shift; j >= 0  ; j-- )
        {
            dspfx_a32mac_q15(*pCoeff++, *p--, acc);
        }

        p=pBufLast;

        for ( k = order; k > shift ; k-- )
        {
            dspfx_a32mac_q15(*pCoeff++, *p--, acc);
        }

        *pDst++  = dspfx_q15_cast_a32(acc);
        MODULE_ADDR_INC(pDataNew, pBufFirst, pBufLast);
        shift=pDataNew-pBufFirst;
    }
    inst->shift = (uint16_t)(shift);
}
#endif
#endif //_ARCv2EM_XY_AGU_MEDIUM

#define XY_CF_ADDR 0
#define XY_ST_ADDR 0

/**
 * Q15 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_fir_q15(fir_q15_t* inst, const PTR(q15_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples){
#if defined(_ARC_AS210_mac16)
    if ((inst->numTaps < 2))
    {
        risc_dsp_fir_q15(inst,  pSrc, pDst, nSamples);
    }else{
        int32_t XY_IN_ADDR = inst->numTaps;
        int32_t XY_OUT_ADDR = inst->numTaps;
        int32_t FIR_Q15_BLOCK_SIZE = (XY_REGION_SIZE/sizeof(q15_t) - inst->numTaps);
        int32_t Samples = (int32_t) nSamples;
        // Load filter coefficients to XY memory
        memcpy_StoX_int16(XY_CF_ADDR, (int16_t*)(void*)(inst->pCoeffs), inst->numTaps);
        // fill XY memory with state
        memcpy_StoY_int16(inst->fir_q15_ll.xyptr_states, (int16_t*)(void*)inst->pData, inst->numTaps);
        while (Samples > 0) {
            int32_t block_size = (Samples > FIR_Q15_BLOCK_SIZE) ? FIR_Q15_BLOCK_SIZE:Samples;
            // fill XY memory with input data
            memcpy_StoX_int16(XY_IN_ADDR, (int16_t*)(void*)pSrc, block_size);
            // call LL FIR filter function
            dsp_fir_q15_XtoY_cfXstY_MAC16x16(&inst->fir_q15_ll, XY_IN_ADDR, XY_OUT_ADDR, block_size);
            // Store output data
            memcpy_YtoS_int16((int16_t*)(void*)pDst, XY_OUT_ADDR, block_size);
            Samples -= block_size;
            pSrc += block_size;
            pDst += block_size;
        };
        // Store filter state
        memcpy_YtoS_int16((int16_t*)(void*)inst->pData, inst->fir_q15_ll.xyptr_states, inst->numTaps);
    }
#else

#if defined(__DSPLIB_PRECOMPILE_FIR_Q15)
    if ( precompiled_fir_list[inst->numTaps-1] != NULL) {
        #print precompile check
       (*precompiled_fir_list[inst->numTaps-1])(inst,  pSrc, pDst, nSamples);
    } else {
        risc_dsp_fir_q15(inst,  pSrc, pDst, nSamples);
    }
#else
    risc_dsp_fir_q15(inst,  pSrc, pDst, nSamples);
#endif

#endif
}

/**
 * initialization for Q15 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param pCoeffs - pointer to coefficients vector. Vector length: nTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_fir_getsize_q15()" function.
 * \result dsp_status
 */

static dsp_status wrapped_dsp_fir_init_q15(fir_q15_t* inst, uint16_t nTaps, const q15_t* pCoeffs, q15_t* pState)
{
#if defined(__FXAPI__)
    int32_t i;
    inst->pData = pState + (nTaps+1)*2;
    inst->pCoeffs = pState;
    pState[0] = 0;
    pState[2*nTaps+1] = 0;
    for (i = 0; i < nTaps; i++)
    {
        pState[2*i+1] = pCoeffs[nTaps - i - 1];
        pState[2*(i+1)] = pCoeffs[nTaps - i - 1];
    }
#else
    inst->pData = pState;
    inst->pCoeffs = (q15_t*)pCoeffs;
#endif

    inst->numTaps = nTaps;
    inst->shift = 0;
    _MEM_FILL_Q15((PTR(q15_t))inst->pData, 0, nTaps);
#if defined(_ARC_AS210_mac16)
    // Init low-level FIR filter
    dsp_fir_init_q15_cfXstY_MAC16x16(&inst->fir_q15_ll, nTaps, XY_CF_ADDR, XY_ST_ADDR);
#endif
    return DSP_ERR_OK;
}

#pragma Off_inline(wrapped_dsp_fir_init_q15)

dsp_status _DSPLIB_XYCC dsp_fir_init_q15(fir_q15_t* inst, uint16_t nTaps, const PTR(q15_t) pCoeffs, PTR(q15_t) pState)
{
	return wrapped_dsp_fir_init_q15(inst, nTaps, pCoeffs, pState);
}

void dsp_fir_delete_q15(fir_q15_t* inst) {};

dsp_status _DSPLIB_XYCC dsp_fir_init_q15_ext(  fir_q15_t *inst,  uint16_t nTaps,  PTR(q15_t) pCoeffs,  PTR(q15_t)pState,  uint32_t nSamples)
{
    dsp_fir_init_q15(inst, nTaps, pCoeffs, pState);
    return DSP_ERR_OK;
}

#endif //!defined(EXCLUDE_DSP_FIR_Q15)
