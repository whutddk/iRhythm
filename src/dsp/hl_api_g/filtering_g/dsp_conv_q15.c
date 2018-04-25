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

#if !defined(EXCLUDE_DSP_CONV_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac16) 
#include "dsplib_ll_api.h"
#endif

#pragma Code("dsplib_dsp_conv_q15")
/*================== REAL CONVOLUTION Q15 Function ==========================*/
#if defined _ARCv2EM_XY_AGU_MEDIUM && defined _XY_HAND_OPT
#if 0
static _Inline void risc_dsp_conv_q15(const q15_t * __restrict pSrcA, uint32_t nLenA, const q15_t * __restrict pSrcB, uint32_t nLenB, q15_t * __restrict pDst){
    int32_t tail_size, body_size, i, j;
    int32_t a_offset,  b_offset;
    dspfx_accum32();

    _DSPLIB_store_regs(3, 5, 2);

    tail_size = dspfx_min_q31(nLenA, nLenB) - 1; // Calculate the size of convolution 'tails'
    body_size = nLenA + nLenB - 1 - (tail_size << 1); // Calculate the size of convolution 'body'

    a_offset = -1;
    b_offset =  0;

    if (nLenA > nLenB){
        _set_agu_ap0((uint32_t)pSrcA);
        _set_agu_ap1((uint32_t)pSrcB);
    } else {
        _set_agu_ap0((uint32_t)pSrcB);
        _set_agu_ap1((uint32_t)pSrcA);
    }
    _set_agu_ap2((uint32_t)pDst); // Set output pointer

    _set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(1)); // First input pointer increment by 1
    _set_agu_mod1(0, AGU_DT_q15, AGU_AD_DEC_q15_REG(0)); // First input pointer decrement
    _set_agu_mod2(1, AGU_DT_q15, AGU_AD_DEC_q15(1)); // Second input pointer decrement by 1
    _set_agu_mod3(1, AGU_DT_q15, AGU_AD_INC_q15_REG(1)); // Second input pointer increment

    _set_agu_mod4(2, AGU_DT_q15, AGU_AD_INC_q15(1)); // Output pointer increment

    // 'tail' at the begin
    for(i = 0; i < tail_size; i++){

        a_offset++;
        b_offset++;

        _set_agu_os0(a_offset); // Set first input decrement
        _set_agu_os1(b_offset); // Set second input pointer increment

        dspfx_reset_a32();
        for(j = 0; j < i; j++){
            dspfx_a32mac_q15(_agu_u0(), _agu_u2(), acc);
        }
        dspfx_a32mac_q15(_agu_u1(), _agu_u3(), acc);
        _st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample

    }
    // Convolution 'body'
    for(i = 0; i < body_size; i++){

        dspfx_reset_a32();
        for(j = 0; j < tail_size; j++){
            dspfx_a32mac_q15(_agu_u0(), _agu_u2(), acc);
        }
        dspfx_a32mac_q15(_agu_u1(), _agu_u3(), acc);
        _st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample
    }

    // 'tail' at the end
    for(i = tail_size - 1; i >= 0; i--){

        a_offset--;
        b_offset--;

        _set_agu_os0(a_offset); // Set first input decrement
        _set_agu_os1(b_offset); // Set second input pointer increment

        dspfx_reset_a32();
        for(j = 0; j < i; j++){
            dspfx_a32mac_q15(_agu_u0(), _agu_u2(), acc);
        }
        dspfx_a32mac_q15(_agu_u1(), _agu_u3(), acc);
        _st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample
    }

    _DSPLIB_restore_regs();
}
#else

static _Inline void risc_dsp_conv_q15(const q15_t * __restrict pSrcA, uint32_t nLenA, const q15_t * __restrict pSrcB, uint32_t nLenB, q15_t * __restrict pDst){
    int32_t tail_size, body_size, i, j;
    const q15_t * __restrict psrca;
    const q15_t * __restrict psrcb;
    dspfx_accum32();

    _DSPLIB_store_regs(3, 5, 2);

	tail_size = dspfx_min_q31(nLenA, nLenB) - 1; // Calculate the size of convolution 'tails'
	body_size = nLenA + nLenB - 1 - (tail_size << 1); // Calculate the size of convolution 'body'


	if (nLenA > nLenB) {
		psrca = pSrcA;
		psrcb = pSrcB;

	} else {
		psrca = pSrcB;
		psrcb = pSrcA;
	}
	_set_agu_ap0((uint32_t )psrca);
	_set_agu_ap1((uint32_t )psrcb);
	_set_agu_ap2((uint32_t )pDst); // Set output pointer

	_set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(1)); // First input pointer increment by 1
	_set_agu_mod1(0, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // First input pointer dual increment
	_set_agu_mod2(1, AGU_DT_q15, AGU_AD_DEC_q15(2)); // Second input pointer decrement by 2
	_set_agu_mod3(1, AGU_DT_v2q15|AGU_DT_vector_reverse, AGU_AD_DEC_q15(2)); // Second input pointer dual decrement

	_set_agu_mod4(2, AGU_DT_q15, AGU_AD_INC_q15(1)); // Output pointer increment

	// 'tail' at the begin
	for (i = 1; i < tail_size + 1; i++) {
		// init input pointers
		_set_agu_ap0((uint32_t )psrca);
		_set_agu_ap1((uint32_t )psrcb);
		if ((i & 1) == 0) { // even branch
			dspfx_reset_a32();
			for (j = 0; j < i >> 1; j++) {
				dspfx_a32dmac_q15(_agu_u1(), _agu_u3(), acc);
			}
			psrcb += 2;
		} else {            // odd branch
			dspfx_reset_a32();
			dspfx_a32mac_q15(_agu_u0(), _agu_u2(), acc);
			for (j = 0; j < i >> 1; j++) {
				dspfx_a32dmac_q15(_agu_u1(), _agu_u3(), acc);
			}
		}
		_st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample
	}

	// 'body'
	// init input pointers
	_set_agu_ap0((uint32_t )psrca);
	_set_agu_ap1((uint32_t )psrcb);

	_set_agu_os0(tail_size - 2);
	_set_agu_mod5(0, AGU_DT_v2q15, AGU_AD_DEC_q15_REG(0));
	_set_agu_mod6(1, AGU_DT_v2q15|AGU_DT_vector_reverse, AGU_AD_INC_q15_REG(1));

	if ((tail_size & 1) == 0) { // even branch
		_set_agu_os1(tail_size);
		for (i = 0; i < body_size; i++) {
			dspfx_reset_a32();
			dspfx_a32mac_q15(_agu_u0(), _agu_u2(), acc);
			for (j = 0; j < (tail_size >> 1) - 1; j++) {
				dspfx_a32dmac_q15(_agu_u1(), _agu_u3(), acc);
			}
			for (; j < (tail_size >> 1); j++) {
				dspfx_a32dmac_q15(_agu_u5(), _agu_u6(), acc);
			}
			_st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample
		}
	} else {                   // odd branch
		_set_agu_os1(tail_size - 1);
		for (i = 0; i < body_size; i++) {
			dspfx_reset_a32();
			for (j = 0; j < tail_size >> 1; j++) {
				dspfx_a32dmac_q15(_agu_u1(), _agu_u3(), acc);
			}
			dspfx_a32dmac_q15(_agu_u5(), _agu_u6(), acc);
			_st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample
		}
		psrcb++; // increment psrcb for next step
	}

	psrca += body_size;
	// 'tail' at the begin
	for (i = tail_size; i > 0; i--) {
		_set_agu_ap0((uint32_t )psrca);
		if ((i & 1) == 0) { // even branch

			_set_agu_ap1((uint32_t )(psrcb - 1));
			dspfx_reset_a32();
			for (j = 0; j < i >> 1; j++) {
				dspfx_a32dmac_q15(_agu_u1(), _agu_u3(), acc);
			}
		} else {            // odd branch
			dspfx_reset_a32();
			_set_agu_ap1((uint32_t )psrcb);
			dspfx_a32mac_q15(_agu_u0(), _agu_u2(), acc);
			for (j = 0; j < i >> 1; j++) {
				dspfx_a32dmac_q15(_agu_u1(), _agu_u3(), acc);
			}
		}
		psrca++;
		_st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample
	}
    _DSPLIB_restore_regs();
}


#endif
#else
static _Inline void risc_dsp_conv_q15(const PTR(q15_t) __restrict pSrcA, uint32_t nLenA, const PTR(q15_t) __restrict pSrcB, uint32_t nLenB, PTR(q15_t) __restrict pDst)
{
    uint32_t i, j, Na1, Nb1;
    const PTR(q15_t) pA;
    const PTR(q15_t) pB;
    dspfx_accum32()
    INIT_MACMODE();

    if (nLenA < nLenB)
    {
        pA = pSrcB;
        pB = pSrcA;
        Na1 = nLenB;
        Nb1 = nLenA;
    }
    else
    {
        pA = pSrcA;
        pB = pSrcB;
        Na1 = nLenA;
        Nb1 = nLenB;
    }

    PRAGMA_NO_UNROLL
    for ( i = 0; i <  Nb1; i++ )
    {
        const PTR(q15_t) pAj = pA;
        const PTR(q15_t) pBj = pB++;
        dspfx_reset_a32();
        PRAGMA_UNROLL_2
        for ( j = 0; j <= i; j++ )
            dspfx_a32mac_q15(*pAj++, *pBj--, acc);
        *pDst++ = dspfx_q15_cast_a32(acc);
    }
    pA++;
    PRAGMA_NO_UNROLL
    for (  i = 0; i < Na1-Nb1; i++)
    {
        const PTR(q15_t) pAj = pA++;
        const PTR(q15_t) pBj = pB-1;
		dspfx_reset_a32();
        PRAGMA_UNROLL_2
        for ( j = 0; j < Nb1; j++)
            dspfx_a32mac_q15(*pAj++, *pBj--, acc);
        *pDst++ = dspfx_q15_cast_a32(acc);
    }

    if (Nb1==1)
        return;
    PRAGMA_NO_UNROLL
    for (  i = 0; i < Nb1-1; i++)
    {
        const PTR(q15_t) pAj = pA++;
        const PTR(q15_t) pBj = pB-1;
		dspfx_reset_a32();
        PRAGMA_UNROLL_2
        for ( j = 0; j < Nb1-i-1; j++)
            dspfx_a32mac_q15(*pAj++, *pBj--, acc);
        *pDst++ = dspfx_q15_cast_a32(acc);
    }
}
#endif

#define XY_THRESHOLD 1 // todo: determine threshold
#define XYMAP_INPUT0 (0)
#define XYMAP_INPUT1 (0)

/**
 * Q15 real linear convolution
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nLenA
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] pDst - pointer to output vector. Vector length: nLenA + nLenB - 1
 */

void _DSPLIB_XYCC dsp_conv_q15(const PTR(q15_t) __restrict pSrcA, uint32_t nLenA, const PTR(q15_t) __restrict pSrcB, uint32_t nLenB, PTR(q15_t) __restrict pDst)
{
#if defined(_ARC_AS210_mac16)
    uint32_t output_length = (nLenA + nLenB - 1);
    if ((dspfx_max_q31(nLenA,nLenB) < (uint32_t)XY_THRESHOLD) 
        || ((nLenA + output_length) > (uint32_t)(XY_REGION_SIZE/2))) {
#endif
        risc_dsp_conv_q15(pSrcA, nLenA, pSrcB, nLenB, pDst);
#if defined(_ARC_AS210_mac16) 
    } else {
        uint32_t output_offset = nLenA;
        memcpy_StoX_int16(XYMAP_INPUT0, pSrcA, nLenA);
        memcpy_StoY_int16(XYMAP_INPUT1, pSrcB, nLenB);
        dsp_conv_q15_XYtoX_MAC16x16(XYMAP_INPUT0, nLenA, XYMAP_INPUT1, nLenB, output_offset);
        memcpy_XtoS_int16(pDst, output_offset, nLenA + nLenB - 1);
    }
#endif
}

#endif //!defined(EXCLUDE_DSP_CONV_Q15)
