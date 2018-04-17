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

#if !defined(EXCLUDE_DSP_BILINEAR_INTERP_F32)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_bilinear_interp_f32")
/**
 * f32 BILINEAR INTERPOLATION PROCESSING FUNCTION
 *
 * \param pGrid - pointer to grid table that is stored in matrix structure
 * \param x - arguement for f(x,y) interpolation
 * \param y - arguement for f(x,y) interpolation
 * \result calculated y=f(x,y)
 */

f32_t dsp_bilinear_interp_f32(const matrix_f32_t * pGrid, f32_t x, f32_t y)
{
    f32_t out;
    f32_t *pData = pGrid->pData;
    int32_t colNum = pGrid->colCnt;
    int32_t x_idx, y_idx;
    f32_t x_fract, y_fract, f00, f01, f10, f11;
    int32_t cols, xdelta, ydelta;

    //get indexes from x,y values
    x_idx = (int32_t) x;
    y_idx = (int32_t) y;

    //check boundaries
    if(x < (f32_t)0.0f || x > (f32_t)(pGrid->rowCnt - 1) || y < (f32_t)0.0f  || y > (f32_t)(colNum - 1))
      return 0;

    cols = dspfx_mpy_i32(x_idx, colNum) + y_idx;

    //get fractional values from x,y
    x_fract = x - x_idx;
    y_fract = y - y_idx;

    xdelta = (x_idx < (pGrid->rowCnt - 1)) ? colNum : 0;
    ydelta = (y_idx < (colNum - 1)) ? 1 : 0;

    //get the set of nearest values
    f00 = pData[cols];
    f01 = pData[cols + xdelta];
    f10 = pData[cols + ydelta];
    f11 = pData[cols + xdelta+ydelta];


    //calculate interpolated value
    out = f00 + (f01 - f00)*x_fract + (f10 - f00)*y_fract + (f00 - f01 - f10 + f11)*x_fract*y_fract;

    return (out);
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_BILINEAR_INTERP_F32)
