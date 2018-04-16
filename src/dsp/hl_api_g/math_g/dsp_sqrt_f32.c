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

#if !defined(EXCLUDE_DSP_SQRT_F32)

#include "dsplib_primitives.h"
#include "dsplib.h"

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif

#pragma Code("dsplib_dsp_sqrt_f32")


/**
 * f32 square root function
 *
 * \param[in]  op
 * \result square root
 */

f32_t _DSPLIB_NOXYCC dsp_sqrt_f32(f32_t op)
{
#if defined(__Xfpus_div)
    op = dspfx_abs_f32(op);
    return _fssqrt(op);
#else
    f32_t root = 0.0;
    q31_t root_q,n;
    q31_t op_q31;
    q31_t e;
    q31_t m_q23;

    op=dspfx_abs_f32(op);

    if (op == 0.0) return root;

    op_q31=((*(q31_t*)(void*)(&op )));
    e = (op_q31 >> 23 ) & 0xFF;
    m_q23 =  e ? ( op_q31 & 0x7FFFFF ) | 0x800000 : ( op_q31 & 0x7FFFFF ) << 1;

    e -= 127;
    n=(e+2)& ~1;

    op_q31= m_q23 <<(8-n+e);//(31-23-n+e); //Q31 
    root_q = dsp_sqrt_q31(op_q31); //Q31 
    e=e>>1;//div 2
    //build float
    m_q23=((root_q >> (7)) & 0x7FFFFF) ;//Q23 
    e=((e+127) & 0xFF)<<23;
    op_q31=e|m_q23;
    root=((*(f32_t*)(void*)(&op_q31 )));

    return root;
#endif
}

#pragma Code("dsplib_dsp_sqrt_f32_ext")

/**
 * f32 square root function
 *
 * \param[in] op
 * \param pOut
 * \result dsp_status
 */

dsp_status _DSPLIB_NOXYCC dsp_sqrt_f32_ext(f32_t op, f32_t * pOut)
{
    if(op > 0)
    {
        *pOut = dsp_sqrt_f32(op);
        return DSP_ERR_OK;
    }
    else
    {
        *pOut = 0;
        return DSP_ERR_WRONG_ARGUMENT;
    }
}

#endif //!defined(EXCLUDE_DSP_SQRT_F32)
