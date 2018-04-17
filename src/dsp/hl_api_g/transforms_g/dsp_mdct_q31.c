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

#if !defined(EXCLUDE_DSP_MDCT_Q31)
#pragma Code("dsplib_dsp_mdct_q31")

#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsp_transform_tables.h"

/**
 * MDCT instance initialization function for q31 data type
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_mdct_getsize_q31()" function.
 */

dsp_status _DSPLIB_XYCC dsp_mdct_init_q31(mdct_q31_t* handle, uint32_t length, PTR(uint32_t) pState)
{
    dsp_status err = DSP_ERR_OK;
    int32_t order = 30-dspfx_norm_q31(length/2);
    uint8_t* data_ptr = (uint8_t*) (void*) pState;        
    if ((length/2 > DSP_MAX_TRANSFORM_SIZE))
        return DSP_ERR_LENGTH_TOO_BIG;
    if ((length < 16))
        return DSP_ERR_LENGTH_TOO_SMALL;
    if (length/2 != (1 << order))
        return DSP_ERR_LENGTH_NOT_POWER2;
    if (dsp_dct4_q31_prerot_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;
    if (dsp_dct4_q31_postrot_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;

    handle->length = length;
#if defined(_ARC_AS210_mac32)
    if (handle->length > XY_MDCT_Q31_THRESHOLD) 
#endif
    {
#ifdef _ARCv2EM_XY_11D
        handle->prerot  = (q31_t*) (void*) data_ptr; data_ptr += length * sizeof(q31_t) / 2;
        handle->postrot = (q31_t*) (void*) data_ptr; data_ptr += length * sizeof(q31_t) / 4;
        memcpy(handle->prerot,   dsp_dct4_q31_prerot_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(q31_t) / 2);
        memcpy(handle->postrot, dsp_dct4_q31_postrot_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(q31_t) / 4);
#else          
        handle->prerot  = (q31_t*)dsp_dct4_q31_prerot_tab[order - DSP_MIN_TRANSFORM_ORDER];
        handle->postrot = (q31_t*)dsp_dct4_q31_postrot_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif
        handle->temp_buffer = (q31_t*) (void*) data_ptr; data_ptr += length * sizeof(q31_t) / 2;        
        err = dsp_cfft_init_q31(&handle->hfft, length / 4, (PTR(uint32_t))data_ptr);
    } 
    return err;
};

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__ && defined __GUARD_ACCUM__

#if defined _XY_HAND_OPT

static void _DSPLIB_XYCC risc_dsp_mdct_q31(const mdct_q31_t* handle, const q31_t* input, q31_t* output)
{
    int32_t i;
    dspfx_accum64()
    int32_t N = handle->length >> 2;
    q31_t xSin, xCos;
	q31_t *ptemp = ((uint32_t)handle->temp_buffer) ? handle->temp_buffer : output;

    _DSPLIB_store_regs(6 , 8 ,0);
	_set_agu_ap0(input + 3*N - 1);
	_set_agu_ap1(input + 3*N);
	_set_agu_ap2(input);
	_set_agu_ap3(input + 2*N - 1);
	_set_agu_ap4(output);
	_set_agu_ap5(output + N);

	_set_agu_mod0(0, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod1(1, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod2(2, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod3(3, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod4(4, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod5(5, AGU_DT_q31, AGU_AD_INC_q31(1));

    // MDCT preprocess
    for (i = 0; i < N; i++)
    {
        _st_agu_u4(dspfx_neg_q31(dspfx_add_q31(_agu_u0(), _agu_u1())));
        _st_agu_u5(              dspfx_sub_q31(_agu_u2(), _agu_u3()));
    }

	_set_agu_ap0(output);
	_set_agu_ap1(output + N*2 - 1);
	_set_agu_ap2(ptemp);
	_set_agu_ap3(ptemp + N*2 - 1);
	_set_agu_ap4(handle->prerot);

	_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod1(1, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod2(2, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod3(3, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod4(4, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod5(4, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod6(4, AGU_DT_q31, AGU_AD_INC_q31(0));
	_set_agu_mod7(4, AGU_DT_q31, AGU_AD_INC_q31(2));

    // DCT4 Prerotation
    for (i = 0; i < N>>1; i++)
    {
        q31_t tmp0_re = _agu_u0();
        q31_t tmp0_im = _agu_u0();
		q31_t tmp1_im = _agu_u1();
        q31_t tmp1_re = _agu_u1();
                   dspfx_a64mul_q31( tmp0_re, _agu_u4(), acc);
        _st_agu_u2(dspfx_a64mac_q31( tmp1_im, _agu_u5(), acc));
                   dspfx_a64mul_q31( tmp1_im, _agu_u4(), acc);
        _st_agu_u2(dspfx_a64msu_q31( tmp0_re, _agu_u4(), acc));
                   dspfx_a64mul_q31( tmp0_im, _agu_u4(), acc);
        _st_agu_u3(dspfx_a64msu_q31( tmp1_re, _agu_u6(), acc));
                   dspfx_a64mul_q31( tmp0_im, _agu_u5(), acc);
        _st_agu_u3(dspfx_a64mac_q31( tmp1_re, _agu_u7(), acc));
    }

    dsp_cfft_q31(&handle->hfft, (PTR(q31_t))ptemp, (PTR(q31_t))output);

    xCos = dsp_q31_cast_rnd_f32( 1.0000000000);
    xSin = dsp_q31_cast_rnd_f32( 0.0000000000);

	_set_agu_ap0(output);
	_set_agu_ap1(output + N*2 - 1);
	_set_agu_ap2(handle->postrot);

	_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod2(1, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod3(1, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod4(2, AGU_DT_q31, AGU_AD_INC_q31(1));

    // DCT4 Postrotation
    for (i = 0; i < N>>1; i++)             
    {
        q31_t tmp0_re = _agu_u0();
        q31_t tmp0_im = _agu_u1();
		q31_t tmp1_im = _agu_u2();
        q31_t tmp1_re = _agu_u3();
                   dspfx_a64mul_q31(tmp0_re, xCos, acc);
        _st_agu_u0(dspfx_a64mac_q31(tmp0_im, xSin, acc));
                   dspfx_a64mul_q31(tmp0_re, xSin, acc);
        _st_agu_u2(dspfx_a64msu_q31(tmp0_im, xCos, acc));
        xCos = _agu_u4();
        xSin = _agu_u4();
                   dspfx_a64mul_q31(tmp1_re, xCos, acc);
        _st_agu_u0(dspfx_a64msu_q31(tmp1_im, xSin, acc));
                   dspfx_a64mul_q31(tmp1_re, xSin, acc);
        _st_agu_u2(dspfx_a64mac_q31(tmp1_im, xCos, acc));
    }

    _DSPLIB_restore_regs();
}

#pragma Off_inline(risc_dsp_mdct_q31)

#else

static _Inline void risc_dsp_mdct_q31(const mdct_q31_t* handle, const PTR(q31_t) input, PTR(q31_t) output)
{
    int32_t i;
    dspfx_accum64()
    int32_t N = handle->length >> 2;
    q31_t xSin, xCos;
    PTR(q31_t) prerot_tab  = (PTR(q31_t))handle->prerot;
    PTR(q31_t) postrot_tab = (PTR(q31_t))handle->postrot;
	PTR(q31_t) ptemp = ((uint32_t)handle->temp_buffer) ? (PTR(q31_t)) handle->temp_buffer : output;

    // MDCT preprocess
    for (i = 0; i < N; i++)
    {
        output[i] = dspfx_neg_q31(dspfx_add_q31(input[(3*N-1)-i], input[3*N+i]));
        output[N+i] =       dspfx_sub_q31(input[i],     input[(2*N-1)-i]);
    }

    // DCT4 Prerotation
    for (i = 0; i < N>>1; i++)
    {
        q31_t tmp0_re = COMPLEX_RE(output, i);
        q31_t tmp0_im = COMPLEX_IM(output, i);
        q31_t tmp1_re = COMPLEX_RE(output, N-i-1);
        q31_t tmp1_im = COMPLEX_IM(output, N-i-1);
        xCos = *prerot_tab++; 
        xSin = *prerot_tab++;
                                dspfx_a64mul_q31(tmp0_re, xCos, acc);
        COMPLEX_RE(ptemp, i) = dspfx_a64mac_q31(tmp1_im, xSin, acc);
                                dspfx_a64mul_q31(tmp1_im, xCos, acc);
        COMPLEX_IM(ptemp, i) = dspfx_a64msu_q31(tmp0_re, xSin, acc);
        xCos = *prerot_tab++;
        xSin = *prerot_tab++;
                                    dspfx_a64mul_q31(tmp0_im, xSin, acc);
        COMPLEX_RE(ptemp, N-i-1) = dspfx_a64mac_q31(tmp1_re, xCos, acc);
                                    dspfx_a64mul_q31(tmp0_im, xCos, acc);
        COMPLEX_IM(ptemp, N-i-1) = dspfx_a64msu_q31(tmp1_re, xSin, acc);
    }

    dsp_cfft_q31(&handle->hfft, (PTR(q31_t))ptemp, (PTR(q31_t))output);

    xCos = dsp_q31_cast_rnd_f32(1.0000000000);
    xSin = dsp_q31_cast_rnd_f32(0.0000000000);

    // DCT4 Postrotation
    for (i = 0; i < N>>1; i++)
    {
        q31_t tmp0_re = COMPLEX_RE(output, i);
        q31_t tmp0_im = COMPLEX_IM(output, i);
        q31_t tmp1_re = COMPLEX_RE(output, N-i-1);
        q31_t tmp1_im = COMPLEX_IM(output, N-i-1);
                                dspfx_a64mul_q31(tmp0_re, xCos, acc);
        COMPLEX_RE(output, i) = dspfx_a64mac_q31(tmp0_im, xSin, acc);
                                    dspfx_a64mul_q31(tmp0_re, xSin, acc);
        COMPLEX_IM(output, N-i-1) = dspfx_a64msu_q31(tmp0_im, xCos, acc);
        xCos = *postrot_tab++;
        xSin = *postrot_tab++;
                                    dspfx_a64mul_q31(tmp1_re, xSin, acc);
        COMPLEX_RE(output, N-i-1) = dspfx_a64mac_q31(tmp1_im, xCos, acc);
                                dspfx_a64mul_q31(tmp1_re, xCos, acc);
        COMPLEX_IM(output, i) = dspfx_a64msu_q31(tmp1_im, xSin, acc);
    }
}

#endif

#else

static _Inline void risc_dsp_mdct_q31(const mdct_q31_t* handle, const q31_t* input, q31_t* output)
{
    int32_t i;
    dspfx_accum64()
    int32_t N = handle->length >> 2;
    q31_t xSin, xCos;
    q31_t* prerot_tab  = handle->prerot;
    q31_t* postrot_tab = handle->postrot;

    // MDCT preprocess
    for (i = 0; i < N; i++)
    {
        output[i] = dspfx_neg_q31(dspfx_add_q31(input[(3*N-1)-i], input[3*N+i]));
        output[N+i] =       dspfx_sub_q31(input[i],     input[(2*N-1)-i]);
    }

    // DCT4 Prerotation
    for (i = 0; i < N>>1; i++)
    {
        q31_t tmp0_re = COMPLEX_RE(output, i);
        q31_t tmp0_im = COMPLEX_IM(output, i);
        q31_t tmp1_re = COMPLEX_RE(output, N-i-1);
        q31_t tmp1_im = COMPLEX_IM(output, N-i-1);
        xCos = *prerot_tab++;
        xSin = *prerot_tab++;
                            dspfx_a64mul_q31( tmp0_re, xCos, acc);
        COMPLEX_RE(output, i) =    dspfx_a64mac_q31( tmp1_im, xSin, acc);
                            dspfx_a64mul_q31( tmp1_im, xCos, acc);
        COMPLEX_IM(output, i) =    dspfx_a64msu_q31( tmp0_re, xSin, acc);
        xCos = *prerot_tab++; 
        xSin = *prerot_tab++;
                             dspfx_a64mul_q31( tmp0_im, xSin, acc);
        COMPLEX_RE(output, N-i-1) = dspfx_a64mac_q31( tmp1_re, xCos, acc);
                             dspfx_a64mul_q31( tmp0_im, xCos, acc);
        COMPLEX_IM(output, N-i-1) = dspfx_a64msu_q31( tmp1_re, xSin, acc);
    }

    dsp_cfft_q31(&handle->hfft, (PTR(q31_t))output, (PTR(q31_t))output);

    xCos = dsp_q31_cast_rnd_f32( 1.0000000000);
    xSin = dsp_q31_cast_rnd_f32( 0.0000000000);

    // DCT4 Postrotation
    for (i = 0; i < N>>1; i++)             
    {                         
        q31_t tmp0_re = COMPLEX_RE(output, i);
        q31_t tmp0_im = COMPLEX_IM(output, i);
        q31_t tmp1_re = COMPLEX_RE(output, N-i-1);
        q31_t tmp1_im = COMPLEX_IM(output, N-i-1);
                             dspfx_a64mul_q31(tmp0_re, xCos, acc);
        COMPLEX_RE(output, i)  =    dspfx_a64mac_q31(tmp0_im, xSin, acc);
                             dspfx_a64mul_q31(tmp0_re, xSin, acc);
        COMPLEX_IM(output, N-i-1) = dspfx_a64msu_q31(tmp0_im, xCos, acc);
        xCos = *postrot_tab++;
        xSin = *postrot_tab++;
                             dspfx_a64mul_q31(tmp1_re, xSin, acc);
        COMPLEX_RE(output, N-i-1) = dspfx_a64mac_q31(tmp1_im, xCos, acc);
                             dspfx_a64mul_q31(tmp1_re, xCos, acc);
        COMPLEX_IM(output, i)     = dspfx_a64msu_q31(tmp1_im, xSin, acc);
    }                                        
}

#endif

#define XY_MAP_INPUT 0
#define XY_MAP_OUT 0
#define XY_MAP_PREROT_COEF (0)
#define XY_MAP_POSTROT_COEF (((XY_REGION_SIZE/2))/sizeof(q31_t))

/**
 * MDCT "process" function for q31 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_mdct_q31(const mdct_q31_t* handle, const PTR(q31_t) input, PTR(q31_t) output)
{
#if defined(_ARC_AS210_mac32)
    if (handle->length > XY_MDCT_Q31_THRESHOLD) {
#endif
        risc_dsp_mdct_q31(handle, input, output);
#if defined(_ARC_AS210_mac32)
    } else {
        int32_t length_mdct = handle->length;
        int32_t length_dct4 = handle->length/2;
        int32_t length_fft  = handle->length/4;

        memcpy_StoY_int32(XY_MAP_INPUT, input, length_mdct);

        dsp_mdct_preprocess_q31_YtoX(XY_MAP_INPUT, XY_MAP_OUT, length_mdct);

        dsp_dct4_preprocess_init_q31_cfY((dct4_q31_xy_t*)&handle->xy_dct4, length_dct4, XY_MAP_PREROT_COEF);
        dsp_dct4_preprocess_q31_XtoX_cfY_MAC32x32(&handle->xy_dct4, XY_MAP_OUT, XY_MAP_OUT);

        dsp_cfft_sradix_init_q31_cfY((cfft_srad_q31_xy_t*)&handle->xy_fft, length_fft, 0);
        dsp_cfft_sradix_q31_XtoX_cfY_MAC32x32(&handle->xy_fft, XY_MAP_OUT, XY_MAP_OUT); //in-place radix

        dsp_dct4_postprocess_init_q31_cfY((dct4_q31_xy_t*)&handle->xy_dct4, length_dct4, XY_MAP_POSTROT_COEF);
        dsp_dct4_postprocess_q31_XtoX_cfY_MAC32x32(&handle->xy_dct4, XY_MAP_OUT, XY_MAP_OUT);

        memcpy_XtoS_int32(output, XY_MAP_OUT, length_dct4);
    }
#endif
}

void dsp_mdct_delete_q31(mdct_q31_t* handle) 
{
#if defined(_ARC_AS210_mac32)
    if (handle->length > XY_MDCT_Q31_THRESHOLD) {
#endif
        dsp_cfft_delete_q31(&handle->hfft);
#if defined(_ARC_AS210_mac32) 
    } 
#endif
};

#endif //!defined(EXCLUDE_DSP_MDCT_Q31)
