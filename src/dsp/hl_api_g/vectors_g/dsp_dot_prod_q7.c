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

#if !defined(EXCLUDE_DSP_DOT_PROD_Q7)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_dot_prod_q7")

/**
 * Computes the Q7 dot product of two vectors.
 * The vectors are multiplied element-by-element and then summed. 
 * The return result is in 18.14 format.
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nSamples
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 * \param[out] result - pointer to output result
 */

#if defined _ARCv2EM_XY_AGU_SMALL && defined __FXAPI__

void _DSPLIB_XYCC dsp_dot_prod_q7 (const __xy q7_t* __restrict  pSrcA, const __xy q7_t* __restrict  pSrcB, uint32_t nSamples, q31_t* result)
{
    uint32_t sample;
    accum40_t acc;

	_DSPLIB_store_regs(2, 2, 0);

    _set_agu_ap0(pSrcA); // Set AP registers for pSrcA
    _set_agu_ap1(pSrcB); // Set AP registers for pSrcB

    _set_agu_mod0(0, AGU_DT_i32_cast_i8, AGU_AD_INC_i8(1));
    _set_agu_mod1(1, AGU_DT_i32_cast_i8, AGU_AD_INC_i8(1));

    acc = fx_create_a40(0, 0);
    for (sample = 0; sample < nSamples; sample++){
         acc = fx_a40_mac_nf_q15(acc, (q15_t)_agu_u0(), (q15_t)_agu_u1());
    }
    *result = fx_q31_cast_asr_rnd_a40(acc, 1 - dspfx_accshift_const);

    _DSPLIB_restore_regs();
}

#else

void dsp_dot_prod_q7 (const q7_t* __restrict  pSrcA, const q7_t* __restrict  pSrcB, uint32_t nSamples, q31_t* result)
{
    uint32_t sample;
    dspfx_accum32()
    INIT_MACMODE_LSP();
    dspfx_reset_a32();
    for (sample = 0; sample < nSamples; sample++){
         dspfx_a32mac_q15(*pSrcA++, *pSrcB++, acc);
    }
    *result = dspfx_get_a32(acc);
}

#endif

/******************* Vector Multiplication ***********************************/
#pragma Code()

#endif //!defined(EXCLUDE_DSP_DOT_PROD_Q7)
