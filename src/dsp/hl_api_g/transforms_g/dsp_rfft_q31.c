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

#if !defined(EXCLUDE_DSP_RFFT_Q31)
#pragma Code("dsplib_dsp_rfft_q31")

#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsp_transform_tables.h"

/**
 * Initializes instance structure of real fft transform
 * for q31 data type. 
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_rfft_getsize_q31()" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_rfft_init_q31(rfft_q31_t* handle, uint32_t length, PTR(uint32_t) pState)
{
    int32_t order = 30-dspfx_norm_q31(length);
    uint8_t* data_ptr = (uint8_t*) (void*) pState;    
    handle->length = length;
    if ((length > DSP_MAX_TRANSFORM_SIZE))
        return DSP_ERR_LENGTH_TOO_BIG;
    if ((length < 8))
        return DSP_ERR_LENGTH_TOO_SMALL;
    if (length != (1 << order))
        return DSP_ERR_LENGTH_NOT_POWER2;
    if (dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;

#ifdef _ARCv2EM_XY_11D
    handle->ptwd = (q31_t*)data_ptr; data_ptr += length * sizeof(q31_t);
    memcpy(handle->ptwd, dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(q31_t));
#else
    handle->ptwd = (q31_t*)dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif
    // By default RFFT is unscaled, produce full spectrum
    handle->scaling = DSP_TRANSFORM_SCALING_UNSCALED;
    handle->spectrum_format = DSP_TRANSFORM_FULL_SPECTRUM;
    return dsp_cfft_init_q31(&handle->hfft, length/2, (PTR(uint32_t))data_ptr);
};

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__ && defined __GUARD_ACCUM__

#if defined _XY_HAND_OPT

static void dsp_rfft_postprocess_q31(const rfft_q31_t* handle, const q31_t* input, q31_t* output) {
    uint32_t i;
    dspfx_accum64()
    uint32_t length = handle->length;
    uint32_t length2 = length/2;
	int32_t Re_out_Nplus1;
	int32_t Im_out_Nplus1;

	_DSPLIB_store_regs(4, 6, 0);

	_set_agu_ap0(input);
	_set_agu_ap1(input + length - 1);
	_set_agu_ap2(output);
	_set_agu_ap3(handle->ptwd);

	_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod2(1, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod3(1, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod4(2, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod5(3, AGU_DT_q31, AGU_AD_INC_q31(1));

	// calculate first "sample";
	q31_t nCos = ((int32_t)_agu_u5())>>1;
	q31_t nSin = ((int32_t)_agu_u5())>>1;
	int32_t Are = dsp_q31_cast_rnd_f32(0.5)+nSin;
	int32_t Bre = dsp_q31_cast_rnd_f32(0.5)-nSin;
	int32_t Aim =  nCos;


    if (handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
        // Branch for the scaled RFFT
                   dspfx_a64mul_nf_q31(_agu_u0(), Are, acc);
                   dspfx_a64msu_nf_q31(_agu_u1(), Aim, acc);
                   dspfx_a64mac_nf_q31(_agu_u0(), Bre, acc);
        _st_agu_u4(dspfx_a64msu_nf_q31(_agu_u0(), Aim, acc));
        _st_agu_u4(0);
        // main processing loop. calculate first half of "samples"
        for (i = 1; i < length2; i++)
        {
            q31_t nCos = ((int32_t)_agu_u5())>>1;
            q31_t nSin = ((int32_t)_agu_u5())>>1;
            int32_t Are = dspfx_add_q31(dsp_q31_cast_rnd_f32(0.5), nSin);
			int32_t Bre = dspfx_sub_q31(dsp_q31_cast_rnd_f32(0.5), nSin);
            int32_t Aim =  nCos;
//            int32_t Bim = dspfx_neg_q31(nCos);
                       dspfx_a64mul_nf_q31(_agu_u0(), Are, acc);
                       dspfx_a64msu_nf_q31(_agu_u1(), Aim, acc);
                       dspfx_a64msu_nf_q31(_agu_u2(), Aim, acc);
            _st_agu_u4(dspfx_a64mac_nf_q31(_agu_u3(), Bre, acc));
			           dspfx_a64mul_nf_q31(_agu_u0(), Aim, acc);
                       dspfx_a64mac_nf_q31(_agu_u0(), Are, acc);
			           dspfx_a64msu_nf_q31(_agu_u2(), Bre, acc);
            _st_agu_u4(dspfx_a64msu_nf_q31(_agu_u2(), Aim, acc));
        }
        // calculate middle "sample"
        Re_out_Nplus1 = (COMPLEX_RE(input,0) - COMPLEX_IM(input,0)) >> 1;
        Im_out_Nplus1 = 0;
    } else {
        // Branch for the unscaled RFFT
                   dspfx_a64mul_q31(_agu_u0(), Are, acc);
                   dspfx_a64msu_q31(_agu_u1(), Aim, acc);
                   dspfx_a64mac_q31(_agu_u0(), Bre, acc);
        _st_agu_u4(dspfx_a64msu_q31(_agu_u0(), Aim, acc));
        _st_agu_u4(0);
        // main processing loop. calculate first half of "samples"
        for (i = 1; i < length2; i++)
        {
            q31_t nCos = ((int32_t)_agu_u5())>>1;
            q31_t nSin = ((int32_t)_agu_u5())>>1;
            int32_t Are = dspfx_add_q31(dsp_q31_cast_rnd_f32(0.5), nSin);
			int32_t Bre = dspfx_sub_q31(dsp_q31_cast_rnd_f32(0.5), nSin);
            int32_t Aim =  nCos;
//            int32_t Bim = dspfx_neg_q31(nCos);
                       dspfx_a64mul_q31(_agu_u0(), Are, acc);
                       dspfx_a64msu_q31(_agu_u1(), Aim, acc);
                       dspfx_a64msu_q31(_agu_u2(), Aim, acc);
            _st_agu_u4(dspfx_a64mac_q31(_agu_u3(), Bre, acc));
			           dspfx_a64mul_q31(_agu_u0(), Aim, acc);
                       dspfx_a64mac_q31(_agu_u0(), Are, acc);
			           dspfx_a64msu_q31(_agu_u2(), Bre, acc);
            _st_agu_u4(dspfx_a64msu_q31(_agu_u2(), Aim, acc));
        }
        // calculate middle "sample"
        Re_out_Nplus1 = COMPLEX_RE(input,0) - COMPLEX_IM(input,0);
        Im_out_Nplus1 = 0;
    }    

    switch (handle->spectrum_format)
    {
    case DSP_TRANSFORM_HALF_SPECTRUM:
        // Copy "middle" sample to its position. We have N/2+1 samples in output
        _st_agu_u4(Re_out_Nplus1);
        _st_agu_u4(Im_out_Nplus1);
        break;
    case DSP_TRANSFORM_HALF_SPECTRUM_PACKED:
        // Pack output samples
        output[1] = Re_out_Nplus1;
        break;
    case DSP_TRANSFORM_FULL_SPECTRUM:
    default:
        _st_agu_u4(Re_out_Nplus1);
        _st_agu_u4(Im_out_Nplus1);
		_set_agu_ap0(output + length - 2);
		_set_agu_ap1(output + length - 1);
		_set_agu_mod0(0, AGU_DT_q31, AGU_AD_DEC_q31(2));
		_set_agu_mod1(1, AGU_DT_q31, AGU_AD_DEC_q31(2));

        // calculate second half of "samples"
        for (i = 1; i<length2; i++)
        {
            _st_agu_u4(_agu_u0());
            _st_agu_u4(dspfx_neg_q31(_agu_u1()));
        }
        break;
    }

    _DSPLIB_restore_regs();
};

#else

static void dsp_rfft_postprocess_q31(const rfft_q31_t* handle, const __xy q31_t* input, __xy q31_t* output) {
    uint32_t i;
    dspfx_accum64()
    uint32_t length = handle->length;
    uint32_t length2 = length/2;
    __xy q31_t* pTWD = (__xy q31_t*) handle->ptwd;

    // calculate first "sample";
    q31_t nCos = (*pTWD++)>>1;
    q31_t nSin = (*pTWD++)>>1;
    int32_t Are = dsp_q31_cast_rnd_f32(0.5)+nSin;
    int32_t Bre = dsp_q31_cast_rnd_f32(0.5)-nSin;
    int32_t Aim =  nCos;
    int32_t Re_out_Nplus1;
    int32_t Im_out_Nplus1;

    if (handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
        // Branch for the scaled RFFT
                               dspfx_a64mul_nf_q31(input[0], Are, acc);
                               dspfx_a64mac_nf_q31(input[0], Bre, acc);
                               dspfx_a64msu_nf_q31(input[1], Aim, acc);
        COMPLEX_RE(output,0) = dspfx_a64msu_nf_q31(input[1], Aim, acc);
        COMPLEX_IM(output,0) = 0;

        // main processing loop. calculate first half of "samples"
        for (i = 1; i < length2; i++)
        {
            q31_t nCos = (*pTWD++)>>1;
            q31_t nSin = (*pTWD++)>>1;
            int32_t Are = dspfx_add_q31(dsp_q31_cast_rnd_f32(0.5), nSin);
            int32_t Bre = dspfx_sub_q31(dsp_q31_cast_rnd_f32(0.5), nSin);
            int32_t Aim = nCos;

                                   dspfx_a64mul_nf_q31(COMPLEX_RE(input,i), Are, acc);
                                   dspfx_a64msu_nf_q31(COMPLEX_IM(input,i), Aim, acc);
                                   dspfx_a64mac_nf_q31(COMPLEX_RE(input,(length2 - i)), Bre, acc);
            COMPLEX_RE(output,i) = dspfx_a64msu_nf_q31(COMPLEX_IM(input,(length2 - i)), Aim, acc);

                                   dspfx_a64mul_nf_q31(COMPLEX_IM(input,i), Are, acc);
                                   dspfx_a64mac_nf_q31(COMPLEX_RE(input,i), Aim, acc);
                                   dspfx_a64msu_nf_q31(COMPLEX_RE(input,(length2 - i)), Aim, acc);
            COMPLEX_IM(output,i) = dspfx_a64msu_nf_q31(COMPLEX_IM(input,(length2 - i)), Bre, acc);
        }

        // calculate middle "sample"
        Re_out_Nplus1 = (COMPLEX_RE(input,0) - COMPLEX_IM(input,0)) >> 1;
        Im_out_Nplus1 = 0;
    } else {
        // Branch for the unscaled RFFT
                               dspfx_a64mul_q31(input[0], Are, acc);
                               dspfx_a64mac_q31(input[0], Bre, acc);
                               dspfx_a64msu_q31(input[1], Aim, acc);
        COMPLEX_RE(output,0) = dspfx_a64msu_q31(input[1], Aim, acc);
        COMPLEX_IM(output,0) = 0;

        // main processing loop. calculate first half of "samples"
        for (i = 1; i < length2; i++)
        {
            q31_t nCos = (*pTWD++)>>1;
            q31_t nSin = (*pTWD++)>>1;
            int32_t Are = dspfx_add_q31(dsp_q31_cast_rnd_f32(0.5), nSin);
            int32_t Bre = dspfx_sub_q31(dsp_q31_cast_rnd_f32(0.5), nSin);
            int32_t Aim = nCos;

                                   dspfx_a64mul_q31(COMPLEX_RE(input,i), Are, acc);
                                   dspfx_a64msu_q31(COMPLEX_IM(input,i), Aim, acc);
                                   dspfx_a64mac_q31(COMPLEX_RE(input,(length2 - i)), Bre, acc);
            COMPLEX_RE(output,i) = dspfx_a64msu_q31(COMPLEX_IM(input,(length2 - i)), Aim, acc);

                                   dspfx_a64mul_q31(COMPLEX_IM(input,i), Are, acc);
                                   dspfx_a64mac_q31(COMPLEX_RE(input,i), Aim, acc);
                                   dspfx_a64msu_q31(COMPLEX_RE(input,(length2 - i)), Aim, acc);
            COMPLEX_IM(output,i) = dspfx_a64msu_q31(COMPLEX_IM(input,(length2 - i)), Bre, acc);
        }

        // calculate middle "sample"
        Re_out_Nplus1 = COMPLEX_RE(input,0) - COMPLEX_IM(input,0);
        Im_out_Nplus1 = 0;

    }

    switch (handle->spectrum_format)
    {
    case DSP_TRANSFORM_HALF_SPECTRUM:
        // Copy "middle" sample to its position. We have N/2+1 samples in output
        COMPLEX_RE(output,i) = Re_out_Nplus1;
        COMPLEX_IM(output,i) = Im_out_Nplus1;
        break;
    case DSP_TRANSFORM_HALF_SPECTRUM_PACKED:
        // Pack output samples
        output[1] = Re_out_Nplus1;
        break;
    case DSP_TRANSFORM_FULL_SPECTRUM:
    default:
        COMPLEX_RE(output,i) = Re_out_Nplus1;
        COMPLEX_IM(output,i) = Im_out_Nplus1;
        i++;
        // calculate second half of "samples"
        for (; i<length; i++)
        {
            COMPLEX_RE(output,i) = COMPLEX_RE(output,(length - i));
            COMPLEX_IM(output,i) = dspfx_neg_q31(COMPLEX_IM(output,(length - i)));
        }
        break;
    }

};

#endif

#else

static void dsp_rfft_postprocess_q31(const rfft_q31_t* handle, const q31_t* input, q31_t* output) {
    uint32_t i;
    dspfx_accum64()
    uint32_t length = handle->length;
    uint32_t length2 = length/2;
    q31_t* pTWD = handle->ptwd;

    // calculate first "sample";
    q31_t nCos = (*pTWD++)>>1;
    q31_t nSin = (*pTWD++)>>1;
    int32_t Are = dsp_q31_cast_rnd_f32(0.5)+nSin;
    int32_t Aim =                nCos;
    int32_t Bre = dsp_q31_cast_rnd_f32(0.5)-nSin;
    int32_t Bim =          -nCos;
    int32_t Re_out_Nplus1;
    int32_t Im_out_Nplus1;

    if (handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
        // Branch for the scaled RFFT
                               dspfx_a64mul_nf_q31(input[0], Are, acc);
                               dspfx_a64mac_nf_q31(input[0], Bre, acc);
                               dspfx_a64mac_nf_q31(input[1], Bim, acc);
        COMPLEX_RE(output,0) = dspfx_a64mac_nf_q31(input[1], dspfx_neg_q31(Aim), acc);
        COMPLEX_IM(output,0) = 0;

        // main processing loop. calculate first half of "samples"
        for (i = 1; i < length2; i++)
        {
            q31_t nCos = (*pTWD++)>>1;
            q31_t nSin = (*pTWD++)>>1;
            int32_t Are = dspfx_add_q31(dsp_q31_cast_rnd_f32(0.5), nSin);
            int32_t Aim =                nCos;
            int32_t Bre = dspfx_sub_q31(dsp_q31_cast_rnd_f32(0.5), nSin);
            int32_t Bim =               dspfx_neg_q31(nCos);

            dspfx_a64mul_nf_q31(COMPLEX_RE(input,i), Are, acc);
            dspfx_a64mac_nf_q31(COMPLEX_IM(input,i),dspfx_neg_q31(Aim), acc);
            dspfx_a64mac_nf_q31(COMPLEX_RE(input,(length2 - i)), Bre, acc);
            COMPLEX_RE(output,i) = dspfx_a64mac_nf_q31(COMPLEX_IM(input,(length2 - i)), Bim, acc);

            dspfx_a64mul_nf_q31(COMPLEX_IM(input,i), Are, acc);
            dspfx_a64mac_nf_q31(COMPLEX_RE(input,i), Aim, acc);
            dspfx_a64mac_nf_q31(COMPLEX_RE(input,(length2 - i)), Bim, acc);
            COMPLEX_IM(output,i) = dspfx_a64mac_nf_q31(COMPLEX_IM(input,(length2 - i)),dspfx_neg_q31(Bre), acc);
        }

        // calculate middle "sample"
        Re_out_Nplus1 = (COMPLEX_RE(input,0) - COMPLEX_IM(input,0)) >> 1;
        Im_out_Nplus1 = 0;
    } else {
        // Branch for the unscaled RFFT
                               dspfx_a64mul_q31(input[0], Are, acc);
                               dspfx_a64mac_q31(input[0], Bre, acc);
                               dspfx_a64mac_q31(input[1], Bim, acc);
        COMPLEX_RE(output,0) = dspfx_a64msu_q31(input[1], Aim, acc);
        COMPLEX_IM(output,0) = 0;

        // main processing loop. calculate first half of "samples"
        for (i = 1; i < length2; i++)
        {
            q31_t nCos = (*pTWD++)>>1;
            q31_t nSin = (*pTWD++)>>1;
            int32_t Are = dspfx_add_q31(dsp_q31_cast_rnd_f32(0.5), nSin);
            int32_t Aim = nCos;
            int32_t Bre = dspfx_sub_q31(dsp_q31_cast_rnd_f32(0.5), nSin);
            int32_t Bim = dspfx_neg_q31(nCos);

            dspfx_a64mul_q31(COMPLEX_RE(input,i), Are, acc);
            dspfx_a64msu_q31(COMPLEX_IM(input,i), Aim, acc);
            dspfx_a64mac_q31(COMPLEX_RE(input,(length2 - i)), Bre, acc);
            COMPLEX_RE(output,i) = dspfx_a64mac_q31(COMPLEX_IM(input,(length2 - i)), Bim, acc);

            dspfx_a64mul_q31(COMPLEX_IM(input,i), Are, acc);
            dspfx_a64mac_q31(COMPLEX_RE(input,i), Aim, acc);
            dspfx_a64mac_q31(COMPLEX_RE(input,(length2 - i)), Bim, acc);
            COMPLEX_IM(output,i) = dspfx_a64msu_q31(COMPLEX_IM(input,(length2 - i)), Bre, acc);
        }

        // calculate middle "sample"
        Re_out_Nplus1 = COMPLEX_RE(input,0) - COMPLEX_IM(input,0);
        Im_out_Nplus1 = 0;

    }    

    switch (handle->spectrum_format)
    {
    case DSP_TRANSFORM_HALF_SPECTRUM:
        // Copy "middle" sample to its position. We have N/2+1 samples in output
        COMPLEX_RE(output,i) = Re_out_Nplus1;
        COMPLEX_IM(output,i) = Im_out_Nplus1;
        break;
    case DSP_TRANSFORM_HALF_SPECTRUM_PACKED:
        // Pack output samples
        output[1] = Re_out_Nplus1;
        break;
    case DSP_TRANSFORM_FULL_SPECTRUM:
    default:
        COMPLEX_RE(output,i) = Re_out_Nplus1;
        COMPLEX_IM(output,i) = Im_out_Nplus1;
        i++;
        // calculate second half of "samples"
        for (; i<length; i++)
        {
            COMPLEX_RE(output,i) =  COMPLEX_RE(output,(length - i));
            COMPLEX_IM(output,i) = dspfx_neg_q31(COMPLEX_IM(output,(length - i)));
        }
        break;
    }

};

#endif

#pragma Off_inline(dsp_rfft_postprocess_q31)


static _Inline void risc_dsp_rfft_q31(const rfft_q31_t* handle, const q31_t* input, q31_t* output) 
{
    dsp_cfft_q31(&handle->hfft, (PTR(q31_t))input, (PTR(q31_t))(output + handle->length));
    dsp_rfft_postprocess_q31(handle, (PTR(q31_t)) (output + handle->length), (PTR(q31_t))output);
};

#define XY_MAP_INPUT  (((XY_REGION_SIZE/2))/sizeof(q31_t))
#define XY_MAP_OUTPUT 0
#define XY_MAP_TWIDDLES 0
#define XY_RFFT_Q31_THRESHOLD (((XY_REGION_SIZE/2))/sizeof(q31_t))

/**
 * Real FFT "process" function for q31 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */


void _DSPLIB_XYCC dsp_rfft_q31(const rfft_q31_t* rfft_handle, const PTR(q31_t) input, PTR(q31_t) output)
{
    // Copy scaling parameter to cfft handle
    cfft_q31_t* cfft_handle = (cfft_q31_t*) &rfft_handle->hfft;
    cfft_handle->scaling = rfft_handle->scaling;
    if (rfft_handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
        cfft_handle->shiftBits = 30-dspfx_norm_q31(cfft_handle->length);
    } else {
        cfft_handle->shiftBits = 0;
    }
#if defined(_ARC_AS210_mac32)
    if (rfft_handle->length > XY_RFFT_Q31_THRESHOLD) {
#endif
        risc_dsp_rfft_q31(rfft_handle, input, output);
#if defined(_ARC_AS210_mac32)
    } else {
        cfft_srad_q31_xy_t pInst;
        rfft_q31_xy_t pXYInst;

        dsp_cfft_sradix_init_q31_cfX(
            &pInst, 
            rfft_handle->length/2, 
            XY_MAP_TWIDDLES);

        memcpy_StoX_int32(XY_MAP_INPUT, (int32_t*)(void*)input, rfft_handle->length); 

        pInst.shiftBits = rfft_handle->hfft.shiftBits;

        dsp_cfft_sradix_q31_XtoY_cfX_MAC32x32(
            &pInst,
            XY_MAP_INPUT,
            XY_MAP_OUTPUT);

        dsp_rfft_postprocess_init_q31_cfY(
            &pXYInst, 
            rfft_handle->length, 
            rfft_handle->length);

        dsp_rfft_postprocess_q31_YtoX_cfY_MAC32x32(
            &pXYInst,
            XY_MAP_OUTPUT,
            XY_MAP_OUTPUT);

        memcpy_XtoS_int32((int32_t*)(void*)output, XY_MAP_OUTPUT, rfft_handle->length*2);
    }
#endif
};

void dsp_rfft_delete_q31(rfft_q31_t* handle)
{
    dsp_cfft_delete_q31(&handle->hfft);
}

/**
 * Extended API function.
 * Initializes instance structure of real fft transform
 * for q31 data type. Both Forward and Inverse transform have scaled implementation
 *
 * \param handle - pointer to transform instance structure
 * \param temp0 - reserved parameter
 * \param length - transform size
 * \param flag_ifft - flag that enables inversed transform
 * \param flag_bitrev - flag that indicates bit reversed order of input or output data
 * \result dsp_status
 */

#pragma Code("dsplib_dsp_rfft_q31_ext")

dsp_status dsp_rfft_init_q31_ext(rfft_q31_t *handle, void *temp0,  uint32_t length,  uint32_t flag_ifft,  uint32_t flag_bitrev)
{
    dsp_status status;
    handle->hfft.bitReverse_flag = flag_bitrev;
    handle->hfft.invFlag = flag_ifft;
    if (flag_ifft) {
#if !defined _ARCv2EM_XY_11D        
        status = dsp_rifft_init_q31(handle, length, 0);
#else        
        status = dsp_rifft_init_q31(handle, length, (PTR(uint32_t))temp0);
#endif
        handle->hfft.shiftBits = 29 - dspfx_norm_q31(length);
        handle->hfft.scaling = DSP_TRANSFORM_SCALING_SCALED;
        handle->scaling = DSP_TRANSFORM_SCALING_SCALED;
    } else {
#if !defined _ARCv2EM_XY_11D        
        status = dsp_rfft_init_q31(handle, length, 0);
#else
        status = dsp_rfft_init_q31(handle, length, (PTR(uint32_t))temp0);
#endif
        handle->hfft.shiftBits = 29 - dspfx_norm_q31(length);
        handle->hfft.scaling = DSP_TRANSFORM_SCALING_SCALED;
        handle->scaling = DSP_TRANSFORM_SCALING_SCALED;
    }
    return status;
}

/**
 * Real FFT "process" function for q31 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_rfft_q31_ext(const rfft_q31_t* handle, const PTR(q31_t) input, PTR(q31_t) output)
{
    if (handle->hfft.invFlag) {
        dsp_rifft_q31(handle, input, output);
    } else {
        dsp_rfft_q31(handle, input, output);
#if defined(_ARC_AS210_mac32)
        if (handle->length > XY_RFFT_Q31_THRESHOLD) {
#endif
            dsp_shift_q31(output, 1, output, handle->length*2);
#if defined(_ARC_AS210_mac32)
        }
#endif
    }
}

#endif //!defined(EXCLUDE_DSP_RFFT_Q31)
