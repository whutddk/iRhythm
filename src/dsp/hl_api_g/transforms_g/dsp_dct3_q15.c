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

#if !defined(EXCLUDE_DSP_DCT3_Q15)
#pragma Code("dsplib_dsp_dct3_q15")

#if defined(_ARC_AS210_mac16) 
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsp_transform_tables.h"

/**
 * DCT Type III instance initialization function for q15 data type
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_dct3_getsize_q15()" function.
 */

#if !defined(_ARC_BE)

#define _VSUBADDS2H(a,b) _vsubadds2h((a), (b))
#define _VADDSUBS2H(a,b) _vaddsubs2h((a), (b))
#define _VALGN2H(a,b) _valgn2h((a), (b))
#define _VPACK2HL(a,b) _vpack2hl((a),(b))

#else

#define _VSUBADDS2H(a,b) _vaddsubs2h((a), (b))
#define _VADDSUBS2H(a,b) _vsubadds2h((a), (b))
#define _VALGN2H(a,b) _valgn2h((b), (a))
#define _VPACK2HL(a,b) _vpack2hl((b),(a))

#endif


dsp_status _DSPLIB_XYCC dsp_dct3_init_q15(dct3_q15_t* handle, uint32_t length, PTR(uint32_t) pState)
{
    int32_t order = 30-dspfx_norm_q31(length);
    uint8_t* data_ptr = (uint8_t*) (void*) pState;
    if ((length > DSP_MAX_TRANSFORM_SIZE))
        return DSP_ERR_LENGTH_TOO_BIG;
    if ((length < 8))
        return DSP_ERR_LENGTH_TOO_SMALL;
    if (length != (1 << order))
        return DSP_ERR_LENGTH_NOT_POWER2;
    if (dsp_cifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;
    if (dsp_dct23_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;

    handle->length = length;
#ifdef _ARCv2EM_XY_11D
    handle->ptwd = (q15_t*)data_ptr; data_ptr += length * sizeof(q15_t)/2;
    memcpy(handle->ptwd, dsp_cifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(q15_t)/2);
    handle->ptwd4 = (q15_t*)data_ptr; data_ptr += length * sizeof(q15_t);
    memcpy(handle->ptwd4, dsp_dct23_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(q15_t));
#else    
    handle->ptwd = (q15_t*)dsp_cifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
    handle->ptwd4= (q15_t*)dsp_dct23_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif
    handle->temp_buffer = (q15_t *)(void*)data_ptr; data_ptr += length * sizeof(q15_t);
    return dsp_cfft_init_q15(&handle->hfft, length/2, (PTR(uint32_t))(void*) data_ptr);
};

#if defined(_ARCv2EM_XY_AGU_MEDIUM) 
#if defined _XY_HAND_OPT
static void _DSPLIB_XYCC risc_dsp_dct3_q15(const dct3_q15_t* handle, const q15_t* __restrict input, q15_t* __restrict output)
{
    int32_t i;
    q15_t* __restrict buffer = handle->temp_buffer;
    dspfx_accum32()
        int32_t L = handle->length;
    int32_t N = handle->length >> 1;

    q15_t xCos;
    uint32_t* pcTWD1 = (uint32_t*)(handle->ptwd+2); 
    uint32_t* pcTWD4 = (uint32_t*) handle->ptwd4;

    INIT_MACMODE();

    _DSPLIB_store_regs(4, 5, 0);

    _set_agu_ap0((uint32_t)pcTWD4); // twiddle factors
    _set_agu_ap1((uint32_t)&input[1]); // start of input buffer
    _set_agu_ap2((uint32_t)&input[L - 1]); //end of input buffer
    _set_agu_ap3((uint32_t)&output[2]); //start of temp buffer

    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_q15(2)); // twiddle increment
    _set_agu_mod1(1, AGU_DT_q15, AGU_AD_INC_q15(1)); // input buffer increment
    _set_agu_mod2(2, AGU_DT_q15, AGU_AD_DEC_q15(1)); // input buffer decrement
    _set_agu_mod3(3, AGU_DT_ui32, AGU_AD_INC_q15(2)); // temp buffer increment


    for (i = 1; i < N; i++) 
    { 
        uint32_t A;
        A = _VPACK2HL(_agu_u2(), _agu_u1());
        _st_agu_u3(_cmpychfr(A, _agu_u0()));
    }

#if !defined(_ARC_BE)
    xCos = (q15_t)(_agu_u0() & 0xffff);
#else
    xCos = (q15_t)((uint32_t)_agu_u0() >> 16);
#endif

    COMPLEX_RE(output, 0) = dspfx_add_q15(input[0], dspfx_a32mul_q15(input[N], xCos, acc));
    COMPLEX_IM(output, 0) = dspfx_sub_q15(input[0], dspfx_a32mul_q15(input[N], xCos, acc));

    _set_agu_ap0((uint32_t)pcTWD1); // twiddle factors
    _set_agu_ap1((uint32_t)&output[2]); // start of temp buffer
    _set_agu_ap2((uint32_t)&output[2*N - 2]); //end of temp buffer

    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_q15(2)); // twiddle increment
    _set_agu_mod1(1, AGU_DT_v2q15 | AGU_DT_vector_reverse, AGU_AD_INC_q15(2)); // temp buffer increment
    _set_agu_mod2(2, AGU_DT_v2q15 | AGU_DT_vector_reverse, AGU_AD_DEC_q15(2)); // temp buffer decrement

    _set_agu_mod3(1, AGU_DT_ui32, AGU_AD_INC_q15(0)); // temp buffer non-modified
    _set_agu_mod4(2, AGU_DT_ui32, AGU_AD_INC_q15(0)); // temp buffer non-modified

    for (i = 1; i < N>>1; i++) {

        uint32_t a1a2 = _vasrsr2h(_vadds2h(_agu_u3(), _agu_u4()),0x10001);
        uint32_t a3a4 = _vasrsr2h(_VSUBADDS2H(0, _vsubs2h(_agu_u3(), _agu_u4())),0x10001);
        uint32_t a2a3 = _VALGN2H(a3a4, a1a2);
        uint32_t a5a6 = _cmpychfr(_agu_u0(), a2a3);

        uint32_t a1a6 = _VALGN2H(a1a2, a5a6);
        uint32_t a5a4 = _VALGN2H(a5a6, a3a4);

        _st_agu_u1(_VADDSUBS2H(a1a6, a5a4));
        _st_agu_u2(_VSUBADDS2H(a1a6, a5a4));
    }

    dsp_cfft_q15(&handle->hfft, (PTR(q15_t))output, (PTR(q15_t))buffer);

    _set_agu_ap0((uint32_t)buffer); // start of temp buffer
    _set_agu_ap1((uint32_t)&buffer[2*N - 2]); //end of temp buffer
    _set_agu_ap2((uint32_t)output); // output buffer

    _set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // temp buffer increment
    _set_agu_mod1(1, AGU_DT_v2q15 | AGU_DT_vector_reverse, AGU_AD_DEC_q15(2)); // temp buffer decrement, swap real and imaginary part
    _set_agu_mod2(2, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // output buffer increment

    _set_agu_mod3(0, AGU_DT_v2q15, AGU_AD_INC_q15(0)); // non-increment mode
    _set_agu_mod4(1, AGU_DT_v2q15 | AGU_DT_vector_reverse, AGU_AD_DEC_q15(0)); // non-increment mode

    for (i = 0; i < N >> 1; i++) 
    {
        _st_agu_u2(_vpack2hl(_agu_u4(),_agu_u3()));
        _st_agu_u2(_vpack2hm(_agu_u1(),_agu_u0()));
    }

    _DSPLIB_restore_regs();
}

#pragma Off_inline(risc_dsp_dct3_q15)

#else

static _Inline void risc_dsp_dct3_q15(const dct3_q15_t* handle, const PTR(q15_t) __restrict input, PTR(q15_t) __restrict output) 
{
    int32_t i;
    PTR(q15_t) __restrict buffer = (PTR(q15_t)) handle->temp_buffer;
    PTR(cq15_t) __restrict buffer_c = (PTR(cq15_t))handle->temp_buffer;
    PTR(cq15_t) __restrict output_c = (PTR(cq15_t))output;
    dspfx_accum32()
    int32_t L = handle->length;
    int32_t N = handle->length >> 1;
    //q15_t xSin, xCos;
    q15_t xCos;
    PTR(v2q15_t) __restrict cbuffer = (PTR(v2q15_t))handle->temp_buffer;
    PTR(cq15_t) pcTWD1 =(PTR(cq15_t)) (handle->ptwd+2); 
    PTR(cq15_t) pTWD4_c = (PTR(cq15_t))handle->ptwd4;

    INIT_MACMODE();

    for (i = 1; i < N; i++) 
    {
        cq15_t xTw;
        cq15_t A;
        v2accum40_t acc;
        xTw = *pTWD4_c++;
        A = fx_create_cq15(input[i], input[L - i]);
        acc = fx_v2a40_cjmpy_cq15(A, xTw);
        buffer_c[i] = fx_cq15_cast_rnd_v2a40(acc);
    }

    xCos = fx_get_re_cq15(*pTWD4_c);

    COMPLEX_RE(buffer, 0) = dspfx_add_q15(input[0], dspfx_a32mul_q15(input[N], xCos, acc));
    COMPLEX_IM(buffer, 0) = dspfx_sub_q15(input[0], dspfx_a32mul_q15(input[N], xCos, acc));

    for (i = 1; i < N>>1; i++) {
        cq15_t cTw = *pcTWD1++;
        v2q15_t Bi  = cbuffer[i];
        v2q15_t BNi = cbuffer[N-i];

        v2q15_t a1a2 = fx_asr_rnd_v2q15(fx_add_v2q15(Bi, BNi), fx_create_v2i16(1,1));
        v2q15_t a3a4 = fx_asr_rnd_v2q15(fx_subadd_v2q15(fx_create_v2q15(0,0), fx_sub_v2q15(Bi, BNi)), fx_create_v2i16(1,1));
        v2q15_t a2a3 = fx_align_v2q15(a3a4, a1a2);

        v2q15_t a5a6 = fx_v2q15_cast_rnd_v2a40(fx_v2a40_cjmpy_cq15(cTw, fx_cq15_cast_v2q15(a2a3)));

        v2q15_t a1a6 = fx_align_v2q15(a1a2, a5a6);
        v2q15_t a5a4 = fx_align_v2q15(a5a6, a3a4);
        
        a1a2 = fx_addsub_v2q15(a1a6, a5a4);
        a3a4 = fx_subadd_v2q15(a1a6, a5a4);

        cbuffer[i]   = fx_align_v2q15(a1a2, a1a2);
        cbuffer[N-i] = fx_align_v2q15(a3a4, a3a4);
    }  

    dsp_cfft_q15(&handle->hfft, (PTR(q15_t))buffer, (PTR(q15_t))buffer);

    for (i = 0; i < N >> 1; i++) 
    {
        cq15_t in0, in1;
        in0 = buffer_c[i];
        in1 = buffer_c[N - i - 1];
        output_c[ i << 1    ] = fx_create_cq15(fx_get_re_cq15(in0), fx_get_im_cq15(in1));
        output_c[(i << 1) +1] = fx_create_cq15(fx_get_im_cq15(in0), fx_get_re_cq15(in1));
    }
}


#endif

#else //_ARCv2EM_XY_AGU_MEDIUM


#if defined(__FXAPI__) && defined(__Xdsp_complex)
static _Inline void risc_dsp_dct3_q15(const dct3_q15_t* handle, const q15_t* __restrict input, q15_t* __restrict output) 
{
    int32_t i;
    q15_t* __restrict buffer = handle->temp_buffer;
    cq15_t* __restrict buffer_c = (cq15_t* )handle->temp_buffer;
    cq15_t* __restrict output_c = (cq15_t* )output;
    dspfx_accum32()
    int32_t L = handle->length;
    int32_t N = handle->length >> 1;
    //q15_t xSin, xCos;
    q15_t xCos;
    uint32_t* __restrict cbuffer = (uint32_t*)handle->temp_buffer;
    uint32_t* pcTWD1 =(uint32_t*) (handle->ptwd+2); 
    cq15_t* pTWD4_c = (cq15_t*)handle->ptwd4;

    INIT_MACMODE();

    for (i = 1; i < N; i++) 
    {
        cq15_t xTw;
        cq15_t A;
#if defined(__GUARD_ACCUM__)
        v2accum40_t acc;
        xTw = *pTWD4_c++;
        A = fx_create_cq15(input[i], input[L - i]);
        acc = fx_v2a40_cjmpy_cq15(A, xTw);
        buffer_c[i] = fx_cq15_cast_rnd_v2a40(acc);
#else
        v2accum32_t acc;
        xTw = *pTWD4_c++;
        A = fx_create_cq15(input[i], input[L - i]);
        acc = fx_v2a32_cjmpy_cq15(A, xTw);
        buffer_c[i] = fx_cq15_cast_rnd_v2a32(acc);
#endif
       
    }

    xCos = fx_get_re_cq15(*pTWD4_c);

    COMPLEX_RE(buffer, 0) = dspfx_add_q15(input[0], dspfx_a32mul_q15(input[N], xCos, acc));
    COMPLEX_IM(buffer, 0) = dspfx_sub_q15(input[0], dspfx_a32mul_q15(input[N], xCos, acc));

    for (i = 1; i < N>>1; i++) {
        uint32_t cTw = *pcTWD1++;
        uint32_t Bi  = cbuffer[i];
        uint32_t BNi = cbuffer[N-i];

        uint32_t a1a2 = _vasrsr2h(_vadds2h(Bi, BNi),0x10001);
        uint32_t a3a4 = _vasrsr2h(_VSUBADDS2H(0, _vsubs2h(Bi, BNi)),0x10001);
        uint32_t a2a3 = _VALGN2H(a3a4, a1a2);
        uint32_t a5a6 = _cmpychfr(cTw, a2a3);

        uint32_t a1a6 = _VALGN2H(a1a2, a5a6);
        uint32_t a5a4 = _VALGN2H(a5a6, a3a4);

		a1a2 = _VADDSUBS2H(a1a6, a5a4);
		a3a4 = _VSUBADDS2H(a1a6, a5a4);

        cbuffer[i]   = _VALGN2H(a1a2, a1a2);
        cbuffer[N-i] = _VALGN2H(a3a4, a3a4);
    }


    dsp_cfft_q15(&handle->hfft, (PTR(q15_t))buffer, (PTR(q15_t))buffer);

    for (i = 0; i < N >> 1; i++) 
    {
        cq15_t in0, in1;
        in0 = buffer_c[i];
        in1 = buffer_c[N - i - 1];
        output_c[ i << 1    ] = fx_create_cq15(fx_get_re_cq15(in0), fx_get_im_cq15(in1));
        output_c[(i << 1) +1] = fx_create_cq15(fx_get_im_cq15(in0), fx_get_re_cq15(in1));
    }
}

#else
static _Inline void risc_dsp_dct3_q15(const dct3_q15_t* handle, const q15_t* __restrict input, q15_t* __restrict output) 
{
    int32_t i;
    q15_t* __restrict buffer = handle->temp_buffer;
    dspfx_accum32()
    int32_t L = handle->length;
    int32_t N = handle->length >> 1;
    q15_t xSin, xCos;

    q15_t* __restrict pTWD1 = handle->ptwd+2;
    q15_t* __restrict pTWD4 = handle->ptwd4;


    INIT_MACMODE();

    for (i = 1; i < N; i++) 
    {

        xCos = *pTWD4++;
        xSin = *pTWD4++;

                         dspfx_a32mul_q15(input[i],     xCos, acc);
        COMPLEX_RE(buffer, i) = dspfx_a32mac_q15(input[L - i], xSin, acc);
                         dspfx_a32mul_q15(input[L - i], xCos, acc);
        COMPLEX_IM(buffer, i) = dspfx_a32mac_q15(input[i],     -xSin, acc);

        
    }

    xCos = *pTWD4;

    COMPLEX_RE(buffer, 0) = dspfx_add_q15(input[0], dspfx_a32mul_q15(input[N], xCos, acc));
    COMPLEX_IM(buffer, 0) = dspfx_sub_q15(input[0], dspfx_a32mul_q15(input[N], xCos, acc));

    for (i = 1; i < N>>1; i++) {
        int32_t a1, a2, a3, a4, a5, a6;
        xCos = *pTWD1++;
        xSin = *pTWD1++;

        a1 = dspfx_add_q15(COMPLEX_RE(buffer, i),   COMPLEX_RE(buffer, N - i))>>1;
        a2 = dspfx_add_q15(COMPLEX_IM(buffer, i),   COMPLEX_IM(buffer, N - i))>>1;
        a3 = dspfx_sub_q15(COMPLEX_RE(buffer, N - i), COMPLEX_RE(buffer, i))>>1;
        a4 = dspfx_sub_q15(COMPLEX_IM(buffer, i),     COMPLEX_IM(buffer, N - i))>>1;
             dspfx_a32mul_q15(a2, xCos, acc);
        a5 = dspfx_a32mac_q15(a3, xSin, acc);
             dspfx_a32mul_q15(a2, xSin, acc);
        a6 = dspfx_a32mac_q15(a3, -xCos, acc);
        COMPLEX_RE(buffer, i) = dspfx_sub_q15(a1, a5);
        COMPLEX_IM(buffer, i) = dspfx_add_q15(a6, a4);
        COMPLEX_RE(buffer, N - i) = dspfx_add_q15(a1, a5);
        COMPLEX_IM(buffer, N - i) = dspfx_sub_q15(a6, a4);
    }

    dsp_cfft_q15(&handle->hfft, (PTR(q15_t))buffer, (PTR(q15_t))buffer);

    for (i = 0; i < N >> 1; i++) 
    {

        output[ (i << 2)     ] = COMPLEX_RE(buffer, i);
        output[ (i << 2) + 2 ] = COMPLEX_IM(buffer, i);
        output[ (i << 2) + 1 ] = COMPLEX_IM(buffer, N - i - 1);
        output[ (i << 2) + 3 ] = COMPLEX_RE(buffer, N - i - 1);
        
    }
}
#endif
#endif //_ARCv2EM_XY_AGU_MEDIUM

#define XY_MAP_INPUT    (((XY_REGION_SIZE/2))/sizeof(q15_t))
#define XY_MAP_OUT      0
#define XY_MAP_TMP      (((XY_REGION_SIZE/2))/sizeof(q15_t))
#define XY_MAP_TWIDDLES (0)
#define XY_DCT3_Q15_THRESHOLD (((XY_REGION_SIZE/8))/sizeof(q15_t))


/**
 * DCT Type III "process" function for q15 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_dct3_q15(const dct3_q15_t* handle, const PTR(q15_t) input, PTR(q15_t) output)
{
#if defined(_ARC_AS210_mac16)
    if (handle->length > XY_DCT3_Q15_THRESHOLD) {
#endif
        risc_dsp_dct3_q15(handle, input, output);
#if defined(_ARC_AS210_mac16)
    } else {
        cfft_rad2_q15_xy_t *pInst = (cfft_rad2_q15_xy_t*)&handle->hfft.fft_rad2_xy;
        memcpy_StoY_int16(XY_MAP_INPUT, (void*)input, handle->length);


        dsp_dct3_preprocess_init_q15_cfY(
            (dct3_q15_xy_t*)&handle->dct3_xy,
            handle->length,
            0);
        dsp_dct3_preprocess_q15_YtoX_cfY_MAC16x16(
            &handle->dct3_xy,
            XY_MAP_INPUT,
            XY_MAP_TMP);


        dsp_cfft_radix2_init_q15_cfX_DMAC16x16(
            pInst, 
            handle->length/2, 
            XY_MAP_TWIDDLES);
        pInst->shiftBits = handle->hfft.shiftBits;
        dsp_cfft_radix2_q15_XtoY_cfX_DMAC16x16(
            pInst, 
            XY_MAP_TMP, 
            XY_MAP_TMP);


        dsp_dct3_postprocess_q15_YtoX(
            XY_MAP_TMP, 
            XY_MAP_OUT, 
            handle->length);

        memcpy_XtoS_int16((void*)output, XY_MAP_OUT , handle->length);
    }
#endif
}

void dsp_dct3_delete_q15(dct3_q15_t* handle) 
{
    dsp_cfft_delete_q15(&handle->hfft);
}

#endif //!defined(EXCLUDE_DSP_DCT3_Q15)
