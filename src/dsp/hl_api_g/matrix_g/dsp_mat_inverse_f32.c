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

#if !defined(EXCLUDE_DSP_MAT_INVERSE_F32)

#include "dsplib_primitives.h" 

#pragma Code("dsplib_dsp_mat_inverse_f32")

#if defined _ARCv2EM_XY_AGU_MEDIUM && defined __FXAPI__ && defined __Xfpu_mac && defined _XY_HAND_OPT

dsp_status _DSPLIB_XYCC dsp_mat_inverse_f32(const matrix_f32_t * pSrc, matrix_f32_t * pDst)
{
	// Input and output pointers
	float *pInput = pSrc->pData;
	float *pOutput = pDst->pData;
	// Rows and Columns numbers
	uint32_t Rnum = pSrc->rowCnt;
	uint32_t Cnum = pSrc->colCnt;
	// Column max value
	float maxColumn;
	// Loop counters
	uint32_t i,  j, p, q;
	// Total number of the elements in the matrix
	uint32_t NN;
	// Return status
	dsp_status status = DSP_ERR_OK;

#ifdef DSP_CHECK_MAT_DIMENSIONS
	// Check matrix dimensions
	if((pSrc->rowCnt != pSrc->colCnt) || (pDst->rowCnt != pDst->colCnt)
		|| (pSrc->rowCnt != pDst->rowCnt))
	{
		status = DSP_ERR_MAT_DIMENSIONS;
	}
	else
#endif //#ifdef DSP_CHECK_MAT_DIMENSIONS
	{
		_DSPLIB_store_regs(4, 11, 3);
		// Setup pointer for the output matrix
		NN = dspfx_mpy_i32(Rnum , Rnum);
		// Null output matrix
		_set_agu_ap0(pOutput); // Set AP registers for SrcA
		_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(1));

		for (i = 0; i < NN; i++)
		{
			_st_agu_u0(_as_uint32(0.0));
		}

		_set_agu_ap0(pOutput); // Set AP registers for SrcA
		_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31_REG(0));
		_set_agu_os0(Rnum + 1);

		// Make identity output matrix
		for (i = 0; i < Rnum; i++)
		{
			_st_agu_u0(_as_uint32(1.0));
		}

		_set_agu_mod0(0, AGU_DT_q31, AGU_AD_INC_q31(0));
		_set_agu_mod1(0, AGU_DT_q31, AGU_AD_INC_q31(1));

		_set_agu_mod2(1, AGU_DT_q31, AGU_AD_INC_q31(0));
		_set_agu_mod3(1, AGU_DT_q31, AGU_AD_INC_q31(1));

		_set_agu_mod4(2, AGU_DT_q31, AGU_AD_INC_q31(0));
		_set_agu_mod5(2, AGU_DT_q31, AGU_AD_INC_q31(1));

		_set_agu_mod6(3, AGU_DT_q31, AGU_AD_INC_q31(0));
		_set_agu_mod7(3, AGU_DT_q31, AGU_AD_INC_q31(1));

		_set_agu_mod8(1, AGU_DT_q31, AGU_AD_INC_q31_REG(1));
		_set_agu_mod9(3, AGU_DT_q31, AGU_AD_INC_q31_REG(1));
		_set_agu_os1(Cnum);

		float *pInput1 = pInput;
		float *pOutput1 = pOutput;

#pragma clang loop unroll(disable)
		for (q = 0; q < Cnum; q++)
		{
			uint32_t Cnum_q = Cnum-q;
			_set_agu_ap0(pInput1);
			_set_agu_ap2(pOutput1);

			// Check if pivot element is zero. If so, we need to replace rows
			if (_as_float(_agu_u0()) == 0.0f)
			{
				// Row to exchange
				int exchangeRow = q;
				int row = q;
				// Find the maximum element of the column
				maxColumn = 0.0f;

				_set_agu_ap1(pInput1);

#pragma clang loop unroll(disable)
				for (i = 0; i < Cnum_q; i++)
				{
					float cell_val = dspfx_abs_f32(_as_float(_agu_u8()));
					if (maxColumn < cell_val)
					{
						maxColumn = cell_val;
						exchangeRow = row;
					}
					row++;
				}
				// Check if matrix is singular
				if(maxColumn == 0.0)
				{
					status = DSP_ERR_SINGULAR;
					break;
				}
				// Perform row replace
				_set_agu_ap1((pInput + (exchangeRow * Cnum))); // Set AP registers for SrcA
				_set_agu_ap3((pOutput + (exchangeRow * Cnum))); // Set AP registers for SrcA
				
#pragma clang loop unroll(disable)
				for (j = 0; j < Cnum; j++)
				{
					uint32_t ui32Tmp;
					// Exchange in input
					ui32Tmp = _agu_u0();
					_st_agu_u1(_agu_u2());
					_st_agu_u3(ui32Tmp);
					// Exchange in output
					ui32Tmp = _agu_u4();
					_st_agu_u5(_agu_u6());
					_st_agu_u7(ui32Tmp);
				}
				// restore pivot pointers
				_set_agu_ap0(pInput1);
				_set_agu_ap2(pOutput1);
			}

			// Inverse pivot element of the row
			float inv_input = 1.0 / _as_float(_agu_u0());

			// Multiply each element of the row of the input matrix by the inverse pivot element
#pragma clang loop unroll(disable)
			for (j = 0; j < Cnum_q; j++)
			{
				_st_agu_u1(_as_uint32(MPY_F32(_as_float(_agu_u0()), inv_input)));
			}

			// Multiply each element of the row of the output matrix by the inverse pivot element
#pragma clang loop unroll(disable)
			for (j = 0; j < (Cnum); j++)
			{
				_st_agu_u5(_as_uint32(MPY_F32(_as_float(_agu_u4()), inv_input)));
			}

			_set_agu_ap1((pInput + q)); // Set AP registers
			_set_agu_ap3((pOutput)); // Set AP registers

			_set_agu_mod10(1, AGU_DT_q31, AGU_AD_INC_q31_REG(2));
			_set_agu_os2(q);

			// Replace rows
#pragma clang loop unroll(disable)
			for (p = 0; p < Cnum; p++)
			{
				if(p != q)
				{
					float input = _as_float(_agu_u2());

					// restore pivot pointers
					_set_agu_ap0(pInput1);
					_set_agu_ap2(pOutput1);
#pragma clang loop unroll(disable)
					for (j = 0; j < Cnum_q; j++)
					{
						_st_agu_u3(_as_uint32(_as_float(_agu_u2()) - MPY_F32(input, _as_float(_agu_u1()))));
					}

#pragma clang loop unroll(disable)
					for (j = 0; j < (Cnum); j++)
					{
						_st_agu_u7(_as_uint32(_as_float(_agu_u6()) - MPY_F32(input, _as_float(_agu_u5()))));
					}
					_add(_agu_u10(), 0);
				}
				else
				{
					_add(_agu_u8(), _agu_u9());
				}
			}
			pInput1 += Cnum + 1;
			pOutput1 += Cnum;
		}
		_DSPLIB_restore_regs();
	}

	return (status);
}

#else

/**
 * Floating-point matrix inverse.
 *
 * \param[in] pSrc - pointer to input matrix structure
 * \param[out] pDst - pointer to output matrix structure
 * \result dsp_status
 */

dsp_status dsp_mat_inverse_f32(const matrix_f32_t * pSrc, matrix_f32_t * pDst)
{
    // Input and output pointers
  PTR(float) pInput = (PTR(float)) pSrc->pData;
  PTR(float) pOutput = (PTR(float)) pDst->pData;
  // Rows and Columns numbers
  uint32_t Rnum = pSrc->rowCnt;
  uint32_t Cnum = pSrc->colCnt;
  // Temp working pointers
  PTR(float) pTempIn1, *pTempIn2, *pTempOut1, *pTempOut2;
  PTR(float) pPivot_input, *pRef_input, *pPivot_output, *pRef_output;
  // Column max value
  float maxColumn;
  float input = 0.0f, in_tmp, inv_input;
  // Loop counters
  uint32_t i,  j, p, q;
  // Total number of the elements in the matrix
  uint32_t NN;
  // Return status
  dsp_status status = DSP_ERR_OK;

#ifdef DSP_CHECK_MAT_DIMENSIONS
  // Check matrix dimensions
  if((pSrc->rowCnt != pSrc->colCnt) || (pDst->rowCnt != pDst->colCnt)
     || (pSrc->rowCnt != pDst->rowCnt))
  {
    status = DSP_ERR_MAT_DIMENSIONS;
  }
  else
#endif //#ifdef DSP_CHECK_MAT_DIMENSIONS
  {
    // Setup pointer for the output matrix
    pTempIn2 = pOutput;
    NN = dspfx_mpy_i32(Rnum , Rnum);
    // Null output matrix
    for (i = 0; i < NN; i+= 1)
    {
        pOutput[i] = 0.0;
    }
    // Make identity output matrix
    for (i = 0; i < NN; i+= (Rnum + 1))
    {
        pOutput[i] = 1.0;
    }

    for (q = 0; q < Cnum; q++)
    {
      // Setup pointers to the pivot element
      pTempIn1 = pInput + (q * Cnum);

      // Pivot value
      input = *pTempIn1;

      // Output matrix index
      p = 1;
      // Check if pivot element is zero. If so, we need to replace rows
      if (input == 0.0f){
          // Row to exchange
          int exchangeRow = q;
          int row = q;
          // Find the maximum element of the column
          maxColumn = 0.0f;
          for (i = 0; i < Rnum - q; i++)
          {
              //maxColumn = MAX(maxColumn, ABS_F32(*pTempIn1));
              if (maxColumn < dspfx_abs_f32(*pTempIn1))
              {
                  maxColumn = dspfx_abs_f32(*pTempIn1);
                  exchangeRow = row;
              }
              row++;
              pTempIn1 += Cnum;
          }
          // Check if matrix is singular
          if(maxColumn == 0.0)
          {
              status = DSP_ERR_SINGULAR;
              break;
          }
          // Perform row replace
          // Setup pointers for the input matrix
          pTempIn1 = pInput + (q * Cnum);
          pTempIn2 = pInput + (exchangeRow * Cnum);
          // Setup pointers for the output matrix
          pTempOut1 = pOutput + (q * Cnum);
          pTempOut2 = pOutput + (exchangeRow * Cnum);
          for (j = 0; j < Cnum; j++){
              float fTmp;
              // Exchange in input
              fTmp = *pTempIn1;
              *pTempIn1++ = *pTempIn2;
              *pTempIn2++ = fTmp;
              // Exchange in output
              fTmp = *pTempOut1;
              *pTempOut1++ = *pTempOut2;
              *pTempOut2++ = fTmp;
          }

      }

      // Setup pointers to the pivot element of input and output matrices
      pPivot_input = pInput + (q * Cnum);
      pPivot_output = pOutput + (q * Cnum);

      pTempIn1 = pPivot_input;
      pTempIn2 = pPivot_output;

      // Inverse pivot element of the row
      inv_input = 1 / *pPivot_input;

      // Multiply each element of the row of the input matrix by the inverse pivot element
      for (j = 0; j < (Cnum - q); j++)
      {        
        in_tmp = *pTempIn1;
        *pTempIn1++ = MPY_F32(in_tmp, inv_input);
      }

      // Multiply each element of the row of the output matrix by the inverse pivot element
      for (j = 0; j < (Cnum); j++)
      {
        in_tmp = *pTempIn2;
        *pTempIn2++ = MPY_F32(in_tmp, inv_input);
      }
    
      pTempIn1 = pInput;
      pTempIn2 = pOutput;
      i = 0;
      // Replace rows
      for (p = 0; p < Rnum; p++)
      {
        if(i != q)
        {
          input = *pTempIn1;

          pRef_input = pPivot_input;
          pRef_output = pPivot_output;

          for (j = 0; j < (Cnum - q); j++)
          {
            in_tmp = *pTempIn1;
            *pTempIn1++ = in_tmp - MPY_F32(input, *pRef_input++);
          }

          for (j = 0; j < (Cnum); j++)
          {
            in_tmp = *pTempIn2;
            *pTempIn2++ = in_tmp - MPY_F32(input, *pRef_output++);
          }
        }
        else
        {
           pTempIn1 += Cnum - q;
           pTempIn2 += Cnum;
        }

        pTempIn1 = pTempIn1 + q;
        i++;
      }
      pInput++;
    }
  }

  return (status);
}
#endif

#endif //!defined(EXCLUDE_DSP_MAT_INVERSE_F32)
