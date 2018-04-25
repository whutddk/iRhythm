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

#if !defined(EXCLUDE_DSP_IMDCT_Q15)
#pragma Code("dsplib_dsp_imdct_q15")

#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsp_transform_tables.h"

/**
 * iMDCT instance initialization function for q15 data type
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_imdct_getsize_q15()" function.
 */

dsp_status _DSPLIB_XYCC dsp_imdct_init_q15(mdct_q15_t* handle, uint32_t length, PTR(uint32_t) pState)
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
    if (handle->length > XY_IMDCT_Q15_THRESHOLD) 
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
        err = dsp_cfft_init_q15(&handle->hfft, length/2, (PTR(uint32_t)) data_ptr);
    }
    return err;
};

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __GUARD_ACCUM__

#if defined _XY_HAND_OPT

static _Inline void _DSPLIB_XYCC risc_dsp_imdct_q15(const mdct_q15_t* handle, const q15_t* __restrict input, q15_t* __restrict output)
{
    int32_t i;

    int32_t L = handle->length;
    int32_t N = handle->length >> 1;
	uint32_t tmp0, tmp1;
    int32_t shift = 30-dspfx_norm_q31(L) - 1;
	uint32_t v2shift = (uint32_t)shift << 16 | (uint32_t)shift;
    q15_t* __restrict buffer = output + N; // locate temporary data at the center of output buffer
	q15_t *ptemp = ((uint32_t)handle->temp_buffer) ? handle->temp_buffer : buffer;
    cq15_t* __restrict prerot_tab_c  =  (cq15_t*)handle->prerot;
    cq15_t* __restrict postrot_tab_c  =  (cq15_t*)handle->postrot;

    _DSPLIB_store_regs(5, 8, 0);
    INIT_MACMODE();

	_set_agu_ap0((uint32_t)input); // start of input buffer
	_set_agu_ap1((uint32_t)&input[2*N - 2]); //end of input buffer
	_set_agu_ap2((uint32_t)buffer); // start of output buffer
	_set_agu_ap3((uint32_t)&buffer[2*N - 2]); //end of output buffer

	_set_agu_ap4((uint32_t)prerot_tab_c); // prerotation coefficients

	_set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // input buffer increment
	_set_agu_mod1(1, AGU_DT_v2q15 | AGU_DT_vector_reverse, AGU_AD_DEC_q15(2)); // input buffer decrement, swap real and imaginary part
	_set_agu_mod2(2, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // output buffer increment
	_set_agu_mod3(3, AGU_DT_v2q15, AGU_AD_DEC_q15(2)); // output buffer decrement

	_set_agu_mod4(0, AGU_DT_v2q15, AGU_AD_INC_q15(0)); // input buffer non-modify
	_set_agu_mod5(1, AGU_DT_v2q15 | AGU_DT_vector_reverse, AGU_AD_DEC_q15(0)); // input buffer non-modify
	_set_agu_mod6(4, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // prerotation coefficients increment

	// DCT4 Prerotation
	for (i = 0; i < N>>1; i++)
	{     
		uint32_t tmp0, tmp1;
		tmp0 = _vpack2hl(_agu_u5(),_agu_u4());
		tmp1 = _vpack2hm(_agu_u0(),_agu_u1());
		_st_agu_u2(_vasrsr2h(_cmpychfr(tmp0, _agu_u6()), v2shift));
		_st_agu_u3(_vasrsr2h(_cmpychfr(tmp1, _agu_u6()), v2shift));
	}

	dsp_cfft_q15(&handle->hfft, (PTR(q15_t))buffer, (PTR(q15_t))ptemp);

	_set_agu_ap0((uint32_t)ptemp); // start of output buffer
	_set_agu_ap1((uint32_t)&ptemp[2*N - 2]); //end of output buffer
	_set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // output buffer increment
	_set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_DEC_q15(2)); // output buffer decrement
	_set_agu_mod4(0, AGU_DT_v2q15, AGU_AD_INC_q15(0)); // output buffer non-modify
	_set_agu_mod5(1, AGU_DT_v2q15, AGU_AD_DEC_q15(0)); // output buffer non-modify

	_set_agu_ap4((uint32_t)postrot_tab_c); // postrotation coefficient
	_set_agu_mod6(4, AGU_DT_v2q15, AGU_AD_INC_q15(2)); // postrotation coefficients increment
	_set_agu_mod7(4, AGU_DT_v2q15, AGU_AD_INC_q15(0)); // postrotation coefficients non-modify

	// DCT4 Postrotation

	tmp0 = _cmpychfr(0x00007fff, _agu_u4());
	tmp1 = _cmpyhfr(_agu_u7(), _agu_u5());
	_st_agu_u0(_vpack2hl(tmp1, tmp0));
	_st_agu_u1(_vpack2hm(tmp0, tmp1));

	for (i = 1; i < N>>1; i++)             
	{
		tmp0 = _cmpychfr(_agu_u6(), _agu_u4());
		tmp1 = _cmpyhfr(_agu_u7(), _agu_u5());
		_st_agu_u0(_vpack2hl(tmp1, tmp0));
		_st_agu_u1(_vpack2hm(tmp0, tmp1));
	}                           

	_set_agu_ap0(ptemp + N);
	_set_agu_ap1(ptemp);
	_set_agu_ap2(output);
	_set_agu_ap3(output + L + N);

	_set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2));
	_set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_INC_q15(2));
	_set_agu_mod2(2, AGU_DT_v2q15, AGU_AD_INC_q15(2));
	_set_agu_mod3(3, AGU_DT_v2q15, AGU_AD_INC_q15(2));

    for (i = 0; i < (N >> 1); i++)
    {
        _st_agu_u2(_agu_u0());
        _st_agu_u3(_vnegs2h(_agu_u1()));
    }

	_set_agu_ap0(output);
	_set_agu_ap1(output + L + N);
	_set_agu_ap2(output + L - 2);
	_set_agu_ap3(output + L + N - 2);

	_set_agu_mod0(0, AGU_DT_v2q15 | AGU_DT_vector_reverse, AGU_AD_INC_q15(2));
	_set_agu_mod1(1, AGU_DT_v2q15 | AGU_DT_vector_reverse, AGU_AD_INC_q15(2));
	_set_agu_mod2(2, AGU_DT_v2q15, AGU_AD_DEC_q15(2));
	_set_agu_mod3(3, AGU_DT_v2q15, AGU_AD_DEC_q15(2));

    for (i = 0; i < (N >> 1); i++)
    {
        _st_agu_u2(_vnegs2h(_agu_u0()));
        _st_agu_u3(_agu_u1());
    }
    _DSPLIB_restore_regs();
}

#else

static _Inline void risc_dsp_imdct_q15(const mdct_q15_t* handle, const PTR(q15_t) __restrict input, PTR(q15_t) __restrict output)
{
    int32_t i;

    int32_t L = handle->length;
    int32_t N = handle->length >> 1;

    PTR(q15_t) __restrict buffer = output + N; // locate temporary data at the center of output buffer
    int32_t shift = 30-dspfx_norm_q31(L) - 1;

    PTR(cq15_t) __restrict prerot_tab_c  = (PTR(cq15_t))handle->prerot;
    PTR(cq15_t) __restrict postrot_tab_c  = (PTR(cq15_t))handle->postrot;
	PTR(q15_t) ptemp = ((uint32_t)handle->temp_buffer) ? (PTR(q15_t)) handle->temp_buffer : buffer;
	PTR(cq15_t) ptemp_c = (PTR(cq15_t)) ptemp;

    PTR(cq15_t) __restrict input_c = (PTR(cq15_t)) input;
    PTR(cq15_t) __restrict buffer_c = (PTR(cq15_t)) buffer;
    cq15_t xTw, in0, in1, tmp0, tmp1;

    // Prerotation
    for (i = 0; i < N>>1; i++)
    {
        in0 = input_c[i];
        in1 = input_c[N-i-1];
        tmp0 = fx_create_cq15(fx_get_re_cq15(in0), fx_get_im_cq15(in1));
        tmp1 = fx_create_cq15(fx_get_re_cq15(in1), fx_get_im_cq15(in0));
        buffer_c[i] = dspfx_cjmpy_asr_cq15(tmp0, *prerot_tab_c++, shift);
        buffer_c[N-i-1] = dspfx_cjmpy_asr_cq15(tmp1, *prerot_tab_c++, shift);
    }

    dsp_cfft_q15(&handle->hfft, (PTR(q15_t))buffer, (PTR(q15_t))ptemp);

    // Postrotation
    tmp0 = dspfx_cjmpy_cq15(fx_create_cq15(0x7fff, 0x0000), ptemp_c[0]);
    tmp1 = dspfx_cmpy_cq15(*postrot_tab_c, ptemp_c[N-1]);
    ptemp_c[0] = fx_create_cq15(fx_get_re_cq15(tmp0), fx_get_re_cq15(tmp1));
    ptemp_c[N-1] = fx_create_cq15(fx_get_im_cq15(tmp1), fx_get_im_cq15(tmp0));
    for (i = 1; i < N>>1; i++)
    {
        tmp0 = dspfx_cjmpy_cq15(*postrot_tab_c++, ptemp_c[i]);
        tmp1 = dspfx_cmpy_cq15(*postrot_tab_c, ptemp_c[N-i-1]);
        ptemp_c[i] = fx_create_cq15(fx_get_re_cq15(tmp0), fx_get_re_cq15(tmp1));
        ptemp_c[N-i-1] = fx_create_cq15(fx_get_im_cq15(tmp1), fx_get_im_cq15(tmp0));
    }

    for (i = 0; i < N; i++)
    {
        output[i] = ptemp[N+i];
        output[L+N+i] = dspfx_neg_q15(ptemp[i]);
    }
    for (i = 0; i < N; i++)
    {
        output[L-i-1] = dspfx_neg_q15(output[i]);
        output[L+N-i-1] = output[L+N+i];
    }
}

#endif

#pragma Off_inline(risc_dsp_imdct_q15)

#else

#if defined(__FXAPI__) && defined(__Xdsp_complex)

static _Inline void risc_dsp_imdct_q15(const mdct_q15_t* handle, const q15_t* __restrict input, q15_t* __restrict output) 
{
    int32_t i;

    int32_t L = handle->length;
    int32_t N = handle->length >> 1;

    int32_t shift = 30-dspfx_norm_q31(L) - 1;
    q15_t* __restrict buffer = output + N; // locate temporary data at the center of output buffer
    
    cq15_t* __restrict prerot_tab_c  =  (cq15_t*)handle->prerot;
    cq15_t* __restrict postrot_tab_c  =  (cq15_t*)handle->postrot;
    cq15_t* __restrict input_c = (cq15_t*) input;
    cq15_t* __restrict buffer_c = (cq15_t*) buffer;
    cq15_t xTw, in0, in1, tmp0, tmp1;

    // Prerotation
    for (i = 0; i < N>>1; i++)
    {
        in0 = input_c[i];
        in1 = input_c[N-i-1];
        tmp0 = fx_create_cq15(fx_get_re_cq15(in0), fx_get_im_cq15(in1));
        tmp1 = fx_create_cq15(fx_get_re_cq15(in1), fx_get_im_cq15(in0));
        buffer_c[i] = dspfx_cjmpy_asr_cq15(tmp0, *prerot_tab_c++, shift);
        buffer_c[N-i-1] = dspfx_cjmpy_asr_cq15(tmp1, *prerot_tab_c++, shift);
    }

    dsp_cfft_q15(&handle->hfft, (PTR(q15_t))buffer, (PTR(q15_t))buffer);

    // Postrotation
    xTw  = fx_create_cq15(0x7fff, 0x0000);
    for (i = 0; i < N>>1; i++)             
    {
        tmp0 = dspfx_cjmpy_cq15(xTw, buffer_c[i]);
        xTw = *postrot_tab_c++;
        tmp1 = dspfx_cmpy_cq15(xTw, buffer_c[N-i-1]);
        buffer_c[i] = fx_create_cq15(fx_get_re_cq15(tmp0), fx_get_re_cq15(tmp1));
        buffer_c[N-i-1] = fx_create_cq15(fx_get_im_cq15(tmp1), fx_get_im_cq15(tmp0));
    }

    for (i = 0; i < N; i++)
    {
        output[i] = buffer[N+i];
        output[L+N+i] = dspfx_neg_q15(buffer[i]);
    }
    for (i = 0; i < N; i++)
    {
        output[L-i-1] = dspfx_neg_q15(output[i]);
        output[L+N-i-1] = output[L+N+i];
    }
}


#else
static _Inline void risc_dsp_imdct_q15(const mdct_q15_t* handle, const q15_t* input, q15_t* output) 
{
    int32_t i;
    dspfx_accum32()
    int32_t L = handle->length;
    int32_t N = handle->length >> 1;
    int32_t xCos, xSin;
    int32_t shift = 30-dspfx_norm_q31(L) - 1;
    q15_t* buffer = output + N; // locate temporary data at the center of output buffer

    q15_t* prerot_tab  = handle->prerot;
    q15_t* postrot_tab = handle->postrot;

    INIT_MACMODE();
    // Prerotation
    for (i = 0; i < N>>1; i++)
    {
        q15_t tmp0_re = COMPLEX_RE(input, i);
        q15_t tmp0_im = COMPLEX_IM(input, i);
        q15_t tmp1_re = COMPLEX_RE(input, N-i-1);
        q15_t tmp1_im = COMPLEX_IM(input, N-i-1);
        xCos = *prerot_tab++; 
        xSin = *prerot_tab++;

                            dspfx_a32mul_q15( tmp0_re, xCos, acc);
        COMPLEX_RE(buffer, i) =    dspfx_a32mac_q15( tmp1_im, xSin, acc) >> shift;
                            dspfx_a32mul_q15( tmp1_im, xCos, acc);
        COMPLEX_IM(buffer, i) =    dspfx_a32mac_q15( tmp0_re, dspfx_neg_q15(xSin), acc) >> shift;
        xCos = *prerot_tab++; 
        xSin = *prerot_tab++;
                                dspfx_a32mul_q15( tmp0_im, xSin, acc);
        COMPLEX_RE(buffer, N-i-1) =    dspfx_a32mac_q15( tmp1_re, xCos, acc) >> shift;
                                dspfx_a32mul_q15( tmp0_im, xCos, acc);
        COMPLEX_IM(buffer, N-i-1) =    dspfx_a32mac_q15( tmp1_re, dspfx_neg_q15(xSin), acc) >> shift;
    }

    dsp_cfft_q15(&handle->hfft, (PTR(q15_t))buffer, (PTR(q15_t))buffer);

    xCos = dsp_q15_cast_rnd_f32( 1.0000000000);
    xSin = dsp_q15_cast_rnd_f32( 0.0000000000);

    // DCT4 Postrotation
    for (i = 0; i < N>>1; i++)             
    {                         
        q15_t tmp0_re = COMPLEX_RE(buffer, i);
        q15_t tmp0_im = COMPLEX_IM(buffer, i);
        q15_t tmp1_re = COMPLEX_RE(buffer, N-i-1);
        q15_t tmp1_im = COMPLEX_IM(buffer, N-i-1);
                             dspfx_a32mul_q15(tmp0_re, xCos, acc);
        COMPLEX_RE(buffer, i)  =    dspfx_a32mac_q15(tmp0_im, xSin, acc);
                             dspfx_a32mul_q15(tmp0_re, xSin, acc);
        COMPLEX_IM(buffer, N-i-1) = dspfx_a32mac_q15(tmp0_im, dspfx_neg_q15(xCos), acc);
        xCos = *postrot_tab++;
        xSin = *postrot_tab++;
                             dspfx_a32mul_q15(tmp1_re, xSin, acc);
        COMPLEX_RE(buffer, N-i-1) = dspfx_a32mac_q15(tmp1_im, xCos, acc);
                             dspfx_a32mul_q15(tmp1_re, xCos, acc);
        COMPLEX_IM(buffer, i)     = dspfx_a32mac_q15(tmp1_im, dspfx_neg_q15(xSin), acc);
    }

    for (i = 0; i < N; i++)
    {
        output[i] = buffer[N+i];
        output[L+N+i] = dspfx_neg_q15(buffer[i]);
    }
    for (i = 0; i < N; i++)
    {
        output[L-i-1] = dspfx_neg_q15(output[i]);
        output[L+N-i-1] = output[L+N+i];
    }
}
#endif
#endif

#define XY_MAP_INPUT 0
#define XY_MAP_OUT 0
#define XY_MAP_PREROT_COEF (0)
#define XY_MAP_POSTROT_COEF (0)

/**
 * iMDCT "process" function for q15 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_imdct_q15(const mdct_q15_t* handle, const PTR(q15_t) input, PTR(q15_t) output)
{
#if defined(_ARC_AS210_mac16)
    if (handle->length > XY_IMDCT_Q15_THRESHOLD) {
#endif
        risc_dsp_imdct_q15(handle, input, output);
#if defined(_ARC_AS210_mac16)
    } else {
        int32_t length = handle->length;
        int32_t length_fft = handle->length>>1;
        int32_t shift = 30-dspfx_norm_q31(length_fft);

        memcpy_StoX_int32(XY_MAP_INPUT>>1, (void*)input, length>>1);

        dsp_dct4_preprocess_init_q15_cfY((dct4_q15_xy_t*)&handle->xy_dct4, length, XY_MAP_PREROT_COEF);
        dsp_dct4_preprocess_q15_XtoX_cfY_MAC16x16(&handle->xy_dct4, XY_MAP_INPUT, XY_MAP_OUT);

        dsp_shift_q15_XtoY(XY_MAP_INPUT, -shift, XY_MAP_OUT, length);

        dsp_cfft_radix2_init_q15_cfX_DMAC16x16((cfft_rad2_q15_xy_t*)&handle->xy_fft, length_fft, 0);
        dsp_cfft_radix2_q15_YtoY_cfX_DMAC16x16(&handle->xy_fft, XY_MAP_OUT, XY_MAP_OUT); //in-place radix

        dsp_dct4_postprocess_init_q15_cfX((dct4_q15_xy_t*)&handle->xy_dct4, length, XY_MAP_POSTROT_COEF);
        dsp_dct4_postprocess_q15_YtoY_cfX_MAC16x16(&handle->xy_dct4, XY_MAP_OUT, XY_MAP_OUT);

        dsp_imdct_postprocess_q15_YtoX(XY_MAP_OUT, XY_MAP_INPUT, length);

        memcpy_XtoS_int32((void*)output, XY_MAP_INPUT>>1, length);
    }
#endif
}

void dsp_imdct_delete_q15(mdct_q15_t* handle) 
{
#if defined(_ARC_AS210_mac16)
    if (handle->length > XY_IMDCT_Q15_THRESHOLD) {
#endif
        dsp_cfft_delete_q15(&handle->hfft);
#if defined(_ARC_AS210_mac16) 
    } 
#endif
};

#endif //!defined(EXCLUDE_DSP_IMDCT_Q15)
