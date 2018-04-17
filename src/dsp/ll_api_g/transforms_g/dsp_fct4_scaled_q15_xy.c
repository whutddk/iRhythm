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
#if defined(_ARC600_XY)
#include "dsp_transform_tables_fct4.h"

#pragma Code("dsplib_dsp_fct4_scaled_q15")

#define HALF   ( 1L<<30)
#define M_HALF (-1L<<30)
#define M_ONE  (-1L<<31)
#define SQRT2  (23170L<<16)
// 1 N/2
#define _FCT4_K1(x, y, N2) {                                        \
                                                                    \
      for (p = 0; p < N2; p++) {                                    \
                   b = _mulhflw  (_##x##1_u0(), c);                 \
                       _machflw(_##x##0_u0(), c2);                \
        _set_##y##0_u0(_machflw  (_##x##2_u0(), c2m));              \
                       _machflw(           b, c1);                \
        _set_##y##1_u0(_machflw  (           b, c1));               \
      } /* for p = 0 : N/2-1 */                                     \
                                                                    \
} /* _FCT4_K1 */

#define _FCT4_K(x, y, Delta_nu, N_nu) {                             \
                                                                    \
    for (l = 0; l < Delta_nu; l++) {                                \
      for (p = 0; p < N_nu-1; p++) {                                \
                   b = _mulhflw  (_##x##1_u0(), _##y##3_nu());      \
                       _machflw(_##x##0_u0(), c2);                \
        _set_##y##0_u0(_machflw  (_##x##2_u0(), c2m));              \
                       _machflw(           b, c1);                \
        _set_##y##1_u0(_machflw  (           b, c1));               \
      } /* for p = 0 : N_nu-1 */                                    \
                   b = _mulhflw  (_##x##1_u1(), _##y##3_u0());      \
                       _machflw(_##x##0_u1(), c2);                \
        _set_##y##0_u1(_machflw  (_##x##2_u1(), c2m));              \
                       _machflw(           b, c1);                \
        _set_##y##1_u1(_machflw  (           b, c1));               \
    } /* for l = 0 : Delta_nu-1 */                                  \
                                                                    \
} /* _FCT4_K */

// N/4 2
#define _FCT4_KE1(x, y, N4) {                                       \
                                                                    \
    for (l = 0; l < N4; l++) {                                      \
                   b = _mulhflw  (_##x##1_u0(), _##y##3_nu());      \
                       _machflw(_##x##0_u0(), c2);                \
        _set_##y##0_u0(_machflw  (_##x##2_u0(), c2m));              \
                       _machflw(           b, c1);                \
        _set_##y##1_u0(_machflw  (           b, c1));               \
                                                                    \
                   b = _mulhflw  (_##x##1_u1(), _##y##3_u0());      \
                       _machflw(_##x##0_u1(), c2);                \
        _set_##y##0_u1(_machflw  (_##x##2_u1(), c2m));              \
                       _machflw(           b, c1);                \
        _set_##y##1_u1(_machflw  (           b, c1));               \
    } /* for l = 0 : N/4-1 */                                       \
                                                                    \
} /* _FCT4_KE1 */

// N/2 1
#define _FCT4_KE(x, y, N2) {                                        \
                                                                    \
    for (l = 0; l < N2; l++) {                                      \
                   b = _mulhflw  (_##x##1_u0(), _##y##3_u0());      \
                       _machflw(_##x##0_u0(), c2);                \
        _set_##y##0_u0(_machflw  (_##x##2_u0(), c2m));              \
                       _machflw(           b, c1);                \
        _set_##y##1_u0(_machflw  (           b, c1));               \
    } /* for l = 0 : N/2-1 */                                       \
                                                                    \
} /* _FCT4_KE */

#define _FCT4_LM(x, y, z, N) {                                      \
                                                                    \
    for (l = 0; l < N>>1; l++) {                                    \
        _set_##y##1_u0(_mulhflw  (_##x##0_u0(), _##z##3_u0()));     \
        _set_##y##1_u0(_mulhflw  (_##x##0_u0(), _##z##3_u0()));     \
    } /* for l = 0 : N-1 */                                         \
                                                                    \
} /* _FCT4_LM */

static _Inline void _fct4_x2y_k1 (int32_t N_nu, int32_t c, int32_t c1, int32_t c2, int32_t c2m)
{int32_t p, b;    _FCT4_K1 (x, y, N_nu) }
static _Inline void _fct4_y2x_k1 (int32_t N_nu, int32_t c, int32_t c1, int32_t c2, int32_t c2m)
{int32_t p, b;    _FCT4_K1 (y, x, N_nu) }
static _Inline void _fct4_x2y_k  (int32_t Delta_nu, int32_t N_nu, int32_t c1, int32_t c2, int32_t c2m)
{int32_t l, p, b; _FCT4_K  (x, y, Delta_nu, N_nu) }
static _Inline void _fct4_y2x_k  (int32_t Delta_nu, int32_t N_nu, int32_t c1, int32_t c2, int32_t c2m)
{int32_t l, p, b; _FCT4_K  (y, x, Delta_nu, N_nu) }
static _Inline void _fct4_x2y_ke1(int32_t Delta_nu, int32_t c1, int32_t c2, int32_t c2m)
{int32_t l, b;    _FCT4_KE1(x, y, Delta_nu) }
static _Inline void _fct4_y2x_ke1(int32_t Delta_nu, int32_t c1, int32_t c2, int32_t c2m)
{int32_t l, b;    _FCT4_KE1(y, x, Delta_nu) }
static _Inline void _fct4_x2y_ke (int32_t Delta_nu, int32_t c1, int32_t c2, int32_t c2m)
{int32_t l, b;    _FCT4_KE (x, y, Delta_nu) }
static _Inline void _fct4_y2x_ke (int32_t Delta_nu, int32_t c1, int32_t c2, int32_t c2m)
{int32_t l, b;    _FCT4_KE (y, x, Delta_nu) }

static _Inline void _fct4_x2y_y_lm (int32_t N)
{int32_t l;       _FCT4_LM (x, y, y, N) } // * 2/N
static _Inline void _fct4_y2x_x_lm (int32_t N)
{int32_t l;       _FCT4_LM (y, x, x, N) } // * 2/N
static _Inline void _fct4_y2y_x_lm (int32_t N)
{int32_t l;       _FCT4_LM (y, y, x, N) } // * 2/N
static _Inline void _fct4_x2x_y_lm (int32_t N)
{int32_t l;       _FCT4_LM (x, x, y, N) } // * 2/N

// 1 N/2
#define _FCT4_I_K1(x, y, N, in, out, st) {                          \
                                                                    \
  _set_a##x##0(in);                                                 \
  _set_a##x##1(in + (N>>1)*st);                                     \
  _set_a##x##2(in + (N-1) *st);                                     \
  _set_a##y##0(out);                                                \
  _set_a##y##1(out + 1);                                            \
                                                                    \
  _set_m##x##00( AM_16BIT |  1 *st);                                          \
  _set_m##x##10( AM_16BIT |  1 *st);                                          \
  _set_m##x##20(AM_16BIT | ((-1)*st) & 0x3fff);                                \
  _set_m##y##00(AM_16BIT |   2);                                              \
  _set_m##y##10(AM_16BIT |   2);                                              \
                                                                    \
  _fct4_##x##2##y##_k1((N>>1), c, c1, c2, c2m);                     \
                                                                    \
} /* _FCT4_I_K1 */

#define _FCT4_I_K(x, y, N, Delta_nu, N_nu, in, out, coef) {         \
                                                                    \
  _set_a##x##0(in);                                                 \
  _set_a##x##1(in + (N>>1));                                        \
  _set_a##x##2(in - Delta_nu + N);                                  \
  _set_a##y##0(out);                                                \
  _set_a##y##1(out + (Delta_nu<<1)-1);                              \
  _set_a##y##3(coef + Delta_nu-1);                                  \
                                                                    \
_set_m##x##00(AM_16BIT |  Delta_nu);/**/                                      \
_set_m##x##10(AM_16BIT |  Delta_nu);/**/                                      \
  _set_m##x##20(AM_16BIT |(-Delta_nu) & 0x3fff);                              \
  _set_m##y##00(AM_16BIT |  Delta_nu<<1);                                     \
  _set_m##y##10(AM_16BIT |  Delta_nu<<1);                                     \
                                                                    \
  _set_m##x##01(AM_16BIT |( 1 +  Delta_nu     - (N>>1)) & 0x3fff);            \
  _set_m##x##11(AM_16BIT |( 1 +  Delta_nu     - (N>>1)) & 0x3fff);            \
  _set_m##x##21(AM_16BIT |(  1 -  Delta_nu     + (N>>1)));                      \
  _set_m##y##01(AM_16BIT |( 1 + (Delta_nu<<1) -  N)     & 0x3fff);            \
  _set_m##y##11(AM_16BIT |(-1 + (Delta_nu<<1) -  N)     & 0x3fff);            \
                                                                    \
  _fct4_##x##2##y##_k(Delta_nu, N_nu, c1, c2, c2m);                 \
                                                                    \
} /* _FCT4_I_K */

// N/4 2
#define _FCT4_I_KE1(x, y, N, in, out, coef) {                       \
                                                                    \
  _set_a##x##0(in);                                                 \
  _set_a##x##1(in + (N>>1));                                        \
  _set_a##x##2(in + N - (N>>2));                                    \
  _set_a##y##0(out);                                                \
  _set_a##y##1(out + (N>>1)-1);                                     \
  _set_a##y##3(coef + (N>>2)-1);                                    \
                                                                    \
_set_m##x##00( AM_16BIT |  N>>2);/**/                                     \
_set_m##x##10( AM_16BIT |  N>>2);/**/                                     \
  _set_m##x##20(AM_16BIT |(-(N>>2))     & 0x3fff);                            \
  _set_m##y##00(AM_16BIT |   N>>1);                                           \
  _set_m##y##10(AM_16BIT |   N>>1);                                           \
                                                                    \
  _set_m##x##01(AM_16BIT |( 1 - (N>>2)) & 0x3fff);                            \
  _set_m##x##11(AM_16BIT |( 1 - (N>>2)) & 0x3fff);                            \
  _set_m##x##21(AM_16BIT |  1 + (N>>2));                                      \
  _set_m##y##01(AM_16BIT |( 1 - (N>>1)) & 0x3fff);                            \
  _set_m##y##11(AM_16BIT |(-1 - (N>>1)) & 0x3fff);                            \
                                                                    \
  _fct4_##x##2##y##_ke1((N>>2), c1, c2, c2m);                       \
                                                                    \
} /* _FCT4_I_KE1 */

// N/2 1
#define _FCT4_I_KE(x, y, N, in, out, coef) {                        \
                                                                    \
  _set_a##x##0(in);                                                 \
  _set_a##x##1(in + (N>>1));                                        \
  _set_a##x##2(in + (N>>1));                                        \
  _set_a##y##0(out);                                                \
  _set_a##y##1(out + N-1);                                          \
  _set_a##y##3(coef + (N>>1)-1);                                    \
                                                                    \
  _set_m##x##00(AM_16BIT |  1);                                               \
  _set_m##x##10(AM_16BIT |  1);                                               \
  _set_m##x##20(AM_16BIT |  1);                                               \
  _set_m##y##00(AM_16BIT |  1);                                               \
  _set_m##y##10(AM_16BIT |(-1) & 0x3fff);                                     \
                                                                    \
  _fct4_##x##2##y##_ke((N>>1), c1, c2, c2m);                        \
                                                                    \
} /* _FCT4_I_KE */

#define _FCT4_I_LM(x, y, z, N, in, out, coef, st) {                 \
                                                                    \
  _set_a##x##0(in);                                                 \
  _set_a##y##1(out);                                                \
  _set_a##z##3(coef + N-1);                                         \
                                                                    \
  _set_m##x##00(AM_16BIT |1);                                                 \
  _set_m##y##10(AM_16BIT |1*st);                                              \
  _nop(); \
  _nop(); \
                                                                    \
  _fct4_##x##2##y##_##z##_lm(N);                                    \
                                                                    \
} /* _FCT4_I_LM */

static _Inline void _fct4_x2y_copy (int32_t N1, int32_t x_in, int32_t y_out){
  int32_t i;
  _set_ax3(x_in);
  _set_ay3(y_out);
  _nop();
  _nop();
  for (i = 0; i < N1; i++){
    _set_y3_u0(_x3_u0());
	_set_y3_u0(_x3_u0());
  }
}
static _Inline void _fct4_y2x_copy (int32_t N1, int32_t y_in, int32_t x_out){
  int32_t i;
  _set_ay3(y_in);
  _set_ax3(x_out);
  _nop();
  _nop();
  for (i = 0; i < N1; i++){
    _set_x3_u0(_y3_u0());
	_set_x3_u0(_y3_u0());
  }
}

/**
 *  Fast DCT Type 4 transform "process" function for q15 data type
 *  Scaled version
 *
 * \param inst - pointer to instance structure
 * \param offs_inout[in,out] - X memory offset to input/output vector
 * \param offs_scratch - Y memory offset to scratch vector buffer
 */

void dsp_fct4_scaled_q15_XtoX_scYcfXY_MAC32x16(fct4_q15_xy_t *inst, uint32_t offs_inout, uint32_t offs_scratch) {
int32_t c   = SQRT2,
        c1  = M_ONE,
        c2  =   HALF,
        c2m = M_HALF;

int nu, N, N1, s, twf;
  N = inst->length;
  N1 = N>>1;
  twf = inst->xyptr_twf;
  s = 30 - dspfx_norm_q31(N);
  //time0 = _lr(0x100);
  _set_mx30(1 | (1<<29));
  _set_my30(1 | (1<<29));
  nu = 0; _FCT4_I_K1 (x, y, N, offs_inout, offs_scratch, 1)
  for ( ; nu < s-3; ) {
    nu++; _FCT4_I_K  (y, x, N, (1<<nu), (N1>>nu), offs_scratch, offs_inout, twf)
      if (nu == s-3) break;
    nu++; _FCT4_I_K  (x, y, N, (1<<nu), (N1>>nu), offs_inout, offs_scratch, twf)
  }
  if (s & 1) {
    _FCT4_I_KE1(y, x,    N, offs_scratch, offs_inout, twf)
    _FCT4_I_KE (x, y,    N, offs_inout,   offs_scratch, twf)
    _FCT4_I_LM (y, x, x, N, offs_scratch, offs_inout, twf, 1)
  } else {
    _FCT4_I_KE1(x, y,    N, offs_inout,   offs_scratch, twf)
    _FCT4_I_KE (y, x,    N, offs_scratch, offs_inout, twf)
    _FCT4_I_LM (x, y, y, N, offs_inout,   offs_scratch, twf, 1)
	_fct4_y2x_copy(N1, offs_scratch, offs_inout);
  }

}

/**
 *  Fast DCT Type 4 transform "process" function for q15 data type
 *  Scaled version.
 *
 * \param inst - pointer to instance structure
 * \param offs_inout[in,out] - Y memory offset to input/output vector
 * \param offs_scratch - X memory offset to scratch vector buffer
 */

void dsp_fct4_scaled_q15_YtoY_scXcfXY_MAC32x16(fct4_q15_xy_t *inst, uint32_t offs_inout, uint32_t offs_scratch) {
int32_t c   = SQRT2,
        c1  = M_ONE,
        c2  =   HALF,
        c2m = M_HALF;

int nu, N, N1, s, twf;
  N = inst->length;
  N1 = N>>1;
  twf = inst->xyptr_twf;
  s = 30 - dspfx_norm_q31(N);
  //time0 = _lr(0x100);
  _set_mx30(1 | (1<<29));
  _set_my30(1 | (1<<29));
  nu = 0; _FCT4_I_K1 (y, x, N, offs_inout, offs_scratch, 1)
  for ( ; nu < s-3; ) {
    nu++; _FCT4_I_K  (x, y, N, (1<<nu), (N1>>nu), offs_scratch, offs_inout, twf)
      if (nu == s-3) break;
    nu++; _FCT4_I_K  (y, x, N, (1<<nu), (N1>>nu), offs_inout, offs_scratch, twf)
  }
  if (s & 1) {
    _FCT4_I_KE1(x, y,    N, offs_scratch, offs_inout, twf)
    _FCT4_I_KE (y, x,    N, offs_inout,   offs_scratch, twf)
    _FCT4_I_LM (x, y, y, N, offs_scratch, offs_inout, twf, 1)
  } else {
    _FCT4_I_KE1(y, x,    N, offs_inout,   offs_scratch, twf)
    _FCT4_I_KE (x, y,    N, offs_scratch, offs_inout, twf)
    _FCT4_I_LM (y, x, x, N, offs_inout,   offs_scratch, twf, 1)
	_fct4_x2y_copy(N1, offs_scratch, offs_inout);
  }

}

/**
 * Fast DCT Type 4 transform "process" function for q15 data type
 * Scaled version.
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - X memory offset to input vector
 * \param offs_output[out] - Y memory offset to output vector
 */

void dsp_fct4_scaled_q15_XtoY_cfXY_MAC32x16(fct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output) {
int32_t c   = SQRT2,
        c1  = M_ONE,
        c2  =   HALF,
        c2m = M_HALF;

int nu, N, N1, s, twf;
  N = inst->length;
  N1 = N>>1;
  twf = inst->xyptr_twf;
  s = 30 - dspfx_norm_q31(N);
  //time0 = _lr(0x100);
  _set_mx30(1 | (1<<29));
  _set_my30(1 | (1<<29));
  nu = 0; _FCT4_I_K1 (x, y, N, offs_input, offs_output, 1)
  for ( ; nu < s-3; ) {
    nu++; _FCT4_I_K  (y, x, N, (1<<nu), (N1>>nu), offs_output, offs_input, twf)
      if (nu == s-3) break;
    nu++; _FCT4_I_K  (x, y, N, (1<<nu), (N1>>nu), offs_input, offs_output, twf)
  }
  if (s & 1) {
    _FCT4_I_KE1(y, x,    N, offs_output, offs_input, twf)
    _FCT4_I_KE (x, y,    N, offs_input,   offs_output, twf)
    _FCT4_I_LM (y, x, x, N, offs_output, offs_input, twf, 1)
	_fct4_x2y_copy(N1, offs_input, offs_output);
  } else {
    _FCT4_I_KE1(x, y,    N, offs_input,   offs_output, twf)
    _FCT4_I_KE (y, x,    N, offs_output, offs_input, twf)
    _FCT4_I_LM (x, y, y, N, offs_input,   offs_output, twf, 1)

  }

}

/**
 * Fast DCT Type 4 transform "process" function for q15 data type
 * Scaled version.
 *
 * \param inst - pointer to instance structure
 * \param offs_input[in] - Y memory offset to input vector
 * \param offs_output[out] - X memory offset to output vector
 */

void dsp_fct4_scaled_q15_YtoX_cfXY_MAC32x16(fct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output) {
int32_t c   = SQRT2,
        c1  = M_ONE,
        c2  =   HALF,
        c2m = M_HALF;

int nu, N, N1, s, twf;
  N = inst->length;
  N1 = N>>1;
  twf = inst->xyptr_twf;
  s = 30 - dspfx_norm_q31(N);
  //time0 = _lr(0x100);
  _set_mx30(1 | (1<<29));
  _set_my30(1 | (1<<29));
  nu = 0; _FCT4_I_K1 (y, x, N, offs_input, offs_output, 1)
  for ( ; nu < s-3; ) {
    nu++; _FCT4_I_K  (x, y, N, (1<<nu), (N1>>nu), offs_output, offs_input, twf)
      if (nu == s-3) break;
    nu++; _FCT4_I_K  (y, x, N, (1<<nu), (N1>>nu), offs_input, offs_output, twf)
  }
  if (s & 1) {
    _FCT4_I_KE1(x, y,    N, offs_output, offs_input, twf)
    _FCT4_I_KE (y, x,    N, offs_input,   offs_output, twf)
    _FCT4_I_LM (x, y, y, N, offs_output, offs_input, twf, 1)
	_fct4_y2x_copy(N1, offs_input, offs_output);
  } else {
    _FCT4_I_KE1(y, x,    N, offs_input,   offs_output, twf)
    _FCT4_I_KE (x, y,    N, offs_output, offs_input, twf)
    _FCT4_I_LM (y, x, x, N, offs_input,   offs_output, twf, 1)

  }

}
#pragma Code()

#endif
