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
#include "dsplib_ll_api.h"
#include "dsp_transform_tables.h"

// Produce instance with: MAC32x32; pIn - X; 

#pragma Code("dsplib_dsp_dct3_preprocess_init_q31_cfX")

/**
 *  DCT Type III preprocessing initialization function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_twd - XY memory offset to twiddle table. length: 3/4*length q31_data
 */

dsp_status dsp_dct3_preprocess_init_q31_cfX(dct3_q31_xy_t *inst, uint32_t length, uint32_t offs_twd)   
{
    uint32_t order = 30-dspfx_norm_q31(length);
    inst->length = length;

    inst->xyptr_twd  = offs_twd;
    inst->xyptr_twd4 = offs_twd + (length/2);

    _BurstSMemToXMem(
        inst->xyptr_twd, 
        ((int32_t*)(void*)(dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER]))+2, 
        (length/2));
    _BurstSMemToXMem(
        inst->xyptr_twd4, 
        (int32_t*)(void*)(dsp_dct23_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER]),
        length);
    return 0;
};
// Produce instance with: MAC32x32; pIn - Y; 

#pragma Code("dsplib_dsp_dct3_preprocess_init_q31_cfY")

/**
 *  DCT Type III preprocessing initialization function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_twd - XY memory offset to twiddle table. length: 3/4*length q31_data
 */

dsp_status dsp_dct3_preprocess_init_q31_cfY(dct3_q31_xy_t *inst, uint32_t length, uint32_t offs_twd)   
{
    uint32_t order = 30-dspfx_norm_q31(length);
    inst->length = length;

    inst->xyptr_twd  = offs_twd;
    inst->xyptr_twd4 = offs_twd + (length/2);

    _BurstSMemToYMem(
        inst->xyptr_twd, 
        ((int32_t*)(void*)(dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER]))+2, 
        (length/2));
    _BurstSMemToYMem(
        inst->xyptr_twd4, 
        (int32_t*)(void*)(dsp_dct23_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER]),
        length);
    return 0;
};

// Produce instance with: MAC32x32; pIn - X; 

#pragma Code("dsplib_dsp_dct3_preprocess_q31_XtoY_cfX_MAC32x32")

/**
 *  DCT Type III preprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct3_preprocess_q31_XtoY_cfX_MAC32x32(const dct3_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output) 
{
    int32_t i;
    int32_t L = inst->length;
    int32_t N = inst->length >> 1;
    int32_t xSin, xCos;
    q31_t in_temp;


    _set_ay0(offs_output + 2);
    _set_my00(1);
    _set_ax0(offs_input + 0);
    _set_mx00(1);
    _set_ax1(offs_input + L - 1);
    _set_mx10(AM_OFFS(-1));
    _set_ax3(inst->xyptr_twd4);
    _set_mx30(1);


    in_temp = _x0_u0();

    for (i = 1; i < N; i++) 
    {
        xCos = _x3_u0();
        xSin = _x3_u0();

         _mulflw(_x0_nu(), xCos);
        _machflw(_x0_nu(), xCos);
                    _macflw(_x1_nu(),xSin);
        _set_y0_u0(_machflw(_x1_nu(),xSin));
         _mulflw(_x1_nu(), xCos);
        _machflw(_x1_u0(), xCos);
                    _macflw(_x0_nu(), dspfx_neg_q31(xSin));
        _set_y0_u0(_machflw(_x0_u0(), dspfx_neg_q31(xSin)));
    }

    _set_ay0(offs_output);
    _set_my00(1);
    _set_my01(AM_OFFS(-1));
                _mulflw(_x0_nu(), _x3_nu());
    _set_y0_u0( dspfx_add_q31(in_temp, _machflw(_x0_nu(), _x3_nu())));
                _mulflw(_x0_nu(), _x3_nu());
    _set_y0_u0( dspfx_sub_q31(in_temp, _machflw(_x0_nu(), _x3_nu())));
    _set_ay1(offs_output + 2*N-1);
    _set_my10(AM_OFFS(-1));
    _set_my11(1);
 
    _set_ax2(inst->xyptr_twd);
    _set_mx20(1);

    for (i = 1; i < N>>1; i++) {
        int32_t a1, a2, a3, a4, a5, a6;

        int32_t tmp2i  = _y0_u0();
        int32_t tmp2i1 = _y0_u1();
        int32_t tmp2Ni1= _y1_u0();
        int32_t tmp2Ni = _y1_u1();

        xCos = dspfx_neg_q31(_x2_u0());
        xSin = dspfx_neg_q31(_x2_u0());

        a1 = dspfx_add_q31(tmp2i,  tmp2Ni)>>1;
        a2 = dspfx_add_q31(tmp2i1, tmp2Ni1)>>1;
        a3 = dspfx_sub_q31(tmp2Ni, tmp2i)>>1;
        a4 = dspfx_sub_q31(tmp2i1, tmp2Ni1)>>1;

              _mulflw(a2, xCos);
             _machflw(a2, xCos);
              _macflw(a3, xSin);
        a5 = _machflw(a3, xSin);
              _mulflw(a2, xSin);
             _machflw(a2, xSin);
              _macflw(a3, dspfx_neg_q31(xCos));
        a6 = _machflw(a3, dspfx_neg_q31(xCos));


        _set_y0_u0((dspfx_sub_q31(a1, a5)));
        _set_y0_u0((dspfx_add_q31(a6, a4)));
        _set_y1_u0((dspfx_sub_q31(a6, a4)));
        _set_y1_u0((dspfx_add_q31(a1, a5)));
    }
}
// Produce instance with: MAC32x32; pIn - Y; 

#pragma Code("dsplib_dsp_dct3_preprocess_q31_YtoX_cfY_MAC32x32")

/**
 *  DCT Type III preprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct3_preprocess_q31_YtoX_cfY_MAC32x32(const dct3_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output) 
{
    int32_t i;
    int32_t L = inst->length;
    int32_t N = inst->length >> 1;
    int32_t xSin, xCos;
    q31_t in_temp;


    _set_ax0(offs_output + 2);
    _set_mx00(1);
    _set_ay0(offs_input + 0);
    _set_my00(1);
    _set_ay1(offs_input + L - 1);
    _set_my10(AM_OFFS(-1));
    _set_ay3(inst->xyptr_twd4);
    _set_my30(1);


    in_temp = _y0_u0();

    for (i = 1; i < N; i++) 
    {
        xCos = _y3_u0();
        xSin = _y3_u0();

         _mulflw(_y0_nu(), xCos);
        _machflw(_y0_nu(), xCos);
                    _macflw(_y1_nu(),xSin);
        _set_x0_u0(_machflw(_y1_nu(),xSin));
         _mulflw(_y1_nu(), xCos);
        _machflw(_y1_u0(), xCos);
                    _macflw(_y0_nu(), dspfx_neg_q31(xSin));
        _set_x0_u0(_machflw(_y0_u0(), dspfx_neg_q31(xSin)));
    }

    _set_ax0(offs_output);
    _set_mx00(1);
    _set_mx01(AM_OFFS(-1));
                _mulflw(_y0_nu(), _y3_nu());
    _set_x0_u0( dspfx_add_q31(in_temp, _machflw(_y0_nu(), _y3_nu())));
                _mulflw(_y0_nu(), _y3_nu());
    _set_x0_u0( dspfx_sub_q31(in_temp, _machflw(_y0_nu(), _y3_nu())));
    _set_ax1(offs_output + 2*N-1);
    _set_mx10(AM_OFFS(-1));
    _set_mx11(1);
 
    _set_ay2(inst->xyptr_twd);
    _set_my20(1);

    for (i = 1; i < N>>1; i++) {
        int32_t a1, a2, a3, a4, a5, a6;

        int32_t tmp2i  = _x0_u0();
        int32_t tmp2i1 = _x0_u1();
        int32_t tmp2Ni1= _x1_u0();
        int32_t tmp2Ni = _x1_u1();

        xCos = dspfx_neg_q31(_y2_u0());
        xSin = dspfx_neg_q31(_y2_u0());

        a1 = dspfx_add_q31(tmp2i,  tmp2Ni)>>1;
        a2 = dspfx_add_q31(tmp2i1, tmp2Ni1)>>1;
        a3 = dspfx_sub_q31(tmp2Ni, tmp2i)>>1;
        a4 = dspfx_sub_q31(tmp2i1, tmp2Ni1)>>1;

              _mulflw(a2, xCos);
             _machflw(a2, xCos);
              _macflw(a3, xSin);
        a5 = _machflw(a3, xSin);
              _mulflw(a2, xSin);
             _machflw(a2, xSin);
              _macflw(a3, dspfx_neg_q31(xCos));
        a6 = _machflw(a3, dspfx_neg_q31(xCos));


        _set_x0_u0((dspfx_sub_q31(a1, a5)));
        _set_x0_u0((dspfx_add_q31(a6, a4)));
        _set_x1_u0((dspfx_sub_q31(a6, a4)));
        _set_x1_u0((dspfx_add_q31(a1, a5)));
    }
}
// Produce instance with: MAC24x24; pIn - X; 

#pragma Code("dsplib_dsp_dct3_preprocess_q31_XtoY_cfX_MAC24x24")

/**
 *  DCT Type III preprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct3_preprocess_q31_XtoY_cfX_MAC24x24(const dct3_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output) 
{
    int32_t i;
    int32_t L = inst->length;
    int32_t N = inst->length >> 1;
    int32_t xSin, xCos;
    q31_t in_temp;


    _set_ay0(offs_output + 2);
    _set_my00(1);
    _set_ax0(offs_input + 0);
    _set_mx00(1);
    _set_ax1(offs_input + L - 1);
    _set_mx10(AM_OFFS(-1));
    _set_ax3(inst->xyptr_twd4);
    _set_mx30(1);

    _set_MACMODE(7UL<<16);

    in_temp = _x0_u0();

    for (i = 1; i < N; i++) 
    {
        xCos = _x3_u0();
        xSin = _x3_u0();

        _mulrt(_x0_nu(), xCos);
        _set_y0_u0(_macrt(_x1_nu(),xSin));
        _mulrt(_x1_u0(), xCos);
        _set_y0_u0(_macrt(_x0_u0(), dspfx_neg_q31(xSin)));
    }

    _set_ay0(offs_output);
    _set_my00(1);
    _set_my01(AM_OFFS(-1));
    _set_y0_u0( dspfx_add_q31(in_temp, _mulrt(_x0_nu(), _x3_nu())));
    _set_y0_u0( dspfx_sub_q31(in_temp, _mulrt(_x0_nu(), _x3_nu())));
    _set_ay1(offs_output + 2*N-1);
    _set_my10(AM_OFFS(-1));
    _set_my11(1);
 
    _set_ax2(inst->xyptr_twd);
    _set_mx20(1);

    for (i = 1; i < N>>1; i++) {
        int32_t a1, a2, a3, a4, a5, a6;

        int32_t tmp2i  = _y0_u0();
        int32_t tmp2i1 = _y0_u1();
        int32_t tmp2Ni1= _y1_u0();
        int32_t tmp2Ni = _y1_u1();

        xCos = dspfx_neg_q31(_x2_u0());
        xSin = dspfx_neg_q31(_x2_u0());

        a1 = dspfx_add_q31(tmp2i,  tmp2Ni)>>1;
        a2 = dspfx_add_q31(tmp2i1, tmp2Ni1)>>1;
        a3 = dspfx_sub_q31(tmp2Ni, tmp2i)>>1;
        a4 = dspfx_sub_q31(tmp2i1, tmp2Ni1)>>1;

             _mulrt(a2, xCos);
        a5 = _macrt(a3, xSin);
             _mulrt(a2, xSin);
        a6 = _macrt(a3, dspfx_neg_q31(xCos));


        _set_y0_u0((dspfx_sub_q31(a1, a5)));
        _set_y0_u0((dspfx_add_q31(a6, a4)));
        _set_y1_u0((dspfx_sub_q31(a6, a4)));
        _set_y1_u0((dspfx_add_q31(a1, a5)));
    }
}
// Produce instance with: MAC24x24; pIn - Y; 

#pragma Code("dsplib_dsp_dct3_preprocess_q31_YtoX_cfY_MAC24x24")

/**
 *  DCT Type III preprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct3_preprocess_q31_YtoX_cfY_MAC24x24(const dct3_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output) 
{
    int32_t i;
    int32_t L = inst->length;
    int32_t N = inst->length >> 1;
    int32_t xSin, xCos;
    q31_t in_temp;


    _set_ax0(offs_output + 2);
    _set_mx00(1);
    _set_ay0(offs_input + 0);
    _set_my00(1);
    _set_ay1(offs_input + L - 1);
    _set_my10(AM_OFFS(-1));
    _set_ay3(inst->xyptr_twd4);
    _set_my30(1);

    _set_MACMODE(7UL<<16);

    in_temp = _y0_u0();

    for (i = 1; i < N; i++) 
    {
        xCos = _y3_u0();
        xSin = _y3_u0();

        _mulrt(_y0_nu(), xCos);
        _set_x0_u0(_macrt(_y1_nu(),xSin));
        _mulrt(_y1_u0(), xCos);
        _set_x0_u0(_macrt(_y0_u0(), dspfx_neg_q31(xSin)));
    }

    _set_ax0(offs_output);
    _set_mx00(1);
    _set_mx01(AM_OFFS(-1));
    _set_x0_u0( dspfx_add_q31(in_temp, _mulrt(_y0_nu(), _y3_nu())));
    _set_x0_u0( dspfx_sub_q31(in_temp, _mulrt(_y0_nu(), _y3_nu())));
    _set_ax1(offs_output + 2*N-1);
    _set_mx10(AM_OFFS(-1));
    _set_mx11(1);
 
    _set_ay2(inst->xyptr_twd);
    _set_my20(1);

    for (i = 1; i < N>>1; i++) {
        int32_t a1, a2, a3, a4, a5, a6;

        int32_t tmp2i  = _x0_u0();
        int32_t tmp2i1 = _x0_u1();
        int32_t tmp2Ni1= _x1_u0();
        int32_t tmp2Ni = _x1_u1();

        xCos = dspfx_neg_q31(_y2_u0());
        xSin = dspfx_neg_q31(_y2_u0());

        a1 = dspfx_add_q31(tmp2i,  tmp2Ni)>>1;
        a2 = dspfx_add_q31(tmp2i1, tmp2Ni1)>>1;
        a3 = dspfx_sub_q31(tmp2Ni, tmp2i)>>1;
        a4 = dspfx_sub_q31(tmp2i1, tmp2Ni1)>>1;

             _mulrt(a2, xCos);
        a5 = _macrt(a3, xSin);
             _mulrt(a2, xSin);
        a6 = _macrt(a3, dspfx_neg_q31(xCos));


        _set_x0_u0((dspfx_sub_q31(a1, a5)));
        _set_x0_u0((dspfx_add_q31(a6, a4)));
        _set_x1_u0((dspfx_sub_q31(a6, a4)));
        _set_x1_u0((dspfx_add_q31(a1, a5)));
    }
}


// Produce instance with: MAC32x32; pIn - X; 

#pragma Code("dsplib_dsp_dct3_postprocess_q31_XtoY")

/**
 *  DCT Type III postprocessing function for q31 data type
 *
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param length - transform size
 */

void dsp_dct3_postprocess_q31_XtoY(uint32_t offs_input, uint32_t offs_output, uint32_t length) 
{
    int32_t i;
    int32_t M = length >> 2;


    _set_ax0(offs_input);
    _set_mx00(1);
    _set_ax1(offs_input + length-1);
    _set_mx10(AM_OFFS(-1));
    _set_ay0(offs_output);
    _set_my00(1);

    for (i = 0; i < M; i++) 
    {
        _set_y0_u0(_x0_u0());
        _set_y0_u0(_x1_u0());
        _set_y0_u0(_x0_u0());
        _set_y0_u0(_x1_u0());
    }
}
// Produce instance with: MAC32x32; pIn - Y; 

#pragma Code("dsplib_dsp_dct3_postprocess_q31_YtoX")

/**
 *  DCT Type III postprocessing function for q31 data type
 *
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 * \param length - transform size
 */

void dsp_dct3_postprocess_q31_YtoX(uint32_t offs_input, uint32_t offs_output, uint32_t length) 
{
    int32_t i;
    int32_t M = length >> 2;


    _set_ay0(offs_input);
    _set_my00(1);
    _set_ay1(offs_input + length-1);
    _set_my10(AM_OFFS(-1));
    _set_ax0(offs_output);
    _set_mx00(1);

    for (i = 0; i < M; i++) 
    {
        _set_x0_u0(_y0_u0());
        _set_x0_u0(_y1_u0());
        _set_x0_u0(_y0_u0());
        _set_x0_u0(_y1_u0());
    }
}

#endif
