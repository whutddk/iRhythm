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

#if !defined(EXCLUDE_DSP_IIR_LATTICE_F32)

#include "dsplib_types.h"
#include "dsplib_primitives.h"

#pragma Code("dsplib_dsp_iir_lattice_f32")
/**
 * initialization for f32 IIR lattice filter function
 *
 * \param inst - pointer to instance structure
 * \param nStages - number of filter stages. This number is considered as filter order
 * \param pkCoeffs - pointer to the reflection coefficient array. Coefficients are located in reversed order. {k[N], k[N-1],�, k[1] }. Vector length: nStages
 * \param pvCoeffs - pointer to the ladder coefficient vector. Coefficients are located in reversed order. {v[N], v[N-1],�, v[1], v[0] }. Vector length: nStages + 1
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_iir_lattice_getsize_f32" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_iir_lattice_init_f32(iir_lattice_f32_t *inst, uint16_t nStages, const PTR(f32_t) pkCoeffs, const PTR(f32_t) pvCoeffs, PTR(f32_t) pState)
{
    inst->pkCoeffs = pkCoeffs;
    inst->pvCoeffs = pvCoeffs;
    inst->pData = pState;
    inst->order = nStages;
    _MEM_FILL_F32(pState,0.0f,nStages+1);
    return DSP_ERR_OK;
};

/**
 * f32 IIR lattice filter processing function
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */
#if defined _ARCv2EM_XY

#if defined _XY_HAND_OPT && defined _ARCv2EM_XY_AGU_MEDIUM

void _DSPLIB_XYCC dsp_iir_lattice_f32(const iir_lattice_f32_t *inst, const __xy f32_t * __restrict pSrc, __xy f32_t * __restrict pDst, uint32_t nSamples)
{
    const f32_t* __restrict pvCoef  = inst->pvCoeffs;
    const f32_t* __restrict pkCoef  = inst->pkCoeffs;
    int32_t nTaps = (int32_t)  inst->order;
    f32_t* __restrict pState = inst->pData + 1;

    uint32_t i;
    int32_t j;

    _DSPLIB_store_regs(5,12,2);

    _set_agu_ap0((uint32_t)pSrc);
    _set_agu_ap1((uint32_t)pDst);
    _set_agu_ap2((uint32_t)pkCoef);
    _set_agu_ap3((uint32_t)pvCoef);

    _set_agu_ap4((uint32_t)pState);

    _set_agu_os0(nTaps-1);
    _set_agu_os1(nTaps);

    _set_agu_mod0(0, AGU_DT_ui32, AGU_AD_INC_ui32(1));
    _set_agu_mod1(1, AGU_DT_ui32, AGU_AD_INC_ui32(1));

    _set_agu_mod2(2, AGU_DT_ui32, AGU_AD_INC_ui32(1));
    _set_agu_mod3(3, AGU_DT_ui32, AGU_AD_INC_ui32(1));
    _set_agu_mod4(2, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(0));
    _set_agu_mod5(3, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(1));
    _set_agu_mod6(2, AGU_DT_ui32, AGU_AD_INC_ui32(0));

    _set_agu_mod7(4, AGU_DT_ui32, AGU_AD_INC_ui32(0));
    _set_agu_mod8(4, AGU_DT_ui32, AGU_AD_DEC_ui32(1));
    _set_agu_mod9(4, AGU_DT_ui32, AGU_AD_INC_ui32(2));
    _set_agu_mod10(4, AGU_DT_ui32, AGU_AD_INC_ui32(1));
    _set_agu_mod11(4, AGU_DT_ui32, AGU_AD_DEC_ui32_REG(0));

    PRAGMA_NO_UNROLL
    for (i = 0; i<nSamples; i++)
    {
        f32_t fN_1;
        f32_t gN;
        f32_t fN;
        f32_t out = 0;

        fN =_as_float(_agu_u0());
        for (j = 0; j < nTaps - 1 ; j++)
        {
            fN_1 = SUB_F32(fN, MPY_F32(_as_float(_agu_u6()), _as_float(_agu_u7())));
            gN = ADD_F32(MPY_F32(_as_float(_agu_u2()),fN_1), _as_float(_agu_u8()));
            out = ADD_F32(out, MPY_F32(_as_float(_agu_u3()), gN));
            _st_agu_u9(_as_uint32(gN));
            fN = fN_1;
        }
        fN_1 = SUB_F32(fN, MPY_F32(_as_float(_agu_u6()), _as_float(_agu_u7())));
        gN = ADD_F32(MPY_F32(_as_float(_agu_u4()),fN_1), _as_float(_agu_u8()));
        out = ADD_F32(out, MPY_F32(_as_float(_agu_u3()), gN));

        _st_agu_u10(_as_uint32(gN));
        _st_agu_u1(_as_uint32(ADD_F32(out, MPY_F32(_as_float(_agu_u5()),fN_1))));
        _st_agu_u11(_as_uint32(fN_1));
    }
    _DSPLIB_restore_regs();
};

#else

void _DSPLIB_XYCC dsp_iir_lattice_f32(const iir_lattice_f32_t *inst, const PTR(f32_t) __restrict pSrc, PTR(f32_t) __restrict pDst, uint32_t nSamples)
{
    const PTR(f32_t) pvCoef = (const PTR(f32_t)) inst->pvCoeffs;
    const PTR(f32_t) pkCoef = (const PTR(f32_t)) inst->pkCoeffs;
    int32_t nTaps = (int32_t) inst->order;
    PTR(f32_t) pState = (PTR(f32_t)) inst->pData + 1;

    uint32_t i;
    int32_t j;

    PRAGMA_NO_UNROLL
    for (i = 0; i<nSamples; i++)
    {
        f32_t fN = pSrc[i];
        f32_t out = 0;
        for (j = 0; j < nTaps ; j++)
        {
            f32_t gN_1 = pState[j];
            f32_t fN_1 = fN - pkCoef[j] * gN_1;
            f32_t gN = pkCoef[j] * fN_1 + gN_1;
            out += pvCoef[j] * gN;
            pState[j-1] = gN;
            fN = fN_1;
        }
        pDst[i] = out + pvCoef[nTaps] * fN;
        pState[nTaps - 1] = fN;
    }
};

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
void dsp_iir_lattice_f32(const iir_lattice_f32_t *inst, const f32_t * __restrict pSrc, f32_t * __restrict pDst, uint32_t nSamples)
{
    const f32_t* pvCoef  = inst->pvCoeffs;
    const f32_t* pkCoef  = inst->pkCoeffs;
    int32_t nTaps = (int32_t)  inst->order;
    f32_t* pState = inst->pData + 1;

    uint32_t i;
    int32_t j;

    PRAGMA_NO_UNROLL
    for (i = 0; i<nSamples; i++)
    {
        f32_t fN = pSrc[i];
        f32_t out = 0;
        for (j = 0; j < nTaps ; j++)
        {
            f32_t gN_1 = pState[j];
            f32_t fN_1 = fN - pkCoef[j] * gN_1;
            f32_t gN = pkCoef[j] * fN_1 + gN_1;
            out += pvCoef[j] * gN;
            pState[j-1] = gN;
            fN = fN_1;
        }
        pDst[i] = out + pvCoef[nTaps] * fN;
        pState[nTaps - 1] = fN;
    }
};
#endif //_ARCv2EM_XY_AGU_MEDIUM

void dsp_iir_lattice_delete_f32(iir_lattice_f32_t *inst)
{
    // TODO: Place code here
};


void _DSPLIB_XYCC dsp_iir_lattice_init_f32_ext(  iir_lattice_f32_t *inst,  uint16_t nStages,  PTR(f32_t) pkCoeffs,  PTR(f32_t) pvCoeffs, PTR(f32_t) pState,  uint32_t nSamples)
{
    dsp_iir_lattice_init_f32(inst, nStages, pkCoeffs, pvCoeffs, pState);
    return;
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_IIR_LATTICE_F32)
