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

#if !defined(EXCLUDE_DSP_FIR_DECIMATE_F32)
#pragma Code("dsplib_dsp_fir_decimate_f32")

/**
 * Floating-point real decimation
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed on output
 * \result dsp_status
 */

#if defined _ARCv2EM_XY && defined __FXAPI__

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM 
void _DSPLIB_XYCC dsp_fir_decimate_f32(fir_intdec_f32_t *inst, const __xy f32_t * __restrict pSrc, __xy f32_t * __restrict pDst, uint32_t nSamples)
{
    int32_t i, j;
    uint8_t factor = inst->factor;
    fir_f32_t filter = inst->filter;    
    int32_t order = (int32_t)filter.numTaps-1;
    int32_t shift= (int32_t)filter.shift;
	uint32_t pDataOffset = ((uint32_t)filter.pData & 0x000fffff) >> 2;
	uint32_t alignment_mask = ((4 << (31 - dspfx_norm_q31((int32_t)filter.numTaps))) - 1);
	_DSPLIB_store_regs(5, 9, 4);

	_set_agu_ap0(pSrc);
	_set_agu_ap1(pDst);
	_set_agu_ap2(filter.pData + filter.shift);
	_set_agu_ap3(filter.pData + filter.shift);
	_set_agu_ap4(filter.pCoeffs);

	_set_agu_os0(4);
	_set_agu_os1(((uint32_t)filter.numTaps*4) - 1);

	_set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_ui32(1));
	_set_agu_mod1(1, AGU_DT_ui32, AGU_AD_INC_ui32(1));

	if ( (((uint32_t)filter.pCoeffs & alignment_mask) == 0) && (((uint32_t)filter.pData & alignment_mask) == 0) )
	{
		_set_agu_mod2(2, AGU_DT_ui32, AGU_AD_WRAP_DEC_RR(0, 1));
		_set_agu_mod4(3, AGU_DT_ui32, AGU_AD_WRAP_INC_RR(0, 1));
		_set_agu_mod7(4, AGU_DT_ui32, AGU_AD_WRAP_INC_RR(0, 1));

		_st_agu_u4(_agu_u0());
#pragma  clang loop unroll(disable)
		for ( i = 0; i < (int32_t)nSamples; i++ )
		{
            f32_t acc = 0;
			for ( j = order; j >=0; j-- )
			{
				acc += _as_float(_agu_u7()) * _as_float(_agu_u2());
			}
			_st_agu_u1(_as_uint32(acc));

			for (j = 0; j < factor; j++)
			{
				_st_agu_u4(_agu_u0());
			}
			_set_agu_ap2(_agu_ap3());
			_add(_agu_u2(), 0);
		}
		inst->filter.shift = (uint16_t)((((uint32_t)_agu_ap2()) & alignment_mask) >> 2);
	} else 
	{
		_set_agu_os2(pDataOffset);
		_set_agu_os3(filter.numTaps);

		_set_agu_mod2(2, AGU_DT_ui32, AGU_AD_DEC_ui32(1));
		_set_agu_mod3(2, AGU_DT_ui32, AGU_AD_INC_ui32_REG(3));

		_set_agu_mod4(3, AGU_DT_ui32, AGU_AD_WRAP_INC_RR(0, 1));
		_set_agu_mod5(3, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(2));
		_set_agu_mod6(3, AGU_DT_ui32, AGU_AD_INC_ui32_REG(2));
	 
		_set_agu_mod7(4, AGU_DT_ui32, AGU_AD_INC_ui32(1));
		_set_agu_mod8(4, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(3));

		_st_agu_u5(_agu_u0());
#pragma  clang loop unroll(disable)
		for ( i = 0; i < (int32_t)nSamples; i++ )
		{
			f32_t acc = 0;
			f32_t* pData;

			for ( j = shift; j >=0; j-- )
			{
				acc += _as_float(_agu_u7()) * _as_float(_agu_u2());
			}
			_add(_agu_u3(), 0); //        p=pBufLast;
			for ( j = order; j > shift; j-- )
			{
				acc += _as_float(_agu_u7()) * _as_float(_agu_u2());
			}
			_add(_agu_u8(), 0);
			_st_agu_u1(_as_uint32(acc));

			for (j = 0; j < factor; j++)
			{
				_add(_agu_u4(),0);
				_add(_agu_u6(),0);
				_st_agu_u5(_agu_u0());
			}
			pData = _agu_ap3();
			shift= (((uint32_t)pData) & 0x000fffff) >> 2;
			_set_agu_ap2(pData + pDataOffset);
		}
		inst->filter.shift = (uint16_t)(shift);
	}
	_DSPLIB_restore_regs();
}

#else

void _DSPLIB_XYCC dsp_fir_decimate_f32(fir_intdec_f32_t *inst, const PTR(f32_t) __restrict pSrc, PTR(f32_t) __restrict pDst, uint32_t nSamples)
{
    int32_t i, j;
    uint8_t factor = inst->factor;
    fir_f32_t *filter = &inst->filter;
    int32_t nTaps = (int32_t)filter->numTaps;
    uint32_t input_mod_base = ADDR_ALIGNED_BASE32(filter->pData, nTaps);
    uint32_t input_mod_offset = (((uint32_t)(filter->pData))  - input_mod_base) >> 2;
    const PTR(f32_t) pC = (PTR(f32_t))filter->pCoeffs;
    PTR(f32_t) p = (PTR(f32_t))filter->pData + filter->shift;

    *p = *pSrc++;

	for ( i = 0; i < (int32_t)nSamples; i++ )
	{
		f32_t acc = 0.0;
		for ( j = 0; j < nTaps-1; j++ )
		{
			acc += (*pC++) * (*p);
			_AGU_CIRC_DEC_SAFE(p, 1, nTaps, input_mod_offset);
		}
		*pDst++ = acc + (*pC) * (*p);
		pC -= nTaps - 1;

		for (j = 0; j < factor-1; j++)
		{
			*p = *pSrc++;
			_AGU_CIRC_INC_SAFE(p, 1, nTaps, input_mod_offset);
		}
		*p = *pSrc++;
	}

	filter->shift = p - filter->pData;
}

#endif

#else

void dsp_fir_decimate_f32(fir_intdec_f32_t *inst, const f32_t * __restrict pSrc, f32_t * __restrict pDst, uint32_t nSamples)
{
    uint32_t i, j;
    fir_f32_t filter = inst->filter;    
    const f32_t* pCoeff = filter.pCoeffs;
    f32_t* pData = filter.pData + filter.shift;
    uint16_t numTaps = filter.numTaps;
    uint8_t factor = inst->factor;
    f32_t* pBufLast = filter.pData + numTaps-1;
    f32_t* pBufFirst = filter.pData;
    f32_t acc;

    *pData = *pSrc++;
    PRAGMA_NO_UNROLL
    for ( i = 0; i < nSamples; i++ )
    {
        f32_t*  p=pData;
        const f32_t* pC = pCoeff;        
        acc = 0;
        p = pData;
        for ( j = 0; j < numTaps; j++ )
        {
            MAC_F32(acc, *pC++, *p);
            MODULE_ADDR_DEC(p, pBufFirst, pBufLast);
        }
        *pDst++ = acc;
        for (j = 0; j < factor; j++)
        {
            MODULE_ADDR_INC(pData, pBufFirst, pBufLast);
            *pData=*pSrc++;
        }
    }
    inst->filter.shift = pData - pBufFirst;
}

#endif

/*================== Initialization for REAL F32 DECIMATION Function ========*/

/**
 * initialization for floating-point real decimation
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param decFactor - decimation factor
 * \param pCoeffs - pointer to coefficients vector. Vector length: nTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_fir_decimate_getsize_f32" function. 
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_fir_decimate_init_f32(fir_intdec_f32_t *inst, uint16_t nTaps, uint8_t decFactor, const PTR(f32_t) pCoeffs, PTR(f32_t) pState)
{
    inst->filter.numTaps = nTaps;
    inst->filter.shift = 0;
    inst->filter.pData = pState;
    inst->filter.pCoeffs = ( f32_t *)pCoeffs;
    inst->factor=decFactor;
    _MEM_FILL_F32(pState, 0.0f, nTaps);
    return DSP_ERR_OK;
}


void dsp_fir_decimate_delete_f32(fir_intdec_f32_t *inst)
{
 // TODO: Place code here
};


/**
 * initialization for f32 real decimation
 * API Extension function
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param decFactor - decimation factor
 * \param pCoeffs - pointer to coefficients vector. Vector length: nTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_fir_decimate_getsize_f32()" function. 
 * \param nSamples - number of samples to be processed on input
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_fir_decimate_init_f32_ext( fir_intdec_f32_t *inst,  uint16_t nTaps,  uint8_t decFactor,  PTR(f32_t) pCoeffs, PTR(f32_t) pState,  uint32_t nSamples)
{
    dsp_status sResult;
    sResult = dsp_fir_decimate_init_f32(inst, nTaps, decFactor, pCoeffs, pState);
    if (nSamples % decFactor != 0) return DSP_ERR_LENGTH_NOT_SUPPORTED;
    return sResult;
}


/**
 * f32 real decimation
 * API Extension function
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed on input
 */

void _DSPLIB_XYCC dsp_fir_decimate_f32_ext(fir_intdec_f32_t *inst, const PTR(f32_t) pSrc, PTR(f32_t) pDst, uint32_t nSamples)
{
    uint32_t nOutSamples = nSamples / inst->factor;
    dsp_fir_decimate_f32(inst, pSrc, pDst, nOutSamples);
};

#endif //!defined(EXCLUDE_DSP_FIR_DECIMATE_F32)
