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

#if !defined(EXCLUDE_DSP_MAT_ADD_Q31)
#pragma Code("dsplib_dsp_mat_add_q31")

#if defined(_ARC_AS210_nomac)
#include "dsplib_ll_api.h"
#endif

static _Inline dsp_status _DSPLIB_XYCC risc_dsp_mat_add_q31(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst)
{
#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->rowCnt != pSrcB->rowCnt) || (pSrcB->rowCnt != pDst->rowCnt)
        || (pSrcA->colCnt != pSrcB->colCnt) || (pSrcB->colCnt != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    /* C = A + B */
    dsp_add_q31(
    		(PTR(q31_t))pSrcA->pData,
			(PTR(q31_t))pSrcB->pData,
			(PTR(q31_t))pDst->pData,
			pDst->size);

    return DSP_ERR_OK;
}


#define XY_MAT_ADD_THRESHOLD 32

#define XY_BLOCK_SIZE ((XY_REGION_SIZE/4)/2) // half of region

#define XYMAP_INPUT0 (0)
#define XYMAP_INPUT1 ((XY_REGION_SIZE/4)/2)
#define XYMAP_OUTPUT (0)

/**
 * Q31 matrix addition.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_mat_add_q31(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst)
{
#if defined(_ARC_AS210_nomac)
    int32_t nSamples = pDst->size;
    if (nSamples < XY_MAT_ADD_THRESHOLD || nSamples > XY_BLOCK_SIZE) {
#endif
        return risc_dsp_mat_add_q31(pSrcA, pSrcB, pDst);
#if defined(_ARC_AS210_nomac) 
    } else {
        matrix_q31_t xySrcA,  xySrcB, xyDst;
        ASSERT(nSamples <= XY_BLOCK_SIZE);

        memcpy_StoX_int32(XYMAP_INPUT0, pSrcA->pData, nSamples);
        memcpy_StoX_int32(XYMAP_INPUT1, pSrcB->pData, nSamples);

        dsp_mat_init_q31(&xySrcA, pSrcA->rowCnt, pSrcA->colCnt, (q31_t *)(XYMAP_INPUT0));
        dsp_mat_init_q31(&xySrcB, pSrcB->rowCnt, pSrcB->colCnt, (q31_t *)(XYMAP_INPUT1));
        dsp_mat_init_q31(&xyDst, pDst->rowCnt, pDst->colCnt, (q31_t *)(XYMAP_OUTPUT));

        dsp_mat_add_q31_XXtoY((const matrix_q31_t *)&xySrcA, (const matrix_q31_t *)&xySrcB, &xyDst);

        memcpy_YtoS_int32(pDst->pData, XYMAP_OUTPUT, nSamples);

        return DSP_ERR_OK;
    }
#endif
}

#endif //!defined(EXCLUDE_DSP_MAT_ADD_Q31)
