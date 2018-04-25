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

#if !defined(EXCLUDE_DSP_CMPL_MAT_SUB_F32)
#pragma Code("dsplib_dsp_cmpl_mat_sub_f32")

/**
 * Floating-point complex matrix subtraction.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_cmpl_mat_sub_f32(const matrix_f32_t *pSrcA, const matrix_f32_t *pSrcB, matrix_f32_t *pDst)
{
#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->rowCnt != pSrcB->rowCnt) || (pSrcB->rowCnt != pDst->rowCnt)
        || (pSrcA->colCnt != pSrcB->colCnt) || (pSrcB->colCnt != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    /* C = A + B */
    dsp_sub_f32(
    		(PTR(float))pSrcA->pData,
			(PTR(float))pSrcB->pData,
			(PTR(float))pDst->pData,
			pDst->size * 2);

    return DSP_ERR_OK;
}

#endif //!defined(EXCLUDE_DSP_MAT_SUB_F32)
