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

#if !defined(EXCLUDE_DSP_DCT2_Q31)
#pragma Code("dsplib_dsp_dct2_q31")

#if defined(_ARC_AS210_mac32) 
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsp_transform_tables.h"

/**
 * DCT Type II instance initialization function for q31 data type
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_dct2_getsize_q31()" function.
 */

dsp_status _DSPLIB_XYCC dsp_dct2_init_q31(dct2_q31_t* handle, uint32_t length, PTR(uint32_t) pState)
{
    int32_t order = 30-dspfx_norm_q31(length);
    uint8_t* data_ptr = (uint8_t *)(void*) pState;
    if ((length > DSP_MAX_TRANSFORM_SIZE))
        return DSP_ERR_LENGTH_TOO_BIG;
    if ((length < 8))
        return DSP_ERR_LENGTH_TOO_SMALL;
    if (length != (1 << order))
        return DSP_ERR_LENGTH_NOT_POWER2;
    if (dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;
    if (dsp_dct23_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;

    handle->length = length;

#ifdef _ARCv2EM_XY_11D
    handle->ptwd = (q31_t*)data_ptr; data_ptr += length * sizeof(q31_t)/2;
    memcpy(handle->ptwd, dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(q31_t)/2);
    handle->ptwd4 = (q31_t*)data_ptr; data_ptr += length * sizeof(q31_t);
    memcpy(handle->ptwd4, dsp_dct23_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(q31_t));
 #else
    handle->ptwd = (q31_t*)dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
    handle->ptwd4= (q31_t*)dsp_dct23_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif    
    handle->temp_buffer = (q31_t *)(void*) data_ptr; data_ptr += length * sizeof(q31_t);
    return dsp_cfft_init_q31(&handle->hfft, length / 2, (PTR(uint32_t))(void*) data_ptr);
}

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__ && defined __GUARD_ACCUM__ && defined _XY_HAND_OPT

static void _DSPLIB_XYCC risc_dsp_dct2_q31(const dct2_q31_t* handle, const q31_t* input, q31_t* output)
{
    int32_t i;
    int32_t N = handle->length/2;
    int32_t L = handle->length;
    dspfx_accum64()
    int32_t acc32;
    q31_t* buffer = handle->temp_buffer;
    int32_t xCos, xSin;

    _DSPLIB_store_regs(4, 6, 0);


	_set_agu_ap0(input);
	_set_agu_ap1(output);
	_set_agu_ap2(output + L - 1);

	_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(2));
	_set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod2(0, AGU_DT_q31, AGU_AD_INC_q31(1));

	_set_agu_mod3(1, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod4(2, AGU_DT_q31, AGU_AD_DEC_q31(1));

    for (i = 0; i < N >> 1; i++) 
    {
        _st_agu_u3(_agu_u0()); // COMPLEX_RE(buffer, i)         = input[ (i << 2)     ];
        _st_agu_u3(_agu_u1()); // COMPLEX_IM(buffer, i)         = input[ (i << 2) + 2 ];
        _st_agu_u4(_agu_u0()); // COMPLEX_IM(buffer, N - i - 1) = input[ (i << 2) + 1 ];
        _st_agu_u4(_agu_u2()); // COMPLEX_RE(buffer, N - i - 1) = input[ (i << 2) + 3 ];
    }

    dsp_cfft_q31(&handle->hfft, (PTR(q31_t))output, (PTR(q31_t))buffer);

	_set_agu_ap0(buffer + 2);
	_set_agu_ap1(buffer + L - 2);
	_set_agu_ap2(handle->ptwd + 2);

	_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod2(1, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod3(1, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod4(1, AGU_DT_q31, AGU_AD_DEC_q31(3));
	_set_agu_mod5(2, AGU_DT_q31, AGU_AD_INC_q31(1));

    for (i = 1; i < N>>1; i++) 
    {
        int32_t a1, a2, a3, a4, a5, a6;

        xCos = dspfx_neg_q31(_agu_u5());
        xSin = dspfx_neg_q31(_agu_u5());

        a1 = dspfx_add_q31(_agu_u0(), _agu_u2())>>1;
        a2 = dspfx_add_q31(_agu_u1(), _agu_u3())>>1;
        a3 = dspfx_sub_q31(_agu_u2(), _agu_u0())>>1;
        a4 = dspfx_sub_q31(_agu_u1(), _agu_u3())>>1;

             dspfx_a64mul_q31(a2, xCos, acc);
        a5 = dspfx_a64mac_q31(a3, xSin, acc);
             dspfx_a64mul_q31(a2, xSin, acc);
        a6 = dspfx_a64msu_q31(a3, xCos, acc);

        _st_agu_u0(dspfx_add_q31(a1, a5));
        _st_agu_u0(dspfx_sub_q31(a6, a4));
        _st_agu_u2(dspfx_sub_q31(a1, a5));
        _st_agu_u4(dspfx_add_q31(a6, a4));
    }

    output[0] = dspfx_add_q31(COMPLEX_RE(buffer, 0), COMPLEX_IM(buffer, 0));

	_set_agu_ap0(buffer + 2);
	_set_agu_ap1(output + 1);
	_set_agu_ap2(output + L - 1);
	_set_agu_ap3(handle->ptwd4);

	_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod2(1, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod3(2, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod4(3, AGU_DT_q31, AGU_AD_INC_q31(1));

    for (i = 1; i < N; i++) 
    {
        xCos = _agu_u4();
        xSin = _agu_u4();

                   dspfx_a64mul_q31(_agu_u0(), xCos, acc);
        _st_agu_u2(dspfx_a64msu_q31(_agu_u1(), xSin, acc));
                   dspfx_a64mul_q31(_agu_u0(), xSin, acc);
        _st_agu_u3(dspfx_a64mac_q31(_agu_u0(), xCos, acc));
    }

    acc32 = dspfx_sub_q31(COMPLEX_RE(buffer, 0), COMPLEX_IM(buffer, 0));
    output[N] = dspfx_a64mul_q31(acc32, _agu_u4(), acc); //xCos = _agu_u4();

    _DSPLIB_restore_regs();
}

#pragma Off_inline(risc_dsp_dct2_q31)

#else

static _Inline void risc_dsp_dct2_q31(const dct2_q31_t* handle, const PTR(q31_t) input, PTR(q31_t) output) 
{
    int32_t i;
    int32_t N = handle->length /2;
    int32_t L = handle->length;
    dspfx_accum64()
    int32_t acc32;
    PTR(q31_t) buffer = (PTR(q31_t)) handle->temp_buffer;
    int32_t xCos, xSin;

    PTR(q31_t) pTWD1 = (PTR(q31_t)) handle->ptwd+2;
    PTR(q31_t) pTWD4 = (PTR(q31_t)) handle->ptwd4;

    for (i = 0; i < N >> 1; i++) 
    {
        COMPLEX_RE(buffer, i)         = input[ (i << 2)     ];
        COMPLEX_IM(buffer, i)         = input[ (i << 2) + 2 ];
        COMPLEX_IM(buffer, N - i - 1) = input[ (i << 2) + 1 ];
        COMPLEX_RE(buffer, N - i - 1) = input[ (i << 2) + 3 ];
    }

    dsp_cfft_q31(&handle->hfft, (PTR(q31_t))buffer, (PTR(q31_t))buffer);

    for (i = 1; i < N>>1; i++) 
    {
        int32_t a1, a2, a3, a4, a5, a6;

        xCos = dspfx_neg_q31(*pTWD1++);
        xSin = dspfx_neg_q31(*pTWD1++);

        a1 = dspfx_add_q31(COMPLEX_RE(buffer, i),     COMPLEX_RE(buffer, N - i))>>1;
        a2 = dspfx_add_q31(COMPLEX_IM(buffer, i),     COMPLEX_IM(buffer, N - i))>>1;
        a3 = dspfx_sub_q31(COMPLEX_RE(buffer, N - i), COMPLEX_RE(buffer, i))>>1;
        a4 = dspfx_sub_q31(COMPLEX_IM(buffer, i),     COMPLEX_IM(buffer, N - i))>>1;

             dspfx_a64mul_q31(a2, xCos, acc);
        a5 = dspfx_a64mac_q31(a3, xSin, acc);
             dspfx_a64mul_q31(a2, xSin, acc);
        a6 = dspfx_a64msu_q31(a3, xCos, acc);

        COMPLEX_RE(buffer, i) = dspfx_add_q31(a1, a5);
        COMPLEX_IM(buffer, i) = dspfx_sub_q31(a6, a4);
        COMPLEX_RE(buffer, N - i) = dspfx_sub_q31(a1, a5);
        COMPLEX_IM(buffer, N - i) = dspfx_add_q31(a6, a4);
    }


    output[0] = dspfx_add_q31(COMPLEX_RE(buffer, 0), COMPLEX_IM(buffer, 0));


    for (i = 1; i < N; i++) 
    {
        xCos = *pTWD4++;
        xSin = *pTWD4++;

                    dspfx_a64mul_q31(COMPLEX_RE(buffer, i), xCos, acc);
        output[i] = dspfx_a64msu_q31(COMPLEX_IM(buffer, i), xSin, acc);
                        dspfx_a64mul_q31(COMPLEX_IM(buffer, i),  xCos, acc);
        output[L - i] = dspfx_a64mac_q31(COMPLEX_RE(buffer, i),  xSin, acc);
    }

    xCos = *pTWD4;
    acc32 = dspfx_sub_q31(COMPLEX_RE(buffer, 0), COMPLEX_IM(buffer, 0));
    output[N] = dspfx_a64mul_q31(acc32, xCos, acc);
}

#endif

#define XY_MAP_INPUT    0
#define XY_MAP_OUT      (((XY_REGION_SIZE/2))/sizeof(q31_t))
#define XY_MAP_TMP      (((XY_REGION_SIZE/2))/sizeof(q31_t))
#define XY_DCT2_Q31_THRESHOLD (((XY_REGION_SIZE/4))/sizeof(q31_t))

/**
 * DCT Type II "process" function for q31 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_dct2_q31(const dct2_q31_t* handle, const PTR(q31_t) input, PTR(q31_t) output)
{
#if defined(_ARC_AS210_mac32)
    if (handle->length > XY_DCT2_Q31_THRESHOLD) {
#endif
        risc_dsp_dct2_q31(handle, input, output);
#if defined(_ARC_AS210_mac32)
    } else {
        int32_t dct2_length = handle->length;
        cfft_srad_q31_xy_t pInst;

        memcpy_StoX_int32(XY_MAP_INPUT, (const int32_t * )input, dct2_length);

        dsp_dct2_preprocess_q31_XtoY(
            XY_MAP_INPUT, 
            XY_MAP_TMP, 
            dct2_length);

        dsp_cfft_sradix_init_q31_cfY(
            &pInst, 
            handle->length/2, 
            0);
        pInst.shiftBits = handle->hfft.shiftBits;
        dsp_cfft_sradix_q31_YtoX_cfY_MAC32x32(
            &pInst,
            XY_MAP_TMP,
            XY_MAP_TMP);

        dsp_dct2_postprocess_init_q31_cfY(
            (dct2_q31_xy_t*)&handle->dct2_xy,
            handle->length,
            0);
        dsp_dct2_postprocess_q31_XtoY_cfY_MAC32x32(
            &handle->dct2_xy, 
            XY_MAP_TMP, 
            XY_MAP_OUT);

        memcpy_YtoS_int32((int32_t *)output, XY_MAP_OUT, dct2_length);
    }
#endif
}

void dsp_dct2_delete_q31(dct2_q31_t* handle) 
{
    dsp_cfft_delete_q31(&handle->hfft);
};

#endif //!defined(EXCLUDE_DSP_DCT2_Q31)
