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

#if !defined(EXCLUDE_DSP_MAT_MULT_F32)

#include "dsplib_primitives.h" 

#pragma Code("dsplib_dsp_mat_mult_f32")

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__ && defined __Xfpu_mac

#if defined _XY_HAND_OPT

dsp_status _DSPLIB_XYCC dsp_mat_mult_f32(const matrix_f32_t* pSrcA, const matrix_f32_t* pSrcB, matrix_f32_t* pDst)
{
    uint32_t h = 0, k = 0, p = 0;
    f32_t accf32;
    int32_t A_col = pSrcA->colCnt;
    int32_t D_col = pDst->colCnt;
    int32_t D_row = pDst->rowCnt;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->colCnt != pSrcB->rowCnt) || (pSrcA->rowCnt != pDst->rowCnt) || (pSrcB->colCnt != pDst->colCnt)  )
    { 
        return DSP_ERR_MAT_DIMENSIONS; 
    }
#endif

    if (A_col <= 0) return DSP_ERR_MAT_DIMENSIONS;

	_DSPLIB_store_regs(3, 7, 3);

    _set_agu_ap0(pSrcA->pData); // Set AP registers for SrcA
    _set_agu_ap1(pSrcB->pData); // Set AP registers for SrcB
    _set_agu_ap2(pDst->pData); // Set AP registers for Dst

	_set_agu_os0(pSrcA->colCnt);
	_set_agu_os1(pSrcB->colCnt);
	_set_agu_os2((pSrcB->colCnt * pSrcB->rowCnt)-1);

    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31_REG(0));
    _set_agu_mod2(0, AGU_DT_q31, AGU_AD_INC_q31_REG(0));
    _set_agu_mod3(1, AGU_DT_q31, AGU_AD_INC_q31_REG(1));
    _set_agu_mod4(1, AGU_DT_q31, AGU_AD_DEC_q31_REG(1));
    _set_agu_mod5(1, AGU_DT_q31, AGU_AD_DEC_q31_REG(2));
    _set_agu_mod6(2, AGU_DT_q31, AGU_AD_INC_q31(1));

    /* C = A * B */
    for (h=0; h < D_row; h++) 
	{
        for (k=0; k < D_col; k++) 
		{
            accf32 = 0;
            for (p=0; p < A_col; p++) 
			{
                accf32 += _as_float(_agu_u0()) * _as_float(_agu_u3());
            }
            _st_agu_u6(_as_uint32(accf32));
            _add(_agu_u1(), _agu_u5());
        }
        _add(_agu_u2(), _agu_u4());
    }

	_DSPLIB_restore_regs();
    return DSP_ERR_OK;
}

#else

dsp_status dsp_mat_mult_f32(const matrix_f32_t* pSrcA, const matrix_f32_t* pSrcB, matrix_f32_t* pDst)
{
    uint32_t     h = 0, k = 0, p = 0;
    f32_t        result = 0;
    PTR(f32_t)   A;
    PTR(f32_t)   B;
    PTR(f32_t)   C;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->colCnt != pSrcB->rowCnt) || (pSrcA->rowCnt != pDst->rowCnt) || (pSrcB->colCnt != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    if (pSrcA->colCnt <= 0) return DSP_ERR_MAT_DIMENSIONS;

    A = (PTR(f32_t))pSrcA->pData;
    B = (PTR(f32_t))pSrcB->pData;
    C = (PTR(f32_t))pDst->pData;
    /* C = A * B */
    for (h=0; h < pDst->rowCnt; h++)
    {
        for (k=0; k < pDst->colCnt; k++)
        {
            result = 0;
            for (p=0; p < pSrcA->colCnt; p++)
            {
                MAC_F32(result, *A++, *B);
                B += pSrcB->colCnt;
            }
            *C++ = result;
            B -= pSrcB->colCnt * pSrcA->colCnt - 1;
            A -= pSrcA->colCnt;
        }
        A += pSrcA->colCnt;
        B -= pSrcB->colCnt;
    }
    return DSP_ERR_OK;
}

#endif

#else // defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

/**
 * Floating-point matrix multiplication.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status dsp_mat_mult_f32(const matrix_f32_t* pSrcA, const matrix_f32_t* pSrcB, matrix_f32_t* pDst)
{
    uint32_t     h = 0, k = 0, p = 0;
    f32_t        result = 0;
    f32_t        *A, *B, *C, *tmpA, *tmpB;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->colCnt != pSrcB->rowCnt) || (pSrcA->rowCnt != pDst->rowCnt) || (pSrcB->colCnt != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    A = pSrcA->pData;
    B = pSrcB->pData;
    C = pDst->pData;
    /* C = A * B */
    PRAGMA_NO_UNROLL
    for (h=0; h!=pDst->rowCnt; h++)
    {
    PRAGMA_NO_UNROLL
        for (k=0; k!=pDst->colCnt; k++)
        {
            result = 0;
            tmpB = B++;
            tmpA = A;
            for (p=0; p!=pSrcA->colCnt; p++)
            {
                MAC_F32(result,*tmpA++,*tmpB);
                tmpB += pSrcB->colCnt;
            }
            *C++ = result;
        }
        A += pSrcA->colCnt;
        B -= pSrcB->colCnt;
    }

    return DSP_ERR_OK;
}

#endif //defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__
#endif //!defined(EXCLUDE_DSP_MAT_MULT_F32)
