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

#if !defined(EXCLUDE_DSP_LINEAR_INTERP_Q31)

#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_linear_interp_q31")
/**
 * q31 LINEAR INTERPOLATION PROCESSING FUNCTION
 *
 * \param pYTable - pointer to q31 y=f(x) values 
 * \param x - arguement for f(x) interpolation, must be in q12.20f format - 12 bits for table index and 20 bits for factional part
 * \param tableSize - number of values in table
 * \result calculated y=f(x)
 */

q31_t dsp_linear_interp_q31(const q31_t * pYTable,  int32_t x,  uint32_t tableSize)
{
    q31_t y, y0, y1;                                
    q31_t fract;                                
    int32_t index;

    // get index value from input
    index = ((int32_t)(x & 0xFFF00000) >> 20);

    if(index >= (int32_t)(tableSize - 1))
       return (pYTable[tableSize - 1]);

    else if(x < 0)
      return (pYTable[0]);

    else
    {
      dspfx_accum64()
      //get fract value from input
      fract = (x & 0x000FFFFF) << 11;

      //nearest output values from pYTable
      y0 = pYTable[index];
      y1 = pYTable[index + 1];

          dspfx_a64mul_q31(y0, 0x7FFFFFFF - fract, acc);
      y = dspfx_a64mac_q31(y1, fract, acc);

      // fix STAR: 9000676924 accuracy 0x7fffffff is not exact ONE 0x80000000
      //y = (fract == 0) ? y0 : dspfx_mpy_rnd_q31(y0, dspfx_add_q31(0x7FFFFFFF - fract, 1));
      //y = dspfx_mpy_q31(y0,(0x7FFFFFFF - fract));
      //y = dspfx_add_q31(y, dspfx_mpy_rnd_q31(y1,fract));

      return y;
    }
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_LINEAR_INTERP_Q31)
