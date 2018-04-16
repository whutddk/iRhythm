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
#if defined(_ARC_AS210_mac16)

// Produce instance with: MAC16x16; pIn0 - X; 

#pragma Code("dsplib_dsp_mat_mult_q15_XXtoY_MAC16x16")

/**
 *  Q15 matrix multiplication.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status dsp_mat_mult_q15_XXtoY_MAC16x16(const matrix_q15_t *pSrcA, const matrix_q15_t *pSrcB, matrix_q15_t *pDst) 
{
    uint32_t h = 0, k = 0, p = 0;
    uint32_t hmax;
    uint32_t colCntA = pSrcA->colCnt, rowCntA = pSrcA->rowCnt, colCntB = pSrcB->colCnt;
    int32_t backstrideA = AM_OFFS(- (int32_t)(colCntA - 1));
    int32_t backstrideB = AM_OFFS(- (int32_t)(dspfx_mpy_i32(colCntA - 1, colCntB) - 1));
#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (colCntA != pSrcB->rowCnt) || (rowCntA != pDst->rowCnt)
        || (colCntB != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif


    _set_mx00(AM_16BIT | 1);
    _set_mx01(backstrideA|AM_16BIT);
    _set_mx10(colCntB|AM_16BIT);
    _set_mx11(backstrideB|AM_16BIT);
    _set_ay0((uint32_t)pDst->pData);
    _set_my00(AM_16BIT | 1);

    hmax = dspfx_mpy_i32(rowCntA , colCntA);

    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));

    for (h=0; h < hmax; h += colCntA)
    {
        _set_ax0(((uint32_t)pSrcA->pData) + h);
        _set_ax1((uint32_t)pSrcB->pData);
        for (k=0; k < colCntB; k++)
        {
            _mulrdw(0, 0);

            for (p = 0; p < (colCntA-1); p++)
            {
                _macrdw(_x0_u0(), _x1_u0());
            }
            _set_y0_u0( _macrdw(_x0_u1(), _x1_u1()));
        }
    }
    return DSP_ERR_OK;
}

// Produce instance with: MAC16x16; pIn0 - Y; 

#pragma Code("dsplib_dsp_mat_mult_q15_YYtoX_MAC16x16")

/**
 *  Q15 matrix multiplication.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status dsp_mat_mult_q15_YYtoX_MAC16x16(const matrix_q15_t *pSrcA, const matrix_q15_t *pSrcB, matrix_q15_t *pDst) 
{
    uint32_t h = 0, k = 0, p = 0;
    uint32_t hmax;
    uint32_t colCntA = pSrcA->colCnt, rowCntA = pSrcA->rowCnt, colCntB = pSrcB->colCnt;
    int32_t backstrideA = AM_OFFS(- (int32_t)(colCntA - 1));
    int32_t backstrideB = AM_OFFS(- (int32_t)(dspfx_mpy_i32(colCntA - 1, colCntB) - 1));
#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (colCntA != pSrcB->rowCnt) || (rowCntA != pDst->rowCnt)
        || (colCntB != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif


    _set_my00(AM_16BIT | 1);
    _set_my01(backstrideA|AM_16BIT);
    _set_my10(colCntB|AM_16BIT);
    _set_my11(backstrideB|AM_16BIT);
    _set_ax0((uint32_t)pDst->pData);
    _set_mx00(AM_16BIT | 1);

    hmax = dspfx_mpy_i32(rowCntA , colCntA);

    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));

    for (h=0; h < hmax; h += colCntA)
    {
        _set_ay0(((uint32_t)pSrcA->pData) + h);
        _set_ay1((uint32_t)pSrcB->pData);
        for (k=0; k < colCntB; k++)
        {
            _mulrdw(0, 0);

            for (p = 0; p < (colCntA-1); p++)
            {
                _macrdw(_y0_u0(), _y1_u0());
            }
            _set_x0_u0( _macrdw(_y0_u1(), _y1_u1()));
        }
    }
    return DSP_ERR_OK;
}


#endif
