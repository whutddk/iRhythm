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
#define EXCLUDE_DSP_MAT_MULT_Q31
#endif //defined(EXCLUDE_DSP_MAT_INIT_Q31)

#if !defined(EXCLUDE_DSP_MAT_MULT_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"

#pragma Code("dsplib_dsp_mat_mult_q31")

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

#if defined _XY_HAND_OPT

dsp_status _DSPLIB_XYCC dsp_mat_mult_q31(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst)
{
    uint32_t h = 0, k = 0, p = 0;
    accum72_t acc72;
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
    __builtin_assume(A_col > 0);
    __builtin_assume(D_col > 0);
    __builtin_assume(D_row > 0);
    for (h=0; h < D_row; h++) {
    	if (A_col & 1) {
			for (k=0; k < D_col; k++) {
				acc72 = fx_a72_cast_q63(0);
				acc72=fx_a72_mac_q31(acc72, _agu_u0(), _agu_u3());
				for (p=0; p < (A_col >> 1); p++) {
					acc72=fx_a72_mac_q31(acc72, _agu_u0(), _agu_u3());
					acc72=fx_a72_mac_q31(acc72, _agu_u0(), _agu_u3());
				}
				_st_agu_u6(fx_q31_cast_rnd_a72(acc72));
				_add(_agu_u1(), _agu_u5());
			}
    	} else {
			for (k=0; k < D_col; k++) {
				acc72 = fx_a72_cast_q63(0);
				for (p=0; p < (A_col >> 1); p++) {
					acc72=fx_a72_mac_q31(acc72, _agu_u0(), _agu_u3());
					acc72=fx_a72_mac_q31(acc72, _agu_u0(), _agu_u3());
				}
				_st_agu_u6(fx_q31_cast_rnd_a72(acc72));
				_add(_agu_u1(), _agu_u5());
			}
    	}
        _add(_agu_u2(), _agu_u4());
    }

	_DSPLIB_restore_regs();
    return DSP_ERR_OK;
}

#else

dsp_status _DSPLIB_XYCC dsp_mat_mult_q31(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst)
{
    uint32_t     h = 0, k = 0, p = 0;
    dspfx_accum64()
    PTR(q31_t) __restrict A;
    PTR(q31_t) __restrict B;
    PTR(q31_t) __restrict C;
    uint32_t B_col_cnt = pSrcB->colCnt;
    uint32_t A_col_cnt = pSrcA->colCnt;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->colCnt != pSrcB->rowCnt) || (pSrcA->rowCnt != pDst->rowCnt)
        || (pSrcB->colCnt != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    A = (PTR(q31_t)) pSrcA->pData;
    B = (PTR(q31_t)) pSrcB->pData;
    C = (PTR(q31_t)) pDst->pData;
    /* C = A * B */
    __builtin_assume(A_col_cnt > 0);
    __builtin_assume(B_col_cnt > 0);
    __builtin_assume(pDst->rowCnt > 0);
    PRAGMA_NO_UNROLL
    for (h=0; h < pDst->rowCnt; h++)
    {
        for (k=0; k < B_col_cnt; k++)
        {
            dspfx_reset_a64();
            PRAGMA_UNROLL_2            
            for (p=0; p < A_col_cnt; p++)
            {
                dspfx_a64mac_q31(*A++,*B, acc);
                B += B_col_cnt;
            }
            *C++ = dspfx_q31_cast_a64(acc);
            B -= B_col_cnt * A_col_cnt - 1;
            A -= A_col_cnt;
        }
        A += A_col_cnt;
        B -= B_col_cnt;
    }
    return DSP_ERR_OK;
}

#endif

#else

static _Inline dsp_status risc_dsp_mat_mult_q31(const matrix_q31_t * pSrcA, const matrix_q31_t * pSrcB, matrix_q31_t * pDst)
{
    uint32_t     h = 0, k = 0, p = 0;
    dspfx_accum64()
    q31_t        * __restrict A, * __restrict B, * __restrict C, *tmpA,*tmpB;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->colCnt != pSrcB->rowCnt) || (pSrcA->rowCnt != pDst->rowCnt)
        || (pSrcB->colCnt != pDst->colCnt)
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
            dspfx_reset_a64();
            tmpB = B++;
            tmpA = A;
            for (p=0; p!=pSrcA->colCnt; p++)
            {
                dspfx_a64mac_q31(*tmpA++,*tmpB, acc);
                tmpB += pSrcB->colCnt;
            }
            *C++ = dspfx_q31_cast_a64(acc);
        }
        A += pSrcA->colCnt;
        B -= pSrcB->colCnt;
    }

    return DSP_ERR_OK;
}

#define XY_MAT_MULT_THRESHOLD 32

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define XY_BLOCK_SIZE ((XY_REGION_SIZE/4)/2) // half of region

#define XYMAP_INPUT0 (0)
#define XYMAP_INPUT1 ((XY_REGION_SIZE/4)/2)
#define XYMAP_OUTPUT (0)

/**
 * Q31 matrix multiplication.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_mat_mult_q31(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst)
{
#if defined(_ARC_AS210_mac32)
    int32_t nSamples = MAX(pSrcA->size, pSrcB->size);
    if (nSamples < XY_MAT_MULT_THRESHOLD || nSamples > XY_BLOCK_SIZE) {
#endif
        return risc_dsp_mat_mult_q31(pSrcA, pSrcB, pDst);
#if defined(_ARC_AS210_mac32) 
    } else {
        matrix_q31_t xySrcA,  xySrcB, xyDst;
        ASSERT(nSamples <= XY_BLOCK_SIZE);

        memcpy_StoX_int32(XYMAP_INPUT0, pSrcA->pData, pSrcA->size);
        memcpy_StoX_int32(XYMAP_INPUT1, pSrcB->pData, pSrcB->size);

        dsp_mat_init_q31(&xySrcA, pSrcA->rowCnt, pSrcA->colCnt, (q31_t *)(XYMAP_INPUT0));
        dsp_mat_init_q31(&xySrcB, pSrcB->rowCnt, pSrcB->colCnt, (q31_t *)(XYMAP_INPUT1));
        dsp_mat_init_q31(&xyDst, pDst->rowCnt, pDst->colCnt, (q31_t *)(XYMAP_OUTPUT));

        dsp_mat_mult_q31_XXtoY_MAC32x32((const matrix_q31_t *)&xySrcA, (const matrix_q31_t *)&xySrcB, &xyDst);

        memcpy_YtoS_int32(pDst->pData, XYMAP_OUTPUT, pDst->size);

        return DSP_ERR_OK;
    }
#endif
}

#endif

#endif //!defined(EXCLUDE_DSP_MAT_MULT_Q31)
