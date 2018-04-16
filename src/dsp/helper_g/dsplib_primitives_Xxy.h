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

#ifndef DSPLIB_PRIMITIVES_XY_H
#define DSPLIB_PRIMITIVES_XY_H


#if defined _ARCVER_ARCv2EM && defined __Xagu
#include <arc/xy.h>
#endif

#if defined _ARCVER_ARC600 

_Inline static void _MEM_FILL_Y_Q15(uint32_t XY_OFFSET, q15_t val, uint32_t size) {
    if (size)
    {
        uint32_t i;
        _set_ay0(XY_OFFSET);
        _set_my00(1|(1<<29));
        _nop();
        for (i=0;i<size;i++)
            _set_y0_u0(val<<16);
    }
}

_Inline static void _MEM_FILL_X_Q15(uint32_t XY_OFFSET, q15_t val, uint32_t size) {
    if (size)
    {
        uint32_t i;
        _set_ax0(XY_OFFSET);
        _set_mx00(1|(1<<29));
        _nop();
        for (i=0;i<size;i++)
            _set_x0_u0(val);
    }
}

_Inline static void _MEM_FILL_Y_Q31(uint32_t XY_OFFSET, q31_t val, uint32_t size) {
    if (size)
    {
        uint32_t i;
        _set_ay0(XY_OFFSET);
        _set_my00(1);
        _nop();
        for (i=0;i<size;i++)
            _set_y0_u0(val);
    }
}

_Inline static void _MEM_FILL_X_Q31(uint32_t XY_OFFSET, q31_t val, uint32_t size) {
    if (size)
    {
        uint32_t i;
        _set_ax0(XY_OFFSET);
        _set_mx00(1);
        _nop();
        for (i=0;i<size;i++)
            _set_x0_u0(val);
    }
}

#endif

#endif
