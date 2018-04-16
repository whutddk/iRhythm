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

#ifndef ARC_DSPLIB_HELPER_H
#define ARC_DSPLIB_HELPER_H

#include "dsplib_primitives.h"

#if !defined(_Inline) && defined(_MSC_VER)
#define _Inline _inline
#endif

#if defined(_ARCv2EM_XY_11D)
#include <memory.h>
#endif

#if defined(_ARC600_XY)

#ifndef WRAPPER_BURSTS_ON
#include <memory.h>

#define memcpy_StoX_int32(pdst, pSrc, size) memcpy(((q31_t*)_get_xbase()) + (pdst), (pSrc), (size)*sizeof(q31_t))
#define memcpy_StoY_int32(pdst, pSrc, size) memcpy(((q31_t*)_get_ybase()) + (pdst), (pSrc), (size)*sizeof(q31_t))
#define memcpy_XtoS_int32(pDst, psrc, size) memcpy((pDst), ((q31_t*)_get_xbase()) + (psrc), (size)*sizeof(q31_t))
#define memcpy_YtoS_int32(pDst, psrc, size) memcpy((pDst), ((q31_t*)_get_ybase()) + (psrc), (size)*sizeof(q31_t))

static _Inline uint32_t memcpy_StoX_int16(uint32_t pdst, const q15_t* pSrc, int32_t size) 
{
	memcpy(((q15_t*)_get_xbase()) + (pdst), (pSrc), (size)*sizeof(q15_t));
	return pdst;
};

static _Inline uint32_t memcpy_StoY_int16(uint32_t pdst, const q15_t* pSrc, int32_t size) {
	memcpy(((q15_t*)_get_ybase()) + (pdst), (pSrc), (size)*sizeof(q15_t));
	return pdst;
};

static _Inline void memcpy_XtoS_int16(q15_t* pDst, uint32_t psrc, int32_t size) {
	memcpy((pDst), ((q15_t*)_get_xbase()) + (psrc), (size)*sizeof(q15_t));
};

static _Inline void memcpy_YtoS_int16(q15_t* pDst, uint32_t psrc, int32_t size) {
	memcpy((pDst), ((q15_t*)_get_ybase()) + (psrc), (size)*sizeof(q15_t));
};


#else

#define memcpy_StoX_int32(pdst, pSrc, size) _BurstSMemToXMemSafe((pdst), (pSrc), (size))
#define memcpy_StoY_int32(pdst, pSrc, size) _BurstSMemToYMemSafe((pdst), (pSrc), (size))
#define memcpy_XtoS_int32(pDst, psrc, size) _BurstXMemToSMemSafe((pDst), (psrc), (size))
#define memcpy_YtoS_int32(pDst, psrc, size) _BurstYMemToSMemSafe((pDst), (psrc), (size))

uint32_t memcpy_StoX_int16(uint32_t pdst, const q15_t* pSrc, int32_t size);
uint32_t memcpy_StoY_int16(uint32_t pdst, const q15_t* pSrc, int32_t size);
void memcpy_XtoS_int16(q15_t* pDst, uint32_t psrc, int32_t size);
void memcpy_YtoS_int16(q15_t* pDst, uint32_t psrc, int32_t size);

#endif

#endif

#ifndef ASSERT
#if defined(_MSC_VER) && defined(_DEBUG)
#define ASSERT(x)   if (!(x)) __asm int 3
#else
#undef ASSERT
#define ASSERT(x) {}
#endif
#endif

#if defined(EXCLUDE_DSP_MAT_INIT_Q31)
#define EXCLUDE_DSP_MAT_MULT_Q31
#define EXCLUDE_DSP_MAT_ADD_Q31
#define EXCLUDE_DSP_MAT_SUB_Q31
#define EXCLUDE_DSP_MAT_SCALE_Q31
#define EXCLUDE_DSP_MAT_TRANS_Q31
#define EXCLUDE_DSP_CMPL_MAT_MULT_Q31
#define EXCLUDE_DSP_CMPL_MAT_ADD_Q31
#define EXCLUDE_DSP_CMPL_MAT_SUB_Q31
#define EXCLUDE_DSP_CMPL_MAT_SCALE_Q31
#define EXCLUDE_DSP_CMPL_MAT_TRANS_Q31
#endif //defined(EXCLUDE_DSP_MAT_INIT_Q31)

#if defined(EXCLUDE_DSP_MAT_INIT_Q15)
#define EXCLUDE_DSP_MAT_MULT_Q15
#define EXCLUDE_DSP_MAT_ADD_Q15
#define EXCLUDE_DSP_MAT_SUB_Q15
#define EXCLUDE_DSP_MAT_SCALE_Q15
#define EXCLUDE_DSP_MAT_TRANS_Q15
#define EXCLUDE_DSP_CMPL_MAT_MULT_Q15
#define EXCLUDE_DSP_CMPL_MAT_ADD_Q15
#define EXCLUDE_DSP_CMPL_MAT_SUB_Q15
#define EXCLUDE_DSP_CMPL_MAT_SCALE_Q15
#define EXCLUDE_DSP_CMPL_MAT_TRANS_Q15
#endif //defined(EXCLUDE_DSP_MAT_INIT_Q15)

#if defined(EXCLUDE_DSP_MAT_INIT_F32)
#define EXCLUDE_DSP_MAT_MULT_F32
#define EXCLUDE_DSP_MAT_ADD_F32
#define EXCLUDE_DSP_MAT_SUB_F32
#define EXCLUDE_DSP_MAT_SCALE_F32
#define EXCLUDE_DSP_MAT_TRANS_F32
#define EXCLUDE_DSP_CMPL_MAT_MULT_F32
#define EXCLUDE_DSP_CMPL_MAT_ADD_F32
#define EXCLUDE_DSP_CMPL_MAT_SUB_F32
#define EXCLUDE_DSP_CMPL_MAT_SCALE_F32
#define EXCLUDE_DSP_CMPL_MAT_TRANS_F32
#endif //defined(EXCLUDE_DSP_MAT_INIT_F32)

#if defined(EXCLUDE_DSP_TRANSFORM_TABLES_FFT)
#define EXCLUDE_DSP_CFFT_Q15
#define EXCLUDE_DSP_CIFFT_Q15
#define EXCLUDE_DSP_CFFT_Q31
#define EXCLUDE_DSP_CIFFT_Q31
#define EXCLUDE_DSP_CFFT_RADIX2_Q31
#endif //defined(EXCLUDE_DSP_TRANSFORM_TABLES_FFT)

#if defined(EXCLUDE_DSP_TRANSFORM_TABLES_FFT) || defined(EXCLUDE_DSP_CFFT_Q15)
#define EXCLUDE_DSP_RFFT_Q15
#define EXCLUDE_DSP_RIFFT_Q15
#define EXCLUDE_DSP_DCT2_Q15
#define EXCLUDE_DSP_DCT3_Q15
#define EXCLUDE_DSP_DCT4_Q15
#define EXCLUDE_DSP_MDCT_Q15
#define EXCLUDE_DSP_IMDCT_Q15
#endif

#if defined(EXCLUDE_DSP_TRANSFORM_TABLES_FFT) || defined(EXCLUDE_DSP_CFFT_Q31)
#define EXCLUDE_DSP_RFFT_Q31
#define EXCLUDE_DSP_RIFFT_Q31
#define EXCLUDE_DSP_DCT2_Q31
#define EXCLUDE_DSP_DCT3_Q31
#define EXCLUDE_DSP_DCT4_Q31
#define EXCLUDE_DSP_MDCT_Q31
#define EXCLUDE_DSP_IMDCT_Q31
#endif

#define ADDR_ALIGN(addr, align) (((uint32_t)(addr) + (align) - 1) & (~((align)-1)))

// Next macro is used to get aligned base address for specific length
// Alignment mask is value which is next power of 2 for specific length
// dspfx_norm_q31(length) returns 31 for 0x00000000 and 30 for 0x00000001
#define ADDR_ALIGNED_BASE32(addr, length) (((uint32_t)(addr)) & (~((4<<(30-dspfx_norm_q31(length)))-1)))
#define ADDR_ALIGNED_BASE16(addr, length) (((uint32_t)(addr)) & (~((2<<(30-dspfx_norm_q31(length)))-1)))
#define ADDR_ALIGNED_BASE8(addr, length)  (((uint32_t)(addr)) & (~((1<<(30-dspfx_norm_q31(length)))-1)))

#define MODULE_ADDR_INC(p, pFirst , pLast) if (p==pLast) p=pFirst; else p++;
#define MODULE_ADDR_DEC(p, pFirst , pLast) if (p==pFirst) p=pLast; else p--;

#define _AGU_CIRC_DEC(base, offset, mod) base = _agu_circ_dec(base, sizeof((base)[0])*offset, (mod*sizeof((base)[0])));
#define _AGU_CIRC_INC(base, offset, mod) base = _agu_circ_inc(base, sizeof((base)[0])*offset, (mod*sizeof((base)[0])));

#define _AGU_CIRC_DEC_NOT_ALIGNED(base, offset, mod, align_offset) { base -= align_offset; base = _agu_circ_dec(base, sizeof((base)[0])*offset, (mod*sizeof((base)[0]))); base+= align_offset; };
#define _AGU_CIRC_INC_NOT_ALIGNED(base, offset, mod, align_offset) { base -= align_offset; base = _agu_circ_inc(base, sizeof((base)[0])*offset, (mod*sizeof((base)[0]))); base+= align_offset; };

// Below macros requires loop unswitching feature in compiler.
// Instead this code will be compiled in non-optimal way
#define _AGU_CIRC_DEC_SAFE(base, offset, mod, align_offset) { \
	if (align_offset == 0) { \
		_AGU_CIRC_DEC(base, offset, mod); \
	} else { \
		_AGU_CIRC_DEC_NOT_ALIGNED(base, offset, mod, align_offset); \
	} \
}

// Below macros requires loop unswitching feature in compiler.
// Instead this code will be compiled in non-optimal way
#define _AGU_CIRC_INC_SAFE(base, offset, mod, align_offset) { \
	if (align_offset == 0) { \
		_AGU_CIRC_INC(base, offset, mod); \
	} else { \
		_AGU_CIRC_INC_NOT_ALIGNED(base, offset, mod, align_offset); \
	} \
}

#endif //ARC_DSPLIB_HELPER_H
