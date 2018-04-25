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

#if !defined(EXCLUDE_DSP_OFFSET_Q7)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_offset_q7")

#if defined _ARCv2EM_XY_AGU_SMALL && defined __FXAPI__

void _DSPLIB_XYCC dsp_offset_q7(const __xy q7_t* __restrict pSrc, q7_t offset, __xy q7_t* __restrict pDst, uint32_t nSamples)
{
    int32_t i, N  = nSamples >> 1;
    v2q15_t v2q15_offset = fx_replic_v2q15((q15_t)offset << 8);
	_DSPLIB_store_regs(2, 2, 0);

    _set_agu_ap0(pSrc); // Set AP registers for pSrcA
    _set_agu_ap1(pDst); // Set AP registers for pSrcB

    _set_agu_mod0(0, AGU_DT_v2q15_cast_v2q7, AGU_AD_INC_q7(2));
    _set_agu_mod1(1, AGU_DT_v2q7, AGU_AD_INC_q7(2));

    for (i = 0; i < N; i++) {
        _st_agu_u1(_vpack2hblf(fx_q31_cast_v2q15(fx_add_v2q15(fx_v2q15_cast_q31(_agu_u0()), v2q15_offset))));
    }
    if (nSamples & 1) {
        _set_agu_mod0(0, AGU_DT_q15_cast_q7, AGU_AD_INC_q7(1));
        _set_agu_mod1(1, AGU_DT_q7, AGU_AD_INC_q7(1));
        _st_agu_u1(_vpack2hblf(fx_q31_cast_v2q15(fx_add_v2q15(fx_v2q15_cast_q31(_agu_u0()), v2q15_offset))));
    }

	_DSPLIB_restore_regs();
}

#else //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

/**
 * Adds a constant offset to each element of a vector.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param offset - additive constant
 * \param nSamples - number of samples to be processed
 */

void dsp_offset_q7 (const q7_t* __restrict pSrc, q7_t  offset, q7_t* __restrict pDst, uint32_t nSamples)
{
    int32_t sample;

#if !(defined(ADD_VEC4_Q7))
#if defined(__Xdsp) && defined(__FXAPI__)
    if ( ((uint32_t)pDst & 0x3) 
        || ((uint32_t)pSrc & 0x3) 
        || ((uint32_t)nSamples & 0x3) )
    {
        for (sample = 0; sample < (int32_t)nSamples; sample++){
            pDst[sample] = dspfx_add_q7(pSrc[sample], offset);
        }
    }
    else
    {
        int offset_v2q15 = (offset<<8) | (offset<<24);
        for (sample = 0; sample < (int32_t)(nSamples/4); sample++) {
            int ta02;
            int ta13 = ((int *)pSrc)[sample];
            ta02 = (ta13 << 8) & 0xff00ff00;
            ta13 = ta13 & 0xff00ff00;

            ta02 = dspfx_add_v2q15(ta02,offset_v2q15);
            ta13 = dspfx_add_v2q15(ta13,offset_v2q15);

            ((int *)pDst)[sample] = (ta13 & 0xff00ff00) | ((ta02 >> 8) & 0x00ff00ff);
        }
    }
#else
    for (sample = 0; sample < (int32_t)nSamples; sample++){
        pDst[sample] = dspfx_add_q7(pSrc[sample], offset);
    }
#endif // __Xdsp
#else

    int32_t startSample = (((uint32_t)pSrc) & 0x3);

    //input and ouput are not 'equally' (un)aligned -> use 1d cycle
    if (startSample != ((uint32_t)pDst & 0x3 ))
    {
        for (sample = 0; sample < nSamples; sample++)
            pDst[sample] = dspfx_add_q7(pSrc[sample], offset);
    }
    else //use vector instruction
    {
        q31_t* pDst_32;
        q31_t* pSrc_32;
        int32_t N;
        q31_t offset_32 = ((q31_t)offset & 0xff);
        offset_32 |= (((q31_t)offset<<8) & 0xff00);
        offset_32 |= (((q31_t)offset<<16) & 0xff0000);
        offset_32 |= (((q31_t)offset<<24) & 0xff000000);

        if (startSample) {
            startSample = 4 - startSample;
            for (sample=0; (sample < startSample) && (sample < nSamples); sample++)
                   pDst[sample] = dspfx_add_q7(pSrc[sample],offset);
        }

        pDst_32 = (int32_t*)(void*)(&pDst[startSample]);
        pSrc_32 = (int32_t*)(void*)(&pSrc[startSample]);

        N = ((int32_t)nSamples - startSample) >> 2;
        for (sample = 0; sample < N; sample++) 
             *pDst_32++ = ADD_VEC4_Q7(*pSrc_32++, offset_32);

        for (sample = (N << 2) + startSample; sample<nSamples;sample++) //'tail'
             pDst[sample] = dspfx_add_q7(pSrc[sample],offset);
    }
#endif
}

#endif //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

#endif //!defined(EXCLUDE_DSP_OFFSET_Q7)

