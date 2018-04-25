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

#if !defined(EXCLUDE_DSP_LOG2_Q31)

#include "dsplib_primitives.h"

/*
% Matlab(Octave) code for DSP Lib log2 function
pkg load optim % required by "leasqr" function
format long;

% y = log2(x)
% x = [0.5 , 1)
% y = [-1, 0]

% Lookup tables to reduce input range
% y = log2(x*a) - log2(a)
table_size_log2 = 3; % '0' - no lookup
bits = 0:1:(2^table_size_log2 - 1);
bits = transpose(bits)
maxes = 0.5 + bits * 2^(-1-table_size_log2) + (2^(-1-table_size_log2) )
mins = 0.5 + bits * 2^(-1-table_size_log2)

a_div2 = 0.5./maxes;
log2_a = log2(1./maxes);
new_mins = (mins .* a_div2) * 2;

a_div2, log2_a
new_input_range = [min(new_mins),1]

%% model function:
scale_2 = 2;
% log2 taylor series
F = @ (x, p) scale_2 * ( p(1)*(x-1) + p(2)*(x-1).^2 + p(3)*(x-1).^3 + p(4)*(x-1).^4 + p(5)*(x-1).^5 );
%% independents and dependents:
x = min(new_mins):.00001:1;
y = log2(x);
%% initial values:
init = [1.0 -1.0/2.0 1.0/3.0 -1.0/4.0 1.0/5.0]; % Taylor coeffs
init = init / 0.69314718055994530941723212145818 / scale_2; % scale by 1/(ln(2)*2)
%% other configuration (default values):
tolerance = .000000001;
max_iterations = 20;
weights = ones (size(x));
dp = .000001*ones(size(init)); % bidirectional numeric gradient stepsize
dFdp = "dfdp"; % function for gradient (numerical)

%% start leasqr, be sure that 'verbose' is not set
global verbose; verbose = false;
[f, p, cvg, iter] = ...
leasqr (x, y, init, F, tolerance, max_iterations, ...
weights, dp, dFdp);

p % coeffs

y_lsq = scale_2 * ( p(1)*(x-1) + p(2)*(x-1).^2 + p(3)*(x-1).^3 + p(4)*(x-1).^4 + p(5)*(x-1).^5 );

err_lsq =  y - y_lsq;
error_min = min(err_lsq)
error_max = max(err_lsq)
*/

#pragma Data("dsplib_dsp_log2_data")

static const q31_t fac_a_div2_tb[8] = {
    dsp_q31_cast_rnd_f32((double) 0.888888888888889 ),
    dsp_q31_cast_rnd_f32((double) 0.800000000000000 ),
    dsp_q31_cast_rnd_f32((double) 0.727272727272727 ),
    dsp_q31_cast_rnd_f32((double) 0.666666666666667 ),
    dsp_q31_cast_rnd_f32((double) 0.615384615384615 ),
    dsp_q31_cast_rnd_f32((double) 0.571428571428571 ),
    dsp_q31_cast_rnd_f32((double) 0.533333333333333 ),
    dsp_q31_cast_rnd_f32((double) 0.500000000000000 )
};

static const q31_t log2_a_tb[8] = {
    dsp_q31_cast_rnd_f32((double) 0.830074998557688 ),
    dsp_q31_cast_rnd_f32((double) 0.678071905112638 ),
    dsp_q31_cast_rnd_f32((double) 0.540568381362703 ),
    dsp_q31_cast_rnd_f32((double) 0.415037499278844 ),
    dsp_q31_cast_rnd_f32((double) 0.299560281858908 ),
    dsp_q31_cast_rnd_f32((double) 0.192645077942396 ),
    dsp_q31_cast_rnd_f32((double) 0.093109404391481 ),
    dsp_q31_cast_rnd_f32((double) 0.000000000000000 )
};

#define    coef_1  dsp_q31_cast_rnd_f32((double) 0.721347593759739 )
#define    coef_2  dsp_q31_cast_rnd_f32((double) -0.360665067997034 )
#define    coef_3  dsp_q31_cast_rnd_f32((double) 0.240794935777816 )
#define    coef_4  dsp_q31_cast_rnd_f32((double) -0.174295630014059 )
#define    coef_5  dsp_q31_cast_rnd_f32((double) 0.191233062030985 )

#define minus_one   0x80000000

#pragma Data()

/*
    input value - (0.5;1) in q31
    ouput value - (-1; 0) in q31
*/
#pragma Code("dsplib_dsp_log2_q31")

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif

/**
 * Q31 base 2 logarithmic function
 *
 * \param  x - (0.5;1) in q31
 * \result result of log2 function - (-1; 0) in q31
 */

q31_t _DSPLIB_NOXYCC dsp_log2_q31(q31_t x)
{
    q31_t r;
    if (x > 0x40000000) {
        int32_t tmp;
        {
            int32_t ind;
            q31_t fac_a_div2, log2_a;
            ind = (x >> 27) & 0x7;
            fac_a_div2 = fac_a_div2_tb[ind];
            x = dspfx_mpy_rnd_q31( x, fac_a_div2 ); // a/2*x
            log2_a = log2_a_tb[ind];
            tmp = -log2_a; // Log2(LN_facor_div2*2)
            x <<= 1; // a*x
        }
        x += minus_one;

        r = coef_5;
        r = dspfx_mpy_rnd_q31(x, r);
        r += coef_4;
        r = dspfx_mpy_rnd_q31(x, r);
        r += coef_3;
        r = dspfx_mpy_rnd_q31(x, r);
        r += coef_2;
        r = dspfx_mpy_rnd_q31(x, r);
        r += coef_1;
        r = dspfx_mpy_rnd_q31(x, r);

        r <<= 1;

        r = dspfx_add_q31(r,tmp);
    } else {
        r = 0x80000000;
    }
    return r;
}

#endif //!defined(EXCLUDE_DSP_LOG2_Q31)
