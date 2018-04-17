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
#include "dsp_transform_tables.h"

// Produce instance with: MAC32x32; pIn0 - X; 

#pragma Code("dsplib_dsp_rfft_postprocess_q31_XtoY_cfX_MAC32x32")

/**
 *  Real FFT postprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_rfft_postprocess_q31_XtoY_cfX_MAC32x32(const rfft_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    uint32_t i;
    uint32_t length = inst->length;
    int32_t in0tmp_re, in0tmp_im;
    int32_t Are, Aim,  Bre , Bim;
    q31_t xCos, xSin;
  

    _set_ax2(inst->xyptr_twd);
    _set_mx20(1);
    _set_ax0(offs_input);
    _set_mx00(1);
    _set_ay0(offs_output);
    _set_my00(1);

    xCos = ((q31_t)_x2_u0())>>1;
    xSin = ((q31_t)_x2_u0())>>1;

    Are = dspfx_add_q31(0x3fffffff,xSin);
    Bre = dspfx_sub_q31(0x3fffffff,xSin);
    Aim = xCos;
    Bim = dspfx_neg_q31(xCos);
 
    in0tmp_re = _x0_u0();
    in0tmp_im = _x0_u0();

     _mulflw(in0tmp_re, Are);
    _machflw(in0tmp_re, Are); 
     _macflw(in0tmp_re, Bre);
    _machflw(in0tmp_re, Bre); 
     _macflw(in0tmp_im, Bim);
    _machflw(in0tmp_im, Bim);

                _macflw(in0tmp_im, dspfx_neg_q31(Aim));
    _set_y0_u0(_machflw(in0tmp_im, dspfx_neg_q31(Aim)));
    _set_y0_u0(0);

    _set_ax1(offs_input + length-1);
    _set_mx10(AM_OFFS(-1));

    for (i = 1; i<length / 2; i++)
    {
        q31_t xCos = ((q31_t)_x2_u0())>>1;
        q31_t xSin = ((q31_t)_x2_u0())>>1;
        int32_t Are = dspfx_add_q31(0x3fffffff, xSin);
        int32_t Aim = xCos;
        int32_t neg_Aim = dspfx_neg_q31(Aim);
        int32_t Bre = dspfx_sub_q31(0x3fffffff, xSin);
        int32_t neg_Bre = dspfx_neg_q31(Bre);
        int32_t Bim = dspfx_neg_q31(xCos);

        int32_t input2i   = _x0_u0();
        int32_t input2i1  = _x0_u0();
        int32_t inputm2i1 = _x1_u0();
        int32_t inputm2i  = _x1_u0();

                     _mulflw(input2i, Are);
                    _machflw(input2i, Are);
                     _macflw(input2i1, neg_Aim);
                    _machflw(input2i1, neg_Aim);
                     _macflw(inputm2i, Bre);
                    _machflw(inputm2i, Bre);
                    _macflw(inputm2i1, Bim);
        _set_y0_u0(_machflw(inputm2i1, Bim));

                     _mulflw(input2i1, Are);
                    _machflw(input2i1, Are);
                     _macflw(input2i, Aim);
                    _machflw(input2i, Aim);
                     _macflw(inputm2i, Bim);
                    _machflw(inputm2i, Bim);
                    _macflw(inputm2i1, neg_Bre);
        _set_y0_u0(_machflw(inputm2i1, neg_Bre));
    }
    
    _set_y0_u0( dspfx_sub_q31(in0tmp_re, in0tmp_im));
    _set_y0_u0(0);

    _set_ay1(offs_output + 2*(length/2 - 1));
    _set_my10(1);
    _set_my11(AM_OFFS(-3));

    for (i = 1; i < length/2; i++)
    {
        _set_y0_u0(         _y1_u0());
        _set_y0_u0( dspfx_neg_q31(_y1_u1()));
    }
};

// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_rfft_postprocess_q31_YtoX_cfY_MAC32x32")

/**
 *  Real FFT postprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_rfft_postprocess_q31_YtoX_cfY_MAC32x32(const rfft_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    uint32_t i;
    uint32_t length = inst->length;
    int32_t in0tmp_re, in0tmp_im;
    int32_t Are, Aim,  Bre , Bim;
    q31_t xCos, xSin;
  

    _set_ay2(inst->xyptr_twd);
    _set_my20(1);
    _set_ay0(offs_input);
    _set_my00(1);
    _set_ax0(offs_output);
    _set_mx00(1);

    xCos = ((q31_t)_y2_u0())>>1;
    xSin = ((q31_t)_y2_u0())>>1;

    Are = dspfx_add_q31(0x3fffffff,xSin);
    Bre = dspfx_sub_q31(0x3fffffff,xSin);
    Aim = xCos;
    Bim = dspfx_neg_q31(xCos);
 
    in0tmp_re = _y0_u0();
    in0tmp_im = _y0_u0();

     _mulflw(in0tmp_re, Are);
    _machflw(in0tmp_re, Are); 
     _macflw(in0tmp_re, Bre);
    _machflw(in0tmp_re, Bre); 
     _macflw(in0tmp_im, Bim);
    _machflw(in0tmp_im, Bim);

                _macflw(in0tmp_im, dspfx_neg_q31(Aim));
    _set_x0_u0(_machflw(in0tmp_im, dspfx_neg_q31(Aim)));
    _set_x0_u0(0);

    _set_ay1(offs_input + length-1);
    _set_my10(AM_OFFS(-1));

    for (i = 1; i<length / 2; i++)
    {
        q31_t xCos = ((q31_t)_y2_u0())>>1;
        q31_t xSin = ((q31_t)_y2_u0())>>1;
        int32_t Are = dspfx_add_q31(0x3fffffff, xSin);
        int32_t Aim = xCos;
        int32_t neg_Aim = dspfx_neg_q31(Aim);
        int32_t Bre = dspfx_sub_q31(0x3fffffff, xSin);
        int32_t neg_Bre = dspfx_neg_q31(Bre);
        int32_t Bim = dspfx_neg_q31(xCos);

        int32_t input2i   = _y0_u0();
        int32_t input2i1  = _y0_u0();
        int32_t inputm2i1 = _y1_u0();
        int32_t inputm2i  = _y1_u0();

                     _mulflw(input2i, Are);
                    _machflw(input2i, Are);
                     _macflw(input2i1, neg_Aim);
                    _machflw(input2i1, neg_Aim);
                     _macflw(inputm2i, Bre);
                    _machflw(inputm2i, Bre);
                    _macflw(inputm2i1, Bim);
        _set_x0_u0(_machflw(inputm2i1, Bim));

                     _mulflw(input2i1, Are);
                    _machflw(input2i1, Are);
                     _macflw(input2i, Aim);
                    _machflw(input2i, Aim);
                     _macflw(inputm2i, Bim);
                    _machflw(inputm2i, Bim);
                    _macflw(inputm2i1, neg_Bre);
        _set_x0_u0(_machflw(inputm2i1, neg_Bre));
    }
    
    _set_x0_u0( dspfx_sub_q31(in0tmp_re, in0tmp_im));
    _set_x0_u0(0);

    _set_ax1(offs_output + 2*(length/2 - 1));
    _set_mx10(1);
    _set_mx11(AM_OFFS(-3));

    for (i = 1; i < length/2; i++)
    {
        _set_x0_u0(         _x1_u0());
        _set_x0_u0( dspfx_neg_q31(_x1_u1()));
    }
};

// Produce instance with: MAC24x24; pIn0 - X; 

#pragma Code("dsplib_dsp_rfft_postprocess_q31_XtoY_cfX_MAC24x24")

/**
 *  Real FFT postprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_rfft_postprocess_q31_XtoY_cfX_MAC24x24(const rfft_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    uint32_t i;
    uint32_t length = inst->length;
    int32_t in0tmp_re, in0tmp_im;
    int32_t Are, Aim,  Bre , Bim;
    q31_t xCos, xSin;
  

    _set_MACMODE(7UL<<16);
    _set_ax2(inst->xyptr_twd);
    _set_mx20(1);
    _set_ax0(offs_input);
    _set_mx00(1);
    _set_ay0(offs_output);
    _set_my00(1);

    xCos = ((q31_t)_x2_u0())>>1;
    xSin = ((q31_t)_x2_u0())>>1;

    Are = dspfx_add_q31(0x3fffffff,xSin);
    Bre = dspfx_sub_q31(0x3fffffff,xSin);
    Aim = xCos;
    Bim = dspfx_neg_q31(xCos);
 
    in0tmp_re = _x0_u0();
    in0tmp_im = _x0_u0();

    _mulrt(in0tmp_re, Are); 
    _macrt(in0tmp_re, Bre); 
    _macrt(in0tmp_im, Bim);

    _set_y0_u0(_macrt(in0tmp_im, dspfx_neg_q31(Aim)));
    _set_y0_u0(0);

    _set_ax1(offs_input + length-1);
    _set_mx10(AM_OFFS(-1));

    for (i = 1; i<length / 2; i++)
    {
        q31_t xCos = ((q31_t)_x2_u0())>>1;
        q31_t xSin = ((q31_t)_x2_u0())>>1;
        int32_t Are = dspfx_add_q31(0x3fffffff, xSin);
        int32_t Aim = xCos;
        int32_t neg_Aim = dspfx_neg_q31(Aim);
        int32_t Bre = dspfx_sub_q31(0x3fffffff, xSin);
        int32_t neg_Bre = dspfx_neg_q31(Bre);
        int32_t Bim = dspfx_neg_q31(xCos);

        int32_t input2i   = _x0_u0();
        int32_t input2i1  = _x0_u0();
        int32_t inputm2i1 = _x1_u0();
        int32_t inputm2i  = _x1_u0();

                    _mulrt(input2i, Are);
                    _macrt(input2i1, neg_Aim);
                    _macrt(inputm2i, Bre);
        _set_y0_u0(_macrt(inputm2i1, Bim));

                    _mulrt(input2i1, Are);
                    _macrt(input2i, Aim);
                    _macrt(inputm2i, Bim);
        _set_y0_u0(_macrt(inputm2i1, neg_Bre));
    }
    
    _set_y0_u0( dspfx_sub_q31(in0tmp_re, in0tmp_im));
    _set_y0_u0(0);

    _set_ay1(offs_output + 2*(length/2 - 1));
    _set_my10(1);
    _set_my11(AM_OFFS(-3));

    for (i = 1; i < length/2; i++)
    {
        _set_y0_u0(         _y1_u0());
        _set_y0_u0( dspfx_neg_q31(_y1_u1()));
    }
};

// Produce instance with: MAC24x24; pIn0 - Y; 

#pragma Code("dsplib_dsp_rfft_postprocess_q31_YtoX_cfY_MAC24x24")

/**
 *  Real FFT postprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_rfft_postprocess_q31_YtoX_cfY_MAC24x24(const rfft_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    uint32_t i;
    uint32_t length = inst->length;
    int32_t in0tmp_re, in0tmp_im;
    int32_t Are, Aim,  Bre , Bim;
    q31_t xCos, xSin;
  

    _set_MACMODE(7UL<<16);
    _set_ay2(inst->xyptr_twd);
    _set_my20(1);
    _set_ay0(offs_input);
    _set_my00(1);
    _set_ax0(offs_output);
    _set_mx00(1);

    xCos = ((q31_t)_y2_u0())>>1;
    xSin = ((q31_t)_y2_u0())>>1;

    Are = dspfx_add_q31(0x3fffffff,xSin);
    Bre = dspfx_sub_q31(0x3fffffff,xSin);
    Aim = xCos;
    Bim = dspfx_neg_q31(xCos);
 
    in0tmp_re = _y0_u0();
    in0tmp_im = _y0_u0();

    _mulrt(in0tmp_re, Are); 
    _macrt(in0tmp_re, Bre); 
    _macrt(in0tmp_im, Bim);

    _set_x0_u0(_macrt(in0tmp_im, dspfx_neg_q31(Aim)));
    _set_x0_u0(0);

    _set_ay1(offs_input + length-1);
    _set_my10(AM_OFFS(-1));

    for (i = 1; i<length / 2; i++)
    {
        q31_t xCos = ((q31_t)_y2_u0())>>1;
        q31_t xSin = ((q31_t)_y2_u0())>>1;
        int32_t Are = dspfx_add_q31(0x3fffffff, xSin);
        int32_t Aim = xCos;
        int32_t neg_Aim = dspfx_neg_q31(Aim);
        int32_t Bre = dspfx_sub_q31(0x3fffffff, xSin);
        int32_t neg_Bre = dspfx_neg_q31(Bre);
        int32_t Bim = dspfx_neg_q31(xCos);

        int32_t input2i   = _y0_u0();
        int32_t input2i1  = _y0_u0();
        int32_t inputm2i1 = _y1_u0();
        int32_t inputm2i  = _y1_u0();

                    _mulrt(input2i, Are);
                    _macrt(input2i1, neg_Aim);
                    _macrt(inputm2i, Bre);
        _set_x0_u0(_macrt(inputm2i1, Bim));

                    _mulrt(input2i1, Are);
                    _macrt(input2i, Aim);
                    _macrt(inputm2i, Bim);
        _set_x0_u0(_macrt(inputm2i1, neg_Bre));
    }
    
    _set_x0_u0( dspfx_sub_q31(in0tmp_re, in0tmp_im));
    _set_x0_u0(0);

    _set_ax1(offs_output + 2*(length/2 - 1));
    _set_mx10(1);
    _set_mx11(AM_OFFS(-3));

    for (i = 1; i < length/2; i++)
    {
        _set_x0_u0(         _x1_u0());
        _set_x0_u0( dspfx_neg_q31(_x1_u1()));
    }
};


// Produce instance with: MAC32x32; pIn0 - X; 

#pragma Code("dsplib_dsp_rfft_postprocess_init_q31_cfX")

/**
 *  Real FFT postprocessing initialization function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_twd - XY memory offset to twiddles
 */

dsp_status dsp_rfft_postprocess_init_q31_cfX(rfft_q31_xy_t *inst, uint32_t length, uint32_t offs_twd)  
{
    uint32_t order = 30-dspfx_norm_q31(length);

    inst->length = length;

    inst->xyptr_twd = offs_twd;

    _BurstSMemToXMem(
        inst->xyptr_twd,
        (signed long *)(void*)((q31_t*)dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER]),
        length);

    return DSP_ERR_OK;
};

// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_rfft_postprocess_init_q31_cfY")

/**
 *  Real FFT postprocessing initialization function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_twd - XY memory offset to twiddles
 */

dsp_status dsp_rfft_postprocess_init_q31_cfY(rfft_q31_xy_t *inst, uint32_t length, uint32_t offs_twd)  
{
    uint32_t order = 30-dspfx_norm_q31(length);

    inst->length = length;

    inst->xyptr_twd = offs_twd;

    _BurstSMemToYMem(
        inst->xyptr_twd,
        (signed long *)(void*)((q31_t*)dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER]),
        length);

    return DSP_ERR_OK;
};


#pragma Code()

#endif

