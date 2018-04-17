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

#include "dsplib_types.h"
#include "dsplib_helper.h"

#if defined(_ARC600_XY) && defined(WRAPPER_BURSTS_ON)
#include "arc.h"

uint32_t memcpy_StoX_int16(uint32_t pdst, const q15_t* pSrc, int32_t size) {
	q15_t* xyptr = (q15_t*)_get_xbase();
	if (pdst & 0x1) {
		if ((uint32_t)pSrc & 0x3) {
			xyptr[pdst] = pSrc[0];
			_BurstSMemToXMemSafe((pdst + 1)>>1, (int32_t*)(void*)(pSrc + 1), (size >> 1));
			return pdst;
		} else 
		{
			_BurstSMemToXMemSafe((pdst + 1)>>1, (int32_t*)(void*)pSrc, ((size + 1) >> 1));
			return pdst + 1;
		}
	} else 
	{
		if ((uint32_t)pSrc & 0x3) {
			xyptr[pdst+1] = pSrc[0];
			_BurstSMemToXMemSafe((pdst + 2)>>1, (int32_t*)(void*)(pSrc + 1), (size >> 1));
			return pdst + 1;
		} else 
		{
			_BurstSMemToXMemSafe(pdst>>1, (int32_t*)(void*)pSrc, ((size + 1) >> 1));
			return pdst;
		}
	}
}

uint32_t memcpy_StoY_int16(uint32_t pdst, const q15_t* pSrc, int32_t size) {
	q15_t* xyptr = (q15_t*)_get_ybase();
	if (pdst & 0x1) {
		if ((uint32_t)pSrc & 0x3) {
			xyptr[pdst] = pSrc[0];
			_BurstSMemToYMemSafe((pdst + 1)>>1, (int32_t*)(void*)(pSrc + 1), (size >> 1));
			return pdst;
		} else 
		{
			_BurstSMemToYMemSafe((pdst + 1)>>1, (int32_t*)(void*)pSrc, ((size + 1) >> 1));
			return pdst + 1;
		}
	} else 
	{
		if ((uint32_t)pSrc & 0x3) {
			xyptr[pdst+1] = pSrc[0];
			_BurstSMemToYMemSafe((pdst + 2)>>1, (int32_t*)(void*)(pSrc + 1), (size >> 1));
			return pdst + 1;
		} else 
		{
			_BurstSMemToYMemSafe((pdst>>1), (int32_t*)(void*)pSrc, ((size + 1) >> 1));
			return pdst;
		}
	}
}

void memcpy_XtoS_int16(q15_t* pDst, uint32_t psrc, int32_t size) {
	q15_t* xyptr = (q15_t*)_get_xbase();
	if (psrc & 0x1) {
		if ((uint32_t)pDst & 0x3) {
			pDst[0] = xyptr[psrc];
			_BurstXMemToSMemSafe((int32_t*)(void*)(pDst + 1), (psrc + 1)>>1, ((size - 1) >> 1));
			if (!(size & 1)) pDst[size - 1] = xyptr[psrc + size - 1];
		} else 
			memcpy(pDst, xyptr + psrc, size);
	} else 
	{
		if ((uint32_t)pDst & 0x3) {
			memcpy(pDst, xyptr + psrc, size);
		} else 
		{
			_BurstXMemToSMemSafe((int32_t*)(void*)pDst, (psrc)>>1, (size >> 1));
			if (size & 1) pDst[size - 1] = xyptr[psrc + size - 1];
		}
	}
}

void memcpy_YtoS_int16(q15_t* pDst, uint32_t psrc, int32_t size) {
	q15_t* xyptr = (q15_t*)_get_ybase();
	if (psrc & 0x1) {
		if ((uint32_t)pDst & 0x3) {
			pDst[0] = xyptr[psrc];
			_BurstYMemToSMemSafe((int32_t*)(void*)(pDst + 1), (psrc + 1)>>1, ((size - 1) >> 1));
			if (!(size & 1)) pDst[size - 1] = xyptr[psrc + size - 1];
		} else 
			memcpy(pDst, xyptr + psrc, size);
	} else 
	{
		if ((uint32_t)pDst & 0x3) {
			memcpy(pDst, xyptr + psrc, size);
		} else 
		{
			_BurstYMemToSMemSafe((int32_t*)(void*)pDst, (psrc)>>1, (size >> 1));
			if (size & 1) pDst[size - 1] = xyptr[psrc + size - 1];
		}
	}
}

#endif
