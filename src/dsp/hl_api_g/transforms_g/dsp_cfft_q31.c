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

#if !defined(EXCLUDE_DSP_CFFT_Q31)
#pragma Code("dsplib_dsp_cfft_q31")

#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsplib.h"
#include "dsp_transform_tables.h"

/**
 * Initializes instance structure of complex fft transform
 * for q31 data type. 
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_cfft_getsize_q31()" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_cfft_init_q31(cfft_q31_t* handle, uint32_t length,  PTR(uint32_t) pState)
{
    uint32_t order = 30-dspfx_norm_q31(length);
    if ((length > DSP_MAX_TRANSFORM_SIZE))
        return DSP_ERR_LENGTH_TOO_BIG;
    if ((length < 4))
        return DSP_ERR_LENGTH_TOO_SMALL;
    if (length != (1 << order))
        return DSP_ERR_LENGTH_NOT_POWER2;
    if (dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;
    if (dsp_cfft_q31_twd3_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;
    if (dsp_cfft_bitrev_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;

    handle->length = length;
#ifdef _ARCv2EM_XY_11D
    handle->ptwd  = (q31_t*)((uint8_t*)pState);
    handle->ptwd3 = (q31_t*)((uint8_t*)pState + dsp_cfft_getsize_q31(length)/2);
    memcpy(handle->ptwd, dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], dsp_cfft_getsize_q31(length)/2);
    memcpy(handle->ptwd3, dsp_cfft_q31_twd3_tab[order - DSP_MIN_TRANSFORM_ORDER], dsp_cfft_getsize_q31(length)/2);
#else
    handle->ptwd = (q31_t*)dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
    handle->ptwd3 = (q31_t*)dsp_cfft_q31_twd3_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif    
    handle->tab_bit_rev = (int16_t*)dsp_cfft_bitrev_tab[order - DSP_MIN_TRANSFORM_ORDER];
    handle->shiftBits = 0;
    handle->scaling = DSP_TRANSFORM_SCALING_UNSCALED;
    return DSP_ERR_OK;
};

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

void risc_dsp_cfft_sradix_dit_q31(const cfft_q31_t* handle, q31_t* input, q31_t* output)
{
    int32_t i, k, l;
    int32_t nLength = handle->length;
    int32_t nLength2 = nLength/2;
#if defined   __Xagu_large
    int32_t N8_cplx;
#else
    int32_t N4_cplx;
    q31_t re1, im1, ss1, ss3;    
#endif
    int32_t nOrder = 30 - dspfx_norm_q31(handle->length);
    int32_t unitShiftPrev, unitShift, shiftBits = handle->shiftBits;
   
    q31_t cc1_neg, ss1_neg, cc3_neg, ss3_neg;
    q31_t re0, im0, re2, im2, re3, im3;
    q31_t tre2, tre3;
    q31_t tim2, tim3;

    int32_t ix, id, ido;
	dspfx_accum64();
	_DSPLIB_store_regs(4, 11, 4);

	_set_agu_ap0(input);
	_set_agu_ap1(input + 1);
	_set_agu_ap2(output);
	_set_agu_os0(nLength);

	unitShift = (shiftBits-- >	0) ? 1 : 0;  
	// Process inplace bitreverse reordering
	if (input == output)
	{
		uint32_t outaddr = (uint32_t)input;
		_set_agu_os1((((uint32_t)input - ((uint32_t)input & ~((nLength << 3)-1))) >> 2) + 1);
		_set_agu_os2(((uint32_t)input - ((uint32_t)input & ~((nLength << 3)-1))) >> 2);
		_set_agu_mod0(0, AGU_DT_q31, AGU_AD_BITREV_INC_q31_REG(0));
		_set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31_REG(1));
		_set_agu_mod2(0, AGU_DT_q31, AGU_AD_INC_q31_REG(2));
		_set_agu_mod3(0, AGU_DT_q31, AGU_AD_INC_q31(1));
		_set_agu_mod4(0, AGU_DT_q31, AGU_AD_DEC_q31(1));
		_set_agu_mod5(2, AGU_DT_q31, AGU_AD_INC_q31(1));
		_set_agu_mod6(2, AGU_DT_q31, AGU_AD_DEC_q31(1));

		for (i = 0; i < nLength; i++)
		{
			int32_t temp_real = _agu_u3();
			int32_t temp_imag = _agu_u4();
			uint32_t revaddr = (uint32_t)_agu_ap0();
			if (revaddr > outaddr) 
			{
				_st_agu_u3(_agu_u5());
				_st_agu_u1(_agu_u6());
				_st_agu_u5(temp_real);
				_st_agu_u5(temp_imag);
			}
			else {
				_add(_agu_u3(), _agu_u5());
				_add(_agu_u1(), _agu_u5());
			}
			_agu_u0();
			_agu_u2();
			outaddr += 8;
		}
		_set_agu_ap0(input);
		_set_agu_ap1(input + 1);
		_set_agu_ap2(output);

		_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(2));
		_set_agu_mod1(0, AGU_DT_q31, AGU_AD_INC_q31(0));
		_set_agu_mod2(1, AGU_DT_q31, AGU_AD_INC_q31(2));
		_set_agu_mod3(1, AGU_DT_q31, AGU_AD_INC_q31(0));
		_set_agu_mod4(2, AGU_DT_q31, AGU_AD_INC_q31(1));

		if (unitShift) {
			for (i = 0; i < nLength2; i++)
			{
				re0 = _agu_u0();
				im0 = _agu_u2();
				_st_agu_u4( fx_add_q31(re0, _agu_u1()) >> 1 );
				_st_agu_u4( fx_add_q31(im0, _agu_u3()) >> 1 );
				_st_agu_u4( fx_sub_q31(re0, _agu_u0()) >> 1 );
				_st_agu_u4( fx_sub_q31(im0, _agu_u2()) >> 1 );
			}
		} else {
			for (i = 0; i < nLength2; i++)
			{
				re0 = _agu_u0();
				im0 = _agu_u2();
				_st_agu_u4( fx_add_q31(re0, _agu_u1()));
				_st_agu_u4( fx_add_q31(im0, _agu_u3()));
				_st_agu_u4( fx_sub_q31(re0, _agu_u0()));
				_st_agu_u4( fx_sub_q31(im0, _agu_u2()));
			}
		}

	} else {


		_set_agu_mod0(0, AGU_DT_q31, AGU_AD_BITREV_INC_q31_REG(0));
		_set_agu_mod1(0, AGU_DT_q31, AGU_AD_INC_q31(0));
		_set_agu_mod2(1, AGU_DT_q31, AGU_AD_BITREV_INC_q31_REG(0));
		_set_agu_mod3(1, AGU_DT_q31, AGU_AD_INC_q31(0));
		_set_agu_mod4(2, AGU_DT_q31, AGU_AD_INC_q31(1));

		if ( ((uint32_t)input & ((nLength << 3) - 1) ) == 0 )
		{
			if (unitShift) {
				for (i = 0; i < nLength2; i++)
				{
					re0 = _agu_u0();
					im0 = _agu_u2();
					_st_agu_u4( fx_add_q31(re0, _agu_u1()) >> 1 );
					_st_agu_u4( fx_add_q31(im0, _agu_u3()) >> 1 );
					_st_agu_u4( fx_sub_q31(re0, _agu_u0()) >> 1 );
					_st_agu_u4( fx_sub_q31(im0, _agu_u2()) >> 1 );
				}
			} else {
				for (i = 0; i < nLength2; i++)
				{
					re0 = _agu_u0();
					im0 = _agu_u2();
					_st_agu_u4( fx_add_q31(re0, _agu_u1()) );
					_st_agu_u4( fx_add_q31(im0, _agu_u3()) );
					_st_agu_u4( fx_sub_q31(re0, _agu_u0()) );
					_st_agu_u4( fx_sub_q31(im0, _agu_u2()) );
				}
			}
		} else {
			_set_agu_os1(((uint32_t)input - ((uint32_t)input & ~((nLength << 3)-1))) >> 2);
			_set_agu_mod5(0, AGU_DT_q31, AGU_AD_INC_q31_REG(1));
			_set_agu_mod6(0, AGU_DT_q31, AGU_AD_DEC_q31_REG(1));
			_set_agu_mod7(1, AGU_DT_q31, AGU_AD_INC_q31_REG(1));
			_set_agu_mod8(1, AGU_DT_q31, AGU_AD_DEC_q31_REG(1));

			if (unitShift) {
				for (i = 0; i < nLength2; i++)
				{
					re0 = _agu_u6();
					im0 = _agu_u8();
					_add(_agu_u0(), _agu_u2());
					_add(_agu_u5(), _agu_u7());
					_st_agu_u4( fx_add_q31(re0, _agu_u1()) >> 1 );
					_st_agu_u4( fx_add_q31(im0, _agu_u3()) >> 1 );
					_st_agu_u4( fx_sub_q31(re0, _agu_u6()) >> 1 );
					_st_agu_u4( fx_sub_q31(im0, _agu_u8()) >> 1 );
					_add(_agu_u0(), _agu_u2());
					_add(_agu_u5(), _agu_u7());
				}
			} else {
				for (i = 0; i < nLength2; i++)
				{
					re0 = _agu_u6();
					im0 = _agu_u8();
					_add(_agu_u0(), _agu_u2());
					_add(_agu_u5(), _agu_u7());
					_st_agu_u4( fx_add_q31(re0, _agu_u1()) );
					_st_agu_u4( fx_add_q31(im0, _agu_u3()) );
					_st_agu_u4( fx_sub_q31(re0, _agu_u6()) );
					_st_agu_u4( fx_sub_q31(im0, _agu_u8()) );
					_add(_agu_u0(), _agu_u2());
					_add(_agu_u5(), _agu_u7());
				}
			}
        }
    }
    // stage: nOrder - 2
    _set_agu_mod0(1, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod1(1, AGU_DT_q31, AGU_AD_INC_q31_REG(0));
    _set_agu_mod2(0, AGU_DT_q31, AGU_AD_INC_q31(0));
    _set_agu_mod3(0, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod4(0, AGU_DT_q31, AGU_AD_INC_q31_REG(0));
    _set_agu_mod5(3, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod6(3, AGU_DT_q31, AGU_AD_INC_q31_REG(0));
    _set_agu_mod7(2, AGU_DT_q31, AGU_AD_INC_q31(1));
    _set_agu_mod8(2, AGU_DT_q31, AGU_AD_INC_q31_REG(0));
    // baseop: split radix with trivial multiply
    ix = 0;    id = 8;    ido = 3;
    unitShift = (shiftBits-- > 0) ? 1 : 0;
    while (ix < nLength - 1) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);

		_set_agu_ap0(output + ix*2);
		_set_agu_ap1(output + ix*2 + 4);
		_set_agu_ap2(output + ix*2);
		_set_agu_ap3(output + ix*2 + 4);
		_set_agu_os0(id*2 - 4 + 1);

        //pI0 = output + ix*2;
		if (_Rarely(unitShift)) {
			for (i = 0; i<i_cnt; i++) {
				re2 = _agu_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
				im2 = _agu_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
				re3 = _agu_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
				im3 = _agu_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

				_st_agu_u5( fx_sub_q31(_agu_u2(), re2) >> 1);
				_st_agu_u7( fx_add_q31(_agu_u3(), re2) >> 1);
				_st_agu_u5( fx_sub_q31(_agu_u2(), im2) >> 1);
				_st_agu_u7( fx_add_q31(_agu_u3(), im2) >> 1);
				_st_agu_u5( fx_sub_q31(_agu_u2(), im3) >> 1);
				_st_agu_u7( fx_add_q31(_agu_u3(), im3) >> 1);
				_st_agu_u6( fx_add_q31(_agu_u2(), re3) >> 1);
				_st_agu_u8( fx_sub_q31(_agu_u4(), re3) >> 1);
			}
        } else {
			for (i = 0; i<i_cnt; i++) {
				re2 = _agu_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
				im2 = _agu_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
				re3 = _agu_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
				im3 = _agu_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

				_st_agu_u5( fx_sub_q31(_agu_u2(), re2) );
				_st_agu_u7( fx_add_q31(_agu_u3(), re2) );
				_st_agu_u5( fx_sub_q31(_agu_u2(), im2) );
				_st_agu_u7( fx_add_q31(_agu_u3(), im2) );
				_st_agu_u5( fx_sub_q31(_agu_u2(), im3) );
				_st_agu_u7( fx_add_q31(_agu_u3(), im3) );
				_st_agu_u6( fx_add_q31(_agu_u2(), re3) );
				_st_agu_u8( fx_sub_q31(_agu_u4(), re3) );
			}
		}
        ix = 2 * id - 4;
        id = 4 * id; ido +=2;
    }



#if defined __Xagu_large

    /* Scheme of data processing
     *                       a1
     * R------------------------>--------------->>--------------------
     * I-------------------------\------>------//>>-------------------
     *                            \    / \    //
     * R--------------------------->--/---\-->>-----------------------
     * I---------------------------->/----->//------------------------
     *       a0                           /
     * R------>\---->\---------->--------/----------------------------
     * I-------->=>/--\--------/-\------>-----------------------------
     *                 \      /   \    /
     * R---------------->---->----->--/-------------------------------
     * I----------------->=>/------->/--------------------------------
     *
     *       m m m m   mm m m  a  as   a  a s  ss
     *       u a u a   ua u a  d  du   d  d u  uu
     *       l c l c   lc l c  d  db   d  d b  bb
     *
     *       a2
     * C------>\-->\-------------------
     * S-------->/-->------------------
     *                 a3
     * C---------------->-->\----------
     * S----------------->/-->---------
     *
     */

    // stage: [(nOrder-2) .. 2]
    _set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod2(0, AGU_DT_q31, AGU_AD_INC_q31(0));
	_set_agu_mod3(0, AGU_DT_q31, AGU_AD_INC_q31_REG(0));
	_set_agu_mod4(0, AGU_DT_q31, AGU_AD_DEC_q31_REG(0));
	_set_agu_mod5(0, AGU_DT_q31, AGU_AD_INC_q31_REG(1));

	_set_agu_mod6(1, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod7(1, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod8(1, AGU_DT_q31, AGU_AD_INC_q31_REG(0));
	_set_agu_mod9(1, AGU_DT_q31, AGU_AD_DEC_q31_REG(0));

	_set_agu_mod10(2, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod11(3, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod12(2, AGU_DT_q31, AGU_AD_INC_q31_REG(2));
	_set_agu_mod13(3, AGU_DT_q31, AGU_AD_INC_q31_REG(2));

	_set_agu_mod14(1, AGU_DT_q31, AGU_AD_INC_q31_REG(3));

    // baseop: split-radix
    N8_cplx = 8; // N4 = N / 4
    for (k = nOrder-3; k >= 2 ; k--)
    {
        int32_t d1 = 1<<k;
        int32_t l_cnt = (N8_cplx>>2);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;

		_set_agu_os0(N8_cplx>>1);
		_set_agu_os2(d1*2 - 1);

		_set_agu_ap2(handle->ptwd);
		_set_agu_ap3(handle->ptwd3);

		if (_Rarely(unitShiftPrev || unitShift)) {
	        for (l = 0; l < l_cnt; l++) //pCos1+=d1, pSin1+=d1, pCos3+=d3, pSin3+=d3
	        {
	            int32_t ix = l;
	            int32_t id2 = N8_cplx*4;
				int32_t l_ido = ((nLength) >> (nOrder - k + 1));

	            cc1_neg = _agu_u10();
	            ss1_neg = _agu_u12();
	            cc3_neg = _agu_u11();
	            ss3_neg = _agu_u13();

	            while (ix < nLength - 1)
	            {
	                int32_t i_cnt = dspfx_max_q31(l_ido,1);

					_set_agu_os1(id2 + N8_cplx - 1);
					_set_agu_os3(id2 - 1);

					_set_agu_ap0(output + ix*2 + N8_cplx);
					_set_agu_ap1(output + ix*2);
					__builtin_assume(i_cnt >= 1);
					for (i = 0; i<i_cnt; i++)
					{
							  dspfx_a64mul_q31(_agu_u0(), cc1_neg, acc);
						re2 = dspfx_a64mac_q31(_agu_u2(), ss1_neg, acc);    // int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1 )>>31);
							  dspfx_a64mul_q31(_agu_u1(), cc1_neg, acc);
						im2 = dspfx_a64msu_q31(_agu_u3(), ss1_neg, acc);    // int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1 )>>31);

							  dspfx_a64mul_q31(_agu_u0(), cc3_neg, acc);
						re3 = dspfx_a64mac_q31(_agu_u2(), ss3_neg, acc);    // int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3 )>>31);
							  dspfx_a64mul_q31(_agu_u1(), cc3_neg, acc);
						im3 = dspfx_a64msu_q31(_agu_u4(), ss3_neg, acc);   // int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3 )>>31);

						tre2 = fx_add_q31(re2, re3) >> unitShiftPrev;
						tre3 = fx_sub_q31(re2, re3) >> unitShiftPrev;
						tim2 = fx_add_q31(im2, im3) >> unitShiftPrev;
						tim3 = fx_sub_q31(im2, im3) >> unitShiftPrev;

						_st_agu_u3(/*pI2[0] = */dspfx_add_q31(_agu_u8()/*re0*/, tre2) >> unitShift);
						_st_agu_u0(/*pI3[0] = */dspfx_add_q31(_agu_u6()/*re1*/, tim3) >> unitShift);
						_st_agu_u4(/*pI3[1] = */dspfx_sub_q31(_agu_u9()/*im1*/, tre3) >> unitShift);
						_st_agu_u4(/*pI2[1] = */dspfx_add_q31(_agu_u8()/*im0*/, tim2) >> unitShift);
						_st_agu_u1(/*pI1[1] = */dspfx_add_q31(_agu_u7()/*im1*/, tre3) >> unitShift);
						_st_agu_u4(/*pI1[0] = */dspfx_sub_q31(_agu_u9()/*re1*/, tim3) >> unitShift);
						_st_agu_u0(/*pI0[0] = */dspfx_sub_q31(_agu_u6()/*re0*/, tre2) >> unitShift);
						_st_agu_u5(/*pI0[1] = */dspfx_sub_q31(_agu_u14()/*im0*/, tim2) >> unitShift);
					}
					ix = id2 - N8_cplx + l;
					id2 = 4 * id2; l_ido >>= 2;
				}

            }

		} else {

			for (l = 0; l < l_cnt; l++) //pCos1+=d1, pSin1+=d1, pCos3+=d3, pSin3+=d3
			{
				int32_t ix = l;
				int32_t id2 = N8_cplx*4;
				int32_t l_ido = ((nLength) >> (nOrder - k + 1));

				cc1_neg = _agu_u10();
				ss1_neg = _agu_u12();
				cc3_neg = _agu_u11();
				ss3_neg = _agu_u13();

				while (ix < nLength - 1)
				{
					int32_t i_cnt = dspfx_max_q31(l_ido,1);

					_set_agu_os1(id2 + N8_cplx - 1);
					_set_agu_os3(id2 - 1);

					_set_agu_ap0(output + ix*2 + N8_cplx);
					_set_agu_ap1(output + ix*2);

					__builtin_assume(i_cnt >= 1);
					for (i = 0; i<i_cnt; i++)
					{
							  dspfx_a64mul_q31(_agu_u0(), cc1_neg, acc);
						re2 = dspfx_a64mac_q31(_agu_u2(), ss1_neg, acc);    // int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1 )>>31);
							  dspfx_a64mul_q31(_agu_u1(), cc1_neg, acc);
						im2 = dspfx_a64msu_q31(_agu_u3(), ss1_neg, acc);    // int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1 )>>31);

							  dspfx_a64mul_q31(_agu_u0(), cc3_neg, acc);
						re3 = dspfx_a64mac_q31(_agu_u2(), ss3_neg, acc);    // int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3 )>>31);
							  dspfx_a64mul_q31(_agu_u1(), cc3_neg, acc);
						im3 = dspfx_a64msu_q31(_agu_u4(), ss3_neg, acc);   // int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3 )>>31);

						tre2 = fx_add_q31(re2, re3);
						tre3 = fx_sub_q31(re2, re3);
						tim2 = fx_add_q31(im2, im3);
						tim3 = fx_sub_q31(im2, im3);

						_st_agu_u3(/*pI2[0] = */dspfx_add_q31(_agu_u8()/*re0*/, tre2));
						_st_agu_u0(/*pI3[0] = */dspfx_add_q31(_agu_u6()/*re1*/, tim3));
						_st_agu_u4(/*pI3[1] = */dspfx_sub_q31(_agu_u9()/*im1*/, tre3));
						_st_agu_u4(/*pI2[1] = */dspfx_add_q31(_agu_u8()/*im0*/, tim2));
						_st_agu_u1(/*pI1[1] = */dspfx_add_q31(_agu_u7()/*im1*/, tre3));
						_st_agu_u4(/*pI1[0] = */dspfx_sub_q31(_agu_u9()/*re1*/, tim3));
						_st_agu_u0(/*pI0[0] = */dspfx_sub_q31(_agu_u6()/*re0*/, tre2));
						_st_agu_u5(/*pI0[1] = */dspfx_sub_q31(_agu_u14()/*im0*/, tim2));
					}

					ix = id2 - N8_cplx + l;
					id2 = 4 * id2; l_ido >>= 2;
				}

			}
		}
        N8_cplx <<= 1;
    }


    int32_t delta = ((int32_t)handle->ptwd3 - (int32_t)handle->ptwd)>>2;

	_set_agu_mod11(2, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod13(2, AGU_DT_q31, AGU_AD_DEC_q31_REG(3));
	_set_agu_os2(delta - 1);

    for (k = dspfx_min_q31(1, nOrder-3); k >=0 ; k--) {
        int32_t d2 = 2 << k;
        int32_t l_cnt = (N8_cplx>>2);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;

		_set_agu_os0(N8_cplx>>1);
		_set_agu_os1(N8_cplx + 1);
		_set_agu_os3(delta - d2 + 1);

		_set_agu_ap0(output + N8_cplx);
		_set_agu_ap1(output);
		_set_agu_ap2(handle->ptwd);
		if (_Rarely(unitShiftPrev || unitShift)) {
			__builtin_assume(l_cnt >= 1);
			for (l = 0; l < l_cnt; l++)
			{
					  dspfx_a64mul_q31(_agu_u0(), _agu_u10(), acc);
				re2 = dspfx_a64mac_q31(_agu_u2(), _agu_u11(), acc);    // int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1 )>>31);
					  dspfx_a64mul_q31(_agu_u1(), _agu_u10(), acc);
				im2 = dspfx_a64msu_q31(_agu_u3(), _agu_u12(), acc);    // int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1 )>>31);

					  dspfx_a64mul_q31(_agu_u0(), _agu_u10(), acc);
				re3 = dspfx_a64mac_q31(_agu_u2(), _agu_u11(), acc);    // int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3 )>>31);
					  dspfx_a64mul_q31(_agu_u1(), _agu_u10(), acc);
				im3 = dspfx_a64msu_q31(_agu_u4(), _agu_u13(), acc);   // int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3 )>>31);

				tre2 = fx_add_q31(re2, re3)>> unitShiftPrev;
				tre3 = fx_sub_q31(re2, re3)>> unitShiftPrev;
				tim2 = fx_add_q31(im2, im3)>> unitShiftPrev;
				tim3 = fx_sub_q31(im2, im3)>> unitShiftPrev;

	            _st_agu_u3(/*pI2[0] = */dspfx_add_q31(_agu_u8()/*re0*/, tre2) >> unitShift);
	            _st_agu_u0(/*pI3[0] = */dspfx_add_q31(_agu_u6()/*re1*/, tim3) >> unitShift);
	            _st_agu_u4(/*pI3[1] = */dspfx_sub_q31(_agu_u9()/*im1*/, tre3) >> unitShift);
	            _st_agu_u4(/*pI2[1] = */dspfx_add_q31(_agu_u8()/*im0*/, tim2) >> unitShift);
	            _st_agu_u1(/*pI1[1] = */dspfx_add_q31(_agu_u7()/*im1*/, tre3) >> unitShift);
	            _st_agu_u4(/*pI1[0] = */dspfx_sub_q31(_agu_u9()/*re1*/, tim3) >> unitShift);
	            _st_agu_u0(/*pI0[0] = */dspfx_sub_q31(_agu_u6()/*re0*/, tre2) >> unitShift);
	            _st_agu_u5(/*pI0[1] = */dspfx_sub_q31(_agu_u6()/*im0*/, tim2) >> unitShift);
			}
		} else {
			__builtin_assume(l_cnt >= 1);
			for (l = 0; l < l_cnt; l++)
			{
					  dspfx_a64mul_q31(_agu_u0(), _agu_u10(), acc);
				re2 = dspfx_a64mac_q31(_agu_u2(), _agu_u11(), acc);    // int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1 )>>31);
					  dspfx_a64mul_q31(_agu_u1(), _agu_u10(), acc);
				im2 = dspfx_a64msu_q31(_agu_u3(), _agu_u12(), acc);    // int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1 )>>31);

					  dspfx_a64mul_q31(_agu_u0(), _agu_u10(), acc);
				re3 = dspfx_a64mac_q31(_agu_u2(), _agu_u11(), acc);    // int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3 )>>31);
					  dspfx_a64mul_q31(_agu_u1(), _agu_u10(), acc);
				im3 = dspfx_a64msu_q31(_agu_u4(), _agu_u13(), acc);   // int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3 )>>31);

				tre2 = fx_add_q31(re2, re3);
				tre3 = fx_sub_q31(re2, re3);
				tim2 = fx_add_q31(im2, im3);
				tim3 = fx_sub_q31(im2, im3);

	            _st_agu_u3(/*pI2[0] = */dspfx_add_q31(_agu_u8()/*re0*/, tre2));
	            _st_agu_u0(/*pI3[0] = */dspfx_add_q31(_agu_u6()/*re1*/, tim3));
	            _st_agu_u4(/*pI3[1] = */dspfx_sub_q31(_agu_u9()/*im1*/, tre3));
	            _st_agu_u4(/*pI2[1] = */dspfx_add_q31(_agu_u8()/*im0*/, tim2));
	            _st_agu_u1(/*pI1[1] = */dspfx_add_q31(_agu_u7()/*im1*/, tre3));
	            _st_agu_u4(/*pI1[0] = */dspfx_sub_q31(_agu_u9()/*re1*/, tim3));
	            _st_agu_u0(/*pI0[0] = */dspfx_sub_q31(_agu_u6()/*re0*/, tre2));
	            _st_agu_u5(/*pI0[1] = */dspfx_sub_q31(_agu_u6()/*im0*/, tim2));
			}
		}

        N8_cplx <<= 1;
    }
#else
    // stage: [(nOrder-2) .. 2]
	_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod2(0, AGU_DT_q31, AGU_AD_INC_q31_REG(0));
	_set_agu_mod3(0, AGU_DT_q31, AGU_AD_DEC_q31_REG(1));
	_set_agu_mod4(0, AGU_DT_q31, AGU_AD_INC_q31_REG(2));

	_set_agu_mod5(1, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod6(1, AGU_DT_q31, AGU_AD_INC_q31_REG(3));

	_set_agu_mod7(2, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod8(2, AGU_DT_q31, AGU_AD_INC_q31_REG(3));

    // baseop: split-radix
    N4_cplx = 4; // N4 = N / 4
    for (k = nOrder-3; k >= 2 ; k--) 
    {
        int32_t d1 = 1<<k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;

		_set_agu_ap1(handle->ptwd);
		_set_agu_ap2(handle->ptwd3);
		_set_agu_os0(N4_cplx - 1);
		_set_agu_os1(N4_cplx*3 + 1);
		_set_agu_os3(d1*2 - 1);

        for (l = 0; l < l_cnt; l++) //pCos1+=d1, pSin1+=d1, pCos3+=d3, pSin3+=d3
        {
            int32_t ix = l;
            int32_t id = N4_cplx*4;
            int32_t ido = nOrder - k + 1;

            cc1_neg = _agu_u5();
            ss1_neg = _agu_u6();
            cc3_neg = _agu_u7();
            ss3_neg = _agu_u8();
            ss1 = dspfx_neg_q31(ss1_neg); 
            ss3 = dspfx_neg_q31(ss3_neg);

            while (ix < nLength - 1) 
            {
                int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
                // pI0 = output + ix*2;

				_set_agu_os2(id*2 - N4_cplx*3 - 1);
				_set_agu_ap0(output + ix*2);

				if (_Rarely(unitShift || unitShiftPrev)) {
					for (i = 0; i<i_cnt; i++) 
					{
						re0 = _agu_u0(); //COMPLEX_RE(input, i0)
						im0 = _agu_u2(); //COMPLEX_IM(input, i0)
						re1 = _agu_u0(); //COMPLEX_RE(input, i1)
						im1 = _agu_u2(); //COMPLEX_IM(input, i1)

						      dspfx_a64mul_q31(_agu_u0(), cc1_neg, acc);
						re2 = dspfx_a64mac_q31(_agu_u1(), ss1_neg, acc); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
						      dspfx_a64mul_q31(_agu_u0(), ss1, acc);
						im2 = dspfx_a64mac_q31(_agu_u2(), cc1_neg, acc);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

						      dspfx_a64mul_q31(_agu_u0(), cc3_neg, acc);
						re3 = dspfx_a64mac_q31(_agu_u1(), ss3_neg, acc); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
						      dspfx_a64mul_q31(_agu_u0(), ss3, acc);
						im3 = dspfx_a64mac_q31(_agu_u3(), cc3_neg, acc);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

						tre2 = fx_add_q31(re2, re3) >> unitShiftPrev;
						tre3 = fx_sub_q31(re2, re3) >> unitShiftPrev;
						tim2 = fx_add_q31(im2, im3) >> unitShiftPrev;
						tim3 = fx_sub_q31(im2, im3) >> unitShiftPrev;

						_st_agu_u0( fx_sub_q31(re0, tre2) >> unitShift);
						_st_agu_u2( fx_sub_q31(im0, tim2) >> unitShift);
						_st_agu_u0( fx_sub_q31(re1, tim3) >> unitShift);
						_st_agu_u2( fx_add_q31(im1, tre3) >> unitShift);
						_st_agu_u0( fx_add_q31(re0, tre2) >> unitShift);
						_st_agu_u2( fx_add_q31(im0, tim2) >> unitShift);
						_st_agu_u0( fx_add_q31(re1, tim3) >> unitShift);
						_st_agu_u4( fx_sub_q31(im1, tre3) >> unitShift);
					}
				} else {
					for (i = 0; i<i_cnt; i++) 
					{
						re0 = _agu_u0(); //COMPLEX_RE(input, i0)
						im0 = _agu_u2(); //COMPLEX_IM(input, i0)
						re1 = _agu_u0(); //COMPLEX_RE(input, i1)
						im1 = _agu_u2(); //COMPLEX_IM(input, i1)

							  dspfx_a64mul_q31(_agu_u0(), cc1_neg, acc);
						re2 = dspfx_a64mac_q31(_agu_u1(), ss1_neg, acc); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
							  dspfx_a64mul_q31(_agu_u0(), ss1, acc);
						im2 = dspfx_a64mac_q31(_agu_u2(), cc1_neg, acc);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

							  dspfx_a64mul_q31(_agu_u0(), cc3_neg, acc);
						re3 = dspfx_a64mac_q31(_agu_u1(), ss3_neg, acc); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
							  dspfx_a64mul_q31(_agu_u0(), ss3, acc);
						im3 = dspfx_a64mac_q31(_agu_u3(), cc3_neg, acc);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

						tre2 = fx_add_q31(re2, re3) ;
						tre3 = fx_sub_q31(re2, re3) ;
						tim2 = fx_add_q31(im2, im3) ;
						tim3 = fx_sub_q31(im2, im3) ;

						_st_agu_u0( fx_sub_q31(re0, tre2) );
						_st_agu_u2( fx_sub_q31(im0, tim2) );
						_st_agu_u0( fx_sub_q31(re1, tim3) );
						_st_agu_u2( fx_add_q31(im1, tre3) );
						_st_agu_u0( fx_add_q31(re0, tre2) );
						_st_agu_u2( fx_add_q31(im0, tim2) );
						_st_agu_u0( fx_add_q31(re1, tim3) );
						_st_agu_u4( fx_sub_q31(im1, tre3) );
					}

				}


                ix = 2 * id - N4_cplx*2 + l;
                id = 4 * id; ido +=2;
            }
        }
        N4_cplx <<= 1;
    }
    
	_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod1(0, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod2(0, AGU_DT_q31, AGU_AD_INC_q31_REG(0));
	_set_agu_mod3(0, AGU_DT_q31, AGU_AD_DEC_q31_REG(1));
	_set_agu_mod4(0, AGU_DT_q31, AGU_AD_DEC_q31_REG(2));

	_set_agu_mod5(1, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod6(1, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod7(1, AGU_DT_q31, AGU_AD_INC_q31_REG(3));

	_set_agu_mod8(2, AGU_DT_q31, AGU_AD_INC_q31(1));
	_set_agu_mod9(2, AGU_DT_q31, AGU_AD_DEC_q31(1));
	_set_agu_mod10(2, AGU_DT_q31, AGU_AD_INC_q31_REG(3));

    for (k = dspfx_min_q31(1, nOrder-3); k >=0 ; k--) {
        int32_t d1 = 1 << k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;

		_set_agu_os0(N4_cplx - 1);
		_set_agu_os1(N4_cplx*3 + 1);
		_set_agu_os2(N4_cplx*3 - 1);
		_set_agu_os3(d1*2+1);

		_set_agu_ap0(output);
		_set_agu_ap1(handle->ptwd + 1);
		_set_agu_ap2(handle->ptwd3 + 1);
		if (_Rarely(unitShiftPrev || unitShift)) {
			for (l = 0; l < l_cnt; l++) {
				re0 = _agu_u0(); //COMPLEX_RE(input, i0)
				im0 = _agu_u2(); //COMPLEX_IM(input, i0)
				re1 = _agu_u0(); //COMPLEX_RE(input, i1)
				im1 = _agu_u2(); //COMPLEX_IM(input, i1)

				ss1 = dspfx_neg_q31(_agu_u6()); 
				ss3 = dspfx_neg_q31(_agu_u9());

					  dspfx_a64mul_q31(_agu_u0(), _agu_u5(), acc);
				re2 = dspfx_a64mac_q31(_agu_u1(), _agu_u6(), acc);    // int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1 )>>31);
					  dspfx_a64mul_q31(_agu_u0(), ss1, acc);
				im2 = dspfx_a64mac_q31(_agu_u2(), _agu_u7(), acc);    // int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1 )>>31);

					  dspfx_a64mul_q31(_agu_u0(), _agu_u8(), acc);
				re3 = dspfx_a64mac_q31(_agu_u1(), _agu_u9(), acc);    // int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3 )>>31);
					  dspfx_a64mul_q31(_agu_u0(), ss3, acc);
				im3 = dspfx_a64mac_q31(_agu_u3(), _agu_u10(), acc);   // int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3 )>>31);

				tre2 = fx_add_q31(re2, re3) >> unitShiftPrev;
				tre3 = fx_sub_q31(re2, re3) >> unitShiftPrev;
				tim2 = fx_add_q31(im2, im3) >> unitShiftPrev;
				tim3 = fx_sub_q31(im2, im3) >> unitShiftPrev;

				_st_agu_u0( fx_sub_q31(re0, tre2) >> unitShift );
				_st_agu_u2( fx_sub_q31(im0, tim2) >> unitShift );
				_st_agu_u0( fx_sub_q31(re1, tim3) >> unitShift );
				_st_agu_u2( fx_add_q31(im1, tre3) >> unitShift );
				_st_agu_u0( fx_add_q31(re0, tre2) >> unitShift );
				_st_agu_u2( fx_add_q31(im0, tim2) >> unitShift );
				_st_agu_u0( fx_add_q31(re1, tim3) >> unitShift );
				_st_agu_u4( fx_sub_q31(im1, tre3) >> unitShift );
			}

		} else {
			for (l = 0; l < l_cnt; l++) {
				re0 = _agu_u0(); //COMPLEX_RE(input, i0)
				im0 = _agu_u2(); //COMPLEX_IM(input, i0)
				re1 = _agu_u0(); //COMPLEX_RE(input, i1)
				im1 = _agu_u2(); //COMPLEX_IM(input, i1)

				ss1 = dspfx_neg_q31(_agu_u6()); 
				ss3 = dspfx_neg_q31(_agu_u9());

					  dspfx_a64mul_q31(_agu_u0(), _agu_u5(), acc);
				re2 = dspfx_a64mac_q31(_agu_u1(), _agu_u6(), acc);    // int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1 )>>31);
					  dspfx_a64mul_q31(_agu_u0(), ss1, acc);
				im2 = dspfx_a64mac_q31(_agu_u2(), _agu_u7(), acc);    // int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1 )>>31);

					  dspfx_a64mul_q31(_agu_u0(), _agu_u8(), acc);
				re3 = dspfx_a64mac_q31(_agu_u1(), _agu_u9(), acc);    // int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3 )>>31);
					  dspfx_a64mul_q31(_agu_u0(), ss3, acc);
				im3 = dspfx_a64mac_q31(_agu_u3(), _agu_u10(), acc);   // int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3 )>>31);

				tre2 = fx_add_q31(re2, re3) ;
				tre3 = fx_sub_q31(re2, re3) ;
				tim2 = fx_add_q31(im2, im3) ;
				tim3 = fx_sub_q31(im2, im3) ;

				_st_agu_u0( fx_sub_q31(re0, tre2)  );
				_st_agu_u2( fx_sub_q31(im0, tim2)  );
				_st_agu_u0( fx_sub_q31(re1, tim3)  );
				_st_agu_u2( fx_add_q31(im1, tre3)  );
				_st_agu_u0( fx_add_q31(re0, tre2)  );
				_st_agu_u2( fx_add_q31(im0, tim2)  );
				_st_agu_u0( fx_add_q31(re1, tim3)  );
				_st_agu_u4( fx_sub_q31(im1, tre3)  );
			}
		}
        N4_cplx <<= 1;
    }
#endif
	_DSPLIB_restore_regs();
}

#else

void risc_dsp_cfft_sradix_dit_q31(const cfft_q31_t* handle, q31_t* input, q31_t* output)
{
    int32_t i, k, l;
    int32_t nLength = handle->length;
    int32_t N4_cplx;
    int32_t nOrder = 30 - dspfx_norm_q31(handle->length);
    int32_t unitShiftPrev, unitShift, shiftBits = handle->shiftBits;
   
    q31_t cc1_neg, ss1, ss1_neg, cc3_neg, ss3_neg, ss3;
    q31_t re0, im0, re1, im1, re2, im2, re3, im3;
    q31_t tre2, tre3;
    q31_t tim2, tim3;

    int32_t ix, id, ido;
    q31_t *pI0, *pI1, *pI2, *pI3;

    dspfx_accum64()

    // stage bitreverse reordering
    if (input == output) {
        // in place bit-reverse shuffle
        for (i = 0; i < nLength; i++)
        {
            int32_t i_rev = handle->tab_bit_rev[i];
            if (i < i_rev) {
                q31_t tmp_re = COMPLEX_RE(output, i_rev);
                q31_t tmp_im = COMPLEX_IM(output, i_rev);
                COMPLEX_RE(output, i_rev) = COMPLEX_RE(input, i);
                COMPLEX_IM(output, i_rev) = COMPLEX_IM(input, i);
                COMPLEX_RE(output, i) = tmp_re;
                COMPLEX_IM(output, i) = tmp_im;
            }
        }
    } else {
        // out of place bit-reverse shuffle
        for (i = 0; i < nLength; i++)
        {
            int32_t i_rev = handle->tab_bit_rev[i];
            output[i_rev*2]     = input[i*2];
            output[i_rev*2 + 1] = input[i*2 + 1];
        }
    }

    // stage: nOrder - 1
    // baseop: radix-2 with trivial multiply
    ix = 1;
    id = 4;
    ido = 2;
    unitShift = (shiftBits-- > 0) ? 1 : 0;
    while (ix < nLength) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
        pI0 = output + ix*2 - 2;
        if (unitShift == 0) {
            for (i = 0; i<i_cnt; i++, pI0 += id*2) {
                re0 = pI0[0];
                im0 = pI0[1];
                re1 = pI0[2];
                im1 = pI0[3];
                pI0[0] = dspfx_add_q31(re0, re1);
                pI0[1] = dspfx_add_q31(im0, im1);
                pI0[2] = dspfx_sub_q31(re0, re1);
                pI0[3] = dspfx_sub_q31(im0, im1);
            }
        } else {
            for (i = 0; i<i_cnt; i++, pI0 += id*2) {
                re0 = pI0[0];
                im0 = pI0[1];
                re1 = pI0[2];
                im1 = pI0[3];
                pI0[0] = dspfx_add_q31(re0, re1) >> unitShift;
                pI0[1] = dspfx_add_q31(im0, im1) >> unitShift;
                pI0[2] = dspfx_sub_q31(re0, re1) >> unitShift;
                pI0[3] = dspfx_sub_q31(im0, im1) >> unitShift;
            }
        }
        ix = 2 * id - 1;
        id = 4 * id; ido +=2;
    }

    // stage: nOrder - 2
    // baseop: split radix with trivial multiply
    ix = 0;
    id = 8;
    ido = 3;
    unitShiftPrev = unitShift;
    unitShift = (shiftBits-- > 0) ? 1 : 0;
    while (ix < nLength - 1) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
        pI0 = output + ix*2;
        if (unitShiftPrev || unitShift) {
            for (i = 0; i<i_cnt; i++) {
                re0 = pI0[0]; //COMPLEX_RE(input, i0)
                im0 = pI0[1]; //COMPLEX_IM(input, i0)
                re1 = pI0[2]; //COMPLEX_RE(input, i1)
                im1 = pI0[3]; //COMPLEX_IM(input, i1)
                tre2 = pI0[4]; //COMPLEX_RE(input, i2)
                tim2 = pI0[5]; //COMPLEX_IM(input, i2)
                tre3 = pI0[6]; //COMPLEX_RE(input, i3)
                tim3 = pI0[7]; //COMPLEX_IM(input, i3)

                re2 = dspfx_add_q31(tre2, tre3) >> unitShiftPrev;
                im2 = dspfx_add_q31(tim2, tim3) >> unitShiftPrev;
                re3 = dspfx_sub_q31(tre2, tre3) >> unitShiftPrev;
                im3 = dspfx_sub_q31(tim2, tim3) >> unitShiftPrev;

                pI0[0] = dspfx_add_q31(re0, re2) >> unitShift;
                pI0[1] = dspfx_add_q31(im0, im2) >> unitShift;
                pI0[2] = dspfx_add_q31(re1, im3) >> unitShift;
                pI0[3] = dspfx_sub_q31(im1, re3) >> unitShift;
                pI0[4] = dspfx_sub_q31(re0, re2) >> unitShift;
                pI0[5] = dspfx_sub_q31(im0, im2) >> unitShift;
                pI0[6] = dspfx_sub_q31(re1, im3) >> unitShift;
                pI0[7] = dspfx_add_q31(im1, re3) >> unitShift;

                pI0 += id*2;
            }
        } else {
            for (i = 0; i<i_cnt; i++) {
                re0 = pI0[0]; //COMPLEX_RE(input, i0)
                im0 = pI0[1]; //COMPLEX_IM(input, i0)
                re1 = pI0[2]; //COMPLEX_RE(input, i1)
                im1 = pI0[3]; //COMPLEX_IM(input, i1)
                tre2 = pI0[4]; //COMPLEX_RE(input, i2)
                tim2 = pI0[5]; //COMPLEX_IM(input, i2)
                tre3 = pI0[6]; //COMPLEX_RE(input, i3)
                tim3 = pI0[7]; //COMPLEX_IM(input, i3)

                re2 = dspfx_add_q31(tre2, tre3);
                im2 = dspfx_add_q31(tim2, tim3);
                re3 = dspfx_sub_q31(tre2, tre3);
                im3 = dspfx_sub_q31(tim2, tim3);

                pI0[0] = dspfx_add_q31(re0, re2);
                pI0[1] = dspfx_add_q31(im0, im2);
                pI0[2] = dspfx_add_q31(re1, im3);
                pI0[3] = dspfx_sub_q31(im1, re3);
                pI0[4] = dspfx_sub_q31(re0, re2);
                pI0[5] = dspfx_sub_q31(im0, im2);
                pI0[6] = dspfx_sub_q31(re1, im3);
                pI0[7] = dspfx_add_q31(im1, re3);

                pI0 += id*2;
            }
        }
        ix = 2 * id - 4;
        id = 4 * id; ido +=2;
    }

    // stage: [(nOrder-3) .. 2]
    // baseop: split-radix
    N4_cplx = 4; // N4 = N / 4
    for (k = nOrder-3; k >= 2 ; k--) {
        q31_t *pTWD1 = handle->ptwd;
        q31_t *pTWD3 = handle->ptwd3;
        int32_t d1 = 2<<k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;

        for (l = 0; l < l_cnt; l++, pTWD1+=d1, pTWD3+=d1) {
            int32_t ix = l;
            int32_t id = N4_cplx*4;
            int32_t ido = nOrder - k + 1;

            cc1_neg = *pTWD1;
            cc3_neg = *pTWD3;
            ss1_neg = *(pTWD1+1);
            ss3_neg = *(pTWD3+1);
            ss1 = dspfx_neg_q31(ss1_neg); 
            ss3 = dspfx_neg_q31(ss3_neg);

            while (ix < nLength - 1) {
                int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
                pI0 = output + ix*2;
                if (unitShiftPrev || unitShift) {
                    for (i = 0; i<i_cnt; i++) {
                        pI1 = pI0 + N4_cplx;
                        pI2 = pI1 + N4_cplx;
                        pI3 = pI2 + N4_cplx;

                        re0 = pI0[0]; //COMPLEX_RE(input, i0)
                        im0 = pI0[1]; //COMPLEX_IM(input, i0)
                        re1 = pI1[0]; //COMPLEX_RE(input, i1)
                        im1 = pI1[1]; //COMPLEX_IM(input, i1)

                        tre2 = pI2[0]; //COMPLEX_RE(input, i2)
                        tim2 = pI2[1]; //COMPLEX_IM(input, i2)
                        tre3 = pI3[0]; //COMPLEX_RE(input, i3)
                        tim3 = pI3[1]; //COMPLEX_IM(input, i3)

                              dspfx_a64mul_q31(tre2, cc1_neg, acc);
                        re2 = dspfx_a64mac_q31(tim2, ss1_neg, acc); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                              dspfx_a64mul_q31(tim2, cc1_neg, acc);
                        im2 = dspfx_a64mac_q31(tre2, ss1, acc); //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                              dspfx_a64mul_q31(tre3, cc3_neg, acc);
                        re3 = dspfx_a64mac_q31(tim3, ss3_neg, acc); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                              dspfx_a64mul_q31(tim3, cc3_neg, acc);
                        im3 = dspfx_a64mac_q31(tre3, ss3, acc);  //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                        tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                        tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                        tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                        pI0[0] = dspfx_sub_q31(re0, tre2) >> unitShift;
                        pI0[1] = dspfx_sub_q31(im0, tim2) >> unitShift;
                        pI1[0] = dspfx_sub_q31(re1, tim3) >> unitShift;
                        pI1[1] = dspfx_add_q31(im1, tre3) >> unitShift;
                        pI2[0] = dspfx_add_q31(re0, tre2) >> unitShift;
                        pI2[1] = dspfx_add_q31(im0, tim2) >> unitShift;
                        pI3[0] = dspfx_add_q31(re1, tim3) >> unitShift;
                        pI3[1] = dspfx_sub_q31(im1, tre3) >> unitShift;
                        pI0 += id*2;
                    }
                } else
                {
                    for (i = 0; i<i_cnt; i++) {
                        pI1 = pI0 + N4_cplx;
                        pI2 = pI1 + N4_cplx;
                        pI3 = pI2 + N4_cplx;

                        re0 = pI0[0]; //COMPLEX_RE(input, i0)
                        im0 = pI0[1]; //COMPLEX_IM(input, i0)
                        re1 = pI1[0]; //COMPLEX_RE(input, i1)
                        im1 = pI1[1]; //COMPLEX_IM(input, i1)

                        tre2 = pI2[0]; //COMPLEX_RE(input, i2)
                        tim2 = pI2[1]; //COMPLEX_IM(input, i2)
                        tre3 = pI3[0]; //COMPLEX_RE(input, i3)
                        tim3 = pI3[1]; //COMPLEX_IM(input, i3)

                              dspfx_a64mul_q31(tre2, cc1_neg, acc);
                        re2 = dspfx_a64mac_q31(tim2, ss1_neg, acc); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                              dspfx_a64mul_q31(tim2, cc1_neg, acc);
                        im2 = dspfx_a64mac_q31(tre2, ss1, acc); //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                              dspfx_a64mul_q31(tre3, cc3_neg, acc);
                        re3 = dspfx_a64mac_q31(tim3, ss3_neg, acc); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                              dspfx_a64mul_q31(tim3, cc3_neg, acc);
                        im3 = dspfx_a64mac_q31(tre3, ss3, acc);  //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3);
                        tim2 = dspfx_add_q31(im2, im3);
                        tre3 = dspfx_sub_q31(re2, re3);
                        tim3 = dspfx_sub_q31(im2, im3);

                        pI0[0] = dspfx_sub_q31(re0, tre2);
                        pI0[1] = dspfx_sub_q31(im0, tim2);
                        pI1[0] = dspfx_sub_q31(re1, tim3);
                        pI1[1] = dspfx_add_q31(im1, tre3);
                        pI2[0] = dspfx_add_q31(re0, tre2);
                        pI2[1] = dspfx_add_q31(im0, tim2);
                        pI3[0] = dspfx_add_q31(re1, tim3);
                        pI3[1] = dspfx_sub_q31(im1, tre3);
                        pI0 += id*2;
                    }
                }
                ix = 2 * id - N4_cplx*2 + l;
                id = 4 * id; ido +=2;
            }
        }
        N4_cplx <<= 1;
    }

    for (k = dspfx_min_q31(1, nOrder-3); k >=0 ; k--) {
        q31_t *pTWD1 = handle->ptwd;
        q31_t *pTWD3 = handle->ptwd3;
        int32_t d1 = 2 << k;
        int32_t l_cnt = (N4_cplx>>1);
        pI0 = output;
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        if (unitShiftPrev || unitShift)
        {
            for (l = 0; l < l_cnt; l++, pI0 += 2, pTWD1+=d1, pTWD3+=d1) {
                pI1 = pI0 + N4_cplx;
                pI2 = pI1 + N4_cplx;
                pI3 = pI2 + N4_cplx;

                re0 = pI0[0]; //COMPLEX_RE(input, i0)
                im0 = pI0[1]; //COMPLEX_IM(input, i0)
                re1 = pI1[0]; //COMPLEX_RE(input, i1)
                im1 = pI1[1]; //COMPLEX_IM(input, i1)

                tre2 = pI2[0]; //COMPLEX_RE(input, i2)
                tim2 = pI2[1]; //COMPLEX_IM(input, i2)
                tre3 = pI3[0]; //COMPLEX_RE(input, i3)
                tim3 = pI3[1]; //COMPLEX_IM(input, i3)

                cc1_neg = *pTWD1;
                cc3_neg = *pTWD3;
                ss1_neg = *(pTWD1+1);
                ss3_neg = *(pTWD3+1);
                ss1 = dspfx_neg_q31(ss1_neg); 
                ss3 = dspfx_neg_q31(ss3_neg);

                      dspfx_a64mul_q31(tre2, cc1_neg, acc);
                re2 = dspfx_a64mac_q31(tim2, ss1_neg, acc); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                      dspfx_a64mul_q31(tim2, cc1_neg, acc);
                im2 = dspfx_a64mac_q31(tre2, ss1, acc); //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                      dspfx_a64mul_q31(tre3, cc3_neg, acc);
                re3 = dspfx_a64mac_q31(tim3, ss3_neg, acc); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                      dspfx_a64mul_q31(tim3, cc3_neg, acc);
                im3 = dspfx_a64mac_q31(tre3, ss3, acc);  //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                pI0[0] = dspfx_sub_q31(re0, tre2) >> unitShift;
                pI0[1] = dspfx_sub_q31(im0, tim2) >> unitShift;
                pI1[0] = dspfx_sub_q31(re1, tim3) >> unitShift;
                pI1[1] = dspfx_add_q31(im1, tre3) >> unitShift;
                pI2[0] = dspfx_add_q31(re0, tre2) >> unitShift;
                pI2[1] = dspfx_add_q31(im0, tim2) >> unitShift;
                pI3[0] = dspfx_add_q31(re1, tim3) >> unitShift;
                pI3[1] = dspfx_sub_q31(im1, tre3) >> unitShift;
            }
        } else 
        {
            for (l = 0; l < l_cnt; l++, pI0 += 2, pTWD1+=d1, pTWD3+=d1) {
                pI1 = pI0 + N4_cplx;
                pI2 = pI1 + N4_cplx;
                pI3 = pI2 + N4_cplx;

                re0 = pI0[0]; //COMPLEX_RE(input, i0)
                im0 = pI0[1]; //COMPLEX_IM(input, i0)
                re1 = pI1[0]; //COMPLEX_RE(input, i1)
                im1 = pI1[1]; //COMPLEX_IM(input, i1)

                tre2 = pI2[0]; //COMPLEX_RE(input, i2)
                tim2 = pI2[1]; //COMPLEX_IM(input, i2)
                tre3 = pI3[0]; //COMPLEX_RE(input, i3)
                tim3 = pI3[1]; //COMPLEX_IM(input, i3)

                cc1_neg = *pTWD1;
                cc3_neg = *pTWD3;
                ss1_neg = *(pTWD1+1);
                ss3_neg = *(pTWD3+1);
                ss1 = dspfx_neg_q31(ss1_neg); 
                ss3 = dspfx_neg_q31(ss3_neg);

                      dspfx_a64mul_q31(tre2, cc1_neg, acc);
                re2 = dspfx_a64mac_q31(tim2, ss1_neg, acc); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                      dspfx_a64mul_q31(tim2, cc1_neg, acc);
                im2 = dspfx_a64mac_q31(tre2, ss1, acc); //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                      dspfx_a64mul_q31(tre3, cc3_neg, acc);
                re3 = dspfx_a64mac_q31(tim3, ss3_neg, acc); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                      dspfx_a64mul_q31(tim3, cc3_neg, acc);
                im3 = dspfx_a64mac_q31(tre3, ss3, acc);  //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3);
                tre3 = dspfx_sub_q31(re2, re3);
                tim2 = dspfx_add_q31(im2, im3);
                tim3 = dspfx_sub_q31(im2, im3);

                pI0[0] = dspfx_sub_q31(re0, tre2);
                pI0[1] = dspfx_sub_q31(im0, tim2);
                pI1[0] = dspfx_sub_q31(re1, tim3);
                pI1[1] = dspfx_add_q31(im1, tre3);
                pI2[0] = dspfx_add_q31(re0, tre2);
                pI2[1] = dspfx_add_q31(im0, tim2);
                pI3[0] = dspfx_add_q31(re1, tim3);
                pI3[1] = dspfx_sub_q31(im1, tre3);
            }
        }
        N4_cplx <<= 1;
    }
}

#endif

#if !defined(EXCLUDE_DSP_CFFT_4_Q31)

void dsp_cfft_4_q31(const cfft_q31_t* handle, const q31_t* input, q31_t* output){
    q31_t t0r, t0i;
    q31_t t1r, t1i;
    q31_t t2r, t2i;
    q31_t t3r, t3i;
    
    t0r = dspfx_add_q31(input[0], input[4]);
    t0i = dspfx_add_q31(input[1], input[5]);
    
    t1r = dspfx_add_q31(input[2], input[6]);
    t1i = dspfx_add_q31(input[3], input[7]);
    
    t2r = dspfx_sub_q31(input[0], input[4]);
    t2i = dspfx_sub_q31(input[1], input[5]);
    // multiply by w1 (-j)
    t3i = dspfx_sub_q31(input[6], input[2]);
	t3r = dspfx_sub_q31(input[3], input[7]);
    
    output[0] = dspfx_add_q31(t0r, t1r);
    output[1] = dspfx_add_q31(t0i, t1i);

	output[2] = dspfx_add_q31(t2r, t3r);
	output[3] = dspfx_add_q31(t2i, t3i);
    
    output[4] = dspfx_sub_q31(t0r, t1r);
    output[5] = dspfx_sub_q31(t0i, t1i);    
    
    output[6] = dspfx_sub_q31(t2r, t3r);
    output[7] = dspfx_sub_q31(t2i, t3i);
}
#endif

#if !defined(EXCLUDE_DSP_CFFT_8_Q31)
void dsp_cfft_8_q31(const cfft_q31_t* handle, const q31_t* input, q31_t* output){
    const q31_t cos_pi_4 = 0x5A827995; // cos(pi/4) in Q31
    q31_t t0r, t0i;
    q31_t t1r, t1i;
    q31_t t2r, t2i;
    q31_t t3r, t3i;
    q31_t t4r, t4i;
    q31_t t5r, t5i;
    
    q31_t t0r_sub, t0i_sub;
    q31_t t1r_sub, t1i_sub;
    q31_t t2r_sub, t2i_sub;
    q31_t t3r_sub, t3i_sub;
    
    dspfx_accum64()
    // Node with sums (upper 4 compex inputs)
    t0r = dspfx_add_q31(input[0], input[8]);
    t0i = dspfx_add_q31(input[1], input[9]);
    
    t1r = dspfx_add_q31(input[2], input[10]);
    t1i = dspfx_add_q31(input[3], input[11]);
        
    t2r = dspfx_add_q31(input[4], input[12]);
    t2i = dspfx_add_q31(input[5], input[13]);
    
    t3r = dspfx_add_q31(input[6], input[14]);
    t3i = dspfx_add_q31(input[7], input[15]);
    
    // Node with subs (lower 4 compex inputs)
    t0r_sub = dspfx_sub_q31(input[0], input[8]);
    t0i_sub = dspfx_sub_q31(input[1], input[9]);
    
    t1r_sub = dspfx_sub_q31(input[2], input[10]);
    t1i_sub = dspfx_sub_q31(input[3], input[11]);
    
    // multiply by w1 (0.707 - 0.707j)
    t4r = dspfx_a64mul_q31(t1r_sub, cos_pi_4, acc);
    t4i = dspfx_a64mul_q31(t1i_sub, cos_pi_4, acc);    
    
    t1r_sub = dspfx_add_q31(t4r, t4i);
    t1i_sub = dspfx_sub_q31(t4i, t4r);
    
    // multiply by w2 (-j)
    t2r_sub = dspfx_sub_q31(input[5], input[13]);
    t2i_sub = dspfx_sub_q31(input[12], input[4]);
    
    t3r_sub = dspfx_sub_q31(input[14], input[6]); // * (-1)
    t3i_sub = dspfx_sub_q31(input[7], input[15]);
    
    // multiply by w3 (-0.707 - 0.707j)
    t4r = dspfx_a64mul_q31(t3r_sub, cos_pi_4, acc);
    t4i = dspfx_a64mul_q31(t3i_sub, cos_pi_4, acc);    
    
    t3r_sub = dspfx_add_q31(t4r, t4i);
    t3i_sub = dspfx_sub_q31(t4r, t4i);
    
    // second and third stage
    t4r = dspfx_add_q31(t0r, t2r);
    t4i = dspfx_add_q31(t0i, t2i);
    
    t5r = dspfx_add_q31(t1r, t3r);
    t5i = dspfx_add_q31(t1i, t3i);
    
    output[0] = dspfx_add_q31(t4r, t5r);
    output[1] = dspfx_add_q31(t4i, t5i);
    
    output[8] = dspfx_sub_q31(t4r, t5r);
    output[9] = dspfx_sub_q31(t4i, t5i);
    
    t4r = dspfx_sub_q31(t0r, t2r);
    t4i = dspfx_sub_q31(t0i, t2i);
    
    // multiply by -j
    t5r = dspfx_sub_q31(t1i, t3i);
    t5i = dspfx_sub_q31(t3r, t1r);
    
    output[4] = dspfx_add_q31(t4r, t5r);
    output[5] = dspfx_add_q31(t4i, t5i);
    
    output[12] = dspfx_sub_q31(t4r, t5r);
    output[13] = dspfx_sub_q31(t4i, t5i);
    
    // second and third stage
    t4r = dspfx_add_q31(t0r_sub, t2r_sub);
    t4i = dspfx_add_q31(t0i_sub, t2i_sub);
    
    t5r = dspfx_add_q31(t1r_sub, t3r_sub);
    t5i = dspfx_add_q31(t1i_sub, t3i_sub);
    
    output[2] = dspfx_add_q31(t4r, t5r);
    output[3] = dspfx_add_q31(t4i, t5i);
    
    output[10] = dspfx_sub_q31(t4r, t5r);
    output[11] = dspfx_sub_q31(t4i, t5i);
    
    t4r = dspfx_sub_q31(t0r_sub, t2r_sub);
    t4i = dspfx_sub_q31(t0i_sub, t2i_sub);
    
    // multiply by -j
    t5r = dspfx_sub_q31(t1i_sub, t3i_sub);
    t5i = dspfx_sub_q31(t3r_sub, t1r_sub);
    
    output[6] = dspfx_add_q31(t4r, t5r);
    output[7] = dspfx_add_q31(t4i, t5i);
    
    output[14] = dspfx_sub_q31(t4r, t5r);
    output[15] = dspfx_sub_q31(t4i, t5i);
}
#endif

#define XY_MAP_INPUT (((XY_REGION_SIZE/2))/sizeof(q31_t))
#define XY_MAP_OUT 0
#define XY_MAP_TWIDDLES 0
#define XY_FFT_RADIX2_Q31_THRESHOLD (((XY_REGION_SIZE/4))/sizeof(q31_t))

/**
 * Complex FFT "process" function for q31 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

#pragma Off_inline(dsp_cfft_4_q31, dsp_cfft_8_q31)


void _DSPLIB_XYCC dsp_cfft_q31(const cfft_q31_t* handle, const PTR(q31_t) input, PTR(q31_t) output)
{
#if defined(_ARC_AS210_mac32)
    if (handle->length > XY_FFT_RADIX2_Q31_THRESHOLD) {
        risc_dsp_cfft_sradix_dit_q31(handle, (q31_t*) input, output);
    } else if ( handle->length == 4 && handle->shiftBits == 0) {
        dsp_cfft_4_q31(handle, input, output);
    } else if ( handle->length == 8 && handle->shiftBits == 0) {
        dsp_cfft_8_q31(handle, input, output);
    } else {
        cfft_srad_q31_xy_t pInst;

        dsp_cfft_sradix_init_q31_cfX(
            &pInst, 
            handle->length, 
            XY_MAP_TWIDDLES);

        memcpy_StoX_int32(
            XY_MAP_INPUT, 
            (int32_t *)(void*) input, 
            2*handle->length); 

        pInst.shiftBits = handle->shiftBits;

        dsp_cfft_sradix_q31_XtoY_cfX_MAC32x32(
            &pInst, 
            XY_MAP_INPUT, 
            XY_MAP_OUT);

        memcpy_YtoS_int32(
            (int32_t *)(void*)output, 
            XY_MAP_OUT, 
            2*handle->length);
    }
#else
#if defined(DSP_CFFT_Q31_4)
    if ( handle->length == 4 && handle->shiftBits == 0) {
        dsp_cfft_4_q31(handle, input, output);
    } else
#endif
#if defined(DSP_CFFT_Q31_8)
    if ( handle->length == 8 && handle->shiftBits == 0) {
        dsp_cfft_8_q31(handle, input, output);
    }else
#endif
    {
        risc_dsp_cfft_sradix_dit_q31(handle, (q31_t*) input, output);
    }
#endif
}

void dsp_cfft_delete_q31(cfft_q31_t* handle) {};

#endif //!defined(EXCLUDE_DSP_CFFT_Q31)

#if !defined(EXCLUDE_DSP_CFFT_RADIX2_Q31)
#pragma Code("dsplib_dsp_cfft_radix2_q31")

dsp_status     dsp_cfft_radix2_init_q31 (cfft_q31_t* handle, uint16_t length, uint8_t invFlag, uint8_t bitReverse_flag)
{
    uint32_t* pState = 0; // pState pointer is not used in current release, reserved for future.
    dsp_status status;
    uint32_t order = 30-dspfx_norm_q31(length);

#if defined _ARCv2EM_XY_11D
    return DSP_ERR_LENGTH_TOO_BIG;
#endif

    handle->invFlag = invFlag;
    handle->bitReverse_flag = bitReverse_flag;

    if (0 == invFlag)
    {
        status=dsp_cfft_init_q31(handle, length, (PTR(uint32_t))pState);
        handle->shiftBits = order; // use 1/N multiplier for the FORWARD FFT
        handle->scaling = DSP_TRANSFORM_SCALING_SCALED;
    }
    else
    {
        status=dsp_cifft_init_q31(handle, length, (PTR(uint32_t))pState);
        handle->shiftBits = order; // use 1/N multiplier for the INVERSE FFT
        handle->scaling = DSP_TRANSFORM_SCALING_SCALED;
    }    

    return status;
}


void _DSPLIB_XYCC dsp_cfft_radix2_q31( const cfft_q31_t *handle,  PTR(q31_t) pInOut)
{
    if( 0 == handle->invFlag)
    {
        dsp_scale_q31(pInOut, 0x40000000, 0, pInOut, handle->length*2); // Scale by 0.5
        dsp_cfft_q31(handle,  pInOut, pInOut);
    }
    else
    {
        dsp_scale_q31(pInOut, 0x40000000, 0, pInOut, handle->length*2); // Scale by 0.5
        dsp_cifft_q31( handle,  pInOut, pInOut);
    }
}

#endif

