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

#if !defined(EXCLUDE_DSP_RIFFT_Q15)
#pragma Code("dsplib_dsp_rifft_q15")

#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsp_transform_tables.h"

/**
 * Initializes instance structure of inverse real fft transform
 * for q15 data type. 
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_rifft_getsize_q15()" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_rifft_init_q15(rfft_q15_t* handle, uint32_t length, PTR(uint32_t) pState)
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
#if defined __FXAPI__ && (( defined __Xdsp && defined __Xdsp_complex) || (defined  __Xdsp && __Xdsp_version > 1)) && defined __GUARD_ACCUM__
	if (dsp_rifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
		return DSP_ERR_LENGTH_NO_TABLE;
#ifdef _ARCv2EM_XY_11D
    handle->ptwd = (q15_t*)data_ptr; data_ptr += length * sizeof(q15_t)*2;
    memcpy(handle->ptwd, dsp_rifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(q15_t)*2);
#else
	handle->ptwd = (q15_t*)dsp_rifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif
#else
    if (dsp_cifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;
    handle->ptwd = (q15_t*)dsp_cifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif

    // By default RIFFT is scaled, produce full spectrum
    handle->scaling = DSP_TRANSFORM_SCALING_SCALED;
    handle->spectrum_format = DSP_TRANSFORM_FULL_SPECTRUM;
    handle->temp_buffer = (q15_t *)(void*) data_ptr; data_ptr += length * sizeof(q15_t);    
    return dsp_cifft_init_q15(&handle->hfft, length/2, (PTR(uint32_t)) data_ptr);
};

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__ && defined __GUARD_ACCUM__ && defined _XY_HAND_OPT

static void dsp_rifft_preprocess_q15(const rfft_q15_t* handle,  const q15_t* input, q15_t* output) {
	int32_t i;
	int32_t length2 = handle->length>>1;

	_DSPLIB_store_regs(4, 4, 0);

	_set_agu_ap0((cq15_t*) input);
	_set_agu_ap1((cq15_t*) input + length2);
	_set_agu_ap2((cq15_t*) output);
	_set_agu_ap3((cq15_t*) handle->ptwd);
	
	_set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2));
	_set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_DEC_q15(2));
	_set_agu_mod2(2, AGU_DT_v2q15, AGU_AD_INC_q15(2));
	_set_agu_mod3(3, AGU_DT_v2q15, AGU_AD_INC_q15(2));

	if(handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
		// Branch for the scaled RIFFT
		for (i = 0; i< length2; i++) {
			v2accum40_t acc;
			acc = fx_v2a40_cmpy_cq15(fx_cq15_cast_q31(_agu_u3()), fx_cq15_cast_q31(_agu_u0()));
			acc = fx_v2a40_cjmac_cq15(acc, fx_cq15_cast_q31(_agu_u3()), fx_cq15_cast_q31(_agu_u1()));
			_st_agu_u2(fx_q31_cast_cq15(fx_cq15_cast_rnd_v2a40(acc)));
		}
	} else {
		// Branch for the unscaled RIFFT
		for (i = 0; i< length2; i++) {
			v2accum40_t acc;
			acc = fx_v2a40_cmpy_cq15(fx_cq15_cast_q31(_agu_u3()), fx_cq15_cast_q31(_agu_u0()));
			acc = fx_v2a40_cjmac_cq15(acc, fx_cq15_cast_q31(_agu_u3()), fx_cq15_cast_q31(_agu_u1()));
			_st_agu_u2(fx_q31_cast_cq15(fx_cq15_cast_asl_rnd_v2a40(acc, 1)));
		}
	}

	_DSPLIB_restore_regs();
}


#elif defined __FXAPI__ && (( defined __Xdsp && defined __Xdsp_complex) || (defined  __Xdsp && __Xdsp_version > 1)) && defined __GUARD_ACCUM__

static void dsp_rifft_preprocess_q15(const rfft_q15_t* handle,  const q15_t* input, q15_t* output) {
    int32_t i;
    v2accum40_t acc;
    PTR(cq15_t) __restrict input_c  = (PTR(cq15_t)) input;
    PTR(cq15_t) __restrict output_c = (PTR(cq15_t)) output;
    PTR(cq15_t) __restrict pTWD_c   = (PTR(cq15_t)) handle->ptwd;
    int32_t length2 = handle->length>>1;
    if(handle->scaling == DSP_TRANSFORM_SCALING_SCALED) {
        // Branch for the scaled RIFFT
        for (i = 0; i< length2; i++) {
            acc = fx_v2a40_cmpy_cq15(*pTWD_c++, input_c[i]);
            acc = fx_v2a40_cjmac_cq15(acc, *pTWD_c++, input_c[length2 - i]);
            output_c[i] = fx_cq15_cast_rnd_v2a40(acc);
        }
    } else {
        // Branch for the unscaled RIFFT
        for (i = 0; i< length2; i++) {
            acc = fx_v2a40_cmpy_cq15(*pTWD_c++, input_c[i]);
            acc = fx_v2a40_cjmac_cq15(acc, *pTWD_c++, input_c[length2 - i]);
            output_c[i] = fx_cq15_cast_asl_rnd_v2a40(acc, 1);
        }
    }
}

#else

static void dsp_rifft_preprocess_q15(const rfft_q15_t* handle,  const q15_t* input, q15_t* output) {
    int32_t i;
    dspfx_accum32()
    int32_t length2 = handle->length>>1;
    q15_t* pTWD = handle->ptwd;
    INIT_MACMODE();
    if(handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
        // Branch for the scaled RIFFT
        for (i = 0; i< length2; i++)
        {
            int32_t j = length2 - i;
            q15_t xCos = (*pTWD++)>>1;
            q15_t xSin = (*pTWD++)>>1;
            q15_t iAre = dspfx_sub_q15(dsp_q15_cast_rnd_f32(0.5), xSin);
            q15_t iAim =            xCos;
            q15_t iBre = dspfx_add_q15(dsp_q15_cast_rnd_f32(0.5), xSin);
            q15_t iBim =           -xCos;

                                   dspfx_a32mul_q15( COMPLEX_RE(input, i), iAre, acc);
                                   dspfx_a32mac_q15( COMPLEX_IM(input, i), dspfx_neg_q15(iAim), acc);
                                   dspfx_a32mac_q15( COMPLEX_RE(input, j), iBre, acc);
            COMPLEX_RE(output,i) = dspfx_a32mac_q15( COMPLEX_IM(input, j), iBim, acc);

                                   dspfx_a32mul_q15( COMPLEX_IM(input, i), iAre, acc);
                                   dspfx_a32mac_q15( COMPLEX_RE(input, i), iAim, acc);
                                   dspfx_a32mac_q15( COMPLEX_RE(input, j), iBim, acc);
            COMPLEX_IM(output,i) = dspfx_a32mac_q15( COMPLEX_IM(input, j), dspfx_neg_q15(iBre), acc);
        }
    } else
    {
        // Branch for the unscaled RIFFT
        for (i = 0; i< length2; i++)
        {
            int32_t j = length2 - i;
            q15_t xCos = (*pTWD++)>>1;
            q15_t xSin = (*pTWD++)>>1;
            q15_t iAre = dspfx_sub_q15(dsp_q15_cast_rnd_f32(0.5), xSin);
            q15_t iAim =            xCos;
            q15_t iBre = dspfx_add_q15(dsp_q15_cast_rnd_f32(0.5), xSin);
            q15_t iBim =           -xCos;

                                   dspfx_a32mul_q15( COMPLEX_RE(input, i), iAre, acc);
                                   dspfx_a32mac_q15( COMPLEX_IM(input, i), dspfx_neg_q15(iAim), acc);
                                   dspfx_a32mac_q15( COMPLEX_RE(input, j), iBre, acc);
                                   dspfx_a32mac_q15( COMPLEX_IM(input, j), iBim, acc);
            COMPLEX_RE(output,i) = dspfx_q15_cast_asl_a32(acc, 1);

                                   dspfx_a32mul_q15( COMPLEX_IM(input, i), iAre, acc);
                                   dspfx_a32mac_q15( COMPLEX_RE(input, i), iAim, acc);
                                   dspfx_a32mac_q15( COMPLEX_RE(input, j), iBim, acc);
                                   dspfx_a32mac_q15( COMPLEX_IM(input, j), dspfx_neg_q15(iBre), acc);
            COMPLEX_IM(output,i) = dspfx_q15_cast_asl_a32(acc, 1);
        }
    }
};

#endif

#pragma Off_inline(dsp_rifft_preprocess_q15)

static _Inline void risc_dsp_rifft_q15(const rfft_q15_t* handle, const q15_t* input, q15_t* output) 
{
	q15_t *ptemp = ((uint32_t)handle->temp_buffer) ? handle->temp_buffer : output;
    dsp_rifft_preprocess_q15(handle, input, ptemp);
    dsp_cifft_q15(&handle->hfft, (PTR(q15_t))ptemp, (PTR(q15_t))output);
};


#define XY_MAP_INPUT  (0)
#define XY_MAP_TMP    (((XY_REGION_SIZE/2))/sizeof(q15_t))
#define XY_MAP_OUTPUT 0
#define XY_MAP_TWIDDLES 0
#define XY_RIFFT_Q15_THRESHOLD (((XY_REGION_SIZE/4))/sizeof(q15_t))

/**
 * Real Inverse FFT "process" function for q31 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_rifft_q15(const rfft_q15_t* rfft_handle, const PTR(q15_t) input, PTR(q15_t) output)
{
    // Copy scaling parameter to cfft handle
    cfft_q15_t* cfft_handle = (cfft_q15_t*) &rfft_handle->hfft;
    cfft_handle->scaling = rfft_handle->scaling;
    if (rfft_handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
        cfft_handle->shiftBits = 30-dspfx_norm_q31(cfft_handle->length);
    } else {
        cfft_handle->shiftBits = 0;
    }
#if defined(_ARC_AS210_mac16)
    if (rfft_handle->length > XY_RIFFT_Q15_THRESHOLD) {
#endif
        risc_dsp_rifft_q15(rfft_handle, input, output);
#if defined(_ARC_AS210_mac16)
    } else {
        cfft_rad2_q15_xy_t pInst;
        rfft_q15_xy_t pXYInst;

        memcpy_StoY_int32(XY_MAP_INPUT>>1, (int32_t*)(void*)input,rfft_handle->length);

        dsp_rifft_preprocess_init_q15_cfY(
            &pXYInst, 
            rfft_handle->length,
            rfft_handle->length);

        dsp_rifft_preprocess_q15_YtoX_cfY_MAC16x16(
            &pXYInst, 
            XY_MAP_INPUT, 
            XY_MAP_TMP);

        dsp_cifft_radix2_init_q15_cfX_DMAC16x16(
            &pInst, 
            rfft_handle->length/2, 
            XY_MAP_TWIDDLES);

        pInst.shiftBits = rfft_handle->hfft.shiftBits;

        dsp_cfft_radix2_q15_XtoY_cfX_DMAC16x16(
            &pInst, 
            XY_MAP_TMP, 
            XY_MAP_OUTPUT);//Y to Y

        memcpy_YtoS_int32((int32_t*)(void*)output,XY_MAP_OUTPUT>>1, rfft_handle->length/2);
    }
#endif
};

void dsp_rifft_delete_q15(rfft_q15_t* handle)
{
    dsp_cifft_delete_q15(&handle->hfft);
}

#endif //!defined(EXCLUDE_DSP_RIFFT_Q15)
