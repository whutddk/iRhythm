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

#ifndef ARC_DSPLIB_LL_API_H
#define ARC_DSPLIB_LL_API_H

#include "dsplib_types.h"
//
#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
// 
void dsp_abs_q31_XtoY(int32_t offs_Src, int32_t offs_Dst, uint32_t nSamples);
void dsp_abs_q31_YtoX(int32_t offs_Src, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_add_q31_XXtoY(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples);
void dsp_add_q31_YYtoX(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_sub_q31_XXtoY(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples);
void dsp_sub_q31_YYtoX(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_dot_prod_q31_XY_MAC32x32(int32_t offs_SrcA, int32_t offs_SrcB, uint32_t nSamples, q63_t *result);
void dsp_dot_prod_q31_YX_MAC32x32(int32_t offs_SrcA, int32_t offs_SrcB, uint32_t nSamples, q63_t *result);
void dsp_dot_prod_q31_XY_MAC24x24(int32_t offs_SrcA, int32_t offs_SrcB, uint32_t nSamples, q63_t *result);
void dsp_dot_prod_q31_YX_MAC24x24(int32_t offs_SrcA, int32_t offs_SrcB, uint32_t nSamples, q63_t *result);
// 
void dsp_offset_q31_XtoY(int32_t offs_Src, q31_t offset, int32_t offs_Dst, uint32_t nSamples);
void dsp_offset_q31_YtoX(int32_t offs_Src, q31_t offset, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_negate_q31_XtoY(int32_t offs_Src, int32_t offs_Dst, uint32_t nSamples);
void dsp_negate_q31_YtoX(int32_t offs_Src, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_scale_q31_XtoY_MAC32x32(int32_t offs_Src, q31_t scaleFract, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples);
void dsp_scale_q31_YtoX_MAC32x32(int32_t offs_Src, q31_t scaleFract, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_mult_q31_XXtoY_MAC32x32(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples);
void dsp_mult_q31_YYtoX_MAC32x32(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_shift_q31_XtoX(int32_t offs_Src, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples);
void dsp_shift_q31_YtoX(int32_t offs_Src, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples);
void dsp_shift_q31_XtoY(int32_t offs_Src, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples);
void dsp_shift_q31_YtoY(int32_t offs_Src, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_min_q31_X(int32_t offs_Src, uint32_t nSamples, q31_t *pResult, uint32_t *pIndex);
void dsp_min_q31_Y(int32_t offs_Src, uint32_t nSamples, q31_t *pResult, uint32_t *pIndex);
// 
void dsp_max_q31_X(int32_t offs_Src, uint32_t nSamples, q31_t *pResult, uint32_t *pIndex);
void dsp_max_q31_Y(int32_t offs_Src, uint32_t nSamples, q31_t *pResult, uint32_t *pIndex);
// 
void dsp_cmpl_conj_q31_XtoY(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples);
void dsp_cmpl_conj_q31_YtoX(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples);
// 
void dsp_cmpl_mag_q31_XtoY_MAC32x32(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples);
void dsp_cmpl_mag_q31_YtoX_MAC32x32(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples);
// 
void dsp_cmpl_mag_squared_q31_XtoY_MAC32x32(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples);
void dsp_cmpl_mag_squared_q31_YtoX_MAC32x32(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples);
// 
void dsp_cmpl_mult_cmpl_q31_XXtoY_MAC32x32(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t offs_Dst, uint32_t nSamples);
void dsp_cmpl_mult_cmpl_q31_YYtoX_MAC32x32(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t offs_Dst, uint32_t nSamples);
// 
void dsp_cmpl_dot_prod_q31_XX_MAC32x32(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t nSamples, int64_t *realResult, int64_t *imagResult);
void dsp_cmpl_dot_prod_q31_YY_MAC32x32(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t nSamples, int64_t *realResult, int64_t *imagResult);
// 
void dsp_cmpl_mult_real_q31_XXtoY_MAC32x32(uint32_t pSrcComplex, uint32_t pSrcReal, uint32_t offs_Dst, uint32_t nSamples);
void dsp_cmpl_mult_real_q31_YYtoX_MAC32x32(uint32_t pSrcComplex, uint32_t pSrcReal, uint32_t offs_Dst, uint32_t nSamples);
// 
dsp_status dsp_mat_add_q31_XXtoY(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst);
dsp_status dsp_mat_add_q31_YYtoX(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst);
// 
dsp_status dsp_mat_sub_q31_XXtoY(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst);
dsp_status dsp_mat_sub_q31_YYtoX(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst);
// 
dsp_status dsp_mat_trans_q31_XtoY(const matrix_q31_t *pSrc, matrix_q31_t *pDst);
dsp_status dsp_mat_trans_q31_YtoX(const matrix_q31_t *pSrc, matrix_q31_t *pDst);
// 
dsp_status dsp_mat_scale_q31_XtoY_MAC32x32(const matrix_q31_t *pSrc, q31_t factor, int32_t shift, matrix_q31_t *pDst);
dsp_status dsp_mat_scale_q31_YtoX_MAC32x32(const matrix_q31_t *pSrc, q31_t factor, int32_t shift, matrix_q31_t *pDst);
// 
dsp_status dsp_mat_mult_q31_XXtoY_MAC32x32(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst);
dsp_status dsp_mat_mult_q31_YYtoX_MAC32x32(const matrix_q31_t *pSrcA, const matrix_q31_t *pSrcB, matrix_q31_t *pDst);
// 
void dsp_mdct_preprocess_q31_XtoY(uint32_t offs_input, uint32_t offs_output, uint32_t length);
void dsp_mdct_preprocess_q31_YtoX(uint32_t offs_input, uint32_t offs_output, uint32_t length);
// 
void dsp_imdct_postprocess_q31_XtoY(uint32_t offs_input, uint32_t offs_output, uint32_t length);
void dsp_imdct_postprocess_q31_YtoX(uint32_t offs_input, uint32_t offs_output, uint32_t length);
// 
void dsp_rifft_preprocess_q31_XtoY_cfX_MAC32x32(const rfft_q31_xy_t *inst,  uint32_t offs_input , uint32_t offs_output);
void dsp_rifft_preprocess_q31_YtoX_cfY_MAC32x32(const rfft_q31_xy_t *inst,  uint32_t offs_input , uint32_t offs_output);
void dsp_rifft_preprocess_q31_XtoY_cfX_MAC24x24(const rfft_q31_xy_t *inst,  uint32_t offs_input , uint32_t offs_output);
void dsp_rifft_preprocess_q31_YtoX_cfY_MAC24x24(const rfft_q31_xy_t *inst,  uint32_t offs_input , uint32_t offs_output);
dsp_status dsp_rifft_preprocess_init_q31_cfX(rfft_q31_xy_t *inst, uint32_t length, uint32_t offs_twd);
dsp_status dsp_rifft_preprocess_init_q31_cfY(rfft_q31_xy_t *inst, uint32_t length, uint32_t offs_twd);
// 
void dsp_rfft_postprocess_q31_XtoY_cfX_MAC32x32(const rfft_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_rfft_postprocess_q31_YtoX_cfY_MAC32x32(const rfft_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_rfft_postprocess_q31_XtoY_cfX_MAC24x24(const rfft_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_rfft_postprocess_q31_YtoX_cfY_MAC24x24(const rfft_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
dsp_status dsp_rfft_postprocess_init_q31_cfX(rfft_q31_xy_t *inst, uint32_t length, uint32_t offs_twd);
dsp_status dsp_rfft_postprocess_init_q31_cfY(rfft_q31_xy_t *inst, uint32_t length, uint32_t offs_twd);
// 
void dsp_cfft_sradix_q31_XtoX_cfY_MAC32x32(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cfft_sradix_q31_YtoY_cfX_MAC32x32(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cfft_sradix_q31_XtoX_cfY_MAC24x24(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cfft_sradix_q31_YtoY_cfX_MAC24x24(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cfft_sradix_q31_XtoY_cfX_MAC32x32(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cfft_sradix_q31_YtoX_cfY_MAC32x32(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cfft_sradix_q31_XtoY_cfX_MAC24x24(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cfft_sradix_q31_YtoX_cfY_MAC24x24(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
dsp_status dsp_cfft_sradix_init_q31_cfX(cfft_srad_q31_xy_t *inst, uint32_t length, uint32_t offs_twiddle);
dsp_status dsp_cfft_sradix_init_q31_cfY(cfft_srad_q31_xy_t *inst, uint32_t length, uint32_t offs_twiddle);
// 
void dsp_cifft_sradix_q31_XtoX_cfY_MAC32x32(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cifft_sradix_q31_YtoY_cfX_MAC32x32(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cifft_sradix_q31_XtoX_cfY_MAC24x24(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cifft_sradix_q31_YtoY_cfX_MAC24x24(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cifft_sradix_q31_XtoY_cfX_MAC32x32(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cifft_sradix_q31_YtoX_cfY_MAC32x32(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cifft_sradix_q31_XtoY_cfX_MAC24x24(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cifft_sradix_q31_YtoX_cfY_MAC24x24(const cfft_srad_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
dsp_status dsp_cifft_sradix_init_q31_cfX(cfft_srad_q31_xy_t *inst, uint32_t length, uint32_t offs_twiddle);
dsp_status dsp_cifft_sradix_init_q31_cfY(cfft_srad_q31_xy_t *inst, uint32_t length, uint32_t offs_twiddle);
// 
dsp_status dsp_dct2_postprocess_init_q31_cfX(dct2_q31_xy_t *inst, uint32_t length, uint32_t offs_twd);
dsp_status dsp_dct2_postprocess_init_q31_cfY(dct2_q31_xy_t *inst, uint32_t length, uint32_t offs_twd);
void  dsp_dct2_preprocess_q31_XtoY(uint32_t offs_input, uint32_t offs_output, uint32_t length);
void  dsp_dct2_preprocess_q31_YtoX(uint32_t offs_input, uint32_t offs_output, uint32_t length);
void dsp_dct2_postprocess_q31_XtoY_cfY_MAC32x32(const dct2_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct2_postprocess_q31_YtoX_cfX_MAC32x32(const dct2_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct2_postprocess_q31_XtoY_cfY_MAC24x24(const dct2_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct2_postprocess_q31_YtoX_cfX_MAC24x24(const dct2_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
// 
dsp_status dsp_dct3_preprocess_init_q31_cfX(dct3_q31_xy_t *inst, uint32_t length, uint32_t offs_twd);
dsp_status dsp_dct3_preprocess_init_q31_cfY(dct3_q31_xy_t *inst, uint32_t length, uint32_t offs_twd);
void dsp_dct3_preprocess_q31_XtoY_cfX_MAC32x32(const dct3_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct3_preprocess_q31_YtoX_cfY_MAC32x32(const dct3_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct3_preprocess_q31_XtoY_cfX_MAC24x24(const dct3_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct3_preprocess_q31_YtoX_cfY_MAC24x24(const dct3_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct3_postprocess_q31_XtoY(uint32_t offs_input, uint32_t offs_output, uint32_t length);
void dsp_dct3_postprocess_q31_YtoX(uint32_t offs_input, uint32_t offs_output, uint32_t length);
// 
void dsp_dct4_preprocess_init_q31_cfX(dct4_q31_xy_t *inst, int32_t length, int32_t offs_precoeffs);
void dsp_dct4_preprocess_init_q31_cfY(dct4_q31_xy_t *inst, int32_t length, int32_t offs_precoeffs);
void dsp_dct4_preprocess_q31_XtoX_cfY_MAC32x32(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct4_preprocess_q31_YtoY_cfX_MAC32x32(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct4_preprocess_q31_XtoX_cfY_MAC24x24(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct4_preprocess_q31_YtoY_cfX_MAC24x24(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct4_postprocess_init_q31_cfX(dct4_q31_xy_t *inst, int32_t length, int32_t offs_postcoeffs);
void dsp_dct4_postprocess_init_q31_cfY(dct4_q31_xy_t *inst, int32_t length, int32_t offs_postcoeffs);
void dsp_dct4_postprocess_q31_XtoX_cfY_MAC32x32(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct4_postprocess_q31_YtoY_cfX_MAC32x32(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct4_postprocess_q31_XtoX_cfY_MAC24x24(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct4_postprocess_q31_YtoY_cfX_MAC24x24(const dct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
// 
void dsp_biquad_cascade_df1_q31_X_cfYstY_MAC32x32(  const iir_biquad_q31_xy_t *inst,    uint32_t offs_inout,   uint32_t nSamples);
void dsp_biquad_cascade_df1_q31_Y_cfXstX_MAC32x32(  const iir_biquad_q31_xy_t *inst,    uint32_t offs_inout,   uint32_t nSamples);
dsp_status dsp_biquad_cascade_df1_init_q31_cfXstX(iir_biquad_q31_xy_t *inst, uint32_t numStages, uint32_t pshift, uint32_t offs_coeffs, uint32_t offs_states);
dsp_status dsp_biquad_cascade_df1_init_q31_cfYstY(iir_biquad_q31_xy_t *inst, uint32_t numStages, uint32_t pshift, uint32_t offs_coeffs, uint32_t offs_states);
// 
dsp_status dsp_iir_lattice_init_q31_cfXstX(iir_lattice_q31_xy_t *inst, uint16_t nStages, uint32_t offs_Kcoeffs, uint32_t offs_Vcoeffs, uint32_t offs_states);
dsp_status dsp_iir_lattice_init_q31_cfYstY(iir_lattice_q31_xy_t *inst, uint16_t nStages, uint32_t offs_Kcoeffs, uint32_t offs_Vcoeffs, uint32_t offs_states);
void dsp_iir_lattice_q31_X_cfYstY_MAC32x32(const iir_lattice_q31_xy_t *inst, uint32_t offs_inout, uint32_t nSamples);
void dsp_iir_lattice_q31_Y_cfXstX_MAC32x32(const iir_lattice_q31_xy_t *inst, uint32_t offs_inout, uint32_t nSamples);
// 
void dsp_fir_q31_XtoX_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_YtoX_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_XtoX_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_YtoX_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_XtoY_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_YtoY_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_XtoY_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_YtoY_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
dsp_status dsp_fir_init_q31_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint16_t nTaps,  uint32_t offs_coeffs, uint32_t offs_states);
dsp_status dsp_fir_init_q31_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint16_t nTaps,  uint32_t offs_coeffs, uint32_t offs_states);
// 
void dsp_fir_q31_complex_XtoX_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_complex_YtoX_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_complex_XtoX_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_complex_YtoX_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_complex_XtoY_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_complex_YtoY_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_complex_XtoY_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q31_complex_YtoY_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
dsp_status dsp_fir_init_q31_complex_cfXstY_MAC32x32(fir_q31_xy_t *inst, uint16_t nTaps,  uint32_t offs_coeffs, uint32_t offs_states);
dsp_status dsp_fir_init_q31_complex_cfYstX_MAC32x32(fir_q31_xy_t *inst, uint16_t nTaps,  uint32_t offs_coeffs, uint32_t offs_states);
// 
void dsp_correlate_q31_XYtoX_MAC32x32(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
void dsp_correlate_q31_YXtoX_MAC32x32(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
void dsp_correlate_q31_XYtoY_MAC32x32(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
void dsp_correlate_q31_YXtoY_MAC32x32(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
// 
void dsp_conv_q31_XYtoX_MAC32x32(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
void dsp_conv_q31_YXtoX_MAC32x32(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
void dsp_conv_q31_XYtoY_MAC32x32(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
void dsp_conv_q31_YXtoY_MAC32x32(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
// 
dsp_status dsp_lms_init_q31_cfXstY_MAC32x32(lms_q31_xy_t *inst,  uint16_t nTaps,  q31_t mu, uint32_t postshift,  uint32_t  offs_coeffs,  uint32_t offs_states);
dsp_status dsp_lms_init_q31_cfYstX_MAC32x32(lms_q31_xy_t *inst,  uint16_t nTaps,  q31_t mu, uint32_t postshift,  uint32_t  offs_coeffs,  uint32_t offs_states);
void dsp_lms_q31_XY_cfYstX_MAC32x32(lms_q31_xy_t *inst,    uint32_t offs_inout,    uint32_t offs_referr,    uint32_t nSamples);
void dsp_lms_q31_YX_cfXstY_MAC32x32(lms_q31_xy_t *inst,    uint32_t offs_inout,    uint32_t offs_referr,    uint32_t nSamples);
// 
void dsp_fir_interpolate_q31_XtoX_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q31_YtoX_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q31_XtoX_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q31_YtoX_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q31_XtoY_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q31_YtoY_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q31_XtoY_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q31_YtoY_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
dsp_status dsp_fir_interpolate_init_q31_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint16_t nTaps, uint8_t intFactor,  uint32_t offs_coeffs, uint32_t offs_states);
dsp_status dsp_fir_interpolate_init_q31_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint16_t nTaps, uint8_t intFactor,  uint32_t offs_coeffs, uint32_t offs_states);
// 
void dsp_fir_decimate_q31_XtoX_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q31_YtoX_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q31_XtoX_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q31_YtoX_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q31_XtoY_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q31_YtoY_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q31_XtoY_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q31_YtoY_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
dsp_status dsp_fir_decimate_init_q31_cfXstY_MAC32x32(fir_intdec_q31_xy_t *inst, uint16_t nTaps, uint8_t decFactor,  uint32_t offs_coeffs, uint32_t offs_states);
dsp_status dsp_fir_decimate_init_q31_cfYstX_MAC32x32(fir_intdec_q31_xy_t *inst, uint16_t nTaps, uint8_t decFactor,  uint32_t offs_coeffs, uint32_t offs_states);
//
dsp_status dsp_fct4_init_q31_cfXY(fct4_q31_xy_t *inst, int32_t length, int32_t xyptr_twf);
void dsp_fct4_q31_XtoX_scYcfXY_MAC32x32(fct4_q31_xy_t *inst, uint32_t offs_inout, uint32_t offs_scratch);
void dsp_fct4_q31_YtoY_scXcfXY_MAC32x32(fct4_q31_xy_t *inst, uint32_t offs_inout, uint32_t offs_scratch);
void dsp_fct4_q31_XtoY_cfXY_MAC32x32(fct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_fct4_q31_YtoX_cfXY_MAC32x32(fct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
//
void dsp_fct4_scaled_q31_XtoX_scYcfXY_MAC32x32(fct4_q31_xy_t *inst, uint32_t offs_inout, uint32_t offs_scratch);
void dsp_fct4_scaled_q31_YtoY_scXcfXY_MAC32x32(fct4_q31_xy_t *inst, uint32_t offs_inout, uint32_t offs_scratch);
void dsp_fct4_scaled_q31_XtoY_cfXY_MAC32x32(fct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_fct4_scaled_q31_YtoX_cfXY_MAC32x32(fct4_q31_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
// 
void dsp_abs_q15_XtoY(int32_t offs_Src, int32_t offs_Dst, uint32_t nSamples);
void dsp_abs_q15_YtoX(int32_t offs_Src, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_add_q15_XXtoY(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples);
void dsp_add_q15_YYtoX(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_sub_q15_XXtoY(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples);
void dsp_sub_q15_YYtoX(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_dot_prod_q15_XY_MAC32x32(int32_t offs_SrcA, int32_t offs_SrcB, uint32_t nSamples, q63_t *result);
void dsp_dot_prod_q15_YX_MAC32x32(int32_t offs_SrcA, int32_t offs_SrcB, uint32_t nSamples, q63_t *result);
// 
void dsp_offset_q15_XtoY(int32_t offs_Src, q15_t offset, int32_t offs_Dst, uint32_t nSamples);
void dsp_offset_q15_YtoX(int32_t offs_Src, q15_t offset, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_negate_q15_XtoY(int32_t offs_Src, int32_t offs_Dst, uint32_t nSamples);
void dsp_negate_q15_YtoX(int32_t offs_Src, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_scale_q15_XtoY_MAC16x16(int32_t offs_Src, q15_t scaleFract, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples);
void dsp_scale_q15_YtoX_MAC16x16(int32_t offs_Src, q15_t scaleFract, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_mult_q15_XXtoY_MAC16x16(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples);
void dsp_mult_q15_YYtoX_MAC16x16(int32_t offs_SrcA, int32_t offs_SrcB, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_shift_q15_XtoX(int32_t offs_Src, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples);
void dsp_shift_q15_YtoX(int32_t offs_Src, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples);
void dsp_shift_q15_XtoY(int32_t offs_Src, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples);
void dsp_shift_q15_YtoY(int32_t offs_Src, int8_t shiftBits, int32_t offs_Dst, uint32_t nSamples);
// 
void dsp_min_q15_X(int32_t offs_Src, uint32_t nSamples, q15_t *pResult, uint32_t *pIndex);
void dsp_min_q15_Y(int32_t offs_Src, uint32_t nSamples, q15_t *pResult, uint32_t *pIndex);
// 
void dsp_max_q15_X(int32_t offs_Src, uint32_t nSamples, q15_t *pResult, uint32_t *pIndex);
void dsp_max_q15_Y(int32_t offs_Src, uint32_t nSamples, q15_t *pResult, uint32_t *pIndex);
// 
void dsp_cmpl_conj_q15_XtoY(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples);
void dsp_cmpl_conj_q15_YtoX(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples);
// 
void dsp_cmpl_mag_q15_XtoY_MAC16x16(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples);
void dsp_cmpl_mag_q15_YtoX_MAC16x16(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples);
// 
void dsp_cmpl_mag_squared_q15_XtoY_MAC16x16(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples);
void dsp_cmpl_mag_squared_q15_YtoX_MAC16x16(uint32_t offs_Src, uint32_t offs_Dst, uint32_t nSamples);
// 
void dsp_cmpl_mult_cmpl_q15_XXtoY_DMAC16x16(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t offs_Dst, uint32_t nSamples);
void dsp_cmpl_mult_cmpl_q15_YYtoX_DMAC16x16(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t offs_Dst, uint32_t nSamples);
// 
void dsp_cmpl_dot_prod_q15_XX_DMAC16x16(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t nSamples, int32_t *realResult, int32_t *imagResult);
void dsp_cmpl_dot_prod_q15_YY_DMAC16x16(uint32_t offs_SrcA, uint32_t offs_SrcB, uint32_t nSamples, int32_t *realResult, int32_t *imagResult);
// 
void dsp_cmpl_mult_real_q15_XXtoY_DMAC16x16(uint32_t pSrcComplex, uint32_t pSrcReal, uint32_t offs_Dst, uint32_t nSamples);
void dsp_cmpl_mult_real_q15_YYtoX_DMAC16x16(uint32_t pSrcComplex, uint32_t pSrcReal, uint32_t offs_Dst, uint32_t nSamples);
// 
dsp_status dsp_mat_add_q15_XXtoY(const matrix_q15_t *pSrcA, const matrix_q15_t *pSrcB, matrix_q15_t *pDst);
dsp_status dsp_mat_add_q15_YYtoX(const matrix_q15_t *pSrcA, const matrix_q15_t *pSrcB, matrix_q15_t *pDst);
// 
dsp_status dsp_mat_sub_q15_XXtoY(const matrix_q15_t *pSrcA, const matrix_q15_t *pSrcB, matrix_q15_t *pDst);
dsp_status dsp_mat_sub_q15_YYtoX(const matrix_q15_t *pSrcA, const matrix_q15_t *pSrcB, matrix_q15_t *pDst);
// 
dsp_status dsp_mat_trans_q15_XtoY(const matrix_q15_t *pSrc, matrix_q15_t *pDst);
dsp_status dsp_mat_trans_q15_YtoX(const matrix_q15_t *pSrc, matrix_q15_t *pDst);
// 
dsp_status dsp_mat_scale_q15_XtoY_MAC16x16(const matrix_q15_t *pSrc, q15_t factor, int32_t shift, matrix_q15_t *pDst);
dsp_status dsp_mat_scale_q15_YtoX_MAC16x16(const matrix_q15_t *pSrc, q15_t factor, int32_t shift, matrix_q15_t *pDst);
// 
dsp_status dsp_mat_mult_q15_XXtoY_MAC16x16(const matrix_q15_t *pSrcA, const matrix_q15_t *pSrcB, matrix_q15_t *pDst);
dsp_status dsp_mat_mult_q15_YYtoX_MAC16x16(const matrix_q15_t *pSrcA, const matrix_q15_t *pSrcB, matrix_q15_t *pDst);
// 
void dsp_mdct_preprocess_q15_XtoY(uint32_t offs_input, uint32_t offs_output, uint32_t length);
void dsp_mdct_preprocess_q15_YtoX(uint32_t offs_input, uint32_t offs_output, uint32_t length);
// 
void dsp_imdct_postprocess_q15_XtoY(uint32_t offs_input, uint32_t offs_output, uint32_t length);
void dsp_imdct_postprocess_q15_YtoX(uint32_t offs_input, uint32_t offs_output, uint32_t length);
// 
void dsp_rifft_preprocess_q15_XtoY_cfX_MAC16x16(const rfft_q15_xy_t *inst, uint32_t offs_input , uint32_t offs_output);
void dsp_rifft_preprocess_q15_YtoX_cfY_MAC16x16(const rfft_q15_xy_t *inst, uint32_t offs_input , uint32_t offs_output);
dsp_status dsp_rifft_preprocess_init_q15_cfX(rfft_q15_xy_t *inst, uint32_t length, uint32_t offs_twd);
dsp_status dsp_rifft_preprocess_init_q15_cfY(rfft_q15_xy_t *inst, uint32_t length, uint32_t offs_twd);
// 
void dsp_rfft_postprocess_q15_XtoY_cfX_MAC16x16(const rfft_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_rfft_postprocess_q15_YtoX_cfY_MAC16x16(const rfft_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
dsp_status dsp_rfft_postprocess_init_q15_cfX(rfft_q15_xy_t *inst, uint32_t length, uint32_t offs_twd);
dsp_status dsp_rfft_postprocess_init_q15_cfY(rfft_q15_xy_t *inst, uint32_t length, uint32_t offs_twd);
// 
void dsp_cfft_radix2_q15_XtoX_cfY_DMAC16x16(const cfft_rad2_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cfft_radix2_q15_YtoY_cfX_DMAC16x16(const cfft_rad2_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cfft_radix2_q15_XtoY_cfX_DMAC16x16(const cfft_rad2_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_cfft_radix2_q15_YtoX_cfY_DMAC16x16(const cfft_rad2_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
dsp_status dsp_cfft_radix2_init_q15_cfX_DMAC16x16(cfft_rad2_q15_xy_t *inst, uint32_t length, uint32_t offs_twiddles);
dsp_status dsp_cfft_radix2_init_q15_cfY_DMAC16x16(cfft_rad2_q15_xy_t *inst, uint32_t length, uint32_t offs_twiddles);
dsp_status dsp_cifft_radix2_init_q15_cfX_DMAC16x16(cfft_rad2_q15_xy_t *inst, uint32_t length, uint32_t offs_twiddles);
dsp_status dsp_cifft_radix2_init_q15_cfY_DMAC16x16(cfft_rad2_q15_xy_t *inst, uint32_t length, uint32_t offs_twiddles);
// 
dsp_status dsp_dct2_postprocess_init_q15_cfX(dct2_q15_xy_t *inst, uint32_t length, uint32_t offs_twd);
dsp_status dsp_dct2_postprocess_init_q15_cfY(dct2_q15_xy_t *inst, uint32_t length, uint32_t offs_twd);
void  dsp_dct2_preprocess_q15_XtoY(uint32_t offs_input, uint32_t offs_output, uint32_t length);
void  dsp_dct2_preprocess_q15_YtoX(uint32_t offs_input, uint32_t offs_output, uint32_t length);
void dsp_dct2_postprocess_q15_XtoY_cfY_MAC16x16(const dct2_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct2_postprocess_q15_YtoX_cfX_MAC16x16(const dct2_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
// 
dsp_status dsp_dct3_preprocess_init_q15_cfX(dct3_q15_xy_t *inst, uint32_t length, uint32_t offs_twd);
dsp_status dsp_dct3_preprocess_init_q15_cfY(dct3_q15_xy_t *inst, uint32_t length, uint32_t offs_twd);
void dsp_dct3_preprocess_q15_XtoY_cfX_MAC16x16(const dct3_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct3_preprocess_q15_YtoX_cfY_MAC16x16(const dct3_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct3_postprocess_q15_XtoY(uint32_t offs_input, uint32_t offs_output, uint32_t length);
void dsp_dct3_postprocess_q15_YtoX(uint32_t offs_input, uint32_t offs_output, uint32_t length);
// 
void dsp_dct4_preprocess_init_q15_cfX(dct4_q15_xy_t *inst, int32_t length, int32_t offs_precoeffs);
void dsp_dct4_preprocess_init_q15_cfY(dct4_q15_xy_t *inst, int32_t length, int32_t offs_precoeffs);
void dsp_dct4_preprocess_q15_XtoX_cfY_MAC16x16(const dct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct4_preprocess_q15_YtoY_cfX_MAC16x16(const dct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct4_postprocess_init_q15_cfX(dct4_q15_xy_t *inst, int32_t length, int32_t offs_postcoeffs);
void dsp_dct4_postprocess_init_q15_cfY(dct4_q15_xy_t *inst, int32_t length, int32_t offs_postcoeffs);
void dsp_dct4_postprocess_q15_XtoX_cfY_MAC16x16(const dct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_dct4_postprocess_q15_YtoY_cfX_MAC16x16(const dct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
// 
void dsp_biquad_cascade_df1_q15_X_cfYstY_MAC16x16(  const iir_biquad_q15_xy_t *inst,    uint32_t offs_inout,   uint32_t nSamples);
void dsp_biquad_cascade_df1_q15_Y_cfXstX_MAC16x16(  const iir_biquad_q15_xy_t *inst,    uint32_t offs_inout,   uint32_t nSamples);
dsp_status dsp_biquad_cascade_df1_init_q15_cfXstX(iir_biquad_q15_xy_t *inst, uint32_t numStages, uint32_t pshift,  uint32_t offs_states, uint32_t offs_coeffs);
dsp_status dsp_biquad_cascade_df1_init_q15_cfYstY(iir_biquad_q15_xy_t *inst, uint32_t numStages, uint32_t pshift,  uint32_t offs_states, uint32_t offs_coeffs);
// 
dsp_status dsp_iir_lattice_init_q15_cfXstX(iir_lattice_q15_xy_t *inst, uint16_t nStages, uint32_t  offs_Kcoeffs, uint32_t  offs_Vcoeffs, uint32_t  offs_states);
dsp_status dsp_iir_lattice_init_q15_cfYstY(iir_lattice_q15_xy_t *inst, uint16_t nStages, uint32_t  offs_Kcoeffs, uint32_t  offs_Vcoeffs, uint32_t  offs_states);
void dsp_iir_lattice_q15_X_cfYstY_MAC16x16(const iir_lattice_q15_xy_t *inst, uint32_t  offs_inout, uint32_t nSamples);
void dsp_iir_lattice_q15_Y_cfXstX_MAC16x16(const iir_lattice_q15_xy_t *inst, uint32_t  offs_inout, uint32_t nSamples);
// 
void dsp_fir_q15_XtoX_cfXstY_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_YtoX_cfXstY_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_XtoX_cfYstX_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_YtoX_cfYstX_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_XtoY_cfXstY_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_YtoY_cfXstY_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_XtoY_cfYstX_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_YtoY_cfYstX_MAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
dsp_status dsp_fir_init_q15_cfXstY_MAC16x16(fir_q15_xy_t *inst, uint16_t nTaps,  uint32_t offs_coeffs, uint32_t offs_states);
dsp_status dsp_fir_init_q15_cfYstX_MAC16x16(fir_q15_xy_t *inst, uint16_t nTaps,  uint32_t offs_coeffs, uint32_t offs_states);
// 
void dsp_fir_q15_complex_XtoX_cfXstY_DMAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_complex_YtoX_cfXstY_DMAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_complex_XtoX_cfYstX_DMAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_complex_YtoX_cfYstX_DMAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_complex_XtoY_cfXstY_DMAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_complex_YtoY_cfXstY_DMAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_complex_XtoY_cfYstX_DMAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_q15_complex_YtoY_cfYstX_DMAC16x16(fir_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
dsp_status dsp_fir_init_q15_complex_cfXstY_DMAC16x16(fir_q15_xy_t *inst, uint16_t nTaps,  uint32_t offs_coeffs, uint32_t offs_states);
dsp_status dsp_fir_init_q15_complex_cfYstX_DMAC16x16(fir_q15_xy_t *inst, uint16_t nTaps,  uint32_t offs_coeffs, uint32_t offs_states);
// 
void dsp_correlate_q15_XYtoX_MAC16x16(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
void dsp_correlate_q15_YXtoX_MAC16x16(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
void dsp_correlate_q15_XYtoY_MAC16x16(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
void dsp_correlate_q15_YXtoY_MAC16x16(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
// 
void dsp_conv_q15_XYtoX_MAC16x16(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
void dsp_conv_q15_YXtoX_MAC16x16(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
void dsp_conv_q15_XYtoY_MAC16x16(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
void dsp_conv_q15_YXtoY_MAC16x16(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst);
// 
dsp_status dsp_lms_init_q15_cfXstY_MAC16x16(lms_q15_xy_t *inst,  uint16_t nTaps,  q15_t mu, uint32_t postshift,  uint32_t  offs_coeffs,  uint32_t offs_states);
dsp_status dsp_lms_init_q15_cfYstX_MAC16x16(lms_q15_xy_t *inst,  uint16_t nTaps,  q15_t mu, uint32_t postshift,  uint32_t  offs_coeffs,  uint32_t offs_states);
void dsp_lms_q15_XY_cfYstX_MAC16x16(lms_q15_xy_t *inst,    uint32_t offs_inout,    uint32_t offs_referr,    uint32_t nSamples);
void dsp_lms_q15_YX_cfXstY_MAC16x16(lms_q15_xy_t *inst,    uint32_t offs_inout,    uint32_t offs_referr,    uint32_t nSamples);
// 
void dsp_fir_interpolate_q15_XtoX_cfXstY_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q15_YtoX_cfXstY_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q15_XtoX_cfYstX_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q15_YtoX_cfYstX_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q15_XtoY_cfXstY_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q15_YtoY_cfXstY_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q15_XtoY_cfYstX_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_interpolate_q15_YtoY_cfYstX_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
dsp_status dsp_fir_interpolate_init_q15_cfXstY_MAC16x16(fir_intdec_q15_xy_t *inst, uint16_t nTaps, uint8_t intFactor,  uint32_t offs_coeffs, uint32_t offs_states);
dsp_status dsp_fir_interpolate_init_q15_cfYstX_MAC16x16(fir_intdec_q15_xy_t *inst, uint16_t nTaps, uint8_t intFactor,  uint32_t offs_coeffs, uint32_t offs_states);
// 
void dsp_fir_decimate_q15_XtoX_cfXstY_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q15_YtoX_cfXstY_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q15_XtoX_cfYstX_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q15_YtoX_cfYstX_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q15_XtoY_cfXstY_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q15_YtoY_cfXstY_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q15_XtoY_cfYstX_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
void dsp_fir_decimate_q15_YtoY_cfYstX_MAC16x16(fir_intdec_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output, uint32_t nSamples);
dsp_status dsp_fir_decimate_init_q15_cfXstY_MAC16x16(fir_intdec_q15_xy_t *inst, uint16_t nTaps, uint8_t decFactor,  uint32_t offs_coeffs, uint32_t offs_states);
dsp_status dsp_fir_decimate_init_q15_cfYstX_MAC16x16(fir_intdec_q15_xy_t *inst, uint16_t nTaps, uint8_t decFactor,  uint32_t offs_coeffs, uint32_t offs_states);

//
dsp_status dsp_fct4_init_q15_cfXY(fct4_q15_xy_t *inst, int32_t length, int32_t xyptr_twf);
void dsp_fct4_q15_XtoX_scYcfXY_MAC32x16(fct4_q15_xy_t *inst, uint32_t offs_inout, uint32_t offs_scratch);
void dsp_fct4_q15_YtoY_scXcfXY_MAC32x16(fct4_q15_xy_t *inst, uint32_t offs_inout, uint32_t offs_scratch);
void dsp_fct4_q15_XtoY_cfXY_MAC32x16(fct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_fct4_q15_YtoX_cfXY_MAC32x16(fct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
//
void dsp_fct4_scaled_q15_XtoX_scYcfXY_MAC32x16(fct4_q15_xy_t *inst, uint32_t offs_inout, uint32_t offs_scratch);
void dsp_fct4_scaled_q15_YtoY_scXcfXY_MAC32x16(fct4_q15_xy_t *inst, uint32_t offs_inout, uint32_t offs_scratch);
void dsp_fct4_scaled_q15_XtoY_cfXY_MAC32x16(fct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);
void dsp_fct4_scaled_q15_YtoX_cfXY_MAC32x16(fct4_q15_xy_t *inst, uint32_t offs_input, uint32_t offs_output);

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif //ARC_DSPLIB_LL_API_H 
