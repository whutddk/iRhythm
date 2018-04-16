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
#include "dsp_transform_tables.h"

/* Split-radix decimation in time base-operation combines radix-4 and radix-2 base-operations

              x[i+1,0] ------- x[i+2,0]
                        \   /
                         \ /
              x[i+1,1] ------- x[i+2,0]
                        \/ \/  
                        /\ /\
    x[i,2] w1 -------  ------- x[i+2,0]
                \ /      / \-1
                / \     /   \
    x[i,3] w3 -------  ------- x[i+2,0]
                  -1   j    -1

                  
                  
    Overall dit split-radix algorithm is decomposed into separate stages:
    0. Bit-reverse reordering that is implemented in-place for X to X algorithm
    1. Initial stage of radix-2 base ops combined with bit-reverse reordering (in case of out-of-place i/o data location)
           x[0,2]  -------  x[1,0]
                     \ /   
                     / \   
           x[0,3]  -------  x[1,0]
                        -1 
                        
    2. Split-radix simplified stage (without radix-2 that is processed on 1-st)
            x[1,0] ------- x[2,0]
                    \   /
                     \ /
            x[1,1] ------- x[2,0]
                    \/ \/  
                    /\ /\
            x[1,2] ------- x[2,0]
                     / \-1
                    /   \
            x[1,3] ------- x[2,0]
                        -1
    
    3. Full split radix stages 
    4. Last two split radix stages (that can be simplified in comparision with 3-rd)

*/

// Produce instance with: MAC32x32; pIn0 - X; 

#pragma Code("dsplib_dsp_cifft_sradix_q31_XtoX_cfY_MAC32x32")

/**
 *  split-radix Inverse Complex FFT "process" function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - X memory offset to input vector
 * \param offs_output[out] - X memory offset to output vector
 */

void dsp_cifft_sradix_q31_XtoX_cfY_MAC32x32(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i, k, l;
    int32_t nLength = inst->length;
    int32_t nLength2 = nLength/2;
    int32_t N4_cplx, N = nLength;
    int32_t nOrder = 30 - dspfx_norm_q31(inst->length);
    int32_t unitShiftPrev, unitShift, shiftBits = inst->shiftBits;
   
    q31_t cc1_neg, ss1, ss1_neg, cc3_neg, ss3_neg, ss3;
    q31_t re0, im0, re1, im1, re2, im2, re3, im3;
    q31_t tre2, tre3;
    q31_t tim2, tim3;

    int32_t ix, id, ido;


    
    // Set of bit-reverse addressing
    _set_ax0((offs_input));
    _set_mx00((AM_BITREV|nLength));
    _set_ax1((offs_input + 1));
    _set_mx10((AM_BITREV|nLength));
    _set_ax2((offs_output));
    _set_mx20((1));
    _set_mx21((AM_OFFS(-1)));

    // Process inplace bitreverse reordering
    if (offs_input == offs_output)
    {
        int32_t outaddr = offs_input;
        for (i = 0; i < nLength; i++)
        {
            int32_t revaddr = _ax0();
            int32_t temp_real = _x2_u0();
            int32_t temp_imag = _x2_u1();
    #ifdef _ARC
            __asm__(
                "        cmp      %[revaddr], %[outaddr]               \n"\
                "        mov.gt   %x2_u0, %x0_nu        \n"\
                "        mov.gt   %x2_u0, %x1_nu        \n"\
                "        mov.gt   %x0_u0, %[temp_real]          \n"\
                "        mov.gt   %x1_u0, %[temp_imag]          \n"
                : : [revaddr]"r"(revaddr), [outaddr]"r"(outaddr), [temp_real]"r"(temp_real), [temp_imag]"r"(temp_imag) :
            );
    #else
            if (revaddr > outaddr) 
            {
                _set_x2_u0(_x0_nu());
                _set_x2_u0(_x1_nu());
                _set_x0_u0(temp_real);
                _set_x1_u0(temp_imag);
            }
            else {
                _x0_u0();
                _x1_u0();
                _x2_u0();
                _x2_u0();
            }
    #endif
            outaddr += 2;
        }
        // Set linear addressing mode for input data
        _set_ax0((offs_input));
        _set_mx00((2));
        _set_ax1((offs_input + 1));
        _set_mx10((2));
    } 

    //FFT first stage with bit-reverse or linear addressing of input
    _set_ax2(offs_output);
    unitShift = (shiftBits-- > 0) ? 1 : 0;    
    if (unitShift) {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _x0_u0();
            im0 = _x1_u0();
            _set_x2_u0( dspfx_add_q31(re0, _x0_nu())>>1);
            _set_x2_u0( dspfx_add_q31(im0, _x1_nu())>>1);
            _set_x2_u0( dspfx_sub_q31(re0, _x0_u0())>>1);
            _set_x2_u0( dspfx_sub_q31(im0, _x1_u0())>>1);
        }
    } else {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _x0_u0();
            im0 = _x1_u0();
            _set_x2_u0( dspfx_add_q31(re0, _x0_nu()));
            _set_x2_u0( dspfx_add_q31(im0, _x1_nu()));
            _set_x2_u0( dspfx_sub_q31(re0, _x0_u0()));
            _set_x2_u0( dspfx_sub_q31(im0, _x1_u0()));
        }
    }

    // stage: nOrder - 2
    // baseop: split radix with trivial multiply
    ix = 0;    id = 8;    ido = 3;
    unitShift = (shiftBits-- > 0) ? 1 : 0;
    while (ix < nLength - 1) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
        _set_ax2(offs_output + ix*2);
        _set_mx20(1);
        _set_mx21(id*2 - 4 + 1);
        _set_ax3(offs_output + ix*2+4);
        _set_mx30(1);
        _set_mx31(id*2 - 4 + 1);
        _set_ax0(offs_output + ix*2);
        _set_mx00(1);
        _set_mx01(id*2 - 4 + 1);
        _set_ax1(offs_output + ix*2+4);
        _set_mx10(1);
        _set_mx11(id*2 - 4 + 1);
        //pI0 = output + ix*2;
        if (unitShift) {
            for (i = 0; i<i_cnt; i++) {
                re2 = _x1_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _x1_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _x1_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _x1_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_x3_u0( dspfx_sub_q31(_x0_nu(), re2)>>1);
                _set_x2_u0( dspfx_add_q31(_x0_u0(), re2)>>1);
                _set_x3_u0( dspfx_sub_q31(_x0_nu(), im2)>>1);
                _set_x2_u0( dspfx_add_q31(_x0_u0(), im2)>>1);
                _set_x3_u0( dspfx_add_q31(_x0_nu(), im3)>>1);
                _set_x2_u0( dspfx_sub_q31(_x0_u0(), im3)>>1);
                _set_x3_u1( dspfx_sub_q31(_x0_nu(), re3)>>1);
                _set_x2_u1( dspfx_add_q31(_x0_u1(), re3)>>1);
            }
        } else {
            for (i = 0; i<i_cnt; i++) {
                re2 = _x1_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _x1_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _x1_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _x1_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_x3_u0( dspfx_sub_q31(_x0_nu(), re2));
                _set_x2_u0( dspfx_add_q31(_x0_u0(), re2));
                _set_x3_u0( dspfx_sub_q31(_x0_nu(), im2));
                _set_x2_u0( dspfx_add_q31(_x0_u0(), im2));
                _set_x3_u0( dspfx_add_q31(_x0_nu(), im3));
                _set_x2_u0( dspfx_sub_q31(_x0_u0(), im3));
                _set_x3_u1( dspfx_sub_q31(_x0_nu(), re3));
                _set_x2_u1( dspfx_add_q31(_x0_u1(), re3));
            }
        }
        ix = 2 * id - 4;
        id = 4 * id; ido +=2;
    }

    // stage: [(nOrder-2) .. 2]
    // baseop: split-radix
    N4_cplx = 4; // N4 = N / 4
    for (k = nOrder-3; k >= 2 ; k--) 
    {
        int32_t d1 = 1<<k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ay0((inst->offs_twiddle));
        _set_my00((1));
        _set_my01((d1*2 - 1));
        _set_ay2((inst->offs_twiddle + nLength2));
        _set_my20((1));
        _set_my21((d1*2 - 1));

        for (l = 0; l < l_cnt; l++) //pCos1+=d1, pSin1+=d1, pCos3+=d3, pSin3+=d3
        {
            int32_t ix = l;
            int32_t id = N4_cplx*4;
            int32_t ido = nOrder - k + 1;

            cc1_neg = _y0_u0();
            ss1 = _y0_u1();
            cc3_neg = _y2_u0();
            ss3 = _y2_u1();
            ss1_neg = dspfx_neg_q31(ss1); 
            ss3_neg = dspfx_neg_q31(ss3);

            while (ix < nLength - 1) 
            {
                int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
                // pI0 = output + ix*2;
                _set_ax0((offs_output + ix*2));
                _set_mx00((N4_cplx));
                _set_mx01((id*2 - N4_cplx * 3));
                _set_ax1((offs_output + ix*2 + 1));
                _set_mx10((N4_cplx));
                _set_mx11((id*2 - N4_cplx * 3));
                _set_ax2((offs_output + ix*2));
                _set_mx20((N4_cplx));
                _set_mx21((id*2 - N4_cplx * 3));
                _set_ax3((offs_output + ix*2 + 1));
                _set_mx30((N4_cplx));
                _set_mx31((id*2 - N4_cplx * 3));
                if (unitShiftPrev || unitShift) {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _x0_u0(); //COMPLEX_RE(input, i0)
                        im0 = _x1_u0(); //COMPLEX_IM(input, i0)
                        re1 = _x0_u0(); //COMPLEX_RE(input, i1)
                        im1 = _x1_u0(); //COMPLEX_IM(input, i1)

                               _mulflw(_x0_nu(), cc1_neg);
                              _machflw(_x0_nu(), cc1_neg);
                               _macflw(_x1_nu(), ss1_neg);
                        re2 = _machflw(_x1_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                               _mulflw(_x1_nu(), cc1_neg);
                              _machflw(_x1_u0(), cc1_neg);
                               _macflw(_x0_nu(), ss1);
                        im2 = _machflw(_x0_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                               _mulflw(_x0_nu(), cc3_neg);
                              _machflw(_x0_nu(), cc3_neg);
                               _macflw(_x1_nu(), ss3_neg);
                        re3 = _machflw(_x1_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                               _mulflw(_x1_nu(), cc3_neg);
                              _machflw(_x1_u1(), cc3_neg);
                               _macflw(_x0_nu(), ss3);
                        im3 = _machflw(_x0_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                        tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                        tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                        tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                        _set_x2_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                        _set_x3_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                        _set_x2_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                        _set_x3_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                        _set_x2_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                        _set_x3_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                        _set_x2_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                        _set_x3_u1( dspfx_add_q31(im1, tre3) >> unitShift);
                        //pI0 += id*2;
                    }
                } else {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _x0_u0(); //COMPLEX_RE(input, i0)
                        im0 = _x1_u0(); //COMPLEX_IM(input, i0)
                        re1 = _x0_u0(); //COMPLEX_RE(input, i1)
                        im1 = _x1_u0(); //COMPLEX_IM(input, i1)

                               _mulflw(_x0_nu(), cc1_neg);
                              _machflw(_x0_nu(), cc1_neg);
                               _macflw(_x1_nu(), ss1_neg);
                        re2 = _machflw(_x1_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                               _mulflw(_x1_nu(), cc1_neg);
                              _machflw(_x1_u0(), cc1_neg);
                               _macflw(_x0_nu(), ss1);
                        im2 = _machflw(_x0_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                               _mulflw(_x0_nu(), cc3_neg);
                              _machflw(_x0_nu(), cc3_neg);
                               _macflw(_x1_nu(), ss3_neg);
                        re3 = _machflw(_x1_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                               _mulflw(_x1_nu(), cc3_neg);
                              _machflw(_x1_u1(), cc3_neg);
                               _macflw(_x0_nu(), ss3);
                        im3 = _machflw(_x0_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3);
                        tim2 = dspfx_add_q31(im2, im3);
                        tre3 = dspfx_sub_q31(re2, re3);
                        tim3 = dspfx_sub_q31(im2, im3);

                        _set_x2_u0( dspfx_sub_q31(re0, tre2));
                        _set_x3_u0( dspfx_sub_q31(im0, tim2));
                        _set_x2_u0( dspfx_add_q31(re1, tim3));
                        _set_x3_u0( dspfx_sub_q31(im1, tre3));
                        _set_x2_u0( dspfx_add_q31(re0, tre2));
                        _set_x3_u0( dspfx_add_q31(im0, tim2));
                        _set_x2_u1( dspfx_sub_q31(re1, tim3));
                        _set_x3_u1( dspfx_add_q31(im1, tre3));
                    }
                }
                ix = 2 * id - N4_cplx*2 + l;
                id = 4 * id; ido +=2;
            }
        }
        N4_cplx <<= 1;
    }
    
    for (k = dspfx_min_q31(1, nOrder-3); k >=0 ; k--) {
        int32_t d1 = 1 << k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ay0((inst->offs_twiddle));
        _set_my00((d1*2));
        _set_ay1((inst->offs_twiddle + 1));
        _set_my10((d1*2));
        _set_ay2((inst->offs_twiddle + nLength2));
        _set_my20((d1*2));
        _set_ay3((inst->offs_twiddle + nLength2 + 1));
        _set_my30((d1*2));
        _set_ax0((offs_output));
        _set_mx00((N4_cplx));
        _set_mx01((AM_OFFS(2 - N4_cplx * 3)));
        _set_ax1((offs_output + 1));
        _set_mx10((N4_cplx));
        _set_mx11((AM_OFFS(2 - N4_cplx * 3)));
        _set_ax2((offs_output));
        _set_mx20((N4_cplx));
        _set_mx21((AM_OFFS(2 - N4_cplx * 3)));
        _set_ax3((offs_output + 1));
        _set_mx30((N4_cplx));
        _set_mx31((AM_OFFS(2 - N4_cplx * 3)));
        if (unitShiftPrev || unitShift) {
            for (l = 0; l < l_cnt; l++) {
                re0 = _x0_u0(); //COMPLEX_RE(input, i0)
                im0 = _x1_u0(); //COMPLEX_IM(input, i0)
                re1 = _x0_u0(); //COMPLEX_RE(input, i1)
                im1 = _x1_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_y1_nu()); 
                ss3 = dspfx_neg_q31(_y3_nu());

                       _mulflw(_x0_nu(), _y0_nu());
                      _machflw(_x0_nu(), _y0_nu());
                       _macflw(_x1_nu(), ss1);
                re2 = _machflw(_x1_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                       _mulflw(_x1_nu(), _y0_nu());
                      _machflw(_x1_u0(), _y0_u0());
                       _macflw(_x0_nu(), _y1_nu());
                im2 = _machflw(_x0_u0(), _y1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                       _mulflw(_x0_nu(), _y2_nu());
                      _machflw(_x0_nu(), _y2_nu());
                       _macflw(_x1_nu(), ss3);
                re3 = _machflw(_x1_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                       _mulflw(_x1_nu(), _y2_nu());
                      _machflw(_x1_u1(), _y2_u0());
                       _macflw(_x0_nu(), _y3_nu());
                im3 = _machflw(_x0_u1(), _y3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                _set_x2_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                _set_x3_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                _set_x2_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                _set_x3_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                _set_x2_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                _set_x3_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                _set_x2_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                _set_x3_u1( dspfx_add_q31(im1, tre3) >> unitShift);
            }
        } else {
            for (l = 0; l < l_cnt; l++) {
                re0 = _x0_u0(); //COMPLEX_RE(input, i0)
                im0 = _x1_u0(); //COMPLEX_IM(input, i0)
                re1 = _x0_u0(); //COMPLEX_RE(input, i1)
                im1 = _x1_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_y1_nu()); 
                ss3 = dspfx_neg_q31(_y3_nu());

                       _mulflw(_x0_nu(), _y0_nu());
                      _machflw(_x0_nu(), _y0_nu());
                       _macflw(_x1_nu(), ss1);
                re2 = _machflw(_x1_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                       _mulflw(_x1_nu(), _y0_nu());
                      _machflw(_x1_u0(), _y0_u0());
                       _macflw(_x0_nu(), _y1_nu());
                im2 = _machflw(_x0_u0(), _y1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                       _mulflw(_x0_nu(), _y2_nu());
                      _machflw(_x0_nu(), _y2_nu());
                       _macflw(_x1_nu(), ss3);
                re3 = _machflw(_x1_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                       _mulflw(_x1_nu(), _y2_nu());
                      _machflw(_x1_u1(), _y2_u0());
                       _macflw(_x0_nu(), _y3_nu());
                im3 = _machflw(_x0_u1(), _y3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3);
                tim2 = dspfx_add_q31(im2, im3);
                tre3 = dspfx_sub_q31(re2, re3);
                tim3 = dspfx_sub_q31(im2, im3);

                _set_x2_u0( dspfx_sub_q31(re0, tre2));
                _set_x3_u0( dspfx_sub_q31(im0, tim2));
                _set_x2_u0( dspfx_add_q31(re1, tim3));
                _set_x3_u0( dspfx_sub_q31(im1, tre3));
                _set_x2_u0( dspfx_add_q31(re0, tre2));
                _set_x3_u0( dspfx_add_q31(im0, tim2));
                _set_x2_u1( dspfx_sub_q31(re1, tim3));
                _set_x3_u1( dspfx_add_q31(im1, tre3));
            }        
        }
        N4_cplx <<= 1;
    }
}
// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_cifft_sradix_q31_YtoY_cfX_MAC32x32")

/**
 *  split-radix Inverse Complex FFT "process" function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - Y memory offset to input vector
 * \param offs_output[out] - Y memory offset to output vector
 */

void dsp_cifft_sradix_q31_YtoY_cfX_MAC32x32(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i, k, l;
    int32_t nLength = inst->length;
    int32_t nLength2 = nLength/2;
    int32_t N4_cplx, N = nLength;
    int32_t nOrder = 30 - dspfx_norm_q31(inst->length);
    int32_t unitShiftPrev, unitShift, shiftBits = inst->shiftBits;
   
    q31_t cc1_neg, ss1, ss1_neg, cc3_neg, ss3_neg, ss3;
    q31_t re0, im0, re1, im1, re2, im2, re3, im3;
    q31_t tre2, tre3;
    q31_t tim2, tim3;

    int32_t ix, id, ido;


    
    // Set of bit-reverse addressing
    _set_ay0((offs_input));
    _set_my00((AM_BITREV|nLength));
    _set_ay1((offs_input + 1));
    _set_my10((AM_BITREV|nLength));
    _set_ay2((offs_output));
    _set_my20((1));
    _set_my21((AM_OFFS(-1)));

    // Process inplace bitreverse reordering
    if (offs_input == offs_output)
    {
        int32_t outaddr = offs_input;
        for (i = 0; i < nLength; i++)
        {
            int32_t revaddr = _ay0();
            int32_t temp_real = _y2_u0();
            int32_t temp_imag = _y2_u1();
    #ifdef _ARC
            __asm__(
                "        cmp      %[revaddr], %[outaddr]               \n"\
                "        mov.gt   %y2_u0, %y0_nu        \n"\
                "        mov.gt   %y2_u0, %y1_nu        \n"\
                "        mov.gt   %y0_u0, %[temp_real]          \n"\
                "        mov.gt   %y1_u0, %[temp_imag]          \n"
                : : [revaddr]"r"(revaddr), [outaddr]"r"(outaddr), [temp_real]"r"(temp_real), [temp_imag]"r"(temp_imag) :
            );
    #else
            if (revaddr > outaddr) 
            {
                _set_y2_u0(_y0_nu());
                _set_y2_u0(_y1_nu());
                _set_y0_u0(temp_real);
                _set_y1_u0(temp_imag);
            }
            else {
                _y0_u0();
                _y1_u0();
                _y2_u0();
                _y2_u0();
            }
    #endif
            outaddr += 2;
        }
        // Set linear addressing mode for input data
        _set_ay0((offs_input));
        _set_my00((2));
        _set_ay1((offs_input + 1));
        _set_my10((2));
    } 

    //FFT first stage with bit-reverse or linear addressing of input
    _set_ay2(offs_output);
    unitShift = (shiftBits-- > 0) ? 1 : 0;    
    if (unitShift) {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _y0_u0();
            im0 = _y1_u0();
            _set_y2_u0( dspfx_add_q31(re0, _y0_nu())>>1);
            _set_y2_u0( dspfx_add_q31(im0, _y1_nu())>>1);
            _set_y2_u0( dspfx_sub_q31(re0, _y0_u0())>>1);
            _set_y2_u0( dspfx_sub_q31(im0, _y1_u0())>>1);
        }
    } else {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _y0_u0();
            im0 = _y1_u0();
            _set_y2_u0( dspfx_add_q31(re0, _y0_nu()));
            _set_y2_u0( dspfx_add_q31(im0, _y1_nu()));
            _set_y2_u0( dspfx_sub_q31(re0, _y0_u0()));
            _set_y2_u0( dspfx_sub_q31(im0, _y1_u0()));
        }
    }

    // stage: nOrder - 2
    // baseop: split radix with trivial multiply
    ix = 0;    id = 8;    ido = 3;
    unitShift = (shiftBits-- > 0) ? 1 : 0;
    while (ix < nLength - 1) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
        _set_ay2(offs_output + ix*2);
        _set_my20(1);
        _set_my21(id*2 - 4 + 1);
        _set_ay3(offs_output + ix*2+4);
        _set_my30(1);
        _set_my31(id*2 - 4 + 1);
        _set_ay0(offs_output + ix*2);
        _set_my00(1);
        _set_my01(id*2 - 4 + 1);
        _set_ay1(offs_output + ix*2+4);
        _set_my10(1);
        _set_my11(id*2 - 4 + 1);
        //pI0 = output + ix*2;
        if (unitShift) {
            for (i = 0; i<i_cnt; i++) {
                re2 = _y1_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _y1_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _y1_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _y1_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_y3_u0( dspfx_sub_q31(_y0_nu(), re2)>>1);
                _set_y2_u0( dspfx_add_q31(_y0_u0(), re2)>>1);
                _set_y3_u0( dspfx_sub_q31(_y0_nu(), im2)>>1);
                _set_y2_u0( dspfx_add_q31(_y0_u0(), im2)>>1);
                _set_y3_u0( dspfx_add_q31(_y0_nu(), im3)>>1);
                _set_y2_u0( dspfx_sub_q31(_y0_u0(), im3)>>1);
                _set_y3_u1( dspfx_sub_q31(_y0_nu(), re3)>>1);
                _set_y2_u1( dspfx_add_q31(_y0_u1(), re3)>>1);
            }
        } else {
            for (i = 0; i<i_cnt; i++) {
                re2 = _y1_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _y1_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _y1_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _y1_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_y3_u0( dspfx_sub_q31(_y0_nu(), re2));
                _set_y2_u0( dspfx_add_q31(_y0_u0(), re2));
                _set_y3_u0( dspfx_sub_q31(_y0_nu(), im2));
                _set_y2_u0( dspfx_add_q31(_y0_u0(), im2));
                _set_y3_u0( dspfx_add_q31(_y0_nu(), im3));
                _set_y2_u0( dspfx_sub_q31(_y0_u0(), im3));
                _set_y3_u1( dspfx_sub_q31(_y0_nu(), re3));
                _set_y2_u1( dspfx_add_q31(_y0_u1(), re3));
            }
        }
        ix = 2 * id - 4;
        id = 4 * id; ido +=2;
    }

    // stage: [(nOrder-2) .. 2]
    // baseop: split-radix
    N4_cplx = 4; // N4 = N / 4
    for (k = nOrder-3; k >= 2 ; k--) 
    {
        int32_t d1 = 1<<k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ax0((inst->offs_twiddle));
        _set_mx00((1));
        _set_mx01((d1*2 - 1));
        _set_ax2((inst->offs_twiddle + nLength2));
        _set_mx20((1));
        _set_mx21((d1*2 - 1));

        for (l = 0; l < l_cnt; l++) //pCos1+=d1, pSin1+=d1, pCos3+=d3, pSin3+=d3
        {
            int32_t ix = l;
            int32_t id = N4_cplx*4;
            int32_t ido = nOrder - k + 1;

            cc1_neg = _x0_u0();
            ss1 = _x0_u1();
            cc3_neg = _x2_u0();
            ss3 = _x2_u1();
            ss1_neg = dspfx_neg_q31(ss1); 
            ss3_neg = dspfx_neg_q31(ss3);

            while (ix < nLength - 1) 
            {
                int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
                // pI0 = output + ix*2;
                _set_ay0((offs_output + ix*2));
                _set_my00((N4_cplx));
                _set_my01((id*2 - N4_cplx * 3));
                _set_ay1((offs_output + ix*2 + 1));
                _set_my10((N4_cplx));
                _set_my11((id*2 - N4_cplx * 3));
                _set_ay2((offs_output + ix*2));
                _set_my20((N4_cplx));
                _set_my21((id*2 - N4_cplx * 3));
                _set_ay3((offs_output + ix*2 + 1));
                _set_my30((N4_cplx));
                _set_my31((id*2 - N4_cplx * 3));
                if (unitShiftPrev || unitShift) {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _y0_u0(); //COMPLEX_RE(input, i0)
                        im0 = _y1_u0(); //COMPLEX_IM(input, i0)
                        re1 = _y0_u0(); //COMPLEX_RE(input, i1)
                        im1 = _y1_u0(); //COMPLEX_IM(input, i1)

                               _mulflw(_y0_nu(), cc1_neg);
                              _machflw(_y0_nu(), cc1_neg);
                               _macflw(_y1_nu(), ss1_neg);
                        re2 = _machflw(_y1_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                               _mulflw(_y1_nu(), cc1_neg);
                              _machflw(_y1_u0(), cc1_neg);
                               _macflw(_y0_nu(), ss1);
                        im2 = _machflw(_y0_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                               _mulflw(_y0_nu(), cc3_neg);
                              _machflw(_y0_nu(), cc3_neg);
                               _macflw(_y1_nu(), ss3_neg);
                        re3 = _machflw(_y1_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                               _mulflw(_y1_nu(), cc3_neg);
                              _machflw(_y1_u1(), cc3_neg);
                               _macflw(_y0_nu(), ss3);
                        im3 = _machflw(_y0_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                        tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                        tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                        tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                        _set_y2_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                        _set_y3_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                        _set_y2_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                        _set_y3_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                        _set_y2_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                        _set_y3_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                        _set_y2_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                        _set_y3_u1( dspfx_add_q31(im1, tre3) >> unitShift);
                        //pI0 += id*2;
                    }
                } else {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _y0_u0(); //COMPLEX_RE(input, i0)
                        im0 = _y1_u0(); //COMPLEX_IM(input, i0)
                        re1 = _y0_u0(); //COMPLEX_RE(input, i1)
                        im1 = _y1_u0(); //COMPLEX_IM(input, i1)

                               _mulflw(_y0_nu(), cc1_neg);
                              _machflw(_y0_nu(), cc1_neg);
                               _macflw(_y1_nu(), ss1_neg);
                        re2 = _machflw(_y1_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                               _mulflw(_y1_nu(), cc1_neg);
                              _machflw(_y1_u0(), cc1_neg);
                               _macflw(_y0_nu(), ss1);
                        im2 = _machflw(_y0_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                               _mulflw(_y0_nu(), cc3_neg);
                              _machflw(_y0_nu(), cc3_neg);
                               _macflw(_y1_nu(), ss3_neg);
                        re3 = _machflw(_y1_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                               _mulflw(_y1_nu(), cc3_neg);
                              _machflw(_y1_u1(), cc3_neg);
                               _macflw(_y0_nu(), ss3);
                        im3 = _machflw(_y0_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3);
                        tim2 = dspfx_add_q31(im2, im3);
                        tre3 = dspfx_sub_q31(re2, re3);
                        tim3 = dspfx_sub_q31(im2, im3);

                        _set_y2_u0( dspfx_sub_q31(re0, tre2));
                        _set_y3_u0( dspfx_sub_q31(im0, tim2));
                        _set_y2_u0( dspfx_add_q31(re1, tim3));
                        _set_y3_u0( dspfx_sub_q31(im1, tre3));
                        _set_y2_u0( dspfx_add_q31(re0, tre2));
                        _set_y3_u0( dspfx_add_q31(im0, tim2));
                        _set_y2_u1( dspfx_sub_q31(re1, tim3));
                        _set_y3_u1( dspfx_add_q31(im1, tre3));
                    }
                }
                ix = 2 * id - N4_cplx*2 + l;
                id = 4 * id; ido +=2;
            }
        }
        N4_cplx <<= 1;
    }
    
    for (k = dspfx_min_q31(1, nOrder-3); k >=0 ; k--) {
        int32_t d1 = 1 << k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ax0((inst->offs_twiddle));
        _set_mx00((d1*2));
        _set_ax1((inst->offs_twiddle + 1));
        _set_mx10((d1*2));
        _set_ax2((inst->offs_twiddle + nLength2));
        _set_mx20((d1*2));
        _set_ax3((inst->offs_twiddle + nLength2 + 1));
        _set_mx30((d1*2));
        _set_ay0((offs_output));
        _set_my00((N4_cplx));
        _set_my01((AM_OFFS(2 - N4_cplx * 3)));
        _set_ay1((offs_output + 1));
        _set_my10((N4_cplx));
        _set_my11((AM_OFFS(2 - N4_cplx * 3)));
        _set_ay2((offs_output));
        _set_my20((N4_cplx));
        _set_my21((AM_OFFS(2 - N4_cplx * 3)));
        _set_ay3((offs_output + 1));
        _set_my30((N4_cplx));
        _set_my31((AM_OFFS(2 - N4_cplx * 3)));
        if (unitShiftPrev || unitShift) {
            for (l = 0; l < l_cnt; l++) {
                re0 = _y0_u0(); //COMPLEX_RE(input, i0)
                im0 = _y1_u0(); //COMPLEX_IM(input, i0)
                re1 = _y0_u0(); //COMPLEX_RE(input, i1)
                im1 = _y1_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_x1_nu()); 
                ss3 = dspfx_neg_q31(_x3_nu());

                       _mulflw(_y0_nu(), _x0_nu());
                      _machflw(_y0_nu(), _x0_nu());
                       _macflw(_y1_nu(), ss1);
                re2 = _machflw(_y1_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                       _mulflw(_y1_nu(), _x0_nu());
                      _machflw(_y1_u0(), _x0_u0());
                       _macflw(_y0_nu(), _x1_nu());
                im2 = _machflw(_y0_u0(), _x1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                       _mulflw(_y0_nu(), _x2_nu());
                      _machflw(_y0_nu(), _x2_nu());
                       _macflw(_y1_nu(), ss3);
                re3 = _machflw(_y1_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                       _mulflw(_y1_nu(), _x2_nu());
                      _machflw(_y1_u1(), _x2_u0());
                       _macflw(_y0_nu(), _x3_nu());
                im3 = _machflw(_y0_u1(), _x3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                _set_y2_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                _set_y3_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                _set_y2_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                _set_y3_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                _set_y2_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                _set_y3_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                _set_y2_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                _set_y3_u1( dspfx_add_q31(im1, tre3) >> unitShift);
            }
        } else {
            for (l = 0; l < l_cnt; l++) {
                re0 = _y0_u0(); //COMPLEX_RE(input, i0)
                im0 = _y1_u0(); //COMPLEX_IM(input, i0)
                re1 = _y0_u0(); //COMPLEX_RE(input, i1)
                im1 = _y1_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_x1_nu()); 
                ss3 = dspfx_neg_q31(_x3_nu());

                       _mulflw(_y0_nu(), _x0_nu());
                      _machflw(_y0_nu(), _x0_nu());
                       _macflw(_y1_nu(), ss1);
                re2 = _machflw(_y1_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                       _mulflw(_y1_nu(), _x0_nu());
                      _machflw(_y1_u0(), _x0_u0());
                       _macflw(_y0_nu(), _x1_nu());
                im2 = _machflw(_y0_u0(), _x1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                       _mulflw(_y0_nu(), _x2_nu());
                      _machflw(_y0_nu(), _x2_nu());
                       _macflw(_y1_nu(), ss3);
                re3 = _machflw(_y1_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                       _mulflw(_y1_nu(), _x2_nu());
                      _machflw(_y1_u1(), _x2_u0());
                       _macflw(_y0_nu(), _x3_nu());
                im3 = _machflw(_y0_u1(), _x3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3);
                tim2 = dspfx_add_q31(im2, im3);
                tre3 = dspfx_sub_q31(re2, re3);
                tim3 = dspfx_sub_q31(im2, im3);

                _set_y2_u0( dspfx_sub_q31(re0, tre2));
                _set_y3_u0( dspfx_sub_q31(im0, tim2));
                _set_y2_u0( dspfx_add_q31(re1, tim3));
                _set_y3_u0( dspfx_sub_q31(im1, tre3));
                _set_y2_u0( dspfx_add_q31(re0, tre2));
                _set_y3_u0( dspfx_add_q31(im0, tim2));
                _set_y2_u1( dspfx_sub_q31(re1, tim3));
                _set_y3_u1( dspfx_add_q31(im1, tre3));
            }        
        }
        N4_cplx <<= 1;
    }
}
// Produce instance with: MAC24x24; pIn0 - X; 

#pragma Code("dsplib_dsp_cifft_sradix_q31_XtoX_cfY_MAC24x24")

/**
 *  split-radix Inverse Complex FFT "process" function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - X memory offset to input vector
 * \param offs_output[out] - X memory offset to output vector
 */

void dsp_cifft_sradix_q31_XtoX_cfY_MAC24x24(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i, k, l;
    int32_t nLength = inst->length;
    int32_t nLength2 = nLength/2;
    int32_t N4_cplx, N = nLength;
    int32_t nOrder = 30 - dspfx_norm_q31(inst->length);
    int32_t unitShiftPrev, unitShift, shiftBits = inst->shiftBits;
   
    q31_t cc1_neg, ss1, ss1_neg, cc3_neg, ss3_neg, ss3;
    q31_t re0, im0, re1, im1, re2, im2, re3, im3;
    q31_t tre2, tre3;
    q31_t tim2, tim3;

    int32_t ix, id, ido;


    
    // Set of bit-reverse addressing
    _set_ax0((offs_input));
    _set_mx00((AM_BITREV|nLength));
    _set_ax1((offs_input + 1));
    _set_mx10((AM_BITREV|nLength));
    _set_ax2((offs_output));
    _set_mx20((1));
    _set_mx21((AM_OFFS(-1)));

    // Process inplace bitreverse reordering
    if (offs_input == offs_output)
    {
        int32_t outaddr = offs_input;
        for (i = 0; i < nLength; i++)
        {
            int32_t revaddr = _ax0();
            int32_t temp_real = _x2_u0();
            int32_t temp_imag = _x2_u1();
    #ifdef _ARC
            __asm__(
                "        cmp      %[revaddr], %[outaddr]               \n"\
                "        mov.gt   %x2_u0, %x0_nu        \n"\
                "        mov.gt   %x2_u0, %x1_nu        \n"\
                "        mov.gt   %x0_u0, %[temp_real]          \n"\
                "        mov.gt   %x1_u0, %[temp_imag]          \n"
                : : [revaddr]"r"(revaddr), [outaddr]"r"(outaddr), [temp_real]"r"(temp_real), [temp_imag]"r"(temp_imag) :
            );
    #else
            if (revaddr > outaddr) 
            {
                _set_x2_u0(_x0_nu());
                _set_x2_u0(_x1_nu());
                _set_x0_u0(temp_real);
                _set_x1_u0(temp_imag);
            }
            else {
                _x0_u0();
                _x1_u0();
                _x2_u0();
                _x2_u0();
            }
    #endif
            outaddr += 2;
        }
        // Set linear addressing mode for input data
        _set_ax0((offs_input));
        _set_mx00((2));
        _set_ax1((offs_input + 1));
        _set_mx10((2));
    } 

    //FFT first stage with bit-reverse or linear addressing of input
    _set_ax2(offs_output);
    unitShift = (shiftBits-- > 0) ? 1 : 0;    
    if (unitShift) {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _x0_u0();
            im0 = _x1_u0();
            _set_x2_u0( dspfx_add_q31(re0, _x0_nu())>>1);
            _set_x2_u0( dspfx_add_q31(im0, _x1_nu())>>1);
            _set_x2_u0( dspfx_sub_q31(re0, _x0_u0())>>1);
            _set_x2_u0( dspfx_sub_q31(im0, _x1_u0())>>1);
        }
    } else {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _x0_u0();
            im0 = _x1_u0();
            _set_x2_u0( dspfx_add_q31(re0, _x0_nu()));
            _set_x2_u0( dspfx_add_q31(im0, _x1_nu()));
            _set_x2_u0( dspfx_sub_q31(re0, _x0_u0()));
            _set_x2_u0( dspfx_sub_q31(im0, _x1_u0()));
        }
    }

    // stage: nOrder - 2
    // baseop: split radix with trivial multiply
    ix = 0;    id = 8;    ido = 3;
    unitShift = (shiftBits-- > 0) ? 1 : 0;
    while (ix < nLength - 1) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
        _set_ax2(offs_output + ix*2);
        _set_mx20(1);
        _set_mx21(id*2 - 4 + 1);
        _set_ax3(offs_output + ix*2+4);
        _set_mx30(1);
        _set_mx31(id*2 - 4 + 1);
        _set_ax0(offs_output + ix*2);
        _set_mx00(1);
        _set_mx01(id*2 - 4 + 1);
        _set_ax1(offs_output + ix*2+4);
        _set_mx10(1);
        _set_mx11(id*2 - 4 + 1);
        //pI0 = output + ix*2;
        if (unitShift) {
            for (i = 0; i<i_cnt; i++) {
                re2 = _x1_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _x1_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _x1_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _x1_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_x3_u0( dspfx_sub_q31(_x0_nu(), re2)>>1);
                _set_x2_u0( dspfx_add_q31(_x0_u0(), re2)>>1);
                _set_x3_u0( dspfx_sub_q31(_x0_nu(), im2)>>1);
                _set_x2_u0( dspfx_add_q31(_x0_u0(), im2)>>1);
                _set_x3_u0( dspfx_add_q31(_x0_nu(), im3)>>1);
                _set_x2_u0( dspfx_sub_q31(_x0_u0(), im3)>>1);
                _set_x3_u1( dspfx_sub_q31(_x0_nu(), re3)>>1);
                _set_x2_u1( dspfx_add_q31(_x0_u1(), re3)>>1);
            }
        } else {
            for (i = 0; i<i_cnt; i++) {
                re2 = _x1_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _x1_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _x1_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _x1_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_x3_u0( dspfx_sub_q31(_x0_nu(), re2));
                _set_x2_u0( dspfx_add_q31(_x0_u0(), re2));
                _set_x3_u0( dspfx_sub_q31(_x0_nu(), im2));
                _set_x2_u0( dspfx_add_q31(_x0_u0(), im2));
                _set_x3_u0( dspfx_add_q31(_x0_nu(), im3));
                _set_x2_u0( dspfx_sub_q31(_x0_u0(), im3));
                _set_x3_u1( dspfx_sub_q31(_x0_nu(), re3));
                _set_x2_u1( dspfx_add_q31(_x0_u1(), re3));
            }
        }
        ix = 2 * id - 4;
        id = 4 * id; ido +=2;
    }

    _set_MACMODE(7UL<<16);
    // stage: [(nOrder-2) .. 2]
    // baseop: split-radix
    N4_cplx = 4; // N4 = N / 4
    for (k = nOrder-3; k >= 2 ; k--) 
    {
        int32_t d1 = 1<<k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ay0((inst->offs_twiddle));
        _set_my00((1));
        _set_my01((d1*2 - 1));
        _set_ay2((inst->offs_twiddle + nLength2));
        _set_my20((1));
        _set_my21((d1*2 - 1));

        for (l = 0; l < l_cnt; l++) //pCos1+=d1, pSin1+=d1, pCos3+=d3, pSin3+=d3
        {
            int32_t ix = l;
            int32_t id = N4_cplx*4;
            int32_t ido = nOrder - k + 1;

            cc1_neg = _y0_u0();
            ss1 = _y0_u1();
            cc3_neg = _y2_u0();
            ss3 = _y2_u1();
            ss1_neg = dspfx_neg_q31(ss1); 
            ss3_neg = dspfx_neg_q31(ss3);

            while (ix < nLength - 1) 
            {
                int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
                // pI0 = output + ix*2;
                _set_ax0((offs_output + ix*2));
                _set_mx00((N4_cplx));
                _set_mx01((id*2 - N4_cplx * 3));
                _set_ax1((offs_output + ix*2 + 1));
                _set_mx10((N4_cplx));
                _set_mx11((id*2 - N4_cplx * 3));
                _set_ax2((offs_output + ix*2));
                _set_mx20((N4_cplx));
                _set_mx21((id*2 - N4_cplx * 3));
                _set_ax3((offs_output + ix*2 + 1));
                _set_mx30((N4_cplx));
                _set_mx31((id*2 - N4_cplx * 3));
                if (unitShiftPrev || unitShift) {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _x0_u0(); //COMPLEX_RE(input, i0)
                        im0 = _x1_u0(); //COMPLEX_IM(input, i0)
                        re1 = _x0_u0(); //COMPLEX_RE(input, i1)
                        im1 = _x1_u0(); //COMPLEX_IM(input, i1)

                              _mulrt(_x0_nu(), cc1_neg);
                        re2 = _macrt(_x1_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                              _mulrt(_x1_u0(), cc1_neg);
                        im2 = _macrt(_x0_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                              _mulrt(_x0_nu(), cc3_neg);
                        re3 = _macrt(_x1_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                              _mulrt(_x1_u1(), cc3_neg);
                        im3 = _macrt(_x0_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                        tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                        tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                        tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                        _set_x2_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                        _set_x3_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                        _set_x2_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                        _set_x3_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                        _set_x2_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                        _set_x3_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                        _set_x2_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                        _set_x3_u1( dspfx_add_q31(im1, tre3) >> unitShift);
                        //pI0 += id*2;
                    }
                } else {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _x0_u0(); //COMPLEX_RE(input, i0)
                        im0 = _x1_u0(); //COMPLEX_IM(input, i0)
                        re1 = _x0_u0(); //COMPLEX_RE(input, i1)
                        im1 = _x1_u0(); //COMPLEX_IM(input, i1)

                              _mulrt(_x0_nu(), cc1_neg);
                        re2 = _macrt(_x1_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                              _mulrt(_x1_u0(), cc1_neg);
                        im2 = _macrt(_x0_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                              _mulrt(_x0_nu(), cc3_neg);
                        re3 = _macrt(_x1_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                              _mulrt(_x1_u1(), cc3_neg);
                        im3 = _macrt(_x0_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3);
                        tim2 = dspfx_add_q31(im2, im3);
                        tre3 = dspfx_sub_q31(re2, re3);
                        tim3 = dspfx_sub_q31(im2, im3);

                        _set_x2_u0( dspfx_sub_q31(re0, tre2));
                        _set_x3_u0( dspfx_sub_q31(im0, tim2));
                        _set_x2_u0( dspfx_add_q31(re1, tim3));
                        _set_x3_u0( dspfx_sub_q31(im1, tre3));
                        _set_x2_u0( dspfx_add_q31(re0, tre2));
                        _set_x3_u0( dspfx_add_q31(im0, tim2));
                        _set_x2_u1( dspfx_sub_q31(re1, tim3));
                        _set_x3_u1( dspfx_add_q31(im1, tre3));
                    }
                }
                ix = 2 * id - N4_cplx*2 + l;
                id = 4 * id; ido +=2;
            }
        }
        N4_cplx <<= 1;
    }
    
    for (k = dspfx_min_q31(1, nOrder-3); k >=0 ; k--) {
        int32_t d1 = 1 << k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ay0((inst->offs_twiddle));
        _set_my00((d1*2));
        _set_ay1((inst->offs_twiddle + 1));
        _set_my10((d1*2));
        _set_ay2((inst->offs_twiddle + nLength2));
        _set_my20((d1*2));
        _set_ay3((inst->offs_twiddle + nLength2 + 1));
        _set_my30((d1*2));
        _set_ax0((offs_output));
        _set_mx00((N4_cplx));
        _set_mx01((AM_OFFS(2 - N4_cplx * 3)));
        _set_ax1((offs_output + 1));
        _set_mx10((N4_cplx));
        _set_mx11((AM_OFFS(2 - N4_cplx * 3)));
        _set_ax2((offs_output));
        _set_mx20((N4_cplx));
        _set_mx21((AM_OFFS(2 - N4_cplx * 3)));
        _set_ax3((offs_output + 1));
        _set_mx30((N4_cplx));
        _set_mx31((AM_OFFS(2 - N4_cplx * 3)));
        if (unitShiftPrev || unitShift) {
            for (l = 0; l < l_cnt; l++) {
                re0 = _x0_u0(); //COMPLEX_RE(input, i0)
                im0 = _x1_u0(); //COMPLEX_IM(input, i0)
                re1 = _x0_u0(); //COMPLEX_RE(input, i1)
                im1 = _x1_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_y1_nu()); 
                ss3 = dspfx_neg_q31(_y3_nu());

                      _mulrt(_x0_nu(), _y0_nu());
                re2 = _macrt(_x1_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                      _mulrt(_x1_u0(), _y0_u0());
                im2 = _macrt(_x0_u0(), _y1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                      _mulrt(_x0_nu(), _y2_nu());
                re3 = _macrt(_x1_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                      _mulrt(_x1_u1(), _y2_u0());
                im3 = _macrt(_x0_u1(), _y3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                _set_x2_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                _set_x3_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                _set_x2_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                _set_x3_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                _set_x2_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                _set_x3_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                _set_x2_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                _set_x3_u1( dspfx_add_q31(im1, tre3) >> unitShift);
            }
        } else {
            for (l = 0; l < l_cnt; l++) {
                re0 = _x0_u0(); //COMPLEX_RE(input, i0)
                im0 = _x1_u0(); //COMPLEX_IM(input, i0)
                re1 = _x0_u0(); //COMPLEX_RE(input, i1)
                im1 = _x1_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_y1_nu()); 
                ss3 = dspfx_neg_q31(_y3_nu());

                      _mulrt(_x0_nu(), _y0_nu());
                re2 = _macrt(_x1_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                      _mulrt(_x1_u0(), _y0_u0());
                im2 = _macrt(_x0_u0(), _y1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                      _mulrt(_x0_nu(), _y2_nu());
                re3 = _macrt(_x1_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                      _mulrt(_x1_u1(), _y2_u0());
                im3 = _macrt(_x0_u1(), _y3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3);
                tim2 = dspfx_add_q31(im2, im3);
                tre3 = dspfx_sub_q31(re2, re3);
                tim3 = dspfx_sub_q31(im2, im3);

                _set_x2_u0( dspfx_sub_q31(re0, tre2));
                _set_x3_u0( dspfx_sub_q31(im0, tim2));
                _set_x2_u0( dspfx_add_q31(re1, tim3));
                _set_x3_u0( dspfx_sub_q31(im1, tre3));
                _set_x2_u0( dspfx_add_q31(re0, tre2));
                _set_x3_u0( dspfx_add_q31(im0, tim2));
                _set_x2_u1( dspfx_sub_q31(re1, tim3));
                _set_x3_u1( dspfx_add_q31(im1, tre3));
            }        
        }
        N4_cplx <<= 1;
    }
}
// Produce instance with: MAC24x24; pIn0 - Y; 

#pragma Code("dsplib_dsp_cifft_sradix_q31_YtoY_cfX_MAC24x24")

/**
 *  split-radix Inverse Complex FFT "process" function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - Y memory offset to input vector
 * \param offs_output[out] - Y memory offset to output vector
 */

void dsp_cifft_sradix_q31_YtoY_cfX_MAC24x24(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i, k, l;
    int32_t nLength = inst->length;
    int32_t nLength2 = nLength/2;
    int32_t N4_cplx, N = nLength;
    int32_t nOrder = 30 - dspfx_norm_q31(inst->length);
    int32_t unitShiftPrev, unitShift, shiftBits = inst->shiftBits;
   
    q31_t cc1_neg, ss1, ss1_neg, cc3_neg, ss3_neg, ss3;
    q31_t re0, im0, re1, im1, re2, im2, re3, im3;
    q31_t tre2, tre3;
    q31_t tim2, tim3;

    int32_t ix, id, ido;


    
    // Set of bit-reverse addressing
    _set_ay0((offs_input));
    _set_my00((AM_BITREV|nLength));
    _set_ay1((offs_input + 1));
    _set_my10((AM_BITREV|nLength));
    _set_ay2((offs_output));
    _set_my20((1));
    _set_my21((AM_OFFS(-1)));

    // Process inplace bitreverse reordering
    if (offs_input == offs_output)
    {
        int32_t outaddr = offs_input;
        for (i = 0; i < nLength; i++)
        {
            int32_t revaddr = _ay0();
            int32_t temp_real = _y2_u0();
            int32_t temp_imag = _y2_u1();
    #ifdef _ARC
            __asm__(
                "        cmp      %[revaddr], %[outaddr]               \n"\
                "        mov.gt   %y2_u0, %y0_nu        \n"\
                "        mov.gt   %y2_u0, %y1_nu        \n"\
                "        mov.gt   %y0_u0, %[temp_real]          \n"\
                "        mov.gt   %y1_u0, %[temp_imag]          \n"
                : : [revaddr]"r"(revaddr), [outaddr]"r"(outaddr), [temp_real]"r"(temp_real), [temp_imag]"r"(temp_imag) :
            );
    #else
            if (revaddr > outaddr) 
            {
                _set_y2_u0(_y0_nu());
                _set_y2_u0(_y1_nu());
                _set_y0_u0(temp_real);
                _set_y1_u0(temp_imag);
            }
            else {
                _y0_u0();
                _y1_u0();
                _y2_u0();
                _y2_u0();
            }
    #endif
            outaddr += 2;
        }
        // Set linear addressing mode for input data
        _set_ay0((offs_input));
        _set_my00((2));
        _set_ay1((offs_input + 1));
        _set_my10((2));
    } 

    //FFT first stage with bit-reverse or linear addressing of input
    _set_ay2(offs_output);
    unitShift = (shiftBits-- > 0) ? 1 : 0;    
    if (unitShift) {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _y0_u0();
            im0 = _y1_u0();
            _set_y2_u0( dspfx_add_q31(re0, _y0_nu())>>1);
            _set_y2_u0( dspfx_add_q31(im0, _y1_nu())>>1);
            _set_y2_u0( dspfx_sub_q31(re0, _y0_u0())>>1);
            _set_y2_u0( dspfx_sub_q31(im0, _y1_u0())>>1);
        }
    } else {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _y0_u0();
            im0 = _y1_u0();
            _set_y2_u0( dspfx_add_q31(re0, _y0_nu()));
            _set_y2_u0( dspfx_add_q31(im0, _y1_nu()));
            _set_y2_u0( dspfx_sub_q31(re0, _y0_u0()));
            _set_y2_u0( dspfx_sub_q31(im0, _y1_u0()));
        }
    }

    // stage: nOrder - 2
    // baseop: split radix with trivial multiply
    ix = 0;    id = 8;    ido = 3;
    unitShift = (shiftBits-- > 0) ? 1 : 0;
    while (ix < nLength - 1) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
        _set_ay2(offs_output + ix*2);
        _set_my20(1);
        _set_my21(id*2 - 4 + 1);
        _set_ay3(offs_output + ix*2+4);
        _set_my30(1);
        _set_my31(id*2 - 4 + 1);
        _set_ay0(offs_output + ix*2);
        _set_my00(1);
        _set_my01(id*2 - 4 + 1);
        _set_ay1(offs_output + ix*2+4);
        _set_my10(1);
        _set_my11(id*2 - 4 + 1);
        //pI0 = output + ix*2;
        if (unitShift) {
            for (i = 0; i<i_cnt; i++) {
                re2 = _y1_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _y1_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _y1_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _y1_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_y3_u0( dspfx_sub_q31(_y0_nu(), re2)>>1);
                _set_y2_u0( dspfx_add_q31(_y0_u0(), re2)>>1);
                _set_y3_u0( dspfx_sub_q31(_y0_nu(), im2)>>1);
                _set_y2_u0( dspfx_add_q31(_y0_u0(), im2)>>1);
                _set_y3_u0( dspfx_add_q31(_y0_nu(), im3)>>1);
                _set_y2_u0( dspfx_sub_q31(_y0_u0(), im3)>>1);
                _set_y3_u1( dspfx_sub_q31(_y0_nu(), re3)>>1);
                _set_y2_u1( dspfx_add_q31(_y0_u1(), re3)>>1);
            }
        } else {
            for (i = 0; i<i_cnt; i++) {
                re2 = _y1_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _y1_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _y1_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _y1_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_y3_u0( dspfx_sub_q31(_y0_nu(), re2));
                _set_y2_u0( dspfx_add_q31(_y0_u0(), re2));
                _set_y3_u0( dspfx_sub_q31(_y0_nu(), im2));
                _set_y2_u0( dspfx_add_q31(_y0_u0(), im2));
                _set_y3_u0( dspfx_add_q31(_y0_nu(), im3));
                _set_y2_u0( dspfx_sub_q31(_y0_u0(), im3));
                _set_y3_u1( dspfx_sub_q31(_y0_nu(), re3));
                _set_y2_u1( dspfx_add_q31(_y0_u1(), re3));
            }
        }
        ix = 2 * id - 4;
        id = 4 * id; ido +=2;
    }

    _set_MACMODE(7UL<<16);
    // stage: [(nOrder-2) .. 2]
    // baseop: split-radix
    N4_cplx = 4; // N4 = N / 4
    for (k = nOrder-3; k >= 2 ; k--) 
    {
        int32_t d1 = 1<<k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ax0((inst->offs_twiddle));
        _set_mx00((1));
        _set_mx01((d1*2 - 1));
        _set_ax2((inst->offs_twiddle + nLength2));
        _set_mx20((1));
        _set_mx21((d1*2 - 1));

        for (l = 0; l < l_cnt; l++) //pCos1+=d1, pSin1+=d1, pCos3+=d3, pSin3+=d3
        {
            int32_t ix = l;
            int32_t id = N4_cplx*4;
            int32_t ido = nOrder - k + 1;

            cc1_neg = _x0_u0();
            ss1 = _x0_u1();
            cc3_neg = _x2_u0();
            ss3 = _x2_u1();
            ss1_neg = dspfx_neg_q31(ss1); 
            ss3_neg = dspfx_neg_q31(ss3);

            while (ix < nLength - 1) 
            {
                int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
                // pI0 = output + ix*2;
                _set_ay0((offs_output + ix*2));
                _set_my00((N4_cplx));
                _set_my01((id*2 - N4_cplx * 3));
                _set_ay1((offs_output + ix*2 + 1));
                _set_my10((N4_cplx));
                _set_my11((id*2 - N4_cplx * 3));
                _set_ay2((offs_output + ix*2));
                _set_my20((N4_cplx));
                _set_my21((id*2 - N4_cplx * 3));
                _set_ay3((offs_output + ix*2 + 1));
                _set_my30((N4_cplx));
                _set_my31((id*2 - N4_cplx * 3));
                if (unitShiftPrev || unitShift) {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _y0_u0(); //COMPLEX_RE(input, i0)
                        im0 = _y1_u0(); //COMPLEX_IM(input, i0)
                        re1 = _y0_u0(); //COMPLEX_RE(input, i1)
                        im1 = _y1_u0(); //COMPLEX_IM(input, i1)

                              _mulrt(_y0_nu(), cc1_neg);
                        re2 = _macrt(_y1_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                              _mulrt(_y1_u0(), cc1_neg);
                        im2 = _macrt(_y0_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                              _mulrt(_y0_nu(), cc3_neg);
                        re3 = _macrt(_y1_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                              _mulrt(_y1_u1(), cc3_neg);
                        im3 = _macrt(_y0_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                        tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                        tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                        tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                        _set_y2_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                        _set_y3_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                        _set_y2_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                        _set_y3_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                        _set_y2_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                        _set_y3_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                        _set_y2_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                        _set_y3_u1( dspfx_add_q31(im1, tre3) >> unitShift);
                        //pI0 += id*2;
                    }
                } else {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _y0_u0(); //COMPLEX_RE(input, i0)
                        im0 = _y1_u0(); //COMPLEX_IM(input, i0)
                        re1 = _y0_u0(); //COMPLEX_RE(input, i1)
                        im1 = _y1_u0(); //COMPLEX_IM(input, i1)

                              _mulrt(_y0_nu(), cc1_neg);
                        re2 = _macrt(_y1_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                              _mulrt(_y1_u0(), cc1_neg);
                        im2 = _macrt(_y0_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                              _mulrt(_y0_nu(), cc3_neg);
                        re3 = _macrt(_y1_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                              _mulrt(_y1_u1(), cc3_neg);
                        im3 = _macrt(_y0_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3);
                        tim2 = dspfx_add_q31(im2, im3);
                        tre3 = dspfx_sub_q31(re2, re3);
                        tim3 = dspfx_sub_q31(im2, im3);

                        _set_y2_u0( dspfx_sub_q31(re0, tre2));
                        _set_y3_u0( dspfx_sub_q31(im0, tim2));
                        _set_y2_u0( dspfx_add_q31(re1, tim3));
                        _set_y3_u0( dspfx_sub_q31(im1, tre3));
                        _set_y2_u0( dspfx_add_q31(re0, tre2));
                        _set_y3_u0( dspfx_add_q31(im0, tim2));
                        _set_y2_u1( dspfx_sub_q31(re1, tim3));
                        _set_y3_u1( dspfx_add_q31(im1, tre3));
                    }
                }
                ix = 2 * id - N4_cplx*2 + l;
                id = 4 * id; ido +=2;
            }
        }
        N4_cplx <<= 1;
    }
    
    for (k = dspfx_min_q31(1, nOrder-3); k >=0 ; k--) {
        int32_t d1 = 1 << k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ax0((inst->offs_twiddle));
        _set_mx00((d1*2));
        _set_ax1((inst->offs_twiddle + 1));
        _set_mx10((d1*2));
        _set_ax2((inst->offs_twiddle + nLength2));
        _set_mx20((d1*2));
        _set_ax3((inst->offs_twiddle + nLength2 + 1));
        _set_mx30((d1*2));
        _set_ay0((offs_output));
        _set_my00((N4_cplx));
        _set_my01((AM_OFFS(2 - N4_cplx * 3)));
        _set_ay1((offs_output + 1));
        _set_my10((N4_cplx));
        _set_my11((AM_OFFS(2 - N4_cplx * 3)));
        _set_ay2((offs_output));
        _set_my20((N4_cplx));
        _set_my21((AM_OFFS(2 - N4_cplx * 3)));
        _set_ay3((offs_output + 1));
        _set_my30((N4_cplx));
        _set_my31((AM_OFFS(2 - N4_cplx * 3)));
        if (unitShiftPrev || unitShift) {
            for (l = 0; l < l_cnt; l++) {
                re0 = _y0_u0(); //COMPLEX_RE(input, i0)
                im0 = _y1_u0(); //COMPLEX_IM(input, i0)
                re1 = _y0_u0(); //COMPLEX_RE(input, i1)
                im1 = _y1_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_x1_nu()); 
                ss3 = dspfx_neg_q31(_x3_nu());

                      _mulrt(_y0_nu(), _x0_nu());
                re2 = _macrt(_y1_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                      _mulrt(_y1_u0(), _x0_u0());
                im2 = _macrt(_y0_u0(), _x1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                      _mulrt(_y0_nu(), _x2_nu());
                re3 = _macrt(_y1_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                      _mulrt(_y1_u1(), _x2_u0());
                im3 = _macrt(_y0_u1(), _x3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                _set_y2_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                _set_y3_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                _set_y2_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                _set_y3_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                _set_y2_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                _set_y3_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                _set_y2_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                _set_y3_u1( dspfx_add_q31(im1, tre3) >> unitShift);
            }
        } else {
            for (l = 0; l < l_cnt; l++) {
                re0 = _y0_u0(); //COMPLEX_RE(input, i0)
                im0 = _y1_u0(); //COMPLEX_IM(input, i0)
                re1 = _y0_u0(); //COMPLEX_RE(input, i1)
                im1 = _y1_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_x1_nu()); 
                ss3 = dspfx_neg_q31(_x3_nu());

                      _mulrt(_y0_nu(), _x0_nu());
                re2 = _macrt(_y1_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                      _mulrt(_y1_u0(), _x0_u0());
                im2 = _macrt(_y0_u0(), _x1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                      _mulrt(_y0_nu(), _x2_nu());
                re3 = _macrt(_y1_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                      _mulrt(_y1_u1(), _x2_u0());
                im3 = _macrt(_y0_u1(), _x3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3);
                tim2 = dspfx_add_q31(im2, im3);
                tre3 = dspfx_sub_q31(re2, re3);
                tim3 = dspfx_sub_q31(im2, im3);

                _set_y2_u0( dspfx_sub_q31(re0, tre2));
                _set_y3_u0( dspfx_sub_q31(im0, tim2));
                _set_y2_u0( dspfx_add_q31(re1, tim3));
                _set_y3_u0( dspfx_sub_q31(im1, tre3));
                _set_y2_u0( dspfx_add_q31(re0, tre2));
                _set_y3_u0( dspfx_add_q31(im0, tim2));
                _set_y2_u1( dspfx_sub_q31(re1, tim3));
                _set_y3_u1( dspfx_add_q31(im1, tre3));
            }        
        }
        N4_cplx <<= 1;
    }
}


// Produce instance with: MAC32x32; pIn0 - X; 

#pragma Code("dsplib_dsp_cifft_sradix_q31_XtoY_cfX_MAC32x32")

/**
 *  split-radix Inverse Complex FFT "process" function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - X memory offset to input vector
 * \param offs_output[out] - Y memory offset to output vector
 */

void dsp_cifft_sradix_q31_XtoY_cfX_MAC32x32(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i, k, l;
    int32_t nLength = inst->length;
    int32_t nLength2 = nLength/2;
    int32_t N4_cplx, N = nLength;
    int32_t nOrder = 30 - dspfx_norm_q31(inst->length);
    int32_t unitShiftPrev, unitShift, shiftBits = inst->shiftBits;
   
    q31_t cc1_neg, ss1, ss1_neg, cc3_neg, ss3_neg, ss3;
    q31_t re0, im0, re1, im1, re2, im2, re3, im3;
    q31_t tre2, tre3;
    q31_t tim2, tim3;

    int32_t ix, id, ido;


    // Set of bit-reverse addressing
    _set_ax0(offs_input);
    _set_mx00(AM_BITREV|nLength);
    _set_ax1(offs_input + 1);
    _set_mx10(AM_BITREV|nLength);
    _set_ay0(offs_output);
    _set_my00(1);
    _set_my01(AM_OFFS(-1));

    //FFT first stage with bit-reverse or linear addressing of input
    _set_ay0(offs_output);
    unitShift = (shiftBits-- > 0) ? 1 : 0;    
    if (unitShift) {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _x0_u0();
            im0 = _x1_u0();
            _set_y0_u0( dspfx_add_q31(re0, _x0_nu())>>1);
            _set_y0_u0( dspfx_add_q31(im0, _x1_nu())>>1);
            _set_y0_u0( dspfx_sub_q31(re0, _x0_u0())>>1);
            _set_y0_u0( dspfx_sub_q31(im0, _x1_u0())>>1);
        }
    } else {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _x0_u0();
            im0 = _x1_u0();
            _set_y0_u0( dspfx_add_q31(re0, _x0_nu()));
            _set_y0_u0( dspfx_add_q31(im0, _x1_nu()));
            _set_y0_u0( dspfx_sub_q31(re0, _x0_u0()));
            _set_y0_u0( dspfx_sub_q31(im0, _x1_u0()));
        }
    }

    
    // stage: nOrder - 2
    // baseop: split radix with trivial multiply
    ix = 0;    
    id = 8;    
    ido = 3;
    unitShift = (shiftBits-- > 0) ? 1 : 0;
    while (ix < nLength - 1) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
        _set_ay0(offs_output + ix*2);
        _set_my00(1);
        _set_my01(id*2 - 4 + 1);
        _set_ay1(offs_output + ix*2+4);
        _set_my10(1);
        _set_my11(id*2 - 4 + 1);
        _set_ay3(offs_output + ix*2);
        _set_my30(1);
        _set_my31(id*2 - 4 + 1);
        _set_ay2(offs_output + ix*2+4);
        _set_my20(1);
        _set_my21(id*2 - 4 + 1);
        //pI0 = output + ix*2;
        if (unitShift) {
            for (i = 0; i<i_cnt; i++) {
                re2 = _y2_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _y2_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _y2_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _y2_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_y1_u0( dspfx_sub_q31(_y3_nu(), re2)>>1);
                _set_y0_u0( dspfx_add_q31(_y3_u0(), re2)>>1);
                _set_y1_u0( dspfx_sub_q31(_y3_nu(), im2)>>1);
                _set_y0_u0( dspfx_add_q31(_y3_u0(), im2)>>1);
                _set_y1_u0( dspfx_add_q31(_y3_nu(), im3)>>1);
                _set_y0_u0( dspfx_sub_q31(_y3_u0(), im3)>>1);
                _set_y1_u1( dspfx_sub_q31(_y3_nu(), re3)>>1);
                _set_y0_u1( dspfx_add_q31(_y3_u1(), re3)>>1);
            }
        } else {
            for (i = 0; i<i_cnt; i++) {
                re2 = _y2_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _y2_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _y2_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _y2_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_y1_u0( dspfx_sub_q31(_y3_nu(), re2));
                _set_y0_u0( dspfx_add_q31(_y3_u0(), re2));
                _set_y1_u0( dspfx_sub_q31(_y3_nu(), im2));
                _set_y0_u0( dspfx_add_q31(_y3_u0(), im2));
                _set_y1_u0( dspfx_add_q31(_y3_nu(), im3));
                _set_y0_u0( dspfx_sub_q31(_y3_u0(), im3));
                _set_y1_u1( dspfx_sub_q31(_y3_nu(), re3));
                _set_y0_u1( dspfx_add_q31(_y3_u1(), re3));
            }
        }
        ix = 2 * id - 4;
        id = 4 * id; ido +=2;
    }

   
    // stage: [(nOrder-2) .. 2]
    // baseop: split-radix
    N4_cplx = 4; // N4 = N / 4
    for (k = nOrder-3; k >= 2 ; k--) 
    {
        int32_t d1 = 1<<k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ax0((inst->offs_twiddle));
        _set_mx00((1));
        _set_mx01((d1*2 - 1));
        _set_ax2((inst->offs_twiddle + nLength2));
        _set_mx20((1));
        _set_mx21((d1*2 - 1));

        for (l = 0; l < l_cnt; l++) //pCos1+=d1, pSin1+=d1, pCos3+=d3, pSin3+=d3
        {
            int32_t ix = l;
            int32_t id = N4_cplx*4;
            int32_t ido = nOrder - k + 1;

            cc1_neg = _x0_u0();
            ss1 = _x0_u1();
            cc3_neg = _x2_u0();
            ss3 = _x2_u1();
            ss1_neg = dspfx_neg_q31(ss1);
            ss3_neg = dspfx_neg_q31(ss3);

            while (ix < nLength - 1) 
            {
                int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
                // pI0 = output + ix*2;
                _set_ay3((offs_output + ix*2));
                _set_my30((N4_cplx));
                _set_my31((id*2 - N4_cplx * 3));
                _set_ay2((offs_output + ix*2 + 1));
                _set_my20((N4_cplx));
                _set_my21((id*2 - N4_cplx * 3));
                _set_ay0((offs_output + ix*2));
                _set_my00((N4_cplx));
                _set_my01((id*2 - N4_cplx * 3));
                _set_ay1((offs_output + ix*2 + 1));
                _set_my10((N4_cplx));
                _set_my11((id*2 - N4_cplx * 3));
                if (unitShiftPrev || unitShift) {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _y3_u0(); //COMPLEX_RE(input, i0)
                        im0 = _y2_u0(); //COMPLEX_IM(input, i0)
                        re1 = _y3_u0(); //COMPLEX_RE(input, i1)
                        im1 = _y2_u0(); //COMPLEX_IM(input, i1)

                               _mulflw(_y3_nu(), cc1_neg);
                              _machflw(_y3_nu(), cc1_neg);
                               _macflw(_y2_nu(), ss1_neg);
                        re2 = _machflw(_y2_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                               _mulflw(_y2_nu(), cc1_neg);
                              _machflw(_y2_u0(), cc1_neg);
                               _macflw(_y3_nu(), ss1);
                        im2 = _machflw(_y3_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                               _mulflw(_y3_nu(), cc3_neg);
                              _machflw(_y3_nu(), cc3_neg);
                               _macflw(_y2_nu(), ss3_neg);
                        re3 = _machflw(_y2_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                               _mulflw(_y2_nu(), cc3_neg);
                              _machflw(_y2_u1(), cc3_neg);
                               _macflw(_y3_nu(), ss3);
                        im3 = _machflw(_y3_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                        tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                        tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                        tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                        _set_y0_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                        _set_y1_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                        _set_y0_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                        _set_y1_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                        _set_y0_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                        _set_y1_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                        _set_y0_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                        _set_y1_u1( dspfx_add_q31(im1, tre3) >> unitShift);
                        //pI0 += id*2;
                    }
                } else {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _y3_u0(); //COMPLEX_RE(input, i0)
                        im0 = _y2_u0(); //COMPLEX_IM(input, i0)
                        re1 = _y3_u0(); //COMPLEX_RE(input, i1)
                        im1 = _y2_u0(); //COMPLEX_IM(input, i1)

                               _mulflw(_y3_nu(), cc1_neg);
                              _machflw(_y3_nu(), cc1_neg);
                               _macflw(_y2_nu(), ss1_neg);
                        re2 = _machflw(_y2_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                               _mulflw(_y2_nu(), cc1_neg);
                              _machflw(_y2_u0(), cc1_neg);
                               _macflw(_y3_nu(), ss1);
                        im2 = _machflw(_y3_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                               _mulflw(_y3_nu(), cc3_neg);
                              _machflw(_y3_nu(), cc3_neg);
                               _macflw(_y2_nu(), ss3_neg);
                        re3 = _machflw(_y2_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                               _mulflw(_y2_nu(), cc3_neg);
                              _machflw(_y2_u1(), cc3_neg);
                               _macflw(_y3_nu(), ss3);
                        im3 = _machflw(_y3_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3);
                        tim2 = dspfx_add_q31(im2, im3);
                        tre3 = dspfx_sub_q31(re2, re3);
                        tim3 = dspfx_sub_q31(im2, im3);

                        _set_y0_u0( dspfx_sub_q31(re0, tre2));
                        _set_y1_u0( dspfx_sub_q31(im0, tim2));
                        _set_y0_u0( dspfx_add_q31(re1, tim3));
                        _set_y1_u0( dspfx_sub_q31(im1, tre3));
                        _set_y0_u0( dspfx_add_q31(re0, tre2));
                        _set_y1_u0( dspfx_add_q31(im0, tim2));
                        _set_y0_u1( dspfx_sub_q31(re1, tim3));
                        _set_y1_u1( dspfx_add_q31(im1, tre3));
                    }
                }
                ix = 2 * id - N4_cplx*2 + l;
                id = 4 * id; ido +=2;
            }
        }
        N4_cplx <<= 1;
    }
    
    for (k = dspfx_min_q31(1, nOrder-3); k >=0 ; k--) {
        int32_t d1 = 1 << k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ax0((inst->offs_twiddle));
        _set_mx00((d1*2));
        _set_ax1((inst->offs_twiddle + 1));
        _set_mx10((d1*2));
        _set_ax2((inst->offs_twiddle + nLength2));
        _set_mx20((d1*2));
        _set_ax3((inst->offs_twiddle + nLength2 + 1));
        _set_mx30((d1*2));
        _set_ay3((offs_output));
        _set_my30((N4_cplx));
        _set_my31((AM_OFFS(2 - N4_cplx * 3)));
        _set_ay2((offs_output + 1));
        _set_my20((N4_cplx));
        _set_my21((AM_OFFS(2 - N4_cplx * 3)));
        _set_ay0((offs_output));
        _set_my00((N4_cplx));
        _set_my01((AM_OFFS(2 - N4_cplx * 3)));
        _set_ay1((offs_output + 1));
        _set_my10((N4_cplx));
        _set_my11((AM_OFFS(2 - N4_cplx * 3)));
        if (unitShiftPrev || unitShift) {
            for (l = 0; l < l_cnt; l++) {
                re0 = _y3_u0(); //COMPLEX_RE(input, i0)
                im0 = _y2_u0(); //COMPLEX_IM(input, i0)
                re1 = _y3_u0(); //COMPLEX_RE(input, i1)
                im1 = _y2_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_x1_nu()); 
                ss3 = dspfx_neg_q31(_x3_nu());

                       _mulflw(_y3_nu(), _x0_nu());
                      _machflw(_y3_nu(), _x0_nu());
                       _macflw(_y2_nu(), ss1);
                re2 = _machflw(_y2_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                       _mulflw(_y2_nu(), _x0_nu());
                      _machflw(_y2_u0(), _x0_u0());
                       _macflw(_y3_nu(), _x1_nu());
                im2 = _machflw(_y3_u0(), _x1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                       _mulflw(_y3_nu(), _x2_nu());
                      _machflw(_y3_nu(), _x2_nu());
                       _macflw(_y2_nu(), ss3);
                re3 = _machflw(_y2_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                       _mulflw(_y2_nu(), _x2_nu());
                      _machflw(_y2_u1(), _x2_u0());
                       _macflw(_y3_nu(), _x3_nu());
                im3 = _machflw(_y3_u1(), _x3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                _set_y0_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                _set_y1_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                _set_y0_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                _set_y1_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                _set_y0_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                _set_y1_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                _set_y0_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                _set_y1_u1( dspfx_add_q31(im1, tre3) >> unitShift);
            }
        } else {
            for (l = 0; l < l_cnt; l++) {
                re0 = _y3_u0(); //COMPLEX_RE(input, i0)
                im0 = _y2_u0(); //COMPLEX_IM(input, i0)
                re1 = _y3_u0(); //COMPLEX_RE(input, i1)
                im1 = _y2_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_x1_nu()); 
                ss3 = dspfx_neg_q31(_x3_nu());

                       _mulflw(_y3_nu(), _x0_nu());
                      _machflw(_y3_nu(), _x0_nu());
                       _macflw(_y2_nu(), ss1);
                re2 = _machflw(_y2_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                       _mulflw(_y2_nu(), _x0_nu());
                      _machflw(_y2_u0(), _x0_u0());
                       _macflw(_y3_nu(), _x1_nu());
                im2 = _machflw(_y3_u0(), _x1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                       _mulflw(_y3_nu(), _x2_nu());
                      _machflw(_y3_nu(), _x2_nu());
                       _macflw(_y2_nu(), ss3);
                re3 = _machflw(_y2_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                       _mulflw(_y2_nu(), _x2_nu());
                      _machflw(_y2_u1(), _x2_u0());
                       _macflw(_y3_nu(), _x3_nu());
                im3 = _machflw(_y3_u1(), _x3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3);
                tim2 = dspfx_add_q31(im2, im3);
                tre3 = dspfx_sub_q31(re2, re3);
                tim3 = dspfx_sub_q31(im2, im3);

                _set_y0_u0( dspfx_sub_q31(re0, tre2));
                _set_y1_u0( dspfx_sub_q31(im0, tim2));
                _set_y0_u0( dspfx_add_q31(re1, tim3));
                _set_y1_u0( dspfx_sub_q31(im1, tre3));
                _set_y0_u0( dspfx_add_q31(re0, tre2));
                _set_y1_u0( dspfx_add_q31(im0, tim2));
                _set_y0_u1( dspfx_sub_q31(re1, tim3));
                _set_y1_u1( dspfx_add_q31(im1, tre3));
            }        
        }
        N4_cplx <<= 1;
    }
}
// Produce instance with: MAC32x32; pIn0 - Y; 

#pragma Code("dsplib_dsp_cifft_sradix_q31_YtoX_cfY_MAC32x32")

/**
 *  split-radix Inverse Complex FFT "process" function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - Y memory offset to input vector
 * \param offs_output[out] - X memory offset to output vector
 */

void dsp_cifft_sradix_q31_YtoX_cfY_MAC32x32(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i, k, l;
    int32_t nLength = inst->length;
    int32_t nLength2 = nLength/2;
    int32_t N4_cplx, N = nLength;
    int32_t nOrder = 30 - dspfx_norm_q31(inst->length);
    int32_t unitShiftPrev, unitShift, shiftBits = inst->shiftBits;
   
    q31_t cc1_neg, ss1, ss1_neg, cc3_neg, ss3_neg, ss3;
    q31_t re0, im0, re1, im1, re2, im2, re3, im3;
    q31_t tre2, tre3;
    q31_t tim2, tim3;

    int32_t ix, id, ido;


    // Set of bit-reverse addressing
    _set_ay0(offs_input);
    _set_my00(AM_BITREV|nLength);
    _set_ay1(offs_input + 1);
    _set_my10(AM_BITREV|nLength);
    _set_ax0(offs_output);
    _set_mx00(1);
    _set_mx01(AM_OFFS(-1));

    //FFT first stage with bit-reverse or linear addressing of input
    _set_ax0(offs_output);
    unitShift = (shiftBits-- > 0) ? 1 : 0;    
    if (unitShift) {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _y0_u0();
            im0 = _y1_u0();
            _set_x0_u0( dspfx_add_q31(re0, _y0_nu())>>1);
            _set_x0_u0( dspfx_add_q31(im0, _y1_nu())>>1);
            _set_x0_u0( dspfx_sub_q31(re0, _y0_u0())>>1);
            _set_x0_u0( dspfx_sub_q31(im0, _y1_u0())>>1);
        }
    } else {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _y0_u0();
            im0 = _y1_u0();
            _set_x0_u0( dspfx_add_q31(re0, _y0_nu()));
            _set_x0_u0( dspfx_add_q31(im0, _y1_nu()));
            _set_x0_u0( dspfx_sub_q31(re0, _y0_u0()));
            _set_x0_u0( dspfx_sub_q31(im0, _y1_u0()));
        }
    }

    
    // stage: nOrder - 2
    // baseop: split radix with trivial multiply
    ix = 0;    
    id = 8;    
    ido = 3;
    unitShift = (shiftBits-- > 0) ? 1 : 0;
    while (ix < nLength - 1) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
        _set_ax0(offs_output + ix*2);
        _set_mx00(1);
        _set_mx01(id*2 - 4 + 1);
        _set_ax1(offs_output + ix*2+4);
        _set_mx10(1);
        _set_mx11(id*2 - 4 + 1);
        _set_ax3(offs_output + ix*2);
        _set_mx30(1);
        _set_mx31(id*2 - 4 + 1);
        _set_ax2(offs_output + ix*2+4);
        _set_mx20(1);
        _set_mx21(id*2 - 4 + 1);
        //pI0 = output + ix*2;
        if (unitShift) {
            for (i = 0; i<i_cnt; i++) {
                re2 = _x2_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _x2_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _x2_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _x2_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_x1_u0( dspfx_sub_q31(_x3_nu(), re2)>>1);
                _set_x0_u0( dspfx_add_q31(_x3_u0(), re2)>>1);
                _set_x1_u0( dspfx_sub_q31(_x3_nu(), im2)>>1);
                _set_x0_u0( dspfx_add_q31(_x3_u0(), im2)>>1);
                _set_x1_u0( dspfx_add_q31(_x3_nu(), im3)>>1);
                _set_x0_u0( dspfx_sub_q31(_x3_u0(), im3)>>1);
                _set_x1_u1( dspfx_sub_q31(_x3_nu(), re3)>>1);
                _set_x0_u1( dspfx_add_q31(_x3_u1(), re3)>>1);
            }
        } else {
            for (i = 0; i<i_cnt; i++) {
                re2 = _x2_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _x2_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _x2_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _x2_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_x1_u0( dspfx_sub_q31(_x3_nu(), re2));
                _set_x0_u0( dspfx_add_q31(_x3_u0(), re2));
                _set_x1_u0( dspfx_sub_q31(_x3_nu(), im2));
                _set_x0_u0( dspfx_add_q31(_x3_u0(), im2));
                _set_x1_u0( dspfx_add_q31(_x3_nu(), im3));
                _set_x0_u0( dspfx_sub_q31(_x3_u0(), im3));
                _set_x1_u1( dspfx_sub_q31(_x3_nu(), re3));
                _set_x0_u1( dspfx_add_q31(_x3_u1(), re3));
            }
        }
        ix = 2 * id - 4;
        id = 4 * id; ido +=2;
    }

   
    // stage: [(nOrder-2) .. 2]
    // baseop: split-radix
    N4_cplx = 4; // N4 = N / 4
    for (k = nOrder-3; k >= 2 ; k--) 
    {
        int32_t d1 = 1<<k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ay0((inst->offs_twiddle));
        _set_my00((1));
        _set_my01((d1*2 - 1));
        _set_ay2((inst->offs_twiddle + nLength2));
        _set_my20((1));
        _set_my21((d1*2 - 1));

        for (l = 0; l < l_cnt; l++) //pCos1+=d1, pSin1+=d1, pCos3+=d3, pSin3+=d3
        {
            int32_t ix = l;
            int32_t id = N4_cplx*4;
            int32_t ido = nOrder - k + 1;

            cc1_neg = _y0_u0();
            ss1 = _y0_u1();
            cc3_neg = _y2_u0();
            ss3 = _y2_u1();
            ss1_neg = dspfx_neg_q31(ss1);
            ss3_neg = dspfx_neg_q31(ss3);

            while (ix < nLength - 1) 
            {
                int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
                // pI0 = output + ix*2;
                _set_ax3((offs_output + ix*2));
                _set_mx30((N4_cplx));
                _set_mx31((id*2 - N4_cplx * 3));
                _set_ax2((offs_output + ix*2 + 1));
                _set_mx20((N4_cplx));
                _set_mx21((id*2 - N4_cplx * 3));
                _set_ax0((offs_output + ix*2));
                _set_mx00((N4_cplx));
                _set_mx01((id*2 - N4_cplx * 3));
                _set_ax1((offs_output + ix*2 + 1));
                _set_mx10((N4_cplx));
                _set_mx11((id*2 - N4_cplx * 3));
                if (unitShiftPrev || unitShift) {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _x3_u0(); //COMPLEX_RE(input, i0)
                        im0 = _x2_u0(); //COMPLEX_IM(input, i0)
                        re1 = _x3_u0(); //COMPLEX_RE(input, i1)
                        im1 = _x2_u0(); //COMPLEX_IM(input, i1)

                               _mulflw(_x3_nu(), cc1_neg);
                              _machflw(_x3_nu(), cc1_neg);
                               _macflw(_x2_nu(), ss1_neg);
                        re2 = _machflw(_x2_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                               _mulflw(_x2_nu(), cc1_neg);
                              _machflw(_x2_u0(), cc1_neg);
                               _macflw(_x3_nu(), ss1);
                        im2 = _machflw(_x3_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                               _mulflw(_x3_nu(), cc3_neg);
                              _machflw(_x3_nu(), cc3_neg);
                               _macflw(_x2_nu(), ss3_neg);
                        re3 = _machflw(_x2_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                               _mulflw(_x2_nu(), cc3_neg);
                              _machflw(_x2_u1(), cc3_neg);
                               _macflw(_x3_nu(), ss3);
                        im3 = _machflw(_x3_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                        tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                        tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                        tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                        _set_x0_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                        _set_x1_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                        _set_x0_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                        _set_x1_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                        _set_x0_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                        _set_x1_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                        _set_x0_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                        _set_x1_u1( dspfx_add_q31(im1, tre3) >> unitShift);
                        //pI0 += id*2;
                    }
                } else {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _x3_u0(); //COMPLEX_RE(input, i0)
                        im0 = _x2_u0(); //COMPLEX_IM(input, i0)
                        re1 = _x3_u0(); //COMPLEX_RE(input, i1)
                        im1 = _x2_u0(); //COMPLEX_IM(input, i1)

                               _mulflw(_x3_nu(), cc1_neg);
                              _machflw(_x3_nu(), cc1_neg);
                               _macflw(_x2_nu(), ss1_neg);
                        re2 = _machflw(_x2_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                               _mulflw(_x2_nu(), cc1_neg);
                              _machflw(_x2_u0(), cc1_neg);
                               _macflw(_x3_nu(), ss1);
                        im2 = _machflw(_x3_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                               _mulflw(_x3_nu(), cc3_neg);
                              _machflw(_x3_nu(), cc3_neg);
                               _macflw(_x2_nu(), ss3_neg);
                        re3 = _machflw(_x2_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                               _mulflw(_x2_nu(), cc3_neg);
                              _machflw(_x2_u1(), cc3_neg);
                               _macflw(_x3_nu(), ss3);
                        im3 = _machflw(_x3_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3);
                        tim2 = dspfx_add_q31(im2, im3);
                        tre3 = dspfx_sub_q31(re2, re3);
                        tim3 = dspfx_sub_q31(im2, im3);

                        _set_x0_u0( dspfx_sub_q31(re0, tre2));
                        _set_x1_u0( dspfx_sub_q31(im0, tim2));
                        _set_x0_u0( dspfx_add_q31(re1, tim3));
                        _set_x1_u0( dspfx_sub_q31(im1, tre3));
                        _set_x0_u0( dspfx_add_q31(re0, tre2));
                        _set_x1_u0( dspfx_add_q31(im0, tim2));
                        _set_x0_u1( dspfx_sub_q31(re1, tim3));
                        _set_x1_u1( dspfx_add_q31(im1, tre3));
                    }
                }
                ix = 2 * id - N4_cplx*2 + l;
                id = 4 * id; ido +=2;
            }
        }
        N4_cplx <<= 1;
    }
    
    for (k = dspfx_min_q31(1, nOrder-3); k >=0 ; k--) {
        int32_t d1 = 1 << k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ay0((inst->offs_twiddle));
        _set_my00((d1*2));
        _set_ay1((inst->offs_twiddle + 1));
        _set_my10((d1*2));
        _set_ay2((inst->offs_twiddle + nLength2));
        _set_my20((d1*2));
        _set_ay3((inst->offs_twiddle + nLength2 + 1));
        _set_my30((d1*2));
        _set_ax3((offs_output));
        _set_mx30((N4_cplx));
        _set_mx31((AM_OFFS(2 - N4_cplx * 3)));
        _set_ax2((offs_output + 1));
        _set_mx20((N4_cplx));
        _set_mx21((AM_OFFS(2 - N4_cplx * 3)));
        _set_ax0((offs_output));
        _set_mx00((N4_cplx));
        _set_mx01((AM_OFFS(2 - N4_cplx * 3)));
        _set_ax1((offs_output + 1));
        _set_mx10((N4_cplx));
        _set_mx11((AM_OFFS(2 - N4_cplx * 3)));
        if (unitShiftPrev || unitShift) {
            for (l = 0; l < l_cnt; l++) {
                re0 = _x3_u0(); //COMPLEX_RE(input, i0)
                im0 = _x2_u0(); //COMPLEX_IM(input, i0)
                re1 = _x3_u0(); //COMPLEX_RE(input, i1)
                im1 = _x2_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_y1_nu()); 
                ss3 = dspfx_neg_q31(_y3_nu());

                       _mulflw(_x3_nu(), _y0_nu());
                      _machflw(_x3_nu(), _y0_nu());
                       _macflw(_x2_nu(), ss1);
                re2 = _machflw(_x2_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                       _mulflw(_x2_nu(), _y0_nu());
                      _machflw(_x2_u0(), _y0_u0());
                       _macflw(_x3_nu(), _y1_nu());
                im2 = _machflw(_x3_u0(), _y1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                       _mulflw(_x3_nu(), _y2_nu());
                      _machflw(_x3_nu(), _y2_nu());
                       _macflw(_x2_nu(), ss3);
                re3 = _machflw(_x2_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                       _mulflw(_x2_nu(), _y2_nu());
                      _machflw(_x2_u1(), _y2_u0());
                       _macflw(_x3_nu(), _y3_nu());
                im3 = _machflw(_x3_u1(), _y3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                _set_x0_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                _set_x1_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                _set_x0_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                _set_x1_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                _set_x0_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                _set_x1_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                _set_x0_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                _set_x1_u1( dspfx_add_q31(im1, tre3) >> unitShift);
            }
        } else {
            for (l = 0; l < l_cnt; l++) {
                re0 = _x3_u0(); //COMPLEX_RE(input, i0)
                im0 = _x2_u0(); //COMPLEX_IM(input, i0)
                re1 = _x3_u0(); //COMPLEX_RE(input, i1)
                im1 = _x2_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_y1_nu()); 
                ss3 = dspfx_neg_q31(_y3_nu());

                       _mulflw(_x3_nu(), _y0_nu());
                      _machflw(_x3_nu(), _y0_nu());
                       _macflw(_x2_nu(), ss1);
                re2 = _machflw(_x2_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                       _mulflw(_x2_nu(), _y0_nu());
                      _machflw(_x2_u0(), _y0_u0());
                       _macflw(_x3_nu(), _y1_nu());
                im2 = _machflw(_x3_u0(), _y1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                       _mulflw(_x3_nu(), _y2_nu());
                      _machflw(_x3_nu(), _y2_nu());
                       _macflw(_x2_nu(), ss3);
                re3 = _machflw(_x2_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                       _mulflw(_x2_nu(), _y2_nu());
                      _machflw(_x2_u1(), _y2_u0());
                       _macflw(_x3_nu(), _y3_nu());
                im3 = _machflw(_x3_u1(), _y3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3);
                tim2 = dspfx_add_q31(im2, im3);
                tre3 = dspfx_sub_q31(re2, re3);
                tim3 = dspfx_sub_q31(im2, im3);

                _set_x0_u0( dspfx_sub_q31(re0, tre2));
                _set_x1_u0( dspfx_sub_q31(im0, tim2));
                _set_x0_u0( dspfx_add_q31(re1, tim3));
                _set_x1_u0( dspfx_sub_q31(im1, tre3));
                _set_x0_u0( dspfx_add_q31(re0, tre2));
                _set_x1_u0( dspfx_add_q31(im0, tim2));
                _set_x0_u1( dspfx_sub_q31(re1, tim3));
                _set_x1_u1( dspfx_add_q31(im1, tre3));
            }        
        }
        N4_cplx <<= 1;
    }
}
// Produce instance with: MAC24x24; pIn0 - X; 

#pragma Code("dsplib_dsp_cifft_sradix_q31_XtoY_cfX_MAC24x24")

/**
 *  split-radix Inverse Complex FFT "process" function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - X memory offset to input vector
 * \param offs_output[out] - Y memory offset to output vector
 */

void dsp_cifft_sradix_q31_XtoY_cfX_MAC24x24(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i, k, l;
    int32_t nLength = inst->length;
    int32_t nLength2 = nLength/2;
    int32_t N4_cplx, N = nLength;
    int32_t nOrder = 30 - dspfx_norm_q31(inst->length);
    int32_t unitShiftPrev, unitShift, shiftBits = inst->shiftBits;
   
    q31_t cc1_neg, ss1, ss1_neg, cc3_neg, ss3_neg, ss3;
    q31_t re0, im0, re1, im1, re2, im2, re3, im3;
    q31_t tre2, tre3;
    q31_t tim2, tim3;

    int32_t ix, id, ido;


    // Set of bit-reverse addressing
    _set_ax0(offs_input);
    _set_mx00(AM_BITREV|nLength);
    _set_ax1(offs_input + 1);
    _set_mx10(AM_BITREV|nLength);
    _set_ay0(offs_output);
    _set_my00(1);
    _set_my01(AM_OFFS(-1));

    //FFT first stage with bit-reverse or linear addressing of input
    _set_ay0(offs_output);
    unitShift = (shiftBits-- > 0) ? 1 : 0;    
    if (unitShift) {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _x0_u0();
            im0 = _x1_u0();
            _set_y0_u0( dspfx_add_q31(re0, _x0_nu())>>1);
            _set_y0_u0( dspfx_add_q31(im0, _x1_nu())>>1);
            _set_y0_u0( dspfx_sub_q31(re0, _x0_u0())>>1);
            _set_y0_u0( dspfx_sub_q31(im0, _x1_u0())>>1);
        }
    } else {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _x0_u0();
            im0 = _x1_u0();
            _set_y0_u0( dspfx_add_q31(re0, _x0_nu()));
            _set_y0_u0( dspfx_add_q31(im0, _x1_nu()));
            _set_y0_u0( dspfx_sub_q31(re0, _x0_u0()));
            _set_y0_u0( dspfx_sub_q31(im0, _x1_u0()));
        }
    }

    
    // stage: nOrder - 2
    // baseop: split radix with trivial multiply
    ix = 0;    
    id = 8;    
    ido = 3;
    unitShift = (shiftBits-- > 0) ? 1 : 0;
    while (ix < nLength - 1) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
        _set_ay0(offs_output + ix*2);
        _set_my00(1);
        _set_my01(id*2 - 4 + 1);
        _set_ay1(offs_output + ix*2+4);
        _set_my10(1);
        _set_my11(id*2 - 4 + 1);
        _set_ay3(offs_output + ix*2);
        _set_my30(1);
        _set_my31(id*2 - 4 + 1);
        _set_ay2(offs_output + ix*2+4);
        _set_my20(1);
        _set_my21(id*2 - 4 + 1);
        //pI0 = output + ix*2;
        if (unitShift) {
            for (i = 0; i<i_cnt; i++) {
                re2 = _y2_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _y2_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _y2_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _y2_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_y1_u0( dspfx_sub_q31(_y3_nu(), re2)>>1);
                _set_y0_u0( dspfx_add_q31(_y3_u0(), re2)>>1);
                _set_y1_u0( dspfx_sub_q31(_y3_nu(), im2)>>1);
                _set_y0_u0( dspfx_add_q31(_y3_u0(), im2)>>1);
                _set_y1_u0( dspfx_add_q31(_y3_nu(), im3)>>1);
                _set_y0_u0( dspfx_sub_q31(_y3_u0(), im3)>>1);
                _set_y1_u1( dspfx_sub_q31(_y3_nu(), re3)>>1);
                _set_y0_u1( dspfx_add_q31(_y3_u1(), re3)>>1);
            }
        } else {
            for (i = 0; i<i_cnt; i++) {
                re2 = _y2_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _y2_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _y2_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _y2_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_y1_u0( dspfx_sub_q31(_y3_nu(), re2));
                _set_y0_u0( dspfx_add_q31(_y3_u0(), re2));
                _set_y1_u0( dspfx_sub_q31(_y3_nu(), im2));
                _set_y0_u0( dspfx_add_q31(_y3_u0(), im2));
                _set_y1_u0( dspfx_add_q31(_y3_nu(), im3));
                _set_y0_u0( dspfx_sub_q31(_y3_u0(), im3));
                _set_y1_u1( dspfx_sub_q31(_y3_nu(), re3));
                _set_y0_u1( dspfx_add_q31(_y3_u1(), re3));
            }
        }
        ix = 2 * id - 4;
        id = 4 * id; ido +=2;
    }

   
    _set_MACMODE(7UL<<16);
    // stage: [(nOrder-2) .. 2]
    // baseop: split-radix
    N4_cplx = 4; // N4 = N / 4
    for (k = nOrder-3; k >= 2 ; k--) 
    {
        int32_t d1 = 1<<k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ax0((inst->offs_twiddle));
        _set_mx00((1));
        _set_mx01((d1*2 - 1));
        _set_ax2((inst->offs_twiddle + nLength2));
        _set_mx20((1));
        _set_mx21((d1*2 - 1));

        for (l = 0; l < l_cnt; l++) //pCos1+=d1, pSin1+=d1, pCos3+=d3, pSin3+=d3
        {
            int32_t ix = l;
            int32_t id = N4_cplx*4;
            int32_t ido = nOrder - k + 1;

            cc1_neg = _x0_u0();
            ss1 = _x0_u1();
            cc3_neg = _x2_u0();
            ss3 = _x2_u1();
            ss1_neg = dspfx_neg_q31(ss1);
            ss3_neg = dspfx_neg_q31(ss3);

            while (ix < nLength - 1) 
            {
                int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
                // pI0 = output + ix*2;
                _set_ay3((offs_output + ix*2));
                _set_my30((N4_cplx));
                _set_my31((id*2 - N4_cplx * 3));
                _set_ay2((offs_output + ix*2 + 1));
                _set_my20((N4_cplx));
                _set_my21((id*2 - N4_cplx * 3));
                _set_ay0((offs_output + ix*2));
                _set_my00((N4_cplx));
                _set_my01((id*2 - N4_cplx * 3));
                _set_ay1((offs_output + ix*2 + 1));
                _set_my10((N4_cplx));
                _set_my11((id*2 - N4_cplx * 3));
                if (unitShiftPrev || unitShift) {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _y3_u0(); //COMPLEX_RE(input, i0)
                        im0 = _y2_u0(); //COMPLEX_IM(input, i0)
                        re1 = _y3_u0(); //COMPLEX_RE(input, i1)
                        im1 = _y2_u0(); //COMPLEX_IM(input, i1)

                              _mulrt(_y3_nu(), cc1_neg);
                        re2 = _macrt(_y2_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                              _mulrt(_y2_u0(), cc1_neg);
                        im2 = _macrt(_y3_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                              _mulrt(_y3_nu(), cc3_neg);
                        re3 = _macrt(_y2_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                              _mulrt(_y2_u1(), cc3_neg);
                        im3 = _macrt(_y3_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                        tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                        tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                        tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                        _set_y0_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                        _set_y1_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                        _set_y0_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                        _set_y1_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                        _set_y0_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                        _set_y1_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                        _set_y0_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                        _set_y1_u1( dspfx_add_q31(im1, tre3) >> unitShift);
                        //pI0 += id*2;
                    }
                } else {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _y3_u0(); //COMPLEX_RE(input, i0)
                        im0 = _y2_u0(); //COMPLEX_IM(input, i0)
                        re1 = _y3_u0(); //COMPLEX_RE(input, i1)
                        im1 = _y2_u0(); //COMPLEX_IM(input, i1)

                              _mulrt(_y3_nu(), cc1_neg);
                        re2 = _macrt(_y2_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                              _mulrt(_y2_u0(), cc1_neg);
                        im2 = _macrt(_y3_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                              _mulrt(_y3_nu(), cc3_neg);
                        re3 = _macrt(_y2_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                              _mulrt(_y2_u1(), cc3_neg);
                        im3 = _macrt(_y3_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3);
                        tim2 = dspfx_add_q31(im2, im3);
                        tre3 = dspfx_sub_q31(re2, re3);
                        tim3 = dspfx_sub_q31(im2, im3);

                        _set_y0_u0( dspfx_sub_q31(re0, tre2));
                        _set_y1_u0( dspfx_sub_q31(im0, tim2));
                        _set_y0_u0( dspfx_add_q31(re1, tim3));
                        _set_y1_u0( dspfx_sub_q31(im1, tre3));
                        _set_y0_u0( dspfx_add_q31(re0, tre2));
                        _set_y1_u0( dspfx_add_q31(im0, tim2));
                        _set_y0_u1( dspfx_sub_q31(re1, tim3));
                        _set_y1_u1( dspfx_add_q31(im1, tre3));
                    }
                }
                ix = 2 * id - N4_cplx*2 + l;
                id = 4 * id; ido +=2;
            }
        }
        N4_cplx <<= 1;
    }
    
    for (k = dspfx_min_q31(1, nOrder-3); k >=0 ; k--) {
        int32_t d1 = 1 << k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ax0((inst->offs_twiddle));
        _set_mx00((d1*2));
        _set_ax1((inst->offs_twiddle + 1));
        _set_mx10((d1*2));
        _set_ax2((inst->offs_twiddle + nLength2));
        _set_mx20((d1*2));
        _set_ax3((inst->offs_twiddle + nLength2 + 1));
        _set_mx30((d1*2));
        _set_ay3((offs_output));
        _set_my30((N4_cplx));
        _set_my31((AM_OFFS(2 - N4_cplx * 3)));
        _set_ay2((offs_output + 1));
        _set_my20((N4_cplx));
        _set_my21((AM_OFFS(2 - N4_cplx * 3)));
        _set_ay0((offs_output));
        _set_my00((N4_cplx));
        _set_my01((AM_OFFS(2 - N4_cplx * 3)));
        _set_ay1((offs_output + 1));
        _set_my10((N4_cplx));
        _set_my11((AM_OFFS(2 - N4_cplx * 3)));
        if (unitShiftPrev || unitShift) {
            for (l = 0; l < l_cnt; l++) {
                re0 = _y3_u0(); //COMPLEX_RE(input, i0)
                im0 = _y2_u0(); //COMPLEX_IM(input, i0)
                re1 = _y3_u0(); //COMPLEX_RE(input, i1)
                im1 = _y2_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_x1_nu()); 
                ss3 = dspfx_neg_q31(_x3_nu());

                      _mulrt(_y3_nu(), _x0_nu());
                re2 = _macrt(_y2_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                      _mulrt(_y2_u0(), _x0_u0());
                im2 = _macrt(_y3_u0(), _x1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                      _mulrt(_y3_nu(), _x2_nu());
                re3 = _macrt(_y2_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                      _mulrt(_y2_u1(), _x2_u0());
                im3 = _macrt(_y3_u1(), _x3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                _set_y0_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                _set_y1_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                _set_y0_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                _set_y1_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                _set_y0_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                _set_y1_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                _set_y0_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                _set_y1_u1( dspfx_add_q31(im1, tre3) >> unitShift);
            }
        } else {
            for (l = 0; l < l_cnt; l++) {
                re0 = _y3_u0(); //COMPLEX_RE(input, i0)
                im0 = _y2_u0(); //COMPLEX_IM(input, i0)
                re1 = _y3_u0(); //COMPLEX_RE(input, i1)
                im1 = _y2_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_x1_nu()); 
                ss3 = dspfx_neg_q31(_x3_nu());

                      _mulrt(_y3_nu(), _x0_nu());
                re2 = _macrt(_y2_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                      _mulrt(_y2_u0(), _x0_u0());
                im2 = _macrt(_y3_u0(), _x1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                      _mulrt(_y3_nu(), _x2_nu());
                re3 = _macrt(_y2_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                      _mulrt(_y2_u1(), _x2_u0());
                im3 = _macrt(_y3_u1(), _x3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3);
                tim2 = dspfx_add_q31(im2, im3);
                tre3 = dspfx_sub_q31(re2, re3);
                tim3 = dspfx_sub_q31(im2, im3);

                _set_y0_u0( dspfx_sub_q31(re0, tre2));
                _set_y1_u0( dspfx_sub_q31(im0, tim2));
                _set_y0_u0( dspfx_add_q31(re1, tim3));
                _set_y1_u0( dspfx_sub_q31(im1, tre3));
                _set_y0_u0( dspfx_add_q31(re0, tre2));
                _set_y1_u0( dspfx_add_q31(im0, tim2));
                _set_y0_u1( dspfx_sub_q31(re1, tim3));
                _set_y1_u1( dspfx_add_q31(im1, tre3));
            }        
        }
        N4_cplx <<= 1;
    }
}
// Produce instance with: MAC24x24; pIn0 - Y; 

#pragma Code("dsplib_dsp_cifft_sradix_q31_YtoX_cfY_MAC24x24")

/**
 *  split-radix Inverse Complex FFT "process" function for q31 data type
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - Y memory offset to input vector
 * \param offs_output[out] - X memory offset to output vector
 */

void dsp_cifft_sradix_q31_YtoX_cfY_MAC24x24(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output)  
{
    int32_t i, k, l;
    int32_t nLength = inst->length;
    int32_t nLength2 = nLength/2;
    int32_t N4_cplx, N = nLength;
    int32_t nOrder = 30 - dspfx_norm_q31(inst->length);
    int32_t unitShiftPrev, unitShift, shiftBits = inst->shiftBits;
   
    q31_t cc1_neg, ss1, ss1_neg, cc3_neg, ss3_neg, ss3;
    q31_t re0, im0, re1, im1, re2, im2, re3, im3;
    q31_t tre2, tre3;
    q31_t tim2, tim3;

    int32_t ix, id, ido;


    // Set of bit-reverse addressing
    _set_ay0(offs_input);
    _set_my00(AM_BITREV|nLength);
    _set_ay1(offs_input + 1);
    _set_my10(AM_BITREV|nLength);
    _set_ax0(offs_output);
    _set_mx00(1);
    _set_mx01(AM_OFFS(-1));

    //FFT first stage with bit-reverse or linear addressing of input
    _set_ax0(offs_output);
    unitShift = (shiftBits-- > 0) ? 1 : 0;    
    if (unitShift) {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _y0_u0();
            im0 = _y1_u0();
            _set_x0_u0( dspfx_add_q31(re0, _y0_nu())>>1);
            _set_x0_u0( dspfx_add_q31(im0, _y1_nu())>>1);
            _set_x0_u0( dspfx_sub_q31(re0, _y0_u0())>>1);
            _set_x0_u0( dspfx_sub_q31(im0, _y1_u0())>>1);
        }
    } else {
        for (i = 0; i < nLength2; i++)
        {
            re0 = _y0_u0();
            im0 = _y1_u0();
            _set_x0_u0( dspfx_add_q31(re0, _y0_nu()));
            _set_x0_u0( dspfx_add_q31(im0, _y1_nu()));
            _set_x0_u0( dspfx_sub_q31(re0, _y0_u0()));
            _set_x0_u0( dspfx_sub_q31(im0, _y1_u0()));
        }
    }

    
    // stage: nOrder - 2
    // baseop: split radix with trivial multiply
    ix = 0;    
    id = 8;    
    ido = 3;
    unitShift = (shiftBits-- > 0) ? 1 : 0;
    while (ix < nLength - 1) {
        int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
        _set_ax0(offs_output + ix*2);
        _set_mx00(1);
        _set_mx01(id*2 - 4 + 1);
        _set_ax1(offs_output + ix*2+4);
        _set_mx10(1);
        _set_mx11(id*2 - 4 + 1);
        _set_ax3(offs_output + ix*2);
        _set_mx30(1);
        _set_mx31(id*2 - 4 + 1);
        _set_ax2(offs_output + ix*2+4);
        _set_mx20(1);
        _set_mx21(id*2 - 4 + 1);
        //pI0 = output + ix*2;
        if (unitShift) {
            for (i = 0; i<i_cnt; i++) {
                re2 = _x2_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _x2_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _x2_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _x2_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_x1_u0( dspfx_sub_q31(_x3_nu(), re2)>>1);
                _set_x0_u0( dspfx_add_q31(_x3_u0(), re2)>>1);
                _set_x1_u0( dspfx_sub_q31(_x3_nu(), im2)>>1);
                _set_x0_u0( dspfx_add_q31(_x3_u0(), im2)>>1);
                _set_x1_u0( dspfx_add_q31(_x3_nu(), im3)>>1);
                _set_x0_u0( dspfx_sub_q31(_x3_u0(), im3)>>1);
                _set_x1_u1( dspfx_sub_q31(_x3_nu(), re3)>>1);
                _set_x0_u1( dspfx_add_q31(_x3_u1(), re3)>>1);
            }
        } else {
            for (i = 0; i<i_cnt; i++) {
                re2 = _x2_u0(); //pI0[4]; //COMPLEX_RE(input, i2)
                im2 = _x2_u0(); //pI0[5]; //COMPLEX_IM(input, i2)
                re3 = _x2_u0(); //pI0[6]; //COMPLEX_RE(input, i3)
                im3 = _x2_u1(); //pI0[7]; //COMPLEX_IM(input, i3)

                _set_x1_u0( dspfx_sub_q31(_x3_nu(), re2));
                _set_x0_u0( dspfx_add_q31(_x3_u0(), re2));
                _set_x1_u0( dspfx_sub_q31(_x3_nu(), im2));
                _set_x0_u0( dspfx_add_q31(_x3_u0(), im2));
                _set_x1_u0( dspfx_add_q31(_x3_nu(), im3));
                _set_x0_u0( dspfx_sub_q31(_x3_u0(), im3));
                _set_x1_u1( dspfx_sub_q31(_x3_nu(), re3));
                _set_x0_u1( dspfx_add_q31(_x3_u1(), re3));
            }
        }
        ix = 2 * id - 4;
        id = 4 * id; ido +=2;
    }

   
    _set_MACMODE(7UL<<16);
    // stage: [(nOrder-2) .. 2]
    // baseop: split-radix
    N4_cplx = 4; // N4 = N / 4
    for (k = nOrder-3; k >= 2 ; k--) 
    {
        int32_t d1 = 1<<k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ay0((inst->offs_twiddle));
        _set_my00((1));
        _set_my01((d1*2 - 1));
        _set_ay2((inst->offs_twiddle + nLength2));
        _set_my20((1));
        _set_my21((d1*2 - 1));

        for (l = 0; l < l_cnt; l++) //pCos1+=d1, pSin1+=d1, pCos3+=d3, pSin3+=d3
        {
            int32_t ix = l;
            int32_t id = N4_cplx*4;
            int32_t ido = nOrder - k + 1;

            cc1_neg = _y0_u0();
            ss1 = _y0_u1();
            cc3_neg = _y2_u0();
            ss3 = _y2_u1();
            ss1_neg = dspfx_neg_q31(ss1);
            ss3_neg = dspfx_neg_q31(ss3);

            while (ix < nLength - 1) 
            {
                int32_t i_cnt = dspfx_max_q31((nLength) >> ido,1);
                // pI0 = output + ix*2;
                _set_ax3((offs_output + ix*2));
                _set_mx30((N4_cplx));
                _set_mx31((id*2 - N4_cplx * 3));
                _set_ax2((offs_output + ix*2 + 1));
                _set_mx20((N4_cplx));
                _set_mx21((id*2 - N4_cplx * 3));
                _set_ax0((offs_output + ix*2));
                _set_mx00((N4_cplx));
                _set_mx01((id*2 - N4_cplx * 3));
                _set_ax1((offs_output + ix*2 + 1));
                _set_mx10((N4_cplx));
                _set_mx11((id*2 - N4_cplx * 3));
                if (unitShiftPrev || unitShift) {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _x3_u0(); //COMPLEX_RE(input, i0)
                        im0 = _x2_u0(); //COMPLEX_IM(input, i0)
                        re1 = _x3_u0(); //COMPLEX_RE(input, i1)
                        im1 = _x2_u0(); //COMPLEX_IM(input, i1)

                              _mulrt(_x3_nu(), cc1_neg);
                        re2 = _macrt(_x2_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                              _mulrt(_x2_u0(), cc1_neg);
                        im2 = _macrt(_x3_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                              _mulrt(_x3_nu(), cc3_neg);
                        re3 = _macrt(_x2_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                              _mulrt(_x2_u1(), cc3_neg);
                        im3 = _macrt(_x3_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                        tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                        tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                        tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                        _set_x0_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                        _set_x1_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                        _set_x0_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                        _set_x1_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                        _set_x0_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                        _set_x1_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                        _set_x0_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                        _set_x1_u1( dspfx_add_q31(im1, tre3) >> unitShift);
                        //pI0 += id*2;
                    }
                } else {
                    for (i = 0; i<i_cnt; i++) 
                    {
                        re0 = _x3_u0(); //COMPLEX_RE(input, i0)
                        im0 = _x2_u0(); //COMPLEX_IM(input, i0)
                        re1 = _x3_u0(); //COMPLEX_RE(input, i1)
                        im1 = _x2_u0(); //COMPLEX_IM(input, i1)

                              _mulrt(_x3_nu(), cc1_neg);
                        re2 = _macrt(_x2_nu(), ss1_neg); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                              _mulrt(_x2_u0(), cc1_neg);
                        im2 = _macrt(_x3_u0(), ss1);     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                              _mulrt(_x3_nu(), cc3_neg);
                        re3 = _macrt(_x2_nu(), ss3_neg); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                              _mulrt(_x2_u1(), cc3_neg);
                        im3 = _macrt(_x3_u1(), ss3);     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                        tre2 = dspfx_add_q31(re2, re3);
                        tim2 = dspfx_add_q31(im2, im3);
                        tre3 = dspfx_sub_q31(re2, re3);
                        tim3 = dspfx_sub_q31(im2, im3);

                        _set_x0_u0( dspfx_sub_q31(re0, tre2));
                        _set_x1_u0( dspfx_sub_q31(im0, tim2));
                        _set_x0_u0( dspfx_add_q31(re1, tim3));
                        _set_x1_u0( dspfx_sub_q31(im1, tre3));
                        _set_x0_u0( dspfx_add_q31(re0, tre2));
                        _set_x1_u0( dspfx_add_q31(im0, tim2));
                        _set_x0_u1( dspfx_sub_q31(re1, tim3));
                        _set_x1_u1( dspfx_add_q31(im1, tre3));
                    }
                }
                ix = 2 * id - N4_cplx*2 + l;
                id = 4 * id; ido +=2;
            }
        }
        N4_cplx <<= 1;
    }
    
    for (k = dspfx_min_q31(1, nOrder-3); k >=0 ; k--) {
        int32_t d1 = 1 << k;
        int32_t l_cnt = (N4_cplx>>1);
        unitShiftPrev = unitShift;
        unitShift = (shiftBits-- > 0) ? 1 : 0;
        _set_ay0((inst->offs_twiddle));
        _set_my00((d1*2));
        _set_ay1((inst->offs_twiddle + 1));
        _set_my10((d1*2));
        _set_ay2((inst->offs_twiddle + nLength2));
        _set_my20((d1*2));
        _set_ay3((inst->offs_twiddle + nLength2 + 1));
        _set_my30((d1*2));
        _set_ax3((offs_output));
        _set_mx30((N4_cplx));
        _set_mx31((AM_OFFS(2 - N4_cplx * 3)));
        _set_ax2((offs_output + 1));
        _set_mx20((N4_cplx));
        _set_mx21((AM_OFFS(2 - N4_cplx * 3)));
        _set_ax0((offs_output));
        _set_mx00((N4_cplx));
        _set_mx01((AM_OFFS(2 - N4_cplx * 3)));
        _set_ax1((offs_output + 1));
        _set_mx10((N4_cplx));
        _set_mx11((AM_OFFS(2 - N4_cplx * 3)));
        if (unitShiftPrev || unitShift) {
            for (l = 0; l < l_cnt; l++) {
                re0 = _x3_u0(); //COMPLEX_RE(input, i0)
                im0 = _x2_u0(); //COMPLEX_IM(input, i0)
                re1 = _x3_u0(); //COMPLEX_RE(input, i1)
                im1 = _x2_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_y1_nu()); 
                ss3 = dspfx_neg_q31(_y3_nu());

                      _mulrt(_x3_nu(), _y0_nu());
                re2 = _macrt(_x2_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                      _mulrt(_x2_u0(), _y0_u0());
                im2 = _macrt(_x3_u0(), _y1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                      _mulrt(_x3_nu(), _y2_nu());
                re3 = _macrt(_x2_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                      _mulrt(_x2_u1(), _y2_u0());
                im3 = _macrt(_x3_u1(), _y3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3) >> unitShiftPrev;
                tim2 = dspfx_add_q31(im2, im3) >> unitShiftPrev;
                tre3 = dspfx_sub_q31(re2, re3) >> unitShiftPrev;
                tim3 = dspfx_sub_q31(im2, im3) >> unitShiftPrev;

                _set_x0_u0( dspfx_sub_q31(re0, tre2) >> unitShift);
                _set_x1_u0( dspfx_sub_q31(im0, tim2) >> unitShift);
                _set_x0_u0( dspfx_add_q31(re1, tim3) >> unitShift);
                _set_x1_u0( dspfx_sub_q31(im1, tre3) >> unitShift);
                _set_x0_u0( dspfx_add_q31(re0, tre2) >> unitShift);
                _set_x1_u0( dspfx_add_q31(im0, tim2) >> unitShift);
                _set_x0_u1( dspfx_sub_q31(re1, tim3) >> unitShift);
                _set_x1_u1( dspfx_add_q31(im1, tre3) >> unitShift);
            }
        } else {
            for (l = 0; l < l_cnt; l++) {
                re0 = _x3_u0(); //COMPLEX_RE(input, i0)
                im0 = _x2_u0(); //COMPLEX_IM(input, i0)
                re1 = _x3_u0(); //COMPLEX_RE(input, i1)
                im1 = _x2_u0(); //COMPLEX_IM(input, i1)

                ss1 = dspfx_neg_q31(_y1_nu()); 
                ss3 = dspfx_neg_q31(_y3_nu());

                      _mulrt(_x3_nu(), _y0_nu());
                re2 = _macrt(_x2_nu(), ss1); //int32_t tre2 = (int32_t)(( (int64_t)re2*cc1 - (int64_t)im2*ss1)>>31);
                      _mulrt(_x2_u0(), _y0_u0());
                im2 = _macrt(_x3_u0(), _y1_u0());     //int32_t tim2 = (int32_t)(( (int64_t)im2*cc1 + (int64_t)re2*ss1)>>31);

                      _mulrt(_x3_nu(), _y2_nu());
                re3 = _macrt(_x2_nu(), ss3); //int32_t tre3 = (int32_t)(( (int64_t)re3*cc3 - (int64_t)im3*ss3)>>31);
                      _mulrt(_x2_u1(), _y2_u0());
                im3 = _macrt(_x3_u1(), _y3_u0());     //int32_t tim3 = (int32_t)(( (int64_t)im3*cc3 + (int64_t)re3*ss3)>>31);

                tre2 = dspfx_add_q31(re2, re3);
                tim2 = dspfx_add_q31(im2, im3);
                tre3 = dspfx_sub_q31(re2, re3);
                tim3 = dspfx_sub_q31(im2, im3);

                _set_x0_u0( dspfx_sub_q31(re0, tre2));
                _set_x1_u0( dspfx_sub_q31(im0, tim2));
                _set_x0_u0( dspfx_add_q31(re1, tim3));
                _set_x1_u0( dspfx_sub_q31(im1, tre3));
                _set_x0_u0( dspfx_add_q31(re0, tre2));
                _set_x1_u0( dspfx_add_q31(im0, tim2));
                _set_x0_u1( dspfx_sub_q31(re1, tim3));
                _set_x1_u1( dspfx_add_q31(im1, tre3));
            }        
        }
        N4_cplx <<= 1;
    }
}


// Produce instance with: MAC32x32; pCoeffs0 - X; 

#pragma Code("dsplib_dsp_cifft_sradix_init_q31_cfX")

/**
 * Initializes instance structure of split-radix inverse complex 
 * fft transform for q31 data type. 
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_twiddle - X memory offset to twiddles table
 * \result dsp_status
 */

dsp_status dsp_cifft_sradix_init_q31_cfX(cfft_srad_q31_xy_t *inst, uint32_t length, uint32_t offs_twiddle)  
{
    uint32_t order = 30-dspfx_norm_q31(length);
    int32_t outaddr = offs_twiddle;
    
    inst->length = length;
    inst->offs_twiddle = offs_twiddle;
    inst->shiftBits = order;
 
    _BurstSMemToXMem(
        offs_twiddle, 
        dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], 
        length/2);
    _BurstSMemToXMem(
        offs_twiddle + length/2, 
        dsp_cfft_q31_twd3_tab[order - DSP_MIN_TRANSFORM_ORDER], 
        length/2);    

    return DSP_ERR_OK;
};

// Produce instance with: MAC32x32; pCoeffs0 - Y; 

#pragma Code("dsplib_dsp_cifft_sradix_init_q31_cfY")

/**
 * Initializes instance structure of split-radix inverse complex 
 * fft transform for q31 data type. 
 *
 * \param inst - pointer to instance structure
 * \param length - transform size
 * \param offs_twiddle - Y memory offset to twiddles table
 * \result dsp_status
 */

dsp_status dsp_cifft_sradix_init_q31_cfY(cfft_srad_q31_xy_t *inst, uint32_t length, uint32_t offs_twiddle)  
{
    uint32_t order = 30-dspfx_norm_q31(length);
    int32_t outaddr = offs_twiddle;
    
    inst->length = length;
    inst->offs_twiddle = offs_twiddle;
    inst->shiftBits = order;
 
    _BurstSMemToYMem(
        offs_twiddle, 
        dsp_cfft_q31_twd_tab[order - DSP_MIN_TRANSFORM_ORDER], 
        length/2);
    _BurstSMemToYMem(
        offs_twiddle + length/2, 
        dsp_cfft_q31_twd3_tab[order - DSP_MIN_TRANSFORM_ORDER], 
        length/2);    

    return DSP_ERR_OK;
};


#endif
