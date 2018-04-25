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

#if !defined(EXCLUDE_DSP_SQRT_Q31)

#include "dsplib_primitives.h"
#include "dsplib.h"

#if defined(__Xdsp) && (__Xdsp_version > 1)
#include <arc/arc_intrinsics.h>
#endif

_Inline q31_t _DSPLIB_NOXYCC dsp_reciprt_q31(q31_t x)
{
    q31_t tmp;
    q31_t PLUSONE2Q30 = (1L<<30) ;
    q31_t iy,a;

    a = dspfx_mpy_rnd_q31(x,  0x39d90000);
    a = dspfx_add_q31(a, 0x900ebee0);

    iy = dspfx_mpy_rnd_q31(x,  a);
    iy = dspfx_add_q31(iy, 0x573b645a);

    iy = dspfx_asl_q31(iy, 1);

    a = dspfx_mpy_rnd_q31(x, iy);
    a = dspfx_mpy_rnd_q31(a, iy);
    a = dspfx_sub_q31(PLUSONE2Q30, dspfx_asl_q31(a,1));

    tmp = dspfx_mpy_rnd_q31(a, iy);
    iy = dspfx_add_q31(tmp, iy);

    a = dspfx_mpy_rnd_q31(x, iy);
    a = dspfx_mpy_rnd_q31(a, iy);
    a = dspfx_sub_q31(PLUSONE2Q30, dspfx_asl_q31(a,1));

    tmp = dspfx_mpy_rnd_q31(a, iy);
    iy = dspfx_add_q31(tmp, iy);

    a = dspfx_mpy_rnd_q31(x, iy);
    a = dspfx_mpy_rnd_q31(a, iy);
    a = dspfx_sub_q31(PLUSONE2Q30, dspfx_asl_q31(a,1));

    tmp = dspfx_mpy_rnd_q31(a, iy);
    iy = dspfx_add_q31(tmp, iy);
    return iy; // 1/sqrt(x)
}

#ifndef DSP_SQRT_PRESENT

#pragma Code("dsplib_dsp_sqrt_q31")

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif

/**
 * q15 square root function
 *
 * \param[in]  op
 * \result square root
 */

#if !defined(DSP_MULTIPLIER_PRESENT)
#ifdef SQRT_BINSEARCH_HIGHPRECISION
q31_t _DSPLIB_NOXYCC dsp_sqrt_q31(q31_t op)
{
    uint64_t h,A,p,r,q,k,d;
    uint64_t x = (uint64_t)((int64_t)dspfx_abs_q31(op) << 32);

    if (op == 0) return 0;

    h = 0; 
    A = 0;
    p = r = 0; 
    q = k = (int64_t)1 << 63;

    while (h != 32) {
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

    return (A>>32);
};

#else
q31_t _DSPLIB_NOXYCC dsp_sqrt_q31(q31_t op)
{
    uint32_t h,A,p,r,q,k,d;
    int32_t e;
    uint32_t x = (uint32_t)dspfx_abs_q31(op);

    if (op == 0) return 0;

    e = dspfx_norm_q31(x) & ~1 ; 

    x<<=e;

    h = 0; 
    A = 0;
    p = r = 0; 
    q = k = (int32_t)1 << 31;

    while (h != 32) {
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
        if ((p+q+r)==d)break;
    }
    e>>=1;
    return  (A >> e) ; 
};
#endif

#else

#if defined(_ARC) && defined(__Xdsp) &&  __Xdsp_version == 1

#include <arc/arc_intrinsics.h>

static unsigned _Inline sqrt_q31_q31(unsigned v) 
{
    unsigned r;
    _setacc(v<<1, 0x1);
    // 15 msbs
    _sqrtacc(1);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    // 8 more bits
    _aslacc(0x0e030e);
    _sr(0x7, 0x583);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    // 4 more bits
    _aslacc(0x080308);
    _sr(0x3, 0x583);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    _sqrtacc(0);
    // 2 more bits
    _aslacc(0x040304);
    _sr(0x1, 0x583);
    _sqrtacc(0);
    _sqrtacc(0);
#if 0 // 1 more bit
    _aslacc(0x020302);
    _sr(0x0, 0x583);
    _sqrtacc(0);
    r = _getacc(0x02);
    return r>>1;
#else
    r = _getacc(0x02);
    return r;
#endif
}

q31_t _DSPLIB_NOXYCC dsp_sqrt_q31(q31_t op)
{
    if (op == 0) return 0;
    op = _abss(op);
    return sqrt_q31_q31(op);
}

#else

/*
    1/sqrt(op) for input range [0.25, 1).
    Newton method converges in 6-7 iterations

    Find 1/sqrt(op) for input range [0.25, 1). Newton method converges in 6-7! iterations


    Newton's method for 1/x
    Xn+1 = Xn * (2 - bXn)
*/
#if defined(__Xdsp) && (__Xdsp_version > 1) &&  (defined(__Xdsp_divsqrt_radix2) || defined(__Xdsp_divsqrt_radix4))
q31_t _DSPLIB_NOXYCC dsp_sqrt_q31(q31_t op)
{
    op = dspfx_abs_q31(op);
    return _sqrtf(op);
}
#else

q31_t _DSPLIB_NOXYCC dsp_sqrt_q31(q31_t op)
{
    q31_t e;
    if (op == 0) return 0;
    op = dspfx_abs_q31(op);
    // 1. Imagine that op is q.31 fractional.  Normalize it to range [0.25, 1)

    e = dspfx_norm_q31(op) & ~1 ;
    // 2. shifting left of op is equal to multiplication it to (2^e)

    op <<= e;
    // 3. Shifting of normalization constant finds sqrt(2^e)

    e >>= 1;
    // 4. Find 1/sqrt(op) for input range [0.25, 1). Newton method converges in 6-7! iterations for y_0=1.

    // 5. Find op*(1/sqrt(op))
    op = dspfx_mpy_rnd_q31(dsp_reciprt_q31(op), op) ;

    // 6. Apply shifted normalization constant that moves op result to actual range
    return (e-1 >= 0) ? (op >> (e-1)) : dspfx_add_q31(op, op) ;
}

#endif // defined(__Xdsp) && (__Xdsp_version > 1) &&  (defined(__Xdsp_divsqrt_radix2) || defined(__Xdsp_divsqrt_radix4))
#endif // defined(_ARC) && defined(__Xdsp) &&  __Xdsp_version == 1
#endif // !defined(DSP_MULTIPLIER_PRESENT)
#endif // DSP_SQRT_PRESENT

#pragma Code("dsplib_dsp_sqrt_q31_ext")

/**
 * q31 square root function
 *
 * \param[in] op
 * \param[out] pOut
 * \result dsp_status
 */

dsp_status _DSPLIB_NOXYCC dsp_sqrt_q31_ext(q31_t op, q31_t * pOut)
{
    if(op > 0)
    {
        *pOut = dsp_sqrt_q31(op);
        return DSP_ERR_OK;
    }
    else
    {
        *pOut = 0;
        return DSP_ERR_WRONG_ARGUMENT;
    }
}

#endif //!defined(EXCLUDE_DSP_SQRT_Q31)
