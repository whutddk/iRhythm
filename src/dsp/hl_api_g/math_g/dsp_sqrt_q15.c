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

#if !defined(EXCLUDE_DSP_SQRT_Q15)

#include "dsplib_primitives.h"
#include "dsplib.h"

#if defined(__Xdsp) && __Xdsp_version > 1
#include <arc/arc_intrinsics.h>
#endif

#ifndef DSP_SQRT_PRESENT

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif

#pragma Code("dsplib_dsp_sqrt_q15")

/**
 * q15 square root function
 *
 * \param  op
 * \result square root
 */

#if !defined(DSP_MULTIPLIER_PRESENT)
q15_t _DSPLIB_NOXYCC dsp_sqrt_q15(q15_t op)
{
    uint32_t h,A,p,r,q,k,d;
    uint32_t x = ((uint32_t)dspfx_abs_q15(op)) << 16;

    if (op == 0) return 0;

    h = 0; 
    A = 0;
    p = r = 0; 
    q = k = (int32_t)1 << 31;

    while (h != 16) {
        d = p + q + r;  

        if (d <= x)
        { 
            A = A + k;
            p = d; 
            r = (r >> 1) + q;
        } 
        else
            r = (r >> 1);

        k = (k>>1); 
        q = q>>2;
        h++;
    }

    return (q15_t)(A>>16);
};

#else

#if defined _ARC && defined __Xdsp && __Xdsp_version == 1

#include "fxarc.h"
#include <arc/arc_intrinsics.h>

q15_t _DSPLIB_NOXYCC dsp_sqrt_q15(q15_t op)
{
    q31_t op31 = (int)op<<16; 
    op31 = _abss(op31);
    _setacc(op31, 0x1);  
    _sqrtacc(1);

    for (int i = 1; i < 16; i++) {
        _sqrtacc(0);
    }

    op31 = _getacc(0x00);    
    op31 = _mpyfr(op31, (unsigned)0xb504f334/2);

    return (q15_t)(op31);
}

#else

#if defined(__Xdsp) && (__Xdsp_version > 1) && (defined(__Xdsp_divsqrt_radix2) || defined(__Xdsp_divsqrt_radix4))

q15_t _DSPLIB_NOXYCC dsp_sqrt_q15(q15_t op)
{
    q31_t r;
    r = dspfx_abs_q31(op << 16);
    r = _sqrt(r);              // integer square root, reduces cycles
    r = _mpywhkul(r, 0xb505);  // multiply by sqrt(1/2)
    return r;
}


#else

static _Inline q31_t dsp_reciprt_q15(q31_t x)
{
    q31_t tmp;
    q31_t PLUSONE2Q14 = (1L<<14)<<16 ;
    q31_t iy,a;
    //x <<= 16;
       a = dspfx_mpyh_rnd_q31(0x39d9 << 16, x);
    a = dspfx_add_q31(a, 0x900e << 16);

    iy = dspfx_mpyh_rnd_q31(x, a);
    iy = dspfx_add_q31(iy, 0x573b << 16);

    iy = dspfx_asl_q31(iy,1);

    a = dspfx_mpyh_rnd_q31(x, iy);
    a = dspfx_mpyh_rnd_q31(a, iy);
    a = dspfx_sub_q31(PLUSONE2Q14, dspfx_asl_q31(a , 1));

    tmp = dspfx_mpyh_rnd_q31(a, iy);
    iy = dspfx_add_q31(tmp, iy);

    a = dspfx_mpyh_rnd_q31(x, iy);
    a = dspfx_mpyh_rnd_q31(a, iy);
    a = dspfx_sub_q31(PLUSONE2Q14, dspfx_asl_q31(a , 1));

    tmp = dspfx_mpyh_rnd_q31(a, iy);
    iy = dspfx_add_q31(tmp, iy);

    a = dspfx_mpyh_rnd_q31(x, iy);
    a = dspfx_mpyh_rnd_q31(a, iy);
    a = dspfx_sub_q31(PLUSONE2Q14, dspfx_asl_q31(a , 1));

    tmp = dspfx_mpyh_rnd_q31(a, iy);
    iy = dspfx_add_q31(tmp, iy);

    return (dspfx_add_q31(iy,0x8000));
}


q15_t _DSPLIB_NOXYCC dsp_sqrt_q15(q15_t op)
{
    q31_t e, r, op31 = op << 16;
    if (op31 == 0) return 0 ;
    op31 = dspfx_abs_q31(op31);

    e = dspfx_norm_q31(op31) & ~1 ;
    op31 <<= e ;
    e >>= 1 ;

    op31 = dspfx_mpyh_rnd_q31(op31, (q31_t)dsp_reciprt_q15(op31)) ;
    r = (e-1 >= 0) ? (op31 >> (e-1)) : dspfx_add_q31(op31, op31);
    return  r >> 16;
}


#endif // defined(__Xdsp) && (__Xdsp_version > 1) && (defined(__Xdsp_divsqrt_radix2) || defined(__Xdsp_divsqrt_radix4))

#endif // defined _ARC && defined __Xdsp && __Xdsp_version == 1

#endif // !defined(DSP_MULTIPLIER_PRESENT)

#endif // DSP_SQRT_PRESENT

#pragma Code("dsplib_dsp_sqrt_q15_ext")

/**
 * q15 square root function
 *
 * \param op
 * \param pOut
 * \result dsp_status
 */

dsp_status _DSPLIB_NOXYCC dsp_sqrt_q15_ext(q15_t op, q15_t * pOut)
{    
    if(op > 0)
    {
        *pOut = dsp_sqrt_q15(op);
        return DSP_ERR_OK;
    }
    else
    {
        *pOut = 0;
        return DSP_ERR_WRONG_ARGUMENT;
    }
}

#endif //!defined(EXCLUDE_DSP_SQRT_Q15)
