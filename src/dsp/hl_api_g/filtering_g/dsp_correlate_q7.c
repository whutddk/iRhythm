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

#if !defined(EXCLUDE_DSP_CORRELATE_Q7)

#include "dsplib_types.h"
#include "dsplib_primitives.h"
#pragma Code("dsplib_dsp_correlate_q7")
/*================== REAL CORRELATION Q7 Function ==========================*/

/**
 * Q7 real cross-correlation
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nLenA
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] pDst - pointer to output vector. Vector length: nLenA + nLenB - 1
 */
#if defined _ARCv2EM_XY_AGU_MEDIUM && defined _XY_HAND_OPT
void _DSPLIB_XYCC dsp_correlate_q7( const __xy q7_t * __restrict pSrcA, uint32_t nLenA, const __xy q7_t * __restrict pSrcB, uint32_t nLenB, __xy q7_t * __restrict pDst){
    int32_t tail_size, body_size, i, j;
    int32_t a_offset,  b_offset;
    dspfx_accum16();

    _DSPLIB_store_regs(3, 5, 2);

    tail_size = dspfx_min_q31(nLenA, nLenB) - 1; // Calculate the size of correlation 'tails'
    body_size = nLenA + nLenB - 1 - (tail_size << 1); // Calculate the size of correlation 'body'

    a_offset = -1;
    b_offset = 0;

    if (nLenA > nLenB){
        _set_agu_ap0((uint32_t)pSrcA);
        _set_agu_ap1((uint32_t)(pSrcB + tail_size));
        _set_agu_ap2((uint32_t)pDst); // Set output pointer
        _set_agu_mod4(2, AGU_DT_i8, AGU_AD_INC_q7(1)); // Output pointer increment
    } else {
        _set_agu_ap0((uint32_t)pSrcB);
        _set_agu_ap1((uint32_t)(pSrcA + tail_size));
        _set_agu_ap2((uint32_t)(pDst + nLenA + nLenB - 2)); // Set output pointer
        _set_agu_mod4(2, AGU_DT_i8, AGU_AD_DEC_q7(1)); // Output pointer increment
    }


    _set_agu_mod0(0, AGU_DT_i32_cast_i8, AGU_AD_INC_q7(1)); // First input pointer increment by 1
    _set_agu_mod1(0, AGU_DT_i32_cast_i8, AGU_AD_DEC_q7_REG(0)); // First input pointer decrement
    _set_agu_mod2(1, AGU_DT_i32_cast_i8, AGU_AD_INC_q7(1)); // Second input pointer increment by 1
    _set_agu_mod3(1, AGU_DT_i32_cast_i8, AGU_AD_DEC_q7_REG(1)); // Second input pointer decrement



    // 'tail' at the begin
    for(i = 0; i < tail_size; i++){

        a_offset++;
        b_offset++;

        _set_agu_os0(a_offset); // Set first input decrement
        _set_agu_os1(b_offset); // Set second input pointer decrement

        dspfx_reset_a16();
        for(j = 0; j < i; j++){
            dspfx_a16mac_q7(_agu_u0(), _agu_u2(), acc);
        }
        dspfx_a16mac_q7(_agu_u1(), _agu_u3(), acc);
        _st_agu_u4(dspfx_q7_cast_a16(acc)); // Store output sample

    }
    // Correlation 'body'
    for(i = 0; i < body_size; i++){

        dspfx_reset_a16();
        for(j = 0; j < tail_size; j++){
            dspfx_a16mac_q7(_agu_u0(), _agu_u2(), acc);
        }
        dspfx_a16mac_q7(_agu_u1(), _agu_u3(), acc);
        _st_agu_u4(dspfx_q7_cast_a16(acc)); // Store output sample
    }

    // 'tail' at the end
    for(i = tail_size - 1; i >= 0; i--){

        a_offset--;
        b_offset--;

        _set_agu_os0(a_offset); // Set first input decrement
        _set_agu_os1(b_offset); // Set second input pointer decrement

        dspfx_reset_a16();
        for(j = 0; j < i; j++){
            dspfx_a16mac_q7(_agu_u0(), _agu_u2(), acc);
        }
        dspfx_a16mac_q7(_agu_u1(), _agu_u3(), acc);
        _st_agu_u4(dspfx_q7_cast_a16(acc)); // Store output sample
    }

    _DSPLIB_restore_regs();
}
#else //_ARCv2EM_XY_AGU_MEDIUM
void _DSPLIB_XYCC dsp_correlate_q7( const PTR(q7_t) __restrict pSrcA, uint32_t nLenA, const PTR(q7_t) __restrict pSrcB, uint32_t nLenB, PTR(q7_t) __restrict pDst)
{
    uint32_t i , j, Na1, Nb1, inc;
    const PTR(q7_t) pA;
    const PTR(q7_t) pB;
    PTR(q7_t) pC;
    dspfx_accum16()
    INIT_MACMODE_LSP();

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
        const PTR(q7_t) pAj = pA;
        const PTR(q7_t) pBj = pB + Nb1 -1 -i;
        dspfx_reset_a16();
        for ( j = 0; j < i + 1; j++)
            dspfx_a16mac_q7(*pAj++, *pBj++, acc);
        *pC=dspfx_q7_cast_a16(acc);
        pC+=inc;
    }

    PRAGMA_NO_UNROLL
    for ( i = 0; i < Na1 - Nb1; i++)
    {
        const PTR(q7_t) pAj = pA++;
        const PTR(q7_t) pBj = pB;
        dspfx_reset_a16();
        for ( j = 0;  j < Nb1; j++)
            dspfx_a16mac_q7(*pAj++, *pBj++, acc);
        *pC=dspfx_q7_cast_a16(acc);
        pC+=inc;
    }

    PRAGMA_NO_UNROLL
    for ( i = Na1-Nb1; i < Na1; i++ )
    {
        const PTR(q7_t) pAj = pA++;
        const PTR(q7_t) pBj = pB;
        dspfx_reset_a16();
        for ( j = i; j < Na1; j++ )
            dspfx_a16mac_q7(*pAj++, *pBj++, acc);
        *pC=dspfx_q7_cast_a16(acc);
        pC+=inc;
    }
}
#endif //_ARCv2EM_XY_AGU_MEDIUM

#endif //!defined(EXCLUDE_DSP_CORRELATE_Q7)
