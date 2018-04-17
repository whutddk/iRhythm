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

#if !defined(EXCLUDE_DSP_RFFT_Q15)
#pragma Code("dsplib_dsp_rfft_q15")

#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsp_transform_tables.h"


/**
 * Initializes instance structure of real fft transform
 * for q15 data type. 
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_rfft_getsize_q15()" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_rfft_init_q15(rfft_q15_t* handle, uint32_t length, PTR(uint32_t) pState)
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
	if (dsp_rfft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
		return DSP_ERR_LENGTH_NO_TABLE;
#ifdef _ARCv2EM_XY_11D
    handle->ptwd = (q15_t*)data_ptr; data_ptr += length * sizeof(q15_t) * 2;
    memcpy(handle->ptwd, dsp_rfft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], length * sizeof(q15_t) * 2);
#else
    handle->ptwd = (q15_t*) dsp_rfft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif

#else
	if (dsp_cifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
		return DSP_ERR_LENGTH_NO_TABLE;
	handle->ptwd = (q15_t*) dsp_cifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif
    // By default RFFT is unscaled, produce full spectrum
    handle->scaling = DSP_TRANSFORM_SCALING_UNSCALED;
    handle->spectrum_format = DSP_TRANSFORM_FULL_SPECTRUM;
    return dsp_cfft_init_q15(&handle->hfft, length/2, (PTR(uint32_t)) data_ptr);
};

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__ && defined __GUARD_ACCUM__

#if defined _XY_HAND_OPT

static void dsp_rfft_postprocess_q15(const rfft_q15_t* handle, const q15_t* input, q15_t* output) {
	int32_t i;
	int32_t length2 = handle->length>>1;
	q15_t Re_out_Nplus1;
	q15_t Im_out_Nplus1;

	_DSPLIB_store_regs(4, 4, 0);

	_set_agu_ap0((cq15_t*) input + 1);
	_set_agu_ap1((cq15_t*) input + length2 - 1);
	_set_agu_ap2((cq15_t*) output + 1);
	_set_agu_ap3(&handle->ptwd[4]);

	_set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2));
	_set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_DEC_q15(2));
	_set_agu_mod2(2, AGU_DT_v2q15, AGU_AD_INC_q15(2));
	_set_agu_mod3(3, AGU_DT_v2q15, AGU_AD_INC_q15(2));

	if (handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
		// Branch for the  scaled RFFT
		COMPLEX_RE(output,0) = dspfx_add_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0)) >> 1;
		COMPLEX_IM(output,0) = 0;
		for (i = 1; i< length2; i++) {
			v2accum40_t acc;
			acc = fx_v2a40_cmpy_cq15(fx_cq15_cast_q31(_agu_u3()), fx_cq15_cast_q31(_agu_u0()));
			acc = fx_v2a40_cjmac_cq15(acc, fx_cq15_cast_q31(_agu_u3()), fx_cq15_cast_q31(_agu_u1()));
			_st_agu_u2(fx_q31_cast_cq15(fx_cq15_cast_asr_rnd_v2a40(acc, 1)));
		}
		Re_out_Nplus1 = dspfx_sub_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0)) >> 1;
		Im_out_Nplus1 = 0;
	} else
	{
		// Branch for the  unscaled RFFT
		COMPLEX_RE(output,0) = dspfx_add_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0));
		COMPLEX_IM(output,0) = 0;
		for (i = 1; i< length2; i++) {
			v2accum40_t acc;
			acc = fx_v2a40_cmpy_cq15(fx_cq15_cast_q31(_agu_u3()), fx_cq15_cast_q31(_agu_u0()));
			acc = fx_v2a40_cjmac_cq15(acc, fx_cq15_cast_q31(_agu_u3()), fx_cq15_cast_q31(_agu_u1()));
			_st_agu_u2(fx_q31_cast_cq15(fx_cq15_cast_rnd_v2a40(acc)));
		}
		Re_out_Nplus1 = dspfx_sub_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0));
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
		// Copy "middle" sample to its position.
		COMPLEX_RE(output,i) = Re_out_Nplus1;
		COMPLEX_IM(output,i) = Im_out_Nplus1;
		_set_agu_ap0((cq15_t*) output + length2 - 1);
		_set_agu_ap1((cq15_t*) output + length2 + 1);
		_set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_DEC_q15(2));
		_set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_INC_q15(2));
		for (i = 1; i < length2; i++)
		{
			_st_agu_u1(fx_q31_cast_v2q15(fx_addsub_v2q15(fx_create_v2q15(0,0), fx_v2q15_cast_q31(_agu_u0()))));
		}
		break;
	}

	_DSPLIB_restore_regs();
}

#else

static void dsp_rfft_postprocess_q15(const rfft_q15_t* handle, const q15_t* input, q15_t* output) {
    int32_t i;
    __xy cq15_t * __restrict input_c  = (__xy cq15_t*) input;
    __xy cq15_t * __restrict output_c = (__xy cq15_t*) output;
    __xy cq15_t * __restrict pTWD_c   = (__xy cq15_t*) &handle->ptwd[4];
    int32_t length2 = handle->length>>1;
    int32_t length = handle->length;
    q15_t Re_out_Nplus1;
    q15_t Im_out_Nplus1;
    v2accum40_t acc;

    if (handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
        // Branch for the  scaled RFFT
        COMPLEX_RE(output,0) = dspfx_add_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0)) >> 1;
        COMPLEX_IM(output,0) = 0;
        for (i = 1; i< length2; i++) {
            acc = fx_v2a40_cmpy_cq15(*pTWD_c++, input_c[i]);
            acc = fx_v2a40_cjmac_cq15(acc, *pTWD_c++, input_c[length2 - i]);
            output_c[i] = fx_cq15_cast_asl_rnd_v2a40(acc, -1);
        }
        Re_out_Nplus1 = dspfx_sub_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0)) >> 1;
        Im_out_Nplus1 = 0;
    } else
    {
        // Branch for the  unscaled RFFT
        COMPLEX_RE(output,0) = dspfx_add_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0));
        COMPLEX_IM(output,0) = 0;
        for (i = 1; i< length2; i++) {
            acc = fx_v2a40_cmpy_cq15(*pTWD_c++, input_c[i]);
            acc = fx_v2a40_cjmac_cq15(acc, *pTWD_c++, input_c[length2 - i]);
            output_c[i] = fx_cq15_cast_rnd_v2a40(acc);
        }
        Re_out_Nplus1 = dspfx_sub_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0));
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
        // Copy "middle" sample to its position.
        COMPLEX_RE(output,i) = Re_out_Nplus1;
        COMPLEX_IM(output,i) = Im_out_Nplus1;
        for (; i<length; i++)
        {
            v2q15_t tmp0, out0;
            tmp0 = ((__xy v2q15_t*)output_c)[length - i];
            out0 = fx_addsub_v2q15(fx_create_v2q15(0,0), tmp0);
            ((__xy v2q15_t*)output_c)[i] = out0;
        }
        break;
    }
}

#endif

#elif defined __FXAPI__ && (( defined __Xdsp && defined __Xdsp_complex) || (defined  __Xdsp && __Xdsp_version > 1)) && defined __GUARD_ACCUM__

static void dsp_rfft_postprocess_q15(const rfft_q15_t* handle, const q15_t* input, q15_t* output) {
    int32_t i;
    cq15_t * __restrict input_c  = (cq15_t*) input;
    cq15_t * __restrict output_c = (cq15_t*) output;
    cq15_t * __restrict pTWD_c   = (cq15_t*) &handle->ptwd[4];
    int32_t length2 = handle->length>>1;
    int32_t length = handle->length;
    q15_t Re_out_Nplus1;
    q15_t Im_out_Nplus1;
    v2accum40_t acc;

    if (handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
        // Branch for the  scaled RFFT
        COMPLEX_RE(output,0) = dspfx_add_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0)) >> 1;
        COMPLEX_IM(output,0) = 0;
        for (i = 1; i< length2; i++) {
            acc = fx_v2a40_cmpy_cq15(*pTWD_c++, input_c[i]);
            acc = fx_v2a40_cjmac_cq15(acc, *pTWD_c++, input_c[length2 - i]);
            output_c[i] = fx_cq15_cast_asl_rnd_v2a40(acc, -1);
        }
        Re_out_Nplus1 = dspfx_sub_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0)) >> 1;
        Im_out_Nplus1 = 0;
    } else
    {
        // Branch for the  unscaled RFFT
        COMPLEX_RE(output,0) = dspfx_add_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0));
        COMPLEX_IM(output,0) = 0;
        for (i = 1; i< length2; i++) {
            acc = fx_v2a40_cmpy_cq15(*pTWD_c++, input_c[i]);
            acc = fx_v2a40_cjmac_cq15(acc, *pTWD_c++, input_c[length2 - i]);
            output_c[i] = fx_cq15_cast_rnd_v2a40(acc);
        }
        Re_out_Nplus1 = dspfx_sub_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0));
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
        // Copy "middle" sample to its position.
        COMPLEX_RE(output,i) = Re_out_Nplus1;
        COMPLEX_IM(output,i) = Im_out_Nplus1;
        for (; i<length; i+=2)
        {
            v2q15_t tmp0, tmp1, out0, out1;
            tmp0 = ((v2q15_t*)output_c)[length - i];        
            tmp1 = ((v2q15_t*)output_c)[length - i - 1];
            out0 = fx_addsub_v2q15(fx_create_v2q15(0,0), tmp0);
            out1 = fx_addsub_v2q15(fx_create_v2q15(0,0), tmp1);
            ((v2q15_t*)output_c)[i] = out0;        
            ((v2q15_t*)output_c)[i+1] = out1;
        }
        break;
    }
}

#else

static void dsp_rfft_postprocess_q15(const rfft_q15_t* handle, const q15_t* input, q15_t* output) {
    uint32_t i;
    dspfx_accum32()
    uint32_t length = handle->length;
    uint32_t length2 = handle->length>>1;
    q15_t *pTWD = handle->ptwd+2;
    q15_t Re_out_Nplus1;
    q15_t Im_out_Nplus1;

    if (handle->scaling == DSP_TRANSFORM_SCALING_SCALED){
        // Branch for the scaled RFFT
        INIT_MACMODE_NF();
        COMPLEX_RE(output,0) = dspfx_add_q15(input[1],input[0])>>1;
        COMPLEX_IM(output,0) = 0;
        for (i = 1; i < length2; i++)
        {
            q15_t xCos = (*pTWD++)>>1;
            q15_t xSin = (*pTWD++)>>1;
            q15_t Are = dspfx_sub_q15(dsp_q15_cast_rnd_f32(0.5), xSin);
            q15_t Aim = dspfx_neg_q15(xCos);
            q15_t Bre = dspfx_add_q15(dsp_q15_cast_rnd_f32(0.5), xSin);
            q15_t Bim = xCos;

                                   dspfx_a32mul_nf_q15(COMPLEX_RE(input,i), Are, acc);
                                   dspfx_a32mac_nf_q15(COMPLEX_IM(input,i),-Aim, acc);
                                   dspfx_a32mac_nf_q15(COMPLEX_RE(input,(length2 - i)), Bre, acc);
            COMPLEX_RE(output,i) = dspfx_a32mac_nf_q15(COMPLEX_IM(input,(length2 - i)), Bim, acc);

                                   dspfx_a32mul_nf_q15(COMPLEX_IM(input,i), Are, acc);
                                   dspfx_a32mac_nf_q15(COMPLEX_RE(input,i), Aim, acc);
                                   dspfx_a32mac_nf_q15(COMPLEX_RE(input,(length2 - i)), Bim, acc);
            COMPLEX_IM(output,i) = dspfx_a32mac_nf_q15(COMPLEX_IM(input,(length2 - i)),-Bre, acc);
        }

        Re_out_Nplus1 = dspfx_sub_q15(COMPLEX_RE(input,0),COMPLEX_IM(input,0)) >> 1;
        Im_out_Nplus1 = 0;
    } else {
        // Branch for the unscaled RFFT
		INIT_MACMODE();
        COMPLEX_RE(output,0) = dspfx_add_q15(input[1],input[0]);
        COMPLEX_IM(output,0) = 0;
        for (i = 1; i < length2; i++)
        {
            q15_t xCos = (*pTWD++)>>1;
            q15_t xSin = (*pTWD++)>>1;
            q15_t Are = dspfx_sub_q15(dsp_q15_cast_rnd_f32(0.5), xSin);
            q15_t Aim = dspfx_neg_q15(xCos);
            q15_t Bre = dspfx_add_q15(dsp_q15_cast_rnd_f32(0.5), xSin);
            q15_t Bim = xCos;

								   dspfx_a32mul_q15(COMPLEX_RE(input,i), Are, acc);
								   dspfx_a32mac_q15(COMPLEX_IM(input,i),-Aim, acc);
								   dspfx_a32mac_q15(COMPLEX_RE(input,(length2 - i)), Bre, acc);
            COMPLEX_RE(output,i) = dspfx_a32mac_q15(COMPLEX_IM(input,(length2 - i)), Bim, acc);

								   dspfx_a32mul_q15(COMPLEX_IM(input,i), Are, acc);
								   dspfx_a32mac_q15(COMPLEX_RE(input,i), Aim, acc);
								   dspfx_a32mac_q15(COMPLEX_RE(input,(length2 - i)), Bim, acc);
            COMPLEX_IM(output,i) = dspfx_a32mac_q15(COMPLEX_IM(input,(length2 - i)),-Bre, acc);
        }

        Re_out_Nplus1 = dspfx_sub_q15(COMPLEX_RE(input,0), COMPLEX_IM(input,0));
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
            COMPLEX_IM(output,i) = -COMPLEX_IM(output,(length - i));
        }
        break;
    }

};
#endif
static _Inline void risc_dsp_rfft_q15(const rfft_q15_t* handle, const q15_t* input, q15_t* output)
{
    dsp_cfft_q15(&handle->hfft, (PTR(q15_t))input, (PTR(q15_t))(output + handle->length));
    dsp_rfft_postprocess_q15(handle, output + handle->length, output);
};

#define XY_MAP_INPUT  (((XY_REGION_SIZE/2))/sizeof(q15_t))
#define XY_MAP_OUTPUT 0
#define XY_MAP_TWIDDLES 0
#define XY_RFFT_Q15_THRESHOLD (((XY_REGION_SIZE/4))/sizeof(q15_t))

/**
 * Real FFT "process" function for q15 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_rfft_q15(const rfft_q15_t* rfft_handle, const PTR(q15_t) input, PTR(q15_t) output)
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
    if (rfft_handle->length > XY_RFFT_Q15_THRESHOLD) {
#endif
        risc_dsp_rfft_q15(rfft_handle, input, output);
#if defined(_ARC_AS210_mac16)
    } else {
        cfft_q15_t* handle = (cfft_q15_t*) &rfft_handle->hfft;
        cfft_rad2_q15_xy_t * pInst = (cfft_rad2_q15_xy_t *) &handle->fft_rad2_xy;


        memcpy_StoX_int16(XY_MAP_INPUT, (void*)input, rfft_handle->length); 


        dsp_cfft_radix2_init_q15_cfX_DMAC16x16(
            pInst, 
            rfft_handle->length/2, 
            XY_MAP_TWIDDLES);
        pInst->shiftBits = handle->shiftBits;
        dsp_cfft_radix2_q15_XtoY_cfX_DMAC16x16(pInst, 
            XY_MAP_INPUT, 
            XY_MAP_OUTPUT);//Y to Y

        dsp_rfft_postprocess_init_q15_cfY(
            (rfft_q15_xy_t*)&rfft_handle->rfft_xy,
            rfft_handle->length,
            rfft_handle->length);
        dsp_rfft_postprocess_q15_YtoX_cfY_MAC16x16(
            &rfft_handle->rfft_xy, 
            XY_MAP_OUTPUT, 
            XY_MAP_OUTPUT);


        memcpy_XtoS_int16((void*)output, XY_MAP_OUTPUT, rfft_handle->length*2);
    }
#endif
};

void dsp_rfft_delete_q15(rfft_q15_t* handle)
{
    dsp_cfft_delete_q15(&handle->hfft);
}

/**
 * Extended API function.
 * Initializes instance structure of real fft transform
 * for q15 data type. 
 *
 * \param handle - pointer to transform instance structure
 * \param temp0 - reserved parameter
 * \param length - transform size
 * \param flag_ifft - flag that enables inversed transform
 * \param flag_bitrev - flag that indicates bit reversed order of input or output data
 * \result dsp_status
 */

#pragma Code("dsplib_dsp_rfft_q15_ext")

dsp_status dsp_rfft_init_q15_ext(rfft_q15_t *handle, void *temp0,  uint32_t length,  uint32_t flag_ifft,  uint32_t flag_bitrev)
{
    dsp_status status;
    handle->hfft.bitReverse_flag = flag_bitrev;
    handle->hfft.invFlag = flag_ifft;
    if (flag_ifft) {
#if !defined _ARCv2EM_XY_11D
        status = dsp_rifft_init_q15(handle, length, 0);
#else
        status = dsp_rifft_init_q15(handle, length, (PTR(uint32_t))temp0);
#endif
        handle->hfft.shiftBits = 29 - dspfx_norm_q31(length);
        handle->hfft.scaling = DSP_TRANSFORM_SCALING_SCALED;
        handle->scaling = DSP_TRANSFORM_SCALING_SCALED;
    } else {
#if !defined _ARCv2EM_XY_11D
        status = dsp_rfft_init_q15(handle, length, 0);
#else
        status = dsp_rfft_init_q15(handle, length, (PTR(uint32_t))temp0);
#endif
        handle->hfft.shiftBits = 29 - dspfx_norm_q31(length);
        handle->hfft.scaling = DSP_TRANSFORM_SCALING_SCALED;
        handle->scaling = DSP_TRANSFORM_SCALING_SCALED;
    }
    return status;
}

/**
 * Real FFT "process" function for q15 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_rfft_q15_ext(const rfft_q15_t* handle, const PTR(q15_t) input, PTR(q15_t) output)
{
    if (handle->hfft.invFlag) {
        dsp_rifft_q15(handle, input, output);
    } else {
        dsp_rfft_q15(handle, input, output);
#if defined(_ARC_AS210_mac16)
        // Run dsp_scale only for non-XY version
        if (handle->length > XY_RFFT_Q15_THRESHOLD) {
#endif
            dsp_shift_q15(output, 1, output, handle->length*2);
#if defined(_ARC_AS210_mac16)
        }
#endif
    }

}

#endif //!defined(EXCLUDE_DSP_RFFT_Q15)
