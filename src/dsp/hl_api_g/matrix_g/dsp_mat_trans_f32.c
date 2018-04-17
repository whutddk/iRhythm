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

#if !defined(EXCLUDE_DSP_MAT_TRANS_F32)

#include "dsplib_primitives.h" 

#pragma Code("dsplib_dsp_mat_trans_f32")
/**
 * Floating-point matrix transpose.
 *
 * \param[in] pSrc - pointer to input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */
#if defined _ARCv2EM_XY_AGU_MEDIUM

#if defined _XY_HAND_OPT

dsp_status _DSPLIB_XYCC dsp_mat_trans_f32(const matrix_f32_t *pSrc, matrix_f32_t *pDst)
{
    uint32_t c,r;
    uint32_t width = pSrc->colCnt;
    uint32_t height = pSrc->rowCnt-1;
    int32_t backstride = - (int32_t)(pSrc->size - width - 1);

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrc->rowCnt != pDst->colCnt) || (pSrc->colCnt != pDst->rowCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

	_DSPLIB_store_regs(2, 3, 2);

    _set_agu_ap0(pSrc->pData); // Set AP registers for SrcA
    _set_agu_ap1(pDst->pData); // Set AP registers for SrcB

	_set_agu_os0(backstride);
    _set_agu_os1(width);
	_set_agu_mod1(0, AGU_DT_q31, AGU_AD_INC_q31_REG(0));
    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31_REG(1));
    _set_agu_mod2(1, AGU_DT_q31, AGU_AD_INC_q31(1));

    for (c = 0; c < width; c++)
    {
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

dsp_status _DSPLIB_XYCC dsp_mat_trans_f32(const matrix_f32_t *pSrc, matrix_f32_t *pDst)
{
    int32_t c,r;
    PTR(f32_t) dst = (PTR(f32_t))pDst->pData;
    PTR(f32_t) src = (PTR(f32_t))pSrc->pData;
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

#else

#if defined _ARCVER_ARCv2HS_DSP && defined (__Xunaligned)

dsp_status dsp_mat_trans_f32(const matrix_f32_t *pSrc, matrix_f32_t *pDst)
{
    uint32_t    c,r;
    f32_t* dst = pDst->pData;
    f32_t* src = pSrc->pData;
    uint32_t width = pSrc->colCnt;
    uint32_t height = pSrc->rowCnt;


#ifdef DSP_CHECK_MAT_DIMENSIONS
    if ( (pSrc->rowCnt != pDst->colCnt) || (pSrc->colCnt != pDst->rowCnt) )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    pDst->rowCnt = pSrc->colCnt;
    pDst->colCnt = pSrc->rowCnt;
    if (( width & 1) || (height & 1))
    {
            int32_t backstride = - (int32_t)(pSrc->size - 1);
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
    }else{
        PTR(f32_t) __restrict dst2 = (PTR(f32_t)) pDst->pData+height;
            int32_t backstride = - (int32_t)(pSrc->size - 2);
        PRAGMA_NO_UNROLL
            for (c = 0; c<(width>>1); c++)
            {
                for (r = 0; r<(height>>1); r++)
                {
                    f32_t tmp;
                    f32_t m00=*src++;
                    f32_t m01=*src++;
                    src += (width-2);
                    f32_t m10=*src++;
                    f32_t m11=*src++;
                    src += (width-2);
                    tmp=m01;
                    m01=m10;
                    m10=tmp;
                    *dst++=m00;
                    *dst++=m01;
                    *dst2++=m10;
                    *dst2++=m11;
                }
                src  += backstride;
                dst  += height;
                dst2 += height;
            }
    }

        return DSP_ERR_OK;
}
#else
dsp_status dsp_mat_trans_f32(const matrix_f32_t *pSrc, matrix_f32_t *pDst)
{
    uint32_t    c,r;
    f32_t* dst = pDst->pData;
    f32_t* src = pSrc->pData;
    uint32_t width = pSrc->colCnt;
    uint32_t height = pSrc->rowCnt;
    int32_t backstride = - (int32_t)(pSrc->size - 1);

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
#endif //_ARCVER_ARCv2HS_DSP && defined (__Xunaligned)

#endif //_ARCv2EM_XY_AGU_MEDIUM

#endif //!defined(EXCLUDE_DSP_MAT_TRANS_F32)
