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

#if !defined(EXCLUDE_DSP_FIR_INTERPOLATE_F32)
#pragma Code("dsplib_dsp_fir_interpolate_f32")

/**
 * Floating-point real interpolation
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

#if defined _ARCv2EM_XY && defined __FXAPI__

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM

void _DSPLIB_XYCC dsp_fir_interpolate_f32(fir_intdec_f32_t *inst, const __xy f32_t * __restrict pSrc,  __xy f32_t * __restrict pDst, uint32_t nSamples)
{
	int32_t i, j, k;
	fir_f32_t* pFilter = &(inst->filter);
	uint32_t numTaps = pFilter->numTaps;
	uint32_t factor = inst->factor;
	int32_t dly_line_size = inst->dly_line_size;
	uint32_t factor_threshold = numTaps - (dly_line_size - 1) * factor;
	uint32_t alignment_mask = ((4 << (31 - dspfx_norm_q31((int32_t)dly_line_size))) - 1);

	_DSPLIB_store_regs(5, 12, 4);

	_set_agu_ap0(pSrc);
	_set_agu_ap1(pDst);
	_set_agu_ap2(pFilter->pData + pFilter->shift);
	_set_agu_ap3(pFilter->pData + pFilter->shift);
	_set_agu_ap4(pFilter->pCoeffs);

	_set_agu_os0(sizeof(f32_t));
	_set_agu_os1(dly_line_size*sizeof(f32_t) - 1);
	_set_agu_os2((dly_line_size-1)*factor - 1);

	_set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_ui32(1));
	_set_agu_mod1(1, AGU_DT_ui32, AGU_AD_INC_ui32(1));

	_set_agu_mod2(2, AGU_DT_ui32, AGU_AD_WRAP_INC_RR(0, 1));
	_set_agu_mod3(3, AGU_DT_ui32, AGU_AD_WRAP_DEC_RR(0, 1));
	_set_agu_mod4(3, AGU_DT_ui32, AGU_AD_WRAP_INC_RR(0, 1));

	_set_agu_mod5(4, AGU_DT_ui32, AGU_AD_INC_ui32(factor));
	_set_agu_mod6(4, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(2));
	_set_agu_mod7(4, AGU_DT_ui32, AGU_AD_DEC_ui32(factor));

	if (((uint32_t)pFilter->pData & alignment_mask) == 0) {

#pragma nounroll
		for ( i = 0; i < nSamples; i++)
		{
			_st_agu_u2(_agu_u0());
#pragma nounroll
			for ( j = 0; j < factor; j++ ) {
				float acc = 0;
#pragma nounroll
				for (k = 0; k < dly_line_size-1; k++)
				{
					acc += _as_float(_agu_u3()) * _as_float(_agu_u5());
				}
				if (j < factor_threshold)
					acc += _as_float(_agu_u3()) * _as_float(_agu_u6());
				else 
					_add(_agu_u3(), _agu_u6());
				_st_agu_u1(_as_uint32(acc));
			}
			_add(_agu_u4(), _agu_u7());
		}
		pFilter->shift = (f32_t*)_agu_ap2() - pFilter->pData;
	} else {
		uint32_t pDataOffset = ((uint32_t)pFilter->pData & alignment_mask) >> 2;
		_set_agu_os3(pDataOffset);

		_set_agu_mod8(2, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(3));
		_set_agu_mod9(2, AGU_DT_ui32, AGU_AD_INC_ui32_REG(3));

		_set_agu_mod10(3, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(3));
		_set_agu_mod11(3, AGU_DT_ui32, AGU_AD_INC_ui32_REG(3));

#pragma nounroll
		for ( i = 0; i < nSamples; i++)
		{
			_st_agu_u8(_agu_u0());
			_add(_agu_u2(), 0);
			_add(_agu_u9(), 0);
#pragma nounroll
			for ( j = 0; j < factor; j++ ) {
				float acc = 0;
#pragma nounroll
				for (k = 0; k < dly_line_size-1; k++)
				{
					acc += _as_float(_agu_u10()) * _as_float(_agu_u5());
					_add(_agu_u3(), 0);
					_add(_agu_u11(), 0);
				}
				if (j < factor_threshold)
					acc += _as_float(_agu_u10()) * _as_float(_agu_u6());
				else 
					_add(_agu_u10(), _agu_u6());
				_add(_agu_u3(), 0);
				_add(_agu_u11(), 0);
				_st_agu_u1(_as_uint32(acc));
			}
			_add(_agu_u10(), _agu_u7());
			_add(_agu_u4(), 0);
			_add(_agu_u11(), 0);
		}
		pFilter->shift = (f32_t*)_agu_ap2() - pFilter->pData;
	}
	_DSPLIB_restore_regs();
}

#else

void _DSPLIB_XYCC dsp_fir_interpolate_f32(fir_intdec_f32_t *inst, const __xy f32_t * __restrict pSrc,  __xy f32_t * __restrict pDst, uint32_t nSamples)
{
	int32_t i, j, k;
	fir_f32_t* pFilter = &(inst->filter);
	uint32_t numTaps = pFilter->numTaps;
	uint32_t factor = inst->factor;
	int32_t dly_line_size = inst->dly_line_size;
	uint32_t factor_threshold = numTaps - (dly_line_size - 1) * factor;
	uint32_t alignment_mask = ((4 << (31 - dspfx_norm_q31((int32_t)dly_line_size))) - 1);
	uint32_t align_offset = ((uint32_t)pFilter->pData & alignment_mask) >> 2;

	PTR(f32_t) p = (PTR(f32_t))pFilter->pData + pFilter->shift;
	PTR(f32_t) pCoeff = (PTR(f32_t))pFilter->pCoeffs;

	for ( i = 0; i < nSamples; i++)
	{
		*p = *pSrc++;
		for ( j = 0; j < factor; j++ ) {
			f32_t acc = 0.0;
			for (k = 0; k < dly_line_size-1; k++)
			{
				acc += (*p) * (*pCoeff);
				_AGU_CIRC_DEC_SAFE(p, 1, dly_line_size, align_offset);
				pCoeff += factor;
			}
			if (j < factor_threshold) {
				acc += (*p) * (*pCoeff);
				_AGU_CIRC_DEC_SAFE(p, 1, dly_line_size, align_offset);
				pCoeff -= (dly_line_size-1)*factor - 1;
			}
			else
			{
				_AGU_CIRC_DEC_SAFE(p, 1, dly_line_size, align_offset);
				pCoeff -= (dly_line_size-1)*factor - 1;
			}
			*pDst++ = acc;
		}
		_AGU_CIRC_INC_SAFE(p, 1, dly_line_size, align_offset);
		pCoeff -= factor;
	}
	pFilter->shift = p - pFilter->pData;
}

#endif

#else

void dsp_fir_interpolate_f32(fir_intdec_f32_t *inst, const f32_t * __restrict pSrc, f32_t * __restrict pDst, uint32_t nSamples)
{
    uint32_t i, j;
    fir_f32_t* pFilter = &(inst->filter); 
    uint32_t numTaps = pFilter->numTaps;
    f32_t* pBufLast = pFilter->pData + pFilter->numTaps - 1;
    f32_t* pBufFirst = pFilter->pData;
    f32_t* pData = pFilter->pData-1 + pFilter->shift;
    
    uint32_t factor = inst->factor;
    uint32_t numY = dspfx_mpy_i32(nSamples,factor);
    uint32_t rem = factor-1;
    float acc=0;
    PRAGMA_NO_UNROLL
    for ( i = 0; i < numY; i++)
    {
        f32_t* p;
        const f32_t *pCoeffs;
        if ((rem++)==(factor-1))
        {
            MODULE_ADDR_INC(pData, pBufFirst, pBufLast);
            *pData = *pSrc++;                        
            rem=0;
        }     
        p = pData;
        acc = 0;
        pCoeffs = pFilter->pCoeffs + rem;
        for ( j = rem; j < numTaps; j+=factor)
        {            
            MAC_F32(acc, *p, *pCoeffs);
            MODULE_ADDR_DEC(p, pBufFirst, pBufLast);
            pCoeffs+=factor;
        }
        *pDst++ = acc;
    }
    MODULE_ADDR_INC(pData, pBufFirst, pBufLast);
    pFilter->shift = pData - pBufFirst;
}

#endif

/*================== Initialization for REAL F32 INTERPOLATION Function =====*/

/**
 * initialization for floating-point real interpolation
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param intFactor - interpolation factor
 * \param pCoeffs - pointer to coefficients vector. Vector length: nTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_fir_interpolate_getsize_f32" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_fir_interpolate_init_f32(fir_intdec_f32_t *inst, uint16_t nTaps, uint8_t intFactor, const PTR(f32_t) pCoeffs, PTR(f32_t) pState)
{
    inst->filter.numTaps = nTaps;
    inst->filter.shift = 0;
    inst->filter.pData = pState;    
    inst->filter.pCoeffs = ( f32_t *)pCoeffs;
    inst->factor = intFactor;
#if defined _ARCv2EM_XY && defined __FXAPI__
	inst->dly_line_size = (nTaps + intFactor - 1)/intFactor;
#endif
    _MEM_FILL_F32(pState,0.0f,nTaps);
    return DSP_ERR_OK;
}

void dsp_fir_interpolate_delete_f32(fir_intdec_f32_t *inst) {};

dsp_status _DSPLIB_XYCC dsp_fir_interpolate_init_f32_ext(fir_intdec_f32_t *inst, uint8_t intFactor, uint16_t nTaps, PTR(f32_t) pCoeffs, PTR(f32_t) pState, uint32_t nSamples)
{
    return dsp_fir_interpolate_init_f32(inst, nTaps, intFactor, pCoeffs, pState);
}

#endif //!defined(EXCLUDE_DSP_FIR_INTERPOLATE_F32)
