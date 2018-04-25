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

#if !defined(EXCLUDE_DSP_FIR_Q7)

#include "dsplib_primitives.h"

/*================== REAL Q7 FIR Function ==================================*/
#define MODULE_ADDR_INC(p, pFirst , pLast) if (p==pLast) p=pFirst; else p++;
#define MODULE_ADDR_DEC(p, pFirst , pLast) if (p==pFirst) p=pLast; else p--;

#pragma Code("dsplib_dsp_fir_q7")
/**
 * Q7 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */
#ifdef _ARCv2EM_XY

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM
void _DSPLIB_XYCC dsp_fir_q7(fir_q7_t* inst, const __xy q7_t* __restrict pSrc, __xy q7_t* __restrict pDst, uint32_t nSamples)
{
    int32_t i, j;
    uint32_t norm_ntaps;
    uint32_t input_mod_base, input_mod_offset;
    uint32_t order = inst->numTaps-1;
    uint32_t shift= inst->shift;
    dspfx_accum16();
    norm_ntaps = 31 - dspfx_norm_q31(inst->numTaps);
    input_mod_base = ((uint32_t)(inst->pData)) & ~((uint32_t)(1<<norm_ntaps) - 1);
    input_mod_offset = (((uint32_t)(inst->pData)) - input_mod_base);

    _DSPLIB_store_regs(4, 8, 4);

    _set_agu_ap0((uint32_t)pSrc);
    _set_agu_ap1((uint32_t)pDst);
    _set_agu_ap2((uint32_t)(inst->pData + shift));
    _set_agu_ap3((uint32_t)inst->pCoeffs);

    _set_agu_os0(sizeof(q7_t));
    _set_agu_os1((inst->numTaps*sizeof(q7_t)-1));
    _set_agu_os2(input_mod_offset);
    _set_agu_os3(order);

    _set_agu_mod0(0, AGU_DT_i32_cast_i8, AGU_AD_INC_q7(1)); // input increment
    _set_agu_mod1(1, AGU_DT_q7, AGU_AD_INC_q7(1)); // output increment


    _set_agu_mod3(2, AGU_DT_i32_cast_i8, AGU_AD_WRAP_DEC_RR(0, 1)); // delay line modulo addressing
    _set_agu_mod5(2, AGU_DT_i32_cast_i8, AGU_AD_INC_q7(0)); // delay line non-modify
    _set_agu_mod8(2, AGU_DT_i8, AGU_AD_INC_q7(0)); // delay line non-modify

    _set_agu_mod6(3, AGU_DT_i32_cast_i8, AGU_AD_INC_q7(1)); // coefficients increment
    _set_agu_mod7(3, AGU_DT_i32_cast_i8, AGU_AD_DEC_q7_REG(3)); // coefficients decrement

    if (input_mod_offset == 0){

#pragma  clang loop unroll(disable)
        for ( i = 0; i < nSamples; i++ )
        {
            dspfx_reset_a16();

            _st_agu_u8(_agu_u0());

            for ( j = 0; j < order  ; j++ )
            {
                dspfx_a16mac_q7(_agu_u6(), _agu_u3(), acc);
            }

            dspfx_a16mac_q7(_agu_u7(), _agu_u5(), acc);

            _st_agu_u1(dspfx_q7_cast_a16(acc));

        }

    }else{
        _set_agu_mod2(2, AGU_DT_i32_cast_i8, AGU_AD_DEC_q7_REG(2));
        _set_agu_mod4(2, AGU_DT_i32_cast_i8, AGU_AD_INC_q7_REG(2));

#pragma  clang loop unroll(disable)
        for ( i = 0; i < nSamples; i++ )
        {
            dspfx_reset_a16();

            _st_agu_u8(_agu_u0()); // non-increment

            for ( j = 0; j < order  ; j++ )
            {
                dspfx_a16mac_q7(_agu_u6(), _agu_u2(), acc); // decrement to input_mod_base
                _agu_u3(); // modulo decrement
                _agu_u4(); // increment to actual address
            }

            dspfx_a16mac_q7(_agu_u7(), _agu_u5(), acc); // non-increment

            _st_agu_u1(dspfx_q7_cast_a16(acc));
        }
    }
    shift = ((uint32_t)_agu_ap2() - (uint32_t)(inst->pData));
    inst->shift = (uint16_t)(shift);

    _DSPLIB_restore_regs();
}

#else

void _DSPLIB_XYCC dsp_fir_q7(fir_q7_t* inst, const PTR(q7_t) __restrict pSrc, PTR(q7_t) __restrict pDst, uint32_t nSamples)
{
    int32_t i, j;
    uint32_t input_mod_base, input_mod_offset;
    uint32_t order = inst->numTaps-1;
    PTR(q7_t) pData = (PTR(q7_t)) inst->pData + inst->shift;
    const PTR(q7_t) pCoeff = (const PTR(q7_t)) inst->pCoeffs;
    dspfx_accum16();
    input_mod_base = ADDR_ALIGNED_BASE8(inst->pData, inst->numTaps);
    input_mod_offset = (((uint32_t)(inst->pData)) - input_mod_base);

	for ( i = 0; i < nSamples; i++ )
	{
		dspfx_reset_a16();
		*pData = *pSrc++;
		for ( j = 0; j < order; j++ )
		{
			dspfx_a16mac_q7(*pCoeff++, *pData, acc);
			_AGU_CIRC_DEC_SAFE(pData, 1, inst->numTaps, input_mod_offset);
		}
		dspfx_a16mac_q7(*pCoeff, *pData, acc); pCoeff -= order;
		*pDst++ = dspfx_q7_cast_a16(acc);
	}

    inst->shift = pData - inst->pData;
}

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
void dsp_fir_q7(fir_q7_t* inst, const q7_t* __restrict pSrc, q7_t* __restrict pDst, uint32_t nSamples)
{
    int32_t j;
    uint32_t i, k;
    const q7_t* pCoeff;
    uint32_t order = inst->numTaps-1;
    q7_t* pDataNew = inst->pData + inst->shift;
    q7_t* pBufLast = inst->pData + order;
    q7_t* pBufFirst = inst->pData;
    uint32_t shift= inst->shift;
    q7_t* p;    
    dspfx_accum16()
    INIT_MACMODE_LSP();
    for ( i = 0; i < nSamples; i++ )
    {
        dspfx_reset_a16();
        *pDataNew = *pSrc++;
        p = pDataNew;
        pCoeff = inst->pCoeffs;
        for ( j = shift; j >= 0  ; j-- )
        {
            dspfx_a16mac_q7(*pCoeff++, *p--, acc);
        }

        p=pBufLast;

        for ( k = order; k > shift ; k-- )
        {
            dspfx_a16mac_q7(*pCoeff++, *p--, acc);
        }
        *pDst++  = dspfx_q7_cast_a16(acc);
        MODULE_ADDR_INC(pDataNew, pBufFirst, pBufLast);
        shift=pDataNew-pBufFirst;
    }
    inst->shift = (uint16_t)(shift);
}
#endif //_ARCv2EM_XY_AGU_MEDIUM
/*================== REAL Q7 FIR INIT Function =============================*/

/**
 * initialization for Q7 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param pCoeffs - pointer to coefficients vector. Vector length: nTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_fir_getsize_q7" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_fir_init_q7(fir_q7_t* inst, uint16_t nTaps, const PTR(q7_t) pCoeffs, PTR(q7_t) pState)
{
    inst->numTaps = nTaps;
    inst->pData = pState;
    inst->pCoeffs = (q7_t*) pCoeffs;
    inst->shift = 0;
    _MEM_FILL_Q7(pState,0,nTaps);
    return DSP_ERR_OK;
}


void dsp_fir_delete_q7(fir_q7_t* inst)
{
 // TODO: Place code here
};


void _DSPLIB_XYCC dsp_fir_init_q7_ext(  fir_q7_t *inst,  uint16_t nTaps,  PTR(q7_t) pCoeffs,  PTR(q7_t) pState,  uint32_t nSamples)
{
    dsp_fir_init_q7(inst,  nTaps, pCoeffs,  pState);
    return;
}

#endif //!defined(EXCLUDE_DSP_FIR_Q7)
