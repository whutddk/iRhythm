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

#if !defined(EXCLUDE_DSP_MAT_INIT_Q31)

#include "dsplib_primitives.h" 
#pragma Code("dsplib_dsp_mat_init_q31")
/**
 * Q31 matrix initialization.
 *
 * \param matrix - pointer to matrix structure to be initialized
 * \param nRows - number of rows
 * \param nColumns - number of columns
 * \param pData - pointer to buffer for matrix data
 */

void _DSPLIB_XYCC dsp_mat_init_q31(matrix_q31_t *matrix, uint16_t nRows, uint16_t nColumns, PTR(q31_t) pData)
{
    matrix->rowCnt = nRows;
    matrix->colCnt = nColumns;
    matrix->size = dspfx_mpy_i32(nRows, nColumns);
    matrix->pData = pData;
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_MAT_INIT_Q31)
