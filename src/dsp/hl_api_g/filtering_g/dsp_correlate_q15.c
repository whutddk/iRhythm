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

#if !defined(EXCLUDE_DSP_CORRELATE_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac16) 
#include "dsplib_ll_api.h"
#endif

#pragma Code("dsplib_dsp_correlate_q15")
/*================== REAL CORRELATION Q15 Function ==========================*/

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined _XY_HAND_OPT

static _Inline void  risc_dsp_correlate_q15(const q15_t * __restrict pSrcA, uint32_t nLenA, const q15_t * __restrict pSrcB, uint32_t nLenB, q15_t * __restrict pDst ){
    int32_t tail_size, body_size, i, j;
    const q15_t * __restrict psrca;
    const q15_t * __restrict psrcb;
    dspfx_accum32();

    _DSPLIB_store_regs(3, 5, 2);

    tail_size = dspfx_min_q31(nLenA, nLenB) - 1; // Calculate the size of correlation 'tails'
    body_size = nLenA + nLenB - 1 - (tail_size << 1); // Calculate the size of correlation 'body'


    if (nLenA > nLenB){
    	psrca =  pSrcA;
    	psrcb = (pSrcB + tail_size);
        _set_agu_ap2((uint32_t)pDst); // Set output pointer
        _set_agu_mod4(2, AGU_DT_q15, AGU_AD_INC_q15(1)); // Output pointer increment
    } else {
    	psrca =  pSrcB;
    	psrcb = (pSrcA + tail_size);
        _set_agu_ap2((uint32_t)(pDst + nLenA + nLenB - 2)); // Set output pointer
        _set_agu_mod4(2, AGU_DT_q15, AGU_AD_DEC_q15(1)); // Output pointer increment
    }

    _set_agu_ap0((uint32_t)psrca);
    _set_agu_ap1((uint32_t)psrcb);

    _set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(1)); // First input pointer increment by 1
    _set_agu_mod2(1, AGU_DT_q15, AGU_AD_INC_q15(1)); // Second input pointer increment by 1


    _set_agu_mod5(0, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // First input pointer dual increment by 2
    _set_agu_mod6(1, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // Second input pointer dual increment by 2



    // 'tail' at the begin
	for (i = 1; i < tail_size + 1; i++) {

		_set_agu_ap0((uint32_t )psrca);
		_set_agu_ap1((uint32_t )psrcb);
		if ((i & 1) == 0) {
			dspfx_reset_a32();
			for (j = 0; j < i >> 1; j++) {
				dspfx_a32dmac_q15(_agu_u5(), _agu_u6(), acc);
			}
			_st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample
		} else {
			dspfx_reset_a32();
			dspfx_a32mac_q15(_agu_u0(), _agu_u2(), acc);
			for (j = 0; j < i >> 1; j++) {
				dspfx_a32dmac_q15(_agu_u5(), _agu_u6(), acc);
			}
			_st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample
		}
		psrcb--;

	}
    // Correlation 'body'
	_set_agu_ap0((uint32_t )psrca);
	_set_agu_ap1((uint32_t )psrcb);
	if (tail_size & 1) {

		_set_agu_os0(tail_size - 2);
		_set_agu_os1(tail_size - 1);
		_set_agu_mod1(0, AGU_DT_v2q15, AGU_AD_DEC_q15_REG(0)); // First input pointer decrement
		_set_agu_mod3(1, AGU_DT_v2q15, AGU_AD_DEC_q15_REG(1)); // Second input pointer decrement
		for (i = 0; i < body_size; i++) {
			dspfx_reset_a32();
			for (j = 0; j < tail_size>>1; j++) {
				dspfx_a32dmac_q15(_agu_u5(), _agu_u6(), acc);
			}
			dspfx_a32dmac_q15(_agu_u1(), _agu_u3(), acc);
			_st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample
		}
	} else {
		_set_agu_os0(tail_size-1);
		_set_agu_os1(tail_size);
		_set_agu_mod1(0, AGU_DT_q15, AGU_AD_DEC_q15_REG(0)); // First input pointer decrement
		_set_agu_mod3(1, AGU_DT_q15, AGU_AD_DEC_q15_REG(1)); // Second input pointer decrement
		for (i = 0; i < body_size; i++) {

			dspfx_reset_a32();
			for (j = 0; j < tail_size>>1; j++) {
				dspfx_a32dmac_q15(_agu_u5(), _agu_u6(), acc);
			}
			dspfx_a32mac_q15(_agu_u1(), _agu_u3(), acc);
			_st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample
		}
	}

    // 'tail' at the end

	psrca+=body_size;

    for(i = tail_size; i > 0; i--){

        _set_agu_ap0((uint32_t)psrca);
        _set_agu_ap1((uint32_t)psrcb);
		if ((i & 1) == 0) {
			dspfx_reset_a32();
			for (j = 0; j < i >> 1; j++) {
				dspfx_a32dmac_q15(_agu_u5(), _agu_u6(), acc);
			}
			_st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample
		} else {
			dspfx_reset_a32();
			dspfx_a32mac_q15(_agu_u0(), _agu_u2(), acc);
			for (j = 0; j < i >> 1; j++) {
				dspfx_a32dmac_q15(_agu_u5(), _agu_u6(), acc);
			}
			_st_agu_u4(dspfx_q15_cast_a32(acc)); // Store output sample
		}
		psrca++;
    }

    _DSPLIB_restore_regs();

}

#else //_ARCv2EM_XY_AGU_MEDIUM

static _Inline void risc_dsp_correlate_q15(const PTR(q15_t) __restrict pSrcA, uint32_t nLenA, const PTR(q15_t) __restrict pSrcB, uint32_t nLenB, PTR(q15_t) __restrict pDst )
{
    uint32_t i , j, Na1, Nb1, inc;
    const PTR(q15_t) pA;
    const PTR(q15_t) pB;
    PTR(q15_t) pC;
    dspfx_accum32()
    INIT_MACMODE();

    if ( nLenB > nLenA)
    {
        pA = pSrcB;
        Na1 = nLenB;
        pB = pSrcA;
        Nb1 = nLenA;        
        inc=-1;
        pC = pDst+Na1+Nb1-2;

    }
    else
    {
        pA = pSrcA;
        Na1 = nLenA;
        pB = pSrcB;
        Nb1 = nLenB;
        inc=1;
        pC = pDst;
    }

    PRAGMA_NO_UNROLL
    for ( i = 0; i < Nb1-1; i++)
    {
    	const PTR(q15_t) pBj = pB + Nb1 -1 -i;
        const PTR(q15_t) pAj = pA;
        dspfx_reset_a32();
        PRAGMA_UNROLL_2
        for ( j = 0; j <= i; j++)
            dspfx_a32mac_q15(*pAj++, *pBj++, acc);
        *pC = dspfx_q15_cast_a32(acc);
        pC += inc;
    }

    PRAGMA_NO_UNROLL
    for ( i = 0; i < Na1 - Nb1; i++)
    {
    	const PTR(q15_t) pBj = pB;
        const PTR(q15_t) pAj = pA++;
        dspfx_reset_a32();
        PRAGMA_UNROLL_2
        for ( j = 0;  j < Nb1; j++)
            dspfx_a32mac_q15(*pAj++, *pBj++, acc);
        *pC = dspfx_q15_cast_a32(acc);
        pC += inc;
    }
    
    PRAGMA_NO_UNROLL
    for ( i = Na1-Nb1; i < Na1; i++ )
    {
    	const PTR(q15_t) pBj = pB;
        const PTR(q15_t) pAj = pA++;
        dspfx_reset_a32();
        PRAGMA_UNROLL_2
        for ( j = i; j < Na1; j++ )
            dspfx_a32mac_q15(*pAj++, *pBj++, acc);
        *pC = dspfx_q15_cast_a32(acc);
        pC += inc;
    }
}

#endif //_ARCv2EM_XY_AGU_MEDIUM

#define XY_THRESHOLD 1 // todo: determine threshold
#define XYMAP_INPUT0 (0)
#define XYMAP_INPUT1 (0)

/**
 * Q15 real cross-correlation
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nLenA
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] pDst - pointer to output vector. Vector length: nLenA + nLenB - 1
 */

void _DSPLIB_XYCC dsp_correlate_q15(const PTR(q15_t) __restrict pSrcA, uint32_t nLenA, const PTR(q15_t) __restrict pSrcB, uint32_t nLenB, PTR(q15_t) __restrict pDst )
{
#if defined(_ARC_AS210_mac16)
    uint32_t output_length = (nLenA + nLenB - 1);
    if ((dspfx_max_q31(nLenA,nLenB) < (uint32_t)XY_THRESHOLD) 
        || ((nLenA + output_length) > (uint32_t)(XY_REGION_SIZE/2))) {
#endif
        risc_dsp_correlate_q15(pSrcA, nLenA, pSrcB, nLenB, pDst);
#if defined(_ARC_AS210_mac16) 
    } else {
        uint32_t output_offset = nLenA;
        memcpy_StoX_int16(XYMAP_INPUT0, pSrcA, nLenA);
        memcpy_StoY_int16(XYMAP_INPUT1, pSrcB, nLenB);
        dsp_correlate_q15_XYtoX_MAC16x16(XYMAP_INPUT0, nLenA, XYMAP_INPUT1, nLenB, output_offset);
        memcpy_XtoS_int16(pDst, output_offset, nLenA + nLenB - 1);
    }
#endif
}

#endif //!defined(EXCLUDE_DSP_CORRELATE_Q15)
