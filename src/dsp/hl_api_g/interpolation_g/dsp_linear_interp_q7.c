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

#if !defined(EXCLUDE_DSP_LINEAR_INTERP_Q7)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_linear_interp_q7")
/**
 * q7 LINEAR INTERPOLATION PROCESSING FUNCTION
 *
 * \param pYTable - pointer to q7 y=f(x) values 
 * \param x - arguement for f(x) interpolation, must be in q12.20f format - 12 bits for table index and 20 bits for factional part
 * \param tableSize - number of values in table
 * \result calculated y=f(x)
 */

q7_t dsp_linear_interp_q7(const q7_t * pYTable,  int32_t x,  uint32_t tableSize)
{
    q15_t y, y0, y1;                                
    q15_t fract;                                
    int32_t index;

    // get index value from input
    index = ((int32_t)(x & 0xFFF00000) >> 20);

    if(index >= (int32_t)(tableSize - 1))
       return (pYTable[tableSize - 1]);

    else if(x < 0)
      return (pYTable[0]);

    else
    {
      dspfx_accum32()
      INIT_MACMODE();
      //get fract value from input
      fract = (q15_t)((uint32_t)(((x & 0x000FFFFF)) >> 5));

      //nearest output values from pYTable
      y0 = pYTable[index];
      y1 = pYTable[index + 1];

      dspfx_a32mul_q15(y0, 0x7FFF - fract, acc);
      dspfx_a32mac_q15(y1, fract, acc);
      y = (q7_t)dspfx_q15_cast_a32(acc);

      return (y);
    }
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_LINEAR_INTERP_Q7)
