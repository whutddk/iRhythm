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
#if defined(_ARC_AS210_mac32)

// Produce instance with: MAC32x32; pSt - X; 

#pragma Code("dsplib_dsp_iir_lattice_init_q31_cfXstX")

/**
 *  initialization for Q31 IIR lattice filter function
 *
 * \param inst - pointer to instance structure
 * \param nStages - number of filter stages. This number is considered as filter order
 * \param offs_Kcoeffs - XY memory offset to the reflection coefficient array. Coefficients are located in reversed order. {k[N], k[N-1],…, k[1] }. Vector length: nStages
 * \param offs_Vcoeffs - XY memory offset to the ladder coefficient vector. Coefficients are located in reversed order. {v[N], v[N-1],…, v[1], v[0] }. Vector length: nStages + 1
 * \param offs_states - XY memory offset to states vector buffer. Vector length: nStages + 1
 * \result dsp_status
 */

dsp_status dsp_iir_lattice_init_q31_cfXstX(iir_lattice_q31_xy_t *inst, uint16_t nStages, uint32_t offs_Kcoeffs, uint32_t offs_Vcoeffs, uint32_t offs_states)   
{
    inst->order = nStages;
    inst->xyptr_states = offs_states;
    inst->xyptr_pkcoeffs = offs_Kcoeffs;
    inst->xyptr_pvcoeffs = offs_Vcoeffs;

    _MEM_FILL_X_Q31(offs_states, 0, nStages*2);

    return DSP_ERR_OK;
};
// Produce instance with: MAC32x32; pSt - Y; 

#pragma Code("dsplib_dsp_iir_lattice_init_q31_cfYstY")

/**
 *  initialization for Q31 IIR lattice filter function
 *
 * \param inst - pointer to instance structure
 * \param nStages - number of filter stages. This number is considered as filter order
 * \param offs_Kcoeffs - XY memory offset to the reflection coefficient array. Coefficients are located in reversed order. {k[N], k[N-1],…, k[1] }. Vector length: nStages
 * \param offs_Vcoeffs - XY memory offset to the ladder coefficient vector. Coefficients are located in reversed order. {v[N], v[N-1],…, v[1], v[0] }. Vector length: nStages + 1
 * \param offs_states - XY memory offset to states vector buffer. Vector length: nStages + 1
 * \result dsp_status
 */

dsp_status dsp_iir_lattice_init_q31_cfYstY(iir_lattice_q31_xy_t *inst, uint16_t nStages, uint32_t offs_Kcoeffs, uint32_t offs_Vcoeffs, uint32_t offs_states)   
{
    inst->order = nStages;
    inst->xyptr_states = offs_states;
    inst->xyptr_pkcoeffs = offs_Kcoeffs;
    inst->xyptr_pvcoeffs = offs_Vcoeffs;

    _MEM_FILL_Y_Q31(offs_states, 0, nStages*2);

    return DSP_ERR_OK;
};


// Produce instance with: MAC32x32; pInOut - X; 

#pragma Code("dsplib_dsp_iir_lattice_q31_X_cfYstY_MAC32x32")

/**
 *  Q31 IIR lattice filter processing function
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_iir_lattice_q31_X_cfYstY_MAC32x32(const iir_lattice_q31_xy_t *inst, uint32_t offs_inout, uint32_t nSamples)  
{
    int32_t nTaps = (int32_t)  inst->order;
    uint32_t xyptr_stlast = inst->xyptr_states+ nTaps;
    uint32_t xyptr_st0 = inst->xyptr_states;
    int32_t i,j;


    _set_ax0(offs_inout);
    _set_mx00(1);
    _set_ay0(inst->xyptr_pkcoeffs);
    _set_my00((AM_MOD(nTaps)|1));
    _set_ay1(inst->xyptr_pvcoeffs);
    _set_my10((AM_MOD(nTaps+1)|1));
    _set_ay2(xyptr_st0);
    _set_my20((AM_MOD(nTaps)|1));
    _set_ay3(xyptr_st0 + 1);
    _set_my30(1);
    _set_my31((AM_OFFS(1-nTaps)));

    for (i = 0; i < (int32_t)nSamples; i++)
    {
        q31_t fN = _x0_nu();
        q31_t out = 0;

        for (j = 0; j < nTaps ; j++)
        {
            q31_t gN;
            q31_t st = _y3_u0();

            fN  = dspfx_sub_q31(fN,dspfx_mpy_rnd_q31(_y0_nu(), st));
            gN  = dspfx_add_q31(dspfx_mpy_rnd_q31(_y0_u0(),fN), st);
            out = dspfx_add_q31(out, dspfx_mpy_rnd_q31(_y1_u0(), gN));

            _set_y2_u0( gN);
        }
        _set_ay3(xyptr_stlast);
        _set_x0_u0( dspfx_add_q31(out, dspfx_mpy_rnd_q31(_y1_u0(),fN)));
        _set_y3_u1( fN);
    }
};
// Produce instance with: MAC32x32; pInOut - Y; 

#pragma Code("dsplib_dsp_iir_lattice_q31_Y_cfXstX_MAC32x32")

/**
 *  Q31 IIR lattice filter processing function
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void dsp_iir_lattice_q31_Y_cfXstX_MAC32x32(const iir_lattice_q31_xy_t *inst, uint32_t offs_inout, uint32_t nSamples)  
{
    int32_t nTaps = (int32_t)  inst->order;
    uint32_t xyptr_stlast = inst->xyptr_states+ nTaps;
    uint32_t xyptr_st0 = inst->xyptr_states;
    int32_t i,j;


    _set_ay0(offs_inout);
    _set_my00(1);
    _set_ax0(inst->xyptr_pkcoeffs);
    _set_mx00((AM_MOD(nTaps)|1));
    _set_ax1(inst->xyptr_pvcoeffs);
    _set_mx10((AM_MOD(nTaps+1)|1));
    _set_ax2(xyptr_st0);
    _set_mx20((AM_MOD(nTaps)|1));
    _set_ax3(xyptr_st0 + 1);
    _set_mx30(1);
    _set_mx31((AM_OFFS(1-nTaps)));

    for (i = 0; i < (int32_t)nSamples; i++)
    {
        q31_t fN = _y0_nu();
        q31_t out = 0;

        for (j = 0; j < nTaps ; j++)
        {
            q31_t gN;
            q31_t st = _x3_u0();

            fN  = dspfx_sub_q31(fN,dspfx_mpy_rnd_q31(_x0_nu(), st));
            gN  = dspfx_add_q31(dspfx_mpy_rnd_q31(_x0_u0(),fN), st);
            out = dspfx_add_q31(out, dspfx_mpy_rnd_q31(_x1_u0(), gN));

            _set_x2_u0( gN);
        }
        _set_ax3(xyptr_stlast);
        _set_y0_u0( dspfx_add_q31(out, dspfx_mpy_rnd_q31(_x1_u0(),fN)));
        _set_x3_u1( fN);
    }
};

#endif
