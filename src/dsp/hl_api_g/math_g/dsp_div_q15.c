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

#if !defined(EXCLUDE_DSP_DIV_Q15)

#include "dsplib_primitives.h"
#include "dsplib_helper.h"

#if !defined(__Xea)
const int16_t mul8_q15[] = {
    0x71c7,0x6666,0x5d17,0x5555,
    0x4ec4,0x4924,0x4444,0x4000,
} ;
#endif

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif

#pragma Code("dsplib_dsp_div_q15")

/**
 * Q15 division for numerator < denumerator
 *
 * \param num
 * \param den
 * \result q15_t result of division
 */

q15_t _DSPLIB_NOXYCC dsp_div_q15(q15_t num, q15_t den)
{
#if defined __Xea
    {
        q31_t var_out;
        int32_t i;
        q31_t Lm, L_denom;
        q31_t sign;

        Lm = (q31_t)num<<16;
        L_denom = (q31_t)den<<16;
        sign = (Lm ^ L_denom);

        Lm = dspfx_abs_q31(Lm);
        L_denom = dspfx_abs_q31(L_denom);
        Lm = dspfx_min_q31(Lm,L_denom);

        Lm = _divaw(Lm,L_denom);
        for(i=0;i<7;i++)
        {
            Lm = _divaw(Lm,L_denom);
            Lm = _divaw(Lm,L_denom);
        } // remainder in MSW of Lm quotient in SW of Lm
        var_out = (q15_t) Lm;

        if ((sign ^ var_out) < 0)
            var_out = dspfx_neg_q31(var_out);

        return(var_out);
    } 
#elif defined(_ARC) && defined(__Xdsp) && __Xdsp_version == 1
#include <arc/arc_intrinsics.h>
    {
        q31_t var_out;    
        q31_t Lm = _asls(num, 16);
        q31_t L_denom = _asls(den, 16);
        q31_t sign;
        sign = (Lm ^ L_denom); 
        Lm = _abss(Lm);
        _setacc(Lm, 0x001);               
        L_denom = _abss(L_denom);
        if (Lm >= L_denom) return sign < 0 ? 0x8000 : 0x7fff;
        // initialize low accumulator to numerator      
        // set high accumulator to 0 
        
        _divacc(L_denom);
        
        for (int i = 0; i < 7; i++)
        { 
            _divacc(L_denom);
            _divacc(L_denom);
        } 

        var_out = _getacc(0x00);
        if ((sign ^ var_out) < 0) var_out = _negs(var_out);
        return((q15_t)var_out);
    }
#elif defined(_ARC) && defined(__Xdsp) && (__Xdsp_version > 1) && (defined(__Xdsp_divsqrt_radix2) || defined(__Xdsp_divsqrt_radix4))
#include <arc/arc_intrinsics.h>
{
	return (q15_t)(_divf((q31_t)num << 16, (q31_t)den << 16) >> 16);
}
#else
    {
        q31_t m, e;
        const q31_t one = 0x7fffffff;
        q31_t sign = (num ^ den) << 16 ; 
        q31_t num31, den31;
        num31 = dspfx_abs_q31(num << 16);
        den31 = dspfx_abs_q31(den << 16);
        ASSERT(num31 <= den31) ;
        num31 = dspfx_min_q31(num31,den31);

        /* normalize the fraction by the denominator */
        e = dspfx_norm_q31(den31);

        den31 <<= e; 
        num31 <<= e; 

        m =  mul8_q15[(den31 >> (11+16)) & 0x7] << 16 ;

        den31 = dspfx_mpy_rnd_q31(den31, m);
        num31 = dspfx_mpy_rnd_q31(num31, m);


        m = one - den31; /* 1Q14 */
        den31<<=1;num31<<=1;

        den31 = dspfx_mpy_rnd_q31(den31, m );
        num31 = dspfx_mpy_rnd_q31(num31, m );


        m = one - den31; /* 1Q14 */
        den31<<=1;num31<<=1;

        den31 = dspfx_mpy_rnd_q31(den31, m );
        num31 = dspfx_mpy_rnd_q31(num31, m );


        m = one - den31;/* 1Q14 */
        den31<<=1;num31<<=1;

        den31 = dspfx_mpy_rnd_q31(den31, m );
        num31 = dspfx_mpy_rnd_q31(num31, m );

        m = one - den31;/* 1Q30 */
        num31<<=1;

        num31 = dspfx_mpy_rnd_q31(num31, m );

        num31<<=1;

        if ((sign ^ num31) < 0)
            num31 = dspfx_neg_q31(num31);

        return num31 >> 16;
    }
#endif
 }

#endif //!defined(EXCLUDE_DSP_DIV_Q15)
