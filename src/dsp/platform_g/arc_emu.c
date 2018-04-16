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
 
#include "arc.h"

// Define xy-memory "traps" on writing/reading at the specific address, on the certain value or both
//#define TRAP_XMEM_ADDR_READ         0
//#define TRAP_XMEM_VALUE_READ        0

//#define TRAP_YMEM_ADDR_READ         0
//#define TRAP_YMEM_VALUE_READ        0

//#define TRAP_XMEM_ADDR_WRITE        0
//#define TRAP_XMEM_VALUE_WRITE       0

//#define TRAP_YMEM_ADDR_WRITE        0
//#define TRAP_YMEM_VALUE_WRITE       0

//#define TRAP_SAT

#undef TRAP

#ifdef TRAP_SAT
    
#define TRAP(s) trap(s)
#include <stdio.h>
void trap(char * trap_str){
    int brk = 1;
}

#else
#define TRAP(s)  
#endif


#if !defined(_ARCVER_ARCv2EM) && !defined(_ARCVER_ARCv2HS)

#if defined(EMU__Xea) || defined(EMU__Xmul32x16) || defined(EMU__Xswap) || defined(EMU__Xnorm) \
    || defined(EMU__Xxmac_d16) || defined(EMU__Xxmac_24) || defined(EMU__Xmpy16) || !defined(_ARC)

// Condition flags
int32_t _Z = 0;    // Zero
int32_t _N = 0;    // Negative
int32_t _C = 0;    // Carry
int32_t _V = 0;    // Overflow

// macro extracts flags with STATUS32 register format
#define GET_FLAGS(F_RG,ZF,NF,CF,VF) \
    ZF = (F_RG & (1 << 11)) >> 11;  \
    NF = (F_RG & (1 << 10)) >> 10;  \
    CF = (F_RG & (1 << 9)) >> 9;    \
    VF = (F_RG & (1 << 8)) >> 8;

// macro sets flags to STATUS32 register
#define SET_FLAGS(F_RG,ZF,NF,CF,VF) \
    F_RG &= 0xfffff0ff; \
    F_RG = F_RG | (ZF << 11) | (NF << 10) | (CF << 9) | (VF << 8);

#define ASch1_B 13
#define AZch1_B 12
#define ANch1_B 11
#define PSch1_B 10
#define Sch1_B   9
#define ASch2_B  8
#define AZch2_B  7
#define ANch2_B  6
#define PSch2_B  5
#define Sch2_B   4

#define GET_MACMODEFLAGS(MACMODE_RG,ASch1,AZch1,ANch1,PSch1,Sch1,ASch2,AZch2,ANch2,PSch2,Sch2) \
    ASch1 = (MACMODE_RG & (1 << ASch1_B)) >> ASch1_B;  \
    AZch1 = (MACMODE_RG & (1 << AZch1_B)) >> AZch1_B;  \
    ANch1 = (MACMODE_RG & (1 << ANch1_B)) >> ANch1_B;  \
    PSch1 = (MACMODE_RG & (1 << PSch1_B)) >> PSch1_B;  \
    Sch1  = (MACMODE_RG & (1 << Sch1_B )) >> Sch1_B ;  \
    ASch2 = (MACMODE_RG & (1 << ASch2_B)) >> ASch2_B;  \
    AZch2 = (MACMODE_RG & (1 << AZch2_B)) >> AZch2_B;  \
    ANch2 = (MACMODE_RG & (1 << ANch2_B)) >> ANch2_B;  \
    PSch2 = (MACMODE_RG & (1 << PSch2_B)) >> PSch2_B;  \
    Sch2  = (MACMODE_RG & (1 << Sch2_B )) >> Sch2_B ;

#define SET_MACMODEFLAGS(MACMODE_RG,ASch1,AZch1,ANch1,PSch1,Sch1,ASch2,AZch2,ANch2,PSch2,Sch2) \
    MACMODE_RG &= 0xffffc00f; \
    MACMODE_RG = MACMODE_RG \
    | (ASch1 << ASch1_B) \
    | (AZch1 << AZch1_B) \
    | (ANch1 << ANch1_B) \
    | (PSch1 << PSch1_B) \
    | (Sch1  << Sch1_B ) \
    | (ASch2 << ASch2_B) \
    | (AZch2 << AZch2_B) \
    | (ANch2 << ANch2_B) \
    | (PSch2 << PSch2_B) \
    | (Sch2  << Sch2_B );

// saturations
#define SAT(DST, SRC, W, T) {\
    if ((SRC) > (1LL<<(W-1))-1) {   \
    TRAP("saturation occured"); \
    (DST) = (T)((1LL<<(W-1))-1);     \
    } else if ((SRC) < (-1LL<<(W-1))) { \
    TRAP("saturation occured"); \
    (DST) = (T)(-1LL<<(W-1));      \
     } else {                        \
    (DST) = (T)(SRC);           \
} \
}

#define SAT_VF(DST, SRC, W, VF, T) {\
    if ((SRC) > (1LL<<(W-1))-1) {   \
    TRAP("saturation occured"); \
    (DST) = (T)((1LL<<(W-1))-1);     \
    VF = 1;                     \
    } else if ((SRC) < (-1LL<<(W-1))) { \
    TRAP("saturation occured"); \
    (DST) = (T)(-1LL<<(W-1));      \
    VF = 1;                     \
    } else {                        \
    (DST) = (T)(SRC);           \
} \
}

#define SATU(DST, SRC, W, T) {  \
    if (((uint64_t)SRC) > ((1LL<<(W))-1)) { \
    TRAP("saturation occured"); \
    (DST) = (T)((1LL<<(W))-1);   \
    } else {                    \
    (DST) = (T)(SRC);       \
    } \
}

#define SATU_VF(DST, SRC, W, VF, T) {  \
    if (((uint64_t)SRC) > ((1LL<<(W))-1)) { \
    TRAP("saturation occured"); \
    (DST) = (T)((1LL<<(W))-1);   \
    VF = 1;                 \
    } else {                    \
    (DST) = (T)(SRC);       \
    } \
}

static int64_t sat_add64_ovf(int64_t src1, int64_t src2, int32_t* ovf)
{
    int64_t r = src1 + src2;
    *ovf = ( ((src1 > 0) && (src2 > INT64_MAX - src1)) || ((src1 < 0) && (src2 < INT64_MIN - src1)) );
    if (*ovf)
    {
        TRAP("64-bit saturation occured");
        if (r < 0)
            r = 0x7FFFFFFFFFFFFFFF;
        else
            r = 0x8000000000000000;
    }
    return r;
}

static uint64_t sat_addu64_ovf(uint64_t src1, uint64_t src2, int32_t* ovf)
{
    uint64_t r = src1 + src2;
    *ovf = (src2 > UINT64_MAX - src1);
    if (*ovf) {
        TRAP("64-bit unsigned saturation occured");
        r = 0xFFFFFFFFFFFFFFFF;
    }
    return r;
}

#define MAX_32 (int32_t)0x7fffffffL
#define MIN_32 (int32_t)0x80000000L

#define INT_MAX (int32_t)0x7fffffff
#define INT_MIN (int32_t)0x80000000

static int32_t saturate32(int64_t L_var1)
{
    int32_t var_out = (int32_t)L_var1;

    if (L_var1 > (((int64_t)1 << 31) - 1))
    {
        TRAP("32-bit saturation occured");
        var_out = (((int64_t)1 << 31) - 1);
    }
    if (L_var1 < ((int64_t)-1 << 31))
    {
        TRAP("32-bit saturation occured");
        var_out = ((int64_t)-1 << 31);
    }

    return var_out;
}

static uint32_t saturateu32(uint64_t L_var1)
{
    uint32_t var_out = (uint32_t)L_var1;

    if (L_var1 > (((uint64_t)1 << 32) - 1))
    {
        TRAP("32-bit unsigned saturation occured");
        var_out = (((uint64_t)1 << 32) - 1);
    }
    return var_out;
}

static int64_t saturate40(int64_t L_var1)
{
    int64_t var_out = L_var1;

    if (L_var1 > (((int64_t)1 << 39) - 1))
    {
        TRAP("40-bit saturation occured");
        var_out = (((int64_t)1 << 39) - 1);
    }
    if (L_var1 < ((int64_t)-1 << 39))
    {
        TRAP("40-bit saturation occured");
        var_out = ((int64_t)-1 << 39);
    }

    return var_out;
}

static uint64_t saturateu40(uint64_t L_var1)
{
    uint64_t var_out = L_var1;

    if (L_var1 > (((uint64_t)1 << 40) - 1))
    {
        TRAP("40-bit unsigned saturation occured");
        var_out = (((uint64_t)1 << 40) - 1);
    }

    return var_out;
}

#define MAX_16 32767
#define MIN_16 -32768

int16_t saturate16(int32_t L_var1)
{
    if(L_var1 < MIN_16)
    {
        TRAP("16-bit saturation occured");
        return MIN_16;
    }
    if(L_var1 > MAX_16)
    {
        TRAP("16-bit saturation occured");
        return MAX_16;
    }
    return (int16_t)L_var1;
}

uint16_t saturateu16(uint32_t L_var1)
{
    if(L_var1 > (0xffff))
    {
        TRAP("16-bit unsigned saturation occured");
        return (0xffff);
    }
    return (uint16_t)L_var1;
}

//
// MUL/MAC 24x24 fractional multiply/accumulate with 48-bit saturation
//

#define Q24         (1UL<<16)
#define P24         (1UL<<17)
#define RM24        (1UL<<18)

#define HIGH24(X)   ((X)>>8)
#define RND(X, A, S) X = (A + (1 << S))
#define RND24(X)    RND(X, Internal_AA_24X24, 23)

static int test_add_ovf(int32_t src1, int32_t src2)
{
    return ( ((src1 > 0) && (src2 > INT_MAX - src1)) || ((src1 < 0) && (src2 < INT_MIN - src1)) );
}

static int test_sub_ovf(int32_t a, int32_t b)
{
    int ub = 0;

    if ((b > 0 && a < INT_MIN + b) ||
        (b < 0 && a > INT_MAX + b)) {
            ub = 1;
    }
    return ub;
}

#endif

#if defined(EMU__Xmul32x16)
    static int64_t AA_32X16;
    uint32_t _get_AUX_XMACLW_H() { return (int32_t)(AA_32X16 >> 32); }
    uint32_t _get_AUX_XMACLW_L() { return (uint32_t)AA_32X16; }
    void _set_AUX_XMACLW_H(uint32_t val) { AA_32X16 = (AA_32X16 & 0xffffffff) | ((int64_t)val << 32);}
    void _set_AUX_XMACLW_L(uint32_t val) { AA_32X16 = (AA_32X16 & 0xffffffff00000000) | val;}
#endif

#if defined(EMU__Xxmac_d16)
    static int64_t AA1;
    static int64_t AA2;
    static uint32_t EMU_AUX_FBF_STORE_16;
    uint32_t _get_AUX_XMAC0() { return ( (((AA1<<24>>24)>>(32 - 16)) & 0xffff0000) | (((AA2<<24>>24)>>32) & 0xffff) ); }
    uint32_t _get_AUX_XMAC1() { return (uint32_t)(AA1); }
    uint32_t _get_AUX_XMAC2() { return (uint32_t)(AA2); }
    void _set_AUX_XMAC0(uint32_t val) { AA1 = (AA1 & 0xffffffff) | ((int64_t)(val >> 16) << 32 << 24 >> 24); AA2 = (AA2 & 0xffffffff) | ((int64_t)(val & 0xffff) << 32 << 24 >> 24);}
    void _set_AUX_XMAC1(uint32_t val) { AA1 = (int32_t)val; }
    void _set_AUX_XMAC2(uint32_t val) { AA2 = (int32_t)val; }
    uint32_t _get_AUX_FBF_STORE_16() { return EMU_AUX_FBF_STORE_16; }
    void _set_AUX_FBF_STORE_16(uint32_t val) { EMU_AUX_FBF_STORE_16 = val; }
#endif

#if defined(EMU__Xxmac_24)
    static int64_t Internal_AA_24X24;
    uint32_t _get_AUX_XMAC0_24() { return (int8_t)(Internal_AA_24X24>>48); }
    uint32_t _get_AUX_XMAC1_24() { return (uint32_t)(Internal_AA_24X24>>16); }
    uint32_t _get_AUX_XMAC2_24() { return (uint32_t)(Internal_AA_24X24<<16); }
    void _set_AUX_XMAC0_24(uint32_t val) { Internal_AA_24X24 = (Internal_AA_24X24 & 0x0000ffffffffffff) | ((int64_t)val << 56 >> 8); }
    void _set_AUX_XMAC1_24(uint32_t val) { Internal_AA_24X24 = (int64_t)(int32_t)val << 16; }
    void _set_AUX_XMAC2_24(uint32_t val) { Internal_AA_24X24 = (Internal_AA_24X24 & 0xffffffffffff0000) | (val >> 16); }
#endif

#ifdef EMU__Xdmulpf
static int32_t ACC1_H_dmulpf;
static int32_t ACC2_H_dmulpf;
#endif // EMU__Xdmulpf

#ifdef EMU__Xcrc
static uint32_t aux_crc_mode;
static uint32_t aux_crc_poly;
#endif // EMU__Xcrc


#if !defined(_ARC) || (!defined(__Xmul32x16) && !defined(__Xxmac_d16) && (defined(EMU__Xmul32x16) || defined(EMU__Xxmac_d16)))
static int32_t RISC_ACC1;
static uint32_t RISC_ACC2;

int32_t  _get_ACC1() { return RISC_ACC1; }
uint32_t _get_ACC2() { return RISC_ACC2; }

void  _set_ACC1(uint32_t x) { RISC_ACC1 = x; }
void  _set_ACC2(uint32_t x) { RISC_ACC2 = x; }
#endif

/*
 18 17 16    15 14    13    12    11    10    9     8     7     6     5    4   3   2  1  0
           RM16  G ASch1 AZch1 ANch1 PSch1 Sch1 ASch2 AZch2 ANch2 PSch2 Sch2 P16 Q16 CS CA
                  |      channel 1 flags       |        channel 2 flags     |
*/

#if !defined(_ARC) || (!defined(__Xmul32x16) && !defined(__Xxmac_d16) && !defined(__Xxmac_24) && (defined(EMU__Xmul32x16) || defined(EMU__Xxmac_d16) || defined(EMU__Xxmac_24)))
static uint32_t RISC_AUX_MACMODE;

uint32_t _get_MACMODE() { return RISC_AUX_MACMODE; }
void _set_MACMODE(uint32_t mode) {
    int32_t P,Q,G,RM,CS,CA;
    CA = mode & 1; //
    CS = (mode & (1<<1)) >> 1; //
    Q = (mode & (1<<2)) >> 2; //
    P = (mode & (1<<3)) >> 3; //
    G = (mode & (1<<14)) >> 14; //
    RM = (mode & (1<<15)) >> 15; //

    ASSERT( ((CA|CS) == 0) || ((Q | P | G | RM) == 0) );
    ASSERT((mode & 0xfff80000) == 0); // reserved bits are '0'

    if (CA) {
        RISC_AUX_MACMODE &= 0xffffdeff; // clear ASch1, ASch2
    }
    if (CS) {
        RISC_AUX_MACMODE &= 0xfffffdef; // clear Sch1, Sch2
    }

    RISC_AUX_MACMODE = mode;
}
#endif

#ifndef _ARC

uint32_t _get_STATUS32() 
{
    uint32_t r = 0;
    r = (_Z << 11) | (_N << 10) | (_C << 9) | (_V << 8);
    return r;
}

#endif // !_ARC


/*
    Arithmetic and Logical Operations
*/

#ifndef _ARC

int32_t _add(int32_t src1, int32_t src2)
{
    return src1 + src2;
}

int32_t _add_f(int32_t src1, int32_t src2)
{
    int32_t r;
    _V = 0;

    r = src1 + src2;
    if ( test_add_ovf(src1 , src2) ) 
    { 
        _V = 1;
    }

    _C = ((((uint64_t)(uint32_t)src1 + (uint64_t)(uint32_t)src2) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _adc(int32_t src1, int32_t src2)
{
    return src1 + src2 + _C;
}

int32_t _adc_f(int32_t src1, int32_t src2)
{
    int32_t r;
    _V = 0; 

    if (src1 < src2)
    {
        if ( test_add_ovf(src1 + _C , src2) ) 
            _V = 1;
    } 
    else
    {
        if (src2 == 0x7fffffff)
        {
            _V = 1;
        } else {
            if ( test_add_ovf(src1 , src2 + _C) ) 
                _V = 1;
        }
    }

    r = src1 + src2;

    r = r + _C;

    _C = ((((uint64_t)(uint32_t)src1 + (uint64_t)(uint32_t)src2 + _C) & ((uint64_t)1 << 32)) ? 1 : 0);
    _N = r < 0;
    _Z = r == 0;

    return r;
}

int32_t _sub(int32_t src1, int32_t src2)
{
    return src1 - src2;
}

int32_t _sub_f(int32_t src1, int32_t src2)
{
    int32_t r;
    _V = 0;

    r = src1 - src2;

    if ( test_sub_ovf(src1, src2) )
        _V = 1;

    _C = ((((uint64_t)(uint32_t)src1 - ((uint32_t)src2)) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _sbc(int32_t src1, int32_t src2)
{
    return src1 - src2 - _C;
}

int32_t _sbc_f(int32_t src1, int32_t src2)
{
    int32_t r;
    _V = 0;

    if (src1 != 0x80000000)
    {
        if ( test_sub_ovf(src1 - _C , src2) ) 
            _V = 1;
    } 
    else
    {
        if (src2 > 0)
        {
            _V = 1;
        } else {
            if ( test_sub_ovf(src1 , src2 + _C) ) 
                _V = 1;
        }
    }

    r = src1 - src2;
    r = r - _C;

    _C = ((((uint64_t)(uint32_t)src1 - (uint64_t)(uint32_t)src2 - _C) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _and(int32_t src1, int32_t src2)
{
    return (src1 & src2);
}

int32_t _and_f(int32_t src1, int32_t src2)
{
    int32_t r = (src1 & src2);

    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _or(int32_t src1, int32_t src2)
{
    return (src1 | src2);
}

int32_t _or_f(int32_t src1, int32_t src2)
{
    int32_t r = (src1 | src2);

    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _bic(int32_t src1, int32_t src2)
{
    return (src1 & (~src2)); 
}

int32_t _bic_f(int32_t src1, int32_t src2)
{
    int32_t r = src1 & (~src2);
    _Z = r == 0;
    _N = r < 0;
    return (r);
}

int32_t _xor(int32_t a, int32_t b)
{
    int32_t r = a ^ b;
    return r;
}

int32_t _xor_f(int32_t a, int32_t b)
{
    int32_t r = a ^ b;

    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _max(int32_t a, int32_t b)
{
    return (((a) > (b)) ? (a) : (b));
}

int32_t _max_f(int32_t a, int32_t b)
{
    int32_t r;
    _V = 0;

    r = a - b;
    if ( test_sub_ovf(a, b) )
    {
        _V = 1;
    }

    _C = b >= a;
    _Z = r == 0;
    _N = r < 0;

    return (a >= b ? a : b);
}

int32_t _min(int32_t a, int32_t b)
{
    return (((a) < (b)) ? (a) : (b));
}

int32_t _min_f(int32_t a, int32_t b)
{
    int32_t r;
    _V = 0;

    r = a - b;
    if ( test_sub_ovf(a, b) ) {
        _V = 1;
    }
    _C = b <= a;
    _Z = r == 0;
    _N = r < 0;

    return (a <= b ? a : b);
}

int32_t _rsub(int32_t src1, int32_t src2)
{
    return src2 - src1;
}

int32_t _rsub_f(int32_t src1, int32_t src2)
{
    int32_t r;
    _V = 0;

    if ( test_sub_ovf(src2, src1) )
        _V = 1;

    r = src2 - src1;

    _C = ((((uint64_t)(uint32_t)src2 - ((uint32_t)src1)) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _bset(int32_t x, int32_t m)
{
    return x | (1 << m);
}

int32_t _bset_f(int32_t x, int32_t m)
{
    int32_t r = x | (1 << m);
    _Z = r == 0;
    _N = r < 0;
    return r;
}

int32_t _bclr(int32_t x, int32_t m)
{
    return x & (0xffffffff^(1<<m));
}

int32_t _bclr_f(int32_t x, int32_t m)
{
    int32_t r = x & (0xffffffff^(1<<m));
    _Z = r == 0;
    _N = r < 0;
    return r;
}

int32_t _bxor(int32_t x, int32_t m)
{
    int32_t r = (x ^ (1 << m));
    return r;
}

int32_t _bxor_f(int32_t x, int32_t m)
{
    int32_t r = (x ^ (1 << m));
    _Z = r == 0;
    _N = r < 0;
    return r;
}

int32_t _bmsk(int32_t x, int32_t m)
{
    int32_t r = x;
    if ((m & 31) != 31) r &= ((1 << ((m & 31) + 1)) - 1);
    return r;
}

int32_t _bmsk_f(int32_t x, int32_t m)
{
    int32_t r = x;
    if ((m & 31) != 31) r &= ((1 << ((m & 31) + 1)) - 1);

    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _add1(int32_t src1, int32_t src2)
{
    return src1 + (src2 << 1);
}

int32_t _add1_f(int32_t src1, int32_t src2)
{
    int32_t r;
    _V = 0;

    r = src2 << 1;
    r = src1 + r;

    if ( test_add_ovf(src1, src2 << 1) ) 
    {
        _V = 1;
    }
    _C = ((((uint64_t)(uint32_t)src1 + ((uint32_t)src2 << 1)) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _add2(int32_t src1, int32_t src2)
{
    return src1 + (src2 << 2);
}

int32_t _add2_f(int32_t src1, int32_t src2)
{
    int32_t r;
    _V = 0;

    r = src2 << 2;
    r = src1 + r;

    if ( test_add_ovf(src1,src2 << 2) )
    {
        _V = 1;
    }

    _C = ((((uint64_t)(uint32_t)src1 + ((uint32_t)src2 << 2)) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _add3(int32_t src1, int32_t src2)
{
    return src1 + (src2 << 3);
}

int32_t _add3_f(int32_t src1, int32_t src2)
{
    int32_t r;
    _V = 0;

    r = src2 << 3;
    r = src1 + r;

    if ( test_add_ovf(src1, src2 << 3) )
    {
        _V = 1;
    }

    _C = ((((uint64_t)(uint32_t)src1 + ((uint32_t)src2 << 3)) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _sub1(int32_t src1, int32_t src2)
{
    return src1 - (src2 << 1);
}

int32_t _sub1_f(int32_t src1, int32_t src2)
{
    int32_t r;
    _V = 0;

    r = src2 << 1;
    r = src1 - r;

    if ( test_sub_ovf(src1, src2 << 1) )
        _V = 1;

    _C = ((((uint64_t)(uint32_t)src1 - ((uint32_t)src2 << 1)) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _sub2(int32_t src1, int32_t src2)
{
    return src1 - (src2 << 2);
}

int32_t _sub2_f(int32_t src1, int32_t src2)
{
    int32_t r;
    _V = 0;

    r = src2 << 2;
    r = src1 - r;

    if ( test_sub_ovf(src1, src2 << 2) )
        _V = 1;

    _C = ((((uint64_t)(uint32_t)src1 - ((uint32_t)src2 << 2)) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;

    return r;
}

int32_t _sub3(int32_t src1, int32_t src2)
{
    return src1 - (src2 << 3);
}

int32_t _sub3_f(int32_t src1, int32_t src2)
{
    int32_t r;
    _V = 0;

    r = src2 << 3;
    r = src1 - r;

    if ( test_sub_ovf(src1, src2 << 3) )
        _V = 1;

    _C = ((((uint64_t)(uint32_t)src1 - ((uint32_t)src2 << 3)) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;

    return r;
}

// no result instructions
void _btst(int32_t x, int32_t m)
{
    int32_t r;
    r = x & (1 << m);
    _Z = r == 0;
    _N = r < 0;
}

void _tst(int32_t a, int32_t b)
{
    int32_t r;
    r = a & b;
    _Z = r == 0;
    _N = r < 0;
}

void _cmp(int32_t a, int32_t b)
{
    int32_t r;
    _V = 0;

    r = a - b;

    if ( test_sub_ovf(a, b) )
        _V = 1;

    _C = ((((uint64_t)(uint32_t)a - ((uint32_t)b)) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;
}

void _rcmp(int32_t a, int32_t b)
{
    int32_t r;
    _V = 0;

    r = b - a;

    if ( test_sub_ovf(b, a) )
        _V = 1;

    _C = ((((uint64_t)(uint32_t)b - ((uint32_t)a)) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;
}

/* Single Operand Instructions */
int32_t  _mov  (int32_t c) 
{
    return (c); 
}

int32_t  _mov_f  (int32_t c)
{
    _Z = c == 0; 
    _N = c < 0; 
    return (c);
}

int32_t  _sexb      (int32_t c) { 
    int32_t r = (int32_t)(c << (32 - 8)) >> (32 - 8);
    return r;
}
int32_t  _sexb_f    (int32_t c) { 
    int32_t r = (c << (32 - 8)) >> (32 - 8);
    _Z = r == 0;
    _N = r < 0;
    return r;
}
int32_t  _sexw      (int32_t c) { 
    int32_t r = (c << (32 - 16)) >> (32 - 16);
    return r;
}
int32_t  _sexw_f    (int32_t c) { 
    int32_t r = (c << (32 - 16)) >> (32 - 16);
    _Z = r == 0;
    _N = r < 0;
    return r;
}

int32_t  _extb      (int32_t c) { 
    return (c & 0xff); 
}
int32_t  _extb_f    (int32_t c) { 
    int32_t r = c & 0xff;
    _Z = r == 0;
    _N = 0;
    return r;
}
int32_t  _extw      (int32_t c) {
    int32_t r = c & 0xffff;
    return r;
}
int32_t  _extw_f    (int32_t c) {
    int32_t r = c & 0xffff;
    _Z = r == 0;
    _N = r < 0;
    return r;
}

int32_t  _not      (int32_t c) { 
    int32_t r = ~c;
    return r;
}
int32_t  _not_f    (int32_t c) { 
    int32_t r = ~c;
    _Z = r == 0;
    _N = r < 0;
    return r;
}
int32_t  _neg      (int32_t c) {
    int32_t r = -c;
    return r;
}
int32_t  _neg_f    (int32_t c) { 
    int32_t r = _sub_f(0,c);
    return r;
}

uint32_t _abs(int32_t x)
{
    if (x < 0) return 0-x;
    else return x;
}

uint32_t _abs_f(int32_t x)
{
    _Z = x == 0;
    _N = x == 0x80000000;
    _C = x < 0;
    _V = _N;

    if (x < 0) return 0-x;
    else return x;
}

void    _flag  (uint32_t c) {
    _Z = (c & (1 << 11)) >> 11;
    _N = (c & (1 << 10)) >> 10;
    _C = (c & (1 << 9)) >> 9;
    _V = (c & (1 << 8)) >> 8;
}

void    _brk   () {;/*warning: no emulation*/}

int32_t  _asl1      (int32_t a)
{ 
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    r = _asl(a,1);
    _flag(status32_tmp);
    return r;
}
int32_t  _asl1_f    (int32_t a)
{ 
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);

    r = _asl_f(a,1);
    _V = 0;
    if ((a ^ r) & 0x80000000)
        _V = 1;

    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
    return r;
}

int32_t  _asr1      (int32_t a) { return _asr(a,1);}
int32_t  _asr1_f    (int32_t a) { return _asr_f(a,1);}
int32_t  _lsr1      (int32_t a) { return _lsr(a,1);}
int32_t  _lsr1_f    (int32_t a) { return _lsr_f(a,1);}
int32_t  _ror1      (int32_t a) { return _ror(a,1);}
int32_t  _ror1_f    (int32_t a) { return _ror_f(a,1);}

int32_t _rlc(int32_t src1)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    r = (src1 << 1) + (_C & 1);
    _flag(status32_tmp);
    return r;
}

int32_t _rlc_f(int32_t src1)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);

    r = (src1 << 1) + (_C & 1);
    _C = (uint32_t)src1 >> 31;
    _Z = (r == 0);
    _N = r < 0;
    _V = ((src1 & 0x80000000) ^ ((src1<<1) & 0x80000000))>>31;

    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
    return r;
}

int32_t  _rrc      (int32_t a) { 
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    r = ((uint32_t)a >> 1) + ((_C & 1) << 31);
    _flag(status32_tmp);
    return r;
}
int32_t  _rrc_f    (int32_t a) { 
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);

    r = ((uint32_t)a >> 1) + ((_C & 1) << 31);
    _C = a & 0x1;
    _Z = (r == 0);
    _N = r < 0;

    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
    return r;
}

// Memory
uint32_t _core_read(uint32_t regno)
{
    uint32_t r;
    switch (regno)
    {
        case REG_ACC1 : r = _get_ACC1(); break;
        case REG_ACC2 : r = _get_ACC2(); break;
        case REG_X0_U0 : r = _x0_u0(); break;
        case REG_X0_U1 : r = _x0_u1(); break;
        case REG_X1_U0 : r = _x1_u0(); break;
        case REG_X1_U1 : r = _x1_u1(); break;
        case REG_X2_U0 : r = _x2_u0(); break;
        case REG_X2_U1 : r = _x2_u1(); break;
        case REG_X3_U0 : r = _x3_u0(); break;
        case REG_X3_U1 : r = _x3_u1(); break;
        case REG_Y0_U0 : r = _y0_u0(); break;
        case REG_Y0_U1 : r = _y0_u1(); break;
        case REG_Y1_U0 : r = _y1_u0(); break;
        case REG_Y1_U1 : r = _y1_u1(); break;
        case REG_Y2_U0 : r = _y2_u0(); break;
        case REG_Y2_U1 : r = _y2_u1(); break;
        case REG_Y3_U0 : r = _y3_u0(); break;
        case REG_Y3_U1 : r = _y3_u1(); break;
        case REG_X0_NU : r = _x0_nu(); break;
        case REG_X1_NU : r = _x1_nu(); break;
        case REG_X2_NU : r = _x2_nu(); break;
        case REG_X3_NU : r = _x3_nu(); break;
        case REG_Y0_NU : r = _y0_nu(); break;
        case REG_Y1_NU : r = _y1_nu(); break;
        case REG_Y2_NU : r = _y2_nu(); break;
        case REG_Y3_NU : r = _y2_nu(); break;
    default:
        ASSERT(0); // register number is not supported
    }
    return r;
}

void _core_write(uint32_t val, uint32_t regno)
{
    switch (regno)
    {
    case REG_ACC1 : _set_ACC1(val); break;
    case REG_ACC2 : _set_ACC2(val); break;
    case REG_X0_U0 : _set_x0_u0(val); break;
    case REG_X0_U1 : _set_x0_u1(val); break;
    case REG_X1_U0 : _set_x1_u0(val); break;
    case REG_X1_U1 : _set_x1_u1(val); break;
    case REG_X2_U0 : _set_x2_u0(val); break;
    case REG_X2_U1 : _set_x2_u1(val); break;
    case REG_X3_U0 : _set_x3_u0(val); break;
    case REG_X3_U1 : _set_x3_u1(val); break;
    case REG_Y0_U0 : _set_y0_u0(val); break;
    case REG_Y0_U1 : _set_y0_u1(val); break;
    case REG_Y1_U0 : _set_y1_u0(val); break;
    case REG_Y1_U1 : _set_y1_u1(val); break;
    case REG_Y2_U0 : _set_y2_u0(val); break;
    case REG_Y2_U1 : _set_y2_u1(val); break;
    case REG_Y3_U0 : _set_y3_u0(val); break;
    case REG_Y3_U1 : _set_y3_u1(val); break;
    case REG_X0_NU : _set_x0_nu(val); break;
    case REG_X1_NU : _set_x1_nu(val); break;
    case REG_X2_NU : _set_x2_nu(val); break;
    case REG_X3_NU : _set_x3_nu(val); break;
    case REG_Y0_NU : _set_y0_nu(val); break;
    case REG_Y1_NU : _set_y1_nu(val); break;
    case REG_Y2_NU : _set_y2_nu(val); break;
    case REG_Y3_NU : _set_y2_nu(val); break;
    default:
        ASSERT(0); // register number is not supported
    }
}

uint32_t _lr(uint32_t regno)
{
    uint32_t r;
    switch (regno)
    {
    case STATUS32     : r = _get_STATUS32(); break;
    case AUX_XMAC0_24 : r = _get_AUX_XMAC0_24(); break;
    case AUX_XMAC1_24 : r = _get_AUX_XMAC1_24(); break;
    case AUX_XMAC2_24 : r = _get_AUX_XMAC2_24(); break;
    case AUX_CRC_POLY : r = _get_AUX_CRC_POLY(); break;
    case AUX_CRC_MODE : r = _get_AUX_CRC_MODE(); break;
    case AUX_MACMODE  : r = _get_MACMODE(); break;
    case AUX_XMAC0    : r = _get_AUX_XMAC0(); break;
    case AUX_XMAC1    : r = _get_AUX_XMAC1(); break;
    case AUX_XMAC2    : r = _get_AUX_XMAC2(); break;
    case AUX_FBF_STORE_16 : r = _get_AUX_FBF_STORE_16(); break;
    case XYLSBASEX : r = _get_xbase(); break;
    case XYLSBASEY : r = _get_ybase(); break;
    case AUX_XMACLW_H : r = _get_AUX_XMACLW_H(); break;
    case AUX_XMACLW_L : r = _get_AUX_XMACLW_L(); break;
    case DSPRAM_BUILD : r = (DSPRAM_MOD << 18) | (DSPRAM_EP << 17) | (DSPRAM_RA << 16) | (DSPRAM_NBANKS << 12) | (DSPRAM_MEMSIZE << 8) | (DSPRAM_VERSION); break;
    case AX0 : r = _ax0(); break;
    case AX1 : r = _ax1(); break;
    case AX2 : r = _ax2(); break;
    case AX3 : r = _ax3(); break;
    case AY0 : r = _ay0(); break;
    case AY1 : r = _ay1(); break;
    case AY2 : r = _ay2(); break;
    case AY3 : r = _ay3(); break;
    case MX00 : r = _mx00(); break;
    case MX01 : r = _mx01(); break;
    case MX10 : r = _mx10(); break;
    case MX11 : r = _mx11(); break;
    case MX20 : r = _mx20(); break;
    case MX21 : r = _mx21(); break;
    case MX30 : r = _mx30(); break;
    case MX31 : r = _mx31(); break;
    case MY00 : r = _my00(); break;
    case MY01 : r = _my01(); break;
    case MY10 : r = _my10(); break;
    case MY11 : r = _my11(); break;
    case MY20 : r = _my20(); break;
    case MY21 : r = _my21(); break;
    case MY30 : r = _my30(); break;
    case MY31 : r = _my31(); break;
    default:
        ASSERT(0); // register number is not supported
    }
    return r;
}

void _sr(uint32_t val, uint32_t regno)
{
    switch (regno)
    {
    //case STATUS32     : READ-ONLY register
    case AUX_XMAC0_24 : _set_AUX_XMAC0_24(val); break;
    case AUX_XMAC1_24 : _set_AUX_XMAC1_24(val); break;
    case AUX_XMAC2_24 : _set_AUX_XMAC2_24(val); break;
    case AUX_CRC_POLY : _set_AUX_CRC_POLY(val); break;
    case AUX_CRC_MODE : _set_AUX_CRC_MODE(val); break;
    case AUX_MACMODE  : _set_MACMODE(val); break;
    case AUX_XMAC0    : _set_AUX_XMAC0(val); break;
    case AUX_XMAC1    : _set_AUX_XMAC1(val); break;
    case AUX_XMAC2    : _set_AUX_XMAC2(val); break;
    case AUX_FBF_STORE_16 : _set_AUX_FBF_STORE_16(val); break;
    case XYLSBASEX : _set_xbase(val); break;
    case XYLSBASEY : _set_ybase(val); break;
    case AUX_XMACLW_H : _set_AUX_XMACLW_H(val); break;
    case AUX_XMACLW_L : _set_AUX_XMACLW_L(val); break;
    //case DSPRAM_BUILD : READ-ONLY register
    case AX0 : _set_ax0(val); break;
    case AX1 : _set_ax1(val); break;
    case AX2 : _set_ax2(val); break;
    case AX3 : _set_ax3(val); break;
    case AY0 : _set_ay0(val); break;
    case AY1 : _set_ay1(val); break;
    case AY2 : _set_ay2(val); break;
    case AY3 : _set_ay3(val); break;
    case MX00 : _set_mx00(val); break;
    case MX01 : _set_mx01(val); break;
    case MX10 : _set_mx10(val); break;
    case MX11 : _set_mx11(val); break;
    case MX20 : _set_mx20(val); break;
    case MX21 : _set_mx21(val); break;
    case MX30 : _set_mx30(val); break;
    case MX31 : _set_mx31(val); break;
    case MY00 : _set_my00(val); break;
    case MY01 : _set_my01(val); break;
    case MY10 : _set_my10(val); break;
    case MY11 : _set_my11(val); break;
    case MY20 : _set_my20(val); break;
    case MY21 : _set_my21(val); break;
    case MY30 : _set_my30(val); break;
    case MY31 : _set_my31(val); break;
    default:
        ASSERT(0); // register number is not supported
    }
}

#endif // !_ARC

#ifdef EMU__Xbarrel_shifter

int32_t _asl(int32_t x, int32_t s)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    r = x << (s & 0x1F);
    _flag(status32_tmp);
    return r;
}

int32_t _asl_f(int32_t x, int32_t s)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);

    r = x << (s & 0x1F);
    _C = (( ((uint64_t)(uint32_t)x << (s & 0x1F)) & ((uint64_t)1 << 32)) ? 1 : 0);
    _Z = r == 0;
    _N = r < 0;

    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
    return r;
}

int32_t _asr(int32_t x, int32_t s)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    r = x >> (s & 0x1F);
    _flag(status32_tmp);
    return r;
}

int32_t _asr_f(int32_t x, int32_t s)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);

    s &= 0x1f;
    r = x >> (s & 0x1F);
    _C = (s == 0 ? 0 : (((uint32_t)x & ((uint32_t)1 << (s - 1))) ? 1 : 0));
    _Z = r == 0;
    _N = r < 0;

    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
    return r;
}

int32_t _lsr(int32_t src1, int32_t s)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    r = ((uint32_t)src1 >> s);
    _flag(status32_tmp);
    return r;
}

int32_t _lsr_f(int32_t src1, int32_t s)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);

    s &= 0x1f;
    r = ((uint32_t)src1 >> s);
    _C = (s == 0 ? 0 : (((uint32_t)src1 & ((uint32_t)1 << (s - 1))) ? 1 : 0));
    _Z = r == 0;
    _N = r < 0;

    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
    return r;
}

int32_t _ror(int32_t src1, int32_t s)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    r = (((uint32_t)src1 >> s)|((uint32_t)src1 << (32-s)));
    _flag(status32_tmp);
    return r;
}

int32_t _ror_f(int32_t src1, int32_t s)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);

    s &= 0x1f;
    r = ((uint32_t)src1 >> s)|((uint32_t)src1 << (32-s));
    _C = (s == 0 ? 0 : (((uint32_t)src1 & ((uint32_t)1 << (s - 1))) ? 1 : 0));
    _Z = r == 0;
    _N = r < 0;

    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
    return (r);
}

#endif // EMU__Xbarrel_shifter


#ifdef EMU__Xnorm
int32_t _norm(int32_t x)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    if (x == 0|| x==-1)
        r = 0x1F;
    else
    {
        r = 0;

        while (!((x>0)? (x & 0x40000000) : (~x & 0x40000000)))
        {
            r++;
            x <<= 1;
        }
    }
    _flag(status32_tmp);
    return r;
}
int32_t _norm_f(int32_t x)
{
    signed r;
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _Z = x == 0;
    _N = x < 0;

    if (x == 0 || x==-1)
        r = 0x1F;
    else
    {
        r = 0;
        while (!((x>0) ? (x & 0x40000000) : (~x & 0x40000000)))
        {
            r++;
            x <<= 1;
        }
    }
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
    return r;
}
int32_t _normw(int32_t x)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    
    if ((int16_t)x == 0 || (int16_t)x == -1)
        r = 0x0F;
    else
    {
        r = 0;

        while (!(((int16_t)x>0)? ((int16_t)x & 0x4000) : (~(int16_t)x & 0x4000)))
        {
            r++;
            x <<= 1;
        }
    }
    _flag(status32_tmp);
    return r;
}
int32_t _normw_f(int32_t x)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _Z = (int16_t)x == 0;
    _N = (int16_t)x < 0;

    if ((int16_t)x == 0 || (int16_t)x == -1)
        r = 0x0F;
    else
    {
        r = 0;
        while (!(((int16_t)x>0)? ((int16_t)x & 0x4000) : (~(int16_t)x & 0x4000)))
        {
            r++;
            x <<= 1;
        }
    }
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
    return r;
}
#endif // EMU__Xnorm

#ifdef EMU__Xswap
int32_t _swap(int32_t x)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    r = (((x >> 16) & 0x0000FFFF) | ((x << 16) & 0xFFFF0000));
    _flag(status32_tmp);
    return r;
}
int32_t _swap_f(int32_t x)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    r = (((x >> 16) & 0x0000FFFF) | ((x << 16) & 0xFFFF0000));
    _Z = r == 0;
    _N = r < 0;
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
    return r;
}
#endif // EMU__Xswap

#ifdef EMU__Xmpy16
uint32_t     _mpyuw(uint16_t b, uint16_t c)
{
    uint32_t r;
    int32_t status32_tmp = _get_STATUS32();
    r = b * c;
    _flag(status32_tmp);
    return r;
}
uint32_t     _mpyuw_f(uint16_t b, uint16_t c)
{
    uint32_t r;
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    r = b * c;
    _Z = r == 0;
    _N = 0;
    _V = 0;
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
    return r;
}
int32_t     _mpyw(int16_t b, int16_t c)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    r = b * c;
    _flag(status32_tmp);
    return r;
}
int32_t     _mpyw_f(int16_t b, int16_t c)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    r = b * c;
    _Z = r == 0;
    _N = r < 0;
    _V = 0;
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
    return r;
}
#endif

#ifdef EMU__Xea
/*
    Extended Arithmetic Library
*/

int32_t _adds(int32_t x, int32_t y)
{
    int32_t a = x + y;
    int32_t status32_tmp = _get_STATUS32();

    if ( test_add_ovf(x,y) ) { 
        TRAP("sat32");
        if (a < 0)
            a = 0x7FFFFFFF;
        else
            a = 0x80000000;
    }

    _flag(status32_tmp);
    return a;
}

int32_t _adds_f(int32_t x, int32_t y)
{
    int32_t a;
#ifdef _ARC
    int32_t status32_tmp;
    status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif

    _V = 0;
    if (!((x ^ y)>>31))
    {
        if (x >= 0)
        {
            _C = ((((uint64_t)(uint32_t)x + (uint64_t)(uint32_t)y) & ((uint64_t)1 << 32)) ? 1 : _C);
        }
    }
    a = x + y;

    if ( test_add_ovf(x,y) )
    { 
        TRAP("sat32");
        _V = 1;
        if (a < 0)
            a = 0x7FFFFFFF;
        else
            a = 0x80000000;
    }

    _Z = a == 0;
    _N = a < 0;
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return a;
}

int32_t _subs(int32_t x, int32_t y)
{
    int32_t a = x - y;
    int32_t status32_tmp = _get_STATUS32();

    if ( test_sub_ovf(x,y) ) 
    { 
        TRAP("sat32");
        if (a < 0)
            a = 0x7FFFFFFF;
        else
            a = 0x80000000;
    }

    _flag(status32_tmp);
    return a;
}

int32_t _subs_f(int32_t x, int32_t y)
{
    int32_t a;
#ifdef _ARC
    int32_t status32_tmp;
    status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    _V = 0;

    a = x - y;

    if ( test_sub_ovf(x,y) ) 
    {
        TRAP("sat32");
        _V = 1;
        if (a < 0)
            a = 0x7FFFFFFF;
        else
            a = 0x80000000;
    }

    if (!((x ^ (-y))>>31))
    {
        if (x < 0)
        {
            _C = ((((uint64_t)(uint32_t)x - ((uint32_t)y)) & ((uint64_t)1 << 32)) ? 1 : _C);
        }
    }

    _Z = a == 0;
    _N = a < 0;
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return a;
}

int32_t _divaw(int32_t src1, int32_t src2)
{
    uint32_t src1_temp, dest;
    int32_t status32_tmp = _get_STATUS32();

    ASSERT(src1 >= 0);
    ASSERT(src2 > 0);
    ASSERT(src2 >= (src1 & (int32_t)0xffff0000));
    ASSERT((src2 & 0xffff) == 0);
    if (src1 == 0)
        dest = 0;
    else
    {
        src1_temp = src1 << 1;
        if ((src1_temp >= (uint32_t)src2) && (src2)) 
            dest = ((src1_temp - (src2)) | 0x00000001);
        else
            dest = src1_temp;
    }
    _flag(status32_tmp);
    return dest;
}

int32_t _asls(int32_t a, int32_t n)
{
    int64_t s;
    int32_t status32_tmp = _get_STATUS32();
    s = a;
    if (n >= 0)
    {
        if (n <= 31)
        {
            s <<= n;

            if (s > (((int64_t)1 << 31) - 1))
            {
                TRAP("sat32");
                s = (((int64_t)1 << 31) - 1);
            }
            if (s < ((int64_t)-1 << 31))
            {
                TRAP("sat32");
                s = ((int64_t)-1 << 31);
            }
        }
        else
        {
            if (s > 0)
                s = 0x7FFFFFFF;
            else
                if (s < 0)
                    s = 0x80000000;
        }
    }
    else
    {
        n = _min(-n, 31);
        if (n == 0x80000000) n = 31;

        s >>= n;
    }
    _flag(status32_tmp);
    return (int32_t)s;
}

int32_t _asls_f(int32_t a, int32_t n)
{
    int64_t s = a;
    int32_t r;
#ifdef _ARC
    int32_t status32_tmp;
    status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    _V = 0;

    if (n >= 0)
    {
        if (n <= 31)
        {
            s <<= n;

            if (s > (((int64_t)1 << 31) - 1))
            {
                TRAP("sat32");
                _V = 1;
                s = (((int64_t)1 << 31) - 1);
            }
            if (s < ((int64_t)-1 << 31))
            {
                TRAP("sat32");
                _V = 1;
                s = ((int64_t)-1 << 31);
            }
        }
        else
        {
            if (s > 0) {
                _V = 1;
                s = 0x7FFFFFFF;
            } 
            if (s < 0) {
                _V = 1;
                s = 0x80000000;
            }
        }
    }
    else
    {
        n = _min(-n, 31);
        if (n == 0x80000000) n = 31;
        s >>= n;
    }

    r = (int32_t)s;

    _Z = r == 0;
    _N = r < 0;
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

int32_t _asrs(int32_t a, int32_t s)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();

    if (s == 0x80000000)
    {
        s = s + 1;
    }
    r = _asls(a, -s);

    _flag(status32_tmp);
    return r;
}

int32_t _asrs_f(int32_t a, int32_t n)
{
    int64_t s = a;
    int32_t r;
#ifdef _ARC
    int32_t status32_tmp;
    status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif

    if (n == 0x80000000)
    {
        n = n + 1;
    }

    n = -n;

    _V = 0;

    if (n >= 0)
    {
        if (n <= 31)
        {
            s <<= n;

            if (s > (((int64_t)1 << 31) - 1))
            {
                TRAP("sat32");
                _V = 1;
                s = (((int64_t)1 << 31) - 1);
            }
            if (s < ((int64_t)-1 << 31))
            {
                TRAP("sat32");
                _V = 1;
                s = ((int64_t)-1 << 31);
            }
        }
        else
        {
            if (s > 0) {
                _V = 1;
                s = 0x7FFFFFFF;
            } 
            if (s < 0) {
                _V = 1;
                s = 0x80000000;
            }
        }
    }
    else
    {
        n = _min(-n, 31);
        if (n == 0x80000000) n = 31;
        s >>= n;
    }

    r = (int32_t)s;

    _Z = r == 0;
    _N = r < 0;
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

int32_t _subsdw(int32_t a, int32_t b)
{
    int32_t a_high, a_low, b_high, b_low;
    uint32_t res;
    int32_t status32_tmp = _get_STATUS32();

    a_high = (int16_t)_asr(a, 16);
    a_low  = (int16_t)_bmsk(a, 15);
    b_high = (int16_t)_asr(b, 16);
    b_low  = (int16_t)_bmsk(b, 15);

    res = (saturate16(a_high - b_high) << 16) | (saturate16(a_low - b_low) & 0x0000FFFF);

    _flag(status32_tmp);
    return res;
}

int32_t _addsdw(int32_t a, int32_t b)
{
    int32_t a_high, a_low, b_high, b_low;
    uint32_t res;
    int32_t status32_tmp = _get_STATUS32();

    a_high = (int16_t)_asr(a, 16);
    a_low  = (int16_t)_bmsk(a, 15);
    b_high = (int16_t)_asr(b, 16);
    b_low  = (int16_t)_bmsk(b, 15);

    res = (saturate16(a_high + b_high) << 16) | (saturate16(a_low + b_low) & 0x0000FFFF);

    _flag(status32_tmp);
    return res;
}

// Extended Arithmetic Single Operand Instructions
int16_t _sat16(int32_t a)
{
    int32_t status32_tmp = _get_STATUS32();

    if (a < -32768)
        a = -32768;

    if (a > 32767)
        a = 32767;

    _flag(status32_tmp);
    return (int16_t)a;
}

int16_t _sat16_f(int32_t a)
{
#ifdef _ARC
    int32_t status32_tmp;
    status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    if (a < -32768)
    {
        a = -32768;
        _V = 1;
    }
    else
        if (a > 32767)
        {
            a = 32767;
            _V = 1;
        }
        else
            _V = 0;

    _Z = a == 0;
    _N = a < 0;
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return (int16_t)a;
}

int16_t _rnd16(int32_t a)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();

    if (a >= 0x7fff8000 && a <= 0x7fffffff)
        r = 0x7FFF;
    else 
        r = ((a + 0x8000) >> 16) & 0xFFFF;

    _flag(status32_tmp);
    return (int16_t)(r);
}

int16_t _rnd16_f(int32_t a)
{
    int32_t r;
#ifdef _ARC
    int32_t status32_tmp;
    status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    if (a >= 0x7fff8000 && a <= 0x7fffffff)
    {
        r = 0x7FFF;
        _V = 1;
    }
    else
    {
        r = (int16_t)(((a+0x8000) >> 16) & 0xFFFF);
        _V = 0;
    }

    _Z = r == 0;
    _N = r < 0;
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return (int16_t)r;
}

int16_t _abssw(int32_t x)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();
    x = (int16_t)x;
    if ((uint32_t)x <= 0x00007FFF)
        r = x;
    else {
        r = -x;
        if ((uint32_t)x == 0xFFFF8000)
        {
            TRAP("sat16");
            r = 0x00007FFF;
        } 
    }
    _flag(status32_tmp);
    return (int16_t)r;
}

int16_t _abssw_f(int32_t x)
{
    int32_t r;
#ifdef _ARC
    int32_t status32_tmp;
    status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    x = (int16_t)x;
    _Z = x == 0;
    _N = x < 0;
    _V = 0;

    if ((uint32_t)x <= 0x00007FFF)
        r = x;
    else {
        r = -x;
        if ((uint32_t)x == 0xFFFF8000)
        {
            TRAP("sat16");
            _V = 1;
            r = 0x00007FFF;
        }
    }
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return (int16_t)r;
}

int32_t _abss(int32_t x)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();

    if ((uint32_t)x <= 0x7FFFFFFF)
        r = x;
    else {
        r = -x;
        if ((uint32_t)x == 0x80000000)
        {
            TRAP("sat32");
            r = 0x7FFFFFFF;
        } 
    }

    _flag(status32_tmp);
    return r;
}

int32_t _abss_f(int32_t x)
{
    int32_t r;
#ifdef _ARC
    int32_t status32_tmp;
    status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    _Z = x == 0;
    _N = x < 0;
    _V = 0;

    if ((uint32_t)x <= 0x7FFFFFFF)
        r = x;
    else {
        r = -x;
        if ((uint32_t)x == 0x80000000)
        {
            TRAP("sat32");
            _V = 1;
            r = 0x7FFFFFFF;
        }
    }
       
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

int16_t _negsw(int32_t b)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();

    if ((b & 0xffff) == 0x8000)
    {
        TRAP("sat32");
        r = 0x7fff;
    }
    else
        r = -b;

    _flag(status32_tmp);
    return (int16_t)r;
}

int16_t _negsw_f(int32_t b)
{
    int32_t r;
#ifdef _ARC
    int32_t status32_tmp;
    status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    b &= 0xffff;
    if (b == 0x8000)
    {
        TRAP("sat32");
        r = 0x7fff;
        _V = 1;
    }
    else
    {
        r = -(int16_t)b;
        _V = 0;
    }

    _Z = r == 0;
    _N = r < 0;
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return (int16_t)r;
}

int32_t _negs(int32_t a)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32();

    if (a == 0x80000000)
    {
        TRAP("sat32");
        r = 0x7fffffff;
    }
    else
        r = -a;

    _flag(status32_tmp);
    return r;
}

int32_t _negs_f(int32_t a)
{
    int32_t r;
#ifdef _ARC
    int32_t status32_tmp;
    status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    if (a == 0x80000000)
    {
        TRAP("sat32");
        r = 0x7fffffff;
        _V = 1;
    }
    else
    {
        r = -a;
        _V = 0;
    }

    _Z = r == 0;
    _N = r < 0;
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

#endif // EMU__Xea

/*
    ARC600 DSP Options
*/

/* 
    Multiply-Accumulate with 32-bit X 16-Bit Data 
*/

#ifdef EMU__Xmul32x16

int32_t _mullw(int32_t src1, int32_t src2)
{
    int32_t r, ovf;
    int32_t status32_tmp = _get_STATUS32();

    AA_32X16 = (int64_t)src1 * (src2 & 0xFFFF);

    _set_ACC1((int32_t)(AA_32X16>>32));
    _set_ACC2((uint32_t)(AA_32X16));

    SAT_VF(r, AA_32X16, 32, ovf, int32_t);

    _flag(status32_tmp);
    return r;
}

int32_t _mullw_f(int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp;
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1;
#ifdef _ARC
    int32_t status32_tmp;
    status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    macmode_tmp = _get_MACMODE();
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _V = 0;

    AA_32X16 = (int64_t)src1 * (src2 & 0xFFFF);

    _set_ACC1((int32_t)(AA_32X16>>32));
    _set_ACC2((uint32_t)(AA_32X16));

    SAT_VF(r, AA_32X16, 32, _V, int32_t);

    _Z = r == 0;
    _N = r < 0;

    Sch1 = Sch1 | _V;
    ANch1 = AA_32X16 < 0;
    AZch1 = AA_32X16 == 0;

    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

int32_t _maclw(int32_t src1, int32_t src2)
{
    int32_t r, ovf;
    int32_t status32_tmp = _get_STATUS32();

    AA_32X16 = sat_add64_ovf(AA_32X16, (int64_t)src1 * (src2 & 0xFFFF), &ovf);

    _set_ACC1((int32_t)(AA_32X16>>32));
    _set_ACC2((uint32_t)(AA_32X16));

    SAT(r, AA_32X16, 32, int32_t);

    _flag(status32_tmp);

    return r;
}

int32_t _maclw_f(int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp;
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    macmode_tmp = _get_MACMODE();
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _V = 0;

    AA_32X16 = sat_add64_ovf(AA_32X16, (int64_t)src1 * (src2 & 0xFFFF), &_V);

    _set_ACC1((int32_t)(AA_32X16>>32));
    _set_ACC2((uint32_t)(AA_32X16));

    _Z = ((uint32_t)(AA_32X16)) == 0;
    _N = ((int32_t)(AA_32X16>>32)) < 0;

    if (AA_32X16 > (((int64_t)1 << 31) - 1))
    {
        _V = 1;
    }
    if (AA_32X16 < ((int64_t)-1 << 31))
    {
        _V = 1;
    }

    r = (int32_t)saturate32(AA_32X16);

    Sch1 = Sch1 | _V;
    ANch1 = AA_32X16 < 0;
    AZch1 = AA_32X16 == 0;

    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

int32_t _mulflw(int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32(), macmode_tmp = _get_MACMODE();

    AA_32X16 = 0;
    r = _macflw(src1, src2);
    
     _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t _mulflw_f(int32_t src1, int32_t src2)
{
    int32_t r;
    
    AA_32X16 = 0;
    r = _macflw_f(src1, src2);
    
    return r;
}

int32_t _macflw(int32_t src1, int32_t src2)
{
    int32_t r, ovf;
    int32_t status32_tmp = _get_STATUS32();

    AA_32X16 = sat_add64_ovf(AA_32X16, ((int64_t)src1 * (src2 & 0xFFFF)) << 1, &ovf);

    /* ACC <-- AA */
    _set_ACC1((int32_t)(AA_32X16>>32));
    _set_ACC2((uint32_t)(AA_32X16));

    r = (int32_t)saturate32(AA_32X16);

    _flag(status32_tmp);

    return r;
}

int32_t _macflw_f(int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp;
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    macmode_tmp = _get_MACMODE();
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _V = 0;

    AA_32X16 = sat_add64_ovf(AA_32X16, ((int64_t)src1 * (src2 & 0xFFFF)) << 1, &_V);

    _set_ACC1((int32_t)(AA_32X16>>32));
    _set_ACC2((uint32_t)(AA_32X16));

    r = (int32_t)saturate32(AA_32X16);
    
    if (AA_32X16 > (((int64_t)1 << 31) - 1))
    {
        _V = 1;
    }
    if (AA_32X16 < ((int64_t)-1 << 31))
    {
        _V = 1;
    }

    _Z = r == 0;
    _N = (r & 0x80000000) != 0;

    Sch1 = Sch1 | _V;
    ANch1 = AA_32X16 < 0;
    AZch1 = AA_32X16 == 0;

    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

uint32_t _mululw(uint32_t src1, uint32_t src2)
{
    uint32_t r;
    int32_t status32_tmp = _get_STATUS32();
    AA_32X16 = (uint64_t)src1 * (src2 & 0xFFFF);

    _set_ACC1((uint32_t)((uint64_t)AA_32X16>>32));
    _set_ACC2((uint32_t)(AA_32X16));

    SATU(r, AA_32X16, 32, uint32_t);

    _flag(status32_tmp);
    return r;
}

uint32_t _mululw_f(uint32_t src1, uint32_t src2)
{
    uint32_t r;
    int32_t macmode_tmp;
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    macmode_tmp = _get_MACMODE();
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _V = 0;
    AA_32X16 = (uint64_t)src1 * (src2 & 0xFFFF);

    _set_ACC1((uint32_t)((uint64_t)AA_32X16>>32));
    _set_ACC2((uint32_t)(AA_32X16));

    SATU_VF(r, AA_32X16, 32, _V, uint32_t);

    _Z = r == 0;
    _N = (r & 0x80000000) != 0;

    Sch1 = Sch1 | _V;
    ANch1 = AA_32X16 < 0;
    AZch1 = AA_32X16 == 0;

    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

uint32_t _machulw(uint32_t src1, uint32_t src2)
{
    uint32_t r;
    int32_t status32_tmp = _get_STATUS32(), macmode_tmp = _get_MACMODE();

    r = _machulw_f( src1, src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

uint32_t _machulw_f(uint32_t src1, uint32_t src2)
{
    int32_t r;
    int32_t macmode_tmp;
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    macmode_tmp = _get_MACMODE();
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _V = 0;

    AA_32X16 = sat_addu64_ovf(AA_32X16, ((int64_t)src1 * (src2 >> 16)) << 16, &_V);

    _set_ACC1((int32_t)(AA_32X16>>32));
    _set_ACC2((uint32_t)(AA_32X16));

    r = (int32_t)(AA_32X16 >> 32);

    _Z = r == 0;
    _N = r < 0;

    Sch1 = Sch1 | _V;
    ASch1 = ASch1 | _V;
    ANch1 = AA_32X16 < 0;
    AZch1 = AA_32X16 == 0;

    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

int32_t _mulhlw(int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32(), macmode_tmp = _get_MACMODE();

    AA_32X16 = 0;
    r = _machlw_f( src1,  src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t _mulhlw_f(int32_t src1, int32_t src2)
{
    int32_t r;

    AA_32X16 = 0;
    r = _machlw_f(src1, src2);

    return r;
}

int32_t _machlw(int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32(), macmode_tmp = _get_MACMODE();

    r = _machlw_f(src1, src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t _machlw_f(int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp;
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    macmode_tmp = _get_MACMODE();
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _V = 0;
    AA_32X16 = sat_add64_ovf(AA_32X16, ((int64_t)src1 * (src2 >> 16)) << 16, &_V);

    _set_ACC1((int32_t)(AA_32X16>>32));
    _set_ACC2((uint32_t)(AA_32X16));

    r = (int32_t)(AA_32X16 >> 32);

    _Z = r == 0;
    _N = r < 0;

    Sch1 = Sch1 | _V;
    ASch1 = ASch1 | _V;
    ANch1 = AA_32X16 < 0;
    AZch1 = AA_32X16 == 0;

    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

int32_t _mulhflw(int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32(), macmode_tmp = _get_MACMODE();

    AA_32X16 = 0;
    r = _machflw_f( src1,  src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t _mulhflw_f(int32_t src1, int32_t src2)
{
    int32_t r;

    AA_32X16 = 0;
    r = _machflw_f( src1,  src2);

    return r;
}

int32_t _machflw(int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t status32_tmp = _get_STATUS32(), macmode_tmp = _get_MACMODE();

    r = _machflw_f( src1,  src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t _machflw_f(int32_t src1, int32_t src2)
{
    int32_t r;
    int64_t Mult_product;
    int32_t macmode_tmp;
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1;
#ifdef _ARC
    int32_t status32_tmp;
    status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    macmode_tmp = _get_MACMODE();
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _V = 0;

    Mult_product = ((int64_t)src1 * (src2 >> 16)) << 16;

    //Mult_product <<= 1;
    AA_32X16 =  sat_add64_ovf(AA_32X16, Mult_product, &_V);
    AA_32X16 =  sat_add64_ovf(AA_32X16, Mult_product, &_V);

    _set_ACC1((int32_t)(AA_32X16>>32));
    _set_ACC2((int32_t)(AA_32X16));
    r = (int32_t)(AA_32X16 >> 32);

    _Z = r == 0;
    _N = r < 0;

    Sch1 = Sch1 | _V;
    ASch1 = ASch1 | _V;
    ANch1 = AA_32X16 < 0;
    AZch1 = AA_32X16 == 0;

    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch1, AZch1, ANch1, PSch1, Sch1);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

#endif // EMU__Xmul32x16

#ifdef EMU__Xxmac_d16
/* Multiply-Accumulate with Dual-Word Data */

#define swHI(x)     ((int16_t)((int32_t)x >> 16))
#define uwHI(x)     ((uint16_t)((uint32_t)x >> 16))

#define swLO(x)     ((int16_t)((int32_t)x & 0xFFFF))
#define uwLO(x)     ((uint16_t)((uint32_t)x & 0xFFFF))

int32_t     _muldw     (int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t status32_tmp = _get_STATUS32();

    r = _muldw_f(src1,src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t     _muldw_f   (int32_t src1, int32_t src2) 
{ 
    int32_t r;
    AA1 = 0;
    AA2 = 0;
    r = _macdw_f(src1, src2);
    return r;
}

uint32_t    _muludw    (uint32_t src1, uint32_t src2) 
{
    uint32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t status32_tmp = _get_STATUS32();
   
    r = _muludw_f(src1,src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

uint32_t     _muludw_f  (uint32_t src1, uint32_t src2)
{ 
    uint32_t r;
    AA1 = 0;
    AA2 = 0;
    r = _macudw_f(src1, src2);
    return r;
}

int32_t     _mulrdw    (int32_t src1, int32_t src2)
{
    int32_t r;
    AA1 = 0;
    AA2 = 0;
    r = _macrdw(src1,src2);
    return r;
}

int32_t     _mulrdw_f  (int32_t src1, int32_t src2)
{ 
    int32_t r;
    AA1 = 0;
    AA2 = 0;
    r = _macrdw_f(src1, src2);
    return r;
}

int32_t     _macdw     (int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t status32_tmp = _get_STATUS32();

    r = _macdw_f(src1,src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t     _macdw_f   (int32_t src1, int32_t src2)
{ 
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    {
        int64_t Mult_L_res, Mult_H_res;
        int32_t acc1_tmp, acc2_tmp;
        int32_t SF1 = 0, SF2 = 0;

        _V = 0;

        Mult_L_res = (int64_t)(swLO(src1) * swLO(src2));
        Mult_H_res = (int64_t)(swHI(src1) * swHI(src2));

        if (macmode_tmp & 0x4) { // fractional
            Mult_L_res <<= 1;
            Mult_H_res <<= 1;
        }

        if (macmode_tmp & (1<<14)) { // guard-bit
            int32_t false_sat1, false_sat2;
            false_sat1 = Mult_H_res == 0x80000000;
            false_sat2 = Mult_L_res == 0x80000000;
            SAT_VF(Mult_H_res,Mult_H_res,32,SF1,int64_t);
            SAT_VF(Mult_L_res,Mult_L_res,32,SF2,int64_t);

            ASch1 |= (SF1 & !false_sat1);
            ASch2 |= (SF2 & !false_sat2);
        }

        AA2 += Mult_L_res;
        AA1 += Mult_H_res;

        if (macmode_tmp & (1<<14)) { // guard-bit
            int32_t false_sat1, false_sat2;
            false_sat1 = AA1 == 0x7fffffff;
            false_sat2 = AA2 == 0x7fffffff;
            SAT_VF(AA2, AA2, 32, SF2, int64_t);
            SAT_VF(AA1, AA1, 32, SF1, int64_t);
            ASch1 |= (SF1 & !false_sat1);
            ASch2 |= (SF2 & !false_sat2);
        } else {
            SAT_VF(AA2, AA2, 40, SF2, int64_t);
            SAT_VF(AA1, AA1, 40, SF1, int64_t);
            ASch1 |= SF1;
            ASch2 |= SF2;
        }
        
        SAT_VF(acc1_tmp,AA1,32,SF1,int32_t);
        SAT_VF(acc2_tmp,AA2,32,SF2,int32_t);

        _set_ACC1(acc1_tmp);
        _set_ACC2(acc2_tmp);

        if (macmode_tmp & 0x8) { // return MSP
            r = (acc1_tmp & 0xffff0000) | ((acc2_tmp>>16) & 0x0000ffff);
        }
        else {
            SAT_VF(acc1_tmp,acc1_tmp,16,SF1,int32_t);
            SAT_VF(acc2_tmp,acc2_tmp,16,SF2,int32_t);
            r = (acc1_tmp << 16) | (acc2_tmp & 0xffff);
        }
    
        _V = SF1 | SF2;
        _Z = r == 0;
        _N = (r & 0x80008000) != 0;

        AZch1 = acc1_tmp == 0;
        ANch1 = acc1_tmp < 0;
        PSch1 = SF1;
        Sch1 = Sch1 | SF1;

        AZch2 = acc2_tmp == 0;
        ANch2 = acc2_tmp < 0;
        PSch2 = SF2;
        Sch2 = Sch2 | SF2;
    }
    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

uint32_t    _macudw    (uint32_t src1, uint32_t src2)
{
    uint32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t status32_tmp = _get_STATUS32();

    r = _macudw_f(src1,src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

uint32_t    _macudw_f  (uint32_t src1, uint32_t src2) 
{ 
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    {
        uint64_t Mult_L_res, Mult_H_res;
        uint32_t acc1_tmp, acc2_tmp;
        int32_t SF1 = 0, SF2 = 0;
        _V = 0;
        Mult_L_res = (uint64_t)uwLO(src1) * uwLO(src2);
        Mult_H_res = (uint64_t)uwHI(src1) * uwHI(src2);

        if (macmode_tmp & 0x4) { // fractional
            Mult_L_res = Mult_L_res << 1;
            Mult_H_res = Mult_H_res << 1;
        } 

        if (macmode_tmp & (1<<14)) { // guard-bit
            SATU_VF(Mult_H_res,Mult_H_res,32,SF1,uint64_t);
            SATU_VF(Mult_L_res,Mult_L_res,32,SF2,uint64_t);
            ASch1 |= SF1;
            ASch2 |= SF2;
        }

        AA1 += Mult_H_res;
        AA2 += Mult_L_res;
        
        if (macmode_tmp & (1<<14)) { // guard-bit
            SATU_VF(AA2, AA2, 32, SF2, uint64_t);
            SATU_VF(AA1, AA1, 32, SF1, uint64_t);
        }  else {
            SATU_VF(AA2, AA2, 40, SF2, uint64_t);
            SATU_VF(AA1, AA1, 40, SF1, uint64_t);
        }
        ASch1 |= SF1;
        ASch2 |= SF2;

        SATU_VF(acc1_tmp,AA1,32,SF1,uint32_t);
        SATU_VF(acc2_tmp,AA2,32,SF2,uint32_t);

        _set_ACC1(acc1_tmp);
        _set_ACC2(acc2_tmp);

        if (macmode_tmp & 0x8) {//if P is set then return MSP
            r = ((acc1_tmp & 0xffff0000) | ((acc2_tmp>>16) & 0x0000ffff));
        }
        else { // LSP
            SATU_VF(acc1_tmp,acc1_tmp,16,SF1,uint32_t); // 
            SATU_VF(acc2_tmp,acc2_tmp,16,SF2,uint32_t); // 
            r = (acc1_tmp << 16) | (acc2_tmp & 0xffff);
        }

        _V = SF1 | SF2;
        _Z = r == 0;
        _N = 0;

        AZch1 = acc1_tmp == 0;
        ANch1 = 0;
        PSch1 = SF1;
        Sch1 = Sch1 | SF1;

        AZch2 = acc2_tmp == 0;
        ANch2 = 0;
        PSch2 = SF2;
        Sch2 = Sch2 | SF2;
    }
    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

int32_t     _macrdw    (int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t status32_tmp = _get_STATUS32();

    r = _macrdw_f(src1,src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t     _macrdw_f  (int32_t src1, int32_t src2)
{ 
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    {
        int64_t Mult_L_res, Mult_H_res;
        int32_t acc1_tmp, acc2_tmp;
        int32_t SF1 = 0, SF2 = 0;

        _V = 0;

        Mult_L_res = (int64_t)(swLO(src1) * swLO(src2));
        Mult_H_res = (int64_t)(swHI(src1) * swHI(src2));

        if (macmode_tmp & 0x4) { // fractional
            Mult_L_res <<= 1;
            Mult_H_res <<= 1;
        }

        if (macmode_tmp & (1<<14)) { // guard-bit
            int32_t false_sat1, false_sat2;
            false_sat1 = Mult_H_res == 0x80000000;
            false_sat2 = Mult_L_res == 0x80000000;
            SAT_VF(Mult_H_res,Mult_H_res,32,SF1,int64_t);
            SAT_VF(Mult_L_res,Mult_L_res,32,SF2,int64_t);

            ASch1 |= (SF1 & !false_sat1);
            ASch2 |= (SF2 & !false_sat2);
        }

        AA2 += Mult_L_res;
        AA1 += Mult_H_res;

        if (macmode_tmp & (1<<14)) { // guard-bit
            int32_t false_sat1, false_sat2;
            false_sat1 = AA1 == 0x7fffffff;
            false_sat2 = AA2 == 0x7fffffff;
            SAT_VF(AA2, AA2, 32, SF2, int64_t);
            SAT_VF(AA1, AA1, 32, SF1, int64_t);
            ASch1 |= (SF1 & !false_sat1);
            ASch2 |= (SF2 & !false_sat2);
        } else {
            SAT_VF(AA2, AA2, 40, SF2, int64_t);
            SAT_VF(AA1, AA1, 40, SF1, int64_t);
            ASch1 |= SF1;
            ASch2 |= SF2;
        }

        // round
        Mult_H_res = (AA1 + 0x8000);
        Mult_L_res = (AA2 + 0x8000);

        SAT_VF(acc1_tmp,((Mult_H_res)),32,SF1,int32_t);
        SAT_VF(acc2_tmp,((Mult_L_res)),32,SF2,int32_t);

        {
            int32_t tmp1,tmp2;
            tmp1 = acc1_tmp;
            tmp2 = acc2_tmp;
            if (macmode_tmp & (1<<15)) { // RM
                if (((acc1_tmp & 0xffff) == 0)) {
                    tmp1 &= 0xfffeffff;
                }
                if (((acc2_tmp & 0xffff) == 0)) {
                    tmp2 &= 0xfffeffff;
                }
            }
            _set_ACC1((tmp1) & 0xffff0000);
            _set_ACC2((tmp2) & 0xffff0000);
        }
        

        if (macmode_tmp & 0x8) { // return MSP
            if (macmode_tmp & (1<<15)) { // RM
                if (((acc1_tmp & 0xffff) == 0)) {
                    acc1_tmp &= 0xfffeffff;
                }
                if (((acc2_tmp & 0xffff) == 0)) {
                    acc2_tmp &= 0xfffeffff;
                }
            }
            r = (acc1_tmp & 0xffff0000) | ((acc2_tmp>>16) & 0x0000ffff);
        }
        else {
            
            if (acc1_tmp & 0xffff0000) {
                SF1 = 1;
                if (acc1_tmp > 0)
                    acc1_tmp = 0x7fff;
                else
                    acc1_tmp = 0x8000;
            }
            if (acc2_tmp & 0xffff0000) {
                SF2 = 1;
                if (acc2_tmp > 0)
                    acc2_tmp = 0x7fff;
                else
                    acc2_tmp = 0x8000;
            }

            r = ((acc1_tmp) << 16) | ((acc2_tmp) & 0xffff);
        }

        _V = SF1 | SF2;
        _Z = r == 0;
        _N = (r & 0x80008000) != 0;

        AZch1 = AA1 == 0;
        ANch1 = AA1 < 0;
        PSch1 = SF1;
        Sch1 = Sch1 | SF1;

        AZch2 = AA2 == 0;
        ANch2 = AA2 < 0;
        PSch2 = SF2;
        Sch2 = Sch2 | SF2;
    }
    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

int32_t     _msubdw    (int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t status32_tmp = _get_STATUS32();

    r = _msubdw_f(src1,src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t     _msubdw_f  (int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    {
        int64_t Mult_L_res, Mult_H_res;
        int32_t acc1_tmp, acc2_tmp;
        int32_t SF1 = 0, SF2 = 0;

        Mult_H_res = (int64_t)(swHI(src1) * swHI(src2));
        Mult_L_res = (int64_t)(swLO(src1) * swLO(src2));

        if (macmode_tmp & 0x4) { // fractional
            Mult_H_res <<= 1;
            Mult_L_res <<= 1;
        }

        if (macmode_tmp & (1<<14)) { // guard-bit
            int32_t false_sat1, false_sat2;
            
            false_sat1 = Mult_H_res == 0x80000000;
            false_sat2 = Mult_L_res == 0x80000000;
            SAT_VF(Mult_H_res,Mult_H_res,32,SF1,int64_t);
            SAT_VF(Mult_L_res,Mult_L_res,32,SF2,int64_t);

            ASch1 |= (SF1 & !false_sat1);
            ASch2 |= (SF2 & !false_sat2);
        }

        AA2 -= Mult_L_res;
        AA1 -= Mult_H_res;

        if (macmode_tmp & (1<<14)) { // guard-bit
            int32_t SF1_tmp = 0, SF2_tmp = 0;
            SAT_VF(AA1, AA1, 32, SF1_tmp, int64_t);
            SAT_VF(AA2, AA2, 32, SF2_tmp, int64_t);
            ASch1 |= SF1_tmp;
            ASch2 |= SF2_tmp;
            SF1 |= SF1_tmp;
            SF2 |= SF2_tmp;
        } else {
            SAT_VF(AA2, AA2, 40, SF2, int64_t);
            SAT_VF(AA1, AA1, 40, SF1, int64_t);
            ASch1 |= SF1;
            ASch2 |= SF2;
        }

        SAT_VF(acc1_tmp,AA1,32,SF1,int32_t);
        SAT_VF(acc2_tmp,AA2,32,SF2,int32_t);

        _set_ACC1(acc1_tmp);
        _set_ACC2(acc2_tmp);

        if (macmode_tmp & 0x8) { // return MSP
            r = (acc1_tmp & 0xffff0000) | ((acc2_tmp>>16) & 0x0000ffff);
        }
        else {
            SAT_VF(acc1_tmp,acc1_tmp,16,SF1,int32_t);
            SAT_VF(acc2_tmp,acc2_tmp,16,SF2,int32_t);
            r = (acc1_tmp << 16) | (acc2_tmp & 0xffff);
        }

        _V = SF1 | SF2;
        _Z = r == 0;
        _N = (r & 0x80008000) != 0;

        AZch1 = acc1_tmp == 0;
        ANch1 = acc1_tmp < 0;
        PSch1 = SF1;
        Sch1 = Sch1 | SF1;

        AZch2 = acc2_tmp == 0;
        ANch2 = acc2_tmp < 0;
        PSch2 = SF2;
        Sch2 = Sch2 | SF2;
    }
    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

int32_t     _cmacrdw   (int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t status32_tmp = _get_STATUS32();

    r = _cmacrdw_f(src1,src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t     _cmacrdw_f (int32_t src1, int32_t src2)
{ 
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    {
        int64_t Mult_L_res, Mult_H_res;
        int32_t acc1_tmp, acc2_tmp;
        int32_t SF1 = 0, SF2 = 0;
        int32_t false_sat1, false_sat2;

        _V = 0;

        Mult_L_res = (int64_t)(swLO(src1) * swLO(src2));
        Mult_H_res = (int64_t)(swHI(src1) * swHI(src2));

        if (macmode_tmp & 0x4) { // fractional
            Mult_L_res <<= 1;
            Mult_H_res <<= 1;
        }

        false_sat1 = Mult_H_res == 0x80000000;
        false_sat2 = Mult_L_res == 0x80000000;

        if (macmode_tmp & (1<<14)) { // guard-bit
            int32_t SF1_tmp = 0, SF2_tmp = 0;
            SAT_VF(Mult_H_res,Mult_H_res,32,SF2_tmp,int64_t);
            SAT_VF(Mult_L_res,Mult_L_res,32,SF1_tmp,int64_t);
            SF1 |= SF1_tmp;
            SF2 |= SF2_tmp;
            ASch1 |= (SF2_tmp & !false_sat1);
            ASch2 |= (SF1_tmp & !false_sat2);
        }

        AA1 += Mult_L_res;
        AA2 -= Mult_H_res;

        if (macmode_tmp & (1<<14)) { // guard-bit
            int32_t SF1_tmp = 0, SF2_tmp = 0;
            SAT_VF(AA1, AA1, 32, SF1_tmp, int64_t);
            SAT_VF(AA2, AA2, 32, SF2_tmp, int64_t);
            ASch1 |= SF1_tmp;
            ASch2 |= SF2_tmp;
            SF1 |= SF1_tmp;
            SF2 |= SF2_tmp;
        } else {
            SAT_VF(AA1, AA1, 40, SF1, int64_t);
            SAT_VF(AA2, AA2, 40, SF2, int64_t);
            ASch1 |= SF1;
            ASch2 |= SF2;
        }

        // round
        Mult_H_res = (AA1 + 0x8000);
        Mult_L_res = (AA2 + 0x8000);

        SAT_VF(acc1_tmp,((Mult_H_res)),32,SF1,int32_t);
        SAT_VF(acc2_tmp,((Mult_L_res)),32,SF2,int32_t);

        _set_ACC1((acc1_tmp) & 0xffff0000);
        _set_ACC2((acc2_tmp) & 0xffff0000);

        if (macmode_tmp & 0x8) { // return MSP
            r = (acc1_tmp & 0xffff0000) | ((acc2_tmp>>16) & 0x0000ffff);
        }
        else {

            if (acc1_tmp & 0xffff0000) {
                SF1 = 1;
                if (acc1_tmp > 0)
                    acc1_tmp = 0x7fff;
                else
                    acc1_tmp = 0x8000;
            }
            if (acc2_tmp & 0xffff0000) {
                SF2 = 1;
                if (acc2_tmp > 0)
                    acc2_tmp = 0x7fff;
                else
                    acc2_tmp = 0x8000;
            }
            r = ((acc1_tmp) << 16) | ((acc2_tmp) & 0xffff);
        }

        _V = SF1 | SF2;
        _Z = r == 0;
        _N = (r & 0x80008000) != 0;

        AZch1 = AA1 == 0;
        ANch1 = AA1 < 0;
        PSch1 = SF1;
        Sch1 = Sch1 | SF1;

        AZch2 = AA2 == 0;
        ANch2 = AA2 < 0;
        PSch2 = SF2;
        Sch2 = Sch2 | SF2;
    }
    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

/** \brief FFT Butterfly Dual Word
*
*   FBFDW performs the following calculation for each word:
*   dest <= sat16(AUX_FBF_STORE.high<<1 – (rnd16(A1)).high) : sat16(AUX_FBF_STORE.low<<1 – (rnd16(A2)).high)
*   AUX_FBF_STORE <= src
*   A1 <= SIGN_EXTEND(src.high) : 0000
*   A2 <= SIGN_EXTEND(src.low) : 0000
*/
uint32_t  _fbfdw   (uint32_t src)
{
    uint32_t r;
    int64_t fbf_store_high, fbf_store_low;
    int32_t macmode_tmp = _get_MACMODE();

    int32_t rm = ((macmode_tmp & (1<<15)/*MACMODE_RM_BIT*/) != 0);

    int32_t V1 = (AA1 < INT32_MIN) || (AA1 > INT32_MAX);
    int32_t V2 = (AA2 < INT32_MIN) || (AA2 > INT32_MAX);

    int64_t a1r, a2r;
    int32_t res1, res2;
    int32_t res1s, res2s;

    if (V1) {
        a1r = AA1;
    } else {
        AA1 = (AA1 + 0x8000);
        if (rm) { // convergent RM
            if (((AA1 & 0xffff) == 0)) {
                AA1 &= 0xfffffffffffeffff;
            }
        }
        a1r = AA1;
    }
    if (V2) {
        a2r = AA2;
    } else {
        AA2 = (AA2 + 0x8000);
        if (rm) { // convergent RM
            if (((AA2 & 0xffff) == 0)) {
                AA2 &= 0xfffffffffffeffff;
            }
        }
        a2r = AA2;
    }

    fbf_store_high = ((int32_t)_get_AUX_FBF_STORE_16() >> 16) << 1;
    fbf_store_low = ((int32_t)_get_AUX_FBF_STORE_16() << 16) >> 15;

    res1 = (int32_t)(fbf_store_high - (a1r >> 16));
    res2 = (int32_t)(fbf_store_low - (a2r >> 16));

    SAT(res1s,res1,16,int32_t);
    SAT(res2s,res2,16,int32_t);

    r = (res1s << 16) | (res2s & 0xffff);

    // The source is stored into the accumulator's MSP portion, sign extended.
    AA1 = (int64_t)(int16_t)(src >> 16) << 16;
    AA2 = (int64_t)(int16_t)(src) << 16;
    _set_AUX_FBF_STORE_16(src);

    return r; // emulation not supported
}

#endif // EMU__Xxmac_d16

#ifdef EMU__Xxmac_24

// Signed Multiply 24x24-bit Without Rounding
int32_t _mult(int32_t src1, int32_t src2)
{
    int32_t r;
    Internal_AA_24X24 = 0;
    r = _mact(src1,src2);
    return r;
} /* __mult */

int32_t   _mult_f    (int32_t src1, int32_t src2)
{
    int32_t r;
    Internal_AA_24X24 = 0;
    r = _mact_f(src1, src2);
    return r;
}

// Unsigned Multiply 24x24-bit Without Rounding
uint32_t _mulut(uint32_t src1, uint32_t src2)
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t status32_tmp = _get_STATUS32();

    r = _mulut_f(src1,src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

uint32_t   _mulut_f    (uint32_t src1, uint32_t src2)
{
    uint32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    {
        uint64_t Mult_res;
        int32_t SF = 0;

        _V = 0;

        Mult_res = (uint64_t)HIGH24(src1) * HIGH24(src2);

        if (macmode_tmp & Q24) {  // Fractional arithmetic
            Mult_res <<= 1;
        }

        Internal_AA_24X24 = Mult_res;

        SATU_VF(Internal_AA_24X24, Internal_AA_24X24, 56, SF, uint64_t);

        if (macmode_tmp & P24) {  // MSP write-back mode
            SATU_VF(Mult_res, Internal_AA_24X24, 48, SF, uint64_t);
            r = (uint32_t)(Mult_res>>24)<<8;   // HIGH.24(SAT.48(A)) : 0x00 
        } else {                  // LSP write-back mode
            SATU_VF(r, Internal_AA_24X24 , 32, SF, uint32_t);
        }

        _V = SF;
        _Z = r == 0;
        _N = 0;

        AZch1 = 0;
        ANch1 = 0;
        PSch1 = 0;
        Sch1 = Sch1;

        AZch2 = Internal_AA_24X24 == 0;
        ANch2 = 0;
        PSch2 = SF;
        Sch2 = Sch2 | SF;
    }
    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

// Signed Multiply 24x24-bit With Rounding - With Result
int32_t _mulrt(int32_t src1, int32_t src2)
{
    int32_t r;
    Internal_AA_24X24 = 0;
    r = _macrt(src1,src2);
    return r;
}

int32_t   _mulrt_f    (int32_t src1, int32_t src2)
{
    int32_t r;
    Internal_AA_24X24 = 0;
    r = _macrt_f(src1, src2);
    return r;
}

// Multiply-Accumulate 24x24-bit Without Rounding - With Result
int32_t _mact(int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t status32_tmp = _get_STATUS32();

    r = _mact_f(src1,src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t   _mact_f    (int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    {
        int64_t Mult_res;
        int32_t SF = 0;

        _V = 0;

        Mult_res = (int64_t)HIGH24(src1) * HIGH24(src2);

        if (macmode_tmp & Q24) {  // Fractional arithmetic
            Mult_res <<= 1;
        }
        
        Internal_AA_24X24 += Mult_res;

        SAT_VF(Internal_AA_24X24, Internal_AA_24X24, 56, SF, int64_t);
        ASch2 |= SF;

        if (macmode_tmp & P24) {  // MSP write-back mode
            SAT_VF(Mult_res, Internal_AA_24X24, 48, SF, int64_t);
            r = (int32_t)(Mult_res>>24)<<8;   // HIGH.24(SAT.48(A)) : 0x00 
        } else {                  // LSP write-back mode
            SAT_VF(r, Internal_AA_24X24 , 32, SF, int32_t);
        }
       
        _V = SF;
        _Z = r == 0;
        _N = (r & 0x80000000) != 0;

        AZch1 = 0;
        ANch1 = 0;
        PSch1 = 0;
        Sch1 = Sch1;

        AZch2 = Internal_AA_24X24 == 0;
        ANch2 = Internal_AA_24X24 < 0;
        PSch2 = SF;
        Sch2 = Sch2 | SF;
    }
    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

// Multiply-Accumulate 24x24-bit With Rounding - With Result
int32_t _macrt(int32_t src1, int32_t src2) 
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t status32_tmp = _get_STATUS32();

    r = _macrt_f(src1,src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t   _macrt_f    (int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    {
        int64_t Mult_res;
        int32_t SF = 0;

        _V = 0;

        Mult_res = (int64_t)HIGH24(src1) * HIGH24(src2);

        if (macmode_tmp & Q24) {  // Fractional arithmetic
            Mult_res <<= 1;
        }

        Internal_AA_24X24 += Mult_res;

        SAT_VF(Internal_AA_24X24, Internal_AA_24X24, 56, SF, int64_t);
        ASch2 |= SF;

        RND24(Mult_res);

        if (macmode_tmp & RM24) {
            if (((Mult_res & 0xffffff) == 0)) {
                Mult_res &= 0xfffffffffeffffff;
            }
        }

        if (macmode_tmp & P24) {  // MSP write-back mode
            SAT_VF(Mult_res, Mult_res, 48, SF, int64_t);
            r = (int32_t)(Mult_res>>24)<<8;   // HIGH.24(SAT.48(A)) : 0x00 
        } else {                  // LSP write-back mode
            SAT_VF(r, Mult_res , 32, SF, int32_t);
        }

        _V = SF;
        _Z = r == 0;
        _N = (r & 0x80000000) != 0;

        AZch1 = 0;
        ANch1 = 0;
        PSch1 = 0;
        Sch1 = Sch1;

        AZch2 = Internal_AA_24X24 == 0;
        ANch2 = Internal_AA_24X24 < 0;
        PSch2 = SF;
        Sch2 = Sch2 | SF;
    }
    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

// Multiply-Subtract 24x24-bit Without Rounding - With Result
int32_t _msubt(int32_t src1, int32_t src2) 
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t status32_tmp = _get_STATUS32();

    r = _msubt_f(src1,src2);

    _set_MACMODE(macmode_tmp);
    _flag(status32_tmp);
    return r;
}

int32_t   _msubt_f    (int32_t src1, int32_t src2)
{
    int32_t r;
    int32_t macmode_tmp = _get_MACMODE();
    int32_t ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2;
#ifdef _ARC
    int32_t status32_tmp = _get_STATUS32();
    GET_FLAGS(status32_tmp,_Z,_N,_C,_V);
#endif
    GET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    {
        int64_t Mult_res;
        int32_t SF = 0;

        _V = 0;

        Mult_res = (int64_t)HIGH24(src1) * HIGH24(src2);

        if (macmode_tmp & Q24) {  // Fractional arithmetic
            Mult_res <<= 1;
        }

        Internal_AA_24X24 -= Mult_res;

        SAT_VF(Internal_AA_24X24, Internal_AA_24X24, 56, SF, int64_t);
        ASch2 |= SF;

        if (macmode_tmp & P24) {  // MSP write-back mode
            SAT_VF(Mult_res, Internal_AA_24X24, 48, SF, int64_t);
            r = (int32_t)(Mult_res>>24)<<8;   // HIGH.24(SAT.48(A)) : 0x00 
        } else {                  // LSP write-back mode
            SAT_VF(r, Internal_AA_24X24 , 32, SF, int32_t);
        }

        _V = SF;
        _Z = r == 0;
        _N = (r & 0x80000000) != 0;

        AZch1 = 0;
        ANch1 = 0;
        PSch1 = 0;
        Sch1 = Sch1;

        AZch2 = Internal_AA_24X24 == 0;
        ANch2 = Internal_AA_24X24 < 0;
        PSch2 = SF;
        Sch2 = Sch2 | SF;
    }
    SET_MACMODEFLAGS(macmode_tmp, ASch1, AZch1, ANch1, PSch1, Sch1, ASch2, AZch2, ANch2, PSch2, Sch2);
    _set_MACMODE(macmode_tmp);
#ifdef _ARC
    SET_FLAGS(status32_tmp,_Z,_N,_C,_V);
    _flag(status32_tmp);
#endif
    return r;
}

#endif // EMU__Xxmac_24

#ifdef EMU__Xdmulpf

int32_t M_implicit_src = 0;
uint32_t *pA_implicit_src = 0;
int32_t BASE_implicit_src = 0;
int32_t *pXYBank = 0;

int32_t last_func_pf_flag = 0;
int32_t pf_implicit_dst2 = 0;

///////////////////////////////////////////////////////////////////////////////
// 80 accumulator for pseudo floating point dual MUL/MAC implementation
///////////////////////////////////////////////////////////////////////////////
typedef struct
{
    int32_t H;
    int32_t M;
    int32_t L;
} int80_t;

static int80_t A1_80;
static int80_t A2_80;

int32_t toLong_norm80(int80_t* A) {
    return ( A->H << 16 ) | ( ( unsigned int )A->M >> 16 );
}

void int80_exp(int80_t *A, long long m47, int e5 ) {

    A->L = 0;
    A->M = (int)(( m47 << 1 ) & 0xFFFFFFFF);
    A->H = (int)(( m47 >> 31 ) & 0xFFFFFFFF);

    if ( e5 ) {
        A->L = A->M << ( 32 - e5 );
        A->M = ( (unsigned int)A->M >> e5 ) | ( A->H << (32 - e5) );
        A->H >>= e5;
    }
}

void saturate80(int80_t *A) {

    if ( A->H > 0x7FFF ) {
        A->H = 0x7FFF;
        A->M = A->L = 0xFFFFFFFF;
    } else if ( A->H < -(0x8000) ) {
        A->H = 0xFFFF8000;
        A->M = A->L = 0;
    }
}

void operator_plus (int80_t* A1, int80_t A2) 
{
    A1->L = _add_f(A1->L , A2.L);
    A1->M = _adc_f(A1->M , A2.M);
    A1->H = _adc(A1->H , A2.H);
}

int64_t saturate47(int64_t a)
{
    if (a < (int64_t)0xFFFFC00000000000)
        a = (int64_t)0xFFFFC00000000000;
    else
        if (a > (int64_t)0x00003FFFFFFFFFFF)
            a = (int64_t)0x00003FFFFFFFFFFF;

    return a;
}

int32_t get_xy(
    uint32_t ModifierRG, 
    uint32_t AddressRG, 
    int32_t *XYBankBase
    );

void update_a(
    uint32_t ModifierRG, 
    uint32_t *AddressRG,
    int32_t base_address
    );

int32_t _dmulpf_xy(int32_t src1, int32_t src2 )
{
    int32_t res;
    int32_t src3_implicit = get_xy(M_implicit_src,*pA_implicit_src, pXYBank); 
    
    int32_t m = src2 >> 16;
    int32_t e = src2 & 0x1F;

    int64_t ams = saturate47((int64_t)src1 * m);
    int64_t bms = saturate47((int64_t)src3_implicit * m);

    int80_t a1, a2;

    int32_t status32_tmp = _get_STATUS32();

    if (BASE_implicit_src != -1)
        update_a(M_implicit_src, pA_implicit_src, BASE_implicit_src);

    int80_exp(&a1, ams, e );
    int80_exp(&a2, bms, e );

    A1_80 = a1;
    A2_80 = a2;

    ACC1_H_dmulpf = toLong_norm80(&a1);
    ACC2_H_dmulpf = toLong_norm80(&a2);
    res = ACC1_H_dmulpf;
    pf_implicit_dst2 = ACC2_H_dmulpf;

    last_func_pf_flag = 1;

    _flag(status32_tmp);
    return res;
}

int32_t _dmacpf_xy(int32_t src1, int32_t src2)
{
    int32_t res;
    int32_t src3_implicit = get_xy(M_implicit_src,*pA_implicit_src, pXYBank);
    int32_t m = src2 >> 16;
    int32_t e = src2 & 0x1F;

    int64_t ams = saturate47((int64_t)src1 * m);
    int64_t bms = saturate47((int64_t)src3_implicit * m);

    int80_t a1, a2;

    int32_t status32_tmp = _get_STATUS32();

    if (BASE_implicit_src != -1)
        update_a(M_implicit_src, pA_implicit_src, BASE_implicit_src);

    int80_exp(&a1, ams, e );
    int80_exp(&a2, bms, e );

    operator_plus(&A1_80,a1);
    operator_plus(&A2_80,a2);

    saturate80(&A1_80);
    saturate80(&A2_80);

    ACC1_H_dmulpf = toLong_norm80(&A1_80);
    ACC2_H_dmulpf = toLong_norm80(&A2_80);
    res = ACC1_H_dmulpf;
    pf_implicit_dst2 = ACC2_H_dmulpf;
    last_func_pf_flag = 1;

    _flag(status32_tmp);
    return res;
}

#endif // EMU__Xdmulpf

#ifdef EMU__Xcrc

uint32_t _crc(uint32_t a, uint32_t b)
{
    uint32_t indata, syn;
    uint32_t r5 = 0;
    int i;
    int32_t status32_tmp = _get_STATUS32();

    /*  The CRC instruction takes two operands. 
        The first is data. The data width is fixed at 16 bits and the data 
        must be in the most significant 16bits of the word. 
    */
    indata = a & 0xffff0000;
    /*  The second operand is the current value of the CRC syndrome 
        (this is a w-bit value, left justified, where w is the width of the polynomial).
    */
    syn = b;

    if(aux_crc_mode & 0x1) //reflection
    {
        
        for(i = 0; i < 8; i++)
        {
            r5 = r5 >> 1;
            if(indata & 0x80000000)
                r5 = r5 | 0x80000000;
            if (indata&0x00800000)
                r5 = r5 | 0x00800000;
            if (indata&0x00008000)
                r5 = r5 | 0x00008000;
            if (indata&0x00000080)
                r5 = r5 | 0x00000080;
            indata = indata << 1;
        }
        indata=r5;
    }

    syn = syn ^ indata;
    for(i = 0; i < 16; i++)
    {
        if(syn&0x80000000)
        {
            syn = syn << 1;
            syn = syn ^ aux_crc_poly;
        }
        else
            syn = syn << 1;
    }

    _flag(status32_tmp);
    return syn;
}

uint32_t _get_AUX_CRC_POLY()
{
    return aux_crc_poly;
}

uint32_t _get_AUX_CRC_MODE()
{
    return aux_crc_mode;
}

void _set_AUX_CRC_POLY(uint32_t x)
{
    aux_crc_poly = x;
}

void _set_AUX_CRC_MODE(uint32_t x)
{
    aux_crc_mode = x & 0x1;
}

#endif // EMU__Xcrc

#ifndef _ARC

/*
    Timers
*/

void _START_TIMER_0()
{
    ;// no emulation provided
}

uint32_t _READ_TIMER_0()
{
    return(0);// no emulation provided
}

void _STOP_TIMER_0()
{
    ;// no emulation provided
}

void _START_TIMER_1()
{
    ;// no emulation provided
}

uint32_t _READ_TIMER_1()
{
    return(0);// no emulation provided
}

void _STOP_TIMER_1()
{
    ;// no emulation provided
}

/*
    Stack
*/

uint32_t _get_SP() 
{
    return(0);// no emulation provided
}

void * _get_stack_start()
{
    return (0);// no emulation provided
}

void * _get_stack_end()
{
    return (0);// no emulation provided
}

#endif // !_ARC

#ifdef EMU__Xxy // XY memory simulation

XYBank m_arXYMemory[XYMEM_XY_NUM];

#define OFFSET_BITS 14
#define MODULO_BITS 13
#define NOT_UPDATE (-1)

uint32_t rev(uint32_t q, int s) {
    int32_t nu, r = 0;
    for (nu = 1; nu <= s; nu++) {
        r |= (q & 1) << (s - nu);
        q >>= 1;
    }
    return r;
}

void update_a(
    uint32_t ModifierRG, 
    uint32_t *AddressRG,
    int32_t base_address
    )
{
    int32_t offset;
    int32_t modulo; 

    offset = (int32_t)(ModifierRG << (32 - OFFSET_BITS)) >> (32 - OFFSET_BITS);
    modulo = (uint32_t)(ModifierRG << (16 - MODULO_BITS)) >> (32 - MODULO_BITS);

    if (modulo == 0)
        modulo = XYMEM_MAX_OFFSET16;

    if ((ModifierRG & AM_BITREV) != 0) { // reverse addressing
        int s = 13, m = (1<<s)-1;
        *AddressRG = rev((rev(*AddressRG & m, s) + rev(offset & m, s)) & m, s);
    } 
    else
    {
        if (offset) 
        {
            int32_t tmp_addr;
            tmp_addr = *AddressRG + offset - base_address;
            
            if ((offset > 0) && (tmp_addr >= modulo))
                tmp_addr -= modulo; 
            if ((offset < 0) && (tmp_addr < 0))
                tmp_addr += modulo;
            
            *AddressRG = tmp_addr + base_address;
        }
    }
}

uint32_t setup_baseaddr( 
    uint32_t modulo, // modulo addressing range
    uint32_t AddressRG
    )
{
    uint32_t buffer_step_power2; // base address step for multiple modulo buffers
    uint32_t m_a_tmp;
    uint32_t baseAddr;

    if (modulo == 0) // could be 0 if modifier register has not been set
        modulo = XYMEM_MAX_OFFSET16;

    buffer_step_power2 = 31 - _norm(modulo - (modulo > 1));

    ASSERT((buffer_step_power2 != 0)&&((1 << buffer_step_power2) <= XYMEM_MAX_OFFSET16));
    
    m_a_tmp = AddressRG & (XYMEM_MAX_OFFSET16 - 1);
    baseAddr = (m_a_tmp >> (buffer_step_power2)) << (buffer_step_power2);

    ASSERT((baseAddr < XYMEM_MAX_OFFSET16));
    return baseAddr;
}

void set_m(
    uint32_t *pModifierRG,
    uint32_t pAddressRG,
    uint32_t *pmodulo,
    int32_t *poffset,
    uint32_t m
    )
{
    ASSERT(((m >> 30) != 0x2) && ((m >> 30) != 0x3)); // reserved addressing modes
    ASSERT(((m >> 14) & 0x1) == 0x0); // reserved bit
    m &= 0xffffbfff; // reserved bit clearing (to match HW)
    *pModifierRG = m;
    *poffset = (int32_t)(m << (32 - OFFSET_BITS)) >> (32 - OFFSET_BITS);
    *pmodulo = (uint32_t)((m >> 16) << (32 - MODULO_BITS)) >> (32 - MODULO_BITS);

    ASSERT(((m & AM_16BIT)&&(*pmodulo <= XYMEM_MAX_OFFSET16))||(*pmodulo <= XYMEM_MAX_OFFSET32));
    ASSERT(((m & AM_16BIT)&&(_abs(*poffset) <= XYMEM_MAX_OFFSET16))||(_abs(*poffset) <= XYMEM_MAX_OFFSET32));
    
    if (*pmodulo == 0)
        *pmodulo = XYMEM_MAX_OFFSET16;

    ASSERT(*pmodulo > _abs(*poffset)); 
}

int32_t get_xy(
    uint32_t ModifierRG, 
    uint32_t AddressRG, 
    int32_t *XYBankBase
    )
{
    int32_t r;
    
    if ((ModifierRG & AM_16BIT) == 0)
    {
        r = XYBankBase[ AddressRG & ((XYMEM_MAX_OFFSET32)-1) ];
    }
    else
    {
        r = (int32_t)((int16_t*)(void *)XYBankBase)[AddressRG] << 16;

        if (ModifierRG & AM_DUAL)
            r = r | ((uint32_t)r >> 16);
    }

    // Check for memory "traps"
#if defined(TRAP_XMEM_ADDR_READ)
    if (XYBankBase == m_arXYMemory[0] && AddressRG == TRAP_XMEM_ADDR_READ
#if defined(TRAP_XMEM_VALUE_READ)
        && r == TRAP_XMEM_VALUE_READ
#endif
        )
    {
        TRAP("XY read match");
    }
#if defined(TRAP_YMEM_ADDR_READ)
    if (XYBankBase == m_arXYMemory[1] && AddressRG == TRAP_YMEM_ADDR_READ
#if defined(TRAP_YMEM_VALUE_READ)
        && r == TRAP_YMEM_VALUE_READ
#endif
        )
    {
        TRAP("XY read match");
    }
#elif defined(TRAP_XMEM_VALUE_READ)
    if (XYBankBase == m_arXYMemory[0] && r == TRAP_XMEM_VALUE_READ)
        TRAP("XY read match");
#endif
#elif defined(TRAP_YMEM_VALUE_READ)
    if (XYBankBase == m_arXYMemory[1] && r == TRAP_YMEM_VALUE_READ)
        TRAP("XY read match");
#endif

    return r;
}

void set_xy(
    uint32_t ModifierRG, 
    uint32_t *AddressRG, 
    int32_t *XYBankBase,
    int32_t v)
{
    // Check for memory "traps"
#if defined(TRAP_XMEM_ADDR_WRITE)
    if (XYBankBase == m_arXYMemory[0] && AddressRG == TRAP_XMEM_ADDR_WRITE
#if defined(TRAP_XMEM_VALUE_WRITE)
        && v == TRAP_XMEM_VALUE_WRITE
#endif
        )
    {
        TRAP("XY write match");
    }
#if defined(TRAP_YMEM_ADDR_WRITE)
    if (XYBankBase == m_arXYMemory[1] && AddressRG == TRAP_YMEM_ADDR_WRITE
#if defined(TRAP_YMEM_VALUE_WRITE)
        && v == TRAP_YMEM_VALUE_WRITE
#endif
        )
    {
        TRAP("XY write match");
    }
#elif defined(TRAP_XMEM_VALUE_WRITE)
    if (XYBankBase == m_arXYMemory[0] && v == TRAP_XMEM_VALUE_WRITE)
        TRAP("XY write match");
#endif
#elif defined(TRAP_YMEM_VALUE_WRITE)
    if (XYBankBase == m_arXYMemory[1] && v == TRAP_YMEM_VALUE_WRITE)
        TRAP("XY write match");
#endif

    if ((ModifierRG & AM_16BIT) == 0)
    {
        XYBankBase[ *AddressRG & ((XYMEM_MAX_OFFSET32)-1)] = v;
    }
    else
    {
        *AddressRG &= ((XYMEM_MAX_OFFSET16)-1);
        ((int16_t*)(void *)XYBankBase)[*AddressRG] = (int16_t)(v >> 16);
    }
}

XYBank* XMEM = &m_arXYMemory[0];
XYBank* YMEM = &m_arXYMemory[1];

uint32_t _AX0 = 0;
uint32_t _AX1 = 0;
uint32_t _AX2 = 0;
uint32_t _AX3 = 0;
uint32_t _AY0 = 0;
uint32_t _AY1 = 0;
uint32_t _AY2 = 0;
uint32_t _AY3 = 0;

uint32_t AX0_PREV = 0;
uint32_t AX1_PREV = 0;
uint32_t AX2_PREV = 0;
uint32_t AX3_PREV = 0;

uint32_t AY0_PREV = 0;
uint32_t AY1_PREV = 0;
uint32_t AY2_PREV = 0;
uint32_t AY3_PREV = 0;

uint32_t _MX00 = 0;
uint32_t _MX01 = 0;
uint32_t _MX10 = 0;
uint32_t _MX11 = 0;
uint32_t _MX20 = 0;
uint32_t _MX21 = 0;
uint32_t _MX30 = 0;
uint32_t _MX31 = 0;
uint32_t _MY00 = 0;
uint32_t _MY01 = 0;
uint32_t _MY10 = 0;
uint32_t _MY11 = 0;
uint32_t _MY20 = 0;
uint32_t _MY21 = 0;
uint32_t _MY30 = 0;
uint32_t _MY31 = 0;

uint32_t MODX00 = 0;
uint32_t MODX01 = 0;
uint32_t MODX10 = 0;
uint32_t MODX11 = 0;
uint32_t MODX20 = 0;
uint32_t MODX21 = 0;
uint32_t MODX30 = 0;
uint32_t MODX31 = 0;

uint32_t MODY00 = 0;
uint32_t MODY01 = 0;
uint32_t MODY10 = 0;
uint32_t MODY11 = 0;
uint32_t MODY20 = 0;
uint32_t MODY21 = 0;
uint32_t MODY30 = 0;
uint32_t MODY31 = 0;

uint32_t BASEX00 = 0;
uint32_t BASEX01 = 0;
uint32_t BASEX10 = 0;
uint32_t BASEX11 = 0;
uint32_t BASEX20 = 0;
uint32_t BASEX21 = 0;
uint32_t BASEX30 = 0;
uint32_t BASEX31 = 0;

uint32_t BASEY00 = 0;
uint32_t BASEY01 = 0;
uint32_t BASEY10 = 0;
uint32_t BASEY11 = 0;
uint32_t BASEY20 = 0;
uint32_t BASEY21 = 0;
uint32_t BASEY30 = 0;
uint32_t BASEY31 = 0;

int32_t INCX00 = 0;
int32_t INCX01 = 0;
int32_t INCX10 = 0;
int32_t INCX11 = 0;
int32_t INCX20 = 0;
int32_t INCX21 = 0;
int32_t INCX30 = 0;
int32_t INCX31 = 0;

int32_t INCY00 = 0;
int32_t INCY01 = 0;
int32_t INCY10 = 0;
int32_t INCY11 = 0;
int32_t INCY20 = 0;
int32_t INCY21 = 0;
int32_t INCY30 = 0;
int32_t INCY31 = 0;

uint32_t _get_xbase()
{
    return((uint32_t)&m_arXYMemory[0]);
}

uint32_t _get_ybase()
{
    return((uint32_t)&m_arXYMemory[1]);
}

void _set_xbase(uint32_t xbase)
{
    ; // no emulation
}

void _set_ybase(uint32_t ybase)
{
    ; // no emulation
}

void _flushx() 
{
    ; // no emulation
}

void _flushy() 
{
    ; // no emulation
}

///////////////////////////////////////

#define IMPLEMENT_XMEM_ACCESSOR(I)      \
uint32_t _ax##I()  \
{    \
    return _AX##I;\
}    \
\
void _set_ax##I(uint32_t n)            \
{    \
    AX##I##_PREV = _AX##I;\
    _AX##I = n & (XYMEM_MAX_OFFSET16-1);\
    BASEX##I##0 = setup_baseaddr(MODX##I##0, _AX##I);\
    BASEX##I##1 = setup_baseaddr(MODX##I##1, _AX##I);\
}    \
\
uint32_t _mx##I##0()   \
{    \
    return _MX##I##0;\
}    \
\
void _set_mx##I##0(uint32_t m)         \
{    \
    set_m(&_MX##I##0, _AX##I, &MODX##I##0, &INCX##I##0, m);\
    BASEX##I##0 = setup_baseaddr(MODX##I##0, _AX##I);\
}    \
\
uint32_t _mx##I##1()   \
{    \
    return _MX##I##1;\
}    \
\
void _set_mx##I##1(uint32_t m)         \
{    \
    set_m(&_MX##I##1, _AX##I, &MODX##I##1, &INCX##I##1, m);\
    BASEX##I##1 = setup_baseaddr(MODX##I##1, _AX##I);\
}    \
\
int32_t _x##I##_u0()   \
{    \
    int32_t r;\
    \
    M_implicit_src = _MY##I##0;\
    pA_implicit_src = &_AY##I;\
    BASE_implicit_src = BASEY##I##0;\
    pXYBank = m_arXYMemory[1];\
    \
    AX##I##_PREV = _AX##I;\
    r = get_xy(_MX##I##0,_AX##I, m_arXYMemory[0]);\
    update_a(_MX##I##0, &_AX##I, BASEX##I##0);\
    return r;  \
}    \
\
int32_t _x##I##_u1()   \
{    \
    int32_t r;\
    \
    M_implicit_src = _MY##I##1;\
    pA_implicit_src = &_AY##I;\
    BASE_implicit_src = BASEY##I##1;\
    pXYBank = m_arXYMemory[1];\
    \
    AX##I##_PREV = _AX##I;\
    r = get_xy(_MX##I##1,_AX##I, m_arXYMemory[0]);\
    update_a(_MX##I##1, &_AX##I, BASEX##I##1);\
    return r;  \
}    \
\
int32_t _x##I##_nu()   \
{    \
    int32_t r;\
    \
    M_implicit_src = _MY##I##0;\
    pA_implicit_src = &_AY##I;\
    BASE_implicit_src = -1;\
    pXYBank = m_arXYMemory[1];\
    \
    r = get_xy(_MX##I##0,_AX##I, m_arXYMemory[0]);\
    return r;  \
}    \
\

///////////////////////////////////////
#define IMPLEMENT_YMEM_ACCESSOR(I)      \
uint32_t _ay##I()  \
{    \
    return _AY##I;\
}    \
\
void _set_ay##I(uint32_t n)    \
{    \
    AY##I##_PREV = _AY##I;\
    _AY##I = n & (XYMEM_MAX_OFFSET16-1);\
    BASEY##I##0 = setup_baseaddr(MODY##I##0, _AY##I);\
    BASEY##I##1 = setup_baseaddr(MODY##I##1, _AY##I);\
}    \
\
uint32_t _my##I##0()   \
{    \
    return _MY##I##0;\
}    \
\
void _set_my##I##0(uint32_t m) \
{    \
    set_m(&_MY##I##0, _AY##I, &MODY##I##0, &INCY##I##0, m);\
    BASEY##I##0 = setup_baseaddr(MODY##I##0, _AY##I);\
}    \
\
uint32_t _my##I##1()   \
{    \
    return _MY##I##1;\
}    \
\
void _set_my##I##1(uint32_t m) \
{    \
    set_m(&_MY##I##1, _AY##I, &MODY##I##1, &INCY##I##1, m);\
    BASEY##I##1 = setup_baseaddr(MODY##I##1, _AY##I);\
}    \
\
int32_t _y##I##_u0()   \
{    \
    int32_t r;\
    \
    M_implicit_src = _MX##I##0;\
    pA_implicit_src = &_AX##I;\
    BASE_implicit_src = BASEX##I##0;\
    pXYBank = m_arXYMemory[0];\
    \
    AY##I##_PREV = _AY##I;\
    r = get_xy(_MY##I##0,_AY##I, m_arXYMemory[1]);\
    update_a(_MY##I##0, &_AY##I, BASEY##I##0);\
    return r;  \
}    \
\
int32_t _y##I##_u1()   \
{    \
    int32_t r;\
    \
    M_implicit_src = _MX##I##1;\
    pA_implicit_src = &_AX##I;\
    BASE_implicit_src = BASEX##I##1;\
    pXYBank = m_arXYMemory[0];\
    \
    AY##I##_PREV = _AY##I;\
    r = get_xy(_MY##I##1,_AY##I, m_arXYMemory[1]);\
    update_a(_MY##I##1, &_AY##I, BASEY##I##1);\
    return r;  \
}    \
\
int32_t _y##I##_nu()   \
{    \
    int32_t r;\
    \
    M_implicit_src = _MX##I##0;\
    pA_implicit_src = &_AX##I;\
    BASE_implicit_src = -1;\
    pXYBank = m_arXYMemory[0];\
    \
    r = get_xy(_MY##I##0,_AY##I, m_arXYMemory[1]);\
    return r;  \
}    \
\

///////////////////////////////////////
// 4 accessors to x-bank
IMPLEMENT_XMEM_ACCESSOR(0)
IMPLEMENT_XMEM_ACCESSOR(1)
IMPLEMENT_XMEM_ACCESSOR(2)
IMPLEMENT_XMEM_ACCESSOR(3)

///////////////////////////////////////
// 4 accessors to y-bank
IMPLEMENT_YMEM_ACCESSOR(0)
IMPLEMENT_YMEM_ACCESSOR(1)
IMPLEMENT_YMEM_ACCESSOR(2)
IMPLEMENT_YMEM_ACCESSOR(3)

void _CopyXMemToXMem(uint32_t dest, uint32_t src, uint32_t n)
{
    uint32_t i;
    int ax0_tmp;
    int ax1_tmp;
    int mx00_tmp;
    int mx10_tmp;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    ax0_tmp = _ax0();
    ax1_tmp = _ax1();
    mx00_tmp = _mx00();
    mx10_tmp = _mx10();

    _set_mx00(1);
    _set_ax0(dest);
    _set_mx10(1);
    _set_ax1(src);

    for (i = 0; i < n; i++) {
        _set_x0_u0(_x1_u0());
    }

    _set_ax0(ax0_tmp);
    _set_mx00(mx00_tmp);
    _set_ax1(ax1_tmp);
    _set_mx10(mx10_tmp);
}

void _CopyYMemToXMem(uint32_t dest, uint32_t src, uint32_t n)
{
    uint32_t i;
    int ax0_tmp;
    int ay0_tmp;
    int mx00_tmp;
    int my00_tmp;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    ax0_tmp = _ax0();
    ay0_tmp = _ay0();
    mx00_tmp = _mx00();
    my00_tmp = _my00();

    _set_mx00(1);
    _set_ax0(dest);
    _set_my00(1);
    _set_ay0(src);

    for (i = 0; i < n; i++) {
        _set_x0_u0(_y0_u0());
    }

    _set_ax0(ax0_tmp);
    _set_mx00(mx00_tmp);
    _set_ay0(ay0_tmp);
    _set_my00(my00_tmp);
}

void _CopyXMemToYMem(uint32_t dest, uint32_t src, uint32_t n)
{
    uint32_t i;
    int ay0_tmp;
    int ax0_tmp;
    int my00_tmp;
    int mx00_tmp;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    ay0_tmp = _ay0();
    ax0_tmp = _ax0();
    my00_tmp = _my00();
    mx00_tmp = _mx00();

    _set_my00(1);
    _set_ay0(dest);
    _set_mx00(1);
    _set_ax0(src);

    for (i = 0; i < n; i++) {
        _set_y0_u0(_x0_u0());
    }

    _set_ay0(ay0_tmp);
    _set_my00(my00_tmp);
    _set_ax0(ax0_tmp);
    _set_mx00(mx00_tmp);
}

void _CopyYMemToYMem(uint32_t dest, uint32_t src, uint32_t n)
{
    uint32_t i;
    int ay0_tmp;
    int ay1_tmp;
    int my00_tmp;
    int my10_tmp;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    ay0_tmp = _ay0();
    ay1_tmp = _ay1();
    my00_tmp = _my00();
    my10_tmp = _my10();

    _set_my00(1);
    _set_ay0(dest);
    _set_my10(1);
    _set_ay1(src);

    for (i = 0; i < n; i++) {
        _set_y0_u0(_y1_u0());
    }

    _set_ay0(ay0_tmp);
    _set_my00(my00_tmp);
    _set_ay1(ay1_tmp);
    _set_my10(my10_tmp);
}

void _BurstXMem(uint32_t dest, int32_t val, int n)
{
    int i;
    int mx_tmp;
    int ax0_tmp;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    mx_tmp = _mx00();
    ax0_tmp = _ax0();
	
    _set_mx00(1);
    _set_ax0(dest);

    for (i = 0; i < n; i++)
        _set_x0_u0(val);

    _set_mx00(mx_tmp);
    _set_ax0(ax0_tmp);
}

void _BurstYMem(uint32_t dest, int32_t val, int n)
{
    int i;
    int my_tmp;
    int ay0_tmp;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    my_tmp = _mx00();
    ay0_tmp = _ax0();

    _set_my00(1);
    _set_ay0(dest);

    for (i = 0; i < n; i++)
        _set_y0_u0(val);

    _set_my00(my_tmp);
    _set_ay0(ay0_tmp);
}

void _BurstSMem(int32_t* dest, int32_t val, int n)
{
    int i;
    for (i = 0; i < n; i++)
        dest[i] = val;
}

void _BurstSMemToXMem(unsigned dest, const int32_t* src, int n)
{
    int i;
    int ax0_tmp;
    int mx_tmp;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    ax0_tmp = _ax0();
    mx_tmp = _mx00();

    _set_mx00(1);
    _set_ax0(dest);

    for (i = 0; i < n; i++)
        _set_x0_u0(src[i]);

    _set_ax0(ax0_tmp);
    _set_mx00(mx_tmp);
}

void _BurstSMemToYMem(unsigned dest, const int32_t* src, int n)
{
    int i;
    int ay0_tmp;
    int my_tmp;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    ay0_tmp = _ay0();
    my_tmp = _my00();

    _set_my00(1);
    _set_ay0(dest);

    for (i = 0; i < n; i++)
        _set_y0_u0(src[i]);

    _set_ay0(ay0_tmp);
    _set_my00(my_tmp);
}

void _BurstXMemToSMem(int32_t* dest, const unsigned src, int n)
{
    int i;
    int ax0_tmp;
    int mx_tmp;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    ax0_tmp = _ax0();
    mx_tmp = _mx00();

    _set_mx00(1);
    _set_ax0(src);

    for (i = 0; i < n; i++)
        dest[i] = _x0_u0();

    _set_ax0(ax0_tmp);
    _set_mx00(mx_tmp);
}

void _BurstYMemToSMem(int32_t* dest, const unsigned src, int n)
{
    int i;
    int ay0_tmp;
    int my_tmp;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    ay0_tmp = _ay0();
    my_tmp = _my00();

    _set_my00(1);
    _set_ay0(src);

    for (i = 0; i < n; i++)
        dest[i] = _y0_u0();

    _set_ay0(ay0_tmp);
    _set_my00(my_tmp);
}

void _BurstSMemToXMemPk24(unsigned dest, const int32_t* src, int n)
{
    int i;
    int ay0_tmp;
    int my_tmp;
    unsigned int outData = 0;
    unsigned int inData  = 0;
    int tail;
    const int32_t * pi32In = src;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    ay0_tmp = _ax0();
    my_tmp = _mx00();
    tail = n & 3;

    _set_mx00(1);
    _set_ax0(dest);

    for (i = 0; i < (n/4); i++)
    {
        inData  = *pi32In++;
        outData = _asl(inData, 8);
        _set_x0_u0(outData);

        outData = _lsr(inData, 16);
        inData  = *pi32In++;
        outData = (outData & 0x0000ff00) | _asl(inData, 16);
        _set_x0_u0(outData);

        outData = _lsr(inData, 8);
        inData  = *pi32In++;
        outData = (outData & 0x00ffff00) | _asl(inData, 24);
        _set_x0_u0(outData);

        outData = _and(inData, 0xffffff00);
        _set_x0_u0(outData);
    }

    if (tail == 3){
        inData  = *pi32In++;
        outData = _asl(inData, 8);
        _set_x0_u0(outData);

        outData = _lsr(inData, 16);
        inData  = *pi32In++;
        outData = (outData & 0x0000ff00) | _asl(inData, 16);
        _set_x0_u0(outData);

        outData = _lsr(inData, 8);
        inData  = *pi32In++;
        outData = (outData & 0x00ffff00) | _asl(inData, 24);
        _set_x0_u0(outData);
    }

    if (tail == 2){
        inData  = *pi32In++;
        outData = _asl(inData, 8);
        _set_x0_u0(outData);

        outData = _lsr(inData, 16);
        inData  = *pi32In++;
        outData = (outData & 0x0000ff00) | _asl(inData, 16);
        _set_x0_u0(outData);
    }

    if (tail == 1){
        inData  = *pi32In++;
        outData = _asl(inData, 8);
        _set_x0_u0(outData);
    }

    _set_ax0(ay0_tmp);
    _set_mx00(my_tmp);
}

void _BurstSMemToYMemPk24(unsigned dest, const int32_t* src, int n)
{
    int i;
    int ay0_tmp;
    int my_tmp;
    unsigned int outData = 0;
    unsigned int inData  = 0;
    int tail;
    const int32_t * pi32In = src;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    ay0_tmp = _ay0();
    my_tmp = _my00();
    tail = n & 3;

    _set_my00(1);
    _set_ay0(dest);

    for (i = 0; i < (n/4); i++)
    {
        inData  = *pi32In++;
        outData = _asl(inData, 8);
        _set_y0_u0(outData);

        outData = _lsr(inData, 16);
        inData  = *pi32In++;
        outData = (outData & 0x0000ff00) | _asl(inData, 16);
        _set_y0_u0(outData);

        outData = _lsr(inData, 8);
        inData  = *pi32In++;
        outData = (outData & 0x00ffff00) | _asl(inData, 24);
        _set_y0_u0(outData);

        outData = _and(inData, 0xffffff00);
        _set_y0_u0(outData);
    }

    if (tail == 3){
        inData  = *pi32In++;
        outData = _asl(inData, 8);
        _set_y0_u0(outData);

        outData = _lsr(inData, 16);
        inData  = *pi32In++;
        outData = (outData & 0x0000ff00) | _asl(inData, 16);
        _set_y0_u0(outData);

        outData = _lsr(inData, 8);
        inData  = *pi32In++;
        outData = (outData & 0x00ffff00) | _asl(inData, 24);
        _set_y0_u0(outData);
    }

    if (tail == 2){
        inData  = *pi32In++;
        outData = _asl(inData, 8);
        _set_y0_u0(outData);

        outData = _lsr(inData, 16);
        inData  = *pi32In++;
        outData = (outData & 0x0000ff00) | _asl(inData, 16);
        _set_y0_u0(outData);
    }

    if (tail == 1){
        inData  = *pi32In++;
        outData = _asl(inData, 8);
        _set_y0_u0(outData);
    }

    _set_ay0(ay0_tmp);
    _set_my00(my_tmp);
}

void _BurstXMemToSMemPk24(int32_t* dest, const unsigned src, int n)
{
    int i;
    int ax0_tmp;
    int mx_tmp;
    unsigned int outData = 0;
    unsigned int inData  = 0;
    int tail;
    int32_t * pi32Out = dest;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    ax0_tmp = _ax0();
    mx_tmp = _mx00();
    tail = n & 3;

    _set_mx00(1);
    _set_ax0(src);

    for (i = 0; i < (n>>2); i++)
    {
        inData  = _x0_u0();
        outData = _lsr(inData, 8);

        inData  = _x0_u0();
        outData = outData | _and(_asl(inData, 16), 0xff000000);
        *pi32Out++ = outData;
        outData = _lsr(inData, 16);

        inData  = _x0_u0();
        outData = outData  | _and(_asl(inData, 8), 0xffff0000);
        *pi32Out++ = outData;
        outData = _lsr(inData, 24);

        inData  = _x0_u0();
        outData = outData | _and(_asl(inData, 0), 0xffffff00);
        *pi32Out++ = outData;
    }

    if (tail == 3) {
        inData  = _x0_u0();
        outData = _lsr(inData, 8);

        inData  = _x0_u0();
        outData = outData | _and(_asl(inData, 16), 0xff000000);
        *pi32Out++ = outData;
        outData = _lsr(inData, 16);

        inData  = _x0_u0();
        outData = outData  | _and(_asl(inData, 8), 0xffff0000);
        *pi32Out++ = outData;
        outData = _lsr(inData, 24);
        *pi32Out &= 0xffffff00;
        *pi32Out++ |= outData;
    }

    if (tail == 2){
        inData  = _x0_u0();
        outData = _lsr(inData, 8);

        inData  = _x0_u0();
        outData = outData | _and(_asl(inData, 16), 0xff000000);
        *pi32Out++ = outData;
        outData = _lsr(inData, 16);
        *pi32Out &= 0xffff0000;
        *pi32Out++ |= outData;
    }

    if (tail == 1){
        inData  = _x0_u0();
        outData = _lsr(inData, 8);
        *pi32Out &= 0xff000000;
        *pi32Out++ |= outData;
    }

    _set_ax0(ax0_tmp);
    _set_mx00(mx_tmp);
}

void _BurstYMemToSMemPk24(int32_t* dest, const unsigned src, int n)
{
    int i;
    int ax0_tmp;
    int mx_tmp;
    unsigned int outData = 0;
    unsigned int inData  = 0;
    int tail;
    int32_t * pi32Out = dest;

	ASSERT(n<=XYMEM_XY_SIZE);
	ASSERT(n!=0);
	if(n==0) return;
    ax0_tmp = _ay0();
    mx_tmp = _my00();
    tail = n & 3;

    _set_my00(1);
    _set_ay0(src);

    for (i = 0; i < (n>>2); i++)
    {
        inData  = _y0_u0();
        outData = _lsr(inData, 8);

        inData  = _y0_u0();
        outData = outData | _and(_asl(inData, 16), 0xff000000);
        *pi32Out++ = outData;
        outData = _lsr(inData, 16);

        inData  = _y0_u0();
        outData = outData  | _and(_asl(inData, 8), 0xffff0000);
        *pi32Out++ = outData;
        outData = _lsr(inData, 24);

        inData  = _y0_u0();
        outData = outData | _and(_asl(inData, 0), 0xffffff00);
        *pi32Out++ = outData;
    }

    if (tail == 3) {
        inData  = _y0_u0();
        outData = _lsr(inData, 8);

        inData  = _y0_u0();
        outData = outData | _and(_asl(inData, 16), 0xff000000);
        *pi32Out++ = outData;
        outData = _lsr(inData, 16);

        inData  = _y0_u0();
        outData = outData  | _and(_asl(inData, 8), 0xffff0000);
        *pi32Out++ = outData;
        outData = _lsr(inData, 24);
        *pi32Out &= 0xffffff00;
        *pi32Out++ |= outData;
    }

    if (tail == 2){
        inData  = _y0_u0();
        outData = _lsr(inData, 8);

        inData  = _y0_u0();
        outData = outData | _and(_asl(inData, 16), 0xff000000);
        *pi32Out++ = outData;
        outData = _lsr(inData, 16);
        *pi32Out &= 0xffff0000;
        *pi32Out++ |= outData;
    }

    if (tail == 1){
        inData  = _y0_u0();
        outData = _lsr(inData, 8);
        *pi32Out &= 0xff000000;
        *pi32Out++ |= outData;
    }

    _set_ay0(ax0_tmp);
    _set_my00(mx_tmp);
}


void _WaitTillBurstComplete() 
{
    //
} 

#endif // EMU__Xxy

#endif

// Xdsp extension for ARCv2EM
#if defined(EMU__Xdsp) && defined(_ARCVER_ARCv2EM)

typedef struct
{
    int32_t ACC;    
    int32_t Z;
    int32_t N;
    int32_t C;
    int32_t V;
    int32_t G;
    int8_t GUARD;
} ACC_t; // struct for acc lo and hi half

typedef struct
{
    ACC_t *ACC0_LO;
    ACC_t *ACC0_HI;
} ACC0_t; // struct for acc

static ACC_t ACC0_LO;
static ACC_t ACC0_HI;
static ACC0_t ACC0 = {&ACC0_LO, &ACC0_HI};

typedef struct
{
    DSP_CTRL_ROUNDING_MODE RM;  // Rounding mode
    int32_t       GE;  // Guard bits enable
    int32_t       PA;  // Pre-accum shift
    int32_t       SAT; // Sticky saturation flag
} DSP_CTRL_t;
static DSP_CTRL_t DSP_CTRL; // DSP_CTRL register 

typedef struct
{
    int32_t       SC; // FFT scaling
} DSP_FFT_CTRL_t;
static DSP_FFT_CTRL_t DSP_FFT_CTRL; // DSP_FFT_CTRL register

typedef struct
{
    int16_t       REAL; // Real part
    int16_t       IMAG; // Imaginary part
} DSP_BFLY_t;
static DSP_BFLY_t DSP_BFLY; // DSP_BFLY register

int64_t _get_ACC_H(ACC_t acc){ // Get half accumulator value using guard bits
    int64_t acc64;
    if (DSP_CTRL.GE) // Check for using guard bits
    {
        acc64 = ((int64_t)acc.GUARD) << 32 | (acc.ACC & 0xffffffff); // use guard bits if they are enabled
    } else {
        acc64 = (int64_t) (acc.ACC);
    }
    return acc64;
}


void _set_ACC_H(ACC_t *acc, int64_t acc64){ // set half accumulator value using guard bits    
    if (DSP_CTRL.GE) // Check for using guard bits
    {
        acc->ACC =   (int32_t)(acc64 & 0xffffffff); // use guard bits if they are enabled
        acc->GUARD = (int8_t )(acc64 >> 32);
    } else {
        acc->ACC = (int32_t)acc64;
    }
}

static int64_t Xdsp_round_acc32(ACC_t acc, int32_t p){ // Round ACC0_LO or ACC0_HI
    int64_t acc64;
    acc64 = _get_ACC_H(acc);
    if (p > 0){
        switch (DSP_CTRL.RM)
        {    
        case DSP_CTRL_ROUNDING_UP:
            acc64 = (acc64 + (int64_t)(1 << (p - 1) ));
            break;
        case DSP_CTRL_CONVERGENT:
            acc64 = (acc64 + (int64_t)(1 << (p - 1) ));
            if ((acc64 & ((int64_t)(1 << (p - 1) ) - 1)) == 0) {
                acc64 &= ~(int64_t)(1 << p);
            }
            break;
        case DSP_CTRL_TRUNC:
        default:            
            break;
        }
        acc64 = acc64 & ~(int64_t)((1 << p) - 1); // clear least p bits
    } else {
        acc64 = (int64_t)acc.ACC;
    }
    return acc64;
}
static int32_t Xdsp_sat32(int64_t src, int32_t p, int32_t *s){ // Saturate to 32 bit value
    int32_t sat_value;
    int32_t v = 0;
    if (src > ( 1LL << (p)) - 1){
        sat_value = ( 1LL << (p)) - 1;
        v = 1;
    }else if (src < (-1LL << (p))){
        sat_value = (-1LL << (p));
        v = 1;
    }else{
        sat_value = (int32_t)src;
    }
    *s = v;
    return sat_value;
}


void _set_DSP_CTRL(uint32_t mode){
    DSP_CTRL_ROUNDING_MODE RM;
    switch (mode & 0x3)
    {
    case 2:
        RM = DSP_CTRL_ROUNDING_UP;
        break;
    case 3:
        RM = DSP_CTRL_CONVERGENT;
    default:
    case 0:
    case 1:
        RM = DSP_CTRL_TRUNC;
        break;
    }
    DSP_CTRL.RM  = RM;
    DSP_CTRL.GE  = (mode >>  2) & 1;
    DSP_CTRL.PA  = (mode >>  3) & 1;
    DSP_CTRL.SAT = (mode >> 16) & 1;
}

void _set_DSP_FFT_CTRL(uint32_t mode){
    DSP_FFT_CTRL.SC = mode & 1;
}


int _cbflyhf0r(uint32_t b, uint32_t c){

    int16_t b_h0 = (int16_t) (b & 0xffff);
    int16_t b_h1 = (int16_t) (b >> 16);
    int16_t c_h0 = (int16_t) (c & 0xffff);
    int16_t c_h1 = (int16_t) (c >> 16);
    int32_t a_h0;
    int32_t a_h1;


    int32_t sl, sh;
    _set_ACC_H(&ACC0_HI, Xdsp_sat32(((int64_t)DSP_BFLY.IMAG << 15) + (int64_t)b_h1*c_h0 + (int64_t)b_h0*c_h1, 31, &ACC0_HI.V));
    _set_ACC_H(&ACC0_LO, Xdsp_sat32(((int64_t)DSP_BFLY.REAL << 15) + (int64_t)b_h0*c_h0 - (int64_t)b_h1*c_h1, 31, &ACC0_LO.V));

    if (DSP_FFT_CTRL.SC)
    {
        a_h1 = Xdsp_sat32(Xdsp_round_acc32(ACC0_HI, 16) >> 16, 15, &sh);
        a_h0 = Xdsp_sat32(Xdsp_round_acc32(ACC0_LO, 16) >> 16, 15, &sl);
    } else {
        a_h1 = Xdsp_sat32(Xdsp_round_acc32(ACC0_HI, 15) >> 15, 15, &sh);
        a_h0 = Xdsp_sat32(Xdsp_round_acc32(ACC0_LO, 15) >> 15, 15, &sl);
    }
    DSP_CTRL.SAT |= ACC0_HI.V || ACC0_LO.V || sl || sh;
    return (uint32_t)(a_h1 << 16) | (uint32_t)(a_h0 & 0xffff);
}

int _cbflyhf1r(uint32_t b){
    int16_t b_h0 = (int16_t) (b & 0xffff);
    int16_t b_h1 = (int16_t) (b >> 16);
    int32_t a_h0;
    int32_t a_h1;

    int32_t sl, sh;
    _set_ACC_H(&ACC0_HI,  Xdsp_sat32(((int64_t)DSP_BFLY.IMAG << (15 + 1)) - _get_ACC_H(ACC0_HI), 31, &ACC0_HI.V));
    _set_ACC_H(&ACC0_LO,  Xdsp_sat32(((int64_t)DSP_BFLY.REAL << (15 + 1)) - _get_ACC_H(ACC0_LO), 31, &ACC0_LO.V));
    if (DSP_FFT_CTRL.SC)
    {
        a_h1 = Xdsp_sat32(Xdsp_round_acc32(ACC0_HI, 16) >> 16, 15, &sh);
        a_h0 = Xdsp_sat32(Xdsp_round_acc32(ACC0_LO, 16) >> 16, 15, &sl);
    } else {
        a_h1 = Xdsp_sat32(Xdsp_round_acc32(ACC0_HI, 15) >> 15, 15, &sh);
        a_h0 = Xdsp_sat32(Xdsp_round_acc32(ACC0_LO, 15) >> 15, 15, &sl);
    }
    DSP_BFLY.IMAG = b_h1;
    DSP_BFLY.REAL = b_h0;
    DSP_CTRL.SAT |= ACC0_HI.V || ACC0_LO.V || sl || sh;
    return (uint32_t)(a_h1 << 16) | (uint32_t)(a_h0 & 0xffff);
}

int32_t _dmpyhbl(uint32_t b, uint32_t c) {
    int16_t b_h0 = (int16_t) (b & 0xffff);
    int16_t b_h1 = (int16_t) (b >> 16);
    int8_t c_b0 = (int8_t) (b & 0xff);
    int8_t c_b1 = (int8_t) ((b >> 8) & 0xff);
    int64_t accwide = (int64_t)((int32_t)b_h0 * (int32_t)c_b0 + (int32_t)b_h1 * (int32_t)c_b1);
    if (DSP_CTRL.GE) {            // only if guard bits are enabled
        ACC0_HI.GUARD = (int8_t)(accwide >> 63);
    }
    ACC0_LO.ACC = accwide & 0xffffffff;
    ACC0_HI.ACC = (accwide >> 32) & 0xffffffff;
    return accwide & 0xffffffff;
}

int32_t _dmpyhbm(uint32_t b, uint32_t c) {
    int16_t b_h0 = (int16_t) (b & 0xffff);
    int16_t b_h1 = (int16_t) (b >> 16);
    int8_t c_b2 = (int8_t) ((b >> 16) & 0xff);
    int8_t c_b3 = (int8_t) ((b >> 24) & 0xff);
    int64_t accwide = (int64_t)((int32_t)b_h0 * (int32_t)c_b2 + (int32_t)b_h1 * (int32_t)c_b3);
    if (DSP_CTRL.GE) {            // only if guard bits are enabled
        ACC0_HI.GUARD = (int8_t)(accwide >> 63);
    }
    ACC0_LO.ACC = accwide & 0xffffffff;
    ACC0_HI.ACC = (accwide >> 32) & 0xffffffff;
    return accwide & 0xffffffff;
}

int32_t _dmachbl(uint32_t b, uint32_t c) {
    int16_t b_h0 = (int16_t) (b & 0xffff);
    int16_t b_h1 = (int16_t) (b >> 16);
    int8_t c_b0 = (int8_t) (b & 0xff);
    int8_t c_b1 = (int8_t) ((b >> 8) & 0xff);
    int64_t accwide =   ((int64_t)ACC0_HI.ACC << 32) | (int64_t)((uint32_t)ACC0_LO.ACC);
    accwide += (int64_t)((int32_t)b_h0 * (int32_t)c_b0 + (int32_t)b_h1 * (int32_t)c_b1);
    if (DSP_CTRL.GE) {            // only if guard bits are enabled
        ACC0_HI.GUARD = (int8_t)(accwide >> 63);
    }
    ACC0_LO.ACC = accwide & 0xffffffff;
    ACC0_HI.ACC = (accwide >> 32) & 0xffffffff;
    return accwide & 0xffffffff;
}

int32_t _dmachbm(uint32_t b, uint32_t c) {
    int16_t b_h0 = (int16_t) (b & 0xffff);
    int16_t b_h1 = (int16_t) (b >> 16);
    int8_t c_b2 = (int8_t) ((b >> 16) & 0xff);
    int8_t c_b3 = (int8_t) ((b >> 24) & 0xff);
    int64_t accwide =   ((int64_t)ACC0_HI.ACC << 32) | (int64_t)((uint32_t)ACC0_LO.ACC);
    accwide += (int64_t)((int32_t)b_h0 * (int32_t)c_b2 + (int32_t)b_h1 * (int32_t)c_b3);
    if (DSP_CTRL.GE) {            // only if guard bits are enabled
        ACC0_HI.GUARD = (int8_t)(accwide >> 63);
    }
    ACC0_LO.ACC = accwide & 0xffffffff;
    ACC0_HI.ACC = (accwide >> 32) & 0xffffffff;
    return accwide & 0xffffffff;
}

uint32_t _valgn2h(uint32_t b, uint32_t c) {
    uint32_t a_h1 = (b << 16);   // b_h0 << 16
    uint32_t a_h0 = (c >> 16);   // c_h1 >> 16
    return (a_h1 | a_h0);
}

#endif
