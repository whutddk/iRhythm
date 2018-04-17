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

// Produce instance with: MAC32x32; pCoeff - X; 

#pragma Code("dsplib_dsp_dct4_preprocess_init_q31_cfX")

/**
 *  DCT Type IV preprocessing initialization function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_precoeffs
 */

void dsp_dct4_preprocess_init_q31_cfX(dct4_q31_xy_t *inst, int32_t length, int32_t offs_precoeffs)  
{
    int32_t order = 30 - dspfx_norm_q31(length);
    inst->xyptr_prerot = offs_precoeffs;
    inst->length = length;
    _BurstSMemToXMem(offs_precoeffs, dsp_dct4_q31_prerot_tab[order - DSP_MIN_TRANSFORM_ORDER], length);
}

// Produce instance with: MAC32x32; pCoeff - Y; 

#pragma Code("dsplib_dsp_dct4_preprocess_init_q31_cfY")

/**
 *  DCT Type IV preprocessing initialization function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_precoeffs
 */

void dsp_dct4_preprocess_init_q31_cfY(dct4_q31_xy_t *inst, int32_t length, int32_t offs_precoeffs)  
{
    int32_t order = 30 - dspfx_norm_q31(length);
    inst->xyptr_prerot = offs_precoeffs;
    inst->length = length;
    _BurstSMemToYMem(offs_precoeffs, dsp_dct4_q31_prerot_tab[order - DSP_MIN_TRANSFORM_ORDER], length);
}



// Produce instance with: MAC32x32; pIn0 - X; 

#pragma Code("dsplib_dsp_dct4_preprocess_q31_XtoX_cfY_MAC32x32")

/**
 *  DCT Type IV preprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct4_preprocess_q31_XtoX_cfY_MAC32x32(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i;
    int32_t N = (inst->length) >> 2;

    _set_ay0(inst->xyptr_prerot);
    _set_my00(1);
    _set_ax0(offs_input);
    _set_mx00(1);
    _set_ax1(offs_input  + N*4 - 1);
    _set_mx10(AM_OFFS(-1));
    _set_ax2(offs_output);
    _set_mx20(1);
    _set_ax3(offs_output + N*4 - 1);
    _set_mx30(AM_OFFS(-1));

    for (i = 0; i < N; i++)
    {
        int32_t xCos = _y0_u0();
        int32_t negSin = dspfx_neg_q31(_y0_nu());

        int32_t tmp0_re = _x0_u0();
        int32_t tmp0_im = _x0_u0();
        int32_t tmp1_im = _x1_u0(); // read imag (due to reversed counting)
        int32_t tmp1_re = _x1_u0(); // read real

                     _mulflw(tmp1_im, _y0_nu());
                    _machflw(tmp1_im, _y0_u0()); // *xSin
                    _macflw(tmp0_re, xCos);
        _set_x2_u0( _machflw(tmp0_re, xCos));
                     _mulflw(tmp1_im, xCos);
                    _machflw(tmp1_im, xCos);
                    _macflw(tmp0_re, negSin);
        _set_x2_u0( _machflw(tmp0_re, negSin));

        xCos = _y0_u0(); 
        negSin = dspfx_neg_q31(_y0_nu());

                     _mulflw(tmp0_im, xCos);
                    _machflw(tmp0_im, xCos);
                    _macflw(tmp1_re, negSin);
        _set_x3_u0( _machflw(tmp1_re, negSin));
                     _mulflw(tmp0_im, _y0_nu());
                    _machflw(tmp0_im, _y0_u0()); // *xSin
                    _macflw(tmp1_re, xCos);
        _set_x3_u0( _machflw(tmp1_re, xCos));
    }    
}
// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_dct4_preprocess_q31_YtoY_cfX_MAC32x32")

/**
 *  DCT Type IV preprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct4_preprocess_q31_YtoY_cfX_MAC32x32(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i;
    int32_t N = (inst->length) >> 2;

    _set_ax0(inst->xyptr_prerot);
    _set_mx00(1);
    _set_ay0(offs_input);
    _set_my00(1);
    _set_ay1(offs_input  + N*4 - 1);
    _set_my10(AM_OFFS(-1));
    _set_ay2(offs_output);
    _set_my20(1);
    _set_ay3(offs_output + N*4 - 1);
    _set_my30(AM_OFFS(-1));

    for (i = 0; i < N; i++)
    {
        int32_t xCos = _x0_u0();
        int32_t negSin = dspfx_neg_q31(_x0_nu());

        int32_t tmp0_re = _y0_u0();
        int32_t tmp0_im = _y0_u0();
        int32_t tmp1_im = _y1_u0(); // read imag (due to reversed counting)
        int32_t tmp1_re = _y1_u0(); // read real

                     _mulflw(tmp1_im, _x0_nu());
                    _machflw(tmp1_im, _x0_u0()); // *xSin
                    _macflw(tmp0_re, xCos);
        _set_y2_u0( _machflw(tmp0_re, xCos));
                     _mulflw(tmp1_im, xCos);
                    _machflw(tmp1_im, xCos);
                    _macflw(tmp0_re, negSin);
        _set_y2_u0( _machflw(tmp0_re, negSin));

        xCos = _x0_u0(); 
        negSin = dspfx_neg_q31(_x0_nu());

                     _mulflw(tmp0_im, xCos);
                    _machflw(tmp0_im, xCos);
                    _macflw(tmp1_re, negSin);
        _set_y3_u0( _machflw(tmp1_re, negSin));
                     _mulflw(tmp0_im, _x0_nu());
                    _machflw(tmp0_im, _x0_u0()); // *xSin
                    _macflw(tmp1_re, xCos);
        _set_y3_u0( _machflw(tmp1_re, xCos));
    }    
}
// Produce instance with: MAC24x24; pIn0 - X; 

#pragma Code("dsplib_dsp_dct4_preprocess_q31_XtoX_cfY_MAC24x24")

/**
 *  DCT Type IV preprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct4_preprocess_q31_XtoX_cfY_MAC24x24(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i;
    int32_t N = (inst->length) >> 2;

    _set_MACMODE(7UL<<16);
    _set_ay0(inst->xyptr_prerot);
    _set_my00(1);
    _set_ax0(offs_input);
    _set_mx00(1);
    _set_ax1(offs_input  + N*4 - 1);
    _set_mx10(AM_OFFS(-1));
    _set_ax2(offs_output);
    _set_mx20(1);
    _set_ax3(offs_output + N*4 - 1);
    _set_mx30(AM_OFFS(-1));

    for (i = 0; i < N; i++)
    {
        int32_t xCos = _y0_u0();
        int32_t negSin = dspfx_neg_q31(_y0_nu());

        int32_t tmp0_re = _x0_u0();
        int32_t tmp0_im = _x0_u0();
        int32_t tmp1_im = _x1_u0(); // read imag (due to reversed counting)
        int32_t tmp1_re = _x1_u0(); // read real

                    _mulrt(tmp1_im, _y0_u0()); // *xSin
        _set_x2_u0( _macrt(tmp0_re, xCos));
                    _mulrt(tmp1_im, xCos);
        _set_x2_u0( _macrt(tmp0_re, negSin));

        xCos = _y0_u0(); 
        negSin = dspfx_neg_q31(_y0_nu());

                    _mulrt(tmp0_im, xCos);
        _set_x3_u0( _macrt(tmp1_re, negSin));
                    _mulrt(tmp0_im, _y0_u0()); // *xSin
        _set_x3_u0( _macrt(tmp1_re, xCos));
    }    
}
// Produce instance with: MAC24x24; pIn0 - Y; 

#pragma Code("dsplib_dsp_dct4_preprocess_q31_YtoY_cfX_MAC24x24")

/**
 *  DCT Type IV preprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct4_preprocess_q31_YtoY_cfX_MAC24x24(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i;
    int32_t N = (inst->length) >> 2;

    _set_MACMODE(7UL<<16);
    _set_ax0(inst->xyptr_prerot);
    _set_mx00(1);
    _set_ay0(offs_input);
    _set_my00(1);
    _set_ay1(offs_input  + N*4 - 1);
    _set_my10(AM_OFFS(-1));
    _set_ay2(offs_output);
    _set_my20(1);
    _set_ay3(offs_output + N*4 - 1);
    _set_my30(AM_OFFS(-1));

    for (i = 0; i < N; i++)
    {
        int32_t xCos = _x0_u0();
        int32_t negSin = dspfx_neg_q31(_x0_nu());

        int32_t tmp0_re = _y0_u0();
        int32_t tmp0_im = _y0_u0();
        int32_t tmp1_im = _y1_u0(); // read imag (due to reversed counting)
        int32_t tmp1_re = _y1_u0(); // read real

                    _mulrt(tmp1_im, _x0_u0()); // *xSin
        _set_y2_u0( _macrt(tmp0_re, xCos));
                    _mulrt(tmp1_im, xCos);
        _set_y2_u0( _macrt(tmp0_re, negSin));

        xCos = _x0_u0(); 
        negSin = dspfx_neg_q31(_x0_nu());

                    _mulrt(tmp0_im, xCos);
        _set_y3_u0( _macrt(tmp1_re, negSin));
                    _mulrt(tmp0_im, _x0_u0()); // *xSin
        _set_y3_u0( _macrt(tmp1_re, xCos));
    }    
}

// Produce instance with: MAC32x32; pCoeff - X; 

#pragma Code("dsplib_dsp_dct4_postprocess_init_q31_cfX")

/**
 *  DCT Type IV postprocessing initialization function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_postcoeffs
 */

void dsp_dct4_postprocess_init_q31_cfX(dct4_q31_xy_t *inst, int32_t length, int32_t offs_postcoeffs)  
{
    int32_t order = 30 - dspfx_norm_q31(length);
    inst->xyptr_postrot = offs_postcoeffs;
    inst->length = length;
    _BurstSMemToXMem(offs_postcoeffs, dsp_dct4_q31_postrot_tab[order - DSP_MIN_TRANSFORM_ORDER], length>>1);
}

// Produce instance with: MAC32x32; pCoeff - Y; 

#pragma Code("dsplib_dsp_dct4_postprocess_init_q31_cfY")

/**
 *  DCT Type IV postprocessing initialization function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_postcoeffs
 */

void dsp_dct4_postprocess_init_q31_cfY(dct4_q31_xy_t *inst, int32_t length, int32_t offs_postcoeffs)  
{
    int32_t order = 30 - dspfx_norm_q31(length);
    inst->xyptr_postrot = offs_postcoeffs;
    inst->length = length;
    _BurstSMemToYMem(offs_postcoeffs, dsp_dct4_q31_postrot_tab[order - DSP_MIN_TRANSFORM_ORDER], length>>1);
}



// Produce instance with: MAC32x32; pIn0 - X; 

#pragma Code("dsplib_dsp_dct4_postprocess_q31_XtoX_cfY_MAC32x32")

/**
 *  DCT Type IV postprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct4_postprocess_q31_XtoX_cfY_MAC32x32(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i;
    int32_t N = (inst->length) >> 2;
    int32_t xCos, xSin;

    _set_ay0(inst->xyptr_postrot);
    _set_my00(1);
    _set_ax0(offs_input);
    _set_mx00(1);
    _set_ax1(offs_input  + N*4 - 1);
    _set_mx10(AM_OFFS(-1));
    _set_ax2(offs_output);
    _set_mx20(1);
    _set_ax3(offs_output + N*4 - 1);
    _set_mx30(AM_OFFS(-1));

    xCos = dsp_q31_cast_rnd_f32( 1.0000000000);
    xSin = dsp_q31_cast_rnd_f32( 0.0000000000);

    for (i = 0; i < N; i++)
    {
        int32_t tmp0_re = _x0_u0();
        int32_t tmp0_im = _x0_u0();
        int32_t tmp1_im = _x1_u0();
        int32_t tmp1_re = _x1_u0();

                     _mulflw(tmp0_re, xCos);
                    _machflw(tmp0_re, xCos); 
                    _macflw(tmp0_im, xSin);
        _set_x2_u0( _machflw(tmp0_im, xSin));
                     _mulflw(tmp0_re, xSin);
                    _machflw(tmp0_re, xSin);
                    _macflw(tmp0_im, dspfx_neg_q31(xCos));
        _set_x3_u0( _machflw(tmp0_im, dspfx_neg_q31(xCos)));

        xCos = _y0_u0();
        xSin = _y0_u0();

                     _mulflw(tmp1_re, xSin);
                    _machflw(tmp1_re, xSin); // *xSin
                    _macflw(tmp1_im, xCos);
        _set_x3_u0( _machflw(tmp1_im, xCos));
                     _mulflw(tmp1_re, xCos);
                    _machflw(tmp1_re, xCos);
                    _macflw(tmp1_im, dspfx_neg_q31(xSin));
        _set_x2_u0( _machflw(tmp1_im, dspfx_neg_q31(xSin)));
    }    
}

// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_dct4_postprocess_q31_YtoY_cfX_MAC32x32")

/**
 *  DCT Type IV postprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct4_postprocess_q31_YtoY_cfX_MAC32x32(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i;
    int32_t N = (inst->length) >> 2;
    int32_t xCos, xSin;

    _set_ax0(inst->xyptr_postrot);
    _set_mx00(1);
    _set_ay0(offs_input);
    _set_my00(1);
    _set_ay1(offs_input  + N*4 - 1);
    _set_my10(AM_OFFS(-1));
    _set_ay2(offs_output);
    _set_my20(1);
    _set_ay3(offs_output + N*4 - 1);
    _set_my30(AM_OFFS(-1));

    xCos = dsp_q31_cast_rnd_f32( 1.0000000000);
    xSin = dsp_q31_cast_rnd_f32( 0.0000000000);

    for (i = 0; i < N; i++)
    {
        int32_t tmp0_re = _y0_u0();
        int32_t tmp0_im = _y0_u0();
        int32_t tmp1_im = _y1_u0();
        int32_t tmp1_re = _y1_u0();

                     _mulflw(tmp0_re, xCos);
                    _machflw(tmp0_re, xCos); 
                    _macflw(tmp0_im, xSin);
        _set_y2_u0( _machflw(tmp0_im, xSin));
                     _mulflw(tmp0_re, xSin);
                    _machflw(tmp0_re, xSin);
                    _macflw(tmp0_im, dspfx_neg_q31(xCos));
        _set_y3_u0( _machflw(tmp0_im, dspfx_neg_q31(xCos)));

        xCos = _x0_u0();
        xSin = _x0_u0();

                     _mulflw(tmp1_re, xSin);
                    _machflw(tmp1_re, xSin); // *xSin
                    _macflw(tmp1_im, xCos);
        _set_y3_u0( _machflw(tmp1_im, xCos));
                     _mulflw(tmp1_re, xCos);
                    _machflw(tmp1_re, xCos);
                    _macflw(tmp1_im, dspfx_neg_q31(xSin));
        _set_y2_u0( _machflw(tmp1_im, dspfx_neg_q31(xSin)));
    }    
}

// Produce instance with: MAC24x24; pIn0 - X; 

#pragma Code("dsplib_dsp_dct4_postprocess_q31_XtoX_cfY_MAC24x24")

/**
 *  DCT Type IV postprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct4_postprocess_q31_XtoX_cfY_MAC24x24(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i;
    int32_t N = (inst->length) >> 2;
    int32_t xCos, xSin;

    _set_MACMODE(7UL<<16);
    _set_ay0(inst->xyptr_postrot);
    _set_my00(1);
    _set_ax0(offs_input);
    _set_mx00(1);
    _set_ax1(offs_input  + N*4 - 1);
    _set_mx10(AM_OFFS(-1));
    _set_ax2(offs_output);
    _set_mx20(1);
    _set_ax3(offs_output + N*4 - 1);
    _set_mx30(AM_OFFS(-1));

    xCos = dsp_q31_cast_rnd_f32( 1.0000000000);
    xSin = dsp_q31_cast_rnd_f32( 0.0000000000);

    for (i = 0; i < N; i++)
    {
        int32_t tmp0_re = _x0_u0();
        int32_t tmp0_im = _x0_u0();
        int32_t tmp1_im = _x1_u0();
        int32_t tmp1_re = _x1_u0();

                    _mulrt(tmp0_re, xCos); 
        _set_x2_u0( _macrt(tmp0_im, xSin));
                    _mulrt(tmp0_re, xSin);
        _set_x3_u0( _macrt(tmp0_im, dspfx_neg_q31(xCos)));

        xCos = _y0_u0();
        xSin = _y0_u0();

                    _mulrt(tmp1_re, xSin); // *xSin
        _set_x3_u0( _macrt(tmp1_im, xCos));
                    _mulrt(tmp1_re, xCos);
        _set_x2_u0( _macrt(tmp1_im, dspfx_neg_q31(xSin)));
    }    
}

// Produce instance with: MAC24x24; pIn0 - Y; 

#pragma Code("dsplib_dsp_dct4_postprocess_q31_YtoY_cfX_MAC24x24")

/**
 *  DCT Type IV postprocessing function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct4_postprocess_q31_YtoY_cfX_MAC24x24(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i;
    int32_t N = (inst->length) >> 2;
    int32_t xCos, xSin;

    _set_MACMODE(7UL<<16);
    _set_ax0(inst->xyptr_postrot);
    _set_mx00(1);
    _set_ay0(offs_input);
    _set_my00(1);
    _set_ay1(offs_input  + N*4 - 1);
    _set_my10(AM_OFFS(-1));
    _set_ay2(offs_output);
    _set_my20(1);
    _set_ay3(offs_output + N*4 - 1);
    _set_my30(AM_OFFS(-1));

    xCos = dsp_q31_cast_rnd_f32( 1.0000000000);
    xSin = dsp_q31_cast_rnd_f32( 0.0000000000);

    for (i = 0; i < N; i++)
    {
        int32_t tmp0_re = _y0_u0();
        int32_t tmp0_im = _y0_u0();
        int32_t tmp1_im = _y1_u0();
        int32_t tmp1_re = _y1_u0();

                    _mulrt(tmp0_re, xCos); 
        _set_y2_u0( _macrt(tmp0_im, xSin));
                    _mulrt(tmp0_re, xSin);
        _set_y3_u0( _macrt(tmp0_im, dspfx_neg_q31(xCos)));

        xCos = _x0_u0();
        xSin = _x0_u0();

                    _mulrt(tmp1_re, xSin); // *xSin
        _set_y3_u0( _macrt(tmp1_im, xCos));
                    _mulrt(tmp1_re, xCos);
        _set_y2_u0( _macrt(tmp1_im, dspfx_neg_q31(xSin)));
    }    
}


#endif
