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

#if !defined(EXCLUDE_DSP_POW2X_Q31)

#include "dsplib_primitives.h"

/*
% Matlab(Octave) code for DSP Lib function
pkg load optim % required by "leasqr" function
format long;

% y = pow2(x)
% x = [-1 , 0]
% y = [0.5, 1]

%% model function:
scale_2 = 1;
% log2 taylor series
F = @ (x, p) scale_2 * ( p(1) + p(2)*x + p(3)*x.^2 + p(4)*x.^3 + p(5)*x.^4 + p(6)*x.^5 + p(7)*x.^6 + p(8)*x.^7 );
%% independents and dependents:
x = [-1:.00001:0];
y = pow2(x);
%% initial values:
init = [1.0 0.69314718055994530941723212145818 0.24022650695910071233355126316333 0.055504108664821579953142263768622 0.0096181291076284771619790715736589 0.0013333558146428443423412221987996 1.5403530393381609954437097332742e-4 1.525273380405984028002543901201e-5 ];
init = init / scale_2; % scale by 1/(ln(2)*2)
%% other configuration (default values):
tolerance = .000000001;
max_iterations = 20;
weights = ones (size(x));
dp = .000001*ones(size(init)); % bidirectional numeric gradient stepsize
dFdp = "dfdp"; % function for gradient (numerical)

%% start leasqr, be sure that 'verbose' is not set
global verbose; verbose = false;
[f, p, cvg, iter] = leasqr (x, y, init, F, tolerance, max_iterations, weights, dp, dFdp);

p % coeffs

err_lsq =  y - F(x,p);
error_min = min(err_lsq)
error_max = max(err_lsq)
*/

#pragma Data("dsplib_dsp_pow_data")

static const q31_t fac_a_tb[8] = {
    dsp_q31_cast_rnd_f32((double)  0.000000000000000 ),
    dsp_q31_cast_rnd_f32((double) -0.125000000000000 ),
    dsp_q31_cast_rnd_f32((double) -0.250000000000000 ),
    dsp_q31_cast_rnd_f32((double) -0.375000000000000 ),
    dsp_q31_cast_rnd_f32((double) -0.500000000000000 ),
    dsp_q31_cast_rnd_f32((double) -0.625000000000000 ),
    dsp_q31_cast_rnd_f32((double) -0.750000000000000 ),
    dsp_q31_cast_rnd_f32((double) -0.875000000000000 )
};

static const q31_t pow2_a_tb[8] = {
    dsp_q31_cast_rnd_f32((double) 1.000000000000000),
    dsp_q31_cast_rnd_f32((double) 0.917004043204671),
    dsp_q31_cast_rnd_f32((double) 0.840896415253715),
    dsp_q31_cast_rnd_f32((double) 0.771105412703970),
    dsp_q31_cast_rnd_f32((double) 0.707106781186548),
    dsp_q31_cast_rnd_f32((double) 0.648419777325505),
    dsp_q31_cast_rnd_f32((double) 0.594603557501361),
    dsp_q31_cast_rnd_f32((double) 0.545253866332629)
};

#pragma Data()

#define    coef_0  dsp_q31_cast_rnd_f32((double) 0.99999999984475385)
#define    coef_1  dsp_q31_cast_rnd_f32((double) 0.69314714324173410)
#define    coef_2  dsp_q31_cast_rnd_f32((double) 0.24022441217632323)
#define    coef_3  dsp_q31_cast_rnd_f32((double) 0.05545924302356971)
#define    coef_4  dsp_q31_cast_rnd_f32((double) 0.00921113779469484)

#pragma Code("dsplib_dsp_pow2x_q31")

#ifndef __CCAC__
#pragma on(AC_32_ONLY)
#endif

/**
 * Q31 base 2 power function
 *
 * \param[in] x - input argument within range [-1;0] in q31
 * \result result of power function - [1;0.5] in q31
 */

q31_t _DSPLIB_NOXYCC dsp_pow2x_q31(q31_t x)
{
    q31_t r, pow2_a, ind;
    
    if (x >= 0L) 
        return (0x7fffffffL); //2^0 = 1
    if (x == 0x80000000L) 
        return (0x40000000L);//2^(-1) = 0.5

    ind = -x >> 28;
    x = x - fac_a_tb[ind]; 
    pow2_a = pow2_a_tb[ind];

    r = coef_4;
    r = dspfx_mpy_rnd_q31(x, r);
    r += coef_3;
    r = dspfx_mpy_rnd_q31(x, r);
    r += coef_2;
    r = dspfx_mpy_rnd_q31(x, r);
    r += coef_1;
    r = dspfx_mpy_rnd_q31(x, r);
    r += coef_0;
    r = dspfx_mpy_rnd_q31(pow2_a, r);

    return r;
}

#endif //!defined(EXCLUDE_DSP_POW2X_Q31)
