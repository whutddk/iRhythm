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

#if !defined(EXCLUDE_DSP_BIQUAD_CASCADE_DF1_Q15)
#pragma Code("dsplib_dsp_biquad_cascade_df1_q15")

#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif

#if defined(_ARCv2EM_XY_AGU_MEDIUM) && defined (_XY_HAND_OPT)
static _Inline void risc_dsp_biquad_cascade_df1_q15(const iir_biquad_q15_t *inst, const q15_t * __restrict pSrc, q15_t * __restrict pDst, uint32_t nSamples)
{
	int32_t stage; // Stage counter
	uint32_t i;// Sample counter
	const q15_t* coeffs = inst->filter.pCoeffs;
	q15_t* states = inst->filter.pData;
	int32_t postshift = inst->postShift;
	int32_t order = inst->filter.numTaps;

	_DSPLIB_store_regs(4, 5, 0);

	_set_agu_ap0(pSrc);
	_set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(0));// input increment
	_set_agu_mod1(0, AGU_DT_q15, AGU_AD_INC_q15(1));// input increment
	_set_agu_mod2(1, AGU_DT_q15, AGU_AD_INC_q15(1));// output increment
	_set_agu_mod3(2, AGU_DT_q15, AGU_AD_WRAP_INC_II(sizeof(q15_t), AGU_WRAP_IMM_4));// output increment
	_set_agu_mod4(3, AGU_DT_q15, AGU_AD_WRAP_INC_II(sizeof(q15_t), AGU_WRAP_IMM_4));// output increment

#pragma  clang loop unroll(disable)
	for (stage = 0; stage < order; stage++, states += 4)
	{
		q15_t b0, b1, b2, a1, a2;

		b0 = *coeffs++;
		b1 = *coeffs++;
		b2 = *coeffs++;
		a1 = *coeffs++;
		a2 = *coeffs++;
		_set_agu_ap1(pDst);
		_set_agu_ap2(states); // output states
		_set_agu_ap3(states+2);// input states

		for (i = 0; i < nSamples; i++)
		{
			dspfx_accum32()
			dspfx_a32mul_q15(_agu_u4(), b2, acc);
			dspfx_a32mac_q15(_agu_u4(), b1, acc);
			dspfx_a32mac_q15(_agu_u0(), b0, acc);
			_st_agu_u4(_agu_u1());
			dspfx_a32mac_q15(_agu_u3(), a2, acc);
			dspfx_a32mac_q15(_agu_u3(), a1, acc);
			_st_agu_u2(dspfx_q15_cast_asl_a32(acc, postshift));
			_st_agu_u3(dspfx_q15_cast_asl_a32(acc, postshift));
		}

		_set_agu_ap0(pDst);
	}
	_DSPLIB_restore_regs();
}

#else //_ARCv2EM_XY_AGU_MEDIUM

static _Inline void risc_dsp_biquad_cascade_df1_q15(const iir_biquad_q15_t *inst, const PTR(q15_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples)
{
	int32_t stage; // Stage counter
	uint32_t i; // Sample counter
	PTR(q15_t) src = (PTR(q15_t)) pSrc;
	PTR(q15_t) dst = (PTR(q15_t)) pDst;
	const PTR(q15_t) coeffs = (PTR(q15_t)) inst->filter.pCoeffs;
    int32_t postshift = inst->postShift;
	PTR(q15_t) states = (PTR(q15_t)) inst->filter.pData;

	INIT_MACMODE();
    PRAGMA_NO_UNROLL
	for (stage = 0; stage < inst->filter.numTaps; stage++, states += 4)
	{
		q15_t b0, b1, b2, a1, a2;
		q15_t y0, y1, y2, x0, x1, x2;

		b0 = *coeffs++;
		b1 = *coeffs++;
		b2 = *coeffs++;
		a1 = *coeffs++;
		a2 = *coeffs++;

		y1 = states[0];
		y2 = states[1];
		x1 = states[2];
		x2 = states[3];

#if defined _BIQUAD_UNROLL
        y0 = states[1]; /*y2*/
        for (i = 0; i < nSamples >> 1; i++)
        {
			dspfx_accum32()

			x0 = *src++;
			dspfx_a32mul_q15(x0, b0, acc);
			dspfx_a32mac_q15(x1, b1, acc);
			dspfx_a32mac_q15(x2, b2, acc);
			dspfx_a32mac_q15(y1, a1, acc);
			dspfx_a32mac_q15(y0, a2, acc);

			*dst++ = y0 = dspfx_q15_cast_asl_a32(acc, postshift);


			x2 = *src++;
			dspfx_a32mul_q15(x2, b0, acc);
			dspfx_a32mac_q15(x0, b1, acc);
			dspfx_a32mac_q15(x1, b2, acc);
			dspfx_a32mac_q15(y0, a1, acc);
			dspfx_a32mac_q15(y1, a2, acc);

			*dst++ = y1 = dspfx_q15_cast_asl_a32(acc, postshift);

            x1 = x2;
            x2 = x0;
        }
        if (nSamples & 1) {
			dspfx_accum32()

			x0 = *src++;
			dspfx_a32mul_q15(x0, b0, acc);
			dspfx_a32mac_q15(x1, b1, acc);
			dspfx_a32mac_q15(x2, b2, acc);
			dspfx_a32mac_q15(y1, a1, acc);
			dspfx_a32mac_q15(y0, a2, acc);
            y0 = y1;
			*dst++ = y1 = dspfx_q15_cast_asl_a32(acc, postshift);

            x2 = x1;
            x1 = x0;
        }

        states[0] = y1;
        states[1] = y0;
        states[2] = x1;
        states[3] = x2;
#else
		for (i = 0; i < nSamples; i++) {
			dspfx_accum32()

			x0 = *src++;

			dspfx_a32mul_q15(x0, b0, acc);
			dspfx_a32mac_q15(x1, b1, acc);
			dspfx_a32mac_q15(x2, b2, acc);
			dspfx_a32mac_q15(y1, a1, acc);
			dspfx_a32mac_q15(y2, a2, acc);

			*dst++ = y0 = dspfx_q15_cast_asl_a32(acc, postshift);

			y2 = y1;
			y1 = y0;
			x2 = x1;
			x1 = x0;
		}

		states[0] = y1;
		states[1] = y2;
		states[2] = x1;
		states[3] = x2;
#endif

        src = pDst;
        dst = pDst;
	}
}
#endif //_ARCv2EM_XY_AGU_MEDIUM

#define XY_MAP_INOUT 0
#define XY_MAP_STATES (XY_REGION_SIZE/sizeof(q15_t)/8)
#define XY_MAP_COEFF 0

/**
 * Q15 DF1 type Biquad-Cascade (IIR) filter
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_biquad_cascade_df1_q15(const iir_biquad_q15_t *inst,
		const PTR(q15_t) __restrict pSrc, PTR(q15_t) __restrict pDst, uint32_t nSamples)
{
#if defined(_ARC_AS210_mac16)

	int32_t IIR_Q15_BLOCK_SIZE = XY_REGION_SIZE/sizeof(q15_t)/2;
	int32_t Samples = (int32_t) nSamples;

	memcpy_StoY_int16(XY_MAP_COEFF, (int16_t *)(void*) inst->filter.pCoeffs, 5*inst->filter.numTaps);
	memcpy_StoY_int16(XY_MAP_STATES, (int16_t *)(void*) inst->filter.pData, 4*inst->filter.numTaps);

	while (Samples > 0) {
		int32_t block_size = (Samples > IIR_Q15_BLOCK_SIZE) ? IIR_Q15_BLOCK_SIZE:Samples;

		memcpy_StoX_int16(XY_MAP_INOUT, (int16_t *)(void*) pSrc, block_size);
		dsp_biquad_cascade_df1_q15_X_cfYstY_MAC16x16(&(inst->iir_biquad_xy), XY_MAP_INOUT, block_size);
		memcpy_XtoS_int16(pDst, XY_MAP_INOUT, block_size);

		Samples -= block_size;
		pSrc += block_size;
		pDst += block_size;
	};

	memcpy_YtoS_int16((int16_t *)(void*) inst->filter.pData, XY_MAP_STATES, 4*inst->filter.numTaps);

#else
	risc_dsp_biquad_cascade_df1_q15(inst, pSrc, pDst, nSamples);
#endif
}


/**
 * initialization for Q15 IIR filtering
 *
 * \param inst - pointer to instance structure
 * \param numStages
 * \param pCoeffs - pointer to coefficients vector. Vector length: numStages * 5
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_biquad_cascade_df1_getsize_q15()" function.
 * \param postShift - Bit shift that is applied to output sample inside the process call after each biquad section. This parameter is used to allow representation of filter coefficients in other than q31 format. For example, coefficients may be represented in 2q29 format with postShift = 2
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_biquad_cascade_df1_init_q15(iir_biquad_q15_t* inst,
		uint8_t numStages, const PTR(q15_t) pCoeffs, PTR(q15_t) pState, int8_t postShift)
{
	inst->filter.numTaps = numStages;
	inst->filter.pCoeffs = (q15_t*) pCoeffs;
#if defined (_ARCv2EM_XY_AGU_MEDIUM)
	inst->filter.pData = (q15_t*)ADDR_ALIGN(pState, 4);
#else
	inst->filter.pData = pState;
#endif
	inst->postShift = postShift;
	_MEM_FILL_Q15((PTR(q15_t))inst->filter.pData,0,numStages*4);

#if defined(_ARC_AS210_mac16)
	inst->iir_biquad_xy.filter.xyptr_coeffs = XY_MAP_COEFF;
	inst->iir_biquad_xy.filter.xyptr_states = XY_MAP_STATES;
	inst->iir_biquad_xy.filter.numTaps = numStages;
	inst->iir_biquad_xy.postShift = postShift;
#endif

	return DSP_ERR_OK;
}

void dsp_biquad_cascade_df1_delete_q15(iir_biquad_q15_t* inst) { }

#endif //!defined(EXCLUDE_DSP_BIQUAD_CASCADE_DF1_Q15)
