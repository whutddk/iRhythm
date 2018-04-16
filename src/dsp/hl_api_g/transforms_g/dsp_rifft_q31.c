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
#pragma Code("dsplib_dsp_rifft_q31")

#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsp_transform_tables.h"

/**
 * Initializes instance structure of inverse real fft transform
 * for q31 data type. 
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_rifft_getsize_q31()" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_rifft_init_q31(rfft_q31_t* handle, uint32_t length, PTR(uint32_t) pState)
{
    int32_t order = 30-dspfx_norm_q31(length);
    uint8_t* data_ptr = (uint8_t*) (void*) pState;    
    if ((length > DSP_MAX_TRANSFORM_SIZE))
        return DSP_ERR_LENGTH_TOO_BIG;
    if ((length < 8))
        return DSP_ERR_LENGTH_TOO_SMALL;
    if (length != (1 << order))
        return DSP_ERR_LENGTH_NOT_POWER2;
    if (dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;

    handle->length = length;
#ifdef _ARCv2EM_XY_11D
    handle->ptwd = (q31_t*)data_ptr; data_ptr += length * sizeof(q31_t);
    memcpy(handle->ptwd, dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(q31_t));
#else
    handle->ptwd = (q31_t*)dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif
    // By default RIFFT is unscaled, produce full spectrum
    handle->scaling = DSP_TRANSFORM_SCALING_SCALED;
    handle->spectrum_format = DSP_TRANSFORM_FULL_SPECTRUM;
    handle->temp_buffer = (q31_t *)(void*) data_ptr; data_ptr += length * sizeof(q31_t);
    return dsp_cifft_init_q31(&handle->hfft, length/2, (PTR(uint32_t))data_ptr);
};


#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__ && defined __GUARD_ACCUM__ && defined _XY_HAND_OPT

static void dsp_rifft_preprocess_q31(const rfft_q31_t* handle,  const q31_t* input, q31_t* output) {
    int32_t i;
    dspfx_accum64();
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

    if (handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
        // Branch for the scaled RFFT
        for (i = 0; i< length2; i++)
        {
            q31_t nCos = ((q31_t)_agu_u5())>>1;
            q31_t nSin = ((q31_t)_agu_u5())>>1;
            int32_t iAre = dspfx_add_q31(dsp_q31_cast_rnd_f32(0.5),nSin);
			int32_t iBre = dspfx_sub_q31(dsp_q31_cast_rnd_f32(0.5),nSin);
            int32_t iBim = nCos;
                       dspfx_a64mul_q31( _agu_u0(), iAre, acc);
                       dspfx_a64mac_q31( _agu_u7(), iBim, acc);
                       dspfx_a64mac_q31( _agu_u2(), iBim, acc);
            _st_agu_u4(dspfx_a64mac_q31( _agu_u3(), iBre, acc));
                       dspfx_a64mul_q31( _agu_u1(), iAre, acc);
                       dspfx_a64msu_q31( _agu_u6(), iBim, acc);
                       dspfx_a64msu_q31( _agu_u2(), iBre, acc);
            _st_agu_u4(dspfx_a64mac_q31( _agu_u2(), iBim, acc));
        }
    } else
    {
        // Branch for the unscaled RFFT
        for (i = 0; i< length2; i++)
        {
			q31_t nCos = ((q31_t)_agu_u5())>>1;
			q31_t nSin = ((q31_t)_agu_u5())>>1;
			int32_t iAre = dspfx_add_q31(dsp_q31_cast_rnd_f32(0.5),nSin);
			int32_t iBre = dspfx_sub_q31(dsp_q31_cast_rnd_f32(0.5),nSin);
			int32_t iBim = nCos;
                       dspfx_a64mul_q31( _agu_u0(), iAre, acc);
                       dspfx_a64mac_q31( _agu_u7(), iBim, acc);
		 	           dspfx_a64mac_q31( _agu_u2(), iBim, acc);
                       dspfx_a64mac_q31( _agu_u3(), iBre, acc);
            _st_agu_u4(dspfx_q31_cast_asl_a64(acc, 1));
                       dspfx_a64mul_q31( _agu_u1(), iAre, acc);
                       dspfx_a64msu_q31( _agu_u6(), iBim, acc);
			           dspfx_a64msu_q31( _agu_u2(), iBre, acc);
                       dspfx_a64mac_q31( _agu_u2(), iBim, acc);
            _st_agu_u4(dspfx_q31_cast_asl_a64(acc, 1));
        }
    }

    _DSPLIB_restore_regs();
};


#else 

static void dsp_rifft_preprocess_q31(const rfft_q31_t* handle,  const PTR(q31_t) input, PTR(q31_t) output) {
    int32_t i;
    dspfx_accum64()
    int32_t length2 = handle->length>>1;
    PTR(q31_t) pTWD = (PTR(q31_t))handle->ptwd;
    if (handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
        // Branch for the scaled RFFT
        for (i = 0; i< length2; i++)
        {
            int32_t j = length2 - i;
            q31_t nCos = (*pTWD++)>>1;
            q31_t nSin = (*pTWD++)>>1;
            int32_t iAre = dspfx_add_q31(dsp_q31_cast_rnd_f32(0.5),nSin);
			int32_t iBre = dspfx_sub_q31(dsp_q31_cast_rnd_f32(0.5),nSin);
            int32_t iBim = nCos;

                                   dspfx_a64mul_q31( COMPLEX_RE(input, i), iAre, acc);
                                   dspfx_a64mac_q31( COMPLEX_IM(input, i), iBim, acc);
                                   dspfx_a64mac_q31( COMPLEX_RE(input, j), iBre, acc);
            COMPLEX_RE(output,i) = dspfx_a64mac_q31( COMPLEX_IM(input, j), iBim, acc);

                                   dspfx_a64mul_q31( COMPLEX_IM(input, i), iAre, acc);
                                   dspfx_a64msu_q31( COMPLEX_RE(input, i), iBim, acc);
                                   dspfx_a64mac_q31( COMPLEX_RE(input, j), iBim, acc);
            COMPLEX_IM(output,i) = dspfx_a64msu_q31( COMPLEX_IM(input, j), iBre, acc);
        }
    } else 
    {
        // Branch for the unscaled RFFT
        for (i = 0; i< length2; i++)
        {
            int32_t j = length2 - i;
            q31_t nCos = (*pTWD++)>>1;
            q31_t nSin = (*pTWD++)>>1;
            int32_t iAre = dspfx_add_q31(dsp_q31_cast_rnd_f32(0.5),nSin);
			int32_t iBre = dspfx_sub_q31(dsp_q31_cast_rnd_f32(0.5),nSin);
            int32_t iBim = nCos;

                                   dspfx_a64mul_q31( COMPLEX_RE(input, i), iAre, acc);
                                   dspfx_a64mac_q31( COMPLEX_IM(input, i), iBim, acc);
                                   dspfx_a64mac_q31( COMPLEX_RE(input, j), iBre, acc);
                                   dspfx_a64mac_q31( COMPLEX_IM(input, j), iBim, acc);
            COMPLEX_RE(output,i) = dspfx_q31_cast_asl_a64(acc, 1);

                                   dspfx_a64mul_q31( COMPLEX_IM(input, i), iAre, acc);
                                   dspfx_a64msu_q31( COMPLEX_RE(input, i), iBim, acc);
                                   dspfx_a64mac_q31( COMPLEX_RE(input, j), iBim, acc);
                                   dspfx_a64msu_q31( COMPLEX_IM(input, j), iBre, acc);
            COMPLEX_IM(output,i) = dspfx_q31_cast_asl_a64(acc, 1);
        }
    }
};

#endif

#pragma Off_inline(dsp_rifft_preprocess_q31)

static _Inline void risc_dsp_rifft_q31(const rfft_q31_t* handle, const q31_t* input, q31_t* output) 
{
	q31_t *ptemp = ((uint32_t)handle->temp_buffer) ? handle->temp_buffer : output;
    dsp_rifft_preprocess_q31(handle, (PTR(q31_t)) input, (PTR(q31_t)) ptemp);
    dsp_cifft_q31(&handle->hfft, (PTR(q31_t))ptemp, (PTR(q31_t))output);
};

#define XY_MAP_INPUT  (((XY_REGION_SIZE/2))/sizeof(q31_t))
#define XY_MAP_TMP    (((XY_REGION_SIZE/2))/sizeof(q31_t))
#define XY_MAP_OUTPUT 0
#define XY_MAP_TWIDDLES 0
#define XY_RIFFT_Q31_THRESHOLD (((XY_REGION_SIZE/4))/sizeof(q31_t))

/**
 * Real Inverse FFT "process" function for q31 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_rifft_q31(const rfft_q31_t* rfft_handle, const PTR(q31_t) input, PTR(q31_t) output)
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
    if (rfft_handle->length > XY_RIFFT_Q31_THRESHOLD) {
#endif
        risc_dsp_rifft_q31(rfft_handle, input, output);
#if defined(_ARC_AS210_mac32)
    } else {
        cfft_srad_q31_xy_t pInst;
        rfft_q31_xy_t pXYInst;

        memcpy_StoY_int32(XY_MAP_INPUT, (int32_t*)(void*)input, 2*rfft_handle->length);

        dsp_rifft_preprocess_init_q31_cfY(
            &pXYInst, 
            rfft_handle->length, 
            rfft_handle->length);

        dsp_rifft_preprocess_q31_YtoX_cfY_MAC32x32(
            &pXYInst, 
            XY_MAP_INPUT, 
            XY_MAP_TMP);

        dsp_cifft_sradix_init_q31_cfX(
            &pInst, 
            rfft_handle->length/2, 
            XY_MAP_TWIDDLES);

        pInst.shiftBits = rfft_handle->hfft.shiftBits;

        dsp_cifft_sradix_q31_XtoY_cfX_MAC32x32(
            &pInst, 
            XY_MAP_TMP, 
            XY_MAP_OUTPUT);//Y to Y

        memcpy_YtoS_int32((int32_t*)(void*)output,XY_MAP_OUTPUT, rfft_handle->length);
    }
#endif

};

void dsp_rifft_delete_q31(rfft_q31_t* handle)
{
    dsp_cifft_delete_q31(&handle->hfft);
}

#endif //!defined(EXCLUDE_DSP_RIFFT_Q31)
