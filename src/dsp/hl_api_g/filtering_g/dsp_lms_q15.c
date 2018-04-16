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

#if !defined(EXCLUDE_DSP_LMS_Q15)
#pragma Code("dsplib_dsp_lms_q15")

#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif


#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

#define dspfx_v2q15_mpy_v2q15(L, R) fx_v2q15_cast_rnd_v2a40(fx_v2a40_mpy_v2q15(fx_v2q15_cast_q31(L), fx_v2q15_cast_q31(R)))

#if defined _XY_HAND_OPT
static _Inline void risc_dsp_lms_q15(lms_q15_t * inst,  const q15_t * __restrict pSrc,  const q15_t * __restrict pRef,  q15_t * __restrict pOut,  q15_t * __restrict pErr,  uint32_t nSamples)
{
	uint32_t i;
	int32_t j;
	uint16_t numTaps = inst->numTaps;
	uint32_t postshift = inst->postshift;
	q15_t mu = inst->mu;
	q15_t e, alpha;
	q15_t *pState = inst->pState;  
	q15_t *pCoeffs = inst->pCoeffs;

	uint32_t alignment_mask = ((2 << (31 - dspfx_norm_q31((int32_t)numTaps))) - 1);

	_DSPLIB_store_regs(6, 8, 2);

	_set_agu_ap0(pSrc);
	_set_agu_ap1(pRef);
	_set_agu_ap2(pOut);
	_set_agu_ap3(pErr);
	_set_agu_ap5(inst->pState + inst->shift);


	_set_agu_mod0(4, AGU_DT_q15, AGU_AD_INC_q15(0));
	_set_agu_mod1(4, AGU_DT_q15, AGU_AD_INC_q15(1));
	_set_agu_mod3(0, AGU_DT_q15, AGU_AD_INC_q15(1));
	_set_agu_mod4(2, AGU_DT_q15, AGU_AD_INC_q15(0));
	_set_agu_mod5(2, AGU_DT_q15, AGU_AD_INC_q15(1));
	_set_agu_mod6(1, AGU_DT_q15, AGU_AD_INC_q15(1));
	_set_agu_mod7(3, AGU_DT_q15, AGU_AD_INC_q15(1));

	dspfx_accum32();

	if (((uint32_t)pState & alignment_mask) == 0) {
		_set_agu_os0(sizeof(q15_t));
		_set_agu_os1(numTaps*sizeof(q15_t) - 1);
		_set_agu_mod2(5, AGU_DT_q15, AGU_AD_WRAP_INC_RR(0, 1));
#pragma  clang loop unroll(disable)
		for (i = 0; i < nSamples; i++)
		{
			dspfx_reset_a32();

			// Store current input sample to delay line
			_st_agu_u2(_agu_u3());

			// FIR filter processing
			_set_agu_ap4(pCoeffs); // pC = pCoeffs;
			for (j = 0; j < numTaps; j++)
				dspfx_a32mac_q15(_agu_u1(), _agu_u2(), acc);

			// Store output sample
			_st_agu_u4(dspfx_q15_cast_asl_a32(acc, postshift));

			// Calculate error between reference and output sample
			e = dspfx_sub_q15(_agu_u6(), _agu_u5());

			// Calculate 'alpha' factor
			alpha =  dspfx_mpy_rnd_q15(e, mu);

			// Store error sample
			_st_agu_u7(e);

			_set_agu_ap4(pCoeffs); //pC = pCoeffs;
			// pState points to the oldest sample in delay line
			// Update coefficients
			for (j = 0; j < numTaps; j++)
			{
				_st_agu_u1(fx_q31_cast_v2q15(fx_add_v2q15(dspfx_v2q15_mpy_v2q15(alpha, _agu_u2()), fx_v2q15_cast_q31(_agu_u0()))));
			}
		}
		inst->shift = (q15_t*)_agu_ap5() - pState;
	} else {
		int32_t shift = inst->shift;
		_set_agu_mod2(5, AGU_DT_q15, AGU_AD_INC_q15(1));
#pragma  clang loop unroll(disable)
		for (i = 0; i < nSamples; i++)
		{
			dspfx_reset_a32();

			// Store current input sample to delay line
			_st_agu_u2(_agu_u3());
			shift = (q15_t*)_agu_ap5() - pState;
			if (shift == numTaps) {
				shift = 0;
				_set_agu_ap5(pState);
			}

			// FIR filter processing
			_set_agu_ap4(pCoeffs); // pC = pCoeffs;
			for (j = shift; j < numTaps; j++)
				dspfx_a32mac_q15(_agu_u1(), _agu_u2(), acc);
			_set_agu_ap5(pState);
			for (j = 0; j < shift; j++)
				dspfx_a32mac_q15(_agu_u1(), _agu_u2(), acc);

			// Store output sample
			_st_agu_u4(dspfx_q15_cast_asl_a32(acc, postshift));

			// Calculate error between reference and output sample
			e = dspfx_sub_q15(_agu_u6(), _agu_u5());

			// Calculate 'alpha' factor
			alpha =  dspfx_mpy_rnd_q15(e, mu);

			// Store error sample
			_st_agu_u7(e);

			_set_agu_ap4(pCoeffs); //pC = pCoeffs;
			// pState points to the oldest sample in delay line
			// Update coefficients
			for (j = shift; j < numTaps; j++)
				_st_agu_u1(fx_q31_cast_v2q15(fx_add_v2q15(dspfx_v2q15_mpy_v2q15(alpha, _agu_u2()), fx_v2q15_cast_q31(_agu_u0()))));
			_set_agu_ap5(pState);
			for (j = 0; j < shift; j++)
				_st_agu_u1(fx_q31_cast_v2q15(fx_add_v2q15(dspfx_v2q15_mpy_v2q15(alpha, _agu_u2()), fx_v2q15_cast_q31(_agu_u0()))));


		}
		inst->shift = shift;
	}

	_DSPLIB_restore_regs();

	return;
}

#else

static _Inline void risc_dsp_lms_q15(lms_q15_t * inst,  const PTR(q15_t) __restrict pSrc,  const PTR(q15_t) __restrict pRef,  PTR(q15_t) __restrict pOut,  PTR(q15_t) __restrict pErr,  uint32_t nSamples)
{
	uint32_t i;
	int32_t j;
	uint16_t numTaps = inst->numTaps;
	uint32_t postshift = inst->postshift;
	q15_t mu = inst->mu;
	q15_t e, alpha;
	PTR(q15_t) pState = (PTR(q15_t)) inst->pState;
	PTR(q15_t) pCoeffs = (PTR(q15_t))inst->pCoeffs;
	PTR(q15_t) p = (PTR(q15_t)) inst->pState + inst->shift;

	uint32_t alignment_mask = ((2 << (31 - dspfx_norm_q31((int32_t)numTaps))) - 1);
	uint32_t align_offset = ((uint32_t)inst->pState & alignment_mask) >> 1;

	dspfx_accum32();

	for (i = 0; i < nSamples; i++)
	{
		dspfx_reset_a32();

		// Store current input sample to delay line
		*p = *pSrc++;
		_AGU_CIRC_INC_SAFE(p, 1, numTaps, align_offset);

		// FIR filter processing
		for (j = 0; j < numTaps; j++) {
			dspfx_a32mac_q15(*p, *pCoeffs++, acc);
			_AGU_CIRC_INC_SAFE(p, 1, numTaps, align_offset);
		}
		pCoeffs -= numTaps;

		// Store output sample
		*pOut = dspfx_q15_cast_asl_a32(acc, postshift);

		// Calculate error between reference and output sample
		e = dspfx_sub_q15(*pRef++, *pOut++);

		// Calculate 'alpha' factor
		alpha =  dspfx_mpy_rnd_q15(e, mu);

		// Store error sample
		*pErr++ = e;

		// pState points to the oldest sample in delay line
		// Update coefficients
		for (j = 0; j < numTaps; j++)
		{
			q15_t coef = *pCoeffs;
			*pCoeffs++ = dspfx_add_q15(coef, dspfx_mpy_rnd_q15(alpha, *p));
			_AGU_CIRC_INC_SAFE(p, 1, numTaps, align_offset);
		}
		pCoeffs -= numTaps;

		inst->shift = p - pState;
	}
	return;
}

#endif

#else

static _Inline void risc_dsp_lms_q15(lms_q15_t * inst,  const q15_t * __restrict pSrc,  const q15_t * __restrict pRef,  q15_t * __restrict pOut,  q15_t * __restrict pErr,  uint32_t nSamples)
{
#if defined(__FXAPI__) // new optimized implementation

    int32_t i, j, offs;
    int32_t nTaps = inst->numTaps;

    uint32_t postshift = inst->postshift;
    q15_t mu = inst->mu;
    q15_t e = 0;
    q15_t alpha = 0;

    q15_t * __restrict _pOut = pOut;
    q15_t * __restrict pCoeff;
    q15_t * __restrict pState = inst->pState;
    int32_t N = dspfx_min_q31((nTaps-1), (int32_t)nSamples);
    dspfx_accum32();
    // Initial part for FIR filter: samples are given from INPUT and STATE buffers
    // 2-samples output loop
    PRAGMA_NO_UNROLL
    for (offs = 0; offs < N; offs ++) {
        dspfx_reset_a32();
        pCoeff = inst->pCoeffs;
        for (j = offs+1; j < nTaps; j++) {
            dspfx_a32mac_q15(*pCoeff++, pState[j], acc);
        }
        for (j = 0; j < offs + 1; j++) {
            dspfx_a32mac_q15(*pCoeff++, pSrc[j], acc);
        }
        *_pOut = dspfx_q15_cast_asl_a32(acc, postshift);

        // Calculate error between reference and output sample
        e = dspfx_sub_q15(*pRef++, *_pOut++);

        // Calculate 'alpha' factor
        alpha =  dspfx_mpy_rnd_q15(e, mu);

        // Store error sample
        *pErr++ = e;

        pCoeff = inst->pCoeffs;
        for (j = offs+1; j < nTaps; j++) {
            q31_t tmp =  dspfx_add_q15(*pCoeff, dspfx_mpy_rnd_q15(alpha, pState[j]));
            *pCoeff++ = tmp;
        }
        for (j = 0; j < offs + 1; j++) {
            q31_t tmp =  dspfx_add_q15(*pCoeff, dspfx_mpy_rnd_q15(alpha, pSrc[j]));
            *pCoeff++ = tmp;
        }
    }

    // Main part for FIR filter: samples are given from INPUT
    PRAGMA_NO_UNROLL
    for (i = 0; i < (int32_t)(nSamples - nTaps + 1); i++) 
    {
        dspfx_reset_a32();
        pCoeff = inst->pCoeffs;
        for (j = 0; j < nTaps; j++) {
            dspfx_a32mac_q15(*pCoeff++, pSrc[j+i], acc);
        }
        *_pOut = dspfx_q15_cast_asl_a32(acc, postshift);

        // Calculate error between reference and output sample
        e = dspfx_sub_q15(*pRef++, *_pOut++);

        // Calculate 'alpha' factor
        alpha =  dspfx_mpy_rnd_q15(e, mu);

        // Store error sample
        *pErr++ = e;

        pCoeff = inst->pCoeffs;
        for (j = 0; j < nTaps; j++) {
            q31_t tmp =  dspfx_add_q15(*pCoeff, dspfx_mpy_rnd_q15(alpha, pSrc[j+i]));
            *pCoeff++ = tmp;
        }
    }

    // STATE buffer re-initialization with last samples from INPUT buffer
    for (i = 0; i < nTaps-(int32_t)nSamples; i++) {
        pState[i] = pState[i+nSamples];
    }
    for (; i < nTaps; i++) {
        pState[i] = pSrc[i-(nTaps-nSamples)];
    }

    return;

#else // old less effective implementation

    uint32_t i;
    int32_t j;
    uint16_t shift = inst->shift;
    uint16_t numTaps = inst->numTaps;
    uint32_t postshift = inst->postshift;
    q15_t mu = inst->mu;
    q15_t e = 0;
    q15_t alpha = 0;
    q15_t *pState = inst->pState;  
    q15_t *pCoeffs = inst->pCoeffs;
    q15_t *pBufFirst = pState;
    q15_t *pBufLast = pState + numTaps - 1;
    q15_t *pC;

    dspfx_accum32();
    INIT_MACMODE();
    pState = inst->pState + shift;
    PRAGMA_NO_UNROLL
    for (i = 0; i < nSamples; i++)
    {
        dspfx_reset_a32();

        pC = pCoeffs;

        // Store current input sample to delay line
        *pState = *pSrc++;
        MODULE_ADDR_INC(pState, pBufFirst, pBufLast);
        shift = pState - pBufFirst;
        // pState points to the oldest sample in delay line
        
        // FIR filter processing
        for (j = shift; j < numTaps; j++)
            dspfx_a32mac_q15(*pC++, *pState++, acc);
        pState = pBufFirst;
        for (j = 0;     j < shift;   j++)
            dspfx_a32mac_q15(*pC++, *pState++, acc);

        // Store output sample
        *pOut = dspfx_q15_cast_asl_a32(acc, postshift);

        // Calculate error between reference and output sample
        e = dspfx_sub_q15(*pRef++, *pOut++);

        // Calculate 'alpha' factor
        alpha =  dspfx_mpy_rnd_q15(e, mu);

        // Store error sample
        *pErr++ = e;

        pC = pCoeffs;
        // pState points to the oldest sample in delay line
        // Update coefficients
        for (j = shift; j < numTaps; j++)
        {
            q15_t tmp;
            tmp = dspfx_add_q15(*pC, dspfx_mpy_rnd_q15(alpha, *pState++));
            *pC++ = tmp;
        }

        pState = pBufFirst;
        for (j = 0; j < shift; j++)
        {
            q15_t tmp;
            tmp = dspfx_add_q15(*pC, dspfx_mpy_rnd_q15(alpha, *pState++));
            *pC++ = tmp;
        }        
    }
    inst->shift = shift;
    return;

#endif
}

#endif

/**
 * initialization for q15 real LMS filtering
 *
 * \param inst - pointer to instance structure
 * \param numTaps - number of coefficients
 * \param pCoeffs - pointer to coefficients vector. Vector length: numTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_lms_getsize_f32" function.
 * \param mu - adaptation step size
 * \result dsp_status
 */
 
 
dsp_status _DSPLIB_XYCC dsp_lms_init_q15(lms_q15_t * inst,  uint16_t numTaps,  PTR(q15_t) pCoeffs,  PTR(q15_t) pState,  q15_t mu)
{
    inst->numTaps = numTaps;

    inst->pCoeffs = pCoeffs;
    _MEM_FILL_Q15(pCoeffs, 0, numTaps);

    inst->pState = pState;
    _MEM_FILL_Q15(pState,  0, numTaps);

    inst->mu = mu;
    inst->shift = 0;
    inst->postshift = 0;

#if defined(_ARC_AS210_mac16)
#define XY_MAP_STATES  ((XY_REGION_SIZE)/(2*sizeof(q15_t)))
#define XY_MAP_COEFFS  ((XY_REGION_SIZE)/(2*sizeof(q15_t)))
    inst->lms_q15_ll.mu = mu;
    inst->lms_q15_ll.numTaps = numTaps;
    inst->lms_q15_ll.shift = 0;
    inst->lms_q15_ll.xyptr_states = XY_MAP_STATES;
    inst->lms_q15_ll.xyptr_coeffs = XY_MAP_COEFFS;
    inst->lms_q15_ll.postshift = 0;
#endif
    return DSP_ERR_OK;
}


#define XY_MAP_INOUT 0 //x - input, output
#define XY_MAP_REFERR 0 //y - reference, error

/**
 * Q15 real LMS filtering
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[in] pRef - pointer to 'reference' vector. Vector length: nSamples
 * \param[out] pOut - pointer to output vector. Vector length: nSamples
 * \param[out] pErr - pointer to error vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_lms_q15(lms_q15_t * inst,  const PTR(q15_t) __restrict pSrc,  const PTR(q15_t) __restrict pRef,  PTR(q15_t) __restrict pOut,  PTR(q15_t) __restrict pErr,  uint32_t nSamples)
{
#if defined(_ARC_AS210_mac16)

    int32_t LMS_Q15_BLOCK_SIZE = (XY_REGION_SIZE/sizeof(q15_t) - 4*inst->numTaps);
    int32_t Samples = (int32_t) nSamples;

    memcpy_StoX_int16(inst->lms_q15_ll.xyptr_states, (int16_t *)(void*) inst->pState, inst->numTaps+1); 
    memcpy_StoY_int16(inst->lms_q15_ll.xyptr_coeffs, (int16_t *)(void*) inst->pCoeffs, inst->numTaps); 

    while (Samples > 0) {
        int32_t block_size = (Samples > LMS_Q15_BLOCK_SIZE) ? LMS_Q15_BLOCK_SIZE:Samples;

        memcpy_StoX_int16(XY_MAP_INOUT, (int16_t *)(void*) pSrc,block_size); 
        memcpy_StoY_int16(XY_MAP_REFERR, (int16_t *)(void*) pRef, block_size); 

        dsp_lms_q15_XY_cfYstX_MAC16x16(
            &(inst->lms_q15_ll),
            XY_MAP_INOUT,
            XY_MAP_REFERR,
            block_size);

        memcpy_XtoS_int16(pOut, XY_MAP_INOUT,block_size);
        memcpy_YtoS_int16(pErr, XY_MAP_REFERR, block_size);

        Samples -= block_size;
        pSrc += block_size;
        pOut += block_size;
        pErr+=block_size;
        pRef+=block_size;
    };
    memcpy_YtoS_int16(inst->pCoeffs, inst->lms_q15_ll.xyptr_coeffs, inst->numTaps);
    memcpy_XtoS_int16(inst->pState, inst->lms_q15_ll.xyptr_states,  inst->numTaps+1); 
#else
    risc_dsp_lms_q15(inst, pSrc, pRef, pOut, pErr, nSamples);
#endif

}


void dsp_lms_delete_q15(lms_q15_t * inst) {};


void _DSPLIB_XYCC dsp_lms_init_q15_ext(lms_q15_t * inst,  uint16_t numTaps,  PTR(q15_t) pCoeffs, PTR(q15_t) pState,  q15_t mu,  uint32_t nSamples,  uint32_t postShift)
{

    dsp_lms_init_q15(inst,  numTaps, pCoeffs,  pState,  mu);
    inst->postshift = postShift;

#if defined(_ARC_AS210_mac16)
    inst->lms_q15_ll.postshift = postShift;
#endif

    return;
}
#pragma Code()


#endif //!defined(EXCLUDE_DSP_LMS_Q15)
