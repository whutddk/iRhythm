/* ***** BEGIN LICENSE BLOCK *****
 * Version: RCSL 1.0/RPSL 1.0
 *
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved.
 *
 * The contents of this file, and the files included with this file, are
 * subject to the current version of the RealNetworks Public Source License
 * Version 1.0 (the "RPSL") available at
 * http://www.helixcommunity.org/content/rpsl unless you have licensed
 * the file under the RealNetworks Community Source License Version 1.0
 * (the "RCSL") available at http://www.helixcommunity.org/content/rcsl,
 * in which case the RCSL will apply. You may also obtain the license terms
 * directly from RealNetworks.  You may not use this file except in
 * compliance with the RPSL or, if you have a valid RCSL with RealNetworks
 * applicable to this file, the RCSL.  Please see the applicable RPSL or
 * RCSL for the rights, obligations and limitations governing use of the
 * contents of the file.
 *
 * This file is part of the Helix DNA Technology. RealNetworks is the
 * developer of the Original Code and owns the copyrights in the portions
 * it created.
 *
 * This file, and the files included with this file, is distributed and made
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Technology Compatibility Kit Test Suite(s) Location:
 *    http://www.helixcommunity.org/content/tck
 *
 * Contributor(s):
 *
 * ***** END LICENSE BLOCK ***** */

/**************************************************************************************
 * Fixed-point MP3 decoder
 * Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
 * June 2003
 *
 * assembly.h - assembly language functions and prototypes for supported platforms
 *
 * - inline rountines with access to 64-bit multiply results
 * - x86 (_WIN32) and ARM (ARM_ADS, _WIN32_WCE) versions included
 * - some inline functions are mix of asm and C for speed
 * - some functions are in native asm files, so only the prototype is given here
 *
 * MULSHIFT32(x, y)    signed multiply of two 32-bit integers (x and y), returns top 32 bits of 64-bit result
 * FASTABS(x)          branchless absolute value of signed integer x
 * CLZ(x)              count leading zeros in x
 * MADD64(sum, x, y)   (Windows only) sum [64-bit] += x [32-bit] * y [32-bit]
 * SHL64(sum, x, y)    (Windows only) 64-bit left shift using __int64
 * SAR64(sum, x, y)    (Windows only) 64-bit right shift using __int64
 */

#ifndef _ASSEMBLY_H
#define _ASSEMBLY_H
//#include "arc_dsp_mw.h"
#include "embARC_toolchain.h"

#define ACC0_LO 0x580
#define ACC0_HI 0X582


typedef long long Word64;


static __inline Word64 MADD64(Word64 sum, int x, int y)
{
	return (Word64)x*y + sum;
	// Word64 res;
	// res = (Word64)x*(Word64)y;
	// sum = res + sum;
	// return sum;

	// Word64 res1;
	// int addr,zero;

	// zero = 0;
	// addr = ACC0_LO;

	// addr = ACC0_HI;
	// write_aux_reg(ACC0_LO, 0);
	// write_aux_reg(ACC0_HI, 0);

	// Asm("MAC %0, %1, %2" :"=r"(res1): "r"(x), "r"(y));


	// return res1;
	
}

static __inline Word64 SHL64(Word64 x, int n)
{
	// Word64 res;
	// res = x << n;
	// return res;
	return (x << n);
}

static __inline Word64 SAR64(Word64 x, int n)
{
	// Word64 res;
	// res = x >> n;
	// return res;
	return (x >> n);
}

static __inline int SAR32(int x, int n)
{
	int y;

	Asm("ASRS %0, %1, %2" :"=r"(y): "r"(x), "r"(n));

	return y;
	
}

static __inline int SAL32(int x, int n)
{
	int y;

	Asm("ASLS %0, %1, %2" :"=r"(y): "r"(x), "r"(n));

	return y;
	
}


static __inline int MULSHIFT32(int x, int y)
{
	// Word64  res;
	// res = (Word64)x*(Word64)y;
	// y = (int)(res>>32);
 // 	return y;
	// return _arc_mpym(x,y);

	int  res1,res2;
	// Asm("swape %0, %1" :"=r"(x): "r"(y));
	Asm("MPYM %0, %1, %2" :"=r"(res1): "r"(x), "r"(y));
	// Asm("MPYD %0, %2, %3\n\t"
	// 	"MOV %1,r1"
	// 	:"=r"(res1),"=r"(res2): "r"(x), "r"(y));
	return res1;
}

static __inline int FASTABS(int x)
{
	// return _arc_abss(x);
	// int sign;

	// sign = x >> (sizeof(int) * 8 - 1);
	// x ^= sign;
	// x -= sign;

	// return x;

	int y;
	Asm("ABSS %0, %1" :"=r"(x):  "r"(y));
	return y;

}

static __inline int FASSUB32(int x,int y)
{
	int z;

	Asm("SUBS %0, %1, %2" :"=r"(z): "r"(x), "r"(y));

	return z;

}

static __inline int FASADD32(int x,int y)
{
	int z;

	Asm("ADDS %0, %1, %2" :"=r"(z): "r"(x), "r"(y));

	return z;

}


static __inline int CLZ(int x)
{
	int numZeros;

	if (!x)
		return (sizeof(int) * 8);

	numZeros = 0;
	while (!(x & 0x80000000)) {
		numZeros++;
		x <<= 1;
	}

	return numZeros;
}


#endif /* _ASSEMBLY_H */
