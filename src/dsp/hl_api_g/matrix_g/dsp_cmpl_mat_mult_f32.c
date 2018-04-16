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

#if !defined(EXCLUDE_DSP_CMPL_MAT_MULT_F32)
#pragma Code("dsplib_dsp_cmpl_mat_mult_f32")

dsp_status dsp_cmpl_mat_mult_f32(const matrix_f32_t* pSrcA, const matrix_f32_t* pSrcB, matrix_f32_t* pDst)
{
    int32_t     h = 0, k = 0, p = 0;
    PTR(f32_t) A = (PTR(f32_t))pSrcA->pData;
    PTR(f32_t) B = (PTR(f32_t))pSrcB->pData;
    PTR(f32_t) C = (PTR(f32_t))pDst->pData;
    int32_t A_col_cnt = pSrcA->colCnt;
    int32_t B_col_cnt = pSrcB->colCnt;
    int32_t A_row_cnt = pSrcA->rowCnt;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrcA->colCnt != pSrcB->rowCnt) || (pSrcA->rowCnt != pDst->rowCnt) || (pSrcB->colCnt != pDst->colCnt) )
    {
    	return DSP_ERR_MAT_DIMENSIONS;
    }
#endif

    __builtin_assume(A_col_cnt > 0);
    __builtin_assume(B_col_cnt > 0);
    __builtin_assume(A_row_cnt > 0);
    /* C = A * B */
    for (h=0; h < A_row_cnt; h++)
    {
        for (k=0; k < B_col_cnt; k++)
        {
            f32_t acc_re = 0;
            f32_t acc_im = 0;
            for (p=0; p < A_col_cnt; p++)
            {
                f32_t aRe =*A++;
                f32_t aIm =*A++;
                f32_t bRe =*B++;
                f32_t bIm =*B; B += (B_col_cnt * 2 - 1);

                acc_re += aRe * bRe - aIm * bIm;
                acc_im += aRe * bIm + aIm * bRe;
            }
            *C++ = acc_re;
            *C++ = acc_im;
            B -= (B_col_cnt * A_col_cnt - 1) * 2;
            A -= (A_col_cnt) * 2;
        }
        A += A_col_cnt * 2;
        B -= B_col_cnt * 2;
    }
    return DSP_ERR_OK;
}

#endif //!defined(EXCLUDE_DSP_CMPL_MAT_MULT_F32)
