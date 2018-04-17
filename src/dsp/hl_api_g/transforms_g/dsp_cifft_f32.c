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

#if !defined(EXCLUDE_DSP_CIFFT_F32)
#pragma Code("dsplib_dsp_cifft_f32")

#if defined(_ARC_AS210_mac32)
#include "dsplib_ll_api.h"
#endif
#include "dsp_transform_tables.h"


/**
 * Initializes instance structure of inverse complex fft transform
 * for f32 data type. 
 *
 * \param handle - pointer to transform instance structure
 * \param length - transform size
 * \param pState - pointer to state vector buffer. length of state buffer should be obtained through "dsp_cifft_getsize_f32()" function.
 * \result dsp_status
 */

dsp_status _DSPLIB_XYCC dsp_cifft_init_f32(cfft_f32_t* handle, uint32_t length, PTR(f32_t) pState)
{
    uint32_t order = 30-dspfx_norm_q31(length);
    if ((length > DSP_MAX_TRANSFORM_SIZE))
        return DSP_ERR_LENGTH_TOO_BIG;
    if ((length < 4))
        return DSP_ERR_LENGTH_TOO_SMALL;
    if (length != (1 << order))
        return DSP_ERR_LENGTH_NOT_POWER2;
    if (dsp_cfft_f32_twd_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;
    if (dsp_cfft_f32_twd3_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;
    if (dsp_cfft_bitrev_tab[order - DSP_MIN_TRANSFORM_ORDER] == 0)
        return DSP_ERR_LENGTH_NO_TABLE;

    handle->length = length;

#ifdef _ARCv2EM_XY_11D
    handle->ptwd  = (f32_t*)((uint8_t*)pState);
    handle->ptwd3 = (f32_t*)((uint8_t*)pState + dsp_cifft_getsize_f32(length)/2);
    memcpy(handle->ptwd, dsp_cfft_f32_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], dsp_cifft_getsize_f32(length)/2);
    memcpy(handle->ptwd3, dsp_cfft_f32_twd3_tab[order - DSP_MIN_TRANSFORM_ORDER], dsp_cifft_getsize_f32(length)/2);
#else    
    handle->ptwd = (f32_t*)dsp_cfft_f32_twd_tab[order - DSP_MIN_TRANSFORM_ORDER];
    handle->ptwd3 = (f32_t*)dsp_cfft_f32_twd3_tab[order - DSP_MIN_TRANSFORM_ORDER];
#endif
    handle->tab_bit_rev = (int16_t*)dsp_cfft_bitrev_tab[order - DSP_MIN_TRANSFORM_ORDER];
    handle->scaling = DSP_TRANSFORM_SCALING_SCALED;
    return DSP_ERR_OK;
};

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__

void risc_dsp_cifft_sradix_dit_f32(const cfft_f32_t* handle, f32_t* input, f32_t* output)
{
    int32_t i, k, l;
    int32_t nLength = handle->length;
    int32_t nLength2 = nLength/2;
#if defined   __Xagu_large
    int32_t N8_cplx;
#else
    int32_t N4_cplx;
    f32_t re1, im1, ss1, ss3;
#endif
    int32_t nOrder = 30 - dspfx_norm_q31(handle->length);

    f32_t cc1_neg, ss1_neg, cc3_neg, ss3_neg;
    f32_t re0, im0, re2, im2, re3, im3;
    f32_t tre2, tre3;
    f32_t tim2, tim3;

    int32_t ix, id, ido;
	_DSPLIB_store_regs(4, 11, 4);

	_set_agu_ap0(input);
	_set_agu_ap1(input + 1);
	_set_agu_ap2(output);
	_set_agu_os0(nLength);

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

			for (i = 0; i < nLength2; i++)
			{
				re0 = _as_float(_agu_u0());
				im0 = _as_float(_agu_u2());
				_st_agu_u4( _as_uint32( ADD_F32( re0, _as_float(_agu_u1()) ) ) );
				_st_agu_u4( _as_uint32( ADD_F32( im0, _as_float(_agu_u3()) ) ) );
				_st_agu_u4( _as_uint32( SUB_F32( re0, _as_float(_agu_u0()) ) ) );
				_st_agu_u4( _as_uint32( SUB_F32( im0, _as_float(_agu_u2()) ) ) );
			}

	} else {

		_set_agu_mod0(0, AGU_DT_q31, AGU_AD_BITREV_INC_q31_REG(0));
		_set_agu_mod1(0, AGU_DT_q31, AGU_AD_INC_q31(0));
		_set_agu_mod2(1, AGU_DT_q31, AGU_AD_BITREV_INC_q31_REG(0));
		_set_agu_mod3(1, AGU_DT_q31, AGU_AD_INC_q31(0));
		_set_agu_mod4(2, AGU_DT_q31, AGU_AD_INC_q31(1));

		if ( ((uint32_t)input & ((nLength << 3) - 1) ) == 0 )
		{
				for (i = 0; i < nLength2; i++)
				{
					re0 = _as_float(_agu_u0());
					im0 = _as_float(_agu_u2());
					_st_agu_u4( _as_uint32( ADD_F32( re0, _as_float(_agu_u1()) ) ) );
					_st_agu_u4( _as_uint32( ADD_F32( im0, _as_float(_agu_u3()) ) ) );
					_st_agu_u4( _as_uint32( SUB_F32( re0, _as_float(_agu_u0()) ) ) );
					_st_agu_u4( _as_uint32( SUB_F32( im0, _as_float(_agu_u2()) ) ) );
				}
		} else {
			_set_agu_os1(((uint32_t)input - ((uint32_t)input & ~((nLength << 3)-1))) >> 2);
			_set_agu_mod5(0, AGU_DT_q31, AGU_AD_INC_q31_REG(1));
			_set_agu_mod6(0, AGU_DT_q31, AGU_AD_DEC_q31_REG(1));
			_set_agu_mod7(1, AGU_DT_q31, AGU_AD_INC_q31_REG(1));
			_set_agu_mod8(1, AGU_DT_q31, AGU_AD_DEC_q31_REG(1));

			for (i = 0; i < nLength2; i++)
				{
					re0 = _as_float(_agu_u6());
					im0 = _as_float(_agu_u8());
					_add(_agu_u0(), _agu_u2());
					_add(_agu_u5(), _agu_u7());
					_st_agu_u4( _as_uint32( ADD_F32( re0, _as_float(_agu_u1()) ) ) );
					_st_agu_u4( _as_uint32( ADD_F32( im0, _as_float(_agu_u3()) ) ) );
					_st_agu_u4( _as_uint32( SUB_F32( re0, _as_float(_agu_u6()) ) ) );
					_st_agu_u4( _as_uint32( SUB_F32( im0, _as_float(_agu_u8()) ) ) );
					_add(_agu_u0(), _agu_u2());
					_add(_agu_u5(), _agu_u7());
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
    while (ix < nLength - 1) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);

		_set_agu_ap0(output + ix*2);
		_set_agu_ap1(output + ix*2 + 4);
		_set_agu_ap2(output + ix*2);
		_set_agu_ap3(output + ix*2 + 4);
		_set_agu_os0(id*2 - 4 + 1);

        //pI0 = output + ix*2;
			for (i = 0; i<i_cnt; i++) {
				re2 = _as_float(_agu_u0()); //pI0[4]; //COMPLEX_RE(input, i2)
				im2 = _as_float(_agu_u0()); //pI0[5]; //COMPLEX_IM(input, i2)
				re3 = _as_float(_agu_u0()); //pI0[6]; //COMPLEX_RE(input, i3)
				im3 = _as_float(_agu_u1()); //pI0[7]; //COMPLEX_IM(input, i3)

				_st_agu_u5( _as_uint32( SUB_F32( _as_float(_agu_u2()), re2) ) );
				_st_agu_u7( _as_uint32( ADD_F32( _as_float(_agu_u3()), re2) ) );
				_st_agu_u5( _as_uint32( SUB_F32( _as_float(_agu_u2()), im2) ) );
				_st_agu_u7( _as_uint32( ADD_F32( _as_float(_agu_u3()), im2) ) );
				_st_agu_u5( _as_uint32( ADD_F32( _as_float(_agu_u2()), im3) ) );
				_st_agu_u7( _as_uint32( SUB_F32( _as_float(_agu_u3()), im3) ) );
				_st_agu_u6( _as_uint32( SUB_F32( _as_float(_agu_u2()), re3) ) );
				_st_agu_u8( _as_uint32( ADD_F32( _as_float(_agu_u4()), re3) ) );
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
     *       u s u a   us u a  d  du   d  d u  uu
     *       l u l c   lu l c  d  db   d  d b  bb
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

		_set_agu_os0(N8_cplx>>1);
		_set_agu_os2(d1*2 - 1);

		_set_agu_ap2(handle->ptwd);
		_set_agu_ap3(handle->ptwd3);

			for (l = 0; l < l_cnt; l++) //pCos1+=d1, pSin1+=d1, pCos3+=d3, pSin3+=d3
			{
				int32_t ix = l;
				int32_t id2 = N8_cplx*4;
				int32_t l_ido = ((nLength) >> (nOrder - k + 1));

				cc1_neg = _as_float(_agu_u10());
				ss1_neg = _as_float(_agu_u12());
				cc3_neg = _as_float(_agu_u11());
				ss3_neg = _as_float(_agu_u13());

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
						re2 = MPY_F32(_as_float(_agu_u0()), cc1_neg);
						      MSU_F32(re2, _as_float(_agu_u2()), ss1_neg);    // int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1 )>>31);
						im2 = MPY_F32(_as_float(_agu_u1()), cc1_neg);
						      MAC_F32(im2, _as_float(_agu_u3()), ss1_neg);    // int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1 )>>31);

						re3 = MPY_F32(_as_float(_agu_u0()), cc3_neg);
						      MSU_F32(re3, _as_float(_agu_u2()), ss3_neg);    // int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3 )>>31);
						im3 = MPY_F32(_as_float(_agu_u1()), cc3_neg);
						      MAC_F32(im3, _as_float(_agu_u4()), ss3_neg);   // int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3 )>>31);

						tre2 = ADD_F32(re2, re3);
						tre3 = SUB_F32(re2, re3);
						tim2 = ADD_F32(im2, im3);
						tim3 = SUB_F32(im2, im3);

						_st_agu_u3(/*pI2[0] = */_as_uint32( ADD_F32( _as_float(_agu_u8())/*re0*/, tre2)) );
						_st_agu_u0(/*pI3[0] = */_as_uint32( SUB_F32( _as_float(_agu_u6())/*re1*/, tim3)) );
						_st_agu_u4(/*pI3[1] = */_as_uint32( ADD_F32( _as_float(_agu_u9())/*im1*/, tre3)) );
						_st_agu_u4(/*pI2[1] = */_as_uint32( ADD_F32( _as_float(_agu_u8())/*im0*/, tim2)) );
						_st_agu_u1(/*pI1[1] = */_as_uint32( SUB_F32( _as_float(_agu_u7())/*im1*/, tre3)) );
						_st_agu_u4(/*pI1[0] = */_as_uint32( ADD_F32( _as_float(_agu_u9())/*re1*/, tim3)) );
						_st_agu_u0(/*pI0[0] = */_as_uint32( SUB_F32( _as_float(_agu_u6())/*re0*/, tre2)) );
						_st_agu_u5(/*pI0[1] = */_as_uint32( SUB_F32( _as_float(_agu_u14())/*im0*/, tim2)) );
					}

					ix = id2 - N8_cplx + l;
					id2 = 4 * id2; l_ido >>= 2;
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

		_set_agu_os0(N8_cplx>>1);
		_set_agu_os1(N8_cplx + 1);
		_set_agu_os3(delta - d2 + 1);

		_set_agu_ap0(output + N8_cplx);
		_set_agu_ap1(output);
		_set_agu_ap2(handle->ptwd);
		
			__builtin_assume(l_cnt >= 1);
			for (l = 0; l < l_cnt; l++)
			{
				re2 = MPY_F32(_as_float(_agu_u0()), _as_float(_agu_u10()));
				      MSU_F32(re2, _as_float(_agu_u2()), _as_float(_agu_u11()));    // int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1 )>>31);
				im2 = MPY_F32(_as_float(_agu_u1()), _as_float(_agu_u10()));
				      MAC_F32(im2, _as_float(_agu_u3()), _as_float(_agu_u12()));    // int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1 )>>31);

				re3 = MPY_F32(_as_float(_agu_u0()), _as_float(_agu_u10()));
				      MSU_F32(re3, _as_float(_agu_u2()), _as_float(_agu_u11()));    // int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3 )>>31);
				im3 = MPY_F32(_as_float(_agu_u1()), _as_float(_agu_u10()));
				      MAC_F32(im3, _as_float(_agu_u4()), _as_float(_agu_u13()));   // int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3 )>>31);

				tre2 = ADD_F32(re2, re3);
				tre3 = SUB_F32(re2, re3);
				tim2 = ADD_F32(im2, im3);
				tim3 = SUB_F32(im2, im3);

	            _st_agu_u3(/*pI2[0] = */_as_uint32( ADD_F32( _as_float(_agu_u8())/*re0*/, tre2)) );
	            _st_agu_u0(/*pI3[0] = */_as_uint32( SUB_F32( _as_float(_agu_u6())/*re1*/, tim3)) );
	            _st_agu_u4(/*pI3[1] = */_as_uint32( ADD_F32( _as_float(_agu_u9())/*im1*/, tre3)) );
	            _st_agu_u4(/*pI2[1] = */_as_uint32( ADD_F32( _as_float(_agu_u8())/*im0*/, tim2)) );
	            _st_agu_u1(/*pI1[1] = */_as_uint32( SUB_F32( _as_float(_agu_u7())/*im1*/, tre3)) );
	            _st_agu_u4(/*pI1[0] = */_as_uint32( ADD_F32( _as_float(_agu_u9())/*re1*/, tim3)) );
	            _st_agu_u0(/*pI0[0] = */_as_uint32( SUB_F32( _as_float(_agu_u6())/*re0*/, tre2)) );
	            _st_agu_u5(/*pI0[1] = */_as_uint32( SUB_F32( _as_float(_agu_u6())/*im0*/, tim2)) );
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

            cc1_neg = _as_float(_agu_u5());
            ss1_neg = _as_float(_agu_u6());
            cc3_neg = _as_float(_agu_u7());
            ss3_neg = _as_float(_agu_u8());
            ss1 = -(ss1_neg); 
            ss3 = -(ss3_neg);

            while (ix < nLength - 1) 
            {
                int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
                // pI0 = output + ix*2;

				_set_agu_os2(id*2 - N4_cplx*3 - 1);
				_set_agu_ap0(output + ix*2);

				
					for (i = 0; i<i_cnt; i++) 
					{
						re0 = _as_float(_agu_u0()); //COMPLEX_RE(input, i0)
						im0 = _as_float(_agu_u2()); //COMPLEX_IM(input, i0)
						re1 = _as_float(_agu_u0()); //COMPLEX_RE(input, i1)
						im1 = _as_float(_agu_u2()); //COMPLEX_IM(input, i1)

						re2 = MPY_F32(_as_float(_agu_u0()), cc1_neg);
						      MAC_F32(re2, _as_float(_agu_u1()), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
						im2 = MPY_F32(_as_float(_agu_u0()), ss1_neg);
						      MAC_F32(im2, _as_float(_agu_u2()), cc1_neg);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

						re3 = MPY_F32(_as_float(_agu_u0()), cc3_neg);
						      MAC_F32(re3, _as_float(_agu_u1()), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
						im3 = MPY_F32(_as_float(_agu_u0()), ss3_neg);
						      MAC_F32(im3, _as_float(_agu_u3()), cc3_neg);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

						tre2 = ADD_F32(re2, re3);
						tim2 = ADD_F32(im2, im3);
						tre3 = SUB_F32(re2, re3);
						tim3 = SUB_F32(im2, im3);

						_st_agu_u0( _as_uint32(SUB_F32( re0, tre2 )) );
						_st_agu_u2( _as_uint32(SUB_F32( im0, tim2 )) );
						_st_agu_u0( _as_uint32(ADD_F32( re1, tim3 )) );
						_st_agu_u2( _as_uint32(SUB_F32( im1, tre3 )) );
						_st_agu_u0( _as_uint32(ADD_F32( re0, tre2 )) );
						_st_agu_u2( _as_uint32(ADD_F32( im0, tim2 )) );
						_st_agu_u0( _as_uint32(SUB_F32( re1, tim3 )) );
						_st_agu_u4( _as_uint32(ADD_F32( im1, tre3 )) );
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

		_set_agu_os0(N4_cplx - 1);
		_set_agu_os1(N4_cplx*3 + 1);
		_set_agu_os2(N4_cplx*3 - 1);
		_set_agu_os3(d1*2+1);

		_set_agu_ap0(output);
		_set_agu_ap1(handle->ptwd + 1);
		_set_agu_ap2(handle->ptwd3 + 1);
		for (l = 0; l < l_cnt; l++) {
				re0 = _as_float(_agu_u0()); //COMPLEX_RE(input, i0)
				im0 = _as_float(_agu_u2()); //COMPLEX_IM(input, i0)
				re1 = _as_float(_agu_u0()); //COMPLEX_RE(input, i1)
				im1 = _as_float(_agu_u2()); //COMPLEX_IM(input, i1)

				ss1 = -(_as_float(_agu_u6())); 
				ss3 = -(_as_float(_agu_u9()));

				re2 = MPY_F32( _as_float(_agu_u0()), _as_float(_agu_u5()) );
				      MAC_F32( re2, _as_float(_agu_u1()), ss1);         // int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1 )>>31);
				im2 = MPY_F32( _as_float(_agu_u0()), _as_float(_agu_u6()) );
				      MAC_F32( im2, _as_float(_agu_u2()), _as_float(_agu_u7()) );   // int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1 )>>31);

				re3 = MPY_F32( _as_float(_agu_u0()), _as_float(_agu_u8()) );
				      MAC_F32( re3, _as_float(_agu_u1()), ss3);         // int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3 )>>31);
				im3 = MPY_F32( _as_float(_agu_u0()), _as_float(_agu_u9()) );
				      MAC_F32( im3, _as_float(_agu_u3()), _as_float(_agu_u10()) );  // int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3 )>>31);

				tre2 = ADD_F32(re2, re3) ;
				tim2 = ADD_F32(im2, im3) ;
				tre3 = SUB_F32(re2, re3) ;
				tim3 = SUB_F32(im2, im3) ;

				_st_agu_u0( _as_uint32(SUB_F32( re0, tre2 )) );
				_st_agu_u2( _as_uint32(SUB_F32( im0, tim2 )) );
				_st_agu_u0( _as_uint32(ADD_F32( re1, tim3 )) );
				_st_agu_u2( _as_uint32(SUB_F32( im1, tre3 )) );
				_st_agu_u0( _as_uint32(ADD_F32( re0, tre2 )) );
				_st_agu_u2( _as_uint32(ADD_F32( im0, tim2 )) );
				_st_agu_u0( _as_uint32(SUB_F32( re1, tim3 )) );
				_st_agu_u4( _as_uint32(ADD_F32( im1, tre3 )) );
		}
        N4_cplx <<= 1;
    }
#endif
	_DSPLIB_restore_regs();
}

#else

void risc_dsp_cifft_sradix_dit_f32(const cfft_f32_t* handle, const f32_t* input, f32_t* output)
{
    int32_t i, k, l;
    int32_t nLength = handle->length;
    int32_t N4_cplx;
    int32_t nOrder = 30 - dspfx_norm_q31(handle->length);

    f32_t cc1_neg, ss1, ss1_neg, cc3_neg, ss3_neg, ss3;
    f32_t re0, im0, re1, im1, re2, im2, re3, im3;
    f32_t tre2, tre3;
    f32_t tim2, tim3;

    int32_t ix, id, ido;
    f32_t *pI0, *pI1, *pI2, *pI3;

    if (input == output) {
        // in place bit-reverse shuffle
        for (i = 0; i < nLength; i++)
        {
            int32_t i_rev = handle->tab_bit_rev[i];
            if (i < i_rev) {
                f32_t tmp_re = COMPLEX_RE(output, i_rev);
                f32_t tmp_im = COMPLEX_IM(output, i_rev);
                COMPLEX_RE(output, i_rev) = COMPLEX_RE(input, i);
                COMPLEX_IM(output, i_rev) = COMPLEX_IM(input, i);
                COMPLEX_RE(output, i) = tmp_re;
                COMPLEX_IM(output, i) = tmp_im;
            }
        }
    } else {
        // out of place bit-reverse shuffle
        int16_t* tab_bit_rev = handle->tab_bit_rev;
        const f32_t* pInput = input;
        for (i = 0; i < nLength; i++)
        {
            f32_t *pOutput = output + tab_bit_rev[i]*2;
            *pOutput++ = *pInput++;
            *pOutput   = *pInput++;
        }
    }

    // stage: nOrder - 1
    // baseop: radix-2 with trivial multiply
    ix = 1;
    id = 4;
    ido = 2;
    while (ix < nLength) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
        pI0 = output + ix*2 - 2;
            for (i = 0; i<i_cnt; i++, pI0 += id*2) {
                re0 = pI0[0];
                im0 = pI0[1];
                re1 = pI0[2];
                im1 = pI0[3];
                pI0[0] = ADD_F32( re0, re1);
                pI0[1] = ADD_F32( im0, im1);
                pI0[2] = SUB_F32( re0, re1);
                pI0[3] = SUB_F32( im0, im1);
            }
        ix = 2 * id - 1;
        id = 4 * id; ido +=2;
    }


    // stage: nOrder - 2
    // baseop: split radix with trivial multiply
    ix = 0;
    id = 8;
    ido = 3;
    while (ix < nLength - 1) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
        pI0 = output + ix*2;

            for (i = 0; i<i_cnt; i++) {
                re0 = pI0[0]; //COMPLEX_RE(input, i0)
                im0 = pI0[1]; //COMPLEX_IM(input, i0)
                re1 = pI0[2]; //COMPLEX_RE(input, i1)
                im1 = pI0[3]; //COMPLEX_IM(input, i1)
                tre2 = pI0[4]; //COMPLEX_RE(input, i2)
                tim2 = pI0[5]; //COMPLEX_IM(input, i2)
                tre3 = pI0[6]; //COMPLEX_RE(input, i3)
                tim3 = pI0[7]; //COMPLEX_IM(input, i3)

                re2 = ADD_F32( tre2, tre3);
                im2 = ADD_F32( tim2, tim3);
                re3 = SUB_F32( tre2, tre3);
                im3 = SUB_F32( tim2, tim3);

                pI0[0] = ADD_F32( re0, re2);
                pI0[1] = ADD_F32( im0, im2);
                pI0[2] = SUB_F32( re1, im3);
                pI0[3] = ADD_F32( im1, re3);
                pI0[4] = SUB_F32( re0, re2);
                pI0[5] = SUB_F32( im0, im2);
                pI0[6] = ADD_F32( re1, im3);
                pI0[7] = SUB_F32( im1, re3);

                pI0 += id*2;
            }
        ix = 2 * id - 4;
        id = 4 * id; ido +=2;
    }

    // stage: [(nOrder-3) .. 2]
    // baseop: split-radix
    N4_cplx = 4; // N4 = N / 4
    for (k = nOrder-3; k >= 2 ; k--) {
        f32_t *pTWD1 = handle->ptwd;
        f32_t *pTWD3 = handle->ptwd3;
        int32_t d1 = 2<<k;
        int32_t l_cnt = (N4_cplx>>1);

        for (l = 0; l < l_cnt; l++, pTWD1+=d1, pTWD3+=d1) {
            int32_t ix = l;
            int32_t id = N4_cplx*4;
            int32_t ido = nOrder - k + 1;

            cc1_neg = *pTWD1;
            cc3_neg = *pTWD3;
            ss1_neg = *(pTWD1+1);
            ss3_neg = *(pTWD3+1);
            ss1 = -(ss1_neg); 
            ss3 = -(ss3_neg);

            while (ix < nLength - 1) {
                int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
                pI0 = output + ix*2;

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

                        re2 = MPY_F32(tre2, cc1_neg);
                              MAC_F32(re2, tim2, ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                        im2 = MPY_F32(tim2, cc1_neg);
                              MAC_F32(im2, tre2, ss1_neg); //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                        re3 = MPY_F32(tre3, cc3_neg);
                              MAC_F32(re3, tim3, ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                        im3 = MPY_F32(tim3, cc3_neg);
                              MAC_F32(im3, tre3, ss3_neg);  //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = ADD_F32( re2, re3);
                        tim2 = ADD_F32( im2, im3);
                        tre3 = SUB_F32( re2, re3);
                        tim3 = SUB_F32( im2, im3);

                        pI0[0] = SUB_F32( re0, tre2);
                        pI0[1] = SUB_F32( im0, tim2);
                        pI1[0] = ADD_F32( re1, tim3);
                        pI1[1] = SUB_F32( im1, tre3);
                        pI2[0] = ADD_F32( re0, tre2);
                        pI2[1] = ADD_F32( im0, tim2);
                        pI3[0] = SUB_F32( re1, tim3);
                        pI3[1] = ADD_F32( im1, tre3);
                        pI0 += id*2;
                    }
                ix = 2 * id - N4_cplx*2 + l;
                id = 4 * id; ido +=2;
            }
        }
        N4_cplx <<= 1;
    }

    for (k = dspfx_min_q31(1, nOrder-3); k >=0 ; k--) {
        f32_t *pTWD1 = handle->ptwd;
        f32_t *pTWD3 = handle->ptwd3;
        int32_t d1 = 2 << k;
        int32_t l_cnt = (N4_cplx>>1);
        pI0 = output;
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
                ss1 = -(ss1_neg); 
                ss3 = -(ss3_neg);

                re2 = MPY_F32(tre2, cc1_neg);
                      MAC_F32(re2, tim2, ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                im2 = MPY_F32(tim2, cc1_neg);
                      MAC_F32(im2, tre2, ss1_neg); //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                re3 = MPY_F32(tre3, cc3_neg);
                      MAC_F32(re3, tim3, ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                im3 = MPY_F32(tim3, cc3_neg);
                      MAC_F32(im3, tre3, ss3_neg);  //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = ADD_F32( re2, re3);
                tre3 = SUB_F32( re2, re3);
                tim2 = ADD_F32( im2, im3);
                tim3 = SUB_F32( im2, im3);

                pI0[0] = SUB_F32( re0, tre2);
                pI0[1] = SUB_F32( im0, tim2);
                pI1[0] = ADD_F32( re1, tim3);
                pI1[1] = SUB_F32( im1, tre3);
                pI2[0] = ADD_F32( re0, tre2);
                pI2[1] = ADD_F32( im0, tim2);
                pI3[0] = SUB_F32( re1, tim3);
                pI3[1] = ADD_F32( im1, tre3);
            }
        N4_cplx <<= 1;
    }
}

#endif

#if !defined(EXCLUDE_DSP_CIFFT_4_UNSCALED_F32)
void dsp_cifft_4_unscaled_f32(const cfft_f32_t* handle, const f32_t* input, f32_t* output){
    f32_t t0r, t0i;
    f32_t t1r, t1i;
    f32_t t2r, t2i;
    f32_t t3r, t3i;
    
    t0r = ADD_F32( input[0], input[4]);
    t0i = ADD_F32( input[1], input[5]);
    
    t1r = ADD_F32( input[2], input[6]);
    t1i = ADD_F32( input[3], input[7]);
    
    t2r = SUB_F32( input[0], input[4]);
    t2i = SUB_F32( input[1], input[5]);
    // multiply by w1 (j)
    t3r = SUB_F32( input[7], input[3]);
    t3i = SUB_F32( input[2], input[6]);
    
    output[0] = ADD_F32( t0r, t1r);
    output[1] = ADD_F32( t0i, t1i);
    
    output[4] = SUB_F32( t0r, t1r);
    output[5] = SUB_F32( t0i, t1i);    
    
    output[2] = ADD_F32( t2r, t3r);
    output[3] = ADD_F32( t2i, t3i);
    
    output[6] = SUB_F32( t2r, t3r);
    output[7] = SUB_F32( t2i, t3i);
}
#endif

#if !defined(EXCLUDE_DSP_CIFFT_8_UNSCALED_F32)
void dsp_cifft_8_unscaled_f32(const cfft_f32_t* handle, const f32_t* input, f32_t* output){
    const f32_t cos_pi_4 = 0.7071067811; // cos(pi/4) in F32
    f32_t t0r, t0i;
    f32_t t1r, t1i;
    f32_t t2r, t2i;
    f32_t t3r, t3i;
    f32_t t4r, t4i;
    f32_t t5r, t5i;
    
    f32_t t0r_sub, t0i_sub;
    f32_t t1r_sub, t1i_sub;
    f32_t t2r_sub, t2i_sub;
    f32_t t3r_sub, t3i_sub;
    
    // Node with sums (upper 4 compex inputs)
    t0r = ADD_F32( input[0], input[8]);
    t0i = ADD_F32( input[1], input[9]);
    
    t1r = ADD_F32( input[2], input[10]);
    t1i = ADD_F32( input[3], input[11]);
        
    t2r = ADD_F32( input[4], input[12]);
    t2i = ADD_F32( input[5], input[13]);
    
    t3r = ADD_F32( input[6], input[14]);
    t3i = ADD_F32( input[7], input[15]);
    
    // Node with subs (lower 4 compex inputs)
    t0r_sub = SUB_F32( input[0], input[8]);
    t0i_sub = SUB_F32( input[1], input[9]);
    
    t1r_sub = SUB_F32( input[2], input[10]);
    t1i_sub = SUB_F32( input[3], input[11]);
    
    // multiply by w1 (0.707 + 0.707j)
    t4r = MPY_F32(t1r_sub, cos_pi_4);
    t4i = MPY_F32(t1i_sub, cos_pi_4);    
    
    t1r_sub = SUB_F32( t4r, t4i);
    t1i_sub = ADD_F32( t4i, t4r);
    
    // multiply by w2 (j)
    t2r_sub = SUB_F32( input[13], input[5]);
    t2i_sub = SUB_F32( input[4], input[12]);
    
    t3r_sub = SUB_F32( input[6], input[14]);
    t3i_sub = SUB_F32( input[15], input[7]); // * (-1)
    
    // multiply by w3 (-0.707 + 0.707j)
    t4r = MPY_F32(t3r_sub, cos_pi_4);
    t4i = MPY_F32(t3i_sub, cos_pi_4);    
    
    t3r_sub = SUB_F32( t4i, t4r);
    t3i_sub = ADD_F32( t4r, t4i);
    
    // second and third stage
    t4r = ADD_F32( t0r, t2r);
    t4i = ADD_F32( t0i, t2i);
    
    t5r = ADD_F32( t1r, t3r);
    t5i = ADD_F32( t1i, t3i);
    
    output[0] = ADD_F32( t4r, t5r);
    output[1] = ADD_F32( t4i, t5i);
    
    output[8] = SUB_F32( t4r, t5r);
    output[9] = SUB_F32( t4i, t5i);
    
    t4r = SUB_F32( t0r, t2r);
    t4i = SUB_F32( t0i, t2i);
    
    // multiply by j
    t5r = SUB_F32( t3i, t1i);
    t5i = SUB_F32( t1r, t3r);
    
    output[4] = ADD_F32( t4r, t5r);
    output[5] = ADD_F32( t4i, t5i);
    
    output[12] = SUB_F32( t4r, t5r);
    output[13] = SUB_F32( t4i, t5i);
        
    // second and third stage
    t4r = ADD_F32( t0r_sub, t2r_sub);
    t4i = ADD_F32( t0i_sub, t2i_sub);
    
    t5r = ADD_F32( t1r_sub, t3r_sub);
    t5i = ADD_F32( t1i_sub, t3i_sub);
    
    output[2] = ADD_F32( t4r, t5r);
    output[3] = ADD_F32( t4i, t5i);
    
    output[10] = SUB_F32( t4r, t5r);
    output[11] = SUB_F32( t4i, t5i);
    
    t4r = SUB_F32( t0r_sub, t2r_sub);
    t4i = SUB_F32( t0i_sub, t2i_sub);
    
    // multiply by j
    t5r = SUB_F32( t3i_sub, t1i_sub);
    t5i = SUB_F32( t1r_sub, t3r_sub);
    
    output[6] = ADD_F32( t4r, t5r);
    output[7] = ADD_F32( t4i, t5i);
    
    output[14] = SUB_F32( t4r, t5r);
    output[15] = SUB_F32( t4i, t5i);
}
#endif

#define XY_MAP_INPUT (((XY_REGION_SIZE/2))/sizeof(f32_t))
#define XY_MAP_OUT 0
#define XY_MAP_TWIDDLES 0
#define XY_IFFT_RADIX2_F32_THRESHOLD (((XY_REGION_SIZE/4))/sizeof(f32_t))

/**
 * Inverse Complex FFT "process" function for f32 data type
 *
 * \param handle - pointer to transform instance structure
 * \param[in] input - pointer to input vector. Vector length: defined by transform size
 * \param[out] output - pointer to output vector. Vector length: defined by transform size
 */

#pragma Off_inline(dsp_cifft_4_unscaled_f32, dsp_cifft_8_unscaled_f32)

void _DSPLIB_XYCC dsp_cifft_f32(const cfft_f32_t* handle, const PTR(f32_t) input, PTR(f32_t) output)
{

#if defined(DSP_CFFT_F32_4)
    if ( handle->length == 4 ) {
        dsp_cifft_4_unscaled_f32(handle, input, output);
    } else
#endif
#if defined(DSP_CFFT_F32_8)
    if ( handle->length == 8 ) {
        dsp_cifft_8_unscaled_f32(handle, input, output);
    } else
#endif
    {
        risc_dsp_cifft_sradix_dit_f32(handle, (f32_t*) input, output);
    }

    if (handle->scaling == DSP_TRANSFORM_SCALING_SCALED)
    {
        dsp_scale_f32( output,(1.0/handle->length) ,output, 2*handle->length);
    }
}

void dsp_cifft_delete_f32(cfft_f32_t* handle) {};

#endif //!defined(EXCLUDE_DSP_CIFFT_F32)

