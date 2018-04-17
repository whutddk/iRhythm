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

#if !defined(EXCLUDE_DSP_FIR_DECIMATE_Q15)
#pragma Code("dsplib_dsp_fir_decimate_q15")

#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif

static _Inline void risc_dsp_fir_decimate_q15(fir_intdec_q15_t *inst, const PTR(q15_t) __restrict  pSrc, PTR(q15_t) __restrict  pDst, uint32_t nSamples);

/**
 * Q15 real decimation
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed on output
 */

void _DSPLIB_XYCC dsp_fir_decimate_q15(fir_intdec_q15_t *inst, const PTR(q15_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples){
#if defined(_ARC_AS210_mac16)
	#define XY_CF_ADDR 0
	#define XY_ST_ADDR 0
    if ((inst->fir_intdec_q15_xy.filter.numTaps  >= (XY_REGION_SIZE/(sizeof(q15_t)))))
    {
        risc_dsp_fir_decimate_q15(inst,  pSrc, pDst, nSamples);
    }else
    {

        int32_t XY_IN_ADDR = inst->fir_intdec_q15_xy.filter.numTaps;
        int32_t XY_OUT_ADDR = inst->fir_intdec_q15_xy.filter.numTaps;
        int32_t FIR_Q15_DECIMATE_BLOCK_SIZE = ((XY_REGION_SIZE/sizeof(q15_t) - inst->fir_intdec_q15_xy.filter.numTaps))/inst->factor;
        int32_t Samples = (int32_t) nSamples;
        // Load filter coefficients to XY memory
        memcpy_StoX_int16(XY_CF_ADDR, (int16_t*)(void*)(inst->filter.pCoeffs), inst->fir_intdec_q15_xy.filter.numTaps);
    
        // fill XY memory with state
        memcpy_StoY_int16(inst->fir_intdec_q15_xy.filter.xyptr_states, (int16_t*)(void*)inst->filter.pData, inst->fir_intdec_q15_xy.filter.numTaps);
        while (Samples > 0) {
            int32_t block_size;
            block_size = (Samples > FIR_Q15_DECIMATE_BLOCK_SIZE) ? FIR_Q15_DECIMATE_BLOCK_SIZE:Samples;
            // fill XY memory with input data
            memcpy_StoX_int16(XY_IN_ADDR, (int16_t*)(void*)pSrc, dspfx_mpy_i32(block_size, inst->factor));            
            // call LL FIR filter function
            dsp_fir_decimate_q15_XtoY_cfXstY_MAC16x16(&inst->fir_intdec_q15_xy, XY_IN_ADDR, XY_OUT_ADDR, block_size);
            // Store output data
            memcpy_YtoS_int16((int16_t*)(void*)pDst, XY_OUT_ADDR, block_size);
            Samples -= block_size;
            pSrc += dspfx_mpy_i32(block_size, inst->factor);
            pDst += block_size;
        };
        // Store filter state
        memcpy_YtoS_int16((int16_t*)(void*)inst->filter.pData, inst->fir_intdec_q15_xy.filter.xyptr_states, inst->fir_intdec_q15_xy.filter.numTaps);
    }

#else
    risc_dsp_fir_decimate_q15(inst,  pSrc, pDst, nSamples);
#endif
}

#if defined _ARCv2EM_XY && defined __FXAPI__

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM

static _Inline void risc_dsp_fir_decimate_q15(fir_intdec_q15_t *inst, const PTR(q15_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples)
{
	int32_t i, j;
	uint8_t factor = inst->factor;
	fir_q15_t filter = inst->filter;    
	int32_t order = (int32_t)filter.numTaps-1;
	int32_t shift= (int32_t)filter.shift;
	uint32_t pDataOffset = ((uint32_t)filter.pData & 0x000fffff) >> 1;
	uint32_t alignment_mask = ((2 << (31 - dspfx_norm_q31((int32_t)filter.numTaps))) - 1);
	_DSPLIB_store_regs(5, 9, 4);

	_set_agu_ap0(pSrc);
	_set_agu_ap1(pDst);
	_set_agu_ap2(filter.pData + filter.shift);
	_set_agu_ap3(filter.pData + filter.shift);
	_set_agu_ap4(filter.pCoeffs);

	_set_agu_os0(sizeof(q15_t));
	_set_agu_os1(((uint32_t)filter.numTaps*sizeof(q15_t)) - 1);

	_set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(1));
	_set_agu_mod1(1, AGU_DT_q15, AGU_AD_INC_q15(1));

	if ( (((uint32_t)filter.pCoeffs & alignment_mask) == 0) && (((uint32_t)filter.pData & alignment_mask) == 0) )
	{
		_set_agu_mod2(2, AGU_DT_q15, AGU_AD_WRAP_DEC_RR(0, 1));
		_set_agu_mod4(3, AGU_DT_q15, AGU_AD_WRAP_INC_RR(0, 1));
		_set_agu_mod7(4, AGU_DT_q15, AGU_AD_WRAP_INC_RR(0, 1));

		_st_agu_u4(_agu_u0());
#pragma  clang loop unroll(disable)
		for ( i = 0; i < (int32_t)nSamples; i++ )
		{
			dspfx_accum32();
			dspfx_reset_a32();
			for ( j = order; j >=0; j-- )
			{
				dspfx_a32mac_q15(_agu_u7(), _agu_u2(), acc);
			}
			_st_agu_u1(dspfx_q15_cast_a32(acc));

			for (j = 0; j < factor; j++)
			{
				_st_agu_u4(_agu_u0());
			}
			_set_agu_ap2(_agu_ap3());
			_add(_agu_u2(), 0);
		}
		inst->filter.shift = (uint16_t)((((uint32_t)_agu_ap2()) & alignment_mask) >> 1);
	} else 
	{
		_set_agu_os2(pDataOffset);
		_set_agu_os3(filter.numTaps);

		_set_agu_mod2(2, AGU_DT_q15, AGU_AD_DEC_q15(1));
		_set_agu_mod3(2, AGU_DT_q15, AGU_AD_INC_q15_REG(3));

		_set_agu_mod4(3, AGU_DT_q15, AGU_AD_WRAP_INC_RR(0, 1));
		_set_agu_mod5(3, AGU_DT_q15, AGU_AD_DEC_q15_REG(2));
		_set_agu_mod6(3, AGU_DT_q15, AGU_AD_INC_q15_REG(2));

		_set_agu_mod7(4, AGU_DT_q15, AGU_AD_INC_q15(1));
		_set_agu_mod8(4, AGU_DT_q15, AGU_AD_DEC_q15_REG(3));

		_st_agu_u5(_agu_u0());
#pragma  clang loop unroll(disable)
		for ( i = 0; i < (int32_t)nSamples; i++ )
		{
			q15_t* pData;
			dspfx_accum32();
			dspfx_reset_a32();

			for ( j = shift; j >=0; j-- )
			{
				dspfx_a32mac_q15(_agu_u7(), _agu_u2(), acc);
			}
			_add(_agu_u3(), 0); //        p=pBufLast;
			for ( j = order; j > shift; j-- )
			{
				dspfx_a32mac_q15(_agu_u7(), _agu_u2(), acc);
			}
			_add(_agu_u8(), 0);
			_st_agu_u1(dspfx_q15_cast_a32(acc));

			for (j = 0; j < factor; j++)
			{
				_add(_agu_u4(),0);
				_add(_agu_u6(),0);
				_st_agu_u5(_agu_u0());
			}
			pData = _agu_ap3();
			shift= (((uint32_t)pData) & 0x000fffff) >> 1;
			_set_agu_ap2(pData + pDataOffset);
		}
		inst->filter.shift = (uint16_t)(shift);
	}
	_DSPLIB_restore_regs();
}

#else

static _Inline void risc_dsp_fir_decimate_q15(fir_intdec_q15_t *inst, const PTR(q15_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples)
{
    int32_t i, j;
    uint8_t factor = inst->factor;
    fir_q15_t *filter = &inst->filter;
    int32_t nTaps = (int32_t)filter->numTaps;
    uint32_t input_mod_base = ADDR_ALIGNED_BASE16(filter->pData, nTaps);
    uint32_t input_mod_offset = (((uint32_t)(filter->pData))  - input_mod_base) >> 1;
    const PTR(q15_t) pC = (PTR(q15_t))filter->pCoeffs;
    PTR(q15_t) p = (PTR(q15_t))filter->pData + filter->shift;

    *p = *pSrc++;

	for ( i = 0; i < (int32_t)nSamples; i++ )
	{
		dspfx_accum32();
		dspfx_reset_a32();
		for ( j = 0; j < nTaps-1; j++ )
		{
			dspfx_a32mac_q15(*pC++, *p, acc);
			_AGU_CIRC_DEC_SAFE(p, 1, nTaps, input_mod_offset);
		}
		*pDst++ = dspfx_a32mac_q15(*pC, *p, acc);
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

static _Inline void risc_dsp_fir_decimate_q15(fir_intdec_q15_t *inst, const q15_t * __restrict pSrc, q15_t * __restrict pDst, uint32_t nSamples)
{
    uint32_t i, k;
    int32_t j;
    uint8_t factor = inst->factor;
    fir_q15_t filter = inst->filter;    
    const q15_t* pCoeff = filter.pCoeffs;
    q15_t* pData = filter.pData + filter.shift;
    uint16_t order = filter.numTaps-1;
    q15_t* pBufLast = filter.pData + order;
    q15_t* pBufFirst = filter.pData;
    uint32_t shift=filter.shift;
    dspfx_accum32()
    INIT_MACMODE();

    if (((int32_t)factor <= 0) || ((int32_t)filter.numTaps <= 0) || ((int32_t)nSamples <= 0)) return;

    *pData = *pSrc++;
    PRAGMA_NO_UNROLL
    for ( i = 0; i < nSamples; i++ )
    {
        q15_t* p=pData;
        const q15_t* pC = pCoeff;
        dspfx_reset_a32();     

        for ( j = shift; j >=0 ; j-- )
        {
            dspfx_a32mac_q15(*pC++, *p--, acc);
        }
        p=pBufLast;
        for ( k = order; k > shift; k-- )
        {
            dspfx_a32mac_q15(*pC++, *p--, acc);
        }
        *pDst++ = dspfx_q15_cast_a32(acc);

        for (k = 0; k < factor; k++)
        {
            MODULE_ADDR_INC(pData, pBufFirst, pBufLast);
            *pData=*pSrc++;
        }    
        shift=pData-pBufFirst;
    }
    inst->filter.shift = (uint16_t)(shift);
}

#endif

/*================== Initialization for REAL Q15 DECIMATION Function ========*/

/**
 * initialization for Q15 real decimation
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param decFactor - decimation factor
 * \param pCoeffs - pointer to coefficients vector. Vector length: nTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_fir_decimate_getsize_q15()" function. 
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_fir_decimate_init_q15(fir_intdec_q15_t *inst, uint16_t nTaps, uint8_t decFactor, const PTR(q15_t) pCoeffs, PTR(q15_t) pState)
{
    inst->filter.numTaps = nTaps;
    inst->filter.shift = 0;
    inst->filter.pData = pState;
    inst->filter.pCoeffs = ( q15_t *)pCoeffs;
    inst->factor=decFactor;
    _MEM_FILL_Q15(pState,0,nTaps);
#if defined(_ARC_AS210_mac16)
    // Init low-level FIR filter
    dsp_fir_decimate_init_q15_cfXstY_MAC16x16(&inst->fir_intdec_q15_xy,  nTaps,  decFactor,  XY_CF_ADDR, XY_ST_ADDR) ;
#endif
    return DSP_ERR_OK;
}


void dsp_fir_decimate_delete_q15(fir_intdec_q15_t *inst)
{
 // TODO: Place code here
};

/**
 * initialization for Q15 real decimation
 * API Extension function
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param decFactor - decimation factor
 * \param pCoeffs - pointer to coefficients vector. Vector length: nTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_fir_decimate_getsize_q15()" function. 
 * \param nSamples - number of samples to be processed on input
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_fir_decimate_init_q15_ext(  fir_intdec_q15_t *inst,  uint16_t nTaps,  uint8_t decFactor,  PTR(q15_t) pCoeffs,  PTR(q15_t) pState,  uint32_t nSamples)
{   
    dsp_status sResult;
    sResult = dsp_fir_decimate_init_q15(inst, nTaps, decFactor, pCoeffs, pState);
    if (nSamples % decFactor != 0) return DSP_ERR_LENGTH_NOT_SUPPORTED;
    return sResult;
}

/**
 * Q15 real decimation
 * API Extension function
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed on input
 */

void _DSPLIB_XYCC dsp_fir_decimate_q15_ext(fir_intdec_q15_t *inst, const PTR(q15_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples)
{
    uint32_t nOutSamples = nSamples / inst->factor;
    dsp_fir_decimate_q15(inst, pSrc, pDst, nOutSamples);
};

#endif //!defined(EXCLUDE_DSP_FIR_DECIMATE_Q15)
