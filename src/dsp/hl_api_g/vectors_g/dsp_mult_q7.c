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

#if !defined(EXCLUDE_DSP_MULT_Q7)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_mult_q7")

#if defined _ARCv2EM_XY_AGU_SMALL && defined __FXAPI__

/**
 * Q7 element-by-element multiplication of two vectors.
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nSamples
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_mult_q7(const __xy q7_t* __restrict pSrcA, const __xy q7_t* __restrict pSrcB, __xy q7_t* __restrict pDst, uint32_t nSamples) {
    int32_t i, N  = nSamples >> 1;
	_DSPLIB_store_regs(3, 3, 0);

    _set_agu_ap0(pSrcA); // Set AP registers for pSrcA
    _set_agu_ap1(pSrcB); // Set AP registers for pSrcA
    _set_agu_ap2(pDst); // Set AP registers for pSrcB

    _set_agu_mod0(0, AGU_DT_v2q15_cast_v2q7, AGU_AD_INC_q7(2));
    _set_agu_mod1(1, AGU_DT_v2q15_cast_v2q7, AGU_AD_INC_q7(2));
    _set_agu_mod2(2, AGU_DT_v2q7, AGU_AD_INC_q7(2));

    for (i = 0; i < N; i++) {
        _st_agu_u2(_vpack2hblf(fx_q31_cast_v2q15(fx_v2q15_cast_v2a40(fx_v2a40_mpy_v2q15(fx_v2q15_cast_q31(_agu_u0()), fx_v2q15_cast_q31(_agu_u1()))))));
    }
    if (nSamples & 1) {
        _set_agu_mod0(0, AGU_DT_q15_cast_q7, AGU_AD_INC_q7(1));
        _set_agu_mod1(1, AGU_DT_q15_cast_q7, AGU_AD_INC_q7(1));
        _set_agu_mod2(2, AGU_DT_q7, AGU_AD_INC_q7(1));
        _st_agu_u2(_vpack2hblf(fx_q31_cast_v2q15(fx_v2q15_cast_v2a40(fx_v2a40_mpy_v2q15(fx_v2q15_cast_q31(_agu_u0()), fx_v2q15_cast_q31(_agu_u1()))))));
    }

	_DSPLIB_restore_regs();
}

#else //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

/**
 * Q7 element-by-element multiplication of two vectors.
 *
 * \param[in] pSrcA - pointer to 1-st input vector. Vector length: nSamples
 * \param[in] pSrcB - pointer to 2-nd input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_mult_q7 (const q7_t* __restrict  pSrcA, const q7_t* __restrict pSrcB, q7_t* __restrict pDst, uint32_t nSamples)
{
    int32_t sample;

    INIT_MACMODE_LSP();
#if (!defined(MPY_VEC4_Q7))
#if defined(__Xdsp) && defined(__FXAPI__) && defined(_ARC)
    if ( ((uint32_t)pDst & 0x3) 
        || ((uint32_t)pSrcB & 0x3) 
        || ((uint32_t)pSrcA & 0x3) 
        || ((uint32_t)nSamples & 0x3) )
    {
        for (sample = 0; sample < (int32_t)nSamples; sample++){
            pDst[sample] = dspfx_mpy_q7(pSrcA[sample], pSrcB[sample]);
        }
    }
    else
    {
        for (sample = 0; sample < (int32_t)(nSamples/4); sample++) {
            int ta02,tb02;
            int ta13 = ((int *)pSrcA)[sample];
            int tb13 = ((int *)pSrcB)[sample];
            ta02 = (ta13 << 8) & 0xff00ff00;
            ta13 = ta13 & 0xff00ff00;
            tb02 = (tb13 << 8) & 0xff00ff00;
            tb13 = tb13 & 0xff00ff00;
            ta02 = dspfx_mpy_v2q15(ta02, tb02);
            ta13 = dspfx_mpy_v2q15(ta13, tb13);

            ((int *)pDst)[sample] = (ta13 & 0xff00ff00) | ((ta02 >> 8) & 0x00ff00ff);

        }
    }
#else
    for (sample = 0; sample < (int32_t)nSamples; sample++){
        pDst[sample] = dspfx_mpy_q7(pSrcA[sample], pSrcB[sample]);
    }
#endif

#else
    {
        int32_t  startSample = ((uint32_t)pSrcA) & 0x3;
        
        //input and ouput are not 'equally' (un)aligned -> use 1d cycle
        if ((startSample != ((uint32_t)pDst & 0x3)) || (startSample != ((uint32_t)pSrcB & 0x3)))
        {
            for (sample = 0; sample < nSamples; sample++)
                pDst[sample] = dspfx_mpy_q7(pSrcA[sample], pSrcB[sample]);
        }
        else //use vector instruction
        {
            q31_t* pSrcA_32;
            q31_t* pSrcB_32;
            q31_t* pDst_32;
            int32_t N;
        
            if (startSample) {
                startSample = 4 - startSample;
                for (sample=0; (sample < startSample) && (sample < nSamples); sample++)
                       pDst[sample] = dspfx_mpy_q7(pSrcA[sample],pSrcB[sample]);
            }
        
            pSrcA_32 = (int32_t*)(void*)(&pSrcA[startSample]);
            pSrcB_32 = (int32_t*)(void*)(&pSrcB[startSample]);
            pDst_32 = (int32_t*)(void*)(&pDst[startSample]);
        
            N = ((int32_t)nSamples - startSample) >> 2;
            for (sample = 0; sample < N; sample++) 
            {
                uint32_t acc;
                acc = MPY_VEC4_Q7(*pSrcA_32++, *pSrcB_32++);
                *pDst_32++ = acc;
            }
        
            for (sample = (N << 2) + startSample; sample<nSamples;sample++) //'tail'
                pDst[sample] = dspfx_mpy_q7(pSrcA[sample],pSrcB[sample]);
        
        }
    }
#endif
}

#endif //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

#endif //!defined(EXCLUDE_DSP_MULT_Q7)
