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

#if !defined(EXCLUDE_DSP_FIR_Q15_COMPLEX)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#define XY_CF_ADDR 0
#define XY_ST_ADDR 0
#endif

/*================== COMPLEX Q15 FIR Function ===============================*/

static _Inline void risc_dsp_fir_q15_complex(fir_q15_t* inst, const PTR(q15_t) __restrict  pSrc, PTR(q15_t) __restrict  pDst, uint32_t nSamples);

#define MODULE_ADDR_INC2(p, pFirst , pLast) if (p==pLast) p=pFirst; else p+=2;
#define MODULE_ADDR_DEC2(p, pFirst , pLast) if (p==pFirst) p=pLast; else p-=2;

#pragma Code("dsplib_dsp_fir_q15_complex")
/**
 * Q15 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_fir_q15_complex(fir_q15_t* inst, const PTR(q15_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples){
#if defined(_ARC_AS210_mac16)
    if (inst-> numTaps < 2 || (inst-> numTaps >= (XY_REGION_SIZE/(2*sizeof(q15_t))))){
        risc_dsp_fir_q15_complex(inst,  pSrc, pDst, nSamples);
    }else{
        int32_t XY_IN_ADDR = inst->numTaps*2;
        int32_t XY_OUT_ADDR = inst->numTaps*2;
        int32_t FIR_Q15_COMPLEX_BLOCK_SIZE = ((XY_REGION_SIZE/(2*sizeof(q15_t)) - inst->numTaps*2));
        int32_t Samples = (int32_t) nSamples;
        // Load filter coefficients to XY memory
        memcpy_StoX_int16(XY_CF_ADDR, (int16_t*)(void*)(inst->pCoeffs), inst->numTaps*2);
        // fill XY memory with state
        memcpy_StoY_int16(inst->fir_q15_ll.xyptr_states, (int16_t*)(void*)inst->pData, inst->numTaps*2);
        while (Samples > 0) {
            int32_t block_size = (Samples > FIR_Q15_COMPLEX_BLOCK_SIZE) ? FIR_Q15_COMPLEX_BLOCK_SIZE:Samples;
            // fill XY memory with input data
            memcpy_StoX_int16(XY_IN_ADDR, (int16_t*)(void*)pSrc, block_size*2 );
            // call LL FIR filter function
            dsp_fir_q15_complex_XtoY_cfXstY_DMAC16x16(&inst->fir_q15_ll, XY_IN_ADDR, XY_OUT_ADDR, block_size);
            // Store output data
            memcpy_YtoS_int16((int16_t*)(void*)pDst, XY_OUT_ADDR, block_size*2);
            Samples -= block_size;
            pSrc += block_size*2;
            pDst += block_size*2;
        };
        // Store filter state
        memcpy_YtoS_int16((int16_t*)(void*)inst->pData, inst->fir_q15_ll.xyptr_states, inst->numTaps*2);
    }
#else
    risc_dsp_fir_q15_complex(inst,  pSrc, pDst, nSamples);
#endif
}
#if defined(__FXAPI__) && defined(_ARCv2EM_XY_AGU_MEDIUM) && defined _XY_HAND_OPT
static _Inline void risc_dsp_fir_q15_complex(fir_q15_t* inst, const __xy q15_t * __restrict pSrc, __xy q15_t * __restrict pDst, uint32_t nSamples) {
    uint32_t i, j;
    uint32_t norm_ntaps;
    uint32_t input_mod_base, input_mod_offset;
    uint32_t numTaps = inst->numTaps;
    uint32_t order = inst->numTaps - 1;
    uint32_t shift = inst->shift;

    _DSPLIB_store_regs(4, 8, 4);
    norm_ntaps = 33 - dspfx_norm_q31(numTaps);
    input_mod_base = ((uint32_t)(inst->pData)) & ~((uint32_t)(1<<norm_ntaps) - 1);
    input_mod_offset = (((uint32_t)(inst->pData)) - input_mod_base) >> 2;

    _set_agu_ap0((uint32_t)pSrc);
    _set_agu_ap1((uint32_t)pDst);
    _set_agu_ap2((uint32_t)(inst->pData + (shift<<1)));
    _set_agu_ap3((uint32_t)(inst->pCoeffs));

    _set_agu_os0(sizeof(v2q15_t));
    _set_agu_os1((inst->numTaps*sizeof(v2q15_t)-1));
    _set_agu_os2(input_mod_offset);
    _set_agu_os3(order);

    _set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_ui32(1)); // input increment
    _set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_INC_ui32(1)); // output increment


    _set_agu_mod3(2, AGU_DT_v2q15, AGU_AD_WRAP_DEC_RR(0, 1)); // delay line modulo addressing
    _set_agu_mod5(2, AGU_DT_v2q15, AGU_AD_INC_ui32(0)); // delay line non-modify

    _set_agu_mod6(3, AGU_DT_v2q15, AGU_AD_INC_ui32(1)); // coefficients increment
    _set_agu_mod7(3, AGU_DT_v2q15, AGU_AD_DEC_ui32_REG(3)); // coefficients decrement

    if (input_mod_offset == 0){

    PRAGMA_NO_UNROLL
        for ( i = 0; i < nSamples; i++ )
        {
#if defined(__GUARD_ACCUM__)
            v2accum40_t acc;
            acc = fx_v2a40_cast_v2q15(fx_create_v2q15(0,0));

            _st_agu_u5(_agu_u0()); // non-increment

            for ( j = 0; j < order  ; j++ )
            {
                acc = fx_v2a40_cmac_cq15(acc, fx_cq15_cast_q31(_agu_u6()), fx_cq15_cast_q31(_agu_u3())); // decrement to input_mod_base
            }
            acc = fx_v2a40_cmac_cq15(acc, fx_cq15_cast_q31(_agu_u7()), fx_cq15_cast_q31(_agu_u5())); // non-increment

            _st_agu_u1(fx_q31_cast_cq15(fx_cq15_cast_v2a40(acc)));
#else // __GUARD_ACCUM__
            v2accum32_t acc;
            acc = fx_v2a32_cast_v2q15(fx_create_v2q15(0,0));

            _st_agu_u5(_agu_u0()); // non-increment

            for ( j = 0; j < order  ; j++ )
            {
                acc = fx_v2a32_cmac_cq15(acc, fx_cq15_cast_q31(_agu_u6()), fx_cq15_cast_q31(_agu_u3())); // decrement to input_mod_base
            }
            acc = fx_v2a32_cmac_cq15(acc, fx_cq15_cast_q31(_agu_u7()), fx_cq15_cast_q31(_agu_u5())); // non-increment

            _st_agu_u1(fx_q31_cast_cq15(fx_cq15_cast_v2a40(acc)));
#endif // __GUARD_ACCUM__
        }

    }else{
        _set_agu_mod2(2, AGU_DT_v2q15, AGU_AD_DEC_ui32_REG(2));
        _set_agu_mod4(2, AGU_DT_v2q15, AGU_AD_INC_ui32_REG(2));
    PRAGMA_NO_UNROLL
        for ( i = 0; i < nSamples; i++ )
        {
#if defined(__GUARD_ACCUM__)
            v2accum40_t acc;
            acc = fx_v2a40_cast_v2q15(fx_create_v2q15(0,0));

            _st_agu_u5(_agu_u0()); // non-increment

            for ( j = 0; j < order  ; j++ )
            {
                acc = fx_v2a40_cmac_cq15(acc, fx_cq15_cast_q31(_agu_u6()), fx_cq15_cast_q31(_agu_u2())); // decrement to input_mod_base
                _agu_u3(); // modulo decrement
                _agu_u4(); // increment to actual address
            }
            acc = fx_v2a40_cmac_cq15(acc, fx_cq15_cast_q31(_agu_u7()), fx_cq15_cast_q31(_agu_u5())); // non-increment

            _st_agu_u1(fx_q31_cast_cq15(fx_cq15_cast_v2a40(acc)));
#else // __GUARD_ACCUM__
            v2accum32_t acc;
            acc = fx_v2a32_cast_v2q15(fx_create_v2q15(0,0));

            _st_agu_u5(_agu_u0()); // non-increment

            for ( j = 0; j < order  ; j++ )
            {
                acc = fx_v2a32_cmac_cq15(acc, fx_cq15_cast_q31(_agu_u6()), fx_cq15_cast_q31(_agu_u2())); // decrement to input_mod_base
                _agu_u3(); // modulo decrement
                _agu_u4(); // increment to actual address
            }
            acc = fx_v2a32_cmac_cq15(acc, fx_cq15_cast_q31(_agu_u7()), fx_cq15_cast_q31(_agu_u5())); // non-increment

            _st_agu_u1(fx_q31_cast_cq15(fx_cq15_cast_v2a40(acc)));
#endif // __GUARD_ACCUM__
        }
    }
    shift = ((uint32_t)_agu_ap2() - (uint32_t)(inst->pData)) >> 2;
    inst->shift = (uint16_t)(shift);

    _DSPLIB_restore_regs();
}
#else // _ARCv2EM_XY_AGU_MEDIUM, __FXAPI__
static _Inline void risc_dsp_fir_q15_complex(fir_q15_t* inst, const PTR(q15_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples)
{
#if defined(__FXAPI__) && defined(__Xdsp_complex)
    int32_t i, j, offs;
    int32_t nTaps = inst->numTaps;
    int32_t N = dspfx_min_q31((nTaps-1), (int32_t)nSamples);

    const PTR(cq15_t)__restrict  pCoeff2q15;
    const PTR(cq15_t)__restrict  pSrc2q15;
    PTR(cq15_t)__restrict  pState2q15;
    PTR(cq15_t)__restrict  pDst2q15;

#if defined(__GUARD_ACCUM__)

    v2accum40_t acc;

     //Initial part for FIR filter: samples are given from INPUT and STATE buffers
     //2-samples output loop
    pDst2q15 = (PTR(cq15_t))pDst;
    PRAGMA_NO_UNROLL
    for (offs = 0; offs < N; offs ++) {
    	acc = fx_v2a40_cast_v2q15(fx_create_v2q15(0, 0));
        pCoeff2q15 = (PTR(cq15_t))(inst->pCoeffs+2*nTaps-2);
		pState2q15 = (PTR(cq15_t))(inst->pData+2*offs+2);

        for (j = offs+1; j < nTaps; j++) {
			acc = fx_v2a40_cmac_cq15(acc, *pState2q15++, *pCoeff2q15--);
        }
		pSrc2q15 = (PTR(cq15_t))pSrc;
        for (j = 0; j < offs + 1; j++) {
			acc = fx_v2a40_cmac_cq15(acc, *pSrc2q15++, *pCoeff2q15--);
        }
        *pDst2q15++ = fx_cq15_cast_v2a40(acc);
    }

     //Main part for FIR filter: samples are given from INPUT
    PRAGMA_NO_UNROLL
    for (i = 0; i < (int32_t)(nSamples - nTaps + 1); i++) 
    {    
    	acc = fx_v2a40_cast_v2q15(fx_create_v2q15(0, 0));
        pCoeff2q15 = (PTR(cq15_t))(inst->pCoeffs + 2*(nTaps-1));
		pSrc2q15 = (PTR(cq15_t))(pSrc+2*i);

        for (j = 0; j < nTaps; j++) {
			acc =  fx_v2a40_cmac_cq15(acc, *pSrc2q15++,*pCoeff2q15--);
        }
        *pDst2q15++ = fx_cq15_cast_v2a40(acc);
    }
#else //!defined(__GUARD_ACCUM__)
    v2accum32_t acc;
     //Initial part for FIR filter: samples are given from INPUT and STATE buffers
     //2-samples output loop
    pDst2q15 = (PTR(cq15_t))pDst;
    PRAGMA_NO_UNROLL
    for (offs = 0; offs < N; offs ++) {
    	acc = fx_v2a32_cast_v2q15(fx_create_v2q15((q15_t)0, (q15_t)0));
        pCoeff2q15 = (PTR(cq15_t))(inst->pCoeffs+2*nTaps-2);
		pState2q15 = (PTR(cq15_t))(inst->pData+2*offs+2);

        for (j = offs+1; j < nTaps; j++) {
			acc = fx_v2a32_cmac_cq15(acc,*pState2q15++, *pCoeff2q15--);
        }
		pSrc2q15 = (PTR(cq15_t))pSrc;
        for (j = 0; j < offs + 1; j++) {
			acc = fx_v2a32_cmac_cq15(acc,*pSrc2q15++, *pCoeff2q15--);
        }
        *pDst2q15++ = fx_cq15_cast_v2a32(acc);
    }

     //Main part for FIR filter: samples are given from INPUT
    PRAGMA_NO_UNROLL
    for (i = 0; i < (int32_t)(nSamples - nTaps + 1); i++) 
    {    
    	acc = fx_v2a32_cast_v2q15(fx_create_v2q15((q15_t)0, (q15_t)0));
        pCoeff2q15 = (PTR(cq15_t))(inst->pCoeffs + 2*(nTaps-1));
		pSrc2q15 = (PTR(cq15_t))(pSrc+2*i);

        for (j = 0; j < nTaps; j++) {
			acc =  fx_v2a32_cmac_cq15(acc,*pSrc2q15++, *pCoeff2q15--);
        }
        *pDst2q15++ = fx_cq15_cast_v2a32(acc);
    }
#endif //!defined(__GUARD_ACCUM__)
    pState2q15 = (PTR(cq15_t))inst->pData;
    pSrc2q15 = (PTR(cq15_t))pSrc;
    // STATE buffer re-initialization with last samples from INPUT buffer
    for (i = 0; i < (nTaps-(int32_t)nSamples); i++) {
        pState2q15[i] = pState2q15[i+(int32_t)nSamples];
    }
    for (; i < nTaps; i++) {
        pState2q15[i] = pSrc2q15[i-(nTaps-nSamples)];
    }

#else //!defined(__FXAPI__) || !defined(__Xdsp_complex)
    uint32_t i, j;
    uint32_t numTaps = 2*inst->numTaps;
    const q15_t* pCoeff;
    q15_t* pDataNew = inst->pData + 2*inst->shift;
    q15_t* pBufLast = inst->pData + numTaps - 2;
    q15_t* pBufFirst = inst->pData;
    q15_t* p; 
    dspfx_accum32()
    INIT_MACMODE();

    nSamples = nSamples << 1;

    PRAGMA_NO_UNROLL
    for ( i = 0; i < nSamples; i+=2 )
    {
        *pDataNew = *pSrc++;
        *(pDataNew+1) = *pSrc++;

        p = pDataNew;       
        pCoeff = inst->pCoeffs;
        dspfx_reset_a32();
        for ( j = 0; j < numTaps; j+=2 )
        {
            dspfx_a32mac_q15(*p, *pCoeff, acc);
            dspfx_a32mac_q15(*(p+1), -*(pCoeff+1), acc);
            MODULE_ADDR_DEC2(p, pBufFirst, pBufLast);
            pCoeff+=2;
        }
        *pDst++ = dspfx_q15_cast_a32(acc);

        p = pDataNew;
        pCoeff = inst->pCoeffs;        
        dspfx_reset_a32();
        for ( j = 0; j < numTaps; j+=2 )
        {
            dspfx_a32mac_q15(*(p+1), *pCoeff, acc);
            dspfx_a32mac_q15(*p,  *(pCoeff+1), acc);
            MODULE_ADDR_DEC2(p, pBufFirst, pBufLast);
            pCoeff+=2;
        }
        *pDst++ = dspfx_q15_cast_a32(acc);

        MODULE_ADDR_INC2(pDataNew, pBufFirst, pBufLast);
    }
    inst->shift = (uint16_t)(pDataNew-pBufFirst)/2;
#endif //defined(__FXAPI__) && defined(__Xdsp_complex)
}
#endif // _ARCv2EM_XY_AGU_MEDIUM, __FXAPI__
/*================== COMPLEX Q15 FIR INIT Function===========================*/

/**
 * initialization for Q15 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param pCoeffs - pointer to coefficients vector. Vector length: nTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_fir_getsize_q15_complex" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_fir_init_q15_complex(fir_q15_t* inst, uint16_t nTaps, const PTR(q15_t) pCoeffs, PTR(q15_t) pState)
{
    if ( (inst==0) || ( pCoeffs==0 ) || ( pState==0 ) ) return DSP_ERR_NULL;
    inst->numTaps = nTaps;
    inst->pData = pState;
    inst->pCoeffs = (q15_t*) pCoeffs;
    inst->shift = 0;
    _MEM_FILL_Q15(pState,0, 2*nTaps);
#if defined(_ARC_AS210_mac16)
    // Init low-level FIR filter
    dsp_fir_init_q15_complex_cfXstY_DMAC16x16(&inst->fir_q15_ll, nTaps, XY_CF_ADDR, XY_ST_ADDR);
#endif
    return DSP_ERR_OK;
}


void dsp_fir_delete_q15_complex(fir_q15_t* inst)
{
 // TODO: Place code here
};

#endif //!defined(EXCLUDE_DSP_FIR_Q15_COMPLEX)
