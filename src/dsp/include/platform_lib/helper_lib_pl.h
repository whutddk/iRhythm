/*
// CONFIDENTIAL AND PROPRIETARY INFORMATION
// 
// Copyright (c) 2015 Synopsys, Inc. All rights reserved. 
// This software and documentation contain confidential and
// proprietary information that is the property of 
// Synopsys, Inc. The software and documentation are 
// furnished under a license agreement and may be used 
// or copied only in accordance with the terms of the license 
// agreement. No part of the software and documentation 
// may be reproduced, transmitted, or translated, in any 
// form or by any means, electronic, mechanical, manual, 
// optical, or otherwise, without prior written permission 
// of Synopsys, Inc., or as expressly provided by the license agreement.
// Reverse engineering is prohibited, and reproduction, 
// disclosure or use without specific written authorization
// of Synopsys Inc. is strictly forbidden.
*/
//
#ifndef _HELPER_LIB_PL_H_
#define _HELPER_LIB_PL_H_

#include <stdint.h>

#ifdef API_0_85

#define _cache_invalidate(address, size) cache_invalidate((void*) 0,(uint32_t)address, size)
#define _cache_flush(address, size)      cache_flush((void*) 0,(uint32_t)address, size)

#include "arc_audio_api_helper_lib.h"

#else // 

#define _cache_invalidate(address, size) cache_invalidate_1_0(address, size)
#define _cache_flush(address, size)      cache_flush_1_0(address, size)

/* used to invalidate a block of cache at the given address.                */
void cache_invalidate_1_0(void* address, uint32_t size);

/* used to flush a block of cache at the given address.                     */
void cache_flush_1_0(void* address, uint32_t size);

#endif

#endif //_HELPER_LIB_PL_H_
