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
#define EXCLUDE_DSP_MAT_MULT_Q15
#endif //defined(EXCLUDE_DSP_MAT_INIT_Q15)

#if !defined(EXCLUDE_DSP_MAT_MULT_Q15)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac16)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsplib_helper.h"

#pragma Code("dsplib_dsp_mat_mult_q15")

#if defined _ARCVER_ARCv2HS_DSP && defined (__Xunaligned) && defined __FXAPI__ && defined __GUARD_ACCUM__

dsp_status _DSPLIB_XYCC dsp_mat_mult_q15(const matrix_q15_t * pSrcA,  const matrix_q15_t * pSrcB,  matrix_q15_t * pDst)
{
    int32_t     h = 0, k = 0, p = 0;

    int32_t A_col_cnt = pSrcA->colCnt;
    int32_t B_col_cnt = pSrcB->colCnt;
    int32_t A_row_cnt = pSrcA->rowCnt;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    uint32_t B_row_cnt = pSrcB->rowCnt;	// WARNING unused?
    if (  (A_col_cnt != B_row_cnt) || (A_row_cnt != pDst->rowCnt) || (B_col_cnt != pDst->colCnt)  )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif


    q15_t* A=pSrcA->pData;
    q15_t* B=pSrcB->pData;
    q15_t* C=pDst->pData;
    /* C = A * B */
    __builtin_assume(A_row_cnt > 0);
    __builtin_assume(A_col_cnt > 0);

    if (B_col_cnt & 1) {
        PRAGMA_NO_UNROLL
            for (h=0; h < A_row_cnt; h++)
            {
                accum40_t acc = {};

                PRAGMA_NO_UNROLL
                    for (p=0; p< (A_col_cnt); p++)
                    {
                        acc = fx_a40_mac_q15(acc, *A++,*B); B += B_col_cnt;
                    }
                    *C++ = fx_q15_cast_rnd_a40(acc);
                    A -= A_col_cnt;  B += 1 - A_col_cnt * B_col_cnt;

                    PRAGMA_NO_UNROLL
                        for (k = 0; k < (B_col_cnt >> 1); k ++)
                        {
                            v2accum40_t vacc = {};
#pragma unroll 4
                            for (p=0; p < (A_col_cnt); p++)
                            {
                                vacc = fx_v2a40_mac_v2q15(vacc, fx_replic_v2q15(*A++), fx_v2q15_cast_q31((*(q31_t*)B)));	B += B_col_cnt;
                            }
                            *(v2q15_t *)C = fx_v2q15_cast_rnd_v2a40(vacc); C += 2;
                            A -= A_col_cnt;  B += 2 - A_col_cnt * B_col_cnt;
                        }
                        A += A_col_cnt; // compiler can unify OF with "A -= A_col_cnt;" 
                        B -= B_col_cnt; // compiler can unify OF with "B += B_col_cnt;"
            }
    } else {

        B_col_cnt >>= 1;

        __builtin_assume(A_col_cnt > 0);
        __builtin_assume(B_col_cnt > 0);
        PRAGMA_NO_UNROLL
            for (h=0; h < A_row_cnt; h++)
            {
                PRAGMA_NO_UNROLL
                    for (k = 0; k < B_col_cnt; k ++)
                    {
                        v2accum40_t vacc = {};

#pragma unroll 4
                        for (p=0; p < A_col_cnt; p++)
                        {
                            vacc = fx_v2a40_mac_v2q15(vacc, fx_replic_v2q15(*A), fx_v2q15_cast_q31((*(q31_t*)B)));
                            A += 1;
                            B += 2*B_col_cnt;
                        }
                        *(v2q15_t *)C = fx_v2q15_cast_rnd_v2a40(vacc); C += 2;
                        A -= A_col_cnt;  B += 2 - A_col_cnt * 2*B_col_cnt;
                    }
                    A += A_col_cnt; // compiler can unify OF with "A -= A_col_cnt;"
                    B -= 2*B_col_cnt; // compiler can unify OF with "B += B_col_cnt;"
            }
            //}
    }
    return DSP_ERR_OK;
}
#else

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__ && defined __GUARD_ACCUM__

#if defined _XY_HAND_OPT

dsp_status _DSPLIB_XYCC dsp_mat_mult_q15(const matrix_q15_t * pSrcA,  const matrix_q15_t * pSrcB,  matrix_q15_t * pDst)
{
    int32_t     h = 0, k = 0, p = 0;

    int32_t A_col_cnt = pSrcA->colCnt;
    int32_t B_col_cnt = pSrcB->colCnt;
    int32_t A_row_cnt = pSrcA->rowCnt;

#ifdef DSP_CHECK_MAT_DIMENSIONS
    uint32_t B_row_cnt = pSrcB->rowCnt;	// WARNING unused?
    if (  (A_col_cnt != B_row_cnt) || (A_row_cnt != pDst->rowCnt) || (B_col_cnt != pDst->colCnt)  )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif
	_DSPLIB_store_regs(3, 11, 4);

	_set_agu_ap0(pSrcA->pData); // Set AP registers for A
	_set_agu_ap1(pSrcB->pData); // Set AP registers for B
	_set_agu_ap2(pDst->pData); // Set AP registers for C

	_set_agu_mod0(0, AGU_DT_q15, AGU_AD_INC_q15(1));
	_set_agu_mod1(0, AGU_DT_v2q15_replic_q15, AGU_AD_INC_q15(1));
	_set_agu_mod2(0, AGU_DT_q15, AGU_AD_DEC_q15_REG(2));
	_set_agu_mod3(0, AGU_DT_q15, AGU_AD_INC_q15_REG(2));

    _set_agu_mod4(1, AGU_DT_q15, AGU_AD_INC_q15_REG(3));
    _set_agu_mod5(1, AGU_DT_v2q15, AGU_AD_INC_q15_REG(3));
    _set_agu_mod6(1, AGU_DT_q15, AGU_AD_INC_q15_REG(0));
    _set_agu_mod7(1, AGU_DT_q15, AGU_AD_INC_q15_REG(1));
	_set_agu_mod8(1, AGU_DT_q15, AGU_AD_DEC_q15_REG(3));
	
	_set_agu_os0(1 - A_col_cnt * B_col_cnt);
	_set_agu_os1(2 - A_col_cnt * B_col_cnt);
    _set_agu_os2(A_col_cnt);
    _set_agu_os3(B_col_cnt);

	_set_agu_mod9(2, AGU_DT_q15, AGU_AD_INC_q15(1));
	_set_agu_mod10(2, AGU_DT_v2q15, AGU_AD_INC_q15(2));


    /* C = A * B */
    __builtin_assume(A_row_cnt > 0);
    __builtin_assume(A_col_cnt > 0);

    if (B_col_cnt & 1) {
        for (h=0; h < A_row_cnt; h++)
        {
			accum40_t acc = {};

			if (A_col_cnt & 1)
				acc = fx_a40_mac_q15(acc, _agu_u0(), _agu_u4()) ;//acc = fx_a40_mac_q15(acc, *A++,*B); B += B_col_cnt;
			for (p=0; p< (A_col_cnt>>1); p++)
			{
				acc = fx_a40_mac_q15(acc, _agu_u0(), _agu_u4()) ;//acc = fx_a40_mac_q15(acc, *A++,*B); B += B_col_cnt;
				acc = fx_a40_mac_q15(acc, _agu_u0(), _agu_u4()) ;//acc = fx_a40_mac_q15(acc, *A++,*B); B += B_col_cnt;
			}
			_st_agu_u9(fx_q15_cast_rnd_a40(acc)); // *C++ = fx_q15_cast_rnd_a40(acc);
			_add(_agu_u2(), _agu_u6()); // A -= A_col_cnt;  B += 1 - A_col_cnt * B_col_cnt;

            for (k = 0; k < (B_col_cnt >> 1); k ++)
            {
                v2accum40_t vacc = {};

                if (A_col_cnt & 1)
                    vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u1()), fx_v2q15_cast_q31(_agu_u5()));	// Was fx_c2q15_replic_q15(*A++) ;B += B_col_cnt;
                for (p=0; p < (A_col_cnt>>1); p++)
                {
                    vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u1()), fx_v2q15_cast_q31(_agu_u5()));	// Was fx_c2q15_replic_q15(*A++) ;B += B_col_cnt;
                    vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u1()), fx_v2q15_cast_q31(_agu_u5()));	// Was fx_c2q15_replic_q15(*A++) ;B += B_col_cnt;
                }
                _st_agu_u10(fx_q31_cast_v2q15(fx_v2q15_cast_rnd_v2a40(vacc))); // *(v2q15_t *)C = fx_v2q15_cast_rnd_v2a40(vacc); C += 2;
    			_add(_agu_u2(), _agu_u7());  //  A -= A_col_cnt;  B += 2 - A_col_cnt * B_col_cnt;
            }
    		_add(_agu_u3(), _agu_u8()); //  A += A_col_cnt; // compiler can unify OF with "A -= A_col_cnt;"   B -= B_col_cnt; // compiler can unify OF with "B += B_col_cnt;"
        }
    } else {

		if (A_col_cnt & 1)
		{
			for (h=0; h < A_row_cnt; h++)
			{
				for (k = 0; k < (B_col_cnt >> 1); k ++)
				{
					v2accum40_t vacc = {};

					vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u1()), fx_v2q15_cast_q31(_agu_u5()));	// Was fx_c2q15_replic_q15(*A++) ;B += B_col_cnt;
					for (p=0; p < (A_col_cnt>>1); p++)
					{
						vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u1()), fx_v2q15_cast_q31(_agu_u5()));	// Was fx_c2q15_replic_q15(*A++) ;B += B_col_cnt;
						vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u1()), fx_v2q15_cast_q31(_agu_u5()));	// Was fx_c2q15_replic_q15(*A++) ;B += B_col_cnt;
					}
					_st_agu_u10(fx_q31_cast_v2q15(fx_v2q15_cast_rnd_v2a40(vacc))); // *(v2q15_t *)C = fx_v2q15_cast_rnd_v2a40(vacc); C += 2;
					_add(_agu_u2(), _agu_u7());  //  A -= A_col_cnt;  B += 2 - A_col_cnt * B_col_cnt;
				}
				_add(_agu_u3(), _agu_u8()); //  A += A_col_cnt; // compiler can unify OF with "A -= A_col_cnt;"   B -= B_col_cnt; // compiler can unify OF with "B += B_col_cnt;"
			}
		} else
		{
			A_col_cnt >>= 1;
			B_col_cnt >>= 1;

			__builtin_assume(A_col_cnt > 0);
			__builtin_assume(B_col_cnt > 0);
			for (h=0; h < A_row_cnt; h++)
			{
				for (k = 0; k < B_col_cnt; k ++)
				{
					v2accum40_t vacc = {};

					for (p=0; p < A_col_cnt; p++)
					{
						vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u1()), fx_v2q15_cast_q31(_agu_u5()));	// Was fx_c2q15_replic_q15(*A++) ;B += B_col_cnt;
						vacc = fx_v2a40_mac_v2q15(vacc, fx_v2q15_cast_q31(_agu_u1()), fx_v2q15_cast_q31(_agu_u5()));	// Was fx_c2q15_replic_q15(*A++) ;B += B_col_cnt;
					}
					_st_agu_u10(fx_q31_cast_v2q15(fx_v2q15_cast_rnd_v2a40(vacc))); // *(v2q15_t *)C = fx_v2q15_cast_rnd_v2a40(vacc); C += 2;
					_add(_agu_u2(), _agu_u7());  //  A -= A_col_cnt;  B += 2 - A_col_cnt * B_col_cnt;
				}
				_add(_agu_u3(), _agu_u8()); //  A += A_col_cnt; // compiler can unify OF with "A -= A_col_cnt;"   B -= B_col_cnt; // compiler can unify OF with "B += B_col_cnt;"
			}
		}
    }
	_DSPLIB_restore_regs();
    return DSP_ERR_OK;
}

#else

dsp_status _DSPLIB_XYCC dsp_mat_mult_q15(const matrix_q15_t * pSrcA,  const matrix_q15_t * pSrcB,  matrix_q15_t * pDst)
{
    uint32_t     h = 0, k = 0, p = 0;
    dspfx_accum32()
    PTR(q15_t) __restrict A;
    PTR(q15_t) __restrict B;
    PTR(q15_t) __restrict C;
    uint32_t B_col_cnt = pSrcB->colCnt;
    uint32_t A_col_cnt = pSrcA->colCnt;
#ifdef DSP_CHECK_MAT_DIMENSIONS
    if (  (pSrcA->colCnt != pSrcB->rowCnt) || (pSrcA->rowCnt != pDst->rowCnt)
        || (pSrcB->colCnt != pDst->colCnt)
        )
    { return DSP_ERR_MAT_DIMENSIONS; }
#endif

    A = (PTR(q15_t)) pSrcA->pData;
    B = (PTR(q15_t)) pSrcB->pData;
    C = (PTR(q15_t)) pDst->pData;
    /* C = A * B */

    __builtin_assume(B_col_cnt > 0);
    __builtin_assume(A_col_cnt > 0);
    __builtin_assume(pDst->rowCnt > 0);

    for (h=0; h < pDst->rowCnt; h++)
    {
        for (k=0; k < B_col_cnt; k++)
        {
            dspfx_reset_a32();
            for (p=0; p < A_col_cnt; p++)
            {
                dspfx_a32mac_q15(*A++,*B, acc);
                B += B_col_cnt;
            }
            *C++ = dspfx_q15_cast_a32(acc);
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


static _Inline dsp_status risc_dsp_mat_mult_q15(const matrix_q15_t * pSrcA,const matrix_q15_t * pSrcB, matrix_q15_t * pDst)
{
    uint32_t     h = 0, k = 0, p = 0;
    dspfx_accum32()
    q15_t        * __restrict A, * __restrict B, * __restrict C, *tmpA,*tmpB;
    uint32_t B_col_cnt = pSrcB->colCnt;
    uint32_t A_col_cnt = pSrcA->colCnt;
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

    INIT_MACMODE();
    PRAGMA_NO_UNROLL
    for (h=0; h<pDst->rowCnt; h++)
    {
    PRAGMA_NO_UNROLL
        for (k=0; k<pDst->colCnt; k++)
        {
            dspfx_reset_a32();
            tmpB = B++;
            tmpA = A;
            for (p=0; p< A_col_cnt; p++)
            {
                dspfx_a32mac_q15(*tmpA++,*tmpB, acc);
                tmpB += B_col_cnt;
            }
            *C++ = dspfx_q15_cast_a32(acc);
        }
        A += A_col_cnt;
        B -= B_col_cnt;
    }

    return DSP_ERR_OK;
}

#define XY_MAT_MULT_THRESHOLD 32

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define XY_BLOCK_SIZE ((XY_REGION_SIZE/2)/2) // half of region

#define XYMAP_INPUT0 (0)
#define XYMAP_INPUT1 ((XY_REGION_SIZE/2)/2)
#define XYMAP_OUTPUT (0)

/**
 * Q15 matrix multiplication.
 *
 * \param[in] pSrcA - pointer to 1-st input matrix structure
 * \param[in] pSrcB - pointer to 2-nd input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_mat_mult_q15(const matrix_q15_t *pSrcA, const matrix_q15_t *pSrcB, matrix_q15_t *pDst)
{
#if defined(_ARC_AS210_mac16)
    int32_t nSamples = MAX(pSrcA->size, pSrcB->size);
    if (nSamples < XY_MAT_MULT_THRESHOLD || nSamples > XY_BLOCK_SIZE) {
#endif
        return risc_dsp_mat_mult_q15(pSrcA, pSrcB, pDst);
#if defined(_ARC_AS210_mac16) 
    } else {
        matrix_q15_t xySrcA, xySrcB, xyDst;
        ASSERT(nSamples <= XY_BLOCK_SIZE);

        memcpy((void *)((q15_t *)(_get_xbase()) + XYMAP_INPUT0), pSrcA->pData, pSrcA->size*2);
        memcpy((void *)((q15_t *)(_get_xbase()) + XYMAP_INPUT1), pSrcB->pData, pSrcB->size*2);

        dsp_mat_init_q15(&xySrcA, pSrcA->rowCnt, pSrcA->colCnt, (q15_t *)(XYMAP_INPUT0));
        dsp_mat_init_q15(&xySrcB, pSrcB->rowCnt, pSrcB->colCnt, (q15_t *)(XYMAP_INPUT1));
        dsp_mat_init_q15(&xyDst, pDst->rowCnt, pDst->colCnt, (q15_t *)(XYMAP_OUTPUT));

        dsp_mat_mult_q15_XXtoY_MAC16x16((const matrix_q15_t *)&xySrcA, (const matrix_q15_t *)&xySrcB, &xyDst);

        memcpy((void *)pDst->pData, (void *)((q15_t *)(_get_ybase()) + XYMAP_OUTPUT), pDst->size*2);

        return DSP_ERR_OK;
    }
#endif
}

dsp_status dsp_mat_mult_q15_ext(  const matrix_q15_t * pSrcA,  const matrix_q15_t * pSrcB,  matrix_q15_t * pDst,  q15_t * pState)
{
    dsp_mat_mult_q15(pSrcA, pSrcB, pDst);
    return DSP_ERR_OK;
}

#endif
#endif //_ARCVER_ARCv2HS_DSP
#endif //!defined(EXCLUDE_DSP_MAT_MULT_Q15)
