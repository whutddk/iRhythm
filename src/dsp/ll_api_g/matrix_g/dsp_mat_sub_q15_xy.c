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
#if defined(_ARC_AS210_nomac)
#include "dsplib_ll_api.h"

// Produce instance with: MAC32x32; pIn0 - X; 

#pragma Code("dsplib_dsp_mat_sub_q15_XXtoY")

/**
 *  Q15 matrix subtraction.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status dsp_mat_sub_q15_XXtoY(const matrix_q15_t *pSrcA, const matrix_q15_t *pSrcB, matrix_q15_t *pDst) 
{
    int32_t A, B, C;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->rowCnt != pSrcB->rowCnt) || (pSrcB->rowCnt != pDst->rowCnt)
        || (pSrcA->colCnt != pSrcB->colCnt) || (pSrcB->colCnt != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    A = (int32_t)(void *)pSrcA->pData;
    B = (int32_t)(void *)pSrcB->pData;
    C = (int32_t)(void *)pDst->pData;

    dsp_sub_q15_XXtoY(A,B,C,pDst->size);

    return DSP_ERR_OK;
}

// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_mat_sub_q15_YYtoX")

/**
 *  Q15 matrix subtraction.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status dsp_mat_sub_q15_YYtoX(const matrix_q15_t *pSrcA, const matrix_q15_t *pSrcB, matrix_q15_t *pDst) 
{
    int32_t A, B, C;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->rowCnt != pSrcB->rowCnt) || (pSrcB->rowCnt != pDst->rowCnt)
        || (pSrcA->colCnt != pSrcB->colCnt) || (pSrcB->colCnt != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    A = (int32_t)(void *)pSrcA->pData;
    B = (int32_t)(void *)pSrcB->pData;
    C = (int32_t)(void *)pDst->pData;

    dsp_sub_q15_YYtoX(A,B,C,pDst->size);

    return DSP_ERR_OK;
}


#endif
