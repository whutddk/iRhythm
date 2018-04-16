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

#if !defined(EXCLUDE_DSP_DCT4_Q15)
#pragma Code("dsplib_dsp_dct4_q15")

#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsp_transform_tables.h"

/**
 * DCT Type IV instance initialization function for q15 data type
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_dct4_getsize_q15()" function.
 */

#if !defined(_ARC_BE)

#define _VPACK2HL(a,b) _vpack2hl((a),(b))
#define _VPACK2HM(a,b) _vpack2hm((a),(b))

#define REVERSE_ON_LE (AGU_DT_vector_reverse)

#else

#define _VPACK2HL(a,b) _vpack2hm((b),(a))
#define _VPACK2HM(a,b) _vpack2hl((b),(a))

#define REVERSE_ON_LE (0)

#endif

dsp_status _DSPLIB_XYCC dsp_dct4_init_q15(dct4_q15_t* handle, uint32_t length, PTR(uint32_t) pState)
{
    dsp_status err = DSP_ERR_OK;
    int32_t order = 30-dspfx_norm_q31(length);
    uint8_t* data_ptr = (uint8_t*) (void*) pState;    
    handle->length = length;
    if ((length > DSP_MAX_TRANSFORM_SIZE))
        return DSP_ERR_LENGTH_TOO_BIG;
    if ((length < 8))
        return DSP_ERR_LENGTH_TOO_SMALL;
    if (length != (1 << order))
        return DSP_ERR_LENGTH_NOT_POWER2;
    if (dsp_dct4_q15_prerot_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;
    if (dsp_dct4_q15_postrot_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;

#if defined(_ARC_AS210_mac16)
    if (handle->length > XY_DCT4_Q15_THRESHOLD) 
#endif
    {
#ifdef _ARCv2EM_XY_11D   
        handle->prerot  = (q15_t*) (void*) data_ptr; data_ptr += length * sizeof(q15_t);
        handle->postrot = (q15_t*) (void*) data_ptr; data_ptr += length * sizeof(q15_t) / 2;
        memcpy(handle->prerot,  dsp_dct4_q15_prerot_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(q15_t));
        memcpy(handle->postrot, dsp_dct4_q15_postrot_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(q15_t) / 2);
#else
        handle->prerot  = (q15_t*)dsp_dct4_q15_prerot_tab[order - DSP_MIN_TRANSFORM_ORDER];
        handle->postrot = (q15_t*)dsp_dct4_q15_postrot_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif
        handle->temp_buffer = (q15_t*) (void*) data_ptr; data_ptr += length * sizeof(q15_t);
        err = dsp_cfft_init_q15(&handle->hfft, length/2, (PTR(uint32_t))data_ptr);
    }
    handle->hfft.shiftBits = 0;
    handle->hfft.scaling = DSP_TRANSFORM_SCALING_UNSCALED;
    return err;
};


#if defined(_ARCv2EM_XY_AGU_MEDIUM)

#if defined _XY_HAND_OPT
static _Inline void _DSPLIB_XYCC risc_dsp_dct4_q15(const dct4_q15_t* handle, const q15_t* __restrict input, q15_t* __restrict output)
{
    int32_t i;
    int32_t N = handle->length >> 1;
    uint32_t tmp0, tmp1;
    uint32_t* __restrict prerot_tab_c  =  (uint32_t*)handle->prerot;
    uint32_t* __restrict postrot_tab_c  = (uint32_t*)handle->postrot;

    // select buffer for in-place or out-of-place CFFT transforms
    // out-of-place has better performance characteristics
	q15_t *ptemp = ((uint32_t)handle->temp_buffer) ? handle->temp_buffer : output;

    uint32_t xTw = COMPLEX_CONST_UI32(0x0000, 0x7fff);

    _DSPLIB_store_regs(5, 8, 0);
    INIT_MACMODE();

    _set_agu_ap0((uint32_t)input); // start of input buffer
    _set_agu_ap1((uint32_t)&input[2*N - 2]); //end of input buffer
    _set_agu_ap2((uint32_t)ptemp); // start of output buffer
    _set_agu_ap3((uint32_t)&ptemp[2*N - 2]); //end of output buffer
    _set_agu_ap4((uint32_t)prerot_tab_c); // prerotation coefficients

    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_q15(2)); // input buffer increment
    _set_agu_mod1(1, AGU_DT_v2q15 | REVERSE_ON_LE, AGU_AD_DEC_q15(2)); // input buffer decrement, swap real and imaginary part
    _set_agu_mod2(2, AGU_DT_ui32, AGU_AD_INC_q15(2)); // output buffer increment
    _set_agu_mod3(3, AGU_DT_ui32, AGU_AD_DEC_q15(2)); // output buffer decrement
    _set_agu_mod4(0, AGU_DT_ui32, AGU_AD_INC_q15(0)); // input buffer non-modify
    _set_agu_mod5(1, AGU_DT_v2q15 | REVERSE_ON_LE, AGU_AD_DEC_q15(0)); // input buffer non-modify
    _set_agu_mod6(4, AGU_DT_ui32, AGU_AD_INC_q15(2)); // prerotation coefficients increment

    // DCT4 Prerotation
    for (i = 0; i < N>>1; i++)
    {     
        uint32_t tmp0, tmp1;

        tmp0 = _VPACK2HL(_agu_u5(),_agu_u4());
        tmp1 = _VPACK2HM(_agu_u0(),_agu_u1());
        _st_agu_u2(_cmpychfr(tmp0, _agu_u6()));
        _st_agu_u3(_cmpychfr(tmp1, _agu_u6()));
    }

    dsp_cfft_q15(&handle->hfft, (PTR(q15_t))ptemp, (PTR(q15_t))output);
    
    _set_agu_ap0((uint32_t)output); // start of output buffer
    _set_agu_ap1((uint32_t)&output[2*N - 2]); //end of output buffer
    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_q15(2)); // output buffer increment
    _set_agu_mod1(1, AGU_DT_ui32, AGU_AD_DEC_q15(2)); // output buffer decrement
    _set_agu_mod4(0, AGU_DT_ui32, AGU_AD_INC_q15(0)); // output buffer non-modify
    _set_agu_mod5(1, AGU_DT_ui32, AGU_AD_DEC_q15(0)); // output buffer non-modify

    _set_agu_ap4((uint32_t)postrot_tab_c); // postrotation coefficient
    _set_agu_mod6(4, AGU_DT_ui32, AGU_AD_INC_q15(2)); // postrotation coefficients increment
    _set_agu_mod7(4, AGU_DT_ui32, AGU_AD_INC_q15(0)); // postrotation coefficients non-modify

    // DCT4 Postrotation
    
    tmp0 = _cmpychfr(xTw, _agu_u4());
    tmp1 = _cmpyhfr(_agu_u7(), _agu_u5());
    _st_agu_u0(_VPACK2HL(tmp1, tmp0));
    _st_agu_u1(_VPACK2HM(tmp0, tmp1));
    
    for (i = 1; i < N>>1; i++)             
    {
        tmp0 = _cmpychfr(_agu_u6(), _agu_u4());
        tmp1 = _cmpyhfr(_agu_u7(), _agu_u5());
        _st_agu_u0(_VPACK2HL(tmp1, tmp0));
        _st_agu_u1(_VPACK2HM(tmp0, tmp1));
    }
    _DSPLIB_restore_regs();
}

//#pragma Off_inline(risc_dsp_dct4_q15)

#else

static _Inline void risc_dsp_dct4_q15(const dct4_q15_t* handle, const PTR(q15_t) __restrict input, PTR(q15_t) __restrict output)
{
    int32_t i;
    int32_t N = handle->length >> 1;

    PTR(cq15_t) __restrict prerot_tab_c  = (PTR(cq15_t))handle->prerot;
    PTR(cq15_t) __restrict postrot_tab_c  = (PTR(cq15_t))handle->postrot;
    PTR(cq15_t) __restrict input_c = (PTR(cq15_t)) input;
    PTR(cq15_t) __restrict output_c = (PTR(cq15_t)) output;
    // select buffer for in-place or out-of-place CFFT transforms
    // out-of-place has better performance characteristics
	PTR(q15_t) ptemp = ((uint32_t)handle->temp_buffer) ? (PTR(q15_t))handle->temp_buffer : (PTR(q15_t))output;
    PTR(cq15_t) __restrict temp_c = (PTR(cq15_t)) ptemp;

    cq15_t xTw, in0, in1, tmp0, tmp1;


    // DCT4 Prerotation
    for (i = 0; i < N>>1; i++)
    {
        in0 = input_c[i];
        in1 = input_c[N-i-1];
        tmp0 = fx_create_cq15(fx_get_re_cq15(in0), fx_get_im_cq15(in1));
        tmp1 = fx_create_cq15(fx_get_re_cq15(in1), fx_get_im_cq15(in0));
        temp_c[i] = dspfx_cjmpy_cq15(tmp0, *prerot_tab_c++);
        temp_c[N-i-1] = dspfx_cjmpy_cq15(tmp1, *prerot_tab_c++);
    }

    dsp_cfft_q15(&handle->hfft, ptemp, output);

    // DCT4 Postrotation
    xTw  = fx_create_cq15(0x7fff, 0x0000);

    tmp0 = dspfx_cjmpy_cq15(xTw, output_c[0]);
    tmp1 = dspfx_cmpy_cq15(*postrot_tab_c, output_c[N-1]);
    output_c[0] = fx_create_cq15(fx_get_re_cq15(tmp0), fx_get_re_cq15(tmp1));
    output_c[N-1] = fx_create_cq15(fx_get_im_cq15(tmp1), fx_get_im_cq15(tmp0));

    for (i = 1; i < N>>1; i++)
    {
        tmp0 = dspfx_cjmpy_cq15(*postrot_tab_c++, output_c[i]);
        tmp1 = dspfx_cmpy_cq15(*postrot_tab_c, output_c[N-i-1]);
        output_c[i] = fx_create_cq15(fx_get_re_cq15(tmp0), fx_get_re_cq15(tmp1));
        output_c[N-i-1] = fx_create_cq15(fx_get_im_cq15(tmp1), fx_get_im_cq15(tmp0));
    }
}

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM

#if defined(__FXAPI__) && defined(__Xdsp_complex)
static _Inline void risc_dsp_dct4_q15(const dct4_q15_t* handle, const q15_t* __restrict input, q15_t* __restrict output) 
{
    int32_t i;
    int32_t N = handle->length >> 1;

    cq15_t* __restrict prerot_tab_c  =  (cq15_t*)handle->prerot;
    cq15_t* __restrict postrot_tab_c  =  (cq15_t*)handle->postrot;
    cq15_t* __restrict input_c = (cq15_t*) input;
    cq15_t* __restrict output_c = (cq15_t*) output;
    cq15_t in0, in1, tmp0, tmp1;

    // DCT4 Prerotation
    for (i = 0; i < N>>1; i++)
    {
        in0 = input_c[i];
        in1 = input_c[N-i-1];
        tmp0 = fx_create_cq15(fx_get_re_cq15(in0), fx_get_im_cq15(in1));
        tmp1 = fx_create_cq15(fx_get_re_cq15(in1), fx_get_im_cq15(in0));
        output_c[i] = dspfx_cjmpy_cq15(tmp0, *prerot_tab_c++);
        output_c[N-i-1] = dspfx_cjmpy_cq15(tmp1, *prerot_tab_c++);
    }

    dsp_cfft_q15(&handle->hfft, (PTR(q15_t))output, (PTR(q15_t))output);

    // DCT4 Postrotation
    // first iteration is moved out of the loop
    tmp0 = dspfx_cjmpy_cq15(fx_create_cq15(0x7fff, 0x0000), output_c[0]);
    tmp1 = dspfx_cmpy_cq15(*postrot_tab_c, output_c[N-1]);
    output_c[0] = fx_create_cq15(fx_get_re_cq15(tmp0), fx_get_re_cq15(tmp1));
    output_c[N-1] = fx_create_cq15(fx_get_im_cq15(tmp1), fx_get_im_cq15(tmp0));
    for (i = 1; i < N>>1; i++)
    {
        tmp0 = dspfx_cjmpy_cq15(*postrot_tab_c++, output_c[i]);
        tmp1 = dspfx_cmpy_cq15(*postrot_tab_c, output_c[N-i-1]);
        output_c[i] = fx_create_cq15(fx_get_re_cq15(tmp0), fx_get_re_cq15(tmp1));
        output_c[N-i-1] = fx_create_cq15(fx_get_im_cq15(tmp1), fx_get_im_cq15(tmp0));
    }
}

#else

static _Inline void risc_dsp_dct4_q15(const dct4_q15_t* handle, const q15_t* input, q15_t* output)
{
    int32_t i;
    dspfx_accum32()
        int32_t N = handle->length >> 1;
    q15_t xSin, xCos;
    q15_t* prerot_tab  = handle->prerot;
    q15_t* postrot_tab = handle->postrot;

    INIT_MACMODE();
    // DCT4 Prerotation
    for (i = 0; i < N>>1; i++)
    {
        q15_t tmp0_re = COMPLEX_RE(input, i);
        q15_t tmp0_im = COMPLEX_IM(input, i);
        q15_t tmp1_re = COMPLEX_RE(input, N-i-1);
        q15_t tmp1_im = COMPLEX_IM(input, N-i-1);

        xCos = *prerot_tab++; 
        xSin = *prerot_tab++;
                                dspfx_a32mul_q15( tmp0_re, xCos, acc);
        COMPLEX_RE(output, i) = dspfx_a32mac_q15( tmp1_im, xSin, acc);
                                dspfx_a32mul_q15( tmp1_im, xCos, acc);
        COMPLEX_IM(output, i) = dspfx_a32mac_q15( tmp0_re, dspfx_neg_q15(xSin), acc);

        xCos = *prerot_tab++; 
        xSin = *prerot_tab++;
                                    dspfx_a32mul_q15( tmp0_im, xSin, acc);
        COMPLEX_RE(output, N-i-1) = dspfx_a32mac_q15( tmp1_re, xCos, acc);
                                    dspfx_a32mul_q15( tmp0_im, xCos, acc);
        COMPLEX_IM(output, N-i-1) = dspfx_a32mac_q15( tmp1_re, dspfx_neg_q15(xSin), acc);
    }

    dsp_cfft_q15(&handle->hfft, (PTR(q15_t))output, (PTR(q15_t))output);

    xCos = dsp_q15_cast_rnd_f32( 1.0000000000);
    xSin = dsp_q15_cast_rnd_f32( 0.0000000000);

    // DCT4 Postrotation
    for (i = 0; i < N>>1; i++)             
    {                         
        q15_t tmp0_re = COMPLEX_RE(output, i);
        q15_t tmp0_im = COMPLEX_IM(output, i);
        q15_t tmp1_re = COMPLEX_RE(output, N-i-1);
        q15_t tmp1_im = COMPLEX_IM(output, N-i-1);
        	                        dspfx_a32mul_q15(tmp0_re, xCos, acc);
        COMPLEX_RE(output, i)  =    dspfx_a32mac_q15(tmp0_im, xSin, acc);
                                    dspfx_a32mul_q15(tmp0_re, xSin, acc);
        COMPLEX_IM(output, N-i-1) = dspfx_a32mac_q15(tmp0_im, dspfx_neg_q15(xCos), acc);
        xCos = *postrot_tab++;
        xSin = *postrot_tab++;
                                    dspfx_a32mul_q15(tmp1_re, xSin, acc);
        COMPLEX_RE(output, N-i-1) = dspfx_a32mac_q15(tmp1_im, xCos, acc);
                                    dspfx_a32mul_q15(tmp1_re, xCos, acc);
        COMPLEX_IM(output, i)     = dspfx_a32mac_q15(tmp1_im, dspfx_neg_q15(xSin), acc);
    }                                     
}
#endif
#endif //_ARCv2EM_XY_AGU_MEDIUM

#define XY_MAP_INPUT 0
#define XY_MAP_OUT 0
#define XY_MAP_PREROT_COEF (0)
#define XY_MAP_POSTROT_COEF (0)

/**
 * DCT Type IV "process" function for q15 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_dct4_q15(const dct4_q15_t* handle, const PTR(q15_t) input, PTR(q15_t) output)
{
#if defined(_ARC_AS210_mac16)
    if (handle->length > XY_DCT4_Q15_THRESHOLD) {
#endif
        risc_dsp_dct4_q15(handle, input, output);
#if defined(_ARC_AS210_mac16)
    } else {
        int32_t length_dct4 = handle->length;
        int32_t length_fft = handle->length/2;
        cfft_rad2_q15_xy_t *pInst = (cfft_rad2_q15_xy_t*)&handle->xy_fft;
        memcpy_StoX_int16(XY_MAP_INPUT, (void*)input, length_dct4);

        dsp_dct4_preprocess_init_q15_cfY((dct4_q15_xy_t*)&handle->xy_dct4, length_dct4, XY_MAP_PREROT_COEF);
        dsp_dct4_preprocess_q15_XtoX_cfY_MAC16x16(&handle->xy_dct4, XY_MAP_INPUT, XY_MAP_INPUT);

        dsp_cfft_radix2_init_q15_cfY_DMAC16x16((cfft_rad2_q15_xy_t*)&handle->xy_fft, length_fft, 0);
        pInst->shiftBits = handle->hfft.shiftBits;
        dsp_cfft_radix2_q15_XtoX_cfY_DMAC16x16(&handle->xy_fft, XY_MAP_INPUT, XY_MAP_INPUT); //in-place radix

        dsp_dct4_postprocess_init_q15_cfY((dct4_q15_xy_t*)&handle->xy_dct4, length_dct4, XY_MAP_POSTROT_COEF);
        dsp_dct4_postprocess_q15_XtoX_cfY_MAC16x16(&handle->xy_dct4, XY_MAP_INPUT, XY_MAP_INPUT);

        memcpy_XtoS_int16((void*)output, XY_MAP_INPUT, length_dct4);
    }
#endif
}

void dsp_dct4_delete_q15(dct4_q15_t* handle) 
{
#if defined(_ARC_AS210_mac16)
    if (handle->length > XY_DCT4_Q15_THRESHOLD) {
#endif
        dsp_cfft_delete_q15(&handle->hfft);
#if defined(_ARC_AS210_mac16) 
    } 
#endif
}



#pragma Code("dsplib_dsp_dct4_q15_ext")
// This API is deprecated and is intended to cover compatibility issues.

/**
 * Extended API function. 
 * DCT Type IV instance initialization function for q15 data type
 *
 * \param inst - pointer to transform instance structure
 * \param temp0 - reserved parameter.
 * \param temp1 - reserved parameter.
 * \param temp2 - reserved parameter. 
 * \param length - transform size
 * \param normalize - normalization constant
 */


dsp_status dsp_dct4_init_q15_ext(dct4_q15_t *inst,  void *temp0,  void *temp1,  uint16_t length,  uint16_t temp2,  q15_t normalize)
{
#if !defined _ARCv2EM_XY_11D
    dsp_status result = dsp_dct4_init_q15(inst, length, 0);
#else
    dsp_status result = dsp_dct4_init_q15(inst, length, (PTR(uint32_t)) temp0);
#endif
    inst->normalize = normalize;
    inst->hfft.shiftBits = 29 - dspfx_norm_q31(length);
    inst->hfft.scaling = DSP_TRANSFORM_SCALING_SCALED;
    return result;    
}

/**
 * Extended API function
 * DCT Type IV "process" function for q15 data type
 *
 * \param handle - pointer to transform instance structure
 * \param temp0 - reserved parameter.
 * \param[in, out] in_out - pointer to inplace input/output buffer. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_dct4_q15_ext(const dct4_q15_t *inst,  void *temp0,  PTR(q15_t) in_out)
{
    dsp_dct4_q15(inst, in_out, in_out);
    dsp_scale_q15(in_out, inst->normalize, 0, in_out, inst->length);
}

#endif //!defined(EXCLUDE_DSP_DCT4_Q15)
