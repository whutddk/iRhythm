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

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac16)

// Produce instance with: MAC16x16; pInOut - X; 

#pragma Code("dsplib_dsp_lms_init_q15_cfXstY_MAC16x16")

/**
 * initialization for q15 real LMS filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - number of coefficients
 * \param offs_coeffs - XY memory offset to coefficients buffer
 * \param offs_states - XY memory offset to states vector buffer
 * \param mu - adaptation step size
 * \param postshift - bit shift for FIR output
 * \result dsp_status
 */

dsp_status dsp_lms_init_q15_cfXstY_MAC16x16(lms_q15_xy_t *inst,  uint16_t nTaps,  q15_t mu, uint32_t postshift,  uint32_t  offs_coeffs,  uint32_t offs_states)   
{
    inst->mu = mu;
    inst->numTaps = nTaps;
    inst->shift = 0;
    inst->postshift = postshift;
    inst->xyptr_states = offs_states;
    inst->xyptr_coeffs = offs_coeffs;
    _MEM_FILL_Y_Q15(inst->xyptr_states, 0, nTaps);
    _MEM_FILL_X_Q15(inst->xyptr_coeffs, 0, nTaps);

    return DSP_ERR_OK;
};
// Produce instance with: MAC16x16; pInOut - Y; 

#pragma Code("dsplib_dsp_lms_init_q15_cfYstX_MAC16x16")

/**
 * initialization for q15 real LMS filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - number of coefficients
 * \param offs_coeffs - XY memory offset to coefficients buffer
 * \param offs_states - XY memory offset to states vector buffer
 * \param mu - adaptation step size
 * \param postshift - bit shift for FIR output
 * \result dsp_status
 */

dsp_status dsp_lms_init_q15_cfYstX_MAC16x16(lms_q15_xy_t *inst,  uint16_t nTaps,  q15_t mu, uint32_t postshift,  uint32_t  offs_coeffs,  uint32_t offs_states)   
{
    inst->mu = mu;
    inst->numTaps = nTaps;
    inst->shift = 0;
    inst->postshift = postshift;
    inst->xyptr_states = offs_states;
    inst->xyptr_coeffs = offs_coeffs;
    _MEM_FILL_X_Q15(inst->xyptr_states, 0, nTaps);
    _MEM_FILL_Y_Q15(inst->xyptr_coeffs, 0, nTaps);

    return DSP_ERR_OK;
};

// Produce instance with: MAC16x16; pInOut - X; 

#pragma Code("dsplib_dsp_lms_q15_XY_cfYstX_MAC16x16")

/**
 * Q15 real LMS filtering
 *
 * \param inst - pointer to instance structure
 * \param[in,out] offs_inout - pointer to input/output vector. Vector length: nSamples
 * \param[in,out] offs_referr - pointer to reference/error vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_lms_q15_XY_cfYstX_MAC16x16(lms_q15_xy_t *inst,    uint32_t offs_inout,    uint32_t offs_referr,    uint32_t nSamples)  
{
    int32_t i, j;
    int16_t numTaps = inst->numTaps;
    q31_t mu = inst->mu<<16;
    uint32_t postshift = inst->postshift;


    _set_ax0(offs_inout);
    _set_mx00(1|AM_16BIT);
    _set_ax1(inst->xyptr_states + inst->shift);
    _set_mx10((AM_MOD(numTaps)|AM_OFFS(1)|AM_16BIT));
    _set_ay0(offs_referr);
    _set_my00(1|AM_16BIT);
    _set_ay1(inst->xyptr_coeffs);
    _set_my10((AM_MOD(numTaps)|AM_OFFS(1)|AM_16BIT));
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    for (i=0;i < (int32_t)nSamples; i++)
    {
        q31_t e, alpha, y0;
        
        // Store current input sample to delay line
        _set_x1_u0( _x0_nu());
        
        // FIR filter processing
        _mulrdw(_y1_u0(),_x1_u0());
        for (j = 0; j < numTaps-1; j++) {
            _macrdw(_y1_u0(),_x1_u0());
        }
        // Extract output sample
        y0 = dspfx_asl_q31((q31_t)(_get_ACC1()), postshift);
        
        // Store output sample
        _set_x0_u0( y0);
    
        // Calculate error between reference and output sample
        e  = dspfx_sub_q31(_y0_nu(), y0);
        
        // Calculate 'alpha' factor
        alpha = _mulrdw(e, mu);
        
        // Store error sample
        _set_y0_u0( e);
        
        // Update coefficients
        for (j = 0; j < numTaps; j++) {
            _mulrdw(alpha, _x1_u0());
            _set_y1_u0( dspfx_add_q31(_y1_nu(), _get_ACC1()));
        }
        
    }
    inst->shift = _ax1() - inst->xyptr_states;
}
// Produce instance with: MAC16x16; pInOut - Y; 

#pragma Code("dsplib_dsp_lms_q15_YX_cfXstY_MAC16x16")

/**
 * Q15 real LMS filtering
 *
 * \param inst - pointer to instance structure
 * \param[in,out] offs_inout - pointer to input/output vector. Vector length: nSamples
 * \param[in,out] offs_referr - pointer to reference/error vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_lms_q15_YX_cfXstY_MAC16x16(lms_q15_xy_t *inst,    uint32_t offs_inout,    uint32_t offs_referr,    uint32_t nSamples)  
{
    int32_t i, j;
    int16_t numTaps = inst->numTaps;
    q31_t mu = inst->mu<<16;
    uint32_t postshift = inst->postshift;


    _set_ay0(offs_inout);
    _set_my00(1|AM_16BIT);
    _set_ay1(inst->xyptr_states + inst->shift);
    _set_my10((AM_MOD(numTaps)|AM_OFFS(1)|AM_16BIT));
    _set_ax0(offs_referr);
    _set_mx00(1|AM_16BIT);
    _set_ax1(inst->xyptr_coeffs);
    _set_mx10((AM_MOD(numTaps)|AM_OFFS(1)|AM_16BIT));
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    for (i=0;i < (int32_t)nSamples; i++)
    {
        q31_t e, alpha, y0;
        
        // Store current input sample to delay line
        _set_y1_u0( _y0_nu());
        
        // FIR filter processing
        _mulrdw(_x1_u0(),_y1_u0());
        for (j = 0; j < numTaps-1; j++) {
            _macrdw(_x1_u0(),_y1_u0());
        }
        // Extract output sample
        y0 = dspfx_asl_q31((q31_t)(_get_ACC1()), postshift);
        
        // Store output sample
        _set_y0_u0( y0);
    
        // Calculate error between reference and output sample
        e  = dspfx_sub_q31(_x0_nu(), y0);
        
        // Calculate 'alpha' factor
        alpha = _mulrdw(e, mu);
        
        // Store error sample
        _set_x0_u0( e);
        
        // Update coefficients
        for (j = 0; j < numTaps; j++) {
            _mulrdw(alpha, _y1_u0());
            _set_x1_u0( dspfx_add_q31(_x1_nu(), _get_ACC1()));
        }
        
    }
    inst->shift = _ay1() - inst->xyptr_states;
}
#endif
