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

#if !defined(EXCLUDE_DSP_CMPL_MAT_SCALE_Q15)
#pragma Code("dsplib_dsp_cmpl_mat_scale_q15")

/**
 * Q15 complex matrix scaling.
 * The overall scale factor applied to the fixed-point data is:
 * scale = scaleFract * 2^shift.
 *
 * \param[in] pSrc - pointer to input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \param factor
 * \param shift
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_cmpl_mat_scale_q15(const matrix_q15_t *pSrc, q15_t factor, int32_t shift, matrix_q15_t *pDst)
{
#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrc->rowCnt != pDst->rowCnt) || (pSrc->colCnt != pDst->colCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    dsp_scale_q15(
    		(PTR(q15_t))pSrc->pData,
			factor,
			shift,
			(PTR(q15_t))pDst->pData,
			pDst->size * 2);

    return DSP_ERR_OK;
}

#endif //!defined(EXCLUDE_DSP_MAT_SCALE_Q15)
