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

#if !defined(EXCLUDE_DSP_LINEAR_INTERP_F32)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_linear_interp_f32")
/**
 * f32 LINEAR INTERPOLATION PROCESSING FUNCTION
 *
 * \param pYTable - pointer to f32 y=f(x) values 
 * \param x - argument for f(x_test) interpolation
 * \param tableSize - number of values in table
 * \result calculated y=f(x)
 */
 
f32_t dsp_linear_interp_f32(const f32_t * pYTable,  f32_t x,  uint32_t tableSize)
{
    f32_t y, y0, y1;                                
    f32_t fract;                                
    uint32_t index;

    // get index value from input
    index = (uint32_t) x;

    if(x < 0)
       return (pYTable[0]);
    else if(index >= (tableSize - 1))
       return (pYTable[tableSize - 1]);
    else
    {
      //get fract value from input
      fract = x - index;

      //nearest output values from pYTable
      y0 = pYTable[index];
      y1 = pYTable[index + 1];

      y = y0*(1-fract) + y1*fract;

      return y;
    }
}

f32_t dsp_linear_interp_f32_ext( dsp_linear_interp_instance_f32 *inst,  f32_t xx)
{
    f32_t * pTable = inst->pData;
    f32_t x,x0,x1, y0, y1;                                
    f32_t space = inst->spacing;                    
    int32_t index;
   
    x = inst->x1;
    index = (int32_t) ((xx - x) / space);

    if(index < 0)
       return (pTable[0]);
    else if(index >= (int32_t)inst->nValues)
      return pTable[inst->nValues - 1];
    else
    {
      x0 = x + index * space;
      x1 = x + (index + 1) * space;

      y0 = pTable[index];
      y1 = pTable[index + 1];

      return (y0 + (xx - x0) * ((y1 - y0) / (x1 - x0)));
    }
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_LINEAR_INTERP_F32)
