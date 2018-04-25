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

#if !defined(EXCLUDE_DSP_FIR_Q31)
#pragma Code("dsplib_dsp_fir_q31")

#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif

#if defined(_ARCv2EM_XY_AGU_MEDIUM)
#if defined _XY_HAND_OPT

static _Inline void risc_dsp_fir_q31(fir_q31_t* inst, const q31_t* __restrict pSrc, q31_t* __restrict pDst, uint32_t nSamples)
{
    int32_t i, j;
    uint32_t input_mod_base, input_mod_offset;
    int32_t order = inst->numTaps-1;
    uint32_t shift= inst->shift;
    dspfx_accum64();

    _DSPLIB_store_regs(4, 8, 4);

    input_mod_base = ADDR_ALIGNED_BASE32(inst->pData, inst->numTaps);
    input_mod_offset = (((uint32_t)(inst->pData)) - input_mod_base) >> 2;

    _set_agu_ap0((uint32_t)pSrc);
    _set_agu_ap1((uint32_t)pDst);
    _set_agu_ap2((uint32_t)(inst->pData + shift));
    _set_agu_ap3((uint32_t)(inst->pCoeffs));

    _set_agu_os0(sizeof(q31_t));
    _set_agu_os1((inst->numTaps*sizeof(q31_t)-1));
    _set_agu_os2(input_mod_offset);
    _set_agu_os3(order);

    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1)); // input increment
    _set_agu_mod1(1, AGU_DT_q31, AGU_AD_INC_q31(1)); // output increment

    _set_agu_mod3(2, AGU_DT_q31, AGU_AD_WRAP_DEC_RR(0, 1)); // delay line modulo addressing
    _set_agu_mod5(2, AGU_DT_q31, AGU_AD_INC_q31(0)); // delay line non-modify

    _set_agu_mod6(3, AGU_DT_q31, AGU_AD_INC_q31(1)); // coefficients increment
    _set_agu_mod7(3, AGU_DT_q31, AGU_AD_DEC_q31_REG(3)); // coefficients decrement

    __builtin_assume(order >= 0);
    if (input_mod_offset == 0)
    {
    	if (((order & 1) == 0)) {
#pragma  clang loop unroll(disable)
            for ( i = 0; i < (int32_t)nSamples; i++ )
            {
                dspfx_reset_a64();
                _st_agu_u5(_agu_u0()); // non-increment
                for ( j = 0; j < order >> 1  ; j++ )
                {
                    dspfx_a64mac_q31(_agu_u6(), _agu_u3(), acc); // decrement to input_mod_base
                    dspfx_a64mac_q31(_agu_u6(), _agu_u3(), acc); // decrement to input_mod_base
                }
                dspfx_a64mac_q31(_agu_u7(), _agu_u5(), acc); // non-increment
                _st_agu_u1(dspfx_q31_cast_a64(acc));
            }
    	} else {
#pragma  clang loop unroll(disable)
            for ( i = 0; i < (int32_t)nSamples; i++ )
            {
                dspfx_reset_a64();
                _st_agu_u5(_agu_u0()); // non-increment
                for ( j = 0; j < order >> 1 ; j++ )
                {
                    dspfx_a64mac_q31(_agu_u6(), _agu_u3(), acc); // decrement to input_mod_base
                    dspfx_a64mac_q31(_agu_u6(), _agu_u3(), acc); // decrement to input_mod_base
                }
                dspfx_a64mac_q31(_agu_u6(), _agu_u3(), acc); // decrement to input_mod_base
                dspfx_a64mac_q31(_agu_u7(), _agu_u5(), acc); // non-increment
                _st_agu_u1(dspfx_q31_cast_a64(acc));

            }
    	}

    }else{
        _set_agu_mod2(2, AGU_DT_q31, AGU_AD_DEC_q31_REG(2));
        _set_agu_mod4(2, AGU_DT_q31, AGU_AD_INC_q31_REG(2));
#pragma  clang loop unroll(disable)
        for ( i = 0; i < nSamples; i++ )
        {
            dspfx_reset_a64();
            _st_agu_u5(_agu_u0()); // non-increment
            for ( j = 0; j < order  ; j++ )
            {

                dspfx_a64mac_q31(_agu_u6(), _agu_u2(), acc); // decrement to input_mod_base
                _agu_u3(); // modulo decrement
                _agu_u4(); // increment to actual address
            }
            dspfx_a64mac_q31(_agu_u7(), _agu_u5(), acc); // non-increment
            _st_agu_u1(dspfx_q31_cast_a64(acc));

        }
    }
    shift = ((uint32_t)_agu_ap2() - (uint32_t)(inst->pData)) >> 2;
    inst->shift = (uint16_t)(shift);

    _DSPLIB_restore_regs();

}

#else

static _Inline void risc_dsp_fir_q31(fir_q31_t* inst, const PTR(q31_t) __restrict pSrc, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
    int32_t i, j;
    uint32_t input_mod_base, input_mod_offset;
    int32_t order = inst->numTaps-1;
    PTR(q31_t) pData = (PTR(q31_t)) inst->pData + inst->shift;
    const PTR(q31_t) pCoeff = (const PTR(q31_t)) inst->pCoeffs;
    dspfx_accum64();
    input_mod_base = ADDR_ALIGNED_BASE32(inst->pData, inst->numTaps);
    input_mod_offset = (((uint32_t)(inst->pData)) - input_mod_base) >> 2;

	for ( i = 0; i < nSamples; i++ )
	{
		dspfx_reset_a64();
		*pData = *pSrc++;
		__builtin_assume(order >= 0);
        PRAGMA_UNROLL_2
		for ( j = 0; j < order; j++ )
		{
			dspfx_a64mac_q31(*pCoeff++, *pData, acc);
			_AGU_CIRC_DEC_SAFE(pData, 1, inst->numTaps, input_mod_offset);
		}
		*pDst++ = dspfx_a64mac_q31(*pCoeff, *pData, acc); pCoeff -= order;
	}

    inst->shift = pData - inst->pData;
}
 
#endif
 
#else //_ARCv2EM_XY_AGU_MEDIUM

#if defined DSPLIB_PRECOMPILE_FIR_Q31_1_TAPS  || defined DSPLIB_PRECOMPILE_FIR_Q31_2_TAPS  || \
    defined DSPLIB_PRECOMPILE_FIR_Q31_3_TAPS  || defined DSPLIB_PRECOMPILE_FIR_Q31_4_TAPS  || \
    defined DSPLIB_PRECOMPILE_FIR_Q31_5_TAPS  || defined DSPLIB_PRECOMPILE_FIR_Q31_6_TAPS  || \
    defined DSPLIB_PRECOMPILE_FIR_Q31_7_TAPS  || defined DSPLIB_PRECOMPILE_FIR_Q31_8_TAPS  || \
    defined DSPLIB_PRECOMPILE_FIR_Q31_9_TAPS  || defined DSPLIB_PRECOMPILE_FIR_Q31_10_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q31_11_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q31_12_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q31_13_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q31_14_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q31_15_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q31_16_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q31_17_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q31_18_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q31_19_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q31_20_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q31_21_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q31_22_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q31_23_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q31_24_TAPS || \
    defined DSPLIB_PRECOMPILE_FIR_Q31_25_TAPS || defined DSPLIB_PRECOMPILE_FIR_Q31_26_TAPS
#define __DSPLIB_PRECOMPILE_FIR_Q31
#endif
 
#if defined __DSPLIB_PRECOMPILE_FIR_Q31

// Special branch with precompiled versions which should be installed
 
#define __DSP_FIR_Q31(nTaps) \
static void risc_dsp_fir_q31_## nTaps ## _taps(fir_q31_t* inst, const q31_t* __restrict pSrc, q31_t* __restrict pDst, uint32_t nSamples) \
{ \
    int32_t i, j, offs; \
    q31_t * __restrict _pDst = pDst; \
    const q31_t * __restrict pCoeff; \
    q31_t * __restrict pState = inst->pData; \
    int32_t N = dspfx_min_q31((nTaps-1), (int32_t)nSamples); \
    dspfx_accum64(); \
    for (offs = 0; offs < N; offs ++) { \
        dspfx_reset_a64(); \
        pCoeff = inst->pCoeffs + nTaps - 1; \
        for (j = offs+1; j < nTaps; j++) { \
            dspfx_a64mac_q31(*pCoeff--, pState[j], acc); \
        } \
        for (j = 0; j < offs + 1; j++) { \
            dspfx_a64mac_q31(*pCoeff--, pSrc[j], acc); \
        } \
        *_pDst++ = dspfx_q31_cast_a64(acc); \
    } \
    for (i = 0; i < (int32_t)(nSamples - nTaps + 1); i++)  \
    { \
        dspfx_reset_a64(); \
        pCoeff = inst->pCoeffs + nTaps - 1; \
        _Pragma("unroll 26") \
        for (j = 0; j < nTaps; j++) { \
            dspfx_a64mac_q31(*pCoeff--, pSrc[j+i], acc); \
        } \
        *_pDst++ = dspfx_q31_cast_a64(acc); \
    } \
    for (i = 0; i < nTaps-(int32_t)nSamples; i++) { \
        pState[i] = pState[i+nSamples]; \
    } \
    for (; i < nTaps; i++) { \
        pState[i] = pSrc[i-(nTaps-nSamples)]; \
    } \
}

__DSP_FIR_Q31(1)
__DSP_FIR_Q31(2)
__DSP_FIR_Q31(3)
__DSP_FIR_Q31(4)
__DSP_FIR_Q31(5)
__DSP_FIR_Q31(6)
__DSP_FIR_Q31(7)
__DSP_FIR_Q31(8)
__DSP_FIR_Q31(9)
__DSP_FIR_Q31(10)
__DSP_FIR_Q31(11)
__DSP_FIR_Q31(12)
__DSP_FIR_Q31(13)
__DSP_FIR_Q31(14)
__DSP_FIR_Q31(15)
__DSP_FIR_Q31(16)
__DSP_FIR_Q31(17)
__DSP_FIR_Q31(18)
__DSP_FIR_Q31(19)
__DSP_FIR_Q31(20)
__DSP_FIR_Q31(21)
__DSP_FIR_Q31(22)
__DSP_FIR_Q31(23)
__DSP_FIR_Q31(24)
__DSP_FIR_Q31(25)
__DSP_FIR_Q31(26)

typedef void (*fir_func_q31_t)(fir_q31_t* inst, const q31_t* __restrict pSrc, q31_t* __restrict pDst, uint32_t nSamples);

#define NULL 0
static fir_func_q31_t precompiled_fir_list[26] = {
#if defined DSPLIB_PRECOMPILE_FIR_Q31_1_TAPS
    risc_dsp_fir_q31_1_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_2_TAPS
    risc_dsp_fir_q31_2_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_3_TAPS
    risc_dsp_fir_q31_3_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_4_TAPS
    risc_dsp_fir_q31_4_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_5_TAPS
    risc_dsp_fir_q31_5_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_6_TAPS
    risc_dsp_fir_q31_6_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_7_TAPS
    risc_dsp_fir_q31_7_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_8_TAPS
    risc_dsp_fir_q31_8_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_9_TAPS
    risc_dsp_fir_q31_9_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_10_TAPS
    risc_dsp_fir_q31_10_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_11_TAPS
    risc_dsp_fir_q31_11_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_12_TAPS
    risc_dsp_fir_q31_12_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_13_TAPS
    risc_dsp_fir_q31_13_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_14_TAPS
    risc_dsp_fir_q31_14_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_15_TAPS
    risc_dsp_fir_q31_15_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_16_TAPS
    risc_dsp_fir_q31_16_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_17_TAPS
    risc_dsp_fir_q31_17_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_18_TAPS
    risc_dsp_fir_q31_18_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_19_TAPS
    risc_dsp_fir_q31_19_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_20_TAPS
    risc_dsp_fir_q31_20_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_21_TAPS
    risc_dsp_fir_q31_21_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_22_TAPS
    risc_dsp_fir_q31_22_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_23_TAPS
    risc_dsp_fir_q31_23_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_24_TAPS
    risc_dsp_fir_q31_24_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_25_TAPS
    risc_dsp_fir_q31_25_taps,
#else
    NULL,
#endif
#if defined DSPLIB_PRECOMPILE_FIR_Q31_26_TAPS
    risc_dsp_fir_q31_26_taps,
#else
    NULL,
#endif
};

#endif

static _Inline void risc_dsp_fir_q31(fir_q31_t* inst, const q31_t* __restrict pSrc, q31_t* __restrict pDst, uint32_t nSamples)
{
    if (inst->numTaps <= 0) return;
#if defined(_ARCVER_ARCv2HS) && defined(__Xagu)
	if(ADDR_ALIGNED_BASE32(inst->pData, inst->numTaps*4)==(unsigned int)inst->pData)
	{
    	int32_t i, j;
		int32_t nTaps = inst->numTaps;
		__agu q31_t* __restrict pData = (__agu q31_t* __restrict) inst->pData + inst->shift;
		const q31_t * __restrict pCoeff = inst->pCoeffs;
		dspfx_accum64();

		for ( i = 0; i < nSamples; i++ )
		{
			dspfx_reset_a64();
			*pData = pSrc[i];
//        __builtin_assume(((nTaps*4) % 16) == 0);
#pragma unroll 8
			for ( j = 0; j < nTaps-1; j++ )
			{
				dspfx_a64mac_q31(pCoeff[j], *pData, acc);
				pData = (__xy q31_t * __restrict)_agu_circ_inc((__xy void * __restrict)pData, 4, nTaps*4);
			}
			dspfx_a64mac_q31(pCoeff[j], *pData, acc);
			pDst[i] = dspfx_q31_cast_a64(acc);
		}
    	inst->shift = pData - inst->pData;
    	return;
	}
#endif	 
#if defined(__FXAPI__)
{
    int32_t i, j, offs;
    int32_t nTaps = inst->numTaps;
    q31_t * __restrict _pDst = pDst;
    const q31_t * __restrict pCoeff;
    q31_t * __restrict pState = inst->pData;
    int32_t N = dspfx_min_q31((nTaps-1), (int32_t)nSamples);
    dspfx_accum64();
    //if (nTaps <= 0) return;
    // Initial part for FIR filter: samples are given from INPUT and STATE buffers
    // 2-samples output loop
    PRAGMA_NO_UNROLL
    for (offs = 0; offs < N; offs ++) {
        dspfx_reset_a64();
        pCoeff = inst->pCoeffs+nTaps -1;
        for (j = offs+1; j < nTaps; j++) {
            dspfx_a64mac_q31(*pCoeff--, pState[j], acc);
        }
        for (j = 0; j < offs + 1; j++) {
            dspfx_a64mac_q31(*pCoeff--, pSrc[j], acc);
        }
        *_pDst++ = dspfx_q31_cast_a64(acc);
    }
    // Main part for FIR filter: samples are given from INPUT
    PRAGMA_NO_UNROLL
    for (i = 0; i < (int32_t)(nSamples - nTaps + 1); i++) 
    {    
        dspfx_reset_a64();
        pCoeff = inst->pCoeffs + nTaps - 1;
#pragma unroll 2
        for (j = 0; j < nTaps; j++) {
            dspfx_a64mac_q31(*pCoeff--, pSrc[j+i], acc);
        }
        *_pDst++ = dspfx_q31_cast_a64(acc);
    }
    // STATE buffer re-initialization with last samples from INPUT buffer
    for (i = 0; i < nTaps-(int32_t)nSamples; i++) {
        pState[i] = pState[i+nSamples];
    }
    for (; i < nTaps; i++) {
        pState[i] = pSrc[i-(nTaps-nSamples)];
    }
}
#else
{
    int32_t j;
    uint32_t i, k;
    const q31_t* pCoeff;
    uint32_t order = inst->numTaps-1;
    q31_t* pDataNew = inst->pData + inst->shift;
    q31_t* pBufLast = inst->pData + order;
    q31_t* pBufFirst = inst->pData;
    uint32_t shift= inst->shift;
    q31_t* p;    
    dspfx_accum64()
    PRAGMA_NO_UNROLL
    for ( i = 0; i < nSamples; i++ )
    {
        dspfx_reset_a64();
        //add new state sample
        *pDataNew = *pSrc++;
        // initialize coefficients pointer
        pCoeff = inst->pCoeffs;
        // initialize state vector pointer
        p = pDataNew;
        // run through first half of cyclic buffer of states
        for ( j = shift; j >= 0  ; j-- )
        {
            dspfx_a64mac_q31(*pCoeff++, *p--, acc);
        }

        //run through second half of cyclic buffer of states
        p=pBufLast;
        for ( k = order; k > shift ; k-- )
        {
            dspfx_a64mac_q31(*pCoeff++, *p--, acc);
        }
        // extract output sample from accumulator
        *pDst++  = dspfx_q31_cast_a64(acc);
        // cyclic buffer processing
        MODULE_ADDR_INC(pDataNew, pBufFirst, pBufLast); // wrap around cycle buffer
        shift=pDataNew-pBufFirst;                       // caclulate "shift" value
    }
    inst->shift = (uint16_t)(shift);
}
#endif
}

#endif //_ARCv2EM_XY_AGU_MEDIUM

#define XY_CF_ADDR 0
#define XY_ST_ADDR 0

/**
 * Q31 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_fir_q31(fir_q31_t* inst, const PTR(q31_t) __restrict pSrc, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
    if (inst->numTaps == 0) return;
#if defined(_ARC_AS210_mac32)
    if ((inst->numTaps < 2) || (inst->numTaps >= (XY_REGION_SIZE/sizeof(q31_t))))
    {
        risc_dsp_fir_q31(inst,  pSrc, pDst, nSamples);
    }else{
        int32_t XY_IN_ADDR = inst->numTaps;
        int32_t XY_OUT_ADDR = inst->numTaps;
        int32_t FIR_Q31_BLOCK_SIZE = (XY_REGION_SIZE/sizeof(q31_t) - inst->numTaps);
        int32_t Samples = (int32_t) nSamples;

        // Load filter coefficients to XY memory
        memcpy_StoX_int32(XY_CF_ADDR, (int32_t*)(void*)(inst->pCoeffs), inst->numTaps);
        // fill XY memory with state
        memcpy_StoY_int32(inst->fir_q31_ll.xyptr_states, (int32_t*)(void*)inst->pData, inst->numTaps);
        while (Samples > 0) {
            int32_t block_size = (Samples > FIR_Q31_BLOCK_SIZE) ? FIR_Q31_BLOCK_SIZE:Samples;
            // fill XY memory with input data
            memcpy_StoX_int32(XY_IN_ADDR, (int32_t*)(void*)pSrc, block_size);
            // call LL FIR filter function
            dsp_fir_q31_XtoY_cfXstY_MAC32x32(&inst->fir_q31_ll, XY_IN_ADDR, XY_OUT_ADDR, block_size);
            // Store output data
            memcpy_YtoS_int32((int32_t*)(void*)pDst, XY_OUT_ADDR, block_size );
            Samples -= block_size;
            pSrc += block_size;
            pDst += block_size;
        };
        // Store filter state
        memcpy_YtoS_int32((int32_t*)(void*)inst->pData, inst->fir_q31_ll.xyptr_states, inst->numTaps);
    }
#else

#if defined(__DSPLIB_PRECOMPILE_FIR_Q31)
    if ( precompiled_fir_list[inst->numTaps-1] != NULL) {
       (*precompiled_fir_list[inst->numTaps-1])(inst,  pSrc, pDst, nSamples);
    } else {
        risc_dsp_fir_q31(inst,  pSrc, pDst, nSamples);
    }
#else
    risc_dsp_fir_q31(inst,  pSrc, pDst, nSamples);
#endif

#endif

}
 
/**
 * initialization for Q31 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param pCoeffs - pointer to coefficients vector. Vector length: nTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_fir_getsize_q31()" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_fir_init_q31(fir_q31_t* inst, uint16_t nTaps, const PTR(q31_t) pCoeffs, PTR(q31_t) pState)
{
    inst->numTaps = nTaps;
    inst->pData = pState;
    inst->pCoeffs = (q31_t*) pCoeffs;
    inst->shift = 0;
    _MEM_FILL_Q31(pState,0,nTaps);
#if defined(_ARC_AS210_mac32)
    // Init low-level FIR filter
    dsp_fir_init_q31_cfXstY_MAC32x32(&inst->fir_q31_ll, nTaps, XY_CF_ADDR, XY_ST_ADDR);

#endif
    return DSP_ERR_OK;
}

void dsp_fir_delete_q31(fir_q31_t* inst) {};

void _DSPLIB_XYCC dsp_fir_init_q31_ext(  fir_q31_t *inst,  uint16_t nTaps,  PTR(q31_t) pCoeffs,  PTR(q31_t) pState,  uint32_t nSamples)
{
    dsp_fir_init_q31(inst, nTaps, pCoeffs, pState);
    return;
}

#endif //!defined(EXCLUDE_DSP_FIR_Q31)
