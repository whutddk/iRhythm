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

#if defined(EXCLUDE_DSP_MAT_INIT_Q31)
#define EXCLUDE_DSP_MAT_TRANS_Q31
#endif //defined(EXCLUDE_DSP_MAT_INIT_Q31)

#if !defined(EXCLUDE_DSP_MAT_TRANS_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC600_XY)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"


#pragma Code("dsplib_dsp_mat_trans_q31")

#if defined _ARCv2EM_XY_AGU_MEDIUM

#if defined _XY_HAND_OPT

dsp_status _DSPLIB_XYCC dsp_mat_trans_q31(const matrix_q31_t *pSrc, matrix_q31_t *pDst)
{
    uint32_t    c,r;
    uint32_t width = pSrc->colCnt;
    uint32_t height = pSrc->rowCnt-1;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrc->rowCnt != pDst->colCnt) || (pSrc->colCnt != pDst->rowCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif
	_DSPLIB_store_regs(2, 3, 2);

    _set_agu_ap0(pSrc->pData); // Set AP registers for Src
    _set_agu_ap1(pDst->pData); // Set AP registers for Dst

	_set_agu_os0(-(pSrc->size - width - 1));
    _set_agu_os1(width);
	_set_agu_mod1(0, AGU_DT_q31, AGU_AD_INC_q31_REG(0));
    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31_REG(1));
    _set_agu_mod2(1, AGU_DT_q31, AGU_AD_INC_q31(1));

    for (c = 0; c < width; c++)
    {
    	//__builtin_assume(height > 0);
        for (r = 0; r < height; r++)
        {
			_st_agu_u2(_agu_u0());
        }
		_st_agu_u2(_agu_u1());
    }

	_DSPLIB_restore_regs();

    return DSP_ERR_OK;
}

#else

dsp_status _DSPLIB_XYCC dsp_mat_trans_q31(const matrix_q31_t *pSrc, matrix_q31_t *pDst)
{
    int32_t c,r;
    PTR(q31_t) __restrict dst = (PTR(q31_t)) pDst->pData;
    PTR(q31_t) __restrict src = (PTR(q31_t)) pSrc->pData;
    int32_t width = pSrc->colCnt;
    int32_t height = pSrc->rowCnt;
    int32_t backstride = - (int32_t)(pSrc->size - 1);

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


#endif //_XY_HAND_OPT

#else //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

#if defined _ARCVER_ARCv2HS_DSP && defined (__Xunaligned)
static _Inline dsp_status risc_dsp_mat_trans_q31(const matrix_q31_t *pSrc, matrix_q31_t *pDst)
{
    int32_t    c,r;
    int32_t width = pSrc->colCnt;
    int32_t height = pSrc->rowCnt;


#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrc->rowCnt != pDst->colCnt) || (pSrc->colCnt != pDst->rowCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    pDst->rowCnt = pSrc->colCnt;
    pDst->colCnt = pSrc->rowCnt;

    if (( width & 1) || (height & 1))
    {

        PTR(q31_t) __restrict dst = (PTR(q31_t)) pDst->pData;
        PTR(q31_t) __restrict src = (PTR(q31_t)) pSrc->pData;
        int32_t backstride = - (int32_t)(pSrc->size - 1);
        __builtin_assume(width > 0);
        __builtin_assume(height > 0);
        PRAGMA_NO_UNROLL
            for (c = 0; c < width; c++)
            {
                for (r = 0; r < height; r++)
                {
                    *dst++ = *src;
                    src += width;
                }
                src += backstride;
            }
    }else{

        int32_t backstride = - (int32_t)(pSrc->size - 2);
        PTR(q31_t) __restrict dst = (PTR(q31_t)) pDst->pData;
        PTR(q31_t) __restrict dst2 = (PTR(q31_t)) pDst->pData+height;
        PTR(q31_t) __restrict src = (PTR(q31_t)) pSrc->pData;

        __builtin_assume(width > 0);
        __builtin_assume(height > 0);
        PRAGMA_NO_UNROLL
            for (c = 0; c < (width>>1); c++)
            {
                PRAGMA_UNROLL_2
                    for (r = 0; r < (height>>1); r++)
                    {
                        q31_t tmp;
                        q31_t m00=*src++;
                        q31_t m01=*src++;
                        src += (width-2);
                        q31_t m10=*src++;
                        q31_t m11=*src++;
                        src += (width-2);
                        tmp=m01;
                        m01=m10;
                        m10=tmp;
                        *dst++=m00;
                        *dst++=m01;
                        *dst2++=m10;
                        *dst2++=m11;
                    }
                    src += backstride;
                    dst+=height;
                    dst2+=height;
            }
    }
    return DSP_ERR_OK;
}
#else
static _Inline dsp_status risc_dsp_mat_trans_q31(const matrix_q31_t *pSrc, matrix_q31_t *pDst)
{
    int32_t    c,r;
    q31_t*__restrict dst = pDst->pData;
    q31_t*__restrict src = pSrc->pData;
    int32_t width = pSrc->colCnt;
    int32_t height = pSrc->rowCnt;
    int32_t backstride = - (int32_t)(pSrc->size - 1);

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrc->rowCnt != pDst->colCnt) || (pSrc->colCnt != pDst->rowCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    pDst->rowCnt = pSrc->colCnt;
    pDst->colCnt = pSrc->rowCnt;

    PRAGMA_NO_UNROLL
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

#endif //_ARCVER_ARCv2HS_DSP && defined (__Xunaligned)

#define XY_MAT_TRANS_THRESHOLD 32

#define XY_BLOCK_SIZE ((XY_REGION_SIZE/4)) 

#define XYMAP_INPUT0 (0)
#define XYMAP_OUTPUT (0)

/**
 * Q31 matrix transpose.
 *
 * \param[in] pSrc - pointer to input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status dsp_mat_trans_q31(const matrix_q31_t *pSrc, matrix_q31_t *pDst)
{
#if defined(_ARC600_XY)
    int32_t nSamples = pDst->size;
    if (nSamples < XY_MAT_TRANS_THRESHOLD || nSamples > XY_BLOCK_SIZE) {
#endif
        return risc_dsp_mat_trans_q31(pSrc, pDst);
#if defined(_ARC600_XY) 
    } else {
        matrix_q31_t xySrc, xyDst;
        ASSERT(nSamples <= XY_BLOCK_SIZE);

        memcpy_StoX_int32(XYMAP_INPUT0, pSrc->pData, nSamples);

        dsp_mat_init_q31(&xySrc, pSrc->rowCnt, pSrc->colCnt, (q31_t *)(XYMAP_INPUT0));
        dsp_mat_init_q31(&xyDst, pDst->rowCnt, pDst->colCnt, (q31_t *)(XYMAP_OUTPUT));

        dsp_mat_trans_q31_XtoY((const matrix_q31_t *)&xySrc, &xyDst);

        pDst->rowCnt = xyDst.rowCnt;
        pDst->colCnt = xyDst.colCnt;

        memcpy_YtoS_int32(pDst->pData, XYMAP_OUTPUT, nSamples);

        return DSP_ERR_OK;
    }
#endif
}

#endif //defined _ARCv2EM_XY_AGU_MEDIUM 

#endif //!defined(EXCLUDE_DSP_MAT_TRANS_Q31)
