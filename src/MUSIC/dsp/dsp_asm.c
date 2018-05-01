#include "embARC.h"
#include "embARC_debug.h"

// long long _arc_macc64(long long sum, int x, int y) 
// {

// 	Asm("MACDF %0, %1" :"=r"(temp): "r"(val));

	
// 	return sum;
// }
typedef long long Word64;

// static __inline Word64 SAR64(Word64 x, int n)
// {
// 	Word64 res;
// 	int HIGH,LOW;

// 	// Word64 res;
// 	// res = x >> n;
// 	// return res;
// 	Asm("ASRS %0, %1, %2" :"":"":"");

// 	Asm("swape %0, %1" :"=r"(temp): "r"(val));


// 	return (x >> n);
// }

static __inline int MULSHIFT32(int x, int y)
{
	int  res;
	// res = (Word64)x*(Word64)y;
	// y = (int)(res>>32);
 // 	return y;
	Asm("ASRS %0, %1, %2" :"=m"(res): "m"(x): "m"(y));
	return res;
	// return _arc_mpym(x,y);
}


#include "include.h"






