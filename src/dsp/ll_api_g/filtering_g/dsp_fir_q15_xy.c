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
// Produce instance with: MAC16x16; pIn - X; pCoeffs - X; pOut - X; 

#pragma Code("dsplib_dsp_fir_q15_XtoX_cfXstY_MAC16x16")

/**
 *  Q15 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q15_XtoX_cfXstY_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ax0(offs_input);
    _set_mx00((AM_16BIT|AM_OFFS(1)));
    // Init output XY pointer
    _set_ax1(offs_output);
    _set_mx10((AM_16BIT|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax2(inst->xyptr_coeffs);
    _set_mx20((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ay0(inst->shift + inst->xyptr_states);
    _set_my00((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    T--;
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        _set_y0_nu( _x0_u0());
        // Calculate convolution
        _mulrdw(0, 0);
        for ( i = 0; i < (T >> 1) ; i++ )
        {
            _macrdw(_y0_u0(), _x2_u0());
            _macrdw(_y0_u0(), _x2_u0());
        }
        if (T & 1) _macrdw(_y0_u0(), _x2_u0());
        // Move convolution result to output buffer
        _set_x1_u0( _macrdw(_y0_nu(), _x2_u0()));
    }
    // Save delay line offset
    inst->shift = _ay0() - inst->xyptr_states;
}
// Produce instance with: MAC16x16; pIn - X; pCoeffs - Y; pOut - X; 

#pragma Code("dsplib_dsp_fir_q15_XtoX_cfYstX_MAC16x16")

/**
 *  Q15 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q15_XtoX_cfYstX_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ax0(offs_input);
    _set_mx00((AM_16BIT|AM_OFFS(1)));
    // Init output XY pointer
    _set_ax1(offs_output);
    _set_mx10((AM_16BIT|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay0(inst->xyptr_coeffs);
    _set_my00((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ax2(inst->shift + inst->xyptr_states);
    _set_mx20((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    T--;
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        _set_x2_nu( _x0_u0());
        // Calculate convolution
        _mulrdw(0, 0);
        for ( i = 0; i < (T >> 1) ; i++ )
        {
            _macrdw(_x2_u0(), _y0_u0());
            _macrdw(_x2_u0(), _y0_u0());
        }
        if (T & 1) _macrdw(_x2_u0(), _y0_u0());
        // Move convolution result to output buffer
        _set_x1_u0( _macrdw(_x2_nu(), _y0_u0()));
    }
    // Save delay line offset
    inst->shift = _ax2() - inst->xyptr_states;
}
// Produce instance with: MAC16x16; pOut - Y; pCoeffs - X; pIn - X; 

#pragma Code("dsplib_dsp_fir_q15_XtoY_cfXstY_MAC16x16")

/**
 *  Q15 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q15_XtoY_cfXstY_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ax0(offs_input);
    _set_mx00((AM_16BIT|AM_OFFS(1)));
    // Init output XY pointer
    _set_ay0(offs_output);
    _set_my00((AM_16BIT|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax1(inst->xyptr_coeffs);
    _set_mx10((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ay1(inst->shift + inst->xyptr_states);
    _set_my10((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    T--;
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        _set_y1_nu( _x0_u0());
        // Calculate convolution
        _mulrdw(0, 0);
        for ( i = 0; i < (T >> 1) ; i++ )
        {
            _macrdw(_y1_u0(), _x1_u0());
            _macrdw(_y1_u0(), _x1_u0());
        }
        if (T & 1) _macrdw(_y1_u0(), _x1_u0());
        // Move convolution result to output buffer
        _set_y0_u0( _macrdw(_y1_nu(), _x1_u0()));
    }
    // Save delay line offset
    inst->shift = _ay1() - inst->xyptr_states;
}
// Produce instance with: MAC16x16; pOut - Y; pCoeffs - Y; pIn - X; 

#pragma Code("dsplib_dsp_fir_q15_XtoY_cfYstX_MAC16x16")

/**
 *  Q15 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q15_XtoY_cfYstX_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ax0(offs_input);
    _set_mx00((AM_16BIT|AM_OFFS(1)));
    // Init output XY pointer
    _set_ay0(offs_output);
    _set_my00((AM_16BIT|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay1(inst->xyptr_coeffs);
    _set_my10((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ax1(inst->shift + inst->xyptr_states);
    _set_mx10((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    T--;
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        _set_x1_nu( _x0_u0());
        // Calculate convolution
        _mulrdw(0, 0);
        for ( i = 0; i < (T >> 1) ; i++ )
        {
            _macrdw(_x1_u0(), _y1_u0());
            _macrdw(_x1_u0(), _y1_u0());
        }
        if (T & 1) _macrdw(_x1_u0(), _y1_u0());
        // Move convolution result to output buffer
        _set_y0_u0( _macrdw(_x1_nu(), _y1_u0()));
    }
    // Save delay line offset
    inst->shift = _ax1() - inst->xyptr_states;
}
// Produce instance with: MAC16x16; pOut - X; pCoeffs - X; pIn - Y; 

#pragma Code("dsplib_dsp_fir_q15_YtoX_cfXstY_MAC16x16")

/**
 *  Q15 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q15_YtoX_cfXstY_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ay0(offs_input);
    _set_my00((AM_16BIT|AM_OFFS(1)));
    // Init output XY pointer
    _set_ax0(offs_output);
    _set_mx00((AM_16BIT|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax1(inst->xyptr_coeffs);
    _set_mx10((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ay1(inst->shift + inst->xyptr_states);
    _set_my10((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    T--;
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        _set_y1_nu( _y0_u0());
        // Calculate convolution
        _mulrdw(0, 0);
        for ( i = 0; i < (T >> 1) ; i++ )
        {
            _macrdw(_y1_u0(), _x1_u0());
            _macrdw(_y1_u0(), _x1_u0());
        }
        if (T & 1) _macrdw(_y1_u0(), _x1_u0());
        // Move convolution result to output buffer
        _set_x0_u0( _macrdw(_y1_nu(), _x1_u0()));
    }
    // Save delay line offset
    inst->shift = _ay1() - inst->xyptr_states;
}
// Produce instance with: MAC16x16; pCoeffs - X; pOut - Y; pIn - Y; 

#pragma Code("dsplib_dsp_fir_q15_YtoY_cfXstY_MAC16x16")

/**
 *  Q15 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q15_YtoY_cfXstY_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ay0(offs_input);
    _set_my00((AM_16BIT|AM_OFFS(1)));
    // Init output XY pointer
    _set_ay1(offs_output);
    _set_my10((AM_16BIT|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax0(inst->xyptr_coeffs);
    _set_mx00((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ay2(inst->shift + inst->xyptr_states);
    _set_my20((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    T--;
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        _set_y2_nu( _y0_u0());
        // Calculate convolution
        _mulrdw(0, 0);
        for ( i = 0; i < (T >> 1) ; i++ )
        {
            _macrdw(_y2_u0(), _x0_u0());
            _macrdw(_y2_u0(), _x0_u0());
        }
        if (T & 1) _macrdw(_y2_u0(), _x0_u0());
        // Move convolution result to output buffer
        _set_y1_u0( _macrdw(_y2_nu(), _x0_u0()));
    }
    // Save delay line offset
    inst->shift = _ay2() - inst->xyptr_states;
}
// Produce instance with: MAC16x16; pOut - X; pCoeffs - Y; pIn - Y; 

#pragma Code("dsplib_dsp_fir_q15_YtoX_cfYstX_MAC16x16")

/**
 *  Q15 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q15_YtoX_cfYstX_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ay0(offs_input);
    _set_my00((AM_16BIT|AM_OFFS(1)));
    // Init output XY pointer
    _set_ax0(offs_output);
    _set_mx00((AM_16BIT|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay1(inst->xyptr_coeffs);
    _set_my10((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ax1(inst->shift + inst->xyptr_states);
    _set_mx10((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    T--;
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        _set_x1_nu( _y0_u0());
        // Calculate convolution
        _mulrdw(0, 0);
        for ( i = 0; i < (T >> 1) ; i++ )
        {
            _macrdw(_x1_u0(), _y1_u0());
            _macrdw(_x1_u0(), _y1_u0());
        }
        if (T & 1) _macrdw(_x1_u0(), _y1_u0());
        // Move convolution result to output buffer
        _set_x0_u0( _macrdw(_x1_nu(), _y1_u0()));
    }
    // Save delay line offset
    inst->shift = _ax1() - inst->xyptr_states;
}
// Produce instance with: MAC16x16; pOut - Y; pCoeffs - Y; pIn - Y; 

#pragma Code("dsplib_dsp_fir_q15_YtoY_cfYstX_MAC16x16")

/**
 *  Q15 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q15_YtoY_cfYstX_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ay0(offs_input);
    _set_my00((AM_16BIT|AM_OFFS(1)));
    // Init output XY pointer
    _set_ay1(offs_output);
    _set_my10((AM_16BIT|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay2(inst->xyptr_coeffs);
    _set_my20((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ax0(inst->shift + inst->xyptr_states);
    _set_mx00((AM_16BIT|AM_MOD(T)|AM_OFFS(1)));
    T--;
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        _set_x0_nu( _y0_u0());
        // Calculate convolution
        _mulrdw(0, 0);
        for ( i = 0; i < (T >> 1) ; i++ )
        {
            _macrdw(_x0_u0(), _y2_u0());
            _macrdw(_x0_u0(), _y2_u0());
        }
        if (T & 1) _macrdw(_x0_u0(), _y2_u0());
        // Move convolution result to output buffer
        _set_y1_u0( _macrdw(_x0_nu(), _y2_u0()));
    }
    // Save delay line offset
    inst->shift = _ax0() - inst->xyptr_states;
}
/*================== REAL Q31 FIR INIT Function =============================*/

// Produce instance with: MAC16x16; pCoeffs - X; 

#pragma Code("dsplib_dsp_fir_init_q15_cfXstY_MAC16x16")

/**
 *  initialization for Q15 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param offs_coeffs - XY memory offset to coefficients buffer
 * \param offs_states - XY memory offset to states vector buffer
 * \result dsp_status
 */

dsp_status dsp_fir_init_q15_cfXstY_MAC16x16(fir_q15_xy_t *inst, uint16_t nTaps,  uint32_t offs_coeffs, uint32_t offs_states)  
{
    inst->numTaps = nTaps;
    inst->shift = 0;
    inst->xyptr_coeffs = offs_coeffs;
    inst->xyptr_states = offs_states;
    _MEM_FILL_Y_Q15(offs_states, 0, nTaps);
    return DSP_ERR_OK;
}
// Produce instance with: MAC16x16; pCoeffs - Y; 

#pragma Code("dsplib_dsp_fir_init_q15_cfYstX_MAC16x16")

/**
 *  initialization for Q15 real FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param offs_coeffs - XY memory offset to coefficients buffer
 * \param offs_states - XY memory offset to states vector buffer
 * \result dsp_status
 */

dsp_status dsp_fir_init_q15_cfYstX_MAC16x16(fir_q15_xy_t *inst, uint16_t nTaps,  uint32_t offs_coeffs, uint32_t offs_states)  
{
    inst->numTaps = nTaps;
    inst->shift = 0;
    inst->xyptr_coeffs = offs_coeffs;
    inst->xyptr_states = offs_states;
    _MEM_FILL_X_Q15(offs_states, 0, nTaps);
    return DSP_ERR_OK;
}

#endif
