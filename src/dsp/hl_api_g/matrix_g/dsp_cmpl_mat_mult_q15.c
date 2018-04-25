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

#if !defined(EXCLUDE_DSP_CMPL_MAT_MULT_Q15)
#pragma Code("dsplib_dsp_cmpl_mat_mult_q15")

#if defined __FXAPI__ && defined __GUARD_ACCUM__ && ((defined __Xdsp && defined __Xdsp_complex) || (defined  __Xdsp && __Xdsp_version > 1))

dsp_status dsp_cmpl_mat_mult_q15(const matrix_q15_t* pSrcA, const matrix_q15_t* pSrcB, matrix_q15_t* pDst)
{
    uint32_t h = 0, k = 0, p = 0;
    PTR(cq15_t) A = (PTR(cq15_t))pSrcA->pData;
    PTR(cq15_t) B = (PTR(cq15_t))pSrcB->pData;
    PTR(cq15_t) C = (PTR(cq15_t))pDst->pData;
    int32_t A_col_cnt = pSrcA->colCnt;
    int32_t B_col_cnt = pSrcB->colCnt;
    int32_t A_row_cnt = pSrcA->rowCnt;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->colCnt != pSrcB->rowCnt) || (pSrcA->rowCnt != pDst->rowCnt) || (pSrcB->colCnt != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    __builtin_assume(A_col_cnt > 0);
    __builtin_assume(B_col_cnt > 0);
    __builtin_assume(A_row_cnt > 0);
    /* C = A * B */
    for (h=0; h < A_row_cnt; h++)
    {
        for (k=0; k < B_col_cnt; k++)
        {
        	v2accum40_t vacc = {};
            for (p=0; p < A_col_cnt; p++)
            {
                vacc = fx_v2a40_cmac_cq15(vacc, *A++, *B );
                B += B_col_cnt;
            }
            *C++ = fx_cq15_cast_rnd_v2a40(vacc);
            B -= B_col_cnt * A_col_cnt - 1;
            A -= A_col_cnt;
        }
        A += A_col_cnt;
        B -= B_col_cnt;
    }
    return DSP_ERR_OK;
}


#else

dsp_status dsp_cmpl_mat_mult_q15(const matrix_q15_t* pSrcA, const matrix_q15_t* pSrcB, matrix_q15_t* pDst)
{
    uint32_t h = 0, k = 0, p = 0;
    PTR(q15_t) A = (PTR(q15_t))pSrcA->pData;
    PTR(q15_t) B = (PTR(q15_t))pSrcB->pData;
    PTR(q15_t) C = (PTR(q15_t))pDst->pData;
    int32_t A_col_cnt = pSrcA->colCnt;
    int32_t B_col_cnt = pSrcB->colCnt;
    int32_t A_row_cnt = pSrcA->rowCnt;

    INIT_MACMODE();
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
            q15_t acc_re = 0;
            q15_t acc_im = 0;
            for (p=0; p < A_col_cnt; p++)
            {
                dspfx_accum32();
                q15_t aRe = *A++;
                q15_t aIm = *A++;
                q15_t bRe = *B++;
                q15_t bIm = *B; B += (B_col_cnt * 2 - 1);

                                       dspfx_a32mul_q15(aRe, bRe, acc);
                acc_re = dspfx_add_q15(dspfx_a32msu_q15(aIm, bIm, acc), acc_re);

                                       dspfx_a32mul_q15(aRe, bIm, acc);
                acc_im = dspfx_add_q15(dspfx_a32mac_q15(bRe, aIm, acc), acc_im);
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

#endif

#endif //!defined(EXCLUDE_DSP_CMPL_MAT_MULT_Q15)
