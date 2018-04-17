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

#if !defined(EXCLUDE_DSP_CMPL_MAT_MULT_Q31)
#pragma Code("dsplib_dsp_cmpl_mat_mult_q31")

dsp_status dsp_cmpl_mat_mult_q31(const matrix_q31_t* pSrcA, const matrix_q31_t* pSrcB, matrix_q31_t* pDst)
{
    uint32_t     h = 0, k = 0, p = 0;
    PTR(q31_t) A  = (PTR(q31_t))pSrcA->pData;
    PTR(q31_t) B  = (PTR(q31_t))pSrcB->pData;
    PTR(q31_t) C  = (PTR(q31_t))pDst->pData;
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
            q31_t acc_re = 0;
            q31_t acc_im = 0;
            for (p=0; p < A_col_cnt; p++)
            {
                dspfx_accum64();
                q31_t aRe = *A++;
                q31_t aIm = *A++;
                q31_t bRe = *B++;
                q31_t bIm = *B; B += (B_col_cnt * 2 - 1);

                                       dspfx_a64mul_q31(aRe, bRe, acc);
                acc_re = dspfx_add_q31(dspfx_a64msu_q31(aIm, bIm, acc), acc_re);

                                       dspfx_a64mul_q31(aRe, bIm, acc);
                acc_im = dspfx_add_q31(dspfx_a64mac_q31(bRe, aIm, acc), acc_im);
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


#endif //!defined(EXCLUDE_DSP_CMPL_MAT_MULT_Q31)
