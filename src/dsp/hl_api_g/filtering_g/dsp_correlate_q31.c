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

#if !defined(EXCLUDE_DSP_CORRELATE_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32) 
#include "dsplib_ll_api.h"
#endif

#pragma Code("dsplib_dsp_correlate_q31")
/*================== REAL CORRELATION Q31 Function ==========================*/

#ifdef _ARCv2EM_XY

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM

void _DSPLIB_XYCC dsp_correlate_q31( const __xy q31_t * __restrict pSrcA, uint32_t nLenA, const __xy q31_t * __restrict pSrcB, uint32_t nLenB, __xy q31_t * __restrict pDst){
    int32_t tail_size, body_size, i, j;
    int32_t a_offset,  b_offset;
    dspfx_accum64();

    _DSPLIB_store_regs(3, 5, 2);

    tail_size = dspfx_min_q31(nLenA, nLenB) - 1; // Calculate the size of correlation 'tails'
    body_size = nLenA + nLenB - 1 - (tail_size << 1); // Calculate the size of correlation 'body'

    a_offset = -1;
    b_offset = 0;

    if (nLenA > nLenB){
        _set_agu_ap0((uint32_t)pSrcA);
        _set_agu_ap1((uint32_t)(pSrcB + tail_size));
        _set_agu_ap2((uint32_t)pDst); // Set output pointer
        _set_agu_mod4(2, AGU_DT_q31, AGU_AD_INC_q31(1)); // Output pointer increment
    } else {
        _set_agu_ap0((uint32_t)pSrcB);
        _set_agu_ap1((uint32_t)(pSrcA + tail_size));
        _set_agu_ap2((uint32_t)(pDst + nLenA + nLenB - 2)); // Set output pointer
        _set_agu_mod4(2, AGU_DT_q31, AGU_AD_DEC_q31(1)); // Output pointer increment
    }


    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1)); // First input pointer increment by 1
    _set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31_REG(0)); // First input pointer decrement
    _set_agu_mod2(1, AGU_DT_q31, AGU_AD_INC_q31(1)); // Second input pointer increment by 1
    _set_agu_mod3(1, AGU_DT_q31, AGU_AD_DEC_q31_REG(1)); // Second input pointer decrement



    // 'tail' at the begin
    for(i = 0; i < tail_size; i++){

        a_offset++;
        b_offset++;

        _set_agu_os0(a_offset); // Set first input decrement
        _set_agu_os1(b_offset); // Set second input pointer decrement

        dspfx_reset_a64();
#if 0
        for(j = 0; j < i; j++){
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        }
#else
        if (i & 1)
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        for(j = 0; j < (i >> 1); j++) {
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        }
#endif
        dspfx_a64mac_q31(_agu_u1(), _agu_u3(), acc);
        _st_agu_u4(dspfx_q31_cast_a64(acc)); // Store output sample

    }
    // Correlation 'body'
    for(i = 0; i < body_size; i++){

        dspfx_reset_a64();
#if 0
        for(j = 0; j < tail_size; j++){
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        }
#else
        if (tail_size & 1)
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        for(j = 0; j < (tail_size >> 1); j++) {
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        }
#endif
        dspfx_a64mac_q31(_agu_u1(), _agu_u3(), acc);
        _st_agu_u4(dspfx_q31_cast_a64(acc)); // Store output sample
    }

    // 'tail' at the end
    for(i = tail_size - 1; i >= 0; i--){

        a_offset--;
        b_offset--;

        _set_agu_os0(a_offset); // Set first input decrement
        _set_agu_os1(b_offset); // Set second input pointer decrement

        dspfx_reset_a64();
#if 0
        for(j = 0; j < i; j++){
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        }
#else
        if (i & 1)
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        for(j = 0; j < (i >> 1); j++) {
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        }
#endif
        dspfx_a64mac_q31(_agu_u1(), _agu_u3(), acc);
        _st_agu_u4(dspfx_q31_cast_a64(acc)); // Store output sample
    }

    _DSPLIB_restore_regs();
}

#else

void _DSPLIB_XYCC dsp_correlate_q31( const __xy q31_t * __restrict pSrcA, uint32_t nLenA, const __xy q31_t * __restrict pSrcB, uint32_t nLenB, __xy q31_t * __restrict pDst)
{
    int32_t i, j;
    const q31_t * __restrict pA;
    const q31_t * __restrict pB;
    __xy q31_t * __restrict pD;
    int32_t tail_size, body_size, inc;

    tail_size = dspfx_min_q31(nLenA, nLenB) - 1; // Calculate the size of convolution 'tails'
    body_size = nLenA + nLenB - 1 - (tail_size << 1); // Calculate the size of convolution 'body'

    if (nLenA < nLenB)
    {
        pA = pSrcB;
        pB = pSrcA + tail_size;
        pD = pDst + nLenA + nLenB - 2;
        inc = -1;
    }
    else
    {
        pA = pSrcA;
        pB = pSrcB + tail_size;
        pD = pDst;
        inc = 1;
    }

    for ( i = 0; i <  tail_size; i++ )
    {
        __xy const q31_t *pAj = (__xy const q31_t*)pA;
        __xy const q31_t *pBj = (__xy const q31_t*)pB--;
        accum72_t acc = fx_create_a72(0,0);
        PRAGMA_UNROLL_2
        for ( j = 0; j <= i; j++ )
            acc = fx_a72_mac_q31(acc, *pAj++, *pBj++);
        *pD = fx_q31_cast_rnd_a72(acc);
        pD += inc;
    }
	for (i = 0; i < body_size; i++)
	{
		accum72_t acc = fx_create_a72(0,0);
		__xy const q31_t* pAj = (__xy const q31_t*)pA++;
		__xy const q31_t* pBj = (__xy const q31_t*)(pB);
        PRAGMA_UNROLL_2
		for ( j = 0; j < tail_size+1; j++)
			acc = fx_a72_mac_q31(acc, *pAj++, *pBj++);
		*pD = fx_q31_cast_rnd_a72(acc);
        pD += inc;
	}

    for (i = tail_size - 1; i >= 0; i--)
    {
        accum72_t acc = fx_create_a72(0,0);
        __xy const q31_t* pBj = (__xy const q31_t*)(pB);
        __xy const q31_t* pAj = (__xy const q31_t*)pA++;
        PRAGMA_UNROLL_2
        for ( j = 0; j <= i; j++)
            acc = fx_a72_mac_q31(acc, *pAj++, *pBj++);
        *pD = fx_q31_cast_rnd_a72(acc);
        pD += inc;
    }
}


#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
static _Inline void risc_dsp_correlate_q31( const q31_t * __restrict pSrcA, uint32_t nLenA, const q31_t * __restrict pSrcB, uint32_t nLenB, q31_t * __restrict pDst)
{
    uint32_t i , j, Na1, Nb1, inc;
    const q31_t* pA, *pB;
    q31_t* pC;
    dspfx_accum64()

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
        const q31_t* pBj, *pAj;        
        dspfx_reset_a64();
        pBj = pB + Nb1 -1 -i;
        pAj = pA;
        for ( j = 0; j <= i; j++)
            dspfx_a64mac_q31(*pAj++, *pBj++, acc);
        (*pC)=dspfx_q31_cast_a64(acc);
        pC+=inc;
    }

    PRAGMA_NO_UNROLL
    for ( i = 0; i < Na1 - Nb1; i++)
    {
        const q31_t* pBj, *pAj;
        dspfx_reset_a64();
        pBj = pB;
        pAj = pA++;
        for ( j = 0;  j < Nb1; j++)
            dspfx_a64mac_q31(*pAj++, *pBj++, acc);
        (*pC)=dspfx_q31_cast_a64(acc);
        pC+=inc;
    }

    PRAGMA_NO_UNROLL
    for ( i = Na1-Nb1; i < Na1; i++ )
    {
        const q31_t* pBj, *pAj;
        dspfx_reset_a64();
        pBj = pB;
        pAj = pA++;
        for ( j = i; j < Na1; j++ )
            dspfx_a64mac_q31(*pAj++, *pBj++, acc);
        (*pC)=dspfx_q31_cast_a64(acc);
        pC+=inc;
    }
}
#define XY_THRESHOLD 1 // todo: determine threshold
#define XYMAP_INPUT0 (0)
#define XYMAP_INPUT1 (0)

/**
 * Q31 real cross-correlation
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nLenA
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] pDst - pointer to output vector. Vector length: nLenA + nLenB - 1
 */

void dsp_correlate_q31( const q31_t * __restrict pSrcA, uint32_t nLenA, const q31_t * __restrict pSrcB, uint32_t nLenB, q31_t * __restrict pDst)
{
#if defined(_ARC_AS210_mac32)
    uint32_t output_length = (nLenA + nLenB - 1);
    if ((dspfx_max_q31(nLenA,nLenB) < (uint32_t)XY_THRESHOLD) 
        || ((nLenA + output_length) > (uint32_t)(XY_REGION_SIZE/4))) {
#endif
        risc_dsp_correlate_q31(pSrcA, nLenA, pSrcB, nLenB, pDst);
#if defined(_ARC_AS210_mac32) 
    } else {
        uint32_t output_offset = nLenA;
        memcpy_StoX_int32(XYMAP_INPUT0, pSrcA, nLenA);
        memcpy_StoY_int32(XYMAP_INPUT1, pSrcB, nLenB);
        dsp_correlate_q31_XYtoX_MAC32x32(XYMAP_INPUT0, nLenA, XYMAP_INPUT1, nLenB, output_offset);
        memcpy_XtoS_int32(pDst, output_offset, output_length);
    }
#endif
}


#endif //_ARCv2EM_XY_AGU_MEDIUM

#endif //!defined(EXCLUDE_DSP_CORRELATE_Q31)
