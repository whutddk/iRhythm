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
#if defined(_ARC600_XY)

// Produce instance with: MAC32x32; pIn0 - X; 

#pragma Code("dsplib_dsp_mat_trans_q31_XtoY")

/**
 *  Q31 matrix transpose.
 *
 * \param[in] pSrc - pointer to input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status dsp_mat_trans_q31_XtoY(const matrix_q31_t *pSrc, matrix_q31_t *pDst) 
{
    uint32_t c,r;
    uint32_t width = pSrc->colCnt;
    uint32_t height = pSrc->rowCnt;
    int32_t backstride = AM_OFFS(- (int32_t)(pSrc->size - 1));

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrc->rowCnt != pDst->colCnt) || (pSrc->colCnt != pDst->rowCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif


    _set_ax0((uint32_t)pSrc->pData);
    _set_mx00(width);
    _set_mx01(backstride);
    _set_ay0((uint32_t)pDst->pData);
    _set_my00(1);

    pDst->rowCnt = pSrc->colCnt;
    pDst->colCnt = pSrc->rowCnt;

    for (c = 0; c<width; c++)
    {
        for (r = 0; r<height; r++)
        {
            _set_y0_u0( _x0_u0() );
        }
        _x0_u1();
    }

    return DSP_ERR_OK;
}

// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_mat_trans_q31_YtoX")

/**
 *  Q31 matrix transpose.
 *
 * \param[in] pSrc - pointer to input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status dsp_mat_trans_q31_YtoX(const matrix_q31_t *pSrc, matrix_q31_t *pDst) 
{
    uint32_t c,r;
    uint32_t width = pSrc->colCnt;
    uint32_t height = pSrc->rowCnt;
    int32_t backstride = AM_OFFS(- (int32_t)(pSrc->size - 1));

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrc->rowCnt != pDst->colCnt) || (pSrc->colCnt != pDst->rowCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif


    _set_ay0((uint32_t)pSrc->pData);
    _set_my00(width);
    _set_my01(backstride);
    _set_ax0((uint32_t)pDst->pData);
    _set_mx00(1);

    pDst->rowCnt = pSrc->colCnt;
    pDst->colCnt = pSrc->rowCnt;

    for (c = 0; c<width; c++)
    {
        for (r = 0; r<height; r++)
        {
            _set_x0_u0( _y0_u0() );
        }
        _y0_u1();
    }

    return DSP_ERR_OK;
}


#endif
