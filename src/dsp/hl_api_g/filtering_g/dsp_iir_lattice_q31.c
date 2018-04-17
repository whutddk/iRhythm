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

#if !defined(EXCLUDE_DSP_IIR_LATTICE_Q31)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif

#define XY_MAP_INOUT 0
#define XY_MAP_STATES (0)
#define XY_MAP_PKCOEFF (XY_REGION_SIZE/sizeof(q31_t)/8)
#define XY_MAP_PVCOEFF (XY_MAP_PKCOEFF + XY_REGION_SIZE/sizeof(q31_t)/8)

#pragma Code("dsplib_dsp_iir_lattice_q31")
/**
 * initialization for Q31 IIR lattice filter function
 *
 * \param inst - pointer to instance structure
 * \param nStages - number of filter stages. This number is considered as filter order
 * \param pkCoeffs - pointer to the reflection coefficient array. Coefficients are located in reversed order. {k[N], k[N-1],�, k[1] }. Vector length: nStages
 * \param pvCoeffs - pointer to the ladder coefficient vector. Coefficients are located in reversed order. {v[N], v[N-1],�, v[1], v[0] }. Vector length: nStages + 1
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_iir_lattice_getsize_q31()" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_iir_lattice_init_q31(iir_lattice_q31_t *inst, uint16_t nStages, const PTR(q31_t) pkCoeffs, const PTR(q31_t) pvCoeffs, PTR(q31_t) pState)
{
    inst->pkCoeffs = pkCoeffs;
    inst->pvCoeffs = pvCoeffs;
    inst->pData = pState;
    inst->order = nStages;
    _MEM_FILL_Q31(pState,0,nStages+1);

#if defined(_ARC_AS210_mac32)
    inst->iir_lattice_xy.order = nStages;
    inst->iir_lattice_xy.xyptr_states   = XY_MAP_STATES;
    inst->iir_lattice_xy.xyptr_pkcoeffs = XY_MAP_PKCOEFF;
    inst->iir_lattice_xy.xyptr_pvcoeffs = XY_MAP_PVCOEFF;
#endif


    return DSP_ERR_OK;
};
#if defined(_ARCv2EM_XY_AGU_MEDIUM)

#if defined _XY_HAND_OPT

static _Inline void risc_dsp_iir_lattice_q31(const iir_lattice_q31_t *inst, const q31_t * __restrict pSrc, q31_t * __restrict pDst, uint32_t nSamples)
{
    const q31_t* __restrict pvCoef  = inst->pvCoeffs;
    const q31_t* __restrict pkCoef  = inst->pkCoeffs;
    int32_t nTaps = (int32_t)  inst->order;
    q31_t* __restrict pState = inst->pData + 1;

    uint32_t i;
    int32_t j;

    _DSPLIB_store_regs(5, 12, 2);

    _set_agu_ap0((uint32_t)pSrc);
    _set_agu_ap1((uint32_t)pDst);
    _set_agu_ap2((uint32_t)pkCoef);
    _set_agu_ap3((uint32_t)pvCoef);

    _set_agu_ap4((uint32_t)pState);

    _set_agu_os0(nTaps-1);
    _set_agu_os1(nTaps);

    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod1(1, AGU_DT_q31, AGU_AD_INC_q31(1));

    _set_agu_mod2(2, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod3(3, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod4(2, AGU_DT_q31, AGU_AD_DEC_q31_REG(0));
    _set_agu_mod5(3, AGU_DT_q31, AGU_AD_DEC_q31_REG(1));
    _set_agu_mod6(2, AGU_DT_q31, AGU_AD_INC_q31(0));

    _set_agu_mod7(4, AGU_DT_q31, AGU_AD_INC_q31(0));
    _set_agu_mod8(4, AGU_DT_q31, AGU_AD_DEC_q31(1));
    _set_agu_mod9(4, AGU_DT_q31, AGU_AD_INC_q31(2));
    _set_agu_mod10(4, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod11(4, AGU_DT_q31, AGU_AD_DEC_q31_REG(0));

    PRAGMA_NO_UNROLL
    for (i = 0; i<nSamples; i++)
    {
        q31_t fN_1;
        q31_t gN;
        q31_t fN;
        q31_t out = 0;

        fN =_agu_u0();
        for (j = 0; j < nTaps - 1 ; j++)
        {
            fN_1 = dspfx_sub_q31(fN, dspfx_mpy_rnd_q31(_agu_u6(), _agu_u7()));
            gN = dspfx_add_q31(dspfx_mpy_rnd_q31(_agu_u2(),fN_1), _agu_u8());
            out = dspfx_add_q31(out, dspfx_mpy_rnd_q31(_agu_u3(), gN));
            _st_agu_u9(gN);
            fN = fN_1;
        }
        fN_1 = dspfx_sub_q31(fN, dspfx_mpy_rnd_q31(_agu_u6(), _agu_u7()));
        gN = dspfx_add_q31(dspfx_mpy_rnd_q31(_agu_u4(),fN_1), _agu_u8());
        out = dspfx_add_q31(out, dspfx_mpy_rnd_q31(_agu_u3(), gN));

        _st_agu_u10(gN);
        _st_agu_u1(dspfx_add_q31(out, dspfx_mpy_rnd_q31(_agu_u5(),fN_1)));
        _st_agu_u11(fN_1);
    }

    _DSPLIB_restore_regs();
};

#else

static _Inline void risc_dsp_iir_lattice_q31(const iir_lattice_q31_t *inst, const PTR(q31_t) __restrict pSrc, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
    const PTR(q31_t) __restrict pvCoef  = (const PTR(q31_t)) inst->pvCoeffs;
    const PTR(q31_t) __restrict pkCoef  = (const PTR(q31_t)) inst->pkCoeffs;
    int32_t nTaps = (int32_t)  inst->order;
    PTR(q31_t) __restrict pState = (PTR(q31_t)) inst->pData + 1;

    uint32_t i;
    int32_t j;

    PRAGMA_NO_UNROLL
    for (i = 0; i<nSamples; i++)
    {
        q31_t fN = *pSrc++;
        q31_t out = 0;
        PTR(q31_t) pst0 = pState;
        PTR(q31_t) pst1 = pState - 1;
        const PTR(q31_t) pkC = pkCoef;
        const PTR(q31_t) pvC = pvCoef;
        for (j = 0; j < nTaps ; j++)
        {
            q31_t gN_1 = *pst0++;
            q31_t kC = *pkC++;
            q31_t fN_1 = dspfx_sub_q31(fN, dspfx_mpy_rnd_q31(kC, gN_1));
            q31_t gN = dspfx_add_q31(dspfx_mpy_rnd_q31(kC,fN_1), gN_1);
            out = dspfx_add_q31(out, dspfx_mpy_rnd_q31(*pvC++, gN));
            *pst1++ = gN;
            fN = fN_1;
        }
        *pDst++ = (dspfx_add_q31(out, dspfx_mpy_rnd_q31(*pvC,fN)));
        pState[nTaps - 1] = fN;
    }
};

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
static _Inline void risc_dsp_iir_lattice_q31(const iir_lattice_q31_t *inst, const q31_t * __restrict pSrc, q31_t * __restrict pDst, uint32_t nSamples)
{
    const q31_t* __restrict pvCoef  = inst->pvCoeffs;
    const q31_t* __restrict pkCoef  = inst->pkCoeffs;
    int32_t nTaps = (int32_t)  inst->order;
    q31_t* __restrict pState = inst->pData + 1;

    uint32_t i;
    int32_t j;

    PRAGMA_NO_UNROLL
    for (i = 0; i<nSamples; i++)
    {
        q31_t fN = *pSrc++;
        q31_t out = 0;
        q31_t* pst0 = pState;
        q31_t* pst1 = pState - 1;
        const q31_t* pkC = pkCoef;
        const q31_t* pvC = pvCoef;
        for (j = 0; j < nTaps ; j++)
        {
            q31_t gN_1 = *pst0++;
            q31_t kC = *pkC++;
            q31_t fN_1 = dspfx_sub_q31(fN, dspfx_mpy_rnd_q31(kC, gN_1));
            q31_t gN = dspfx_add_q31(dspfx_mpy_rnd_q31(kC,fN_1), gN_1);
            out = dspfx_add_q31(out, dspfx_mpy_rnd_q31(*pvC++, gN));
            *pst1++ = gN;
            fN = fN_1;
        }
        *pDst++ = (dspfx_add_q31(out, dspfx_mpy_rnd_q31(*pvC,fN)));
        pState[nTaps - 1] = fN;
    }
};
#endif //_ARCv2EM_XY_AGU_MEDIUM

/**
 * Q31 IIR lattice filter processing function
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_iir_lattice_q31(const iir_lattice_q31_t *inst, const PTR(q31_t) __restrict pSrc, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
#if defined(_ARC_AS210_mac32)

#define LATTICE_Q31_BLOCK_SIZE (XY_REGION_SIZE/sizeof(q31_t)/2)

   int32_t Samples = (int32_t) nSamples;

    memcpy_StoY_int32(XY_MAP_PKCOEFF, inst->pkCoeffs, inst->order); 
    memcpy_StoY_int32(XY_MAP_PVCOEFF, inst->pvCoeffs, inst->order+1); 
    memcpy_StoY_int32(XY_MAP_STATES, inst->pData, inst->order+1); 

    while (Samples > 0) {
        int32_t block_size = (Samples > (int32_t)LATTICE_Q31_BLOCK_SIZE) ? (int32_t)LATTICE_Q31_BLOCK_SIZE:Samples;
        
        memcpy_StoX_int32(XY_MAP_INOUT, (int32_t *)(void*) pSrc, block_size); 
        dsp_iir_lattice_q31_X_cfYstY_MAC32x32(&(inst->iir_lattice_xy), XY_MAP_INOUT ,block_size);
        memcpy_XtoS_int32(pDst, XY_MAP_INOUT, block_size);

        Samples -= block_size;
        pSrc += block_size;
        pDst += block_size;
    };

    memcpy_YtoS_int32(inst->pData, XY_MAP_STATES, inst->order+1); 
#else
    risc_dsp_iir_lattice_q31(inst, pSrc, pDst, nSamples);
#endif
}


void dsp_iir_lattice_delete_q31(iir_lattice_q31_t *inst)
{
    // TODO: Place code here
};

void _DSPLIB_XYCC dsp_iir_lattice_init_q31_ext(  iir_lattice_q31_t *inst,  uint16_t nStages,  PTR(q31_t) pkCoeffs,  PTR(q31_t) pvCoeffs,  PTR(q31_t) pState,  uint32_t nSamples)
{
    dsp_iir_lattice_init_q31(inst, nStages, pkCoeffs, pvCoeffs, pState) ;
    return;
}
#pragma Code()

#endif //!defined(EXCLUDE_DSP_IIR_LATTICE_Q31)
