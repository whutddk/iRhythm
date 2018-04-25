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

#if !defined(EXCLUDE_DSP_FIR_INTERPOLATE_Q31)
#pragma Code("dsplib_dsp_fir_interpolate_q31")

#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif

static _Inline void risc_dsp_fir_interpolate_q31(fir_intdec_q31_t *inst, const PTR(q31_t)__restrict  pSrc, PTR(q31_t)__restrict  pDst, uint32_t nSamples);

/**
 * Q31 real interpolation
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_fir_interpolate_q31(fir_intdec_q31_t *inst, const PTR(q31_t) __restrict  pSrc, PTR(q31_t) __restrict  pDst, uint32_t nSamples){
#if defined(_ARC_AS210_mac32)
	#define XY_CF_ADDR 0
	#define XY_ST_ADDR 0
    if ((inst->fir_intdec_q31_xy.filter.numTaps - (XY_REGION_SIZE/(sizeof(q31_t)))) <= inst->factor)
    {
        risc_dsp_fir_interpolate_q31(inst,  pSrc, pDst, nSamples);
    }else{
        int32_t XY_IN_ADDR = inst->fir_intdec_q31_xy.filter.numTaps;
        int32_t XY_OUT_ADDR = inst->fir_intdec_q31_xy.filter.numTaps;
        int32_t FIR_Q31_INTERPOLATE_BLOCK_SIZE = ((XY_REGION_SIZE/sizeof(q31_t) - inst->fir_intdec_q31_xy.filter.numTaps))/inst->factor;
        int32_t Samples = (int32_t) nSamples;
        // Load filter coefficients to XY memory
        memcpy_StoX_int32(XY_CF_ADDR, (int32_t*)(void*)(inst->filter.pCoeffs), inst->filter.numTaps);
        // Add zeros to complete filter coefficients
        _BurstXMem(XY_CF_ADDR + inst->filter.numTaps, 0, inst->fir_intdec_q31_xy.filter.numTaps - inst->filter.numTaps);
        // fill XY memory with state
        memcpy_StoY_int32(inst->fir_intdec_q31_xy.filter.xyptr_states, (int32_t*)(void*)inst->filter.pData, inst->fir_intdec_q31_xy.dly_line_size);
        while (Samples > 0) {
            int32_t block_size;
            block_size = (Samples > FIR_Q31_INTERPOLATE_BLOCK_SIZE) ? FIR_Q31_INTERPOLATE_BLOCK_SIZE:Samples;
            // fill XY memory with input data
            memcpy_StoX_int32(XY_IN_ADDR, (int32_t*)(void*)pSrc, block_size);            
            // call LL FIR filter function
            dsp_fir_interpolate_q31_XtoY_cfXstY_MAC32x32(&inst->fir_intdec_q31_xy, XY_IN_ADDR, XY_OUT_ADDR, block_size);
            // Store output data
            memcpy_YtoS_int32((int32_t*)(void*)pDst, XY_OUT_ADDR, dspfx_mpy_i32(block_size, inst->factor) );
            Samples -= block_size;
            pSrc += block_size;
            pDst +=  dspfx_mpy_i32(block_size, inst->factor);
        };
        // Store filter state
        memcpy_YtoS_int32((int32_t*)(void*)inst->filter.pData, inst->fir_intdec_q31_xy.filter.xyptr_states, inst->fir_intdec_q31_xy.dly_line_size);
    }

#else
    risc_dsp_fir_interpolate_q31(inst,  pSrc, pDst, nSamples);
#endif
}

#if defined _ARCv2EM_XY && defined __FXAPI__

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM

static _Inline void risc_dsp_fir_interpolate_q31(fir_intdec_q31_t *inst, const PTR(q31_t)__restrict  pSrc, PTR(q31_t)__restrict  pDst, uint32_t nSamples)
{
	int32_t i, j, k;
	fir_q31_t* pFilter = &(inst->filter);
	uint32_t numTaps = pFilter->numTaps;
	uint32_t factor = inst->factor;
	int32_t dly_line_size = inst->dly_line_size;
	uint32_t factor_threshold = numTaps - (dly_line_size - 1) * factor;
	uint32_t alignment_mask = ((4 << (31 - dspfx_norm_q31((int32_t)dly_line_size))) - 1);
	dspfx_accum64();

	_DSPLIB_store_regs(5, 12, 4);

	_set_agu_ap0(pSrc);
	_set_agu_ap1(pDst);
	_set_agu_ap2(pFilter->pData + pFilter->shift);
	_set_agu_ap3(pFilter->pData + pFilter->shift);
	_set_agu_ap4(pFilter->pCoeffs);

	_set_agu_os0(sizeof(q31_t));
	_set_agu_os1(dly_line_size*sizeof(q31_t) - 1);
	_set_agu_os2((dly_line_size-1)*factor - 1);

	_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod1(1, AGU_DT_q31, AGU_AD_INC_q31(1));

	_set_agu_mod2(2, AGU_DT_q31, AGU_AD_WRAP_INC_RR(0, 1));
	_set_agu_mod3(3, AGU_DT_q31, AGU_AD_WRAP_DEC_RR(0, 1));
	_set_agu_mod4(3, AGU_DT_q31, AGU_AD_WRAP_INC_RR(0, 1));

	_set_agu_mod5(4, AGU_DT_q31, AGU_AD_INC_q31(factor));
	_set_agu_mod6(4, AGU_DT_q31, AGU_AD_DEC_q31_REG(2));
	_set_agu_mod7(4, AGU_DT_q31, AGU_AD_DEC_q31(factor));

	if (((uint32_t)pFilter->pData & alignment_mask) == 0) {

#pragma nounroll
		for ( i = 0; i < nSamples; i++)
		{
			_st_agu_u2(_agu_u0());
#pragma nounroll
			for ( j = 0; j < factor; j++ ) {
				dspfx_reset_a64();
#pragma nounroll
				for (k = 0; k < dly_line_size-1; k++)
				{
					dspfx_a64mac_q31(_agu_u3(), _agu_u5(), acc);
				}
				if (j < factor_threshold)
					dspfx_a64mac_q31(_agu_u3(), _agu_u6(), acc);
				else 
					_add(_agu_u3(), _agu_u6());
				_st_agu_u1(dspfx_q31_cast_a64(acc));
			}
			_add(_agu_u4(), _agu_u7());
		}
		pFilter->shift = (q31_t*)_agu_ap2() - pFilter->pData;
	} else {
		uint32_t pDataOffset = ((uint32_t)pFilter->pData & alignment_mask) >> 2;
		_set_agu_os3(pDataOffset);

		_set_agu_mod8(2, AGU_DT_q31, AGU_AD_DEC_q31_REG(3));
		_set_agu_mod9(2, AGU_DT_q31, AGU_AD_INC_q31_REG(3));

		_set_agu_mod10(3, AGU_DT_q31, AGU_AD_DEC_q31_REG(3));
		_set_agu_mod11(3, AGU_DT_q31, AGU_AD_INC_q31_REG(3));

#pragma nounroll
		for ( i = 0; i < nSamples; i++)
		{
			_st_agu_u8(_agu_u0());
			_add(_agu_u2(), 0);
			_add(_agu_u9(), 0);
#pragma nounroll
			for ( j = 0; j < factor; j++ ) {
				dspfx_reset_a64();
#pragma nounroll
				for (k = 0; k < dly_line_size-1; k++)
				{
					dspfx_a64mac_q31(_agu_u10(), _agu_u5(), acc);
					_add(_agu_u3(), 0);
					_add(_agu_u11(), 0);
				}
				if (j < factor_threshold)
					dspfx_a64mac_q31(_agu_u10(), _agu_u6(), acc);
				else 
					_add(_agu_u10(), _agu_u6());
				_add(_agu_u3(), 0);
				_add(_agu_u11(), 0);
				_st_agu_u1(dspfx_q31_cast_a64(acc));
			}
			_add(_agu_u10(), _agu_u7());
			_add(_agu_u4(), 0);
			_add(_agu_u11(), 0);
		}
		pFilter->shift = (q31_t*)_agu_ap2() - pFilter->pData;
	}
	_DSPLIB_restore_regs();
}

#else

static _Inline void risc_dsp_fir_interpolate_q31(fir_intdec_q31_t *inst, const PTR(q31_t)__restrict  pSrc, PTR(q31_t)__restrict  pDst, uint32_t nSamples)
{
	int32_t i, j, k;
	fir_q31_t* pFilter = &(inst->filter);
	uint32_t numTaps = pFilter->numTaps;
	uint32_t factor = inst->factor;
	int32_t dly_line_size = inst->dly_line_size;
	uint32_t factor_threshold = numTaps - (dly_line_size - 1) * factor;
	uint32_t alignment_mask = ((4 << (31 - dspfx_norm_q31((int32_t)dly_line_size))) - 1);
	uint32_t align_offset = ((uint32_t)pFilter->pData & alignment_mask) >> 2;

	dspfx_accum64();

	PTR(q31_t) p = (PTR(q31_t))pFilter->pData + pFilter->shift;
	PTR(q31_t) pCoeff = (PTR(q31_t))pFilter->pCoeffs;

	for ( i = 0; i < nSamples; i++)
	{
		*p = *pSrc++;
		for ( j = 0; j < factor; j++ ) {
			dspfx_reset_a64();
			for (k = 0; k < dly_line_size-1; k++)
			{
				dspfx_a64mac_q31(*p, *pCoeff, acc);
				_AGU_CIRC_DEC_SAFE(p, 1, dly_line_size, align_offset);
				pCoeff += factor;
			}
			if (j < factor_threshold) {
				dspfx_a64mac_q31(*p, *pCoeff, acc);
				_AGU_CIRC_DEC_SAFE(p, 1, dly_line_size, align_offset);
				pCoeff -= (dly_line_size-1)*factor - 1;
			}
			else
			{
				_AGU_CIRC_DEC_SAFE(p, 1, dly_line_size, align_offset);
				pCoeff -= (dly_line_size-1)*factor - 1;
			}
			*pDst++ = dspfx_q31_cast_a64(acc);
		}
		_AGU_CIRC_INC_SAFE(p, 1, dly_line_size, align_offset);
		pCoeff -= factor;
	}
	pFilter->shift = p - pFilter->pData;
}

#endif

#else

static _Inline void risc_dsp_fir_interpolate_q31(fir_intdec_q31_t *inst, const q31_t *__restrict  pSrc, q31_t *__restrict  pDst, uint32_t nSamples)
{
    uint32_t i, j;
    fir_q31_t* pFilter = &(inst->filter);
    uint32_t numTaps = pFilter->numTaps;
    q31_t* pBufLast = pFilter->pData + numTaps - 1;
    q31_t* pBufFirst = pFilter->pData;
    q31_t* pData = pFilter->pData-1 + pFilter->shift;    
    uint32_t factor = inst->factor;
    uint32_t numY = dspfx_mpy_i32(nSamples,factor);
    uint32_t rem = factor-1;
    dspfx_accum64()
    dspfx_reset_a64();
    for ( i = 0; i < numY; i++)
    {
        q31_t* p;
        const q31_t *pCoeffs;
        if ((rem++)==(factor-1))
        {
            MODULE_ADDR_INC(pData, pBufFirst, pBufLast);
            *pData = *pSrc++;                        
            rem=0;
        }     
        p = pData;
        dspfx_reset_a64();
        pCoeffs = pFilter->pCoeffs + rem;
        for ( j = rem; j < numTaps; j+=factor)
        {            
            dspfx_a64mac_q31(*p, *pCoeffs, acc);
            MODULE_ADDR_DEC(p, pBufFirst, pBufLast);
            pCoeffs+=factor;
        }
        *pDst++ = dspfx_q31_cast_a64(acc);
    }
    MODULE_ADDR_INC(pData, pBufFirst, pBufLast);
    pFilter->shift = pData - pBufFirst;
}

#endif

/*================== Initialization for REAL Q31 INTERPOLATION Function =====*/

/**
 * initialization for Q31 real interpolation
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param intFactor - interpolation factor
 * \param pCoeffs - pointer to coefficients vector. Vector length: nTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_fir_interpolate_getsize_q31()" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_fir_interpolate_init_q31(fir_intdec_q31_t *inst, uint16_t nTaps, uint8_t intFactor, const PTR(q31_t) pCoeffs, PTR(q31_t) pState)
{
#if defined(_ARC_AS210_mac32)
    uint16_t nTaps_xy; // number of filter taps for XY version of the filter
#endif
    inst->filter.numTaps = nTaps;
    inst->filter.shift = 0;
    inst->filter.pData = pState;    
    inst->filter.pCoeffs = (q31_t *)pCoeffs;
    inst->factor = intFactor;
#if defined _ARCv2EM_XY && defined __FXAPI__
	inst->dly_line_size = (nTaps + intFactor - 1)/intFactor;
#endif

    _MEM_FILL_Q31(pState,0,nTaps);
#if defined(_ARC_AS210_mac32)
    // Init low-level FIR filter
    nTaps_xy = (nTaps/intFactor)*intFactor; // nTaps_xy must be multiplie of intFactor
    if (nTaps%intFactor) nTaps_xy+=intFactor;
    dsp_fir_interpolate_init_q31_cfXstY_MAC32x32(&inst->fir_intdec_q31_xy,  nTaps_xy,  intFactor,  XY_CF_ADDR, XY_ST_ADDR) ;
#endif
    return DSP_ERR_OK;
}

void dsp_fir_interpolate_delete_q31(fir_intdec_q31_t *inst)
{
 // TODO: Place code here
};

dsp_status _DSPLIB_XYCC dsp_fir_interpolate_init_q31_ext(fir_intdec_q31_t *inst, uint8_t intFactor, uint16_t nTaps, PTR(q31_t) pCoeffs, PTR(q31_t) pState, uint32_t nSamples)
{
    return dsp_fir_interpolate_init_q31(inst, nTaps, intFactor, pCoeffs, pState);
}

#endif //!defined(EXCLUDE_DSP_FIR_INTERPOLATE_Q31)
