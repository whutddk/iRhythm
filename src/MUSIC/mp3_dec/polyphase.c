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
 * polyphase.c - final stage of subband transform (polyphase synthesis filter)
 *
 * This is the C reference version using __int64
 * Look in the appropriate subdirectories for optimized asm implementations
 *   (e.g. arm/asmpoly.s)
 **************************************************************************************/
#include "embARC.h"
#include "embARC_debug.h"

#include "coder.h"
#include "assembly.h"

/* input to Polyphase = Q(DQ_FRACBITS_OUT-2), gain 2 bits in convolution
 *  we also have the implicit bias of 2^15 to add back, so net fraction bits =
 *    DQ_FRACBITS_OUT - 2 - 2 - 15
 *  (see comment on Dequantize() for more info)
 */
#define DEF_NFRACBITS	(DQ_FRACBITS_OUT - 2 - 2 - 15)
#define CSHIFT	12	/* coefficients have 12 leading sign bits for early-terminating mulitplies */
#define MOVE_BIT 16 
#define CHECK_BIT 2
// static __inline short ClipToShort(int x, int fracBits)
// {
// 	int sign;

// 	/* assumes you've already rounded (x += (1 << (fracBits-1))) */
// 	x >>= fracBits;

// 	/* Ken's trick: clips to [-32768, 32767] */
// 	// sign = x >> 31;
// 	// if (sign != (x >> 15))
// 	// 	x = sign ^ ((1 << 15) - 1);

// 	return (short)x;
// }

#define MC0M(x)	{ \
	c1 = *coef;		coef++;		c2 = *coef;		coef++; \
	vLo = *(vb1+(x));			vHi = *(vb1+(23-(x))); \
	cal_temp0 = (short)(SAR32(vLo,MOVE_BIT)) * (short)(SAR32(c1,MOVE_BIT));  cal_temp1 = (short)(SAR32(vHi,MOVE_BIT))*(short)(SAR32(c2,MOVE_BIT)); \
	sum1L += cal_temp0 - cal_temp1; \
}

#define MC1M(x)	{ \
	c1 = *coef;		coef++; \
	vLo = *(vb1+(x)); \
	cal_temp0 = (short)(SAR32(vLo,MOVE_BIT)) * (short)(SAR32(c1,MOVE_BIT)); \
	sum1L += cal_temp0; \
}

#define MC2M(x)	{ \
		c1 = *coef;		coef++;		c2 = *coef;		coef++; \
		vLo = *(vb1+(x));	vHi = *(vb1+(23-(x))); \
		cal_temp0 = (short)(SAR32(vLo,MOVE_BIT)) * (short)(SAR32(c1,MOVE_BIT));  cal_temp1 = (short)(SAR32(vLo,MOVE_BIT))*(short)(SAR32(c2,MOVE_BIT)); \
		cal_temp2 = (short)(SAR32(vHi,MOVE_BIT)) * (short)(SAR32(c2,MOVE_BIT));  cal_temp3 = (short)(SAR32(vHi,MOVE_BIT))*(short)(SAR32(c1,MOVE_BIT)); \
		sum1L += cal_temp0 - cal_temp2; \
		sum2L += cal_temp1 + cal_temp3; \
}

/**************************************************************************************
 * Function:    PolyphaseMono
 *
 * Description: filter one subband and produce 32 output PCM samples for one channel
 *
 * Inputs:      pointer to PCM output buffer
 *              number of "extra shifts" (vbuf format = Q(DQ_FRACBITS_OUT-2))
 *              pointer to start of vbuf (preserved from last call)
 *              start of filter coefficient table (in proper, shuffled order)
 *              no minimum number of guard bits is required for input vbuf
 *                (see additional scaling comments below)
 *
 * Outputs:     32 samples of one channel of decoded PCM data, (i.e. Q16.0)
 *
 * Return:      none
 *
 * TODO:        add 32-bit version for platforms where 64-bit mul-acc is not supported
 *                (note max filter gain - see polyCoef[] comments)
 **************************************************************************************/
void PolyphaseMono(char *pcm, int *vbuf, const int *coefBase)
{
	int i;
	const int *coef;
	int *vb1;
	int vLo, vHi, c1, c2;
	// Word64 sum1L, sum2L, rndVal;
	int sum1L, sum2L, rndVal;
	int cal_temp0,cal_temp1,cal_temp2,cal_temp3;

	// rndVal = (Word64)( 1 << (DEF_NFRACBITS - 1 + (32 - CSHIFT)) );
	rndVal = 0;

	/* special case, output sample 0 */
	coef = coefBase;
	vb1 = vbuf;
	sum1L = rndVal;

	MC0M(0)
	MC0M(1)
	MC0M(2)
	MC0M(3)
	MC0M(4)
	MC0M(5)
	MC0M(6)
	MC0M(7)

	// *(pcm + 0) = (short)SAR64(sum1L, 32-CSHIFT + DEF_NFRACBITS);
	*(pcm + 0) = (char)(sum1L>>CHECK_BIT);

	/* special case, output sample 16 */
	coef = coefBase + 256;
	vb1 = vbuf + 64*16;
	sum1L = rndVal;

	MC1M(0)
	MC1M(1)
	MC1M(2)
	MC1M(3)
	MC1M(4)
	MC1M(5)
	MC1M(6)
	MC1M(7)

	// *(pcm + 16) = (short)SAR64(sum1L, (32 - CSHIFT + DEF_NFRACBITS));
	*(pcm + 16) = (char)(sum1L>>CHECK_BIT);

	/* main convolution loop: sum1L = samples 1, 2, 3, ... 15   sum2L = samples 31, 30, ... 17 */
	coef = coefBase + 16;
	vb1 = vbuf + 64;
	pcm++;

	/* right now, the compiler creates bad asm from this... */
	for (i = 15; i > 0; i--) {
		sum1L = sum2L = rndVal;

		MC2M(0)
		MC2M(1)
		MC2M(2)
		MC2M(3)
		MC2M(4)
		MC2M(5)
		MC2M(6)
		MC2M(7)

		vb1 += 64;
		// *(pcm)       = (short)SAR64(sum1L, (32-CSHIFT + DEF_NFRACBITS));
		// *(pcm + 2*i) = (short)SAR64(sum2L, (32-CSHIFT + DEF_NFRACBITS));
		*(pcm)       = (char)(sum1L>>CHECK_BIT);
		*(pcm + 2*i) = (char)(sum2L>>CHECK_BIT);

		pcm++;
	}
}


/*****************Part 1****************************************/
#define MC0SL(x)	{ \
	c1[(x)] = *coef;		coef++;		\
	Asm("NEGS %0, %1" :"=r"(n_c2[(x)]):"r"(*coef));\
	coef++; \
	vLo[(x)] = *(vb1+(x));		vHi[(x)] = *(vb1+(23-(x))); \
	\
	Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vLo[(x)]), "r"(c1[(x)]));	\
	Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vHi[(x)]), "r"(n_c2[(x)]));	\
}

#define MC0SR(x)	{  \
	vLo[(x)] = *(vb1+(32+(x)));	vHi[(x)] = *(vb1+(55-(x))); \
	\
	Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vLo[(x)]), "r"(c1[(x)]));	\
	Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vHi[(x)]), "r"(n_c2[(x)]));	\
}

/*****************Part 2****************************************/
#define MC1SL(x)	{ \
	c1[(x)] = *coef;		coef++; \
	vLo[(x)] = *(vb1+(x)); \
	\
	Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vLo[(x)]), "r"(c1[(x)]));	\
}

#define MC1SR(x)	{ \
	vLo[(x)] = *(vb1+(32+(x))); \
	\
	Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vLo[(x)]), "r"(c1[(x)]));	\
}



/*****************Part 3****************************************/
/***2nd*********/
#define MC2S1L(x)	{ \
		Asm("NEGS %0, %1" :"=r"(n_c2[(x)]):"r"(c2[(x)]));\
		\
		Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vLo[(x)]), "r"(c1[(x)]));	\
		Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vHi[(x)]), "r"(n_c2[(x)]));	\
}

/****first****/
#define MC2S2L(x)	{ \
		c1[(x)] = *coef;		coef++;		c2[(x)] = *coef;		coef++; \
		vLo[(x)] = *(vb1+(x));	vHi[(x)] = *(vb1+(23-(x))); \
		\
		Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vLo[(x)]), "r"(c2[(x)]));	\
		Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vHi[(x)]), "r"(c1[(x)]));	\
}

/***3rd*********/
#define MC2S1R(x)	{ \
		vLo[(x)] = *(vb1+(32+(x)));	vHi[(x)] = *(vb1+(55-(x))); \
		\
		Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vLo[(x)]), "r"(c1[(x)]));	\
		Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vHi[(x)]), "r"(n_c2[(x)]));	\
}

/***4th*********/
#define MC2S2R(x)	{ \
		\
		Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vLo[(x)]), "r"(c2[(x)]));	\
		Asm("MAC %0, %1, %2" :"=r"(cal_temp0): "r"(vHi[(x)]), "r"(c1[(x)]));	\
}

/**************************************************************************************
 * Function:    PolyphaseStereo
 *
 * Description: filter one subband and produce 32 output PCM samples for each channel
 *
 * Inputs:      pointer to PCM output buffer
 *              number of "extra shifts" (vbuf format = Q(DQ_FRACBITS_OUT-2))
 *              pointer to start of vbuf (preserved from last call)
 *              start of filter coefficient table (in proper, shuffled order)
 *              no minimum number of guard bits is required for input vbuf
 *                (see additional scaling comments below)
 *
 * Outputs:     32 samples of two channels of decoded PCM data, (i.e. Q16.0)
 *
 * Return:      none
 *
 * Notes:       interleaves PCM samples LRLRLR...
 *
 * TODO:        add 32-bit version for platforms where 64-bit mul-acc is not supported
 **************************************************************************************/
void PolyphaseStereo(char *pcm, int *vbuf, const int *coefBase)
{
	int i;
	int *coef;
	int *vb1;
	int vLo[8], vHi[8], c1[8], c2[8],n_c2[8];
	// Word64 sum1L, sum2L, sum1R, sum2R, rndVal;
	int sum1L, sum2L, sum1R, sum2R;
	int cal_temp0;

	// rndVal = (Word64)( 1 << (DEF_NFRACBITS - 1 + (32 - CSHIFT)) );
	// rndVal = 0 ;

/*****************Part 1****************************************/
	/* special case, output sample 0 */
	coef = coefBase;
	vb1 = vbuf;
	// sum1L =  0;

//Reset ACC
	// _arc_aux_write(ACC0_LO, 0);
	// _arc_aux_write(ACC0_HI, 0);

	// MC0SL(0)
	// MC0SL(1)
	// MC0SL(2)
	// MC0SL(3)
	// MC0SL(4)
	// MC0SL(5)
	// MC0SL(6)
	// MC0SL(7)

	// sum1L = _arc_aux_read(ACC0_HI);


	MC0S(&sum1L,&sum1R,coef,vb1);


	// coef = coefBase;
	// vb1 = vbuf;
	// sum1R = 0;

//Reset ACC
	// _arc_aux_write(ACC0_LO, 0);
	// _arc_aux_write(ACC0_HI, 0);

	// MC0SR(0)
	// MC0SR(1)
	// MC0SR(2)
	// MC0SR(3)
	// MC0SR(4)
	// MC0SR(5)
	// MC0SR(6)
	// MC0SR(7)

	// sum1R = _arc_aux_read(ACC0_HI);

	*(pcm ) = (char)(SAR32(sum1L,CHECK_BIT));
	*(pcm + 1) = (char)(SAR32(sum1R,CHECK_BIT));

/*****************Part 2****************************************/
	/* special case, output sample 16 */
	coef = coefBase + 256;
	vb1 = vbuf + 1024;
	// sum1L = 0;

//Reset ACC
	// _arc_aux_write(ACC0_LO, 0);
	_arc_aux_write(ACC0_HI, 0);

	MC1SL(0)
	MC1SL(1)
	MC1SL(2)
	MC1SL(3)
	MC1SL(4)
	MC1SL(5)
	MC1SL(6)
	MC1SL(7)

	sum1L = _arc_aux_read(ACC0_HI);


	//Reset ACC
	// _arc_aux_write(ACC0_LO, 0);
	_arc_aux_write(ACC0_HI, 0);

	MC1SR(0)
	MC1SR(1)
	MC1SR(2)
	MC1SR(3)
	MC1SR(4)
	MC1SR(5)
	MC1SR(6)
	MC1SR(7)

	sum1R = _arc_aux_read(ACC0_HI);

	*(pcm + 32) = (char)(SAR32(sum1L,CHECK_BIT));
	*(pcm + 33) = (char)(SAR32(sum1R,CHECK_BIT));

/*****************Part 3****************************************/

	/* main convolution loop: sum1L = samples 1, 2, 3, ... 15   sum2L = samples 31, 30, ... 17 */
	coef = coefBase + 16;
	vb1 = vbuf + 64;
	pcm += 2;

	/* right now, the compiler creates bad asm from this... */
	for (i = 15; i > 0; i--) {
		// sum1L = sum2L = 0;
		// sum1R = sum2R = 0;

		// coef_save = coef;	//SAVE

//Reset ACC
// _arc_aux_write(ACC0_LO, 0);
_arc_aux_write(ACC0_HI, 0);

		MC2S2L(0)
		MC2S2L(1)
		MC2S2L(2)
		MC2S2L(3)
		MC2S2L(4)
		MC2S2L(5)
		MC2S2L(6)
		MC2S2L(7)

sum2L = _arc_aux_read(ACC0_HI);

		// coef = coef_save;	//Load
//Reset ACC
// _arc_aux_write(ACC0_LO, 0);
_arc_aux_write(ACC0_HI, 0);

		MC2S1L(0)
		MC2S1L(1)
		MC2S1L(2)
		MC2S1L(3)
		MC2S1L(4)
		MC2S1L(5)
		MC2S1L(6)
		MC2S1L(7)

sum1L = _arc_aux_read(ACC0_HI);



		// coef = coef_save;	//RELoad

//Reset ACC
// _arc_aux_write(ACC0_LO, 0);
_arc_aux_write(ACC0_HI, 0);

		MC2S1R(0)
		MC2S1R(1)
		MC2S1R(2)
		MC2S1R(3)
		MC2S1R(4)
		MC2S1R(5)
		MC2S1R(6)
		MC2S1R(7)

sum1R = _arc_aux_read(ACC0_HI);

		// coef = coef_save;	//RELoad

//Reset ACC
// _arc_aux_write(ACC0_LO, 0);
_arc_aux_write(ACC0_HI, 0);

		MC2S2R(0)
		MC2S2R(1)
		MC2S2R(2)
		MC2S2R(3)
		MC2S2R(4)
		MC2S2R(5)
		MC2S2R(6)
		MC2S2R(7)

sum2R = _arc_aux_read(ACC0_HI);

		vb1 += 64;
		*(pcm )         = (char)(SAR32(sum1L,CHECK_BIT));
		*(pcm + 1)         = (char)(SAR32(sum1R,CHECK_BIT));
		*(pcm + 4*i) = (char)(SAR32(sum2L,CHECK_BIT));
		*(pcm + 4*i + 1) = (char)(SAR32(sum2R,CHECK_BIT));
		pcm += 2;
	}
}
