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

static __inline int FASMAX( int x,int y )
{
	int z;
	Asm("MAX %0, %1, %2" :"=r"(z): "r"(x),"r"(y));
	return z;
}

static __inline int FASMIN( int x,int y )
{
	int z;
	Asm("MIN %0, %1, %2" :"=r"(z): "r"(x),"r"(y));
	return z;
}
static __inline void MC0S(int * SL1,int * SR1,int * coef,int * vb1)
{
	/**伪代码

	LD r0，*coef
	negs r1 ,*(coef+1)

	LD r2，*coef+2
	negs r3 ,*coef+3

	LD r4，*coef+4
	negs r5 ,*coef+5

	LD r6，*coef+6
	negs r7 ,*coef+7

	LD r8，*coef+8
	negs r9 ,*coef+9

	LD r10，*coef+10
	negs r11 ,*coef+11

	LD r12，*coef+12
	negs r13 ,*coef+13

	LD r14，*coef+14
	negs r15,*coef+15

	sr ACC0_LO ,0
	sr ACC0_HI ,0

	MAC r16 *(vb1+0) r0
	mac r16 *(vb1+(23-0)) r1

	MAC r16 *(vb1+1) r2
	mac r16 *(vb1+(23-1)) r3

	MAC r16 *(vb1+2) r4
	mac r16 *(vb1+(23-2)) r5

	MAC r16 *(vb1+3) r6
	mac r16 *(vb1+(23-3)) r7

	MAC r16 *(vb1+4) r8
	mac r16 *(vb1+(23-4)) r9

	MAC r16 *(vb1+5) r10
	mac r16 *(vb1+(23-5)) r11

	MAC r16 *(vb1+6) r12
	mac r16 *(vb1+(23-6)) r13

	MAC r16 *(vb1+7) r14
	mac r16 *(vb1+(23-7)) r15

	lr *SL1 ACC0_HI


	sr ACC0_LO ,0
	sr ACC0_HI ,0

	MAC r16 *(vb1+32+0) r0
	mac r16 *(vb1+32+(23-0)) r1

	MAC r16 *(vb1+32+1) r2
	mac r16 *(vb1+32+(23-1)) r3

	MAC r16 *(vb1+32+2) r4
	mac r16 *(vb1+32+(23-2)) r5

	MAC r16 *(vb1+32+3) r6
	mac r16 *(vb1+32+(23-3)) r7

	MAC r16 *(vb1+32+4) r8
	mac r16 *(vb1+32+(23-4)) r9

	MAC r16 *(vb1+32+5) r10
	mac r16 *(vb1+32+(23-5)) r11

	MAC r16 *(vb1+32+6) r12
	mac r16 *(vb1+32+(23-6)) r13

	MAC r16 *(vb1+32+7) r14
	mac r16 *(vb1+32+(23-7)) r15

	lr *SR1 ACC0_HI


	**/

	// Asm("LD %%r0 %2\n\t"
	// "negs %%r1  %3\n\t"

	// "LD %%r2 %4\n\t"
	// "negs %%r3  %5\n\t"

	// "LD %%r4 %6\n\t"
	// "negs %%r5 %7\n\t"

	// "LD %%r6 %8\n\t"
	// "negs %%r7  %9\n\t"

	// "LD %%r8 %10\n\t"
	// "negs %%r9  %11\n\t"

	// "LD %%r10 %12\n\t"
	// "negs %%r11 %13\n\t"

	// "LD %%r12 %14\n\t"
	// "negs %%r13 %15\n\t"

	// "LD %%r14 %16\n\t"
	// "negs %%r15 %17\n\t"

	// "sr ACC0_LO 0\n\t"
	// "sr ACC0_HI 0\n\t"

	// "MAC %%r16 %18 %%r0\n\t"
	// "mac %%r16 %19 %%r1\n\t"

	// "MAC %%r16 %20 %%r2\n\t"
	// "mac %%r16 %21 %%r3\n\t"

	// "MAC %%r16 %22 %%r4\n\t"
	// "mac %%r16 %23 %%r5\n\t"

	// "MAC %%r16 %24 %%r6\n\t"
	// "mac %%r16 %25 %%r7\n\t"

	// "MAC %%r16 %26 %%r8\n\t"
	// "mac %%r16 %27 %%r9\n\t"

	// "MAC %%r16 %28 %%r10\n\t"
	// "mac %%r16 %29 %%r11\n\t"

	// "MAC %%r16 %30 %%r12\n\t"
	// "mac %%r16 %31 %%r13\n\t"

	// "MAC %%r16 %32 %%r14\n\t"
	// "mac %%r16 %33 %%r15\n\t"

	// "lr %0 ACC0_HI\n\t"


	// "sr ACC0_LO 0\n\t"
	// "sr ACC0_HI 0\n\t"

	// "MAC %%r16 %34 %%r0\n\t"
	// "mac %%r16 %35 %%r1\n\t"

	// "MAC %%r16 %36 r2\n\t"
	// "mac %%r16 %37 r3\n\t"

	// "MAC %%r16 %38 %%r4\n\t"
	// "mac %%r16 %39 %%r5\n\t"

	// "MAC %%r16 %40 %%r6\n\t"
	// "mac %%r16 %41 %%r7\n\t"

	// "MAC %%r16 %42 %%r8\n\t"
	// "mac %%r16 %43 %%r9\n\t"

	// "MAC %%r16 %44 %%r10\n\t"
	// "mac %%r16 %45 %%r11\n\t"

	// "MAC %%r16 %46 %%r12\n\t"
	// "mac %%r16 %47 %%r13\n\t"

	// "MAC %%r16 %48 %%r14\n\t"
	// "mac %%r16 %49 %%r15\n\t"

	// "lr %1 ACC0_HI\n\t"
	// : "=r"(*SL1),"=r"(*SR1)
	// : "r"(*coef),"r"(*(coef+1)),"r"(*(coef+2)),"r"(*(coef+3)),"r"(*(coef+4)),"r"(*(coef+5)),"r"(*(coef+6)),"r"(*(coef+7)),"r"(*(coef+8)),"r"(*(coef+9)),"r"(*(coef+10)),"r"(*(coef+11)),"r"(*(coef+12)),
	// 	"r"(*(coef+13)),"r"(*(coef+14)),"r"(*(coef+15)),"r"(*(vb1+0)),"r"(*(vb1+(23-0))),"r"(*(vb1+1)),"r"(*(vb1+(23-1))),"r"(*(vb1+2)),"r"(*(vb1+(23-2))),"r"(*(vb1+3)),"r"(*(vb1+(23-3))),
	// 	"r"(*(vb1+4)),"r"(*(vb1+(23-4))),"r"(*(vb1+5)),"r"(*(vb1+(23-5))),"r"(*(vb1+6)),"r"(*(vb1+(23-6))),"r"(*(vb1+7)),"r"(*(vb1+(23-7))),"r"(*(vb1+32+0)),"r"(*(vb1+32+(23-0))),"r"(*(vb1+32+1)),
	// 	"r"(*(vb1+32+(23-1))),"r"(*(vb1+32+2)),"r"(*(vb1+32+(23-2))),"r"(*(vb1+32+3)),"r"(*(vb1+32+(23-3))),"r"(*(vb1+32+4)),"r"(*(vb1+32+(23-4))),"r"(*(vb1+32+5)),"r"(*(vb1+32+(23-5))),"r"(*(vb1+32+6)),
	// 	"r"(*(vb1+32+(23-6))),"r"(*(vb1+32+7)),"r"(*(vb1+32+(23-7)))
	// : "%r0","%r1","%r2","%r3","%r4","%r5","%r6","%r7","%r8","%r9","%r10","%r11","%r12","%r13","%r14","%r15",
	// "%r16","%r0","%r16","%r1","%r16","%r2","%r16","%r3","%r16","%r4","%r16","%r5","%r16","%r6","%r16","%r7","%r16","%r8","%r16","%r9","%r16","%r10","%r16","%r11","%r16","%r12","%r16","%r13","%r16","%r14","%r16","%r15",
	// "%r16","%r0","%r16","%r1","%r16","%r2","%r16","%r3","%r16","%r4","%r16","%r5","%r16","%r6","%r16","%r7","%r16","%r8","%r16","%r9","%r16","%r10","%r16","%r11","%r16","%r12","%r16","%r13","%r16","%r14","%r16","%r15"
	// );


}
#endif /* _ASSEMBLY_H */
