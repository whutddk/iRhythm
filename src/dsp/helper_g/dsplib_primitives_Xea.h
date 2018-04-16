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

#ifndef DSPLIB_PRIMITIVES_EA_H
#define DSPLIB_PRIMITIVES_EA_H

#undef dspfx_abs_q31
#undef dspfx_abs_q15
#undef dspfx_abs_q7
#undef dspfx_neg_q31
#undef dspfx_neg_q15
#undef dspfx_neg_q7
#undef dspfx_neg_v2q15
#undef dspfx_add_q31
#undef dspfx_add_q15
#undef dspfx_add_q7
#undef dspfx_add_v2q15
#undef dspfx_sub_q31
#undef dspfx_sub_q15
#undef dspfx_sub_q7
#undef dspfx_sub_v2q15
#undef dspfx_asl_q31
#undef dspfx_asl_q15
#undef dspfx_asl_q7
#undef SATURATE_15

#define SATURATE_15(a) (_sat16(a))


#define dspfx_abs_q31(a) ((q31_t)(_abss(a)))
#define dspfx_abs_q15(a) ((q15_t)(_abss(_asls(a, 16)) >> 16))
#define dspfx_abs_q7(a)  ((q7_t) (_abss(_asls(a, 24)) >> 24))

#define dspfx_neg_q31(a) ((q31_t)(_negs(a)))
#define dspfx_neg_q15(a) ((q15_t)(_negs(_asls(a,16)) >> 16))
#define dspfx_neg_q7(a)  ((q7_t) (_negs(_asls(a,24)) >> 24))
// vector primitives
#define dspfx_neg_v2q15(a) (_subsdw(0, a))

#define dspfx_add_q31(a, b) ((q31_t)(_adds(a, b)))
#define dspfx_add_q15(a, b) ((q15_t)(_adds(_asls(a,16), _asls(b,16)) >> 16))
#define dspfx_add_q7(a, b)  ((q7_t) (_adds(_asls(a,24), _asls(b,24)) >> 24))
// vector primitives
#define dspfx_add_v2q15(a, b) ((_addsdw((a), (b))))

#define dspfx_sub_q31(a, b) ((q31_t)(_subs(a, b)))
#define dspfx_sub_q15(a, b) ((q15_t)(_subs(_asls(a,16), _asls(b,16))>>16))
#define dspfx_sub_q7(a, b)  ((q7_t) (_subs(_asls(a,24), _asls(b,24))>>24))
// vector primitives
#define dspfx_sub_v2q15(a, b) ((_subsdw(a, b)))

#define dspfx_asl_q31(a, b) ((q31_t)(_asls(a, b)))
#define dspfx_asl_q15(a, b) ((q15_t)((_asls(a, (b)+16)>>16)))
#define dspfx_asl_q7(a, b)  ((q7_t) ((_asls(a, (b)+24)>>24)))

#endif
