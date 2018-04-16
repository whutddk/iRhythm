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

// Produce instance with: MAC32x32; pIn0 - X; pOut - X; 

#pragma Code("dsplib_dsp_correlate_q31_XYtoX_MAC32x32")

/**
 *  Q31 real cross-correlation
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nLenA
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nLenA + nLenB - 1
 */

void dsp_correlate_q31_XYtoX_MAC32x32(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst)  
{
    int32_t i, j; // loop counters
    int32_t pA, pB;
    int32_t Na1, Nb1;
    
    
    _set_mx00(AM_OFFS(1));
    _set_my00(AM_OFFS(1));

    if ( (int32_t)nLenB > (int32_t)nLenA)
    {
        pA = offs_SrcB;
        Na1 = (int32_t)nLenB;
        pB = offs_SrcA;
        Nb1 = (int32_t)nLenA;        
        _set_ax1(offs_Dst+Na1+Nb1-2);
        _set_mx10(AM_OFFS(-1));
    }
    else
    {
        pA = offs_SrcA;
        Na1 = (int32_t)nLenA;
        pB = offs_SrcB;
        Nb1 = (int32_t)nLenB;
        _set_ax1(offs_Dst);
        _set_mx10(AM_OFFS(1));
    }

    for ( i = 0; i < Nb1-1; i++)
    {
        int k = i;
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ay0(pA);
            _set_ax0(pB + Nb1 -1 -i);
        } else {
            _set_ax0(pA);
            _set_ay0(pB + Nb1 -1 -i);
        }

        _mulhflw(0, 0);

        for ( j = 0; j <= k; j++)
        {
             _macflw(_x0_nu(), _y0_nu());
            _machflw(_x0_u0(), _y0_u0());
        }
        _set_x1_u0( _get_ACC1());
    }

    for ( i = 0; i < Na1 - Nb1; i++)
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ay0(pA++);
            _set_ax0(pB);
        } else {
            _set_ax0(pA++);
            _set_ay0(pB);
        }

        _mulhflw(0, 0);

        for ( j = 0; j < Nb1; j++ )
        {
             _macflw(_x0_nu(), _y0_nu());
            _machflw(_x0_u0(), _y0_u0());
        }
        _set_x1_u0( _get_ACC1());
    }

    for ( i = Na1-Nb1; i < Na1; i++ )
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ay0(pA++);
            _set_ax0(pB);
        } else {
            _set_ax0(pA++);
            _set_ay0(pB);
        }

        _mulhflw(0, 0);

        for ( j = i; j < Na1; j++ )
        {
             _macflw(_x0_nu(), _y0_nu());
            _machflw(_x0_u0(), _y0_u0());
        }
        _set_x1_u0( _get_ACC1());
    }
}
// Produce instance with: MAC32x32; pOut - X; pIn0 - Y; 

#pragma Code("dsplib_dsp_correlate_q31_YXtoX_MAC32x32")

/**
 *  Q31 real cross-correlation
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nLenA
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nLenA + nLenB - 1
 */

void dsp_correlate_q31_YXtoX_MAC32x32(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst)  
{
    int32_t i, j; // loop counters
    int32_t pA, pB;
    int32_t Na1, Nb1;
    
    
    _set_my00(AM_OFFS(1));
    _set_mx10(AM_OFFS(1));

    if ( (int32_t)nLenB > (int32_t)nLenA)
    {
        pA = offs_SrcB;
        Na1 = (int32_t)nLenB;
        pB = offs_SrcA;
        Nb1 = (int32_t)nLenA;        
        _set_ax0(offs_Dst+Na1+Nb1-2);
        _set_mx00(AM_OFFS(-1));
    }
    else
    {
        pA = offs_SrcA;
        Na1 = (int32_t)nLenA;
        pB = offs_SrcB;
        Nb1 = (int32_t)nLenB;
        _set_ax0(offs_Dst);
        _set_mx00(AM_OFFS(1));
    }

    for ( i = 0; i < Nb1-1; i++)
    {
        int k = i;
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ax1(pA);
            _set_ay0(pB + Nb1 -1 -i);
        } else {
            _set_ay0(pA);
            _set_ax1(pB + Nb1 -1 -i);
        }

        _mulhflw(0, 0);

        for ( j = 0; j <= k; j++)
        {
             _macflw(_y0_nu(), _x1_nu());
            _machflw(_y0_u0(), _x1_u0());
        }
        _set_x0_u0( _get_ACC1());
    }

    for ( i = 0; i < Na1 - Nb1; i++)
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ax1(pA++);
            _set_ay0(pB);
        } else {
            _set_ay0(pA++);
            _set_ax1(pB);
        }

        _mulhflw(0, 0);

        for ( j = 0; j < Nb1; j++ )
        {
             _macflw(_y0_nu(), _x1_nu());
            _machflw(_y0_u0(), _x1_u0());
        }
        _set_x0_u0( _get_ACC1());
    }

    for ( i = Na1-Nb1; i < Na1; i++ )
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ax1(pA++);
            _set_ay0(pB);
        } else {
            _set_ay0(pA++);
            _set_ax1(pB);
        }

        _mulhflw(0, 0);

        for ( j = i; j < Na1; j++ )
        {
             _macflw(_y0_nu(), _x1_nu());
            _machflw(_y0_u0(), _x1_u0());
        }
        _set_x0_u0( _get_ACC1());
    }
}
// Produce instance with: MAC32x32; pOut - Y; pIn0 - X; 

#pragma Code("dsplib_dsp_correlate_q31_XYtoY_MAC32x32")

/**
 *  Q31 real cross-correlation
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nLenA
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nLenA + nLenB - 1
 */

void dsp_correlate_q31_XYtoY_MAC32x32(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst)  
{
    int32_t i, j; // loop counters
    int32_t pA, pB;
    int32_t Na1, Nb1;
    
    
    _set_mx00(AM_OFFS(1));
    _set_my10(AM_OFFS(1));

    if ( (int32_t)nLenB > (int32_t)nLenA)
    {
        pA = offs_SrcB;
        Na1 = (int32_t)nLenB;
        pB = offs_SrcA;
        Nb1 = (int32_t)nLenA;        
        _set_ay0(offs_Dst+Na1+Nb1-2);
        _set_my00(AM_OFFS(-1));
    }
    else
    {
        pA = offs_SrcA;
        Na1 = (int32_t)nLenA;
        pB = offs_SrcB;
        Nb1 = (int32_t)nLenB;
        _set_ay0(offs_Dst);
        _set_my00(AM_OFFS(1));
    }

    for ( i = 0; i < Nb1-1; i++)
    {
        int k = i;
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ay1(pA);
            _set_ax0(pB + Nb1 -1 -i);
        } else {
            _set_ax0(pA);
            _set_ay1(pB + Nb1 -1 -i);
        }

        _mulhflw(0, 0);

        for ( j = 0; j <= k; j++)
        {
             _macflw(_x0_nu(), _y1_nu());
            _machflw(_x0_u0(), _y1_u0());
        }
        _set_y0_u0( _get_ACC1());
    }

    for ( i = 0; i < Na1 - Nb1; i++)
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ay1(pA++);
            _set_ax0(pB);
        } else {
            _set_ax0(pA++);
            _set_ay1(pB);
        }

        _mulhflw(0, 0);

        for ( j = 0; j < Nb1; j++ )
        {
             _macflw(_x0_nu(), _y1_nu());
            _machflw(_x0_u0(), _y1_u0());
        }
        _set_y0_u0( _get_ACC1());
    }

    for ( i = Na1-Nb1; i < Na1; i++ )
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ay1(pA++);
            _set_ax0(pB);
        } else {
            _set_ax0(pA++);
            _set_ay1(pB);
        }

        _mulhflw(0, 0);

        for ( j = i; j < Na1; j++ )
        {
             _macflw(_x0_nu(), _y1_nu());
            _machflw(_x0_u0(), _y1_u0());
        }
        _set_y0_u0( _get_ACC1());
    }
}
// Produce instance with: MAC32x32; pOut - Y; pIn0 - Y; 

#pragma Code("dsplib_dsp_correlate_q31_YXtoY_MAC32x32")

/**
 *  Q31 real cross-correlation
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nLenA
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nLenA + nLenB - 1
 */

void dsp_correlate_q31_YXtoY_MAC32x32(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst)  
{
    int32_t i, j; // loop counters
    int32_t pA, pB;
    int32_t Na1, Nb1;
    
    
    _set_my00(AM_OFFS(1));
    _set_mx00(AM_OFFS(1));

    if ( (int32_t)nLenB > (int32_t)nLenA)
    {
        pA = offs_SrcB;
        Na1 = (int32_t)nLenB;
        pB = offs_SrcA;
        Nb1 = (int32_t)nLenA;        
        _set_ay1(offs_Dst+Na1+Nb1-2);
        _set_my10(AM_OFFS(-1));
    }
    else
    {
        pA = offs_SrcA;
        Na1 = (int32_t)nLenA;
        pB = offs_SrcB;
        Nb1 = (int32_t)nLenB;
        _set_ay1(offs_Dst);
        _set_my10(AM_OFFS(1));
    }

    for ( i = 0; i < Nb1-1; i++)
    {
        int k = i;
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ax0(pA);
            _set_ay0(pB + Nb1 -1 -i);
        } else {
            _set_ay0(pA);
            _set_ax0(pB + Nb1 -1 -i);
        }

        _mulhflw(0, 0);

        for ( j = 0; j <= k; j++)
        {
             _macflw(_y0_nu(), _x0_nu());
            _machflw(_y0_u0(), _x0_u0());
        }
        _set_y1_u0( _get_ACC1());
    }

    for ( i = 0; i < Na1 - Nb1; i++)
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ax0(pA++);
            _set_ay0(pB);
        } else {
            _set_ay0(pA++);
            _set_ax0(pB);
        }

        _mulhflw(0, 0);

        for ( j = 0; j < Nb1; j++ )
        {
             _macflw(_y0_nu(), _x0_nu());
            _machflw(_y0_u0(), _x0_u0());
        }
        _set_y1_u0( _get_ACC1());
    }

    for ( i = Na1-Nb1; i < Na1; i++ )
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ax0(pA++);
            _set_ay0(pB);
        } else {
            _set_ay0(pA++);
            _set_ax0(pB);
        }

        _mulhflw(0, 0);

        for ( j = i; j < Na1; j++ )
        {
             _macflw(_y0_nu(), _x0_nu());
            _machflw(_y0_u0(), _x0_u0());
        }
        _set_y1_u0( _get_ACC1());
    }
}

#endif
