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

#if !defined(EXCLUDE_DSP_CMPL_MAT_TRANS_Q31)
#pragma Code("dsplib_dsp_cmpl_mat_trans_q31")

/**
 * Q31 complex matrix transpose.
 *
 * \param[in] pSrc - pointer to input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_cmpl_mat_trans_q31(const matrix_q31_t *pSrc, matrix_q31_t *pDst)
{
    int32_t c,r;
    PTR(q31_t) __restrict dst = (PTR(q31_t))pDst->pData;
    PTR(q31_t) __restrict src = (PTR(q31_t))pSrc->pData;
    int32_t width = pSrc->colCnt;
    int32_t height = pSrc->rowCnt;
    int32_t backstride = - (int32_t)(pSrc->size - 1) * 2;

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
            *dst++ = *src++;
            *dst++ = *src;
            src += pSrc->colCnt * 2 - 1;
        }
        src += backstride;
    }

    return DSP_ERR_OK;
}


#endif //!defined(EXCLUDE_DSP_MAT_TRANS_Q31)
