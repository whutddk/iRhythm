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

#pragma Code("dsplib_dsp_biquad_cascade_df1_q15_X_cfYstY_MAC16x16")

/**
 *  Q15 DF1 type Biquad-Cascade filter
 *
 * \param inst - pointer to instance structure
 * \param offs_inout - XY memory offset to inplace input/output buffer.
 * \param nSamples - number of samples to be processed
 */

void dsp_biquad_cascade_df1_q15_X_cfYstY_MAC16x16(  const iir_biquad_q15_xy_t *inst,    uint32_t offs_inout,   uint32_t nSamples) 
{
    int32_t stage; // Stage counter
    uint32_t i; // Sample counter 

    int32_t postshift = inst->postShift;
    int32_t order  = inst->filter.numTaps;


    _set_mx00(AM_16BIT|1);
    _set_ay0(inst->filter.xyptr_states);
    _set_my00(AM_16BIT|1);
    _set_my01(AM_16BIT|AM_OFFS(-3));
    _set_ay1(inst->filter.xyptr_coeffs);
    _set_my10(AM_16BIT|1);


    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));

    for (stage = 0; stage < order; stage++)
    {
        q31_t b0, b1, b2, a1, a2;
        q31_t y0, y1, y2, x1, x2;
        _set_ax0(offs_inout);
        b0 =  _y1_u0();// *coeffs++;
        b1 =  _y1_u0();// *coeffs++;
        b2 =  _y1_u0();// *coeffs++;
        a1 =  _y1_u0();// *coeffs++;
        a2 =  _y1_u0();// *coeffs++;

        y1 = _y0_u0();//*states++;
        y2 = _y0_u0();
        x1 = _y0_u0();
        x2 = _y0_u1();

        for (i = 0; i < (int32_t)nSamples; i++) 
        {
            _mulrdw(_x0_nu(), b0); //*src++;
            _macrdw(x1, b1);
            _macrdw(x2, b2);
            _macrdw(y1, a1);
            _macrdw(y2, a2);
            y0 = dspfx_asl_q31(_get_ACC1(), postshift);
            y2 = y1;
            y1 = y0;
            x2 = x1;
            x1 = _x0_nu();
            _set_x0_u0(y0);
        }

        //update states
        _set_y0_u0(y1);
        _set_y0_u0(y2);
        _set_y0_u0(x1);
        _set_y0_u0(x2);
    }

};
// Produce instance with: MAC16x16; pInOut - Y; 

#pragma Code("dsplib_dsp_biquad_cascade_df1_q15_Y_cfXstX_MAC16x16")

/**
 *  Q15 DF1 type Biquad-Cascade filter
 *
 * \param inst - pointer to instance structure
 * \param offs_inout - XY memory offset to inplace input/output buffer.
 * \param nSamples - number of samples to be processed
 */

void dsp_biquad_cascade_df1_q15_Y_cfXstX_MAC16x16(  const iir_biquad_q15_xy_t *inst,    uint32_t offs_inout,   uint32_t nSamples) 
{
    int32_t stage; // Stage counter
    uint32_t i; // Sample counter 

    int32_t postshift = inst->postShift;
    int32_t order  = inst->filter.numTaps;


    _set_my00(AM_16BIT|1);
    _set_ax0(inst->filter.xyptr_states);
    _set_mx00(AM_16BIT|1);
    _set_mx01(AM_16BIT|AM_OFFS(-3));
    _set_ax1(inst->filter.xyptr_coeffs);
    _set_mx10(AM_16BIT|1);


    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));

    for (stage = 0; stage < order; stage++)
    {
        q31_t b0, b1, b2, a1, a2;
        q31_t y0, y1, y2, x1, x2;
        _set_ay0(offs_inout);
        b0 =  _x1_u0();// *coeffs++;
        b1 =  _x1_u0();// *coeffs++;
        b2 =  _x1_u0();// *coeffs++;
        a1 =  _x1_u0();// *coeffs++;
        a2 =  _x1_u0();// *coeffs++;

        y1 = _x0_u0();//*states++;
        y2 = _x0_u0();
        x1 = _x0_u0();
        x2 = _x0_u1();

        for (i = 0; i < (int32_t)nSamples; i++) 
        {
            _mulrdw(_y0_nu(), b0); //*src++;
            _macrdw(x1, b1);
            _macrdw(x2, b2);
            _macrdw(y1, a1);
            _macrdw(y2, a2);
            y0 = dspfx_asl_q31(_get_ACC1(), postshift);
            y2 = y1;
            y1 = y0;
            x2 = x1;
            x1 = _y0_nu();
            _set_y0_u0(y0);
        }

        //update states
        _set_x0_u0(y1);
        _set_x0_u0(y2);
        _set_x0_u0(x1);
        _set_x0_u0(x2);
    }

};

// Produce instance with: MAC16x16; pSt - X; 

#pragma Code("dsplib_dsp_biquad_cascade_df1_init_q15_cfXstX")

/**
 *  initialization for Q15 IIR filtering
 *
 * \param inst - pointer to instance structure
 * \param numStages
 * \param pshift
 * \param offs_states - XY memory offset to states vector buffer
 * \param offs_coeffs - XY memory offset to coefficients buffer
 * \result dsp_status
 */

dsp_status dsp_biquad_cascade_df1_init_q15_cfXstX(iir_biquad_q15_xy_t *inst, uint32_t numStages, uint32_t pshift,  uint32_t offs_states, uint32_t offs_coeffs)   
{
    inst->filter.xyptr_coeffs = offs_coeffs;
    inst->filter.xyptr_states = offs_states;
    inst->filter.numTaps = numStages;
    inst->postShift = pshift;
    _MEM_FILL_X_Q15(2*offs_states, 0, numStages*4);

    return DSP_ERR_OK;
};
// Produce instance with: MAC16x16; pSt - Y; 

#pragma Code("dsplib_dsp_biquad_cascade_df1_init_q15_cfYstY")

/**
 *  initialization for Q15 IIR filtering
 *
 * \param inst - pointer to instance structure
 * \param numStages
 * \param pshift
 * \param offs_states - XY memory offset to states vector buffer
 * \param offs_coeffs - XY memory offset to coefficients buffer
 * \result dsp_status
 */

dsp_status dsp_biquad_cascade_df1_init_q15_cfYstY(iir_biquad_q15_xy_t *inst, uint32_t numStages, uint32_t pshift,  uint32_t offs_states, uint32_t offs_coeffs)   
{
    inst->filter.xyptr_coeffs = offs_coeffs;
    inst->filter.xyptr_states = offs_states;
    inst->filter.numTaps = numStages;
    inst->postShift = pshift;
    _MEM_FILL_Y_Q15(2*offs_states, 0, numStages*4);

    return DSP_ERR_OK;
};
#endif
