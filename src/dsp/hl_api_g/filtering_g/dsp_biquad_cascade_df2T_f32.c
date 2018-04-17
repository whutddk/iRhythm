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

#if !defined(EXCLUDE_DSP_BIQUAD_CASCADE_DF2T_F32)
#pragma Code("dsplib_dsp_biquad_cascade_df2T_f32")

/**
 * initialization for f32 IIR filtering
 *
 * \param inst - pointer to instance structure
 * \param numStages
 * \param pCoeffs - pointer to coefficients vector. Vector length: numStages * 5
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_biquad_cascade_df2T_getsize_f32" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_biquad_cascade_df2T_init_f32(iir_biquad_f32_t *inst, uint8_t numStages, const PTR(f32_t) pCoeffs, PTR(f32_t) pState)
{
    inst->filter.numTaps = numStages;
    inst->filter.pCoeffs = (f32_t*) pCoeffs;
    inst->filter.pData = pState;
    _MEM_FILL_F32(pState,0.0f,numStages*2);
    return DSP_ERR_OK;

};

/**
 * f32 DF2 type Biquad-Cascade (IIR) filter
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_biquad_cascade_df2T_f32(const iir_biquad_f32_t *inst, const PTR(f32_t) __restrict pSrc, PTR(f32_t) __restrict pDst, uint32_t nSamples)
{
    int32_t stage; // Stage counter
    uint32_t i; // Sample counter 
    PTR(f32_t) src = (PTR(f32_t)) pSrc;
    PTR(f32_t) dst = pDst;

    const PTR(f32_t) coeffs =  (PTR(f32_t)) inst->filter.pCoeffs;
    PTR(f32_t) states = (PTR(f32_t)) inst->filter.pData;

    PRAGMA_NO_UNROLL
    for (stage = 0; stage < inst->filter.numTaps; stage++, states+=2)
    {
        f32_t b0, b1, b2, a1, a2;
        f32_t w0, w1, w2;

        b0 = *coeffs++;
        b1 = *coeffs++;
        b2 = *coeffs++;
        a1 = *coeffs++;
        a2 = *coeffs++;

        w1 = states[0];
        w2 = states[1];

        for (i = 0; i < nSamples; i++) 
        {
            w0 = src[i] + a1*w1 + a2*w2;

            dst[i] = b0 * w0 + b1 * w1 + b2 * w2;

            w2 = w1;
            w1 = w0;
        }

        states[0] = w1;
        states[1] = w2;

        src = pDst;
    }

};


void dsp_biquad_cascade_df2T_delete_f32(iir_biquad_f32_t* inst)
{
 // TODO: Place code here
};

#endif //!defined(EXCLUDE_DSP_BIQUAD_CASCADE_DF2T_F32)
