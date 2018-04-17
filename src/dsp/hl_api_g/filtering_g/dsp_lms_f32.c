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

#if !defined(EXCLUDE_DSP_LMS_F32)
#pragma Code("dsplib_dsp_lms_f32")

 /**
 * f32 real LMS filtering
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[in] pRef - pointer to 'reference' vector. Vector length: nSamples
 * \param[out] pOut - pointer to output vector. Vector length: nSamples
 * \param[out] pErr - pointer to error vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

#if defined _ARCv2EM_XY && defined __FXAPI__
#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM
void _DSPLIB_XYCC dsp_lms_f32(lms_f32_t * inst,  const __xy f32_t * __restrict pSrc,  const __xy f32_t * __restrict pRef, __xy f32_t  * __restrict pOut, __xy f32_t * __restrict pErr,  uint32_t nSamples)
{
	uint32_t i;
	int32_t j;
	uint16_t numTaps = inst->numTaps;
	f32_t mu = inst->mu;
	f32_t e, alpha;
	f32_t *pState = inst->pState;  
	f32_t *pCoeffs = inst->pCoeffs;

	uint32_t alignment_mask = ((4 << (31 - dspfx_norm_q31((int32_t)numTaps))) - 1);

	_DSPLIB_store_regs(6, 8, 2);

	_set_agu_ap0(pSrc);
	_set_agu_ap1(pRef);
	_set_agu_ap2(pOut);
	_set_agu_ap3(pErr);
	_set_agu_ap5(inst->pState + inst->shift);

	_set_agu_mod0(4, AGU_DT_ui32, AGU_AD_INC_ui32(0));
	_set_agu_mod1(4, AGU_DT_ui32, AGU_AD_INC_ui32(1));
	_set_agu_mod3(0, AGU_DT_ui32, AGU_AD_INC_ui32(1));
	_set_agu_mod4(2, AGU_DT_ui32, AGU_AD_INC_ui32(0));
	_set_agu_mod5(2, AGU_DT_ui32, AGU_AD_INC_ui32(1));
	_set_agu_mod6(1, AGU_DT_ui32, AGU_AD_INC_ui32(1));
	_set_agu_mod7(3, AGU_DT_ui32, AGU_AD_INC_ui32(1));

	if (((uint32_t)pState & alignment_mask) == 0) {
		_set_agu_os0(sizeof(f32_t));
		_set_agu_os1(numTaps*sizeof(f32_t) - 1);
		_set_agu_mod2(5, AGU_DT_ui32, AGU_AD_WRAP_INC_RR(0, 1));
#pragma  clang loop unroll(disable)
		for (i = 0; i < nSamples; i++)
		{
			float acc = 0;

			// Store current input sample to delay line
			_st_agu_u2(_agu_u3());

			// FIR filter processing
			_set_agu_ap4(pCoeffs); // pC = pCoeffs;
			for (j = 0; j < numTaps; j++)
				acc += _as_float(_agu_u1()) * _as_float(_agu_u2());

			// Store output sample
			_st_agu_u4(_as_uint32(acc));

			// Calculate error between reference and output sample
			e = _as_float(_agu_u6())-_as_float(_agu_u5());

			// Calculate 'alpha' factor
			alpha =  e * mu;

			// Store error sample
			_st_agu_u7(_as_uint32(e));

			_set_agu_ap4(pCoeffs); //pC = pCoeffs;
			// pState points to the oldest sample in delay line
			// Update coefficients
			for (j = 0; j < numTaps; j++)
			{
				_st_agu_u1(_as_uint32(_as_float(_agu_u0()) + alpha * _as_float(_agu_u2())));
			}
		}
		inst->shift = (f32_t*)_agu_ap5() - pState;
	} else {
		int32_t shift = inst->shift;
		_set_agu_mod2(5, AGU_DT_q31, AGU_AD_INC_q31(1));
#pragma  clang loop unroll(disable)
		for (i = 0; i < nSamples; i++)
		{
			float acc = 0;

			// Store current input sample to delay line
			_st_agu_u2(_agu_u3());
			shift = (f32_t*)_agu_ap5() - pState;
			if (shift == numTaps) {
				shift = 0;
				_set_agu_ap5(pState);
			}

			// FIR filter processing
			_set_agu_ap4(pCoeffs); // pC = pCoeffs;
			for (j = shift; j < numTaps; j++)
				acc += _as_float(_agu_u1()) * _as_float(_agu_u2());
			_set_agu_ap5(pState);
			for (j = 0; j < shift; j++)
				acc += _as_float(_agu_u1()) * _as_float(_agu_u2());


			// Store output sample
			_st_agu_u4(_as_uint32(acc));

			// Calculate error between reference and output sample
			e = _as_float(_agu_u6())-_as_float(_agu_u5());

			// Calculate 'alpha' factor
			alpha =  e * mu;

			// Store error sample
			_st_agu_u7(_as_uint32(e));

			_set_agu_ap4(pCoeffs); //pC = pCoeffs;
			// pState points to the oldest sample in delay line
			// Update coefficients
			for (j = shift; j < numTaps; j++)
				_st_agu_u1(_as_uint32(_as_float(_agu_u0()) + alpha * _as_float(_agu_u2())));
			_set_agu_ap5(pState);
			for (j = 0; j < shift; j++)
				_st_agu_u1(_as_uint32(_as_float(_agu_u0()) + alpha * _as_float(_agu_u2())));

		}
		inst->shift = shift;
	}

	_DSPLIB_restore_regs();
	return;
}

#else

void _DSPLIB_XYCC dsp_lms_f32(lms_f32_t * inst,  const PTR(f32_t) __restrict pSrc,  const PTR(f32_t) __restrict pRef, PTR(f32_t) __restrict pOut, PTR(f32_t) __restrict pErr,  uint32_t nSamples)
{
	uint32_t i;
	int32_t j;
	uint16_t numTaps = inst->numTaps;
	f32_t mu = inst->mu;
	f32_t e, alpha;
	PTR(f32_t) pState = (PTR(f32_t)) inst->pState;
	PTR(f32_t) pCoeffs = (PTR(f32_t)) inst->pCoeffs;
	PTR(f32_t) p = (PTR(f32_t)) inst->pState + inst->shift;

	uint32_t alignment_mask = ((4 << (31 - dspfx_norm_q31((int32_t)numTaps))) - 1);
	uint32_t align_offset = ((uint32_t)inst->pState & alignment_mask) >> 2;

	for (i = 0; i < nSamples; i++)
	{
		f32_t acc = 0.0;

		// Store current input sample to delay line
		*p = *pSrc++;
		_AGU_CIRC_INC_SAFE(p, 1, numTaps, align_offset);

		// FIR filter processing
		for (j = 0; j < numTaps; j++) {
			acc += (*p) * (*pCoeffs++);
			_AGU_CIRC_INC_SAFE(p, 1, numTaps, align_offset);
		}
		pCoeffs -= numTaps;

		// Store output sample
		*pOut = acc;

		// Calculate error between reference and output sample
		e = *pRef++ - *pOut++;

		// Calculate 'alpha' factor
		alpha =  e * mu;

		// Store error sample
		*pErr++ = e;

		// pState points to the oldest sample in delay line
		// Update coefficients
		for (j = 0; j < numTaps; j++)
		{
			*pCoeffs++ += alpha * (*p);
			_AGU_CIRC_INC_SAFE(p, 1, numTaps, align_offset);
		}
		pCoeffs -= numTaps;
	}

	inst->shift = p - pState;
	return;
}

#endif

#else

void dsp_lms_f32(lms_f32_t * inst,  const f32_t * __restrict pSrc,  const f32_t * __restrict pRef,  f32_t * __restrict pOut,  f32_t * __restrict pErr,  uint32_t nSamples)
{
    uint32_t i;
    int32_t j;
    uint16_t shift = inst->shift;
    uint16_t numTaps = inst->numTaps;
    f32_t mu = inst->mu;
    f32_t e = 0.0;
    f32_t alpha = 0.0;
    f32_t *pState = inst->pState;  
    f32_t *pCoeffs = inst->pCoeffs;
    f32_t *pBufFirst = pState;
    f32_t *pBufLast = pState + numTaps - 1;
    f32_t *pC;
    f32_t acc;

    pState = inst->pState + shift;
    PRAGMA_NO_UNROLL
    for (i = 0; i < nSamples; i++)
    {

        acc = 0.0;
        pC = pCoeffs;

        // Store current input sample to delay line
        *pState = *pSrc++;
        MODULE_ADDR_INC(pState, pBufFirst, pBufLast);
        shift = pState - pBufFirst;
        // pState points to the oldest sample in delay line

        // FIR filter processing
        for (j = shift; j < numTaps; j++)
            MAC_F32(acc, *pC++, *pState++);
        pState = pBufFirst;
        for (j = 0;     j < shift;   j++)
            MAC_F32(acc, *pC++, *pState++);

        // Store output sample
        *pOut = acc;

        // Calculate error between reference and output sample
        e = *pRef++ - *pOut++;

        // Calculate 'alpha' factor
        alpha =  MPY_F32(e, mu);

        // Store error sample
        *pErr++ = e;

        pC = pCoeffs;
        // pState points to the oldest sample in delay line
        // Update coefficients
        for (j = shift; j < numTaps; j++)
        {
            MAC_F32(*pC++, alpha, *pState++);
        }

        pState = pBufFirst;
        for (j = 0;     j < shift;   j++)
        {
            MAC_F32(*pC++, alpha, *pState++);
        }        
    }
    inst->shift = shift;
    return;
}

#endif

/**
 * initialization for f32 real LMS filtering
 *
 * \param inst - pointer to instance structure
 * \param numTaps - number of coefficients
 * \param pCoeffs - pointer to coefficients vector. Vector length: numTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_lms_getsize_f32" function.
 * \param mu - adaptation step size
 * \result dsp_status
 */
 
dsp_status _DSPLIB_XYCC dsp_lms_init_f32(lms_f32_t * inst,  uint16_t numTaps, PTR(f32_t) pCoeffs,  PTR(f32_t) pState,  f32_t mu)
{
    inst->numTaps = numTaps;

    inst->pCoeffs = pCoeffs;
    _MEM_FILL_F32(pCoeffs,0.0f,numTaps);

    inst->pState = pState;
    _MEM_FILL_F32(pState, 0.0f, numTaps);

    inst->mu = mu;
    inst->shift = 0;
    return DSP_ERR_OK;
}


void dsp_lms_delete_f32(lms_f32_t * inst) {};


void _DSPLIB_XYCC dsp_lms_init_f32_ext(  lms_f32_t * inst,  uint16_t numTaps,  PTR(f32_t) pCoeffs,  PTR(f32_t) pState,  f32_t mu,  uint32_t nSamples)
{
    dsp_lms_init_f32(inst,  numTaps,  pCoeffs,  pState,  mu);
    return;
}

#endif //!defined(EXCLUDE_DSP_LMS_F32)
