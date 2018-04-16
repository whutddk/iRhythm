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

#if !defined(EXCLUDE_DSP_SCALE_Q7)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_scale_q7")

#if defined _ARCv2EM_XY_AGU_SMALL && defined __FXAPI__

void _DSPLIB_XYCC dsp_scale_q7(const __xy q7_t* __restrict pSrc, q7_t    scaleFract, int8_t  shift, __xy q7_t* __restrict pDst, uint32_t nSamples)
{
    int32_t i, N  = nSamples >> 1;
    v2q15_t v2q15_scaleFract = fx_create_v2q15((q15_t)scaleFract << 8, (q15_t)scaleFract << 8);
    v2i16_t v2i16_shift = fx_create_v2i16(shift, shift);
	_DSPLIB_store_regs(2, 2, 0);

    _set_agu_ap0(pSrc); // Set AP registers for pSrcA
    _set_agu_ap1(pDst); // Set AP registers for pSrcB

    _set_agu_mod0(0, AGU_DT_v2q15_cast_v2q7, AGU_AD_INC_q7(2));
    _set_agu_mod1(1, AGU_DT_v2q7, AGU_AD_INC_q7(2));

    for (i = 0; i < N; i++) {
        _st_agu_u1(_vpack2hblf(fx_q31_cast_v2q15(fx_asl_v2q15(fx_v2q15_cast_v2a40(fx_v2a40_mpy_v2q15(fx_v2q15_cast_q31(_agu_u0()), v2q15_scaleFract)), v2i16_shift))));
    }
    if (nSamples & 1) {
        _set_agu_mod0(0, AGU_DT_q15_cast_q7, AGU_AD_INC_q7(1));
        _set_agu_mod1(1, AGU_DT_q7, AGU_AD_INC_q7(1));
        _st_agu_u1(_vpack2hblf(fx_q31_cast_v2q15(fx_asl_v2q15(fx_v2q15_cast_v2a40(fx_v2a40_mpy_v2q15(fx_v2q15_cast_q31(_agu_u0()), v2q15_scaleFract)), v2i16_shift))));
    }

	_DSPLIB_restore_regs();
}

#else //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

/**
 * Q7 multiply a vector by a scalar value.
 * The algorithm used is:
 * pDst[n] = (pSrc[n] * scaleFract) << shift,   0 <= n < nSamples. 
 * The overall scale factor applied to the fixed-point data is:
 * scale = scaleFract * 2^shift.
 *
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param scaleFract
 * \param shift
 * \param nSamples - number of samples to be processed
 */

void dsp_scale_q7 (const q7_t* __restrict pSrc, q7_t    scaleFract, int8_t  shift, q7_t* __restrict pDst, uint32_t nSamples)
{
    int32_t sample;
    
	INIT_MACMODE();
#if (!defined(MPY_VEC4_Q7)) || (!defined(ASL_VEC4_Q7))
#if defined(__Xdsp) && defined(__FXAPI__) && defined (_ARC)
    if ( ((uint32_t)pDst & 0x3) 
        || ((uint32_t)pSrc & 0x3) 
        || ((uint32_t)nSamples & 0x3) )
    {
        for (sample = 0; sample < (int32_t)nSamples; sample++){
            pDst[sample] = dspfx_asl_q7((pSrc[sample] * scaleFract)>>7, shift);
        }
    }
    else
    {
        int scaleFract_v2q15 = fx_q31_cast_v2q15(fx_create_v2q15((scaleFract<<8), (scaleFract<<8)));
        v2i16_t shift_v2i16 = fx_create_v2i16((shift),(shift));
        for (sample = 0; sample < (int32_t)(nSamples/4); sample++) {
            int ta02;
            int ta13 = ((int *)pSrc)[sample];
            ta02 = (ta13 << 8) & 0xff00ff00;
            ta13 = ta13 & 0xff00ff00;

            ta02 = dspfx_mpy_v2q15(ta02, scaleFract_v2q15);
            ta02 = fx_q31_cast_v2q15(fx_asl_v2q15(fx_v2q15_cast_q31(ta02),shift_v2i16));
            ta13 = dspfx_mpy_v2q15(ta13, scaleFract_v2q15);
            ta13 = fx_q31_cast_v2q15(fx_asl_v2q15(fx_v2q15_cast_q31(ta13),shift_v2i16));

            ((int *)pDst)[sample] = (ta13 & 0xff00ff00) | ((ta02 >> 8) & 0x00ff00ff);
        }
    }
#else
    for (sample = 0; sample < (int32_t)nSamples; sample++){
        pDst[sample] = dspfx_asl_q7((pSrc[sample] * scaleFract)>>7, shift);
    }
#endif

#else
    {
        int32_t  startSample = ((uint32_t)pSrc) & 0x3;
        
        //input and ouput are not 'equally' (un)aligned -> use 1d cycle
        if (startSample != ((uint32_t)pDst & 0x3 ))
        {
            for (sample=0; sample < nSamples; sample++) 
            {
                pDst[sample] = dspfx_asl_q7((pSrc[sample] * scaleFract)>>7, shift);
            }   
        }
        else //use vector instruction
        {
            q31_t* pSrc_32;
            q31_t* pDst_32;
            q31_t scaleFract_32;
			int32_t N;
	    
            if (startSample) {
                startSample = 4 - startSample;
                for (sample=0; (sample < startSample) && (sample < nSamples); sample++)
                {
                    pDst[sample] = dspfx_asl_q7((pSrc[sample] * scaleFract)>>7, shift);
                }  
            }
        
            pSrc_32 = (int32_t*)(void*)(&pSrc[startSample]);
            pDst_32 = (int32_t*)(void*)(&pDst[startSample]);
        
            scaleFract_32 = ((q31_t)scaleFract & 0xff);
            scaleFract_32 |= (((q31_t)scaleFract<<8) & 0xff00);
            scaleFract_32 |= (((q31_t)scaleFract<<16) & 0xff0000);
            scaleFract_32 |= (((q31_t)scaleFract<<24) & 0xff000000);
        
			N = ((int32_t)nSamples - startSample) >> 2;
			for (sample = 0; sample < N; sample++) 
			{
                uint32_t acc;
                acc = MPY_VEC4_Q7(*pSrc_32++, scaleFract_32);
                *pDst_32++ = ASL_VEC4_Q7(acc, shift);
            }
        
            for (sample = (N << 2) + startSample; sample<nSamples;sample++) //'tail'
            {
                pDst[sample] = dspfx_asl_q7((pSrc[sample] * scaleFract)>>7, shift);
            }   
        
        }
    }
#endif
}

#endif //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

#endif //!defined(EXCLUDE_DSP_SCALE_Q7)
