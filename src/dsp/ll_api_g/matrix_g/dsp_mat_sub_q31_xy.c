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

// Produce instance with: MAC32x32; pIn0 - X; 

#pragma Code("dsplib_dsp_mat_sub_q31_XXtoY")

/**
 *  Q31 matrix subtraction.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status dsp_mat_sub_q31_XXtoY(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst) 
{
    uint32_t h = 0;
    int32_t nSamples;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->rowCnt != pSrcB->rowCnt) || (pSrcB->rowCnt != pDst->rowCnt)
        || (pSrcA->colCnt != pSrcB->colCnt) || (pSrcB->colCnt != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif


    _set_ax0((uint32_t)pSrcA->pData);
    _set_mx00(1);
    _set_ax1((uint32_t)pSrcB->pData);
    _set_mx10(1);
    _set_ay0((uint32_t)pDst->pData);
    _set_my00(1);

    nSamples = pDst->size;

    /* C = A + B */
    for (h = 0; h != nSamples; h++)
    {
        _set_y0_u0( dspfx_sub_q31(_x0_u0(), _x1_u0()));
    }
    return DSP_ERR_OK;
}

// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_mat_sub_q31_YYtoX")

/**
 *  Q31 matrix subtraction.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status dsp_mat_sub_q31_YYtoX(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst) 
{
    uint32_t h = 0;
    int32_t nSamples;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->rowCnt != pSrcB->rowCnt) || (pSrcB->rowCnt != pDst->rowCnt)
        || (pSrcA->colCnt != pSrcB->colCnt) || (pSrcB->colCnt != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif


    _set_ay0((uint32_t)pSrcA->pData);
    _set_my00(1);
    _set_ay1((uint32_t)pSrcB->pData);
    _set_my10(1);
    _set_ax0((uint32_t)pDst->pData);
    _set_mx00(1);

    nSamples = pDst->size;

    /* C = A + B */
    for (h = 0; h != nSamples; h++)
    {
        _set_x0_u0( dspfx_sub_q31(_y0_u0(), _y1_u0()));
    }
    return DSP_ERR_OK;
}


#endif
