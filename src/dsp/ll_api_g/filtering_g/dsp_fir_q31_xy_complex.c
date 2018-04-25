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

#pragma Code("dsplib_dsp_fir_q31_complex_XtoX_cfXstY_MAC32x32")

/**
 *  Q31 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q31_complex_XtoX_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ax0(offs_input);
    _set_mx00(AM_OFFS(1));
    // Init output XY pointer
    _set_ax1(offs_output);
    _set_mx10(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax2(inst->xyptr_coeffs);
    _set_mx20((AM_MOD(T<<1)|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax3((inst->xyptr_coeffs+1));
    _set_mx30((AM_MOD(T<<1)|AM_OFFS(-1)));
    _set_mx31((AM_MOD(T<<1)|AM_OFFS(3)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ay0(inst->shift + inst->xyptr_states);
    _set_my00((AM_MOD(T<<1)|AM_OFFS(1)));
    _set_my01((AM_MOD(T<<1)|AM_OFFS(-1)));
    
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        // Store real part
        _set_y0_u0( _x0_u0());
        // Store imaginary part
        _set_y0_u1( _x0_u0());
        
        // Calculate REAL part of the convolution
         _mulflw(_y0_nu(), _x2_nu());
        _machflw(_y0_u0(), _x2_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_y0_nu(), -_x2_nu());
            _machflw(_y0_u0(), -_x2_u0());
             _macflw(_y0_nu(), _x2_nu());
            _machflw(_y0_u0(), _x2_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_y0_nu(), -_x2_nu());
        _set_x1_u0( _machflw(_y0_u0(), -_x2_u0()));
        
        // Calculate IMAGINARY part of the convolution
         _mulflw(_y0_nu(), _x3_nu());
        _machflw(_y0_u0(), _x3_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_y0_nu(), _x3_nu());
            _machflw(_y0_u0(), _x3_u1());
             _macflw(_y0_nu(), _x3_nu());
            _machflw(_y0_u0(), _x3_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_y0_nu(), _x3_nu());
        _set_x1_u0( _machflw(_y0_u1(), _x3_u1()));
    }
    // Save delay line offset
    inst->shift = _ay0() - inst->xyptr_states;
}
// Produce instance with: MAC32x32; pIn - X; pOut - Y; pCoeffs - X; 

#pragma Code("dsplib_dsp_fir_q31_complex_XtoY_cfXstY_MAC32x32")

/**
 *  Q31 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q31_complex_XtoY_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ax0(offs_input);
    _set_mx00(AM_OFFS(1));
    // Init output XY pointer
    _set_ay0(offs_output);
    _set_my00(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax1(inst->xyptr_coeffs);
    _set_mx10((AM_MOD(T<<1)|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax2((inst->xyptr_coeffs+1));
    _set_mx20((AM_MOD(T<<1)|AM_OFFS(-1)));
    _set_mx21((AM_MOD(T<<1)|AM_OFFS(3)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ay1(inst->shift + inst->xyptr_states);
    _set_my10((AM_MOD(T<<1)|AM_OFFS(1)));
    _set_my11((AM_MOD(T<<1)|AM_OFFS(-1)));
    
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        // Store real part
        _set_y1_u0( _x0_u0());
        // Store imaginary part
        _set_y1_u1( _x0_u0());
        
        // Calculate REAL part of the convolution
         _mulflw(_y1_nu(), _x1_nu());
        _machflw(_y1_u0(), _x1_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_y1_nu(), -_x1_nu());
            _machflw(_y1_u0(), -_x1_u0());
             _macflw(_y1_nu(), _x1_nu());
            _machflw(_y1_u0(), _x1_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_y1_nu(), -_x1_nu());
        _set_y0_u0( _machflw(_y1_u0(), -_x1_u0()));
        
        // Calculate IMAGINARY part of the convolution
         _mulflw(_y1_nu(), _x2_nu());
        _machflw(_y1_u0(), _x2_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_y1_nu(), _x2_nu());
            _machflw(_y1_u0(), _x2_u1());
             _macflw(_y1_nu(), _x2_nu());
            _machflw(_y1_u0(), _x2_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_y1_nu(), _x2_nu());
        _set_y0_u0( _machflw(_y1_u1(), _x2_u1()));
    }
    // Save delay line offset
    inst->shift = _ay1() - inst->xyptr_states;
}
// Produce instance with: MAC32x32; pIn - X; pOut - X; pCoeffs - Y; 

#pragma Code("dsplib_dsp_fir_q31_complex_XtoX_cfYstX_MAC32x32")

/**
 *  Q31 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q31_complex_XtoX_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ax0(offs_input);
    _set_mx00(AM_OFFS(1));
    // Init output XY pointer
    _set_ax1(offs_output);
    _set_mx10(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay0(inst->xyptr_coeffs);
    _set_my00((AM_MOD(T<<1)|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay1((inst->xyptr_coeffs+1));
    _set_my10((AM_MOD(T<<1)|AM_OFFS(-1)));
    _set_my11((AM_MOD(T<<1)|AM_OFFS(3)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ax2(inst->shift + inst->xyptr_states);
    _set_mx20((AM_MOD(T<<1)|AM_OFFS(1)));
    _set_mx21((AM_MOD(T<<1)|AM_OFFS(-1)));
    
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        // Store real part
        _set_x2_u0( _x0_u0());
        // Store imaginary part
        _set_x2_u1( _x0_u0());
        
        // Calculate REAL part of the convolution
         _mulflw(_x2_nu(), _y0_nu());
        _machflw(_x2_u0(), _y0_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_x2_nu(), -_y0_nu());
            _machflw(_x2_u0(), -_y0_u0());
             _macflw(_x2_nu(), _y0_nu());
            _machflw(_x2_u0(), _y0_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_x2_nu(), -_y0_nu());
        _set_x1_u0( _machflw(_x2_u0(), -_y0_u0()));
        
        // Calculate IMAGINARY part of the convolution
         _mulflw(_x2_nu(), _y1_nu());
        _machflw(_x2_u0(), _y1_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_x2_nu(), _y1_nu());
            _machflw(_x2_u0(), _y1_u1());
             _macflw(_x2_nu(), _y1_nu());
            _machflw(_x2_u0(), _y1_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_x2_nu(), _y1_nu());
        _set_x1_u0( _machflw(_x2_u1(), _y1_u1()));
    }
    // Save delay line offset
    inst->shift = _ax2() - inst->xyptr_states;
}
// Produce instance with: MAC32x32; pCoeffs - Y; pOut - Y; pIn - X; 

#pragma Code("dsplib_dsp_fir_q31_complex_XtoY_cfYstX_MAC32x32")

/**
 *  Q31 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q31_complex_XtoY_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ax0(offs_input);
    _set_mx00(AM_OFFS(1));
    // Init output XY pointer
    _set_ay0(offs_output);
    _set_my00(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay1(inst->xyptr_coeffs);
    _set_my10((AM_MOD(T<<1)|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay2((inst->xyptr_coeffs+1));
    _set_my20((AM_MOD(T<<1)|AM_OFFS(-1)));
    _set_my21((AM_MOD(T<<1)|AM_OFFS(3)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ax1(inst->shift + inst->xyptr_states);
    _set_mx10((AM_MOD(T<<1)|AM_OFFS(1)));
    _set_mx11((AM_MOD(T<<1)|AM_OFFS(-1)));
    
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        // Store real part
        _set_x1_u0( _x0_u0());
        // Store imaginary part
        _set_x1_u1( _x0_u0());
        
        // Calculate REAL part of the convolution
         _mulflw(_x1_nu(), _y1_nu());
        _machflw(_x1_u0(), _y1_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_x1_nu(), -_y1_nu());
            _machflw(_x1_u0(), -_y1_u0());
             _macflw(_x1_nu(), _y1_nu());
            _machflw(_x1_u0(), _y1_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_x1_nu(), -_y1_nu());
        _set_y0_u0( _machflw(_x1_u0(), -_y1_u0()));
        
        // Calculate IMAGINARY part of the convolution
         _mulflw(_x1_nu(), _y2_nu());
        _machflw(_x1_u0(), _y2_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_x1_nu(), _y2_nu());
            _machflw(_x1_u0(), _y2_u1());
             _macflw(_x1_nu(), _y2_nu());
            _machflw(_x1_u0(), _y2_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_x1_nu(), _y2_nu());
        _set_y0_u0( _machflw(_x1_u1(), _y2_u1()));
    }
    // Save delay line offset
    inst->shift = _ax1() - inst->xyptr_states;
}
// Produce instance with: MAC32x32; pOut - X; pCoeffs - X; pIn - Y; 

#pragma Code("dsplib_dsp_fir_q31_complex_YtoX_cfXstY_MAC32x32")

/**
 *  Q31 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q31_complex_YtoX_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ay0(offs_input);
    _set_my00(AM_OFFS(1));
    // Init output XY pointer
    _set_ax0(offs_output);
    _set_mx00(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax1(inst->xyptr_coeffs);
    _set_mx10((AM_MOD(T<<1)|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax2((inst->xyptr_coeffs+1));
    _set_mx20((AM_MOD(T<<1)|AM_OFFS(-1)));
    _set_mx21((AM_MOD(T<<1)|AM_OFFS(3)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ay1(inst->shift + inst->xyptr_states);
    _set_my10((AM_MOD(T<<1)|AM_OFFS(1)));
    _set_my11((AM_MOD(T<<1)|AM_OFFS(-1)));
    
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        // Store real part
        _set_y1_u0( _y0_u0());
        // Store imaginary part
        _set_y1_u1( _y0_u0());
        
        // Calculate REAL part of the convolution
         _mulflw(_y1_nu(), _x1_nu());
        _machflw(_y1_u0(), _x1_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_y1_nu(), -_x1_nu());
            _machflw(_y1_u0(), -_x1_u0());
             _macflw(_y1_nu(), _x1_nu());
            _machflw(_y1_u0(), _x1_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_y1_nu(), -_x1_nu());
        _set_x0_u0( _machflw(_y1_u0(), -_x1_u0()));
        
        // Calculate IMAGINARY part of the convolution
         _mulflw(_y1_nu(), _x2_nu());
        _machflw(_y1_u0(), _x2_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_y1_nu(), _x2_nu());
            _machflw(_y1_u0(), _x2_u1());
             _macflw(_y1_nu(), _x2_nu());
            _machflw(_y1_u0(), _x2_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_y1_nu(), _x2_nu());
        _set_x0_u0( _machflw(_y1_u1(), _x2_u1()));
    }
    // Save delay line offset
    inst->shift = _ay1() - inst->xyptr_states;
}
// Produce instance with: MAC32x32; pIn - Y; pOut - Y; pCoeffs - X; 

#pragma Code("dsplib_dsp_fir_q31_complex_YtoY_cfXstY_MAC32x32")

/**
 *  Q31 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q31_complex_YtoY_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ay0(offs_input);
    _set_my00(AM_OFFS(1));
    // Init output XY pointer
    _set_ay1(offs_output);
    _set_my10(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax0(inst->xyptr_coeffs);
    _set_mx00((AM_MOD(T<<1)|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ax1((inst->xyptr_coeffs+1));
    _set_mx10((AM_MOD(T<<1)|AM_OFFS(-1)));
    _set_mx11((AM_MOD(T<<1)|AM_OFFS(3)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ay2(inst->shift + inst->xyptr_states);
    _set_my20((AM_MOD(T<<1)|AM_OFFS(1)));
    _set_my21((AM_MOD(T<<1)|AM_OFFS(-1)));
    
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        // Store real part
        _set_y2_u0( _y0_u0());
        // Store imaginary part
        _set_y2_u1( _y0_u0());
        
        // Calculate REAL part of the convolution
         _mulflw(_y2_nu(), _x0_nu());
        _machflw(_y2_u0(), _x0_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_y2_nu(), -_x0_nu());
            _machflw(_y2_u0(), -_x0_u0());
             _macflw(_y2_nu(), _x0_nu());
            _machflw(_y2_u0(), _x0_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_y2_nu(), -_x0_nu());
        _set_y1_u0( _machflw(_y2_u0(), -_x0_u0()));
        
        // Calculate IMAGINARY part of the convolution
         _mulflw(_y2_nu(), _x1_nu());
        _machflw(_y2_u0(), _x1_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_y2_nu(), _x1_nu());
            _machflw(_y2_u0(), _x1_u1());
             _macflw(_y2_nu(), _x1_nu());
            _machflw(_y2_u0(), _x1_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_y2_nu(), _x1_nu());
        _set_y1_u0( _machflw(_y2_u1(), _x1_u1()));
    }
    // Save delay line offset
    inst->shift = _ay2() - inst->xyptr_states;
}
// Produce instance with: MAC32x32; pOut - X; pCoeffs - Y; pIn - Y; 

#pragma Code("dsplib_dsp_fir_q31_complex_YtoX_cfYstX_MAC32x32")

/**
 *  Q31 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q31_complex_YtoX_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ay0(offs_input);
    _set_my00(AM_OFFS(1));
    // Init output XY pointer
    _set_ax0(offs_output);
    _set_mx00(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay1(inst->xyptr_coeffs);
    _set_my10((AM_MOD(T<<1)|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay2((inst->xyptr_coeffs+1));
    _set_my20((AM_MOD(T<<1)|AM_OFFS(-1)));
    _set_my21((AM_MOD(T<<1)|AM_OFFS(3)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ax1(inst->shift + inst->xyptr_states);
    _set_mx10((AM_MOD(T<<1)|AM_OFFS(1)));
    _set_mx11((AM_MOD(T<<1)|AM_OFFS(-1)));
    
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        // Store real part
        _set_x1_u0( _y0_u0());
        // Store imaginary part
        _set_x1_u1( _y0_u0());
        
        // Calculate REAL part of the convolution
         _mulflw(_x1_nu(), _y1_nu());
        _machflw(_x1_u0(), _y1_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_x1_nu(), -_y1_nu());
            _machflw(_x1_u0(), -_y1_u0());
             _macflw(_x1_nu(), _y1_nu());
            _machflw(_x1_u0(), _y1_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_x1_nu(), -_y1_nu());
        _set_x0_u0( _machflw(_x1_u0(), -_y1_u0()));
        
        // Calculate IMAGINARY part of the convolution
         _mulflw(_x1_nu(), _y2_nu());
        _machflw(_x1_u0(), _y2_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_x1_nu(), _y2_nu());
            _machflw(_x1_u0(), _y2_u1());
             _macflw(_x1_nu(), _y2_nu());
            _machflw(_x1_u0(), _y2_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_x1_nu(), _y2_nu());
        _set_x0_u0( _machflw(_x1_u1(), _y2_u1()));
    }
    // Save delay line offset
    inst->shift = _ax1() - inst->xyptr_states;
}
// Produce instance with: MAC32x32; pIn - Y; pCoeffs - Y; pOut - Y; 

#pragma Code("dsplib_dsp_fir_q31_complex_YtoY_cfYstX_MAC32x32")

/**
 *  Q31 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param nSamples - number of samples to be processed
 */

void dsp_fir_q31_complex_YtoY_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples)  
{
    int32_t i, n; // loop counters
    int32_t N; // Number of samples to process
    int32_t    T; // Filter order
    
    
    
    T = inst->numTaps;
    N = nSamples;
    // Init input XY pointer
    _set_ay0(offs_input);
    _set_my00(AM_OFFS(1));
    // Init output XY pointer
    _set_ay1(offs_output);
    _set_my10(AM_OFFS(1));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay2(inst->xyptr_coeffs);
    _set_my20((AM_MOD(T<<1)|AM_OFFS(1)));
    // Init XY pointer for filter coefficients (modulo T addressing)
    _set_ay3((inst->xyptr_coeffs+1));
    _set_my30((AM_MOD(T<<1)|AM_OFFS(-1)));
    _set_my31((AM_MOD(T<<1)|AM_OFFS(3)));
    // Init XY pointer for filter delay line (modulo T addressing)
    _set_ax0(inst->shift + inst->xyptr_states);
    _set_mx00((AM_MOD(T<<1)|AM_OFFS(1)));
    _set_mx01((AM_MOD(T<<1)|AM_OFFS(-1)));
    
    for ( n = 0; n < N; n++ )
    {
        // Store input sample into delay line
        // Store real part
        _set_x0_u0( _y0_u0());
        // Store imaginary part
        _set_x0_u1( _y0_u0());
        
        // Calculate REAL part of the convolution
         _mulflw(_x0_nu(), _y2_nu());
        _machflw(_x0_u0(), _y2_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_x0_nu(), -_y2_nu());
            _machflw(_x0_u0(), -_y2_u0());
             _macflw(_x0_nu(), _y2_nu());
            _machflw(_x0_u0(), _y2_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_x0_nu(), -_y2_nu());
        _set_y1_u0( _machflw(_x0_u0(), -_y2_u0()));
        
        // Calculate IMAGINARY part of the convolution
         _mulflw(_x0_nu(), _y3_nu());
        _machflw(_x0_u0(), _y3_u0());
        for ( i = 0; i < T - 1 ; i++ )
        {
             _macflw(_x0_nu(), _y3_nu());
            _machflw(_x0_u0(), _y3_u1());
             _macflw(_x0_nu(), _y3_nu());
            _machflw(_x0_u0(), _y3_u0());
        }
        // Move convolution result to output buffer
                    _macflw(_x0_nu(), _y3_nu());
        _set_y1_u0( _machflw(_x0_u1(), _y3_u1()));
    }
    // Save delay line offset
    inst->shift = _ax0() - inst->xyptr_states;
}
/*================== REAL Q31 FIR INIT Function =============================*/

// Produce instance with: MAC32x32; pCoeffs - X; 

#pragma Code("dsplib_dsp_fir_init_q31_complex_cfXstY_MAC32x32")

/**
 *  initialization for Q31 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param offs_coeffs - XY memory offset to coefficients buffer
 * \param offs_states - XY memory offset to states vector buffer
 * \result dsp_status
 */

dsp_status dsp_fir_init_q31_complex_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint16_t nTaps,  uint32_t offs_coeffs, uint32_t offs_states)  
{
    inst->numTaps = nTaps;
    inst->shift = 0;
    inst->xyptr_coeffs = offs_coeffs;
    inst->xyptr_states = offs_states;
    _BurstYMem(offs_states, 0, nTaps << 1);
    return DSP_ERR_OK;
}
// Produce instance with: MAC32x32; pCoeffs - Y; 

#pragma Code("dsplib_dsp_fir_init_q31_complex_cfYstX_MAC32x32")

/**
 *  initialization for Q31 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param offs_coeffs - XY memory offset to coefficients buffer
 * \param offs_states - XY memory offset to states vector buffer
 * \result dsp_status
 */

dsp_status dsp_fir_init_q31_complex_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint16_t nTaps,  uint32_t offs_coeffs, uint32_t offs_states)  
{
    inst->numTaps = nTaps;
    inst->shift = 0;
    inst->xyptr_coeffs = offs_coeffs;
    inst->xyptr_states = offs_states;
    _BurstXMem(offs_states, 0, nTaps << 1);
    return DSP_ERR_OK;
}

#endif
