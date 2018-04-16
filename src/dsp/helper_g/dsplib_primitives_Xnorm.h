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

#ifndef DSPLIB_PRIMITIVES_NORM_H
#define DSPLIB_PRIMITIVES_NORM_H

#undef dspfx_norm_q31
#undef dspfx_norm_q15


#if defined(_ARC) && !defined(_ARCVER_ARC600)
int32_t _normw   (int32_t); // NORM Normalize
#pragma intrinsic(_normw, name => "normw");
int32_t _norm   (int32_t); // NORM Normalize
#pragma intrinsic(_norm, name => "norm");
#endif

#define dspfx_norm_q31 _norm
#define dspfx_norm_q15 _normw

#endif
