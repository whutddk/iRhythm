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
#if defined(_ARC_AS210_mac16)

// Produce instance with: MAC16x16; pIn0 - X; pOut - X; 

#pragma Code("dsplib_dsp_conv_q15_XYtoX_MAC16x16")

/**
 *  Q15 real linear convolution
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nLenA
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nLenA + nLenB - 1
 */

void dsp_conv_q15_XYtoX_MAC16x16(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst)  
{
    int32_t i, j; // loop counters
    int32_t pA, pB;
    int32_t Na1, Nb1;
    
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    if ( (int32_t)nLenB > (int32_t)nLenA)
    {
        pA = offs_SrcB;
        Na1 = (int32_t)nLenB;
        pB = offs_SrcA;
        Nb1 = (int32_t)nLenA;  
        _set_mx00(AM_16BIT|AM_OFFS(-1));
        _set_my00(AM_16BIT|AM_OFFS(1));
    }
    else
    {
        pA = offs_SrcA;
        Na1 = (int32_t)nLenA;
        pB = offs_SrcB;
        Nb1 = (int32_t)nLenB;
        _set_mx00(AM_16BIT|AM_OFFS(1));
        _set_my00(AM_16BIT|AM_OFFS(-1));
    }
    _set_ax1(offs_Dst);
    _set_mx10(AM_16BIT|AM_OFFS(1));

    for ( i = 0; i < Nb1; i++)
    {
        int k = i;
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ay0(pA);
            _set_ax0(pB++);
        } else {
            _set_ax0(pA);
            _set_ay0(pB++);
        }

        _mulrdw(0, 0);

        for ( j = 0; j <= k; j++)
        {
            _macrdw(_x0_u0(), _y0_u0());
        }
        _set_x1_u0( _get_ACC1());
    }
    pA++;

    for ( i = 0; i < Na1 - Nb1; i++)
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ay0(pA++);
            _set_ax0(pB-1);
        } else {
            _set_ax0(pA++);
            _set_ay0(pB-1);
        }

        _mulrdw(0, 0);

        for ( j = 0; j < Nb1; j++ )
        {
            _macrdw(_x0_u0(), _y0_u0());
        }
        _set_x1_u0( _get_ACC1());
    }
    if (Nb1==1)
        return;
    for ( i = 0; i < Nb1-1; i++ )
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ay0(pA++);
            _set_ax0(pB-1);
        } else {
            _set_ax0(pA++);
            _set_ay0(pB-1);
        }

        _mulrdw(0, 0);

        for ( j = 0; j < Nb1-i-1; j++ )
        {
            _macrdw(_x0_u0(), _y0_u0());
        }
        _set_x1_u0( _get_ACC1());
    }
}
// Produce instance with: MAC16x16; pOut - X; pIn0 - Y; 

#pragma Code("dsplib_dsp_conv_q15_YXtoX_MAC16x16")

/**
 *  Q15 real linear convolution
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nLenA
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nLenA + nLenB - 1
 */

void dsp_conv_q15_YXtoX_MAC16x16(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst)  
{
    int32_t i, j; // loop counters
    int32_t pA, pB;
    int32_t Na1, Nb1;
    
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    if ( (int32_t)nLenB > (int32_t)nLenA)
    {
        pA = offs_SrcB;
        Na1 = (int32_t)nLenB;
        pB = offs_SrcA;
        Nb1 = (int32_t)nLenA;  
        _set_my00(AM_16BIT|AM_OFFS(-1));
        _set_mx10(AM_16BIT|AM_OFFS(1));
    }
    else
    {
        pA = offs_SrcA;
        Na1 = (int32_t)nLenA;
        pB = offs_SrcB;
        Nb1 = (int32_t)nLenB;
        _set_my00(AM_16BIT|AM_OFFS(1));
        _set_mx10(AM_16BIT|AM_OFFS(-1));
    }
    _set_ax0(offs_Dst);
    _set_mx00(AM_16BIT|AM_OFFS(1));

    for ( i = 0; i < Nb1; i++)
    {
        int k = i;
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ax1(pA);
            _set_ay0(pB++);
        } else {
            _set_ay0(pA);
            _set_ax1(pB++);
        }

        _mulrdw(0, 0);

        for ( j = 0; j <= k; j++)
        {
            _macrdw(_y0_u0(), _x1_u0());
        }
        _set_x0_u0( _get_ACC1());
    }
    pA++;

    for ( i = 0; i < Na1 - Nb1; i++)
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ax1(pA++);
            _set_ay0(pB-1);
        } else {
            _set_ay0(pA++);
            _set_ax1(pB-1);
        }

        _mulrdw(0, 0);

        for ( j = 0; j < Nb1; j++ )
        {
            _macrdw(_y0_u0(), _x1_u0());
        }
        _set_x0_u0( _get_ACC1());
    }
    if (Nb1==1)
        return;
    for ( i = 0; i < Nb1-1; i++ )
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ax1(pA++);
            _set_ay0(pB-1);
        } else {
            _set_ay0(pA++);
            _set_ax1(pB-1);
        }

        _mulrdw(0, 0);

        for ( j = 0; j < Nb1-i-1; j++ )
        {
            _macrdw(_y0_u0(), _x1_u0());
        }
        _set_x0_u0( _get_ACC1());
    }
}
// Produce instance with: MAC16x16; pOut - Y; pIn0 - X; 

#pragma Code("dsplib_dsp_conv_q15_XYtoY_MAC16x16")

/**
 *  Q15 real linear convolution
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nLenA
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nLenA + nLenB - 1
 */

void dsp_conv_q15_XYtoY_MAC16x16(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst)  
{
    int32_t i, j; // loop counters
    int32_t pA, pB;
    int32_t Na1, Nb1;
    
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    if ( (int32_t)nLenB > (int32_t)nLenA)
    {
        pA = offs_SrcB;
        Na1 = (int32_t)nLenB;
        pB = offs_SrcA;
        Nb1 = (int32_t)nLenA;  
        _set_mx00(AM_16BIT|AM_OFFS(-1));
        _set_my10(AM_16BIT|AM_OFFS(1));
    }
    else
    {
        pA = offs_SrcA;
        Na1 = (int32_t)nLenA;
        pB = offs_SrcB;
        Nb1 = (int32_t)nLenB;
        _set_mx00(AM_16BIT|AM_OFFS(1));
        _set_my10(AM_16BIT|AM_OFFS(-1));
    }
    _set_ay0(offs_Dst);
    _set_my00(AM_16BIT|AM_OFFS(1));

    for ( i = 0; i < Nb1; i++)
    {
        int k = i;
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ay1(pA);
            _set_ax0(pB++);
        } else {
            _set_ax0(pA);
            _set_ay1(pB++);
        }

        _mulrdw(0, 0);

        for ( j = 0; j <= k; j++)
        {
            _macrdw(_x0_u0(), _y1_u0());
        }
        _set_y0_u0( _get_ACC1());
    }
    pA++;

    for ( i = 0; i < Na1 - Nb1; i++)
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ay1(pA++);
            _set_ax0(pB-1);
        } else {
            _set_ax0(pA++);
            _set_ay1(pB-1);
        }

        _mulrdw(0, 0);

        for ( j = 0; j < Nb1; j++ )
        {
            _macrdw(_x0_u0(), _y1_u0());
        }
        _set_y0_u0( _get_ACC1());
    }
    if (Nb1==1)
        return;
    for ( i = 0; i < Nb1-1; i++ )
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ay1(pA++);
            _set_ax0(pB-1);
        } else {
            _set_ax0(pA++);
            _set_ay1(pB-1);
        }

        _mulrdw(0, 0);

        for ( j = 0; j < Nb1-i-1; j++ )
        {
            _macrdw(_x0_u0(), _y1_u0());
        }
        _set_y0_u0( _get_ACC1());
    }
}
// Produce instance with: MAC16x16; pOut - Y; pIn0 - Y; 

#pragma Code("dsplib_dsp_conv_q15_YXtoY_MAC16x16")

/**
 *  Q15 real linear convolution
 *
 * \param[in] offs_SrcA - XY memory offset to 1-st input vector. Vector length: nLenA
 * \param[in] offs_SrcB - XY memory offset to 1-st input vector. Vector length: nLenB
 * \param nLenA
 * \param nLenB
 * \param[out] offs_Dst - XY memory offset to output vector. Vector length: nLenA + nLenB - 1
 */

void dsp_conv_q15_YXtoY_MAC16x16(int32_t offs_SrcA, uint32_t nLenA, int32_t offs_SrcB, uint32_t nLenB, int32_t offs_Dst)  
{
    int32_t i, j; // loop counters
    int32_t pA, pB;
    int32_t Na1, Nb1;
    
    _set_MACMODE((1UL<<15)+(1UL<<3)+(1UL<<2));
    
    if ( (int32_t)nLenB > (int32_t)nLenA)
    {
        pA = offs_SrcB;
        Na1 = (int32_t)nLenB;
        pB = offs_SrcA;
        Nb1 = (int32_t)nLenA;  
        _set_my00(AM_16BIT|AM_OFFS(-1));
        _set_mx00(AM_16BIT|AM_OFFS(1));
    }
    else
    {
        pA = offs_SrcA;
        Na1 = (int32_t)nLenA;
        pB = offs_SrcB;
        Nb1 = (int32_t)nLenB;
        _set_my00(AM_16BIT|AM_OFFS(1));
        _set_mx00(AM_16BIT|AM_OFFS(-1));
    }
    _set_ay1(offs_Dst);
    _set_my10(AM_16BIT|AM_OFFS(1));

    for ( i = 0; i < Nb1; i++)
    {
        int k = i;
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ax0(pA);
            _set_ay0(pB++);
        } else {
            _set_ay0(pA);
            _set_ax0(pB++);
        }

        _mulrdw(0, 0);

        for ( j = 0; j <= k; j++)
        {
            _macrdw(_y0_u0(), _x0_u0());
        }
        _set_y1_u0( _get_ACC1());
    }
    pA++;

    for ( i = 0; i < Na1 - Nb1; i++)
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ax0(pA++);
            _set_ay0(pB-1);
        } else {
            _set_ay0(pA++);
            _set_ax0(pB-1);
        }

        _mulrdw(0, 0);

        for ( j = 0; j < Nb1; j++ )
        {
            _macrdw(_y0_u0(), _x0_u0());
        }
        _set_y1_u0( _get_ACC1());
    }
    if (Nb1==1)
        return;
    for ( i = 0; i < Nb1-1; i++ )
    {
        if ( (int32_t)nLenB > (int32_t)nLenA)
        {
            _set_ax0(pA++);
            _set_ay0(pB-1);
        } else {
            _set_ay0(pA++);
            _set_ax0(pB-1);
        }

        _mulrdw(0, 0);

        for ( j = 0; j < Nb1-i-1; j++ )
        {
            _macrdw(_y0_u0(), _x0_u0());
        }
        _set_y1_u0( _get_ACC1());
    }
}

#endif
