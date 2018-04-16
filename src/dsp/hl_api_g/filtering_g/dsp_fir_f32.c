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

#if !defined(EXCLUDE_DSP_FIR_F32)
#pragma Code("dsplib_dsp_fir_f32")

/**
 * Floating-point real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */
#ifdef _ARCv2EM_XY

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM

void _DSPLIB_XYCC dsp_fir_f32(fir_f32_t* inst, const __xy f32_t* __restrict pSrc, __xy f32_t* __restrict pDst, uint32_t nSamples)
{
    int32_t i, j;
    uint32_t input_mod_base, input_mod_offset;
    uint32_t order = inst->numTaps-1;
    uint32_t shift= inst->shift;

    input_mod_base = ADDR_ALIGNED_BASE32(inst->pData, inst->numTaps);
    input_mod_offset = (((uint32_t)(inst->pData)) - input_mod_base) >> 2;

    _DSPLIB_store_regs(4, 8, 4);

    _set_agu_ap0((uint32_t)pSrc);
    _set_agu_ap1((uint32_t)pDst);
    _set_agu_ap2((uint32_t)(inst->pData + shift));
    _set_agu_ap3((uint32_t)(inst->pCoeffs));

    _set_agu_os0(sizeof(q31_t));
    _set_agu_os1((inst->numTaps*sizeof(q31_t)-1));
    _set_agu_os2(input_mod_offset);
    _set_agu_os3(order);

    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_ui32(1)); // input increment
    _set_agu_mod1(1, AGU_DT_ui32, AGU_AD_INC_ui32(1)); // output increment


    _set_agu_mod3(2, AGU_DT_ui32, AGU_AD_WRAP_DEC_RR(0, 1)); // delay line modulo addressing
    _set_agu_mod5(2, AGU_DT_ui32, AGU_AD_INC_ui32(0)); // delay line non-modify

    _set_agu_mod6(3, AGU_DT_ui32, AGU_AD_INC_ui32(1)); // coefficients increment
    _set_agu_mod7(3, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(3)); // coefficients decrement

    if (input_mod_offset == 0){
#pragma  clang loop unroll(disable)
        for ( i = 0; i < nSamples; i++ )
        {
            f32_t acc = 0;
            _st_agu_u5(_agu_u0()); // non-increment
            for ( j = 0; j < order  ; j++ )
            {
                MAC_F32(acc, _as_float(_agu_u6()), _as_float(_agu_u3())); // decrement to input_mod_base
            }
            MAC_F32(acc,  _as_float(_agu_u7()), _as_float(_agu_u5()));
            _st_agu_u1(_as_uint32(acc));// non-increment

        }

    }else{
        _set_agu_mod2(2, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(2));
        _set_agu_mod4(2, AGU_DT_ui32, AGU_AD_INC_ui32_REG(2));
#pragma  clang loop unroll(disable)
        for ( i = 0; i < nSamples; i++ )
        {
            f32_t acc = 0;
            _st_agu_u5(_agu_u0()); // non-increment
            for ( j = 0; j < order  ; j++ )
            {
                MAC_F32(acc, _as_float(_agu_u6()), _as_float(_agu_u2())); // decrement to input_mod_base
                _agu_u3(); // modulo decrement
                _agu_u4(); // increment to actual address
            }
            MAC_F32(acc,  _as_float(_agu_u7()), _as_float(_agu_u5()));
            _st_agu_u1(_as_uint32(acc));// non-increment
        }
    }
    shift = ((uint32_t)_agu_ap2() - (uint32_t)(inst->pData)) >> 2;
    inst->shift = (uint16_t)(shift);

    _DSPLIB_restore_regs();
}

#else

void _DSPLIB_XYCC dsp_fir_f32(fir_f32_t* inst, const __xy f32_t* __restrict pSrc, __xy f32_t* __restrict pDst, uint32_t nSamples)
{
    int32_t i, j;
    uint32_t input_mod_base, input_mod_offset;
    uint32_t order = inst->numTaps-1;
    PTR(f32_t) pData = (PTR(f32_t)) inst->pData + inst->shift;
    const PTR(f32_t) pCoeff = (const PTR(f32_t)) inst->pCoeffs;

    input_mod_base = ADDR_ALIGNED_BASE32(inst->pData, inst->numTaps);
    input_mod_offset = (((uint32_t)(inst->pData)) - input_mod_base) >> 2;

	for ( i = 0; i < nSamples; i++ )
	{
		f32_t acc = 0;
		*pData = *pSrc++;
		for ( j = 0; j < order; j++ )
		{
			acc += *pCoeff++ * *pData;
			_AGU_CIRC_DEC_SAFE(pData, 1, inst->numTaps, input_mod_offset);
		}
		acc += *pCoeff * *pData; pCoeff -= order;
		*pDst++ = acc;
	}
    inst->shift = pData - inst->pData;
}

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM

#if defined DSPLIB_PRECOMPILE_FIR_F32_1_TAPS  || defined DSPLIB_PRECOMPILE_FIR_F32_2_TAPS  || \
    defined DSPLIB_PRECOMPILE_FIR_F32_3_TAPS  || defined DSPLIB_PRECOMPILE_FIR_F32_4_TAPS  || \
    defined DSPLIB_PRECOMPILE_FIR_F32_5_TAPS  || defined DSPLIB_PRECOMPILE_FIR_F32_6_TAPS  || \
    defined DSPLIB_PRECOMPILE_FIR_F32_7_TAPS  || defined DSPLIB_PRECOMPILE_FIR_F32_8_TAPS  || \
    defined DSPLIB_PRECOMPILE_FIR_F32_9_TAPS  || defined DSPLIB_PRECOMPILE_FIR_F32_10_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_F32_11_TAPS || defined DSPLIB_PRECOMPILE_FIR_F32_12_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_F32_13_TAPS || defined DSPLIB_PRECOMPILE_FIR_F32_14_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_F32_15_TAPS || defined DSPLIB_PRECOMPILE_FIR_F32_16_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_F32_17_TAPS || defined DSPLIB_PRECOMPILE_FIR_F32_18_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_F32_19_TAPS || defined DSPLIB_PRECOMPILE_FIR_F32_20_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_F32_21_TAPS || defined DSPLIB_PRECOMPILE_FIR_F32_22_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_F32_23_TAPS || defined DSPLIB_PRECOMPILE_FIR_F32_24_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_F32_25_TAPS || defined DSPLIB_PRECOMPILE_FIR_F32_26_TAPS
#define __DSPLIB_PRECOMPILE_FIR_F32
#endif
 
#if defined __DSPLIB_PRECOMPILE_FIR_F32

// Special branch with precompiled versions which should be installed
 
#define __DSP_FIR_F32(nTaps) \
static void risc_dsp_fir_f32_## nTaps ## _taps(fir_f32_t* inst, const f32_t* __restrict pSrc, f32_t* __restrict pDst, uint32_t nSamples) \
{ \
    int32_t i, j, offs; \
    f32_t * __restrict _pDst = pDst; \
    const f32_t * __restrict pCoeff; \
    f32_t * __restrict pState = inst->pData; \
    int32_t N = dspfx_min_q31((nTaps-1), (int32_t)nSamples); \
    for (offs = 0; offs < N; offs ++) { \
        f32_t acc = 0; \
        pCoeff = inst->pCoeffs + nTaps - 1; \
        for (j = offs+1; j < nTaps; j++) { \
            acc += *pCoeff-- * pState[j]; \
        } \
        for (j = 0; j < offs + 1; j++) { \
            acc += *pCoeff-- * pSrc[j]; \
        } \
        *_pDst++ = acc; \
    } \
    for (i = 0; i < (int32_t)(nSamples - nTaps + 1); i++)  \
    { \
        f32_t acc = 0; \
        pCoeff = inst->pCoeffs + nTaps - 1; \
        _Pragma("unroll 26") \
        for (j = 0; j < nTaps; j++) { \
            acc += *pCoeff-- * pSrc[j+i]; \
        } \
        *_pDst++ = acc; \
    } \
    for (i = 0; i < nTaps-(int32_t)nSamples; i++) { \
        pState[i] = pState[i+nSamples]; \
    } \
    for (; i < nTaps; i++) { \
        pState[i] = pSrc[i-(nTaps-nSamples)]; \
    } \
}

__DSP_FIR_F32(1)
__DSP_FIR_F32(2)
__DSP_FIR_F32(3)
__DSP_FIR_F32(4)
__DSP_FIR_F32(5)
__DSP_FIR_F32(6)
__DSP_FIR_F32(7)
__DSP_FIR_F32(8)
__DSP_FIR_F32(9)
__DSP_FIR_F32(10)
__DSP_FIR_F32(11)
__DSP_FIR_F32(12)
__DSP_FIR_F32(13)
__DSP_FIR_F32(14)
__DSP_FIR_F32(15)
__DSP_FIR_F32(16)
__DSP_FIR_F32(17)
__DSP_FIR_F32(18)
__DSP_FIR_F32(19)
__DSP_FIR_F32(20)
__DSP_FIR_F32(21)
__DSP_FIR_F32(22)
__DSP_FIR_F32(23)
__DSP_FIR_F32(24)
__DSP_FIR_F32(25)
__DSP_FIR_F32(26)

typedef void (*fir_func_f32_t)(fir_f32_t* inst, const f32_t* __restrict pSrc, f32_t* __restrict pDst, uint32_t nSamples);

#define NULL 0
static fir_func_f32_t precompiled_fir_list[26] = {
#if defined DSPLIB_PRECOMPILE_FIR_F32_1_TAPS
    risc_dsp_fir_f32_1_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_2_TAPS
    risc_dsp_fir_f32_2_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_3_TAPS
    risc_dsp_fir_f32_3_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_4_TAPS
    risc_dsp_fir_f32_4_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_5_TAPS
    risc_dsp_fir_f32_5_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_6_TAPS
    risc_dsp_fir_f32_6_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_7_TAPS
    risc_dsp_fir_f32_7_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_8_TAPS
    risc_dsp_fir_f32_8_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_9_TAPS
    risc_dsp_fir_f32_9_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_10_TAPS
    risc_dsp_fir_f32_10_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_11_TAPS
    risc_dsp_fir_f32_11_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_12_TAPS
    risc_dsp_fir_f32_12_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_13_TAPS
    risc_dsp_fir_f32_13_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_14_TAPS
    risc_dsp_fir_f32_14_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_15_TAPS
    risc_dsp_fir_f32_15_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_16_TAPS
    risc_dsp_fir_f32_16_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_17_TAPS
    risc_dsp_fir_f32_17_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_18_TAPS
    risc_dsp_fir_f32_18_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_19_TAPS
    risc_dsp_fir_f32_19_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_20_TAPS
    risc_dsp_fir_f32_20_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_21_TAPS
    risc_dsp_fir_f32_21_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_22_TAPS
    risc_dsp_fir_f32_22_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_23_TAPS
    risc_dsp_fir_f32_23_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_24_TAPS
    risc_dsp_fir_f32_24_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_25_TAPS
    risc_dsp_fir_f32_25_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_F32_26_TAPS
    risc_dsp_fir_f32_26_taps,
#else
    NULL,
#endif
};

#endif


void dsp_fir_f32(fir_f32_t* inst, const f32_t* __restrict pSrc, f32_t* __restrict pDst, uint32_t nSamples)
{
    uint32_t order = inst->numTaps - 1;
#if defined(__DSPLIB_PRECOMPILE_FIR_F32)
    if ( precompiled_fir_list[order] != NULL) {
       (*precompiled_fir_list[order])(inst,  pSrc, pDst, nSamples);
    } else {
#endif 
    int32_t j;
    uint32_t i,k;
    const f32_t* pCoeff;
    f32_t* pDataNew = inst->pData + inst->shift;
    f32_t* pBufLast = inst->pData + order;
    f32_t* pBufFirst = inst->pData;
    uint32_t shift= inst->shift;
    f32_t* p;    
    f32_t acc;
    PRAGMA_NO_UNROLL
    for ( i = 0; i < nSamples; i++ )
    {
        acc = 0;
        *pDataNew = *pSrc++;
        p = pDataNew;
        pCoeff = inst->pCoeffs;
        for ( j = shift; j >= 0  ; j-- )
        {
            MAC_F32(acc, *pCoeff++, *p--);
        }

        p=pBufLast;

        for ( k = order; k > shift ; k-- )
        {
            MAC_F32(acc,*pCoeff++, *p--);
        }
        *pDst++  = acc;
        MODULE_ADDR_INC(pDataNew, pBufFirst, pBufLast);
        shift=pDataNew-pBufFirst;
    }
    inst->shift = (uint16_t)(shift);
#if defined(__DSPLIB_PRECOMPILE_FIR_F32)
    }
#endif
}
#endif //_ARCv2EM_XY_AGU_MEDIUM

/**
 * initialization for floating-point real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param pCoeffs - pointer to coefficients vector. Vector length: nTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_fir_getsize_f32" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_fir_init_f32(fir_f32_t* inst, uint16_t nTaps, const PTR(f32_t) pCoeffs, PTR(f32_t) pState)
{
    inst->numTaps = nTaps;
    inst->pData = pState;
    inst->pCoeffs = ( f32_t *)pCoeffs;
    inst->shift = 0;
    _MEM_FILL_F32(pState, 0.0f, nTaps);
    return DSP_ERR_OK;
}

void dsp_fir_delete_f32(fir_f32_t* inst) {};

void _DSPLIB_XYCC dsp_fir_init_f32_ext(  fir_f32_t *inst,  uint16_t nTaps,  PTR(f32_t) pCoeffs,  PTR(f32_t) pState,  uint32_t nSamples)
{
    dsp_fir_init_f32(inst,  nTaps, pCoeffs,  pState);
    return;
}

#endif //!defined(EXCLUDE_DSP_FIR_F32)
