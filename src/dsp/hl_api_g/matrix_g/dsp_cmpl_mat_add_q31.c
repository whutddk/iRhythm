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

#if !defined(EXCLUDE_DSP_CMPL_MAT_ADD_Q31)
#pragma Code("dsplib_dsp_cmpl_mat_add_q31")

/**
 * Q31 complex matrix addition.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_cmpl_mat_add_q31(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst)
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
			pDst->size * 2);

    return DSP_ERR_OK;
}

#endif //!defined(EXCLUDE_DSP_MAT_ADD_Q31)
