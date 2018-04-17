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

#if !defined(EXCLUDE_DSP_CONV_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32) 
#include "dsplib_ll_api.h"
#endif

#pragma Code("dsplib_dsp_conv_q31")
/*================== REAL CONVOLUTION Q31 Function ==========================*/
#ifdef _ARCv2EM_XY

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM

void _DSPLIB_XYCC dsp_conv_q31(const __xy q31_t * __restrict pSrcA, uint32_t nLenA, const __xy q31_t * __restrict pSrcB, uint32_t nLenB, __xy q31_t * __restrict pDst){
    int32_t tail_size, body_size, i, j;
    int32_t a_offset,  b_offset;

    _DSPLIB_store_regs(3, 5, 2);

    dspfx_accum64();
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

    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1)); // First input pointer increment by 1
    _set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31_REG(0)); // First input pointer decrement
    _set_agu_mod2(1, AGU_DT_q31, AGU_AD_DEC_q31(1)); // Second input pointer decrement by 1
    _set_agu_mod3(1, AGU_DT_q31, AGU_AD_INC_q31_REG(1)); // Second input pointer increment

    _set_agu_mod4(2, AGU_DT_q31, AGU_AD_INC_q31(1)); // Output pointer increment

    // 'tail' at the begin
    for(i = 0; i < tail_size; i++){

        a_offset++;
        b_offset++;

        _set_agu_os0(a_offset); // Set first input decrement
        _set_agu_os1(b_offset); // Set second input pointer increment

        dspfx_reset_a64();
#if 0
        for(j = 0; j < i; j++){
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        }
#else
        if (i & 1)
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        for(j = 0; j < (i >> 1); j++){
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        }
#endif
        dspfx_a64mac_q31(_agu_u1(), _agu_u3(), acc);
        _st_agu_u4(dspfx_q31_cast_a64(acc)); // Store output sample

    }
    // Convolution 'body'
    for(i = 0; i < body_size; i++){

        dspfx_reset_a64();
#if 0
        for(j = 0; j < tail_size; j++){
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        }
#else
        if (tail_size & 1)
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        for(j = 0; j < (tail_size >> 1); j++){
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
        _set_agu_os1(b_offset); // Set second input pointer increment

        dspfx_reset_a64();
#if 0
        for(j = 0; j < i; j++){
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        }
#else
        if (i & 1)
            dspfx_a64mac_q31(_agu_u0(), _agu_u2(), acc);
        for(j = 0; j < (i >> 1); j++){
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

#if 1


#define AGU_INC(t, inc) ((t) = _agu_inc((t), (inc)*sizeof((t)[0])))
#define AGU_DEC(t, inc) ((t) = _agu_dec((t), (inc)*sizeof((t)[0])))

#define AGU_INC1(t) AGU_INC((t), 1)
#define AGU_DEC1(t) AGU_DEC((t), 1)

void _DSPLIB_XYCC dsp_conv_q31(const __xy q31_t * __restrict pSrcA, uint32_t nLenA, const __xy q31_t * __restrict pSrcB, uint32_t nLenB, __xy q31_t * __restrict pDst)
{
    int32_t i, j;
    __xy q31_t * __restrict pA;
    __xy q31_t * __restrict pB;
    int32_t tail_size, body_size;

    tail_size = dspfx_min_q31(nLenA, nLenB) - 1; // Calculate the size of convolution 'tails'
    body_size = nLenA + nLenB - 1 - (tail_size << 1); // Calculate the size of convolution 'body'

    if (nLenA < nLenB)
    {
        pA = (__xy q31_t * __restrict) pSrcB;
        pB = (__xy q31_t * __restrict) pSrcA;
    }
    else
    {
        pA = (__xy q31_t * __restrict) pSrcA;
        pB = (__xy q31_t * __restrict) pSrcB;
    }

    for ( i = 0; i <  tail_size; i++ )
    {
        __xy q31_t *pAj = (__xy q31_t*)pA;
        __xy q31_t *pBj = (__xy q31_t*)pB; AGU_INC1(pB);
        accum72_t acc = fx_create_a72(0,0);
        PRAGMA_UNROLL_2
        for ( j = 0; j <= i; j++ ) {
            acc = fx_a72_mac_q31(acc, *pAj, *pBj); AGU_INC1(pAj); AGU_DEC1(pBj);
        }
        *pDst = fx_q31_cast_rnd_a72(acc); AGU_INC1(pDst);
    }
	for (i = 0; i < body_size; i++)
	{
		accum72_t acc = fx_create_a72(0,0);
		__xy q31_t* pAj = (__xy q31_t*)pA; AGU_INC1(pA);
		__xy q31_t* pBj = (__xy q31_t*)(pB);
        PRAGMA_UNROLL_2
		for ( j = 0; j < tail_size+1; j++) {
			acc = fx_a72_mac_q31(acc, *pAj, *pBj); AGU_INC1(pAj); AGU_DEC1(pBj);
		}
		*pDst = fx_q31_cast_rnd_a72(acc); AGU_INC1(pDst);
	}

    for (i = tail_size - 1; i >= 0; i--)
    {
        accum72_t acc = fx_create_a72(0,0);
        __xy q31_t* pAj = (__xy q31_t*)pA; AGU_INC1(pA);
        __xy q31_t* pBj = (__xy q31_t*)(pB);
        PRAGMA_UNROLL_2
        for ( j = 0; j <= i; j++) {
            acc = fx_a72_mac_q31(acc, *pAj, *pBj); AGU_INC1(pAj); AGU_DEC1(pBj);
        }
        *pDst = fx_q31_cast_rnd_a72(acc); AGU_INC1(pDst);
    }
}

#else

void _DSPLIB_XYCC dsp_conv_q31(const __xy q31_t * __restrict pSrcA, uint32_t nLenA, const __xy q31_t * __restrict pSrcB, uint32_t nLenB, __xy q31_t * __restrict pDst)
{
    int32_t i, j;
    const q31_t * __restrict pA;
    const q31_t * __restrict pB;
    int32_t tail_size, body_size;

    tail_size = dspfx_min_q31(nLenA, nLenB) - 1; // Calculate the size of convolution 'tails'
    body_size = nLenA + nLenB - 1 - (tail_size << 1); // Calculate the size of convolution 'body'

    if (nLenA < nLenB)
    {
        pA = pSrcB;
        pB = pSrcA;
    }
    else
    {
        pA = pSrcA;
        pB = pSrcB;
    }

    for ( i = 0; i <  tail_size; i++ )
    {
        __xy const q31_t *pAj = (__xy const q31_t*)pA;
        __xy const q31_t *pBj = (__xy const q31_t*)pB++;
        accum72_t acc = fx_create_a72(0,0);

        for ( j = 0; j <= i; j++ )
            acc = fx_a72_mac_q31(acc, *pAj++, *pBj--);
        *pDst++ = fx_q31_cast_rnd_a72(acc);
    }
	for (i = 0; i < body_size; i++)
	{
		accum72_t acc = fx_create_a72(0,0);
		__xy const q31_t* pAj = (__xy const q31_t*)pA++;
		__xy const q31_t* pBj = (__xy const q31_t*)(pB);

		for ( j = 0; j < tail_size+1; j++)
			acc = fx_a72_mac_q31(acc, *pAj++, *pBj--);
		*pDst++ = fx_q31_cast_rnd_a72(acc);
	}

    for (i = tail_size - 1; i >= 0; i--)
    {
        accum72_t acc = fx_create_a72(0,0);
        __xy const q31_t* pBj = (__xy const q31_t*)(pB);
        __xy const q31_t* pAj = (__xy const q31_t*)pA++;

        for ( j = 0; j <= i; j++)
            acc = fx_a72_mac_q31(acc, *pAj++, *pBj--);
        *pDst++ = fx_q31_cast_rnd_a72(acc);
    }
}

#endif

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
static _Inline void risc_dsp_conv_q31(const q31_t * __restrict pSrcA, uint32_t nLenA, const q31_t * __restrict pSrcB, uint32_t nLenB, q31_t * __restrict pDst)
{
    uint32_t i, j, Na1, Nb1;
    const q31_t* pA, *pB;   
    dspfx_accum64()

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
        const q31_t* pAj, *pBj;
        dspfx_reset_a64();
        pBj = pB++;
        pAj = pA;
        for ( j = 0; j <= i; j++ )
            dspfx_a64mac_q31(*pAj++, *pBj--, acc);
        *pDst++ = dspfx_q31_cast_a64(acc);
    }
    pA++;
    PRAGMA_NO_UNROLL
    for (  i = 0; i < Na1-Nb1; i++)
    {
        const q31_t* pAj, *pBj;
        dspfx_reset_a64();
        pBj = pB-1;
        pAj = pA++;
        for ( j = 0; j < Nb1; j++)
            dspfx_a64mac_q31(*pAj++, *pBj--, acc);
        *pDst++ = dspfx_q31_cast_a64(acc);
    }

    if (Nb1==1)
        return;
    PRAGMA_NO_UNROLL
    for (  i = 0; i < Nb1-1; i++)
    {
        const q31_t* pAj, *pBj;
        dspfx_reset_a64();
        pBj = pB-1;
        pAj = pA++;
        for ( j = 0; j < Nb1-i-1; j++)
            dspfx_a64mac_q31(*pAj++, *pBj--, acc);
        *pDst++ = dspfx_q31_cast_a64(acc);
    }
}

#define XY_THRESHOLD 1 // todo: determine threshold
#define XYMAP_INPUT0 (0)
#define XYMAP_INPUT1 (0)

/**
 * Q31 real linear convolution
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nLenA
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] pDst - pointer to output vector. Vector length: nLenA + nLenB - 1
 */

void dsp_conv_q31(const q31_t * __restrict pSrcA, uint32_t nLenA, const q31_t * __restrict pSrcB, uint32_t nLenB, q31_t * __restrict pDst)
{
#if defined(_ARC_AS210_mac32)
    uint32_t output_length = (nLenA + nLenB - 1);
    if ((dspfx_max_q31(nLenA,nLenB) < (uint32_t)XY_THRESHOLD) 
        || ((nLenA + output_length) > (uint32_t)(XY_REGION_SIZE/4))) {
#endif
        risc_dsp_conv_q31(pSrcA, nLenA, pSrcB, nLenB, pDst);
#if defined(_ARC_AS210_mac32) 
    } else {
        uint32_t output_offset = nLenA;
        memcpy_StoX_int32(XYMAP_INPUT0, pSrcA, nLenA);
        memcpy_StoY_int32(XYMAP_INPUT1, pSrcB, nLenB);
        dsp_conv_q31_XYtoX_MAC32x32(XYMAP_INPUT0, nLenA, XYMAP_INPUT1, nLenB, output_offset);
        memcpy_XtoS_int32(pDst, output_offset, output_length);
    }
#endif
}

#endif //_ARCv2EM_XY_AGU_MEDIUM


#endif //!defined(EXCLUDE_DSP_CONV_Q31)
