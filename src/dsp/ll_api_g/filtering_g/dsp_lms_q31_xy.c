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
#if defined(_ARC_AS210_mac32)

// Produce instance with: MAC32x32; pInOut - X; 

#pragma Code("dsplib_dsp_lms_init_q31_cfXstY_MAC32x32")

/**
 * initialization for q31 real LMS filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - number of coefficients
 * \param offs_coeffs - X memory offset to coefficients buffer
 * \param offs_states - Y memory offset to states vector buffer
 * \param mu - adaptation step size
 * \param postshift - bit shift for FIR output
 * \result dsp_status
 */

dsp_status dsp_lms_init_q31_cfXstY_MAC32x32(lms_q31_xy_t *inst,  uint16_t nTaps,  q31_t mu, uint32_t postshift,  uint32_t  offs_coeffs,  uint32_t offs_states) 
{
    inst->mu = mu;
    inst->numTaps = nTaps;
    inst->shift = 0;
    inst->postshift = postshift;
    inst->xyptr_states = offs_states;
    inst->xyptr_coeffs = offs_coeffs;
    _MEM_FILL_Y_Q31(inst->xyptr_states, 0, nTaps);
    _MEM_FILL_X_Q31(inst->xyptr_coeffs, 0, nTaps);

    return DSP_ERR_OK;
};
// Produce instance with: MAC32x32; pInOut - Y; 

#pragma Code("dsplib_dsp_lms_init_q31_cfYstX_MAC32x32")

/**
 * initialization for q31 real LMS filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - number of coefficients
 * \param offs_coeffs - Y memory offset to coefficients buffer
 * \param offs_states - X memory offset to states vector buffer
 * \param mu - adaptation step size
 * \param postshift - bit shift for FIR output
 * \result dsp_status
 */

dsp_status dsp_lms_init_q31_cfYstX_MAC32x32(lms_q31_xy_t *inst,  uint16_t nTaps,  q31_t mu, uint32_t postshift,  uint32_t  offs_coeffs,  uint32_t offs_states) 
{
    inst->mu = mu;
    inst->numTaps = nTaps;
    inst->shift = 0;
    inst->postshift = postshift;
    inst->xyptr_states = offs_states;
    inst->xyptr_coeffs = offs_coeffs;
    _MEM_FILL_X_Q31(inst->xyptr_states, 0, nTaps);
    _MEM_FILL_Y_Q31(inst->xyptr_coeffs, 0, nTaps);

    return DSP_ERR_OK;
};


// Produce instance with: MAC32x32; pInOut - X; 

#pragma Code("dsplib_dsp_lms_q31_XY_cfYstX_MAC32x32")

/**
 * Q31 real LMS filtering
 *
 * \param inst - pointer to instance structure
 * \param[in,out] offs_inout - X memory offset to input/output vector. Vector length: nSamples
 * \param[in,out] offs_referr - Y memory offset to reference/error vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */
 
void dsp_lms_q31_XY_cfYstX_MAC32x32(lms_q31_xy_t *inst,    uint32_t offs_inout,    uint32_t offs_referr,    uint32_t nSamples)  
{
    int32_t i, j;
    int16_t numTaps = inst->numTaps;
    q31_t mu = inst->mu;
    uint32_t postshift = inst->postshift;


    _set_ax0(offs_inout);
    _set_mx00(1);
    _set_ax1(inst->xyptr_states + inst->shift);
    _set_mx10((AM_MOD(numTaps)|AM_OFFS(1)));
    _set_ay0(offs_referr);
    _set_my00(1);
    _set_ay1(inst->xyptr_coeffs);
    _set_my10((AM_MOD(numTaps)|AM_OFFS(1)));
    for (i = 0; i < (int32_t)nSamples; i++)
    {
        q31_t e, alpha, y0;
        
        // Store current input sample to delay line
        _set_x1_u0( _x0_nu());
        
        // FIR filter processing
         _mulflw(_y1_nu(),_x1_nu());
        _machflw(_y1_u0(),_x1_u0());
        for (j = 0; j < numTaps-1; j++) {
             _macflw(_y1_nu(),_x1_nu());
            _machflw(_y1_u0(),_x1_u0());
        }
        // Extract output sample
        y0 = dspfx_asl_q31((q31_t)(_get_ACC1()), postshift);
        
        // Store output sample
        _set_x0_u0( y0);
    
        // Calculate error between reference and output sample
        e  = dspfx_sub_q31(_y0_nu(), y0);
        
        // Calculate 'alpha' factor
                 _mulflw(e, mu);
        alpha = _machflw(e, mu);
        
        // Store error sample
        _set_y0_u0( e);
        
        // Update coefficients
        for (j = 0; j < numTaps; j++) {
             _mulflw(alpha, _x1_nu());
            _machflw(alpha, _x1_u0());
            _set_y1_u0( dspfx_add_q31(_y1_nu(), _get_ACC1()));
        }
        
    }
    inst->shift = _ax1() - inst->xyptr_states;
}
// Produce instance with: MAC32x32; pInOut - Y; 

#pragma Code("dsplib_dsp_lms_q31_YX_cfXstY_MAC32x32")

/**
 * Q31 real LMS filtering
 *
 * \param inst - pointer to instance structure
 * \param[in,out] offs_inout - Y memory offset to input/output vector. Vector length: nSamples
 * \param[in,out] offs_referr - X memory offset to reference/error vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */
 
void dsp_lms_q31_YX_cfXstY_MAC32x32(lms_q31_xy_t *inst,    uint32_t offs_inout,    uint32_t offs_referr,    uint32_t nSamples)  
{
    int32_t i, j;
    int16_t numTaps = inst->numTaps;
    q31_t mu = inst->mu;
    uint32_t postshift = inst->postshift;


    _set_ay0(offs_inout);
    _set_my00(1);
    _set_ay1(inst->xyptr_states + inst->shift);
    _set_my10((AM_MOD(numTaps)|AM_OFFS(1)));
    _set_ax0(offs_referr);
    _set_mx00(1);
    _set_ax1(inst->xyptr_coeffs);
    _set_mx10((AM_MOD(numTaps)|AM_OFFS(1)));
    for (i = 0; i < (int32_t)nSamples; i++)
    {
        q31_t e, alpha, y0;
        
        // Store current input sample to delay line
        _set_y1_u0( _y0_nu());
        
        // FIR filter processing
         _mulflw(_x1_nu(),_y1_nu());
        _machflw(_x1_u0(),_y1_u0());
        for (j = 0; j < numTaps-1; j++) {
             _macflw(_x1_nu(),_y1_nu());
            _machflw(_x1_u0(),_y1_u0());
        }
        // Extract output sample
        y0 = dspfx_asl_q31((q31_t)(_get_ACC1()), postshift);
        
        // Store output sample
        _set_y0_u0( y0);
    
        // Calculate error between reference and output sample
        e  = dspfx_sub_q31(_x0_nu(), y0);
        
        // Calculate 'alpha' factor
                 _mulflw(e, mu);
        alpha = _machflw(e, mu);
        
        // Store error sample
        _set_x0_u0( e);
        
        // Update coefficients
        for (j = 0; j < numTaps; j++) {
             _mulflw(alpha, _y1_nu());
            _machflw(alpha, _y1_u0());
            _set_x1_u0( dspfx_add_q31(_x1_nu(), _get_ACC1()));
        }
        
    }
    inst->shift = _ay1() - inst->xyptr_states;
}
#endif
