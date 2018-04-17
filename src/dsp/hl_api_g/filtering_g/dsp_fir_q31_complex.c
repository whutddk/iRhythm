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

#if !defined(EXCLUDE_DSP_FIR_Q31_COMPLEX)

#include "dsplib_primitives.h"
#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#define XY_CF_ADDR 0
#define XY_ST_ADDR 0
#endif

/*================== COMPLEX Q31 FIR Function ===============================*/
static _Inline void risc_dsp_fir_q31_complex(fir_q31_t* inst, const PTR(q31_t) __restrict  pSrc, PTR(q31_t) __restrict  pDst, uint32_t nSamples);

#define MODULE_ADDR_INC2(p, pFirst , pLast) if (p==pLast) p=pFirst; else p+=2;
#define MODULE_ADDR_DEC2(p, pFirst , pLast) if (p==pFirst) p=pLast; else p-=2;

#pragma Code("dsplib_dsp_fir_q31_complex")
/**
 * Q31 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param[in] pSrc - pointer to input vector. Vector length: nSamples
 * \param[out] pDst - pointer to output vector. Vector length: nSamples
 * \param nSamples - number of samples to be processed
 */

void _DSPLIB_XYCC dsp_fir_q31_complex(fir_q31_t* inst, const PTR(q31_t) __restrict pSrc, PTR(q31_t) __restrict pDst, uint32_t nSamples){
#if defined(_ARC_AS210_mac32)
    if ((inst->numTaps < 2)|| (inst->numTaps*2 >= (XY_REGION_SIZE/(2*sizeof(q31_t)))))
    {
        risc_dsp_fir_q31_complex(inst,  pSrc, pDst, nSamples);
    }else{
        int32_t XY_IN_ADDR = inst->numTaps*2;
        int32_t XY_OUT_ADDR = inst->numTaps*2;
        int32_t FIR_Q31_COMPLEX_BLOCK_SIZE = ((XY_REGION_SIZE/(2*sizeof(q31_t)) - inst->numTaps*2));
        int32_t Samples = (int32_t) nSamples;
        // Load filter coefficients to XY memory
        memcpy_StoX_int32(XY_CF_ADDR, (int32_t*)(void*)(inst->pCoeffs), inst->numTaps << 1);
        // fill XY memory with state
        memcpy_StoY_int32(inst->fir_q31_ll.xyptr_states, (int32_t*)(void*)inst->pData, inst->numTaps << 1);
        while (Samples > 0) {
            int32_t block_size = (Samples > FIR_Q31_COMPLEX_BLOCK_SIZE) ? FIR_Q31_COMPLEX_BLOCK_SIZE:Samples;
            // fill XY memory with input data
            memcpy_StoX_int32(XY_IN_ADDR, (int32_t*)(void*)pSrc, block_size << 1);            
            // call LL FIR filter function
            dsp_fir_q31_complex_XtoY_cfXstY_MAC32x32(&inst->fir_q31_ll, XY_IN_ADDR, XY_OUT_ADDR, block_size);
            // Store output data
            memcpy_YtoS_int32((int32_t*)(void*)pDst, XY_OUT_ADDR, block_size << 1);
            Samples -= block_size;
            pSrc += block_size*2;
            pDst += block_size*2;
        };
        // Store filter state
        memcpy_YtoS_int32((int32_t*)(void*)inst->pData, inst->fir_q31_ll.xyptr_states, inst->numTaps << 1);
    }
#else
    risc_dsp_fir_q31_complex(inst,  pSrc, pDst, nSamples);
#endif
}
#ifdef _ARCv2EM_XY_AGU_MEDIUM

#if defined _XY_HAND_OPT

static _Inline void risc_dsp_fir_q31_complex(fir_q31_t* inst, const PTR(q31_t) __restrict pSrc, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
    uint32_t i, j;
    uint32_t norm_ntaps;
    uint32_t input_mod_base, input_mod_offset;
    uint32_t numTaps2 = 2*inst->numTaps;
    uint32_t numTaps = inst->numTaps;
    uint32_t order = inst->numTaps - 1;
    uint32_t shift = inst->shift;

    _DSPLIB_store_regs(4, 11, 4);

    dspfx_accum64();

    norm_ntaps = 33 - dspfx_norm_q31(numTaps2);
    input_mod_base = ((uint32_t)(inst->pData)) & ~((uint32_t)(1<<norm_ntaps) - 1);
    input_mod_offset = (((uint32_t)(inst->pData)) - input_mod_base) >> 2;

    _set_agu_ap0((uint32_t)pSrc);
    _set_agu_ap1((uint32_t)pDst);
    _set_agu_ap2((uint32_t)(inst->pData + shift));
    _set_agu_ap3((uint32_t)(inst->pCoeffs));

    _set_agu_os0(sizeof(q31_t));
    _set_agu_os1((numTaps2*sizeof(q31_t)-1));
    _set_agu_os2(input_mod_offset);
    _set_agu_os3(order*2);    


    _set_agu_mod0(2, AGU_DT_q31, AGU_AD_WRAP_INC_RR(0, 1)); // delay line modulo increment

    _set_agu_mod3(3, AGU_DT_q31, AGU_AD_INC_q31(1)); // coefficients increment
    _set_agu_mod4(3, AGU_DT_q31, AGU_AD_DEC_q31_REG(3)); // coefficients decrement to start address
    _set_agu_mod5(3, AGU_DT_q31, AGU_AD_DEC_q31(1)); // coefficients decrement
    _set_agu_mod6(3, AGU_DT_q31, AGU_AD_INC_q31(3)); // coefficients increment by 3


    _set_agu_mod7(0, AGU_DT_q31, AGU_AD_INC_q31(1)); // input increment
    _set_agu_mod8(1, AGU_DT_q31, AGU_AD_INC_q31(1)); // output increment

    _set_agu_mod9(2, AGU_DT_q31, AGU_AD_DEC_q31(1)); // delay line decrement

    if (input_mod_offset == 0){

    PRAGMA_NO_UNROLL
        for ( i = 0; i < nSamples; i++ )
        {
            dspfx_reset_a64();
            _st_agu_u0(_agu_u7()); // move real part of input to delay line
            _st_agu_u9(_agu_u7()); // move imaginary part of input to delay line
            for ( j = 0; j < numTaps-1; j++)
            {
                dspfx_a64mac_q31(_agu_u0(), _agu_u3(), acc); // coefficients: +1
                dspfx_a64msu_q31(_agu_u0(), _agu_u3(), acc); // coefficients: +1
            }
            dspfx_a64mac_q31(_agu_u0(), _agu_u3(), acc); // coefficients: +1
            dspfx_a64msu_q31(_agu_u0(), _agu_u4(), acc); // coefficients: decrement by order*2, first imaginary coefficient

            _st_agu_u8(dspfx_q31_cast_a64(acc)); // store output real sample

            dspfx_reset_a64();
            for ( j = 0; j < numTaps-1; j++)
            {
                dspfx_a64mac_q31(_agu_u0(), _agu_u5(), acc); // coefficients: -1
                dspfx_a64mac_q31(_agu_u0(), _agu_u6(), acc); // coefficients: +3
            }

            dspfx_a64mac_q31(_agu_u0(),  _agu_u5(), acc); // coefficients: -1
            dspfx_a64mac_q31(_agu_u9(), _agu_u4(), acc); // coefficients: decrement by order*2, first real coefficient

            _st_agu_u8(dspfx_q31_cast_a64(acc)); // store output imaginary sample

        }
    } else {

        _set_agu_mod1(2, AGU_DT_q31, AGU_AD_DEC_q31_REG(2)); // delay line decrement by input_mod_offset
        _set_agu_mod2(2, AGU_DT_q31, AGU_AD_INC_q31_REG(2)); // delay line increment by input_mod_offset
        _set_agu_mod10(2, AGU_DT_q31, AGU_AD_INC_q31(1)); // delay line decrement

    PRAGMA_NO_UNROLL
        for ( i = 0; i < nSamples; i++ )
        {
            dspfx_reset_a64();
            _st_agu_u10(_agu_u7()); // move real part of input to delay line
            _st_agu_u9(_agu_u7()); // move imaginary part of input to delay line
            for ( j = 0; j < numTaps-1; j++)
            {
                dspfx_a64mac_q31(_agu_u10(), _agu_u3(), acc); // coefficients: +1
                dspfx_a64msu_q31(_agu_u1(), _agu_u3(), acc); // coefficients: +1
                _agu_u0();
                _agu_u2();
            }
            dspfx_a64mac_q31(_agu_u10(), _agu_u3(), acc); // coefficients: +1
            dspfx_a64msu_q31(_agu_u1(), _agu_u4(), acc); // coefficients: decrement by order*2, first imaginary coefficient
            _agu_u0();
            _agu_u2();

            _st_agu_u8(dspfx_q31_cast_a64(acc)); // store output real sample

            dspfx_reset_a64();
            for ( j = 0; j < numTaps-1; j++)
            {
                dspfx_a64mac_q31(_agu_u10(), _agu_u5(), acc); // coefficients: -1
                dspfx_a64mac_q31(_agu_u1(), _agu_u6(), acc); // coefficients: +3
                _agu_u0();
                _agu_u2();
            }

            dspfx_a64mac_q31(_agu_u10(),  _agu_u5(), acc); // coefficients: -1
            dspfx_a64mac_q31(_agu_u9(), _agu_u4(), acc); // coefficients: decrement by order*2, first real coefficient

            _st_agu_u8(dspfx_q31_cast_a64(acc)); // store output imaginary sample

        }
    }
    shift = ((uint32_t)_agu_ap2() - (uint32_t)(inst->pData)) >> 2;
    inst->shift = (uint16_t)(shift);

    _DSPLIB_restore_regs();
}

#else

static _Inline void risc_dsp_fir_q31_complex(fir_q31_t* inst, const PTR(q31_t) __restrict pSrc, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
    int32_t i, j;
    uint32_t norm_ntaps;
    uint32_t input_mod_base, input_mod_offset;
    uint32_t nTaps = inst->numTaps;
    PTR(q31_t) pDataR = (PTR(q31_t)) inst->pData + inst->shift*2;
    PTR(q31_t) pDataI = (PTR(q31_t)) inst->pData + inst->shift*2 + 1;
    const PTR(q31_t) pCoeff = (const PTR(q31_t)) inst->pCoeffs;
    dspfx_accum64();
    norm_ntaps = 33 - dspfx_norm_q31(nTaps*2);
    input_mod_base = ((uint32_t)(inst->pData)) & ~((uint32_t)(1<<norm_ntaps) - 1);
    input_mod_offset = (((uint32_t)(inst->pData)) - input_mod_base) >> 2;

	for ( i = 0; i < nSamples; i++ )
	{
		dspfx_reset_a64();
		*pDataR = *pSrc++;
		*pDataI = *pSrc++;
		for ( j = 0; j < nTaps; j++ )
		{
			dspfx_a64mac_q31(*pCoeff++, *pDataR, acc);
			dspfx_a64msu_q31(*pCoeff++, *pDataI, acc);
			_AGU_CIRC_DEC_SAFE(pDataR, 2, nTaps*2, input_mod_offset);
			_AGU_CIRC_DEC_SAFE(pDataI, 2, nTaps*2, input_mod_offset);
		}
		*pDst++ = dspfx_q31_cast_a64(acc); pCoeff -= nTaps*2;
		dspfx_reset_a64();
		for ( j = 0; j < nTaps; j++ )
		{
			dspfx_a64mac_q31(*pCoeff++, *pDataI, acc);
			dspfx_a64mac_q31(*pCoeff++, *pDataR, acc);
			_AGU_CIRC_DEC_SAFE(pDataR, 2, nTaps*2, input_mod_offset);
			_AGU_CIRC_DEC_SAFE(pDataI, 2, nTaps*2, input_mod_offset);
		}
		*pDst++ = dspfx_q31_cast_a64(acc); pCoeff -= nTaps*2;
		_AGU_CIRC_INC_SAFE(pDataR, 2, nTaps*2, input_mod_offset);
		_AGU_CIRC_INC_SAFE(pDataI, 2, nTaps*2, input_mod_offset);
	}

    inst->shift = (pDataR - inst->pData)/2;
}

#endif

#else //_ARCv2EM_XY_AGU_MEDIUM
static _Inline void risc_dsp_fir_q31_complex(fir_q31_t* inst, const PTR(q31_t) __restrict pSrc, PTR(q31_t) __restrict pDst, uint32_t nSamples)
{
    uint32_t i, j;
    uint32_t numTaps = 2*inst->numTaps;
    const q31_t* pCoeff;
    q31_t* pDataNew = inst->pData + 2*inst->shift;
    q31_t* pBufLast = inst->pData + numTaps - 2;
    q31_t* pBufFirst = inst->pData;
    q31_t* p; 
    dspfx_accum64()

    nSamples = nSamples << 1;
    
    PRAGMA_NO_UNROLL
    for ( i = 0; i < nSamples; i+=2 )
    {
        *pDataNew = *pSrc++;
        *(pDataNew+1) = *pSrc++;

        p = pDataNew;       
        pCoeff = inst->pCoeffs;
        dspfx_reset_a64();
        for ( j = 0; j < numTaps; j+=2 )
        {
            dspfx_a64mac_q31(*p, *pCoeff, acc);
            dspfx_a64msu_q31(*(p+1), *(pCoeff+1), acc);
            MODULE_ADDR_DEC2(p, pBufFirst, pBufLast);
            pCoeff+=2;
        }
        *pDst++ = dspfx_q31_cast_a64(acc);

        p = pDataNew;
        pCoeff = inst->pCoeffs;        
        dspfx_reset_a64();
        for ( j = 0; j < numTaps; j+=2 )
        {
            dspfx_a64mac_q31(*(p+1), *pCoeff, acc);
            dspfx_a64mac_q31(*p,  *(pCoeff+1), acc);
            MODULE_ADDR_DEC2(p, pBufFirst, pBufLast);
            pCoeff+=2;
        }
        *pDst++ = dspfx_q31_cast_a64(acc);

        MODULE_ADDR_INC2(pDataNew, pBufFirst, pBufLast);
    }
    inst->shift = (uint16_t)(pDataNew-pBufFirst)/2;
}
#endif //_ARCv2EM_XY_AGU_MEDIUM
/*================== COMPLEX Q31 FIR INIT Function===========================*/

/**
 * initialization for Q31 complex FIR filtering
 *
 * \param inst - pointer to instance structure
 * \param nTaps - filter length
 * \param pCoeffs - pointer to coefficients vector. Vector length: nTaps
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_fir_getsize_q31_complex" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_fir_init_q31_complex(fir_q31_t* inst, uint16_t nTaps, const PTR(q31_t) pCoeffs, PTR(q31_t) pState)
{
    if ( (inst==0) || ( pCoeffs==0 ) || ( pState==0 ) ) return DSP_ERR_NULL;
    inst->numTaps = nTaps;
    inst->pData = pState;
    inst->pCoeffs = (q31_t *)pCoeffs;
    inst->shift = 0;
    _MEM_FILL_Q31(pState,0, 2*nTaps);
#if defined(_ARC_AS210_mac32)
    // Init low-level FIR filter
    dsp_fir_init_q31_complex_cfXstY_MAC32x32(&inst->fir_q31_ll, nTaps, XY_CF_ADDR, XY_ST_ADDR);
#endif
    return DSP_ERR_OK;
}


void dsp_fir_delete_q31_complex(fir_q31_t* inst)
{
 // TODO: Place code here
};
#pragma Code()

#endif //!defined(EXCLUDE_DSP_FIR_Q31_COMPLEX)
