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

#if defined(EXCLUDE_DSP_MAT_INIT_Q31)
#define EXCLUDE_DSP_MAT_SCALE_Q31
#endif //defined(EXCLUDE_DSP_MAT_INIT_Q31)

#if !defined(EXCLUDE_DSP_MAT_SCALE_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"


#pragma Code("dsplib_dsp_mat_scale_q31")
static _Inline dsp_status _DSPLIB_XYCC risc_dsp_mat_scale_q31(const matrix_q31_t *pSrc, q31_t factor, int32_t shift, matrix_q31_t *pDst)
{

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrc->rowCnt != pDst->rowCnt) || (pSrc->colCnt != pDst->colCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    dsp_scale_q31((PTR(q31_t))pSrc->pData, factor,shift,(PTR(q31_t))pDst->pData, pDst->size);

    return DSP_ERR_OK;
}

#define XY_MAT_SCALE_THRESHOLD 32

#define XY_BLOCK_SIZE ((XY_REGION_SIZE/4)) 

#define XYMAP_INPUT0 (0)
#define XYMAP_OUTPUT (0)

/**
 * Q31 matrix scaling.
 * The overall scale factor applied to the fixed-point data is:
 * scale = scaleFract * 2^shift.
 *
 * \param[in] pSrc - pointer to input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \param factor
 * \param shift
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_mat_scale_q31(const matrix_q31_t *pSrc, q31_t factor, int32_t shift, matrix_q31_t *pDst)
{
#if defined(_ARC_AS210_mac32)
    int32_t nSamples = pDst->size;
    if (nSamples < XY_MAT_SCALE_THRESHOLD || nSamples > XY_BLOCK_SIZE) {
#endif
        return risc_dsp_mat_scale_q31(pSrc, factor, shift, pDst);
#if defined(_ARC_AS210_mac32) 
    } else {
        matrix_q31_t xySrc, xyDst;
        ASSERT(nSamples <= XY_BLOCK_SIZE);

        memcpy_StoX_int32(XYMAP_INPUT0, pSrc->pData, nSamples);

        dsp_mat_init_q31(&xySrc, pSrc->rowCnt, pSrc->colCnt, (q31_t *)(XYMAP_INPUT0));
        dsp_mat_init_q31(&xyDst, pDst->rowCnt, pDst->colCnt, (q31_t *)(XYMAP_OUTPUT));

        dsp_mat_scale_q31_XtoY_MAC32x32((const matrix_q31_t *)&xySrc, factor, shift, &xyDst);

        memcpy_YtoS_int32(pDst->pData, XYMAP_OUTPUT, nSamples);

        return DSP_ERR_OK;
    }
#endif
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_MAT_SCALE_Q31)
