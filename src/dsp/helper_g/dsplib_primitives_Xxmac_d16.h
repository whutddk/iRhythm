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

#ifndef DSPLIB_PRIMITIVES_XMAC_D16_H
#define DSPLIB_PRIMITIVES_XMAC_D16_H

#undef dspfx_mpy_q15
#undef dspfx_q31_mpy_q15
#undef dspfx_mpy_q7
#undef dspfx_mpy_rnd_q15
#undef dspfx_a32mul_q15
#undef dspfx_a32mac_q15
#undef dspfx_a32msu_q15
#undef dspfx_a16mac_q7
#undef dspfx_a32mul_nf_q15
#undef dspfx_a32mac_nf_q15
#undef dspfx_a16mac_nf_q7
#undef dspfx_q15_cast_a32
#undef dspfx_q15_cast_nf_a32
#undef dspfx_q7_cast_a16
#undef dspfx_q15_cast_asl_a32
#undef dspfx_accum32
#undef dspfx_accum16
#undef dspfx_reset_a32
#undef dspfx_reset_a16
#undef dspfx_get_a32
#undef INIT_MACMODE
#undef INIT_MACMODE_LSP
#undef INIT_MACMODE_NF


#undef DSP_MULTIPLIER_PRESENT
#define DSP_MULTIPLIER_PRESENT				// flag that indicates presence of any multiplier unit on platform

//#define dspfx_mpy_v2q15(a, b) (_muldw(a, b))
_Inline static q31_t _muldw_q31_q15(q15_t a, q15_t b) {
    _muldw(a, b);
    return (q31_t)_get_ACC2();
}
#define dspfx_mpy_q15(a, b) (q15_t)((q31_t)_muldw((a),(b)))
#define dspfx_mpy_q7(a, b) (q7_t)((q31_t)_muldw((a),(b))>>8)

#define dspfx_q31_mpy_q15(a, b) ((q31_t)_muldw_q31_q15((a),(b)))

#define dspfx_mpy_rnd_q15(a, b) (q15_t)((q31_t)_mulrdw((a),(b)))

#define dspfx_a32mul_q15(a, b, acc)  (q15_t)((q31_t)_muldw((a),(b))) 
#define dspfx_a32mul_nf_q15(a, b, acc)  (q15_t)((q31_t)_muldw((a),(b))) 

#define dspfx_a32mac_q15(a, b, acc) 	(q15_t)((q31_t)_macdw((a),(b)))
#define dspfx_a32msu_q15(a, b, acc) 	(q15_t)((q31_t)_msubdw((a),(b)))

#define dspfx_a32mac_nf_q15(a, b, acc) 	(q15_t)((q31_t)_macdw((a),(b)))
#define dspfx_a16mac_q7(a,b, acc)  	(q7_t)((q31_t)_macdw((a),(b)) >> 8)
#define dspfx_a16mac_nf_q7(a,b, acc)  	(q7_t)((q31_t)_macdw((a),(b)) >> 8)

#define dspfx_q15_cast_a32(acc) (q15_t)(_get_ACC2() >> 16)  
#define dspfx_q15_cast_nf_a32(acc) (q15_t)(_get_ACC2() >> 16)  
#define dspfx_q7_cast_a16(acc)  (q7_t) (SATURATE_15(_get_ACC2()) >> 8)  

#define dspfx_q15_cast_asl_a32(acc, shift) (q15_t)SATURATE_15((int32_t)_get_ACC2() >> (16-(shift)))  

#define dspfx_accum16()       
#define dspfx_accum32()        
#define dspfx_reset_a32()  _muldw(0, 0)      
#define dspfx_reset_a16()  _muldw(0, 0)

#define dspfx_v2accum32()
#define dspfx_v2a32mul_v2q15(a, b) _mulrdw((a),(b))
#define dspfx_v2a32mac_v2q15(a, b) _macrdw((a),(b))

#define dspfx_q63_get_cast_a32(acc) (int64_t)(((int64_t)((int16_t)((uint32_t)_lr(AUX_XMAC0))) << 32) | (uint64_t)_lr(AUX_XMAC2))>>1

#define dspfx_v2accum40()
#define dspfx_v2a40mul_v2q15(a, b) _mulrdw((a),(b))
#define dspfx_v2a40mac_v2q15(a, b) _macrdw((a),(b))
#define dspfx_q63_get_cast_v2a40(n) ((q63_t)(n == 0 ? \
    (int64_t)(((int64_t)((int32_t)_lr(AUX_XMAC0) >> 16) << 32) | (uint64_t)_lr(AUX_XMAC1)) : \
    (int64_t)(((int64_t)((int16_t)((uint32_t)_lr(AUX_XMAC0))) << 32) | (uint64_t)_lr(AUX_XMAC2))) >> 1)

#define dspfx_create0_v2q15(a) (uint32_t)((uint16_t)(a))
#define dspfx_v2q15_cast_q31(a) (a)

#define dspfx_get_a32(acc)     ((q31_t)_get_ACC2() >> 1)


#define INIT_MACMODE()    _set_MACMODE(0x800c)
#define INIT_MACMODE_NF() _set_MACMODE(0x8008)
#define INIT_MACMODE_LSP() _set_MACMODE(0x8004)

#endif
