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
#include "dsp_transform_tables.h"


// Produce instance with: DMAC16x16; pIn0 - X; 

#pragma Code("dsplib_dsp_cfft_radix2_q15_XtoX_cfY_DMAC16x16")

/**
 *  Radix-2 Complex FFT "process" function for q15 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_cfft_radix2_q15_XtoX_cfY_DMAC16x16(const cfft_rad2_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t nLength2, nLength;
    int32_t nBlocks, nDfts;
    int32_t i, j, k;
    int32_t tmp;
    int32_t shiftBits = inst->shiftBits;
    int32_t Scale = 0x40004000;
    uint32_t offs_twd = inst->xyptr_twd;

    int32_t nOrder = 30-dspfx_norm_q31(inst->length);

    //_set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    _set_MACMODE(0x800c);    
    nLength = inst->length;    
    nLength2 = nLength >> 1;    
    
    
    // Set of bit-reverse addressing
    _set_ax0(offs_input);
    _set_mx00(AM_BITREV|AM_16BIT|nLength);
    _set_ax1(offs_input + 1);
    _set_mx10(AM_BITREV|AM_16BIT|nLength);
    _set_ax2(offs_output);
    _set_mx20(AM_16BIT|1);
    _set_mx21(AM_16BIT|AM_OFFS(-1));

    // Process inplace bitreverse shuffle
    if (offs_input == offs_output)
    {
        int32_t outaddr = offs_input;
        for (i = 0; i < nLength; i++)
        {
            int32_t revaddr = _ax0();
            int32_t temp_real = _x2_u0();
            int32_t temp_imag = _x2_u1();
    #ifdef _ARC
            __asm__(
                "        cmp      %[revaddr], %[outaddr]               \n"\
                "        mov.gt   %x2_u0, %x0_nu        \n"\
                "        mov.gt   %x2_u0, %x1_nu        \n"\
                "        mov.gt   %x0_u0, %[temp_real]          \n"\
                "        mov.gt   %x1_u0, %[temp_imag]          \n"
                : : [revaddr]"r"(revaddr), [outaddr]"r"(outaddr), [temp_real]"r"(temp_real), [temp_imag]"r"(temp_imag) :
            );
    #else
            if (revaddr > outaddr) 
            {
                _set_x2_u0(_x0_nu());
                _set_x2_u0(_x1_nu());
                _set_x0_u0(temp_real);
                _set_x1_u0(temp_imag);
            }
            else {
                _x0_u0();
                _x1_u0();
                _x2_u0();
                _x2_u0();
            }
    #endif
            outaddr += 2;
        }
        offs_input >>= 1;
        offs_output >>= 1;
        // Set linear addressing mode for input data
        _set_ax0(offs_input);
        _set_mx00(2);
        _set_ax1(offs_input + 1);
        _set_mx10(2);
    } else {
        offs_input >>= 1;
        offs_output >>= 1;
        // Set bit-reverse addressing mode for input data
        _set_ax0(offs_input);
        _set_mx00(AM_BITREV|(nLength >> 1));
        _set_ax1(offs_input + 1);
        _set_mx10(AM_BITREV|(nLength >> 1));
    }

    //first stage
    _set_ax2(offs_output);
    _set_mx20(1);
    nBlocks = nLength2;
    for(i = 0 ; i < nBlocks; i++){
        tmp = _x0_u0(); //_y0_u0();
        _set_x2_u0( _addsdw(tmp, _x1_nu()));
        _set_x2_u0( _subsdw(tmp, _x1_u0()));
    }
    // next stage
    
    _set_mx30(1);
    
    _set_ay0(offs_twd);
    _set_ay1(offs_twd + 1);
    
    // Scale data if needed (shiftBits > 0)
    
    if (shiftBits > 0){
        _set_ax2(offs_output);
        _set_ax3(offs_output);
        _nop();
        for(i = 0 ; i < nLength2; i++){
            _set_x3_u0( _mulrdw(Scale, _x2_u0()));
            _set_x3_u0( _mulrdw(Scale, _x2_u0()));
        }
        shiftBits--;
    }
    nBlocks >>= 1;
    nDfts = 2;
    
    for (k = 1; k < nOrder; k++) {
    
        _set_ax2(offs_output);
        _set_mx21((nDfts + 1));
        _set_ax3((offs_output + nDfts));
        _set_mx31((nDfts + 1));
        
        _set_my00((2*nBlocks|AM_16BIT|AM_DUAL));
        _set_my01((AM_OFFS(2*(nBlocks - nLength2))|AM_16BIT|AM_DUAL));
        _set_my10((2*nBlocks|AM_16BIT|AM_DUAL));
        _set_my11((AM_OFFS(2*(nBlocks - nLength2))|AM_16BIT|AM_DUAL));
        
        for (j = 0; j < nBlocks; j++) {
#ifdef _ARC_BE
            uint32_t inputA, inputB;
            uint32_t outputA, outputB;
            inputA = _ror(_x2_nu(), 16);
            _fbfdw(inputA);            
            for (i = 0; i < nDfts - 1; i++) {
                // +w.re*x.re | +w.re*x.im
                inputB = _ror(_x3_nu() , 16);
                _macrdw(_y0_u0(), inputB);                
                // -w.im*x.im | +w.im*x.re
                outputA = _cmacrdw(_y1_u0(), inputB);
                _set_x2_u0( _ror(outputA, 16));
                
                inputA = _ror(_x2_nu(),16);
                outputB = _fbfdw(inputA);
                _set_x3_u0( (_ror(outputB, 16)));
            }
            // +w.re*x.re | +w.re*x.im
            inputB = _ror(_x3_nu() , 16);
            _macrdw(_y0_u1(), inputB);            
            // -w.im*x.im | +w.im*x.re
            outputA = _cmacrdw(_y1_u1(), inputB);
            _set_x2_nu( _ror(outputA, 16));
            
            inputA = _ror(_x2_u1(), 16);
            outputB = _fbfdw(inputA);
            _set_x3_u1( (_ror(outputB, 16)));
#else
            _fbfdw(_x2_nu());            
            for (i = 0; i < nDfts - 1; i++) {
                // +w.re*x.re | +w.re*x.im           
                _macrdw(_y0_u0(), _x3_nu());                
                // -w.im*x.im | +w.im*x.re
                _set_x2_u0( (_cmacrdw(_y1_u0(), _x3_nu())));
                _set_x3_u0((_fbfdw(_x2_nu())));
            }
            // +w.re*x.re | +w.re*x.im
            _macrdw(_y0_u1(), _x3_nu());            
            // -w.im*x.im | +w.im*x.re
            _set_x2_nu( (_cmacrdw(_y1_u1(), _x3_nu())));
            _set_x3_u1( (_fbfdw(_x2_u1())));
#endif
        }
            // Scale data if needed (shiftBits > 0)
    
        if (shiftBits > 0){
            _set_ax2(offs_output);
            _set_ax3(offs_output);
            _nop();
            for(i = 0 ; i < nLength2; i++){
                _set_x3_u0( _mulrdw(Scale, _x2_u0()));
                _set_x3_u0( _mulrdw(Scale, _x2_u0()));
            }
            shiftBits--;
        }
        nBlocks >>= 1;
        nDfts <<= 1;
    }    
}

#pragma Code()

// Produce instance with: DMAC16x16; pIn0 - Y; 

#pragma Code("dsplib_dsp_cfft_radix2_q15_YtoY_cfX_DMAC16x16")

/**
 *  Radix-2 Complex FFT "process" function for q15 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_cfft_radix2_q15_YtoY_cfX_DMAC16x16(const cfft_rad2_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t nLength2, nLength;
    int32_t nBlocks, nDfts;
    int32_t i, j, k;
    int32_t tmp;
    int32_t shiftBits = inst->shiftBits;
    int32_t Scale = 0x40004000;
    uint32_t offs_twd = inst->xyptr_twd;

    int32_t nOrder = 30-dspfx_norm_q31(inst->length);

    //_set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    _set_MACMODE(0x800c);    
    nLength = inst->length;    
    nLength2 = nLength >> 1;    
    
    
    // Set of bit-reverse addressing
    _set_ay0(offs_input);
    _set_my00(AM_BITREV|AM_16BIT|nLength);
    _set_ay1(offs_input + 1);
    _set_my10(AM_BITREV|AM_16BIT|nLength);
    _set_ay2(offs_output);
    _set_my20(AM_16BIT|1);
    _set_my21(AM_16BIT|AM_OFFS(-1));

    // Process inplace bitreverse shuffle
    if (offs_input == offs_output)
    {
        int32_t outaddr = offs_input;
        for (i = 0; i < nLength; i++)
        {
            int32_t revaddr = _ay0();
            int32_t temp_real = _y2_u0();
            int32_t temp_imag = _y2_u1();
    #ifdef _ARC
            __asm__(
                "        cmp      %[revaddr], %[outaddr]               \n"\
                "        mov.gt   %y2_u0, %y0_nu        \n"\
                "        mov.gt   %y2_u0, %y1_nu        \n"\
                "        mov.gt   %y0_u0, %[temp_real]          \n"\
                "        mov.gt   %y1_u0, %[temp_imag]          \n"
                : : [revaddr]"r"(revaddr), [outaddr]"r"(outaddr), [temp_real]"r"(temp_real), [temp_imag]"r"(temp_imag) :
            );
    #else
            if (revaddr > outaddr) 
            {
                _set_y2_u0(_y0_nu());
                _set_y2_u0(_y1_nu());
                _set_y0_u0(temp_real);
                _set_y1_u0(temp_imag);
            }
            else {
                _y0_u0();
                _y1_u0();
                _y2_u0();
                _y2_u0();
            }
    #endif
            outaddr += 2;
        }
        offs_input >>= 1;
        offs_output >>= 1;
        // Set linear addressing mode for input data
        _set_ay0(offs_input);
        _set_my00(2);
        _set_ay1(offs_input + 1);
        _set_my10(2);
    } else {
        offs_input >>= 1;
        offs_output >>= 1;
        // Set bit-reverse addressing mode for input data
        _set_ay0(offs_input);
        _set_my00(AM_BITREV|(nLength >> 1));
        _set_ay1(offs_input + 1);
        _set_my10(AM_BITREV|(nLength >> 1));
    }

    //first stage
    _set_ay2(offs_output);
    _set_my20(1);
    nBlocks = nLength2;
    for(i = 0 ; i < nBlocks; i++){
        tmp = _y0_u0(); //_y0_u0();
        _set_y2_u0( _addsdw(tmp, _y1_nu()));
        _set_y2_u0( _subsdw(tmp, _y1_u0()));
    }
    // next stage
    
    _set_my30(1);
    
    _set_ax0(offs_twd);
    _set_ax1(offs_twd + 1);
    
    // Scale data if needed (shiftBits > 0)
    
    if (shiftBits > 0){
        _set_ay2(offs_output);
        _set_ay3(offs_output);
        _nop();
        for(i = 0 ; i < nLength2; i++){
            _set_y3_u0( _mulrdw(Scale, _y2_u0()));
            _set_y3_u0( _mulrdw(Scale, _y2_u0()));
        }
        shiftBits--;
    }
    nBlocks >>= 1;
    nDfts = 2;
    
    for (k = 1; k < nOrder; k++) {
    
        _set_ay2(offs_output);
        _set_my21((nDfts + 1));
        _set_ay3((offs_output + nDfts));
        _set_my31((nDfts + 1));
        
        _set_mx00((2*nBlocks|AM_16BIT|AM_DUAL));
        _set_mx01((AM_OFFS(2*(nBlocks - nLength2))|AM_16BIT|AM_DUAL));
        _set_mx10((2*nBlocks|AM_16BIT|AM_DUAL));
        _set_mx11((AM_OFFS(2*(nBlocks - nLength2))|AM_16BIT|AM_DUAL));
        
        for (j = 0; j < nBlocks; j++) {
#ifdef _ARC_BE
            uint32_t inputA, inputB;
            uint32_t outputA, outputB;
            inputA = _ror(_y2_nu(), 16);
            _fbfdw(inputA);            
            for (i = 0; i < nDfts - 1; i++) {
                // +w.re*x.re | +w.re*x.im
                inputB = _ror(_y3_nu() , 16);
                _macrdw(_x0_u0(), inputB);                
                // -w.im*x.im | +w.im*x.re
                outputA = _cmacrdw(_x1_u0(), inputB);
                _set_y2_u0( _ror(outputA, 16));
                
                inputA = _ror(_y2_nu(),16);
                outputB = _fbfdw(inputA);
                _set_y3_u0( (_ror(outputB, 16)));
            }
            // +w.re*x.re | +w.re*x.im
            inputB = _ror(_y3_nu() , 16);
            _macrdw(_x0_u1(), inputB);            
            // -w.im*x.im | +w.im*x.re
            outputA = _cmacrdw(_x1_u1(), inputB);
            _set_y2_nu( _ror(outputA, 16));
            
            inputA = _ror(_y2_u1(), 16);
            outputB = _fbfdw(inputA);
            _set_y3_u1( (_ror(outputB, 16)));
#else
            _fbfdw(_y2_nu());            
            for (i = 0; i < nDfts - 1; i++) {
                // +w.re*x.re | +w.re*x.im           
                _macrdw(_x0_u0(), _y3_nu());                
                // -w.im*x.im | +w.im*x.re
                _set_y2_u0( (_cmacrdw(_x1_u0(), _y3_nu())));
                _set_y3_u0((_fbfdw(_y2_nu())));
            }
            // +w.re*x.re | +w.re*x.im
            _macrdw(_x0_u1(), _y3_nu());            
            // -w.im*x.im | +w.im*x.re
            _set_y2_nu( (_cmacrdw(_x1_u1(), _y3_nu())));
            _set_y3_u1( (_fbfdw(_y2_u1())));
#endif
        }
            // Scale data if needed (shiftBits > 0)
    
        if (shiftBits > 0){
            _set_ay2(offs_output);
            _set_ay3(offs_output);
            _nop();
            for(i = 0 ; i < nLength2; i++){
                _set_y3_u0( _mulrdw(Scale, _y2_u0()));
                _set_y3_u0( _mulrdw(Scale, _y2_u0()));
            }
            shiftBits--;
        }
        nBlocks >>= 1;
        nDfts <<= 1;
    }    
}

#pragma Code()



// Produce instance with: DMAC16x16; pIn0 - X; 

#pragma Code("dsplib_dsp_cfft_radix2_q15_XtoY_cfX_DMAC16x16")

/**
 *  Radix-2 Complex FFT "process" function for q15 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_cfft_radix2_q15_XtoY_cfX_DMAC16x16(const cfft_rad2_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t nLength2, nLength;
    int32_t nBlocks, nDfts;
    int32_t i, j, k;
    int32_t tmp;
    int32_t Scale = 0x40004000;
    
    uint32_t offs_twd = inst->xyptr_twd;
    
    int32_t shiftBits = inst->shiftBits;
    int32_t nOrder = 30-dspfx_norm_q31(inst->length);

    //_set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    _set_MACMODE(0x800c);
    offs_input >>= 1;
    offs_output >>= 1;
    nLength = inst->length;    
    nLength2 = nLength >> 1;
    _set_ax0(offs_input);
    _set_mx00(AM_BITREV|(nLength >> 1));
    _set_ay0(offs_output);
    _set_my00(1);
    
    //first stage
    nBlocks = nLength2;
    for(i = 0 ; i < nBlocks; i++){
        tmp = _x0_u0(); //_y0_u0();
        _set_y0_u0( _addsdw(tmp, _x0_nu()));
        _set_y0_u0( _subsdw(tmp, _x0_u0()));
    }
    _set_my10(1);
    // Scale data if needed (shiftBits > 0)
    
    if (shiftBits > 0){
        _set_ay0(offs_output);
        _set_ay1(offs_output);
        _nop();
        for(i = 0 ; i < nLength2; i++){
            _set_y1_u0( _mulrdw(Scale, _y0_u0()));
            _set_y1_u0( _mulrdw(Scale, _y0_u0()));
        }
        shiftBits--;
    }
    
    // Do stages with scale
        
    _set_ax1(offs_twd);
    _set_ax2(offs_twd+1);
    
    nBlocks >>= 1;
    nDfts = 2;
    
    for (k = 1; k < nOrder; k++) {
    
        _set_ay0(offs_output);
        _set_my01((nDfts + 1));
        _set_ay1((offs_output + nDfts));
        _set_my11((nDfts + 1));
        
        _set_mx10((2*nBlocks|AM_16BIT|AM_DUAL));
        _set_mx11((AM_OFFS(2*(nBlocks - nLength2))|AM_16BIT|AM_DUAL));
        _set_mx20((2*nBlocks|AM_16BIT|AM_DUAL));
        _set_mx21((AM_OFFS(2*(nBlocks - nLength2))|AM_16BIT|AM_DUAL));
        
        for (j = 0; j < nBlocks; j++) {
#ifdef _ARC_BE
            uint32_t inputA, inputB;
            uint32_t outputA, outputB;
            inputA = _ror(_y0_nu(), 16);
            _fbfdw(inputA);            
            for (i = 0; i < nDfts - 1; i++) {
                // +w.re*x.re | +w.re*x.im
                inputB = _ror(_y1_nu() , 16);
                _macrdw(_x1_u0(), inputB);                
                // -w.im*x.im | +w.im*x.re
                outputA = _cmacrdw(_x2_u0(), inputB);
                _set_y0_u0( _ror(outputA, 16));
                
                inputA = _ror(_y0_nu(),16);
                outputB = _fbfdw(inputA);
                _set_y1_u0( (_ror(outputB, 16)));
            }
            // +w.re*x.re | +w.re*x.im
            inputB = _ror(_y1_nu() , 16);
            _macrdw(_x1_u1(), inputB);            
            // -w.im*x.im | +w.im*x.re
            outputA = _cmacrdw(_x2_u1(), inputB);
            _set_y0_nu( _ror(outputA, 16));
            
            inputA = _ror(_y0_u1(), 16);
            outputB = _fbfdw(inputA);
            _set_y1_u1( (_ror(outputB, 16)));
#else
            _fbfdw(_y0_nu());            
            for (i = 0; i < nDfts - 1; i++) {
                // +w.re*x.re | +w.re*x.im           
                _macrdw(_x1_u0(), _y1_nu());                
                // -w.im*x.im | +w.im*x.re
                _set_y0_u0( (_cmacrdw(_x2_u0(), _y1_nu())));
                _set_y1_u0((_fbfdw(_y0_nu())));
            }
            // +w.re*x.re | +w.re*x.im
            _macrdw(_x1_u1(), _y1_nu());            
            // -w.im*x.im | +w.im*x.re
            _set_y0_nu( (_cmacrdw(_x2_u1(), _y1_nu())));
            _set_y1_u1( (_fbfdw(_y0_u1())));
#endif
        }
        // Scale data if needed (shiftBits > 0)
        if (shiftBits > 0)
        {
            _set_ay0(offs_output);
            _set_ay1(offs_output);
            _nop();
            for(i = 0 ; i < nLength2; i++){
                _set_y1_u0( _mulrdw(Scale, _y0_u0()));
                _set_y1_u0( _mulrdw(Scale, _y0_u0()));
            }
            shiftBits--;
        }
        nBlocks >>= 1;
        nDfts <<= 1;
    }
}

#pragma Code()

// Produce instance with: DMAC16x16; pIn0 - Y; 

#pragma Code("dsplib_dsp_cfft_radix2_q15_YtoX_cfY_DMAC16x16")

/**
 *  Radix-2 Complex FFT "process" function for q15 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_cfft_radix2_q15_YtoX_cfY_DMAC16x16(const cfft_rad2_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t nLength2, nLength;
    int32_t nBlocks, nDfts;
    int32_t i, j, k;
    int32_t tmp;
    int32_t Scale = 0x40004000;
    
    uint32_t offs_twd = inst->xyptr_twd;
    
    int32_t shiftBits = inst->shiftBits;
    int32_t nOrder = 30-dspfx_norm_q31(inst->length);

    //_set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    _set_MACMODE(0x800c);
    offs_input >>= 1;
    offs_output >>= 1;
    nLength = inst->length;    
    nLength2 = nLength >> 1;
    _set_ay0(offs_input);
    _set_my00(AM_BITREV|(nLength >> 1));
    _set_ax0(offs_output);
    _set_mx00(1);
    
    //first stage
    nBlocks = nLength2;
    for(i = 0 ; i < nBlocks; i++){
        tmp = _y0_u0(); //_y0_u0();
        _set_x0_u0( _addsdw(tmp, _y0_nu()));
        _set_x0_u0( _subsdw(tmp, _y0_u0()));
    }
    _set_mx10(1);
    // Scale data if needed (shiftBits > 0)
    
    if (shiftBits > 0){
        _set_ax0(offs_output);
        _set_ax1(offs_output);
        _nop();
        for(i = 0 ; i < nLength2; i++){
            _set_x1_u0( _mulrdw(Scale, _x0_u0()));
            _set_x1_u0( _mulrdw(Scale, _x0_u0()));
        }
        shiftBits--;
    }
    
    // Do stages with scale
        
    _set_ay1(offs_twd);
    _set_ay2(offs_twd+1);
    
    nBlocks >>= 1;
    nDfts = 2;
    
    for (k = 1; k < nOrder; k++) {
    
        _set_ax0(offs_output);
        _set_mx01((nDfts + 1));
        _set_ax1((offs_output + nDfts));
        _set_mx11((nDfts + 1));
        
        _set_my10((2*nBlocks|AM_16BIT|AM_DUAL));
        _set_my11((AM_OFFS(2*(nBlocks - nLength2))|AM_16BIT|AM_DUAL));
        _set_my20((2*nBlocks|AM_16BIT|AM_DUAL));
        _set_my21((AM_OFFS(2*(nBlocks - nLength2))|AM_16BIT|AM_DUAL));
        
        for (j = 0; j < nBlocks; j++) {
#ifdef _ARC_BE
            uint32_t inputA, inputB;
            uint32_t outputA, outputB;
            inputA = _ror(_x0_nu(), 16);
            _fbfdw(inputA);            
            for (i = 0; i < nDfts - 1; i++) {
                // +w.re*x.re | +w.re*x.im
                inputB = _ror(_x1_nu() , 16);
                _macrdw(_y1_u0(), inputB);                
                // -w.im*x.im | +w.im*x.re
                outputA = _cmacrdw(_y2_u0(), inputB);
                _set_x0_u0( _ror(outputA, 16));
                
                inputA = _ror(_x0_nu(),16);
                outputB = _fbfdw(inputA);
                _set_x1_u0( (_ror(outputB, 16)));
            }
            // +w.re*x.re | +w.re*x.im
            inputB = _ror(_x1_nu() , 16);
            _macrdw(_y1_u1(), inputB);            
            // -w.im*x.im | +w.im*x.re
            outputA = _cmacrdw(_y2_u1(), inputB);
            _set_x0_nu( _ror(outputA, 16));
            
            inputA = _ror(_x0_u1(), 16);
            outputB = _fbfdw(inputA);
            _set_x1_u1( (_ror(outputB, 16)));
#else
            _fbfdw(_x0_nu());            
            for (i = 0; i < nDfts - 1; i++) {
                // +w.re*x.re | +w.re*x.im           
                _macrdw(_y1_u0(), _x1_nu());                
                // -w.im*x.im | +w.im*x.re
                _set_x0_u0( (_cmacrdw(_y2_u0(), _x1_nu())));
                _set_x1_u0((_fbfdw(_x0_nu())));
            }
            // +w.re*x.re | +w.re*x.im
            _macrdw(_y1_u1(), _x1_nu());            
            // -w.im*x.im | +w.im*x.re
            _set_x0_nu( (_cmacrdw(_y2_u1(), _x1_nu())));
            _set_x1_u1( (_fbfdw(_x0_u1())));
#endif
        }
        // Scale data if needed (shiftBits > 0)
        if (shiftBits > 0)
        {
            _set_ax0(offs_output);
            _set_ax1(offs_output);
            _nop();
            for(i = 0 ; i < nLength2; i++){
                _set_x1_u0( _mulrdw(Scale, _x0_u0()));
                _set_x1_u0( _mulrdw(Scale, _x0_u0()));
            }
            shiftBits--;
        }
        nBlocks >>= 1;
        nDfts <<= 1;
    }
}

#pragma Code()



// Produce instance with: DMAC16x16; pIn - X; 

#pragma Code("dsplib_dsp_cfft_radix2_init_q15_cfX_DMAC16x16")

/**
 * Initializes instance structure of raidx2 complex fft transform
 * for q15 data type. 
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_tabcos - XY memory offset to cosine table
 * \param offs_tabsin - XY memory offset to sine table
 * \result dsp_status
 */

dsp_status dsp_cfft_radix2_init_q15_cfX_DMAC16x16(cfft_rad2_q15_xy_t *inst, uint32_t length, uint32_t offs_twiddles)  
{
    uint32_t order = 30-dspfx_norm_q31(length);

    inst->length = length;
    inst->xyptr_twd = offs_twiddles;
    inst->shiftBits = 0;
 
     _BurstSMemToXMem(
        offs_twiddles>>1,
        (const int32_t*)(void*)dsp_cfft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER],
        length>>1);

    return DSP_ERR_OK;
};

#pragma Code()

// Produce instance with: DMAC16x16; pIn - Y; 

#pragma Code("dsplib_dsp_cfft_radix2_init_q15_cfY_DMAC16x16")

/**
 * Initializes instance structure of raidx2 complex fft transform
 * for q15 data type. 
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_tabcos - XY memory offset to cosine table
 * \param offs_tabsin - XY memory offset to sine table
 * \result dsp_status
 */

dsp_status dsp_cfft_radix2_init_q15_cfY_DMAC16x16(cfft_rad2_q15_xy_t *inst, uint32_t length, uint32_t offs_twiddles)  
{
    uint32_t order = 30-dspfx_norm_q31(length);

    inst->length = length;
    inst->xyptr_twd = offs_twiddles;
    inst->shiftBits = 0;
 
     _BurstSMemToYMem(
        offs_twiddles>>1,
        (const int32_t*)(void*)dsp_cfft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER],
        length>>1);

    return DSP_ERR_OK;
};

#pragma Code()


// Produce instance with: DMAC16x16; pIn - X; 

#pragma Code("dsplib_dsp_cifft_radix2_init_q15_cfX_DMAC16x16")

/**
 * Initializes instance structure of raidx2 inverse complex fft transform
 * for q15 data type. 
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_tabcos - XY memory offset to cosine table
 * \param offs_tabsin - XY memory offset to sine table
 * \result dsp_status
 */

dsp_status dsp_cifft_radix2_init_q15_cfX_DMAC16x16(cfft_rad2_q15_xy_t *inst, uint32_t length, uint32_t offs_twiddles)  
{
    uint32_t order = 30-dspfx_norm_q31(length);
    inst->length = length;
    inst->xyptr_twd = offs_twiddles;
    inst->shiftBits = order;
 
     _BurstSMemToXMem(
        offs_twiddles>>1,
        (const int32_t*)(void*)dsp_cifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER],
        length>>1);

    return DSP_ERR_OK;
};

#pragma Code()

// Produce instance with: DMAC16x16; pIn - Y; 

#pragma Code("dsplib_dsp_cifft_radix2_init_q15_cfY_DMAC16x16")

/**
 * Initializes instance structure of raidx2 inverse complex fft transform
 * for q15 data type. 
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_tabcos - XY memory offset to cosine table
 * \param offs_tabsin - XY memory offset to sine table
 * \result dsp_status
 */

dsp_status dsp_cifft_radix2_init_q15_cfY_DMAC16x16(cfft_rad2_q15_xy_t *inst, uint32_t length, uint32_t offs_twiddles)  
{
    uint32_t order = 30-dspfx_norm_q31(length);
    inst->length = length;
    inst->xyptr_twd = offs_twiddles;
    inst->shiftBits = order;
 
     _BurstSMemToYMem(
        offs_twiddles>>1,
        (const int32_t*)(void*)dsp_cifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER],
        length>>1);

    return DSP_ERR_OK;
};

#pragma Code()


#endif
