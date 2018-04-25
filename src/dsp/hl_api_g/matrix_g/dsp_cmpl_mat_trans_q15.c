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

#if !defined(EXCLUDE_DSP_CMPL_MAT_TRANS_Q15)
#pragma Code("dsplib_dsp_cmpl_mat_trans_q15")

/**
 * Q15 complex matrix transpose.
 *
 * \param[in] pSrc - pointer to input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */
#if defined _ARCVER_ARCv2HS_DSP && defined (__Xunaligned)

dsp_status _DSPLIB_XYCC dsp_cmpl_mat_trans_q15(const matrix_q15_t *pSrc, matrix_q15_t *pDst)
{
    int32_t    c,r;
    PTR(uint32_t)__restrict dst = (PTR(uint32_t)) (void*) pDst->pData;
    PTR(uint32_t)__restrict src = (PTR(uint32_t)) (void*) pSrc->pData;
    int32_t width = pSrc->colCnt;
    int32_t height = pSrc->rowCnt;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrc->rowCnt != pDst->colCnt) || (pSrc->colCnt != pDst->rowCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    if (( width & 1) || (height & 1))
    {

        int32_t backstride = - (int32_t)(pSrc->size - 1);
        __builtin_assume(width > 0);
        __builtin_assume(height > 0);
        PRAGMA_NO_UNROLL
            for (c = 0; c < width; c++)
            {
                for (r = 0; r < height; r++)
                {
                    *dst++ = *src;
                    src += width;
                }
                src += backstride;
            }
    }else{

        int32_t backstride = - (int32_t)(pSrc->size - 2);
        PTR(uint32_t) __restrict dst2 = (PTR(uint32_t)) pDst->pData+height;
        __builtin_assume(width > 0);
        __builtin_assume(height > 0);
        PRAGMA_NO_UNROLL
            for (c = 0; c < (width>>1); c++)
            {
                PRAGMA_UNROLL_2
                    for (r = 0; r < (height>>1); r++)
                    {
                        q31_t tmp;
                        q31_t m00=*src++;
                        q31_t m01=*src++;
                        src += (width-2);
                        q31_t m10=*src++;
                        q31_t m11=*src++;
                        src += (width-2);
                        tmp=m01;
                        m01=m10;
                        m10=tmp;
                        *dst++=m00;
                        *dst++=m01;
                        *dst2++=m10;
                        *dst2++=m11;
                    }
                    src += backstride;
                    dst+=height;
                    dst2+=height;
            }
    }
    return DSP_ERR_OK;
}
#else
dsp_status _DSPLIB_XYCC dsp_cmpl_mat_trans_q15(const matrix_q15_t *pSrc, matrix_q15_t *pDst)
{
    int32_t    c,r;
    PTR(uint32_t)__restrict dst = (PTR(uint32_t)) (void*) pDst->pData;
    PTR(uint32_t)__restrict src = (PTR(uint32_t)) (void*) pSrc->pData;
    int32_t width = pSrc->colCnt;
    int32_t height = pSrc->rowCnt;
    int32_t backstride = - (pSrc->size - 1);

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrc->rowCnt != pDst->colCnt) || (pSrc->colCnt != pDst->rowCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    pDst->rowCnt = pSrc->colCnt;
    pDst->colCnt = pSrc->rowCnt;
    __builtin_assume(width > 0);
    __builtin_assume(height > 0);
    for (c = 0; c < width; c++)
    {
        for (r = 0; r < height; r++)
        {
            *dst++ = *src;
            src += width;
        }
        src += backstride;
    }

    return DSP_ERR_OK;
}

#endif // _ARCVER_ARCv2HS_DSP && defined (__Xunaligned)

#endif //!defined(EXCLUDE_DSP_MAT_TRANS_Q15)
