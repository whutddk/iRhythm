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

// Produce instance with: MAC16x16; pCoeff - X; 

#pragma Code("dsplib_dsp_dct4_preprocess_init_q15_cfX")

/**
 *  DCT Type IV preprocessing initialization function for q15 data type
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_precoeffs
 */

void dsp_dct4_preprocess_init_q15_cfX(dct4_q15_xy_t *inst, int32_t length, int32_t offs_precoeffs)  
{
    int32_t order = 30 - dspfx_norm_q31(length);
    inst->xyptr_prerot = offs_precoeffs;
    inst->length = length;
    _BurstSMemToXMem(offs_precoeffs>>1, (void*)dsp_dct4_q15_prerot_tab[order - DSP_MIN_TRANSFORM_ORDER], (length>>1));
}

// Produce instance with: MAC16x16; pCoeff - Y; 

#pragma Code("dsplib_dsp_dct4_preprocess_init_q15_cfY")

/**
 *  DCT Type IV preprocessing initialization function for q15 data type
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_precoeffs
 */

void dsp_dct4_preprocess_init_q15_cfY(dct4_q15_xy_t *inst, int32_t length, int32_t offs_precoeffs)  
{
    int32_t order = 30 - dspfx_norm_q31(length);
    inst->xyptr_prerot = offs_precoeffs;
    inst->length = length;
    _BurstSMemToYMem(offs_precoeffs>>1, (void*)dsp_dct4_q15_prerot_tab[order - DSP_MIN_TRANSFORM_ORDER], (length>>1));
}



// Produce instance with: MAC16x16; pIn0 - X; 

#pragma Code("dsplib_dsp_dct4_preprocess_q15_XtoX_cfY_MAC16x16")

/**
 *  DCT Type IV preprocessing function for q15 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct4_preprocess_q15_XtoX_cfY_MAC16x16(const dct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i;
    int32_t N = (inst->length) >> 2;

    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    _set_ay0(inst->xyptr_prerot);
    _set_my00(AM_16BIT|1);
    _set_ax0(offs_input);
    _set_mx00(AM_16BIT|1);
    _set_ax1(offs_input  + N*4 - 1);
    _set_mx10(AM_16BIT|AM_OFFS(-1));
    _set_ax2(offs_output);
    _set_mx20(AM_16BIT|1);
    _set_ax3(offs_output + N*4 - 1);
    _set_mx30(AM_16BIT|AM_OFFS(-1));

    for (i = 0; i < N; i++)
    {
        int32_t xCos = _y0_u0();
        int32_t negSin = dspfx_neg_q31(_y0_nu());

        int32_t tmp0_re = _x0_u0();
        int32_t tmp0_im = _x0_u0();
        int32_t tmp1_im = _x1_u0(); // read imag (due to reversed counting)
        int32_t tmp1_re = _x1_u0(); // read real

                    _mulrdw(tmp1_im, _y0_u0()); // *xSin
        _set_x2_u0( _macrdw(tmp0_re, xCos));
                    _mulrdw(tmp1_im, xCos);
        _set_x2_u0( _macrdw(tmp0_re, negSin));

        xCos = _y0_u0(); 
        negSin = dspfx_neg_q31(_y0_nu());

                    _mulrdw(tmp0_im, xCos);
        _set_x3_u0( _macrdw(tmp1_re, negSin));
                    _mulrdw(tmp0_im, _y0_u0()); // *xSin
        _set_x3_u0( _macrdw(tmp1_re, xCos));
    }    
}

// Produce instance with: MAC16x16; pIn0 - Y; 

#pragma Code("dsplib_dsp_dct4_preprocess_q15_YtoY_cfX_MAC16x16")

/**
 *  DCT Type IV preprocessing function for q15 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct4_preprocess_q15_YtoY_cfX_MAC16x16(const dct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i;
    int32_t N = (inst->length) >> 2;

    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    _set_ax0(inst->xyptr_prerot);
    _set_mx00(AM_16BIT|1);
    _set_ay0(offs_input);
    _set_my00(AM_16BIT|1);
    _set_ay1(offs_input  + N*4 - 1);
    _set_my10(AM_16BIT|AM_OFFS(-1));
    _set_ay2(offs_output);
    _set_my20(AM_16BIT|1);
    _set_ay3(offs_output + N*4 - 1);
    _set_my30(AM_16BIT|AM_OFFS(-1));

    for (i = 0; i < N; i++)
    {
        int32_t xCos = _x0_u0();
        int32_t negSin = dspfx_neg_q31(_x0_nu());

        int32_t tmp0_re = _y0_u0();
        int32_t tmp0_im = _y0_u0();
        int32_t tmp1_im = _y1_u0(); // read imag (due to reversed counting)
        int32_t tmp1_re = _y1_u0(); // read real

                    _mulrdw(tmp1_im, _x0_u0()); // *xSin
        _set_y2_u0( _macrdw(tmp0_re, xCos));
                    _mulrdw(tmp1_im, xCos);
        _set_y2_u0( _macrdw(tmp0_re, negSin));

        xCos = _x0_u0(); 
        negSin = dspfx_neg_q31(_x0_nu());

                    _mulrdw(tmp0_im, xCos);
        _set_y3_u0( _macrdw(tmp1_re, negSin));
                    _mulrdw(tmp0_im, _x0_u0()); // *xSin
        _set_y3_u0( _macrdw(tmp1_re, xCos));
    }    
}


// Produce instance with: MAC16x16; pCoeff - X; 

#pragma Code("dsplib_dsp_dct4_postprocess_init_q15_cfX")

/**
 *  DCT Type IV postprocessing initialization function for q15 data type
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_postcoeffs
 */

void dsp_dct4_postprocess_init_q15_cfX(dct4_q15_xy_t *inst, int32_t length, int32_t offs_postcoeffs)  
{
    int32_t order = 30 - dspfx_norm_q31(length);
    inst->xyptr_postrot = offs_postcoeffs;
    inst->length = length;
    _BurstSMemToXMem(offs_postcoeffs>>1, (void*)dsp_dct4_q15_postrot_tab[order - DSP_MIN_TRANSFORM_ORDER], length>>1);
}

// Produce instance with: MAC16x16; pCoeff - Y; 

#pragma Code("dsplib_dsp_dct4_postprocess_init_q15_cfY")

/**
 *  DCT Type IV postprocessing initialization function for q15 data type
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_postcoeffs
 */

void dsp_dct4_postprocess_init_q15_cfY(dct4_q15_xy_t *inst, int32_t length, int32_t offs_postcoeffs)  
{
    int32_t order = 30 - dspfx_norm_q31(length);
    inst->xyptr_postrot = offs_postcoeffs;
    inst->length = length;
    _BurstSMemToYMem(offs_postcoeffs>>1, (void*)dsp_dct4_q15_postrot_tab[order - DSP_MIN_TRANSFORM_ORDER], length>>1);
}



// Produce instance with: MAC16x16; pIn0 - X; 

#pragma Code("dsplib_dsp_dct4_postprocess_q15_XtoX_cfY_MAC16x16")

/**
 *  DCT Type IV postprocessing function for q15 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct4_postprocess_q15_XtoX_cfY_MAC16x16(const dct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i;
    int32_t N = (inst->length) >> 2;
    int32_t xCos, xSin;

    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    _set_ay0(inst->xyptr_postrot);
    _set_my00(AM_16BIT|1);
    _set_ax0(offs_input);
    _set_mx00(AM_16BIT|1);
    _set_ax1(offs_input  + N*4 - 1);
    _set_mx10(AM_16BIT|AM_OFFS(-1));
    _set_ax2(offs_output);
    _set_mx20(AM_16BIT|1);
    _set_ax3(offs_output + N*4 - 1);
    _set_mx30(AM_16BIT|AM_OFFS(-1));

    xCos = dsp_q31_cast_rnd_f32( 1.0000000000);
    xSin = dsp_q31_cast_rnd_f32( 0.0000000000);

    for (i = 0; i < N; i++)
    {
        int32_t tmp0_re = _x0_u0();
        int32_t tmp0_im = _x0_u0();
        int32_t tmp1_im = _x1_u0();
        int32_t tmp1_re = _x1_u0();

                    _mulrdw(tmp0_re, xCos); 
        _set_x2_u0( _macrdw(tmp0_im, xSin));
                    _mulrdw(tmp0_re, xSin);
        _set_x3_u0( _macrdw(tmp0_im, dspfx_neg_q31(xCos)));

        xCos = _y0_u0();
        xSin = _y0_u0();

                    _mulrdw(tmp1_re, xSin); // *xSin
        _set_x3_u0( _macrdw(tmp1_im, xCos));
                    _mulrdw(tmp1_re, xCos);
        _set_x2_u0( _macrdw(tmp1_im, dspfx_neg_q31(xSin)));
    }    
}

// Produce instance with: MAC16x16; pIn0 - Y; 

#pragma Code("dsplib_dsp_dct4_postprocess_q15_YtoY_cfX_MAC16x16")

/**
 *  DCT Type IV postprocessing function for q15 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - XY memory offset to input vector
 * \param offs_output[out] - XY memory offset to output vector
 */

void dsp_dct4_postprocess_q15_YtoY_cfX_MAC16x16(const dct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i;
    int32_t N = (inst->length) >> 2;
    int32_t xCos, xSin;

    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    _set_ax0(inst->xyptr_postrot);
    _set_mx00(AM_16BIT|1);
    _set_ay0(offs_input);
    _set_my00(AM_16BIT|1);
    _set_ay1(offs_input  + N*4 - 1);
    _set_my10(AM_16BIT|AM_OFFS(-1));
    _set_ay2(offs_output);
    _set_my20(AM_16BIT|1);
    _set_ay3(offs_output + N*4 - 1);
    _set_my30(AM_16BIT|AM_OFFS(-1));

    xCos = dsp_q31_cast_rnd_f32( 1.0000000000);
    xSin = dsp_q31_cast_rnd_f32( 0.0000000000);

    for (i = 0; i < N; i++)
    {
        int32_t tmp0_re = _y0_u0();
        int32_t tmp0_im = _y0_u0();
        int32_t tmp1_im = _y1_u0();
        int32_t tmp1_re = _y1_u0();

                    _mulrdw(tmp0_re, xCos); 
        _set_y2_u0( _macrdw(tmp0_im, xSin));
                    _mulrdw(tmp0_re, xSin);
        _set_y3_u0( _macrdw(tmp0_im, dspfx_neg_q31(xCos)));

        xCos = _x0_u0();
        xSin = _x0_u0();

                    _mulrdw(tmp1_re, xSin); // *xSin
        _set_y3_u0( _macrdw(tmp1_im, xCos));
                    _mulrdw(tmp1_re, xCos);
        _set_y2_u0( _macrdw(tmp1_im, dspfx_neg_q31(xSin)));
    }    
}


#endif
