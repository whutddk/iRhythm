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

#if !defined(EXCLUDE_DSP_RIFFT_Q31)
#pragma Code("dsplib_dsp_rifft_f32")

#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsp_transform_tables.h"

/**
 * Initializes instance structure of inverse real fft transform
 * for f32 data type. 
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_rifft_getsize_f32()" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_rifft_init_f32(rfft_f32_t* handle, uint32_t length, PTR(f32_t) pState)
{
    int32_t order = 30-dspfx_norm_q31(length);
    f32_t* data_ptr = (f32_t*) (void*) pState;    
    if ((length > DSP_MAX_TRANSFORM_SIZE))
        return DSP_ERR_LENGTH_TOO_BIG;
    if ((length < 8))
        return DSP_ERR_LENGTH_TOO_SMALL;
    if (length != (1 << order))
        return DSP_ERR_LENGTH_NOT_POWER2;
    if (dsp_cfft_f32_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;

    handle->length = length;
#ifdef _ARCv2EM_XY_11D
    handle->ptwd = (f32_t*)data_ptr; data_ptr += length * sizeof(f32_t);
    memcpy(handle->ptwd, dsp_cfft_f32_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(f32_t));
#else
    handle->ptwd = (f32_t*)dsp_cfft_f32_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif
    // By default RIFFT is unscaled, produce full spectrum
    handle->scaling = DSP_TRANSFORM_SCALING_SCALED;
    handle->spectrum_format = DSP_TRANSFORM_FULL_SPECTRUM;
    handle->temp_buffer = (f32_t *)(void*) data_ptr; data_ptr += length * sizeof(f32_t);
    return dsp_cifft_init_f32(&handle->hfft, length/2, (PTR(f32_t))data_ptr);
};


#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__ && defined __GUARD_ACCUM__ && defined _XY_HAND_OPT

static void dsp_rifft_preprocess_f32(const rfft_f32_t* handle,  const f32_t* input, f32_t* output) {
    int32_t i;
    int32_t length2 = handle->length>>1;

    _DSPLIB_store_regs(4, 6, 0);

	_set_agu_ap0(input);
	_set_agu_ap1(input + handle->length + 1);
	_set_agu_ap2(output);
	_set_agu_ap3(handle->ptwd);

	_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod6(0, AGU_DT_q31, AGU_AD_INC_q31(2));
    _set_agu_mod7(0, AGU_DT_q31, AGU_AD_INC_q31(0));
    _set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31(1));

	_set_agu_mod2(1, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod3(1, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod4(2, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod5(3, AGU_DT_q31, AGU_AD_INC_q31(1));

        // Branch for the unscaled RFFT
        for (i = 0; i< length2; i++)
        {
			f32_t nCos = _as_float(_agu_u5())*0.5;//>>1;
			f32_t nSin = _as_float(_agu_u5())*0.5;//>>1;
			f32_t iAre = ADD_F32((0.5),nSin);
			f32_t iBre = SUB_F32((0.5),nSin);
			f32_t iBim = nCos;
                       f32_t acc = MPY_F32( _as_float(_agu_u0()), iAre);
                       MAC_F32(acc, _as_float(_agu_u7()), iBim);
		 	           MAC_F32(acc, _as_float(_agu_u2()), iBim);
                       MAC_F32(acc, _as_float(_agu_u3()), iBre);
            _st_agu_u4(_as_uint32(MPY_F32(acc , 2.0)));//dspfx_q31_cast_asl_a64(acc, 1)
                       acc = MPY_F32( _as_float(_agu_u1()), iAre);
                       MSU_F32(acc, _as_float(_agu_u6()), iBim);
			           MSU_F32(acc, _as_float(_agu_u2()), iBre);
                       MAC_F32(acc, _as_float(_agu_u2()), iBim);
            _st_agu_u4(_as_uint32(MPY_F32(acc, 2.0)));//dspfx_q31_cast_asl_a64(acc, 1)
        }

    _DSPLIB_restore_regs();
};


#else 

static void dsp_rifft_preprocess_f32(const rfft_f32_t* handle,  const PTR(f32_t) input, PTR(f32_t) output) {
    int32_t i;
    int32_t length2 = handle->length>>1;
    PTR(f32_t) pTWD = (PTR(f32_t))handle->ptwd;
            // Branch for the unscaled RFFT
        for (i = 0; i< length2; i++)
        {
            int32_t j = length2 - i;
            f32_t nCos = MPY_F32(*pTWD++, 0.5); // >>1
            f32_t nSin = MPY_F32(*pTWD++, 0.5); // >>1
            f32_t iAre = ADD_F32((0.5),nSin);
			f32_t iBre = SUB_F32((0.5),nSin);
            f32_t iBim = nCos;

                                   f32_t acc = MPY_F32( COMPLEX_RE(input, i), iAre);
                                   MAC_F32(acc, COMPLEX_IM(input, i), iBim);
                                   MAC_F32(acc, COMPLEX_RE(input, j), iBre);
                                   MAC_F32(acc, COMPLEX_IM(input, j), iBim);
            COMPLEX_RE(output,i) = MPY_F32(acc,2.0);//dspfx_q31_cast_asl_a64(acc, 1);

                                   acc= MPY_F32( COMPLEX_IM(input, i), iAre);
                                   MSU_F32(acc, COMPLEX_RE(input, i), iBim);
                                   MAC_F32(acc, COMPLEX_RE(input, j), iBim);
                                   MSU_F32(acc, COMPLEX_IM(input, j), iBre);
            COMPLEX_IM(output,i) = MPY_F32(acc,2.0);//dspfx_q31_cast_asl_a64(acc, 1);
        }
    
};

#endif

#pragma Off_inline(dsp_rifft_preprocess_f32)

static _Inline void risc_dsp_rifft_f32(const rfft_f32_t* handle, const f32_t* input, f32_t* output) 
{
	f32_t *ptemp = ((uint32_t)handle->temp_buffer) ? handle->temp_buffer : output;
    dsp_rifft_preprocess_f32(handle, (PTR(f32_t)) input, (PTR(f32_t)) ptemp);
    dsp_cifft_f32(&handle->hfft, (PTR(f32_t))ptemp, (PTR(f32_t))output);
};

#define XY_MAP_INPUT  (((XY_REGION_SIZE/2))/sizeof(f32_t))
#define XY_MAP_TMP    (((XY_REGION_SIZE/2))/sizeof(f32_t))
#define XY_MAP_OUTPUT 0
#define XY_MAP_TWIDDLES 0
#define XY_RIFFT_Q31_THRESHOLD (((XY_REGION_SIZE/4))/sizeof(f32_t))

/**
 * Real Inverse FFT "process" function for f32 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_rifft_f32(const rfft_f32_t* rfft_handle, const PTR(f32_t) input, PTR(f32_t) output)
{
    // Copy scaling parameter to cfft handle
    cfft_f32_t* cfft_handle = (cfft_f32_t*) &rfft_handle->hfft;
    cfft_handle->scaling = rfft_handle->scaling;
    risc_dsp_rifft_f32(rfft_handle, input, output);
    if (rfft_handle->scaling == DSP_TRANSFORM_SCALING_SCALED)
    {
        dsp_scale_f32( output,(1.0/2) ,output, 2*rfft_handle->length);
    }
};

void dsp_rifft_delete_f32(rfft_f32_t* handle)
{
    dsp_cifft_delete_f32(&handle->hfft);
}

#endif //!defined(EXCLUDE_DSP_RIFFT_Q31)
