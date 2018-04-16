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

#ifndef DSPLIB_PRIMITIVES_H
#define DSPLIB_PRIMITIVES_H

#include "dsplib_types.h"
#include "dsplib.h"

#if (!defined(_ARC) || defined(_ARCVER_ARC600) || defined(_ARCVER_ARCv2EM) || defined(_ARCVER_ARCv2HS)) && (!defined _MSC_VER)
 #include "platform_lib/arc.h"
#else
 #undef __Xea 
 #undef __Xmul32x16
 #undef __Xxmac_d16 
#endif

#include "dsplib_helper.h"

#ifndef _ARC
#define _Inline __inline
#endif

#ifndef SATURATE_MATH_OFF
#define SATURATE_MATH
#endif

#include "dsplib_primitives_general.h"

#if defined(SATURATE_MATH)
#include "dsplib_primitives_general_sat.h"
#endif

#if defined(__Xea)
#include "dsplib_primitives_Xea.h"
#endif

#if defined(__Xmul32x16)
#include "dsplib_primitives_Xmul32x16.h"
#endif

#if defined(__Xxmac_d16)
#include "dsplib_primitives_Xxmac_d16.h"
#endif

#if defined(__Xnorm)
#include "dsplib_primitives_Xnorm.h"
#endif

#if defined(__Xxy)
#include "dsplib_primitives_Xxy.h"
#endif

#if defined(_ARCVER_ARCv2HS)
#include "dsplib_primitives_archs.h"
#endif

// NOTE: fxapi rule should be at last position to override other rules
#if defined(__FXAPI__)
#include "dsplib_primitives_fxapi.h"
#endif

#if defined(DSPLIB_EXTENSION)
#include "dsplib_primitives_extension.h"
#endif

#endif
