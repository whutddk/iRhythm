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

#if !defined(EXCLUDE_DSP_CIFFT_Q15)
#pragma Code("dsplib_dsp_cifft_q15")

#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif
#include "dsp_transform_tables.h"

// _CFFT_VER == 1 should decrease power consumption
// _CFFT_VER == 2 should increase performance on interleaved mem access
#if !defined(_CFFT_VER)
#define _CFFT_VER 2
#endif

/**
 * Initializes instance structure of inverse complex fft transform
 * for q15 data type. 
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_cifft_getsize_q15()" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_cifft_init_q15(cfft_q15_t* handle, uint32_t length, PTR(uint32_t) pState)
{
    uint32_t order = 30-dspfx_norm_q31(length);
    if ((length > DSP_MAX_TRANSFORM_SIZE))
        return DSP_ERR_LENGTH_TOO_BIG;
    if ((length < 4))
        return DSP_ERR_LENGTH_TOO_SMALL;
    if (length != (1 << order))
        return DSP_ERR_LENGTH_NOT_POWER2;
    handle->length = length;
    if (dsp_cifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;
    if (dsp_cfft_bitrev_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;

#ifdef _ARCv2EM_XY_11D
    handle->ptwd = (q15_t*)(uint8_t*)pState;
    memcpy(handle->ptwd, dsp_cifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], dsp_cifft_getsize_q15(length));
#else
    handle->ptwd = (q15_t*)dsp_cifft_q15_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif 

    handle->tab_bit_rev = (int16_t*) dsp_cfft_bitrev_tab[order - DSP_MIN_TRANSFORM_ORDER];

    handle->shiftBits = order;
    handle->scaling = DSP_TRANSFORM_SCALING_SCALED;
    return DSP_ERR_OK;
};

static _Inline void bitrev_permute_q15(uint32_t* __restrict input, uint32_t* __restrict output, int16_t* __restrict bitrev_tab, int32_t length)
{
	int32_t i;
    if (input == output) {
        // in place bit-reverse shuffle
        for (i = 0; i < length; i++)
        {
            int32_t i_rev = bitrev_tab[i];
            if (i < i_rev) {
                uint32_t tmp = output[i_rev];
                output[i_rev] = input[i];
                output[i]     = tmp;
            }
        }
    } else {
        // out of place bit-reverse shuffle
#if 0
    	__builtin_assume((length & 1) == 0);
#pragma unroll 2
        for (i = 0; i < length; i++)
        {
            output[i] = input[bitrev_tab[i]];
        }
#else
        for (i = 0; i < (length >> 1); i++)
        {
            int32_t i_rev0 = *bitrev_tab++;
            int32_t i_rev1 = *bitrev_tab++;
            int32_t t0 = input[i_rev0];
            int32_t t1 = input[i_rev1];
            *output++ = t0;
            *output++ = t1;
        }
#endif
    }
}

static _Inline void risc_dsp_cifft_radix2_q15(const cfft_q15_t* handle, const q15_t* input, q15_t* output)
{
    int32_t nBlocks, nPoints;
    int32_t nLength, nLength2;
    int32_t i, j, k, l, m;
    int32_t shiftBits = handle->shiftBits;
    int32_t unitShift = 0;
    q15_t xRealA, xRealB, xImagA, xImagB, xRealB_t;
    q15_t *pTWD = handle->ptwd;

    dspfx_accum32()

    int32_t nOrder = 30-dspfx_norm_q31(handle->length);

    INIT_MACMODE();
    // Set the transform length
    nLength = 1 << nOrder;
    nLength2 = nLength >> 1;

    bitrev_permute_q15((uint32_t*)input, (uint32_t*)output, handle->tab_bit_rev, nLength);

    // Perform the half length FFT (Decimation in Time)
    nBlocks = nLength2;

    unitShift = (shiftBits-- > 0) ? 1 : 0;
    for (j = 0, i = 0; j < nBlocks; j++, i+=4)
    {
        xRealA = output[i + 0];
        xImagA = output[i + 1];
        xRealB = output[i + 2];
        xImagB = output[i + 3];

        output[i + 0] = dspfx_add_q15(xRealA, xRealB) >> unitShift;
        output[i + 1] = dspfx_add_q15(xImagA, xImagB) >> unitShift;
        output[i + 2] = dspfx_sub_q15(xRealA, xRealB) >> unitShift;
        output[i + 3] = dspfx_sub_q15(xImagA, xImagB) >> unitShift;
    }

    nBlocks >>= 1;
    nPoints = 2;

    for (i = 1; i < nOrder; i++)
    {
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        for (j = 0, l = 0, m = 0; j < nBlocks; j++, l+= nPoints)
        {
            for (k = 0; k < nPoints; k++, l++, m += nBlocks)
            {
                q15_t xCos = pTWD[2*(m & (nLength2-1))];
                q15_t xSin = pTWD[2*(m & (nLength2-1))+1];

                xRealA = COMPLEX_RE(output, l);
                xImagA = COMPLEX_IM(output, l);
                xRealB = COMPLEX_RE(output, l + nPoints);
                xImagB = COMPLEX_IM(output, l + nPoints);

                           dspfx_a32mul_q15( xCos, xRealB, acc);
                xRealB_t = dspfx_a32mac_q15(dspfx_neg_q15(xSin), xImagB, acc);

                           dspfx_a32mul_q15( xCos, xImagB, acc);
                xImagB   = dspfx_a32mac_q15( xSin, xRealB, acc);

                COMPLEX_RE(output, l)           = dspfx_add_q15(xRealA, xRealB_t) >> unitShift;
                COMPLEX_IM(output, l)           = dspfx_add_q15(xImagA, xImagB)   >> unitShift;
                COMPLEX_RE(output, l + nPoints) = dspfx_sub_q15(xRealA, xRealB_t) >> unitShift;
                COMPLEX_IM(output, l + nPoints) = dspfx_sub_q15(xImagA, xImagB)   >> unitShift;
            }
        }

        nBlocks >>= 1;
        nPoints <<= 1;
    }
}

#if defined(__Xdsp)

#define RADIX8_invBASEOP(w00, w01, w02, w03) \
{                                         \
	uint32_t w0 = COMPLEX_CONST_UI32(0x0000, 0x7fff); \
	uint32_t w1 = COMPLEX_CONST_UI32(0x5a82, 0x5a82); \
	uint32_t w2 = COMPLEX_CONST_UI32(0x7fff, 0x0000); \
	uint32_t w3 = COMPLEX_CONST_UI32(0x5a82, 0xa57e); \
                  \
	/* 1st pass */\
	_cbflyhf1r(a0); \
	a0 = _cbflyhf0r(w00, a1); \
	a1 = _cbflyhf1r(a2);      \
	a2 = _cbflyhf0r(w01, a3); \
	a3 = _cbflyhf1r(a4);      \
	a4 = _cbflyhf0r(w02, a5); \
	a5 = _cbflyhf1r(a6);      \
	a6 = _cbflyhf0r(w03, a7); \
	a7 = _cbflyhf1r(a0);      \
	/* 2nd pass */ \
	a0 = _cbflyhf0r(w0, a2);  \
	a2 = _cbflyhf1r(a1);      \
	a1 = _cbflyhf0r(w2, a3);  \
	a3 = _cbflyhf1r(a4);      \
	a4 = _cbflyhf0r(w0, a6);  \
	a6 = _cbflyhf1r(a5);      \
	a5 = _cbflyhf0r(w2, a7);  \
	a7 = _cbflyhf1r(a0);      \
	/* 3rd pass */ \
	a0 = _cbflyhf0r(w0, a4);  \
	a4 = _cbflyhf1r(a1);      \
	a1 = _cbflyhf0r(w1, a5);  \
	a5 = _cbflyhf1r(a2);      \
	a2 = _cbflyhf0r(w2, a6);  \
	a6 = _cbflyhf1r(a3);      \
	a3 = _cbflyhf0r(w3, a7);  \
	a7 = _cbflyhf1r(a0); \
}

#define RADIX4_invBASEOP(w00, w01) \
{ \
	uint32_t w0 = COMPLEX_CONST_UI32(0x0000, 0x7fff); \
	uint32_t w1 = COMPLEX_CONST_UI32(0x7fff, 0x0000); \
	_cbflyhf1r(a0); \
	/* 1st pass */            \
	a0 = _cbflyhf0r(w00, a1); \
	a1 = _cbflyhf1r(a2);      \
	a2 = _cbflyhf0r(w01, a3); \
	a3 = _cbflyhf1r(a0);      \
	/* 2nd pass */            \
	a0 = _cbflyhf0r(w0, a2);  \
	a2 = _cbflyhf1r(a1);      \
	a1 = _cbflyhf0r(w1, a3);  \
	a3 = _cbflyhf1r(a0); \
}

#define RADIX2_BASEOP_UNROLL2(stride, tw00, tw01) \
{ \
	uint32_t a0, a1, a2, b0, b1; \
    a1 = a[1*stride]; \
    a2 = a[2*stride]; \
    b0 = b[0]; \
    b1 = b[1*stride]; \
    a0 = (_cbflyhf0r(tw00, (b0))); \
    b0 = (_cbflyhf1r((a1))); \
    a1 = (_cbflyhf0r(tw01, (b1))); \
    b1 = (_cbflyhf1r((a2))); \
    a[0] = a0; \
    a[1*stride] = a1; \
    b[0] = b0; \
    b[1*stride] = b1; \
    a+=2*stride; \
    b+=2*stride; \
}


#if !defined(_ARCv2EM_XY_AGU_MEDIUM) // non-XY version
#if  defined(__Xdsp_complex) // works only with dsp complex instructions

#if defined(USE_RADIX2_Q15_FFT)
void dsp_cifft_radix2_q15_av2em(const cfft_q15_t* handle, const q15_t* input, q15_t* output)
{
    int32_t nBlocks, nPoints;
    int32_t nLength;
    int32_t i, j, k, m;
    int32_t shiftBits = handle->shiftBits;
    int32_t unitShift = 0;

    uint32_t Tw_mask = ((handle->length/2)-1);
    uint32_t *pTw = (uint32_t*)handle->ptwd;
    uint32_t Tw0 = COMPLEX_CONST_UI32(0x0000, 0x7fff); // W0 = 1 + 0i

    uint32_t *input_c  = (uint32_t*)input;  
    uint32_t *output_c = (uint32_t*)output;

    uint32_t *a;
    uint32_t *b;
    int32_t nOrder = 30-dspfx_norm_q31(handle->length);

    // _set_DSP_CTRL(DSP_CTRL_ROUNDING_UP); this register will be set externally by compiler
    // Set the transform length
    nLength = 1 << nOrder;

    // call radix2 permutation
    bitrev_permute_q15(input_c, output_c, handle->tab_bit_rev, nLength);

    // Perform the half length FFT (Decimation in Time)
    nBlocks = nLength >> 1;

    unitShift = (shiftBits-- > 0) ? 1 : 0;
    _set_DSP_FFT_CTRL(unitShift);
    a = &output_c[0];
    b = &output_c[1];

    _cbflyhf1r(*a);
    for (j = 0; j < nBlocks >> 1;  j++)
    {
	    /*
        *a++     = (_cbflyhf0r(Tw0, (*b++)));
        *a++     = (_cbflyhf1r((*b++)));
		*/

    	RADIX2_BASEOP_UNROLL2(2, Tw0, Tw0);
    }

    nBlocks >>= 1;
    nPoints = 2;

    for (i = 1; i < nOrder-1; i++)
    {
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_DSP_FFT_CTRL(unitShift);
        a = &output_c[0];
        b = &output_c[nPoints];
        for (j = 0, m = 0; j < nBlocks; j++)
        {           
            _cbflyhf1r((*a));
            for (k = 0; k < nPoints >> 1; k++)
            {
                /*
                uint32_t xTw = (pTw[m & Tw_mask]);
                *a++ = (_cbflyhf0r(xTw, (*b)));
                *b++ = (_cbflyhf1r((*a)));
                */
                uint32_t xTw0;
                uint32_t xTw1;
				xTw0 = pTw[m & Tw_mask];
				m += nBlocks;
				xTw1 = pTw[m & Tw_mask];
				m += nBlocks;
		    	RADIX2_BASEOP_UNROLL2(1, xTw0, xTw1);
            }
            b+=nPoints;
            a+=nPoints;
        }

        nBlocks >>= 1;
        nPoints <<= 1;
    }
    a = &output_c[0];
    b = &output_c[nPoints];
    for (; i< nOrder; i++){
        _cbflyhf1r((*a));
        m = 0;
        for (k = 0; k < nPoints >> 1; k++)
        {
            uint32_t xTw0 = (pTw[m++]);
            uint32_t xTw1 = (pTw[m++]);

            RADIX2_BASEOP_UNROLL2(1, xTw0, xTw1);

        }
    }
}

#else

#define ld_post_inc(a,b,c) __asm__ __volatile__("ld.ab %0, [%1, %2]": "=r"(a),"+r"(b):"r"(c))
#define ld_pre_inc(a,b,c) __asm__ __volatile__("ld.a %0, [%1, %2]": "=r"(a),"+r"(b):"r"(c))

#define store_with_stride(a,b,c,d,e,f,g,h,j,k)                     \
    __asm__ __volatile__(                                          \
    "st %1, [%0]\n\t"                                              \
    "add %0,%0,%9\n\t"                                             \
    "st %2, [%0]\n\t"                                              \
    "add %0,%0,%9\n\t"                                             \
    "st %3, [%0]\n\t"                                              \
    "add %0,%0,%9\n\t"                                             \
    "st %4, [%0]\n\t"                                              \
    "add %0,%0,%9\n\t"                                             \
    "st %5, [%0]\n\t"                                              \
    "add %0,%0,%9\n\t"                                             \
    "st %6, [%0]\n\t"                                              \
    "add %0,%0,%9\n\t"                                             \
    "st %7, [%0]\n\t"                                              \
    "add %0,%0,%9\n\t"                                             \
    "st %8, [%0]\n\t"                                              \
    "add %0,%0,%9\n\t"                                             \
    :"+r"(j):                                                      \
    "r"(a),"r"(b),"r"(c),"r"(d),"r"(e),"r"(f),"r"(g),"r"(h),"r"(k) \
    )

void dsp_cifft_mradix_q15_av2em(const cfft_q15_t* handle, const q15_t* input, q15_t* output)
{
    uint32_t* p_input         = (uint32_t*)&input[0];
    uint32_t* p_output        = (uint32_t*)&output[0];
    uint32_t* twfs            = (uint32_t*)&handle->ptwd[0];
    int32_t i, n_dfts, n_blocks;
    int32_t N = handle->length;
    int32_t n_stages = 30-dspfx_norm_q31(N);

    if(handle->scaling == DSP_TRANSFORM_SCALING_SCALED) {
        _set_DSP_FFT_CTRL(1);
    } else {
        _set_DSP_FFT_CTRL(0);
    }

    n_dfts = N >> 3;
    n_blocks = 1;

    // radix2 permutation
    bitrev_permute_q15(p_input, p_output, handle->tab_bit_rev, N);

    // First radix-8 stage without multiplication by twiddle factors
    if (n_stages >= 3)
    {
        int32_t dft;
        uint32_t*  pin = &p_output[0];

        for(dft = 0; dft < n_dfts; dft++){
            uint32_t  a0 = pin[0];
            uint32_t  a1 = pin[1];
            uint32_t  a2 = pin[2];
            uint32_t  a3 = pin[3];
            uint32_t  a4 = pin[4];
            uint32_t  a5 = pin[5];
            uint32_t  a6 = pin[6];
            uint32_t  a7 = pin[7];

            RADIX8_invBASEOP(w0, w0, w0, w0);

            pin[0] = a0;
            pin[1] = a1;
            pin[2] = a2;
            pin[3] = a3;
            pin[4] = a4;
            pin[5] = a5;
            pin[6] = a6;
            pin[7] = a7;

            pin+=8;
        }
        n_dfts = n_dfts >> 3;
        n_blocks = n_blocks << 3;
        n_stages -= 3;
    }

    // Remaining radix-8 stages
    while (n_stages >= 3)
    {
        int32_t dft;
        int32_t b_tw_stride;
        int32_t block;
        for(block = 0, b_tw_stride = 0; block < n_blocks; block++, b_tw_stride+=(n_dfts*sizeof(uint32_t)))
        {
            uint32_t* ptwf = &twfs[0];
            uint32_t W1;
            uint32_t W2;
            uint32_t W3;
            uint32_t W4;
            uint32_t W5;
            uint32_t W6;
            uint32_t W7;

            ld_pre_inc(W1, ptwf,  b_tw_stride);
            ld_pre_inc(W2, ptwf,  b_tw_stride);
            ld_pre_inc(W3, ptwf,  b_tw_stride);
            ld_pre_inc(W4, ptwf,  b_tw_stride);
            ld_pre_inc(W5, ptwf,  b_tw_stride);
            ld_pre_inc(W6, ptwf,  b_tw_stride);
            ld_pre_inc(W7, ptwf,  b_tw_stride);

            uint32_t* pin  = &p_output[block];
            uint32_t* poutput = &p_output[block];
            int32_t b_in_stride = n_blocks*4; // output stride in bytes
            for(dft = 0; dft < n_dfts; dft++){
                uint32_t register a0;
                uint32_t register a1;
                uint32_t register a2;
                uint32_t register a3;
                uint32_t register a4;
                uint32_t register a5;
                uint32_t register a6;
                uint32_t register a7;

                ld_post_inc(a0, pin, b_in_stride);
                ld_post_inc(a1, pin, b_in_stride);
                ld_post_inc(a2, pin, b_in_stride);
                ld_post_inc(a3, pin, b_in_stride);
                ld_post_inc(a4, pin, b_in_stride);
                ld_post_inc(a5, pin, b_in_stride);
                ld_post_inc(a6, pin, b_in_stride);
                ld_post_inc(a7, pin, b_in_stride);

                a2 = _cmpyhfr(a2, W2);
                a4 = _cmpyhfr(a4, W1);
                a6 = _cmpyhfr(a6, W3);

                RADIX8_invBASEOP(W4, W6, W5, W7);

                store_with_stride(a0,a1,a2,a3,a4,a5,a6,a7,poutput,b_in_stride);

            }
        }
        n_dfts = n_dfts >> 3;
        n_blocks = n_blocks << 3;
        n_stages -= 3;
    }

    // Radix-4 stage if needed
    if (n_stages >= 2)
    {
        uint32_t* a = &p_output[0];
        uint32_t nblk1 = n_blocks*1;
        uint32_t nblk2 = n_blocks*2;
		uint32_t nblk3 = n_blocks*3;

        for (i = 0; i < n_blocks;  i++)
        {
            uint32_t a0 = a[0];
            uint32_t a1 = a[nblk1];
            uint32_t a2 = a[nblk2];
            uint32_t a3 = a[nblk3];

            uint32_t W1 = twfs[i];
            uint32_t W2 = twfs[2*i];
            uint32_t W3 = twfs[3*i];

            a2 = _cmpyhfr(a2, W1);

            RADIX4_invBASEOP(W2, W3);

            a[0]     = a0;
            a[nblk1] = a1;
            a[nblk2] = a2;
            a[nblk3] = a3;
            a++;
        }
        n_stages -= 2;
    }

    // Radix-2 stage if needed
    if (n_stages >= 1)
    {
        uint32_t* __restrict a = &p_output[0];
        uint32_t* __restrict b = &p_output[n_blocks];
        uint32_t* __restrict w = &twfs[0];
        _cbflyhf1r(*a);
        for (i = 0; i < n_blocks >> 1;  i++) // unroll by 2
        {
            uint32_t w1, w2;
            w1 = w[0];
            w2 = w[1];

	    	RADIX2_BASEOP_UNROLL2(1, w1, w2);

            w+=2;
        }
    }
}


#endif

#endif //__Xdsp_complex
#else  // ARCv2EM XY version of FFT
#if defined __Xdsp_complex // Support FFT accelerator
void dsp_cifft_radix2_q15_av2em(const cfft_q15_t* handle, const q15_t* input, q15_t* output)
{
    int32_t stage, n_dfts, n_blocks, block, dft, N;
    uint32_t input_bitrev_base, input_bitrev_offset;
    uint32_t* twfs            = (uint32_t*)&handle->ptwd[0];
    int32_t n_stages = 30-dspfx_norm_q31(handle->length);
    N = handle->length;

    input_bitrev_base = (uint32_t)input & ~((uint32_t)(N << 2) - 1);
    input_bitrev_offset = ((uint32_t)input - input_bitrev_base) >> 2;

    _DSPLIB_store_regs(3, 8, 1);

    _set_agu_ap0((uint32_t)input);
    _set_agu_ap1((uint32_t)output);
    _set_agu_os0(input_bitrev_offset);
    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_BITREV_INC_ui32(N >> 1));
    _set_agu_mod1(0, AGU_DT_ui32, AGU_AD_INC_ui32_REG(0));
    _set_agu_mod2(0, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(0));
    _set_agu_mod3(0, AGU_DT_ui32, AGU_AD_INC_ui32(1));
    _set_agu_mod4(1, AGU_DT_ui32, AGU_AD_INC_ui32(1));

    if(handle->scaling == DSP_TRANSFORM_SCALING_SCALED) {
        _set_DSP_FFT_CTRL(1);
    } else { 
        _set_DSP_FFT_CTRL(0);
    }
    n_blocks = N >> 1;
    if (input == output){

        // in-place bit-reversal
        _set_agu_mod5(0, AGU_DT_ui32, AGU_AD_INC_ui32(0));
        _set_agu_mod6(1, AGU_DT_ui32, AGU_AD_INC_ui32(0));
        __builtin_assume(N > 0);
        for (block = 0; block < N; block++){
            uint32_t t;

            if ((unsigned)_agu_ap0() > (unsigned)_agu_ap1()){
                t = _agu_u5();
                _st_agu_u2(_agu_u6());
                _st_agu_u4(t);                     // update pointer to bitrev_base address
            } else {
                _agu_u2();                          // update pointer to bitrev_base address
                _agu_u4();
            }
            _agu_u0();                              // bit-reverse pointer update
            _agu_u1();                              // offset to input
        }

        // perform first stage of dit fft
        _set_agu_ap0((uint32_t)input);
        _set_agu_ap1((uint32_t)output);

        _cbflyhf1r(_agu_u4());
        __builtin_assume(n_blocks > 0);
        for (block = 0; block < n_blocks; block++){        
            uint32_t  w0 = COMPLEX_CONST_UI32(0x0000,0x7fff);
            _st_agu_u3(_cbflyhf0r(_agu_u4(), w0));
            _st_agu_u3(_cbflyhf1r(_agu_u4()));

        }
    } else if (input_bitrev_offset != 0 ) { // input buffer is not aligned by fft length
        _cbflyhf1r(_agu_u2()); // update pointer to bitrev_base address
        __builtin_assume(n_blocks > 0);
        for (block = 0; block < n_blocks; block++){
            uint32_t  w0 = COMPLEX_CONST_UI32(0x0000,0x7fff);
            _agu_u0();                              // bit-reverse pointer update
            _agu_u1();                              // offset to input
            _st_agu_u4(_cbflyhf0r(_agu_u2(), w0)); // update pointer to bitrev_base address

            _agu_u0();                              // bit-reverse pointer update
            _agu_u1();                              // offset to input
            _st_agu_u4(_cbflyhf1r(_agu_u2()));     // update pointer to bitrev_base address

        }
    } else { // input buffer is aligned by fft length
        _cbflyhf1r(_agu_u0());
        __builtin_assume(n_blocks > 0);
        for (block = 0; block < n_blocks; block++){
            uint32_t  w0 = COMPLEX_CONST_UI32(0x0000,0x7fff);
            _st_agu_u4(_cbflyhf0r(_agu_u0(), w0));
            _st_agu_u4(_cbflyhf1r(_agu_u0()));

        }
    }
    n_blocks = n_blocks >> 1;
    n_dfts = 2;

#if _CFFT_VER == 2
    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_ui32_REG(1));
    _set_agu_mod1(0, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(2));
    _set_agu_mod2(1, AGU_DT_ui32, AGU_AD_INC_ui32_REG(1));
    _set_agu_mod5(1, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(2));
    _set_agu_mod6(2, AGU_DT_ui32, AGU_AD_INC_ui32_REG(3));
    _set_agu_mod7(2, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(0));
#endif

    for (stage = 1; stage < n_stages; stage++) {
        // set pointer for inputs
        _set_agu_ap0((uint32_t)output);
        // set pointer for outputs
        _set_agu_ap1((uint32_t)output);

#if _CFFT_VER == 1
        _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_ui32(n_dfts));
        _set_agu_mod1(0, AGU_DT_ui32, AGU_AD_DEC_ui32((N - 1 - n_dfts)));

        _set_agu_mod2(1, AGU_DT_ui32, AGU_AD_INC_ui32(n_dfts));
        _set_agu_mod3(1, AGU_DT_ui32, AGU_AD_DEC_ui32((N - 1 - n_dfts)));

        // set pointer for twiddle factors
        _set_agu_ap2((uint32_t)twfs);
        _set_agu_mod4(2, AGU_DT_ui32, AGU_AD_INC_ui32(n_blocks));        

        _cbflyhf1r(_agu_u0());
        for (dft = 0; dft < n_dfts; dft++) {
            uint32_t w;            
            w = _agu_u4();

            for (block = 0; block < n_blocks - 1; block++) {
                _st_agu_u2(_cbflyhf0r(_agu_u0(), w));
                _st_agu_u2(_cbflyhf1r(_agu_u0()));
            }
            _st_agu_u2(_cbflyhf0r(_agu_u1(), w));
            _st_agu_u3(_cbflyhf1r(_agu_u0()));
        }
#endif
#if _CFFT_VER == 2

        _set_agu_os0(n_blocks * (n_dfts - 1));
        _set_agu_os1(n_dfts);
        _set_agu_os2(n_dfts-1);
        _set_agu_os3(n_blocks);

        // set pointer for twiddle factors
        _set_agu_ap2((uint32_t)twfs);

        _cbflyhf1r(_agu_u0());
        __builtin_assume(n_blocks > 0);
        for (block = 0; block < n_blocks; block++)
        {
        	__builtin_assume(n_dfts - 1 > 0);
            for (dft = 0; dft < n_dfts - 1; dft++)
            {
                _st_agu_u2(_cbflyhf0r(_agu_u1(), _agu_u6()));
                _st_agu_u5(_cbflyhf1r(_agu_u0()));
            }
            _st_agu_u2(_cbflyhf0r(_agu_u3(), _agu_u7()));
            _st_agu_u4(_cbflyhf1r(_agu_u0()));
        }
#endif
        n_blocks >>= 1;
        n_dfts <<= 1;
    }
    _DSPLIB_restore_regs();
}

#else //__Xdsp_complex, FFT accelerator is not present
void dsp_cifft_radix2_q15_av2em_nobfly(const cfft_q15_t* handle, const q15_t* input, q15_t* output)
{
    int32_t stage, n_dfts, n_blocks, block, dft, N, shift;
    uint32_t input_bitrev_base, input_bitrev_offset;
    uint32_t* twfs            = (uint32_t*)&handle->ptwd[0];
    int32_t n_stages = 30-dspfx_norm_q31(handle->length);
    N = handle->length;

    input_bitrev_base = (uint32_t)input & ~((uint32_t)(N << 2) - 1);
    input_bitrev_offset = ((uint32_t)input - input_bitrev_base) >> 2;

    _DSPLIB_store_regs(2, 6, 2);

    _set_agu_ap0((uint32_t)input);
    _set_agu_ap1((uint32_t)output);
    _set_agu_os0(input_bitrev_offset);
    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_BITREV_INC_ui32(N >> 1));
    _set_agu_mod1(0, AGU_DT_ui32, AGU_AD_INC_ui32_REG(0));
    _set_agu_mod2(0, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(0));
    _set_agu_mod3(1, AGU_DT_ui32, AGU_AD_INC_ui32(1));


    if(handle->scaling == DSP_TRANSFORM_SCALING_SCALED) {
    	shift = 0x00010001;
    } else {
    	shift = 0;
    }
    n_blocks = N >> 1;
    if (input == output){

        // in-place bit-reversal
    	_set_agu_mod4(0, AGU_DT_ui32, AGU_AD_INC_ui32(0));
        _set_agu_mod5(1, AGU_DT_ui32, AGU_AD_INC_ui32(0));

        for (block = 0; block < N; block++){
            uint32_t t;

            if ((unsigned)_agu_ap0() > (unsigned)_agu_ap1()){
                t = _agu_u4();
                _st_agu_u2(_agu_u5());
                _st_agu_u3(t);                     // update pointer to bitrev_base address
            } else {
                _agu_u2();                          // update pointer to bitrev_base address
                _agu_u3();
            }
            _agu_u0();                              // bit-reverse pointer update
            _agu_u1();                              // offset to input
        }


        // perform first stage of dit fft
        _set_agu_ap0((uint32_t)input);
        _set_agu_ap1((uint32_t)output);
        _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_ui32(1));
        _set_agu_mod1(1, AGU_DT_ui32, AGU_AD_INC_ui32(1));

        for (block = 0; block < n_blocks; block++){
        	uint32_t a0, b0;
        	a0 = _vasr2h(_agu_u1(), shift);
        	b0 = _vasr2h(_agu_u1(), shift);
            _st_agu_u0(_vadd2h(a0, b0));
            _st_agu_u0(_vsub2h(a0, b0));

        }
    } else if (input_bitrev_offset != 0) {
        for (block = 0; block < n_blocks; block++){

        	uint32_t a0, b0;
            a0 = _vasr2h(_agu_u2(), shift);        // update pointer to bitrev_base address
            _agu_u0();                              // bit-reverse pointer update
            _agu_u1();                              // offset to input
            b0 = _vasr2h(_agu_u2(), shift);        // update pointer to bitrev_base address
            _st_agu_u3(_vadd2h(a0, b0));
            _st_agu_u3(_vsub2h(a0, b0));
            _agu_u0();                              // bit-reverse pointer update
            _agu_u1();                              // offset to input


        }
	} else {
        for (block = 0; block < n_blocks; block++){

        	uint32_t a0, b0;
            a0 = _vasr2h(_agu_u0(), shift);
            b0 = _vasr2h(_agu_u0(), shift);
            _st_agu_u3(_vadd2h(a0, b0));
            _st_agu_u3(_vsub2h(a0, b0));

        }
	}
    n_blocks = n_blocks >> 1;
    n_dfts = 2;

    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_ui32(0));
    _set_agu_mod1(0, AGU_DT_ui32, AGU_AD_INC_ui32_REG(0));
    _set_agu_mod2(0, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(1));
    _set_agu_mod3(1, AGU_DT_ui32, AGU_AD_INC_ui32(0));
    _set_agu_mod4(1, AGU_DT_ui32, AGU_AD_INC_ui32_REG(0));
    _set_agu_mod5(1, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(1));

    for (stage = 1; stage < n_stages; stage++) {
        // set pointer for A
        _set_agu_ap0((uint32_t)output);
        _set_agu_os0(2*n_dfts);
        _set_agu_os1((N - 1 - 2*n_dfts));
        // set pointer for B
        _set_agu_ap1((uint32_t)(output + 2*n_dfts));
        // set pointer for twiddle factors
        _set_agu_ap2((uint32_t)twfs);
        _set_agu_mod6(2, AGU_DT_ui32, AGU_AD_INC_ui32(n_blocks));

        for (dft = 0; dft < n_dfts; dft++) {
            uint32_t w, a0;
            uint32_t T;
            w = _vasr2h(_agu_u6(), shift);

            for (block = 0; block < n_blocks - 1; block++) 
            {
            	T = _cmpyhfr(_agu_u3(), w);         // Load B0, non-increment
            	a0 = _vasr2h(_agu_u0(), shift);    // Load A0, non-increment.
            	_st_agu_u4(_vsub2h(a0, T));         //  Store B1, increment to next B
            	_st_agu_u1(_vadd2h(a0, T));         //  Store A1, increment to next A
            }
            T = _cmpyhfr(_agu_u3(), w);         // Load B0, non-increment
            a0 = _vasr2h(_agu_u0(), shift);    // Load A0, non-increment.
            _st_agu_u5(_vsub2h(a0, T));
            _st_agu_u2(_vadd2h(a0, T));
        }
        n_blocks >>= 1;
        n_dfts <<= 1;
    }
    _DSPLIB_restore_regs();
}


#endif //__Xdsp_complex
#endif // _ARCv2EM_XY_AGU_MEDIUM
#if defined __Xdsp_complex // Use small length transforms only with FFT accelerator
void dsp_cifft_4_q15_av2em(const cfft_q15_t* handle, const q15_t* input, q15_t* output)
{
    uint32_t *pin = (uint32_t*) input;
    uint32_t *pout = (uint32_t*) output;

    uint32_t a0 = pin[0];
    uint32_t a2 = pin[1];
    uint32_t a1 = pin[2];
    uint32_t a3 = pin[3];

    if(handle->scaling == DSP_TRANSFORM_SCALING_SCALED) {
        _set_DSP_FFT_CTRL(1);
    } else {
        _set_DSP_FFT_CTRL(0);
    }

    RADIX4_invBASEOP(w0, w0);

    pout[0] = a0;
    pout[1] = a1;
    pout[2] = a2;
    pout[3] = a3;

}

void dsp_cifft_8_q15_av2em(const cfft_q15_t* handle, const q15_t* input, q15_t* output)
{
    uint32_t *pin = (uint32_t*) input;
    uint32_t *pout = (uint32_t*) output;

    uint32_t  a0 = pin[0];
    uint32_t  a4 = pin[1];
    uint32_t  a2 = pin[2];
    uint32_t  a6 = pin[3];
    uint32_t  a1 = pin[4];
    uint32_t  a5 = pin[5];
    uint32_t  a3 = pin[6];
    uint32_t  a7 = pin[7];

    if(handle->scaling == DSP_TRANSFORM_SCALING_SCALED) {
        _set_DSP_FFT_CTRL(1);
    } else {
        _set_DSP_FFT_CTRL(0);
    }

    RADIX8_invBASEOP(w0, w0, w0, w0);

    pout[0] = a0;
    pout[1] = a1;
    pout[2] = a2;
    pout[3] = a3;
    pout[4] = a4;
    pout[5] = a5;
    pout[6] = a6;
    pout[7] = a7;
}

#pragma Off_inline(dsp_cifft_4_q15_av2em, dsp_cifft_8_q15_av2em)

#endif // __Xdsp_complex
#endif // __Xdsp

#define XY_MAP_INPUT (((XY_REGION_SIZE/2))/sizeof(q15_t))
#define XY_MAP_OUT 0
#define XY_MAP_TWIDDLES 0
#define XY_IFFT_RADIX2_Q15_THRESHOLD (((XY_REGION_SIZE/4))/sizeof(q15_t))

/**
 * Inverse Complex FFT "process" function for q15 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

void _DSPLIB_XYCC dsp_cifft_q15(const cfft_q15_t* handle, const PTR(q15_t) input, PTR(q15_t) output)
{
#if defined(__Xdsp) && defined(__Xdsp_complex)
#if defined(DSP_CFFT_Q15_4)
    if (handle->length == 4){
        dsp_cifft_4_q15_av2em(handle, input, output);
    } else
#endif
#if defined(DSP_CFFT_Q15_8)
    if (handle->length == 8){
        dsp_cifft_8_q15_av2em(handle, input, output);
    } else
#endif
    {
#if defined(USE_RADIX2_Q15_FFT)
        dsp_cifft_radix2_q15_av2em(handle, input, output);          
#else
        dsp_cifft_mradix_q15_av2em(handle, input, output);
#endif
    }
#else
#if defined(_ARC_AS210_mac16)
    if (handle->length > XY_IFFT_RADIX2_Q15_THRESHOLD) {
#endif
#if defined(__Xdsp) && defined(_ARCv2EM_XY_AGU_MEDIUM) // Use ARCv2EM XY version without FFT accelerator
    	dsp_cifft_radix2_q15_av2em_nobfly(handle, input, output);
#else
        risc_dsp_cifft_radix2_q15(handle, input, output);
#endif
#if defined(_ARC_AS210_mac16)
    } else {
        cfft_rad2_q15_xy_t pInst;

        dsp_cifft_radix2_init_q15_cfX_DMAC16x16(
            &pInst, 
            handle->length, 
            XY_MAP_TWIDDLES);

        pInst.shiftBits = handle->shiftBits;

        memcpy_StoX_int16(
            XY_MAP_INPUT, 
            input, 
            handle->length*2); 

        dsp_cfft_radix2_q15_XtoY_cfX_DMAC16x16(
            &pInst, 
            XY_MAP_INPUT, 
            XY_MAP_OUT);

        memcpy_YtoS_int16(
            output, 
            XY_MAP_OUT, 
            handle->length*2);
    }
#endif
#endif
}

void dsp_cifft_delete_q15(cfft_q15_t* handle) { };

#endif //!defined(EXCLUDE_DSP_CIFFT_Q15)
