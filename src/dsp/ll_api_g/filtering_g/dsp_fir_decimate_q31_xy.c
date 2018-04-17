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

// Produce instance with: MAC32x32; pIn - X; pCoeffs - X; pOut - X; 

#pragma Code("dsplib_dsp_fir_decimate_q31_XtoX_cfXstY_MAC32x32")

/**
 *  Q31 real decimation
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed on output
 */

void dsp_fir_decimate_q31_XtoX_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t j, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    uint8_t factor; // Interpolation factor
    uint8_t tmp;
    
    
    
    T = inst->filter.numTaps;
    N = nSamples;
    factor = inst->factor;
    // Init input XY pointer
    _set_ax0(offs_input);
    _set_mx00(AM_OFFS(1));
    // Init output XY pointer
    _set_ax1(offs_output);
    _set_mx10(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax2(inst->filter.xyptr_coeffs);
    _set_mx20((AM_OFFS(1)));
    _set_mx21((AM_OFFS(1 - T)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ay0(inst->filter.shift + inst->filter.xyptr_states);
    if (T < 2)  {
        _set_my00(0);
        _set_my01(0);
    }else{
        _set_my00((AM_MOD(T)|AM_OFFS(1)));
        _set_my01((AM_MOD(T)|AM_OFFS(-1)));
    }
    
    // Store input sample into delay line
    _set_y0_nu( _x0_u0());
    tmp = factor - 1;
    for ( n = 0; n < N; n++ )
    {
        
        // Calculate convolution
        _mulhflw(0, 0);
        for (j = 0; j < T - 1; j++){
             _macflw(_y0_nu(), _x2_nu());
            _machflw(_y0_u0(), _x2_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_y0_nu(), _x2_nu());
        _set_x1_u0( _machflw(_y0_nu(), _x2_u1()));
        // Store input sample into delay line
        for (j = 0; j < tmp >> 1; j++){
            _set_y0_u1( _x0_u0());
            _set_y0_u1( _x0_u0());
        }
        if (tmp & 1) _set_y0_u1( _x0_u0());
        _set_y0_nu( _x0_u0());
    }
    // Save delay line offset
    inst->filter.shift = _ay0() - inst->filter.xyptr_states;
}
// Produce instance with: MAC32x32; pIn - Y; pCoeffs - X; pOut - X; 

#pragma Code("dsplib_dsp_fir_decimate_q31_YtoX_cfXstY_MAC32x32")

/**
 *  Q31 real decimation
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed on output
 */

void dsp_fir_decimate_q31_YtoX_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t j, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    uint8_t factor; // Interpolation factor
    uint8_t tmp;
    
    
    
    T = inst->filter.numTaps;
    N = nSamples;
    factor = inst->factor;
    // Init input XY pointer
    _set_ay0(offs_input);
    _set_my00(AM_OFFS(1));
    // Init output XY pointer
    _set_ax0(offs_output);
    _set_mx00(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax1(inst->filter.xyptr_coeffs);
    _set_mx10((AM_OFFS(1)));
    _set_mx11((AM_OFFS(1 - T)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ay1(inst->filter.shift + inst->filter.xyptr_states);
    if (T < 2)  {
        _set_my10(0);
        _set_my11(0);
    }else{
        _set_my10((AM_MOD(T)|AM_OFFS(1)));
        _set_my11((AM_MOD(T)|AM_OFFS(-1)));
    }
    
    // Store input sample into delay line
    _set_y1_nu( _y0_u0());
    tmp = factor - 1;
    for ( n = 0; n < N; n++ )
    {
        
        // Calculate convolution
        _mulhflw(0, 0);
        for (j = 0; j < T - 1; j++){
             _macflw(_y1_nu(), _x1_nu());
            _machflw(_y1_u0(), _x1_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_y1_nu(), _x1_nu());
        _set_x0_u0( _machflw(_y1_nu(), _x1_u1()));
        // Store input sample into delay line
        for (j = 0; j < tmp >> 1; j++){
            _set_y1_u1( _y0_u0());
            _set_y1_u1( _y0_u0());
        }
        if (tmp & 1) _set_y1_u1( _y0_u0());
        _set_y1_nu( _y0_u0());
    }
    // Save delay line offset
    inst->filter.shift = _ay1() - inst->filter.xyptr_states;
}
// Produce instance with: MAC32x32; pCoeffs - Y; pOut - X; pIn - X; 

#pragma Code("dsplib_dsp_fir_decimate_q31_XtoX_cfYstX_MAC32x32")

/**
 *  Q31 real decimation
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed on output
 */

void dsp_fir_decimate_q31_XtoX_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t j, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    uint8_t factor; // Interpolation factor
    uint8_t tmp;
    
    
    
    T = inst->filter.numTaps;
    N = nSamples;
    factor = inst->factor;
    // Init input XY pointer
    _set_ax0(offs_input);
    _set_mx00(AM_OFFS(1));
    // Init output XY pointer
    _set_ax1(offs_output);
    _set_mx10(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay0(inst->filter.xyptr_coeffs);
    _set_my00((AM_OFFS(1)));
    _set_my01((AM_OFFS(1 - T)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ax2(inst->filter.shift + inst->filter.xyptr_states);
    if (T < 2)  {
        _set_mx20(0);
        _set_mx21(0);
    }else{
        _set_mx20((AM_MOD(T)|AM_OFFS(1)));
        _set_mx21((AM_MOD(T)|AM_OFFS(-1)));
    }
    
    // Store input sample into delay line
    _set_x2_nu( _x0_u0());
    tmp = factor - 1;
    for ( n = 0; n < N; n++ )
    {
        
        // Calculate convolution
        _mulhflw(0, 0);
        for (j = 0; j < T - 1; j++){
             _macflw(_x2_nu(), _y0_nu());
            _machflw(_x2_u0(), _y0_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_x2_nu(), _y0_nu());
        _set_x1_u0( _machflw(_x2_nu(), _y0_u1()));
        // Store input sample into delay line
        for (j = 0; j < tmp >> 1; j++){
            _set_x2_u1( _x0_u0());
            _set_x2_u1( _x0_u0());
        }
        if (tmp & 1) _set_x2_u1( _x0_u0());
        _set_x2_nu( _x0_u0());
    }
    // Save delay line offset
    inst->filter.shift = _ax2() - inst->filter.xyptr_states;
}
// Produce instance with: MAC32x32; pOut - X; pCoeffs - Y; pIn - Y; 

#pragma Code("dsplib_dsp_fir_decimate_q31_YtoX_cfYstX_MAC32x32")

/**
 *  Q31 real decimation
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed on output
 */

void dsp_fir_decimate_q31_YtoX_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t j, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    uint8_t factor; // Interpolation factor
    uint8_t tmp;
    
    
    
    T = inst->filter.numTaps;
    N = nSamples;
    factor = inst->factor;
    // Init input XY pointer
    _set_ay0(offs_input);
    _set_my00(AM_OFFS(1));
    // Init output XY pointer
    _set_ax0(offs_output);
    _set_mx00(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay1(inst->filter.xyptr_coeffs);
    _set_my10((AM_OFFS(1)));
    _set_my11((AM_OFFS(1 - T)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ax1(inst->filter.shift + inst->filter.xyptr_states);
    if (T < 2)  {
        _set_mx10(0);
        _set_mx11(0);
    }else{
        _set_mx10((AM_MOD(T)|AM_OFFS(1)));
        _set_mx11((AM_MOD(T)|AM_OFFS(-1)));
    }
    
    // Store input sample into delay line
    _set_x1_nu( _y0_u0());
    tmp = factor - 1;
    for ( n = 0; n < N; n++ )
    {
        
        // Calculate convolution
        _mulhflw(0, 0);
        for (j = 0; j < T - 1; j++){
             _macflw(_x1_nu(), _y1_nu());
            _machflw(_x1_u0(), _y1_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_x1_nu(), _y1_nu());
        _set_x0_u0( _machflw(_x1_nu(), _y1_u1()));
        // Store input sample into delay line
        for (j = 0; j < tmp >> 1; j++){
            _set_x1_u1( _y0_u0());
            _set_x1_u1( _y0_u0());
        }
        if (tmp & 1) _set_x1_u1( _y0_u0());
        _set_x1_nu( _y0_u0());
    }
    // Save delay line offset
    inst->filter.shift = _ax1() - inst->filter.xyptr_states;
}
// Produce instance with: MAC32x32; pCoeffs - X; pOut - Y; pIn - X; 

#pragma Code("dsplib_dsp_fir_decimate_q31_XtoY_cfXstY_MAC32x32")

/**
 *  Q31 real decimation
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed on output
 */

void dsp_fir_decimate_q31_XtoY_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t j, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    uint8_t factor; // Interpolation factor
    uint8_t tmp;
    
    
    
    T = inst->filter.numTaps;
    N = nSamples;
    factor = inst->factor;
    // Init input XY pointer
    _set_ax0(offs_input);
    _set_mx00(AM_OFFS(1));
    // Init output XY pointer
    _set_ay0(offs_output);
    _set_my00(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax1(inst->filter.xyptr_coeffs);
    _set_mx10((AM_OFFS(1)));
    _set_mx11((AM_OFFS(1 - T)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ay1(inst->filter.shift + inst->filter.xyptr_states);
    if (T < 2)  {
        _set_my10(0);
        _set_my11(0);
    }else{
        _set_my10((AM_MOD(T)|AM_OFFS(1)));
        _set_my11((AM_MOD(T)|AM_OFFS(-1)));
    }
    
    // Store input sample into delay line
    _set_y1_nu( _x0_u0());
    tmp = factor - 1;
    for ( n = 0; n < N; n++ )
    {
        
        // Calculate convolution
        _mulhflw(0, 0);
        for (j = 0; j < T - 1; j++){
             _macflw(_y1_nu(), _x1_nu());
            _machflw(_y1_u0(), _x1_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_y1_nu(), _x1_nu());
        _set_y0_u0( _machflw(_y1_nu(), _x1_u1()));
        // Store input sample into delay line
        for (j = 0; j < tmp >> 1; j++){
            _set_y1_u1( _x0_u0());
            _set_y1_u1( _x0_u0());
        }
        if (tmp & 1) _set_y1_u1( _x0_u0());
        _set_y1_nu( _x0_u0());
    }
    // Save delay line offset
    inst->filter.shift = _ay1() - inst->filter.xyptr_states;
}
// Produce instance with: MAC32x32; pOut - Y; pCoeffs - X; pIn - Y; 

#pragma Code("dsplib_dsp_fir_decimate_q31_YtoY_cfXstY_MAC32x32")

/**
 *  Q31 real decimation
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed on output
 */

void dsp_fir_decimate_q31_YtoY_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t j, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    uint8_t factor; // Interpolation factor
    uint8_t tmp;
    
    
    
    T = inst->filter.numTaps;
    N = nSamples;
    factor = inst->factor;
    // Init input XY pointer
    _set_ay0(offs_input);
    _set_my00(AM_OFFS(1));
    // Init output XY pointer
    _set_ay1(offs_output);
    _set_my10(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax0(inst->filter.xyptr_coeffs);
    _set_mx00((AM_OFFS(1)));
    _set_mx01((AM_OFFS(1 - T)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ay2(inst->filter.shift + inst->filter.xyptr_states);
    if (T < 2)  {
        _set_my20(0);
        _set_my21(0);
    }else{
        _set_my20((AM_MOD(T)|AM_OFFS(1)));
        _set_my21((AM_MOD(T)|AM_OFFS(-1)));
    }
    
    // Store input sample into delay line
    _set_y2_nu( _y0_u0());
    tmp = factor - 1;
    for ( n = 0; n < N; n++ )
    {
        
        // Calculate convolution
        _mulhflw(0, 0);
        for (j = 0; j < T - 1; j++){
             _macflw(_y2_nu(), _x0_nu());
            _machflw(_y2_u0(), _x0_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_y2_nu(), _x0_nu());
        _set_y1_u0( _machflw(_y2_nu(), _x0_u1()));
        // Store input sample into delay line
        for (j = 0; j < tmp >> 1; j++){
            _set_y2_u1( _y0_u0());
            _set_y2_u1( _y0_u0());
        }
        if (tmp & 1) _set_y2_u1( _y0_u0());
        _set_y2_nu( _y0_u0());
    }
    // Save delay line offset
    inst->filter.shift = _ay2() - inst->filter.xyptr_states;
}
// Produce instance with: MAC32x32; pCoeffs - Y; pOut - Y; pIn - X; 

#pragma Code("dsplib_dsp_fir_decimate_q31_XtoY_cfYstX_MAC32x32")

/**
 *  Q31 real decimation
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed on output
 */

void dsp_fir_decimate_q31_XtoY_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t j, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    uint8_t factor; // Interpolation factor
    uint8_t tmp;
    
    
    
    T = inst->filter.numTaps;
    N = nSamples;
    factor = inst->factor;
    // Init input XY pointer
    _set_ax0(offs_input);
    _set_mx00(AM_OFFS(1));
    // Init output XY pointer
    _set_ay0(offs_output);
    _set_my00(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay1(inst->filter.xyptr_coeffs);
    _set_my10((AM_OFFS(1)));
    _set_my11((AM_OFFS(1 - T)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ax1(inst->filter.shift + inst->filter.xyptr_states);
    if (T < 2)  {
        _set_mx10(0);
        _set_mx11(0);
    }else{
        _set_mx10((AM_MOD(T)|AM_OFFS(1)));
        _set_mx11((AM_MOD(T)|AM_OFFS(-1)));
    }
    
    // Store input sample into delay line
    _set_x1_nu( _x0_u0());
    tmp = factor - 1;
    for ( n = 0; n < N; n++ )
    {
        
        // Calculate convolution
        _mulhflw(0, 0);
        for (j = 0; j < T - 1; j++){
             _macflw(_x1_nu(), _y1_nu());
            _machflw(_x1_u0(), _y1_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_x1_nu(), _y1_nu());
        _set_y0_u0( _machflw(_x1_nu(), _y1_u1()));
        // Store input sample into delay line
        for (j = 0; j < tmp >> 1; j++){
            _set_x1_u1( _x0_u0());
            _set_x1_u1( _x0_u0());
        }
        if (tmp & 1) _set_x1_u1( _x0_u0());
        _set_x1_nu( _x0_u0());
    }
    // Save delay line offset
    inst->filter.shift = _ax1() - inst->filter.xyptr_states;
}
// Produce instance with: MAC32x32; pCoeffs - Y; pOut - Y; pIn - Y; 

#pragma Code("dsplib_dsp_fir_decimate_q31_YtoY_cfYstX_MAC32x32")

/**
 *  Q31 real decimation
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed on output
 */

void dsp_fir_decimate_q31_YtoY_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t j, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    uint8_t factor; // Interpolation factor
    uint8_t tmp;
    
    
    
    T = inst->filter.numTaps;
    N = nSamples;
    factor = inst->factor;
    // Init input XY pointer
    _set_ay0(offs_input);
    _set_my00(AM_OFFS(1));
    // Init output XY pointer
    _set_ay1(offs_output);
    _set_my10(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay2(inst->filter.xyptr_coeffs);
    _set_my20((AM_OFFS(1)));
    _set_my21((AM_OFFS(1 - T)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ax0(inst->filter.shift + inst->filter.xyptr_states);
    if (T < 2)  {
        _set_mx00(0);
        _set_mx01(0);
    }else{
        _set_mx00((AM_MOD(T)|AM_OFFS(1)));
        _set_mx01((AM_MOD(T)|AM_OFFS(-1)));
    }
    
    // Store input sample into delay line
    _set_x0_nu( _y0_u0());
    tmp = factor - 1;
    for ( n = 0; n < N; n++ )
    {
        
        // Calculate convolution
        _mulhflw(0, 0);
        for (j = 0; j < T - 1; j++){
             _macflw(_x0_nu(), _y2_nu());
            _machflw(_x0_u0(), _y2_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_x0_nu(), _y2_nu());
        _set_y1_u0( _machflw(_x0_nu(), _y2_u1()));
        // Store input sample into delay line
        for (j = 0; j < tmp >> 1; j++){
            _set_x0_u1( _y0_u0());
            _set_x0_u1( _y0_u0());
        }
        if (tmp & 1) _set_x0_u1( _y0_u0());
        _set_x0_nu( _y0_u0());
    }
    // Save delay line offset
    inst->filter.shift = _ax0() - inst->filter.xyptr_states;
}

// Produce instance with: MAC32x32; pCoeffs - X; 

#pragma Code("dsplib_dsp_fir_decimate_init_q31_cfXstY_MAC32x32")

/**
 *  initialization for Q31 real decimation
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param decFactor - decimation factor
 * \param offs_coeffs - XY memory offset to coefficients buffer
 * \param offs_states - XY memory offset to states vector buffer
 * \result dsp_status
 */

dsp_status dsp_fir_decimate_init_q31_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint16_t nTaps, uint8_t decFactor,  uint32_t offs_coeffs, uint32_t offs_states)  
{
    inst->filter.numTaps = nTaps;
    inst->filter.shift = 0;
    inst->filter.xyptr_coeffs = offs_coeffs;
    inst->filter.xyptr_states = offs_states;
    inst->factor = decFactor;
    _BurstYMem(offs_states, 0, nTaps);
    return DSP_ERR_OK;
}
// Produce instance with: MAC32x32; pCoeffs - Y; 

#pragma Code("dsplib_dsp_fir_decimate_init_q31_cfYstX_MAC32x32")

/**
 *  initialization for Q31 real decimation
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param decFactor - decimation factor
 * \param offs_coeffs - XY memory offset to coefficients buffer
 * \param offs_states - XY memory offset to states vector buffer
 * \result dsp_status
 */

dsp_status dsp_fir_decimate_init_q31_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint16_t nTaps, uint8_t decFactor,  uint32_t offs_coeffs, uint32_t offs_states)  
{
    inst->filter.numTaps = nTaps;
    inst->filter.shift = 0;
    inst->filter.xyptr_coeffs = offs_coeffs;
    inst->filter.xyptr_states = offs_states;
    inst->factor = decFactor;
    _BurstXMem(offs_states, 0, nTaps);
    return DSP_ERR_OK;
}
#endif
