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

#if !defined(EXCLUDE_DSP_BILINEAR_INTERP_Q7)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_bilinear_interp_q7")
/**
 * q7 BILINEAR INTERPOLATION PROCESSING FUNCTION
 *
 * \param pGrid - pointer to grid table that is stored in matrix structure
 * \param x - arguement for f(x,y) interpolation, must be in q12.20f format - 12 bits for table index and 20 bits for factional part
 * \param y - arguement for f(x,y) interpolation, must be in q12.20f format - 12 bits for table index and 20 bits for factional part
 * \result calculated y=f(x,y)
 */

q7_t dsp_bilinear_interp_q7(const matrix_q7_t * pGrid, int32_t x, int32_t y)
{
    q31_t acc1;
    q31_t xfract, yfract;
    q31_t x1, x2, y1, y2;
    uint32_t x_index, y_index;
    q7_t *pTableData = pGrid->pData;
    uint32_t nCols = pGrid->colCnt;
    uint32_t nRows = pGrid->rowCnt;
    int32_t cols, xdelta, ydelta;

    //get indexes from x,y values
    x_index = (uint32_t)x >> 20;
    y_index = (uint32_t)y >> 20;

    //get fractinal parts from x,y values, shift <<11 for q.31
    xfract = (x & 0x000FFFFF) << 11;
    yfract = (y & 0x000FFFFF) << 11;

    //check for table boundaries
    if ((x < 0) || (y < 0) || 
        ((x_index == (nRows - 1)) && (xfract !=0)) || (x_index >= (nRows)) ||
        ((y_index == (nCols - 1)) && (yfract !=0)) || (y_index >= (nCols)))
        return 0;

    cols = dspfx_mpy_i32(nCols, x_index) + y_index;

    // read four nearest output values

    xdelta = (x_index < (nRows - 1)) ? nCols : 0;
    ydelta = (y_index < (nCols - 1)) ? 1 : 0;

    x1 = pTableData[cols];
    x2 = pTableData[cols + xdelta];
    y1 = pTableData[cols + ydelta];
    y2 = pTableData[cols + xdelta + ydelta];

    //res = x1*(1-xfract)*(1-yfract) + x2*xfract*(1-yfract) + y1*(1-xfract)*yfract + y2*xfract*yfract;
    {
        dspfx_accum64()
        q31_t one = 0x7FFFFFFF;
        
        q31_t minus_yfract = (one - yfract);
        q31_t minus_xfract = (one - xfract);

        q31_t out2 =  dspfx_mpy_q31(x2, minus_yfract );
        q31_t out3 =  dspfx_mpy_q31(y1, minus_xfract );
        q31_t out4 =  dspfx_mpy_q31(y2, xfract);
        q31_t out  =  dspfx_mpy_q31(x1, minus_xfract );

        dspfx_a64mul_q31(out,minus_yfract,acc);
        dspfx_a64mac_q31(out2,xfract,acc);
        dspfx_a64mac_q31(out3,yfract,acc);
        dspfx_a64mac_q31(out4,yfract,acc);

        acc1 = dspfx_q31_cast_asl_a64(acc,24) >> 24;
    }

    return (acc1);
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_BILINEAR_INTERP_Q7)
