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
#include "embARC.h"
#include "embARC_debug.h"

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

Asm("LD %%r0, %0\n\t"
	"negs %%r1, %1\n\t"
	"LD %%r2, %2\n\t"
	"negs %%r3, %3\n\t"
	"LD %%r4, %4\n\t"
	"negs %%r5, %5\n\t"
	"LD %%r6, %6\n\t"
	"negs %%r7,  %7\n\t"
	"LD %%r8, %8\n\t"
	"negs %%r9,  %9\n\t"
	"LD %%r10, %10\n\t"
	"negs %%r11, %11\n\t"
	"LD %%r12, %12\n\t"
	"negs %%r13, %13\n\t"
	"LD %%r14, %14\n\t"
	"negs %%r15, %15"
	:
	:"m"(*coef),"r"(*(coef+1)),"m"(*(coef+2)),"r"(*(coef+3)),"m"(*(coef+4)),"r"(*(coef+5)),"m"(*(coef+6)),"r"(*(coef+7)),"m"(*(coef+8)),"r"(*(coef+9)),"m"(*(coef+10)),"r"(*(coef+11)),"m"(*(coef+12)),
	"r"(*(coef+13)),"m"(*(coef+14)),"r"(*(coef+15))
	:"%r0","%r1","%r2","%r3","%r4","%r5","%r6","%r7","%r8","%r9","%r10","%r11","%r12","%r13","%r14","%r15"
	);

	_arc_aux_write(ACC0_LO, 0);
	_arc_aux_write(ACC0_HI, 0);

Asm("LD %%r17, %0\n\t"
	"MAC %%r16, %%r17, %%r0\n\t"
	"LD %%r17, %1\n\t"
	"mac %%r16, %%r17, %%r1\n\t"
	"LD %%r17, %2\n\t"
	"MAC %%r16, %%r17, %%r2\n\t"
	"LD %%r17, %3\n\t"
	"mac %%r16, %%r17, %%r3\n\t"
	"LD %%r17, %4\n\t"
	"MAC %%r16, %%r17, %%r4\n\t"
	"LD %%r17, %5\n\t"
	"mac %%r16, %%r17, %%r5\n\t"
	"LD %%r17, %6\n\t"
	"MAC %%r16, %%r17, %%r6\n\t"
	"LD %%r17, %7\n\t"
	"mac %%r16, %%r17, %%r7\n\t"
	"LD %%r17, %8\n\t"
	"MAC %%r16, %%r17, %%r8\n\t"
	"LD %%r17, %9\n\t"
	"mac %%r16, %%r17, %%r9\n\t"
	"LD %%r17, %10\n\t"
	"MAC %%r16, %%r17, %%r10\n\t"
	"LD %%r17, %11\n\t"
	"mac %%r16, %%r17, %%r11\n\t"
	"LD %%r17, %12\n\t"
	"MAC %%r16, %%r17, %%r12\n\t"
	"LD %%r17, %13\n\t"
	"mac %%r16, %%r17, %%r13\n\t"
	"LD %%r17, %14\n\t"
	"MAC %%r16, %%r17, %%r14\n\t"
	"LD %%r17, %15\n\t"
	"mac %%r16, %%r17, %%r15\n\t"

	: 
	: "m"(*(vb1+0)),"m"(*(vb1+(23))),"m"(*(vb1+1)),"m"(*(vb1+(22))),"m"(*(vb1+2)),"m"(*(vb1+(21))),"m"(*(vb1+3)),"m"(*(vb1+(20))),
	  "m"(*(vb1+4)),"m"(*(vb1+(19))),"m"(*(vb1+5)),"m"(*(vb1+(18))),"m"(*(vb1+6)),"m"(*(vb1+(17))),"m"(*(vb1+7)),"m"(*(vb1+(16)))
	: "%r16","%r17","%r0",
	"%r16","%r17","%r1",
	"%r16","%r17","%r2",
	"%r16","%r17","%r3",
	"%r16","%r17","%r4",
	"%r16","%r17","%r5",
	"%r16","%r17","%r6",
	"%r16","%r17","%r7",
	"%r16","%r17","%r8",
	"%r16","%r17","%r9",
	"%r16","%r17","%r10",
	"%r16","%r17","%r11",
	"%r16","%r17","%r12",
	"%r16","%r17","%r13",
	"%r16","%r17","%r14",
	"%r16","%r17","%r15"
);

	*SL1 = _arc_aux_read(ACC0_HI);

	_arc_aux_write(ACC0_LO, 0);
	_arc_aux_write(ACC0_HI, 0);

Asm("LD %%r17, %0\n\t"
	"MAC %%r16, %%r17, %%r0\n\t"
	"LD %%r17, %1\n\t"
	"mac %%r16, %%r17, %%r1\n\t"
	"LD %%r17, %2\n\t"
	"MAC %%r16, %%r17, %%r2\n\t"
	"LD %%r17, %3\n\t"
	"mac %%r16, %%r17, %%r3\n\t"
	"LD %%r17, %4\n\t"
	"MAC %%r16, %%r17, %%r4\n\t"
	"LD %%r17, %5\n\t"
	"mac %%r16, %%r17, %%r5\n\t"
	"LD %%r17, %6\n\t"
	"MAC %%r16, %%r17, %%r6\n\t"
	"LD %%r17, %7\n\t"
	"mac %%r16, %%r17, %%r7\n\t"
	"LD %%r17, %8\n\t"
	"MAC %%r16, %%r17, %%r8\n\t"
	"LD %%r17, %9\n\t"
	"mac %%r16, %%r17, %%r9\n\t"
	"LD %%r17, %10\n\t"
	"MAC %%r16, %%r17, %%r10\n\t"
	"LD %%r17, %11\n\t"
	"mac %%r16, %%r17, %%r11\n\t"
	"LD %%r17, %12\n\t"
	"MAC %%r16, %%r17, %%r12\n\t"
	"LD %%r17, %13\n\t"
	"mac %%r16, %%r17, %%r13\n\t"
	"LD %%r17, %14\n\t"
	"MAC %%r16, %%r17, %%r14\n\t"
	"LD %%r17, %15\n\t"
	"mac %%r16, %%r17, %%r15\n\t"
	: 
	: 	"m"(*(vb1+32+0)),"m"(*(vb1+32+(23-0))),"m"(*(vb1+32+1)),
		"m"(*(vb1+32+(23-1))),"m"(*(vb1+32+2)),"m"(*(vb1+32+(23-2))),"m"(*(vb1+32+3)),"m"(*(vb1+32+(23-3))),"m"(*(vb1+32+4)),"m"(*(vb1+32+(23-4))),"m"(*(vb1+32+5)),"m"(*(vb1+32+(23-5))),"m"(*(vb1+32+6)),
		"m"(*(vb1+32+(23-6))),"m"(*(vb1+32+7)),"m"(*(vb1+32+(23-7)))
	: 	"%r16","%r17","%r0",
	"%r16","%r17","%r1",
	"%r16","%r17","%r2",
	"%r16","%r17","%r3",
	"%r16","%r17","%r4",
	"%r16","%r17","%r5",
	"%r16","%r17","%r6",
	"%r16","%r17","%r7",
	"%r16","%r17","%r8",
	"%r16","%r17","%r9",
	"%r16","%r17","%r10",
	"%r16","%r17","%r11",
	"%r16","%r17","%r12",
	"%r16","%r17","%r13",
	"%r16","%r17","%r14",
	"%r16","%r17","%r15"
	);

	*SR1 = _arc_aux_read(ACC0_HI);

}


static __inline void MC1S(int * SL1,int * SR1,int * coef,int * vb1)
{
	/*
		伪代码
	
	ld r0, *coef
	ld r1,*(coef+1)
	ld r2,*(coef+2)
	ld r3,*(coef+3)
	ld r4,*(coef+4)
	ld r5,*(coef+5)
	ld r6,*(coef+6)
	ld r7,*(coef+7)

	ld r8,*(vb1)
	ld r9,*(vb1+1)
	ld r10,*(vb1+2)
	ld r11,*(vb1+3)
	ld r12,*(vb1+4)
	ld r13,*(vb1+5)
	ld r14,*(vb1+6)
	ld r15,*(vb1+7)

	_arc_aux_write(ACC0_LO, 0);
	_arc_aux_write(ACC0_HI, 0);
	mac r16 r8,r0
	mac r16 r9,r1
	mac r16 r10,r2
	mac r16 r11,r3
	mac r16 r12,r4
	mac r16 r13,r5
	mac r16 r14,r6
	mac r16 r15,r7
	*SL1 = _arc_aux_read(ACC0_HI);

	_arc_aux_write(ACC0_LO, 0);
	_arc_aux_write(ACC0_HI, 0);

	ld r8,*(vb1+32)
	ld r9,*(vb1+32+1)
	ld r10,*(vb1+32+2)
	ld r11,*(vb1+32+3)
	ld r12,*(vb1+32+4)
	ld r13,*(vb1+32+5)
	ld r14,*(vb1+32+6)
	ld r15,*(vb1+32+7)

	mac r16 r8,r0
	mac r16 r9,r1
	mac r16 r10,r2
	mac r16 r11,r3
	mac r16 r12,r4
	mac r16 r13,r5
	mac r16 r14,r6
	mac r16 r15,r7

	*SR1 = _arc_aux_read(ACC0_HI);
	*/


Asm(
	"ld %%r0, %0 \n\t"
	"ld %%r1, %1\n\t"
	"ld %%r2, %2\n\t"
	"ld %%r3, %3\n\t"
	"ld %%r4, %4\n\t"
	"ld %%r5, %5\n\t"
	"ld %%r6, %6\n\t"
	"ld %%r7, %7\n\t"

	"ld %%r8, %8\n\t"
	"ld %%r9, %9\n\t"
	"ld %%r10,%10\n\t"
	"ld %%r11,%11\n\t"
	"ld %%r12,%12\n\t"
	"ld %%r13,%13\n\t"
	"ld %%r14,%14\n\t"
	"ld %%r15,%15\n\t"
	:
	:"m"(*coef),"m"(*(coef+1)),"m"(*(coef+2)),"m"(*(coef+3)),"m"(*(coef+4)),"m"(*(coef+5)),
	"m"(*(coef+6)),"m"(*(coef+7)),"m"(*(vb1)),"m"(*(vb1+1)),"m"(*(vb1+2)),"m"(*(vb1+3)),
	"m"(*(vb1+4)),"m"(*(vb1+5)),"m"(*(vb1+6)),"m"(*(vb1+7))
	:"%r0","%r1","%r2","%r3","%r4","%r5","%r6","%r7","%r8","%r9","%r10","%r11","%r12","%r13","%r14","%r15"
	);

	_arc_aux_write(ACC0_LO, 0);
	_arc_aux_write(ACC0_HI, 0);

Asm("mac %%r16, %%r8, %%r0\n\t"
	"mac %%r16, %%r9, %%r1\n\t"
	"mac %%r16, %%r10, %%r2\n\t"
	"mac %%r16, %%r11, %%r3\n\t"
	"mac %%r16, %%r12, %%r4\n\t"
	"mac %%r16, %%r13, %%r5\n\t"
	"mac %%r16, %%r14, %%r6\n\t"
	"mac %%r16, %%r15, %%r7"
	:
	:
	:"%r16","%r8","%r0",
	"%r16","%r9","%r1",
	"%r16","%r10","%r2",
	"%r16","%r11","%r3",
	"%r16","%r12","%r4",
	"%r16","%r13","%r5",
	"%r16","%r14","%r6",
	"%r16","%r15","%r7"
	);

	*SL1 = _arc_aux_read(ACC0_HI);

	// Asm ( "LR %0, [0X582]\n\t "
	// 	:"=m" (*SR1));

	_arc_aux_write(ACC0_LO, 0);
	_arc_aux_write(ACC0_HI, 0);

Asm(
	"ld %%r8, %0\n\t"
	"ld %%r9, %1\n\t"
	"ld %%r10, %2\n\t"
	"ld %%r11, %3\n\t"
	"ld %%r12, %4\n\t"
	"ld %%r13, %5\n\t"
	"ld %%r14, %6\n\t"
	"ld %%r15, %7\n\t"

	"mac %%r16, %%r8, %%r0\n\t"
	"mac %%r16, %%r9, %%r1\n\t"
	"mac %%r16, %%r10, %%r2\n\t"
	"mac %%r16, %%r11, %%r3\n\t"
	"mac %%r16, %%r12, %%r4\n\t"
	"mac %%r16, %%r13, %%r5\n\t"
	"mac %%r16, %%r14, %%r6\n\t"
	"mac %%r16, %%r15, %%r7"
	:
	:"m"(*(vb1+32)),"m"(*(vb1+33)),"m"(*(vb1+34)),"m"(*(vb1+35)),"m"(*(vb1+36)),
	"m"(*(vb1+37)),"m"(*(vb1+38)),"m"(*(vb1+39))
	:"%r8","%r9","%r10","%r11","%r12","%r13","%r14","%r15",
	"%r16","%r8","%r0",
	"%r16","%r9","%r1",
	"%r16","%r10","%r2",
	"%r16","%r11","%r3",
	"%r16","%r12","%r4",
	"%r16","%r13","%r5",
	"%r16","%r14","%r6",
	"%r16","%r15","%r7"
);
	// *SR1 = _arc_aux_read(ACC0_HI);

	Asm ( "LR %%r17, [0X582]\n\t "
		"ST %%r17,%0"
		:"=m" (*SR1)
		:
		:"%r17");
}

static __inline void MC2S()
{
	/*伪代码
	ld r0, *coef
	ld r1,	*(coef+1)
	ld r2,	*(coef+2)
	ld r3,	*(coef+3)
	ld r4,	*(coef+4)
	ld r5,	*(coef+5)
	ld r6,	*(coef+6)
	ld r7,	*(coef+7)
	ld r8,	*(coef+8)
	ld r9,	*(coef+9)
	ld r10,	*(coef+10)
	ld r11,	*(coef+11)
	ld r12,	*(coef+12)
	ld r13,	*(coef+13)
	ld r14,	*(coef+14)
	ld r15,	*(coef+15)

	ld r16, *(vb1)
	ld r17, *(vb1+1)
	ld r18, *(vb1+2)
	ld r19, *(vb1+3)
	ld r20, *(vb1+4)
	ld r21, *(vb1+5)
	ld r22, *(vb1+6)
	ld r23, *(vb1+7)

//2L part1
	_arc_aux_write(ACC0_LO, 0);
	_arc_aux_write(ACC0_HI, 0);

	mac r25, r16, r1
	mac r25, r17, r3 
	mac r25, r18, r5
	mac r25, r19, r7 
	mac r25, r20, r9
	mac r25, r21, r11 
	mac r25, r22, r13
	mac r25, r23, r15 

	mac r25, r24, r0

	ld



	*/

}



#endif /* _ASSEMBLY_H */
