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

#if !defined(EXCLUDE_DSP_MAT_SCALE_F32)
#pragma Code("dsplib_dsp_mat_scale_f32")

/**
 * Floating-point matrix scaling.
 *
 * \param[in] pSrc - pointer to input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \param factor
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_mat_scale_f32(const matrix_f32_t *pSrc, f32_t factor, matrix_f32_t *pDst)
{
#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrc->rowCnt != pDst->rowCnt) || (pSrc->colCnt != pDst->colCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    dsp_scale_f32(
    		(PTR(f32_t))pSrc->pData,
			factor,
			(PTR(f32_t))pDst->pData,
			pDst->size);

    return DSP_ERR_OK;
}

#endif //!defined(EXCLUDE_DSP_MAT_SCALE_F32)
