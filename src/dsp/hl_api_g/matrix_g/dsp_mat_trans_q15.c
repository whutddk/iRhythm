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

#if defined(EXCLUDE_DSP_MAT_INIT_Q15)
#define EXCLUDE_DSP_MAT_TRANS_Q15
#endif //defined(EXCLUDE_DSP_MAT_INIT_Q15)

#if !defined(EXCLUDE_DSP_MAT_TRANS_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC600_XY)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsplib_helper.h"

#pragma Code("dsplib_dsp_mat_trans_q15")

#if defined _ARCv2EM_XY_AGU_MEDIUM

#if defined _XY_HAND_OPT

dsp_status _DSPLIB_XYCC dsp_mat_trans_q15(const matrix_q15_t *pSrc, matrix_q15_t *pDst)
{
    int32_t    c,r;
    int32_t width = pSrc->colCnt;
    int32_t height = pSrc->rowCnt;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrc->rowCnt != pDst->colCnt) || (pSrc->colCnt != pDst->rowCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    if ((width >= 2) && (height >= 2)) 
	{
		int32_t half_width = width >> 1;
		int32_t half_height = height >> 1;
		int32_t src_backstride = -(((height & (~1)) * width)- width * 2 - 2);
		int32_t dst_forwardstride = height + 2 + (height & 1);
		int32_t dual_width = width * 2;
		_DSPLIB_store_regs(4, 10, 4);

		_set_agu_ap0(pSrc->pData); // Set AP registers for Src
		_set_agu_ap1((pSrc->pData + width)); // Set AP registers for Src
		_set_agu_ap2(pDst->pData); // Set AP registers for Dst
		_set_agu_ap3((pDst->pData + height)); // Set AP registers for Dst
		_set_agu_os0(src_backstride);
		_set_agu_os1(dst_forwardstride);
		_set_agu_os2(dual_width);

		_set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(0));
		_set_agu_mod1(0, AGU_DT_v2q15, AGU_AD_INC_q15_REG(2));
		_set_agu_mod2(0, AGU_DT_v2q15, AGU_AD_INC_q15_REG(0));
		_set_agu_mod3(1, AGU_DT_v2q15, AGU_AD_INC_q15(0));
		_set_agu_mod4(1, AGU_DT_v2q15, AGU_AD_INC_q15_REG(2));
		_set_agu_mod5(1, AGU_DT_v2q15, AGU_AD_INC_q15_REG(0));
		_set_agu_mod6(2, AGU_DT_v2q15, AGU_AD_INC_q15(2));
		_set_agu_mod7(3, AGU_DT_v2q15, AGU_AD_INC_q15(2));
		_set_agu_mod8(2, AGU_DT_v2q15, AGU_AD_INC_q15_REG(1));
		_set_agu_mod9(3, AGU_DT_v2q15, AGU_AD_INC_q15_REG(1));

		for (c = 0; c < half_width; c++)
		{
			for (r = 0; r < half_height - 1; r++)
			{
				_st_agu_u6(_vpack2hl(_agu_u3(), _agu_u0()));
				_st_agu_u7(_vpack2hm(_agu_u4(), _agu_u1()));
			}
			_st_agu_u8(_vpack2hl(_agu_u3(), _agu_u0()));
			_st_agu_u9(_vpack2hm(_agu_u5(), _agu_u2()));
		}
		if (height & 1) 
		{
			_set_agu_ap0((pSrc->pData + width*(height-1))); // Set AP registers for Src
            _set_agu_ap1((pDst->pData + height - 1)); // Set AP registers for Dst
			_set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(1));
			_set_agu_mod1(1, AGU_DT_q15, AGU_AD_INC_q15_REG(3));
			_set_agu_os3(height);
			for (c = 0; c < width; c++) 
			{
				_st_agu_u1(_agu_u0());
			}
		}
		if (width & 1) 
		{
			_set_agu_ap0((pSrc->pData + width - 1)); // Set AP registers for Src
            _set_agu_ap1((pDst->pData + height*(width - 1))); // Set AP registers for Dst
			_set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15_REG(3));
			_set_agu_mod1(1, AGU_DT_q15, AGU_AD_INC_q15(1));
			_set_agu_os3(width);			
			for (c = 0; c < height; c++) 
			{
				_st_agu_u1(_agu_u0());
			}
		}
		_DSPLIB_restore_regs();
	} 
	else 
	{
		_DSPLIB_store_regs(2, 2, 0);
		_set_agu_ap0(pSrc->pData); // Set AP registers for Src
		_set_agu_ap1(pDst->pData); // Set AP registers for Dst

		_set_agu_mod0(0, AGU_DT_v2q15, AGU_AD_INC_q15(2));
		_set_agu_mod1(1, AGU_DT_v2q15, AGU_AD_INC_q15(2));

		for (c = 0; c < width * height; c++)
		{
    	    _st_agu_u1(_agu_u0());
		}
		_DSPLIB_restore_regs();
	}
    return DSP_ERR_OK;
}

#else

dsp_status _DSPLIB_XYCC dsp_mat_trans_q15(const matrix_q15_t *pSrc, matrix_q15_t *pDst)
{
    int32_t    c,r;
    PTR(q15_t)__restrict dst = (PTR(q15_t)) pDst->pData;
    PTR(q15_t)__restrict src = (PTR(q15_t)) pSrc->pData;
    int32_t width = pSrc->colCnt;
    int32_t height = pSrc->rowCnt;
    int32_t backstride = - (pSrc->size - 1);

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrc->rowCnt != pDst->colCnt) || (pSrc->colCnt != pDst->rowCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    pDst->rowCnt = pSrc->colCnt;
    pDst->colCnt = pSrc->rowCnt;

    __builtin_assume(width > 0);
    __builtin_assume(height > 0);

    for (c = 0; c < width; c++)
    {
        for (r = 0; r < height; r++)
        {
            *dst++ = *src;
            src += width;
        }
        src += backstride;
    }

    return DSP_ERR_OK;
}

#endif

#else

static _Inline dsp_status risc_dsp_mat_trans_q15(const matrix_q15_t *pSrc, matrix_q15_t *pDst)
{
    uint32_t    c,r;
    q15_t*__restrict dst = pDst->pData;
    q15_t*__restrict src = pSrc->pData;
    uint16_t width = pSrc->colCnt;
    uint16_t height = pSrc->rowCnt;
    int32_t backstride = - (pSrc->size - 1);

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrc->rowCnt != pDst->colCnt) || (pSrc->colCnt != pDst->rowCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    pDst->rowCnt = pSrc->colCnt;
    pDst->colCnt = pSrc->rowCnt;

    PRAGMA_NO_UNROLL
    for (c = 0; c<width; c++)
    {
        for (r = 0; r<height; r++)
        {
            *dst++ = *src;
            src += width;
        }
        src += backstride;
    }

    return DSP_ERR_OK;
}

#define XY_MAT_TRANS_THRESHOLD 32

#define XY_BLOCK_SIZE ((XY_REGION_SIZE/2))

#define XYMAP_INPUT0 (0)
#define XYMAP_OUTPUT (0)

/**
 * Q15 matrix transpose.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status dsp_mat_trans_q15(const matrix_q15_t *pSrc, matrix_q15_t *pDst)
{
#if defined(_ARC600_XY)
    int32_t nSamples = pDst->size;
    if (nSamples < XY_MAT_TRANS_THRESHOLD || nSamples > XY_BLOCK_SIZE) {
#endif
        return risc_dsp_mat_trans_q15(pSrc, pDst);
#if defined(_ARC600_XY) 
    } else {
        matrix_q15_t xySrc, xyDst;
        ASSERT(nSamples <= XY_BLOCK_SIZE);

        memcpy((void *)((q15_t *)(_get_xbase()) + XYMAP_INPUT0), pSrc->pData, nSamples*2);

        //_BurstSMemToXMemSafe(XYMAP_INPUT0, pSrcA->pData, nSamples/2);

        dsp_mat_init_q15(&xySrc, pSrc->rowCnt, pSrc->colCnt, (q15_t *)(XYMAP_INPUT0));
        dsp_mat_init_q15(&xyDst, pDst->rowCnt, pDst->colCnt, (q15_t *)(XYMAP_OUTPUT));

        dsp_mat_trans_q15_XtoY((const matrix_q15_t *)&xySrc, &xyDst);

        pDst->rowCnt = xyDst.rowCnt;
        pDst->colCnt = xyDst.colCnt;

        memcpy((void *)pDst->pData, (void *)((q15_t *)(_get_ybase()) + XYMAP_OUTPUT), nSamples*2);
        //_BurstYMemToSMemSafe(pDst->pData, XYMAP_OUTPUT, nSamples/2);

        return DSP_ERR_OK;
    }
#endif
}

#endif

#endif //!defined(EXCLUDE_DSP_MAT_TRANS_Q15)
