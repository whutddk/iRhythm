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

#if defined(EXCLUDE_DSP_MAT_INIT_Q15)
#define EXCLUDE_DSP_MAT_SCALE_Q15
#endif //defined(EXCLUDE_DSP_MAT_INIT_Q15)

#if !defined(EXCLUDE_DSP_MAT_SCALE_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsplib_helper.h"

#pragma Code("dsplib_dsp_mat_scale_q15")

static _Inline dsp_status _DSPLIB_XYCC risc_dsp_mat_scale_q15(const matrix_q15_t *pSrc, q15_t factor, int32_t shift, matrix_q15_t *pDst)
{
#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrc->rowCnt != pDst->rowCnt) || (pSrc->colCnt != pDst->colCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    dsp_scale_q15((PTR(q15_t))pSrc->pData, factor,shift,(PTR(q15_t))pDst->pData, pDst->size);

    return DSP_ERR_OK;
}

#define XY_MAT_SCALE_THRESHOLD 32

#define XY_BLOCK_SIZE ((XY_REGION_SIZE/2))

#define XYMAP_INPUT0 (0)
#define XYMAP_OUTPUT (0)

/**
 * Q15 matrix scaling.
 * The overall scale factor applied to the fixed-point data is:
 * scale = scaleFract * 2^shift.
 *
 * \param[in] pSrc - pointer to input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \param factor
 * \param shift
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_mat_scale_q15(const matrix_q15_t *pSrc, q15_t factor, int32_t shift, matrix_q15_t *pDst)
{
#if defined(_ARC_AS210_mac16)
    int32_t nSamples = pDst->size;
    if (nSamples < XY_MAT_SCALE_THRESHOLD || nSamples > XY_BLOCK_SIZE) {
#endif
        return risc_dsp_mat_scale_q15(pSrc, factor, shift, pDst);
#if defined(_ARC_AS210_mac16) 
    } else {
        matrix_q15_t xySrc, xyDst;
        ASSERT(nSamples <= XY_BLOCK_SIZE);

        memcpy((void *)((q15_t *)(_get_xbase()) + XYMAP_INPUT0), pSrc->pData, nSamples*2);

        //_BurstSMemToXMemSafe(XYMAP_INPUT0, pSrcA->pData, nSamples/2);

        dsp_mat_init_q15(&xySrc, pSrc->rowCnt, pSrc->colCnt, (q15_t *)(XYMAP_INPUT0));
        dsp_mat_init_q15(&xyDst, pDst->rowCnt, pDst->colCnt, (q15_t *)(XYMAP_OUTPUT));

        dsp_mat_scale_q15_XtoY_MAC16x16((const matrix_q15_t *)&xySrc, factor, shift, &xyDst);

        memcpy((void *)pDst->pData, (void *)((q15_t *)(_get_ybase()) + XYMAP_OUTPUT), nSamples*2);
        //_BurstYMemToSMemSafe(pDst->pData, XYMAP_OUTPUT, nSamples/2);

        return DSP_ERR_OK;
    }
#endif
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_MAT_SCALE_Q15)
