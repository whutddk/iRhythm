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

#if !defined(EXCLUDE_DSP_ACOS_Q15)

#pragma Code("dsplib_dsp_acos_q15")

/**
 * Q31 arccosine function
 *
 * \param[input] - input [-1.0, 1.0)
 *
 * \result result of arccosine function [0.0, 1.0) mapped to [0.0, pi)
 */

q15_t _DSPLIB_NOXYCC dsp_acos_q15(q15_t input)
{
    return dspfx_sub_q15(dsp_q15_cast_rnd_f32(0.5), dsp_asin_q15(input));
}

#endif //!defined(EXCLUDE_DSP_ACOS_Q15)
