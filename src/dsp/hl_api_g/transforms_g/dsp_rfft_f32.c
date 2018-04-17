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

#if !defined(EXCLUDE_DSP_RFFT_F32)
#pragma Code("dsplib_dsp_rfft_f32")

#include "dsplib.h"
#include "dsp_transform_tables.h"

/**
 * Initializes instance structure of real fft transform
 * for f32 data type. 
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_rfft_getsize_f32()" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_rfft_init_f32(rfft_f32_t* handle, uint32_t length, PTR(f32_t) pState)
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
    if (dsp_cfft_f32_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;

#ifdef _ARCv2EM_XY_11D
    handle->ptwd = (f32_t*)data_ptr; data_ptr += length * sizeof(f32_t);
    memcpy(handle->ptwd, dsp_cfft_f32_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(f32_t));
#else
    handle->ptwd = (f32_t*)dsp_cfft_f32_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif
    // By default RFFT is unscaled, produce full spectrum
    handle->scaling = DSP_TRANSFORM_SCALING_UNSCALED;
    handle->spectrum_format = DSP_TRANSFORM_FULL_SPECTRUM;
    return dsp_cfft_init_f32(&handle->hfft, length/2, (PTR(f32_t))data_ptr);
};

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__ && defined __GUARD_ACCUM__

#if defined _XY_HAND_OPT

static void dsp_rfft_postprocess_f32(const rfft_f32_t* handle, const f32_t* input, f32_t* output) {
    uint32_t i;
    uint32_t length = handle->length;
    uint32_t length2 = length/2;
	f32_t Re_out_Nplus1;
	f32_t Im_out_Nplus1;

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
	f32_t nCos = _as_float(_agu_u5())*0.5;//>>1;
	f32_t nSin = _as_float(_agu_u5())*0.5;//>>1;
	f32_t Are = (0.5)+nSin;
	f32_t Bre = (0.5)-nSin;
	f32_t Aim =  nCos;


        // Branch for the unscaled RFFT
                  f32_t  acc = MPY_F32(_as_float(_agu_u0()), Are);
                   MSU_F32(acc, _as_float(_agu_u1()), Aim);
                   MAC_F32(acc, _as_float(_agu_u0()), Bre);
        _st_agu_u4(_as_uint32(MSU_F32(acc, _as_float(_agu_u0()), Aim)));
        _st_agu_u4(0);
        // main processing loop. calculate first half of "samples"
        for (i = 1; i < length2; i++)
        {
            f32_t nCos = _as_float(_agu_u5())*0.5;//>>1;
            f32_t nSin = _as_float(_agu_u5())*0.5;//>>1;
            f32_t Are = ADD_F32((0.5), nSin);
			f32_t Bre = SUB_F32((0.5), nSin);
            f32_t Aim =  nCos;
//            int32_t Bim = dspfx_neg_q31(nCos);
                       acc = MPY_F32(_as_float(_agu_u0()), Are);
                       MSU_F32(acc, _as_float(_agu_u1()), Aim);
                       MSU_F32(acc, _as_float(_agu_u2()), Aim);
            _st_agu_u4(_as_uint32(MAC_F32(acc, _as_float(_agu_u3()), Bre)));
			           acc =MPY_F32(_as_float(_agu_u0()), Aim);
                       MAC_F32(acc, _as_float(_agu_u0()), Are);
			           MSU_F32(acc, _as_float(_agu_u2()), Bre);
            _st_agu_u4(_as_uint32(MSU_F32(acc, _as_float(_agu_u2()), Aim)));
        }
        // calculate middle "sample"
        Re_out_Nplus1 = COMPLEX_RE(input,0) - COMPLEX_IM(input,0);
        Im_out_Nplus1 = 0;

    switch (handle->spectrum_format)
    {
    case DSP_TRANSFORM_HALF_SPECTRUM:
        // Copy "middle" sample to its position. We have N/2+1 samples in output
        _st_agu_u4(_as_uint32(Re_out_Nplus1));
        _st_agu_u4(_as_uint32(Im_out_Nplus1));
        break;
    case DSP_TRANSFORM_HALF_SPECTRUM_PACKED:
        // Pack output samples
        output[1] = Re_out_Nplus1;
        break;
    case DSP_TRANSFORM_FULL_SPECTRUM:
    default:
        _st_agu_u4(_as_uint32(Re_out_Nplus1));
        _st_agu_u4(_as_uint32(Im_out_Nplus1));
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

static void dsp_rfft_postprocess_f32(const rfft_f32_t* handle, const __xy f32_t* input, __xy f32_t* output) {
    uint32_t i;
    uint32_t length = handle->length;
    uint32_t length2 = length/2;
    __xy f32_t* pTWD = (__xy f32_t*) handle->ptwd;

    // calculate first "sample";
    f32_t nCos = (*pTWD++)*0.5;//>>1;
    f32_t nSin = (*pTWD++)*0.5;//>>1;
    f32_t Are = ADD_F32((0.5),nSin);
    f32_t Bre = SUB_F32((0.5),nSin);
    f32_t Aim =  nCos;
    f32_t Re_out_Nplus1;
    f32_t Im_out_Nplus1;


        // Branch for the unscaled RFFT
                               f32_t acc = MPY_F32(input[0], Are);
                               MAC_F32(acc, input[0], Bre);
                               MSU_F32(acc, input[1], Aim);
        COMPLEX_RE(output,0) = MSU_F32(acc, input[1], Aim);
        COMPLEX_IM(output,0) = 0;

        // main processing loop. calculate first half of "samples"
        for (i = 1; i < length2; i++)
        {
            f32_t nCos = (*pTWD++)*0.5;//>>1;
            f32_t nSin = (*pTWD++)*0.5;//>>1;
            f32_t Are = ADD_F32((0.5), nSin);
            f32_t Bre = SUB_F32((0.5), nSin);
            f32_t Aim = nCos;

                                   acc = MPY_F32(COMPLEX_RE(input,i), Are);
                                   MSU_F32(acc, COMPLEX_IM(input,i), Aim);
                                   MAC_F32(acc, COMPLEX_RE(input,(length2 - i)), Bre);
            COMPLEX_RE(output,i) = MSU_F32(acc, COMPLEX_IM(input,(length2 - i)), Aim);

                                   acc = MPY_F32(COMPLEX_IM(input,i), Are);
                                   MAC_F32(acc, COMPLEX_RE(input,i), Aim);
                                   MSU_F32(acc, COMPLEX_RE(input,(length2 - i)), Aim);
            COMPLEX_IM(output,i) = MSU_F32(acc, COMPLEX_IM(input,(length2 - i)), Bre);
        }

        // calculate middle "sample"
        Re_out_Nplus1 = COMPLEX_RE(input,0) - COMPLEX_IM(input,0);
        Im_out_Nplus1 = 0;


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
            COMPLEX_IM(output,i) = -(COMPLEX_IM(output,(length - i)));
        }
        break;
    }

};

#endif

#else

static void dsp_rfft_postprocess_f32(const rfft_f32_t* handle, const f32_t* input, f32_t* output) {
    uint32_t i;
    uint32_t length = handle->length;
    uint32_t length2 = length/2;
    f32_t* pTWD = handle->ptwd;

    // calculate first "sample";
    f32_t nCos = (*pTWD++)*0.5;//>>1;
    f32_t nSin = (*pTWD++)*0.5;//>>1;
    f32_t Are = ADD_F32((0.5),nSin);
    f32_t Aim =       nCos;
    f32_t Bre = SUB_F32((0.5),nSin);
    f32_t Bim =      -nCos;
    f32_t Re_out_Nplus1;
    f32_t Im_out_Nplus1;

        // Branch for the unscaled RFFT
                               f32_t acc = MPY_F32(input[0], Are);
                               MAC_F32(acc, input[0], Bre);
                               MAC_F32(acc, input[1], Bim);
        COMPLEX_RE(output,0) = MSU_F32(acc, input[1], Aim);
        COMPLEX_IM(output,0) = 0;

        // main processing loop. calculate first half of "samples"
        for (i = 1; i < length2; i++)
        {
            f32_t nCos = (*pTWD++)*0.5;//>>1;
            f32_t nSin = (*pTWD++)*0.5;//>>1;
            f32_t Are = ((0.5) + nSin);
            f32_t Aim = nCos;
            f32_t Bre = ((0.5) - nSin);
            f32_t Bim = -(nCos);

            acc = MPY_F32(COMPLEX_RE(input,i), Are);
            MSU_F32(acc, COMPLEX_IM(input,i), Aim);
            MAC_F32(acc, COMPLEX_RE(input,(length2 - i)), Bre);
            COMPLEX_RE(output,i) = MAC_F32(acc, COMPLEX_IM(input,(length2 - i)), Bim);

            acc = MPY_F32(COMPLEX_IM(input,i), Are);
            MAC_F32(acc, COMPLEX_RE(input,i), Aim);
            MAC_F32(acc, COMPLEX_RE(input,(length2 - i)), Bim);
            COMPLEX_IM(output,i) = MSU_F32(acc, COMPLEX_IM(input,(length2 - i)), Bre);
        }

        // calculate middle "sample"
        Re_out_Nplus1 = COMPLEX_RE(input,0) - COMPLEX_IM(input,0);
        Im_out_Nplus1 = 0;

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
            COMPLEX_IM(output,i) = -(COMPLEX_IM(output,(length - i)));
        }
        break;
    }

};

#endif

#pragma Off_inline(dsp_rfft_postprocess_f32)


static _Inline void risc_dsp_rfft_f32(const rfft_f32_t* handle, const f32_t* input, f32_t* output) 
{
    dsp_cfft_f32(&handle->hfft, (PTR(f32_t))input, (PTR(f32_t))(output + handle->length));
    dsp_rfft_postprocess_f32(handle, (PTR(f32_t)) (output + handle->length), (PTR(f32_t))output);
};

#define XY_MAP_INPUT  (((XY_REGION_SIZE/2))/sizeof(f32_t))
#define XY_MAP_OUTPUT 0
#define XY_MAP_TWIDDLES 0
#define XY_RFFT_F32_THRESHOLD (((XY_REGION_SIZE/2))/sizeof(f32_t))

/**
 * Real FFT "process" function for f32 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */


void _DSPLIB_XYCC dsp_rfft_f32(const rfft_f32_t* rfft_handle, const PTR(f32_t) input, PTR(f32_t) output)
{
    // Copy scaling parameter to cfft handle
    cfft_f32_t* cfft_handle = (cfft_f32_t*) &rfft_handle->hfft;
    cfft_handle->scaling = rfft_handle->scaling;
    risc_dsp_rfft_f32(rfft_handle, input, output);

};

void dsp_rfft_delete_f32(rfft_f32_t* handle)
{
    dsp_cfft_delete_f32(&handle->hfft);
}

#endif //!defined(EXCLUDE_DSP_RFFT_F32)
