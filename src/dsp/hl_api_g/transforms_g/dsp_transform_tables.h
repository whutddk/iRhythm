#ifndef DSP_MAX_TRANSFORM_SIZE
#define DSP_MAX_TRANSFORM_SIZE 2048
#define DSP_MAX_TRANSFORM_ORDER 11
#endif //DSP_MAX_TRANSFORM_SIZE

#ifndef DSP_MIN_TRANSFORM_SIZE
#define DSP_MIN_TRANSFORM_SIZE 4
#define DSP_MIN_TRANSFORM_ORDER 2
#endif //DSP_MIN_TRANSFORM_SIZE

#if !defined(__Xdsp) || !defined(__Xdsp_complex) || defined(_ARCv2EM_XY_AGU_MEDIUM)
#define USE_RADIX2_Q15_FFT
#endif

#ifdef DSP_SELECTABLE_TRANSFORM_TABLES_SAFE

#define DSP_SELECTABLE_TRANSFORM_TABLES

#define DSP_CFFT_BITREV_4
#define DSP_CFFT_BITREV_8
#define DSP_CFFT_BITREV_16
#define DSP_CFFT_BITREV_32
#define DSP_CFFT_BITREV_64
#define DSP_CFFT_BITREV_128
#define DSP_CFFT_BITREV_256
#define DSP_CFFT_BITREV_512
#define DSP_CFFT_BITREV_1024
#define DSP_CFFT_BITREV_2048
#endif //DSP_SELECTABLE_TRANSFORM_TABLES_SAFE

#ifndef DSP_SELECTABLE_TRANSFORM_TABLES

#define DSP_CFFT_Q31_4
#define DSP_CFFT_F32_4
#define DSP_CFFT_Q15_4
#define DSP_CIFFT_Q15_4
#define DSP_RFFT_Q15_4
#define DSP_RIFFT_Q15_4
#define DSP_DCT23_Q31_4
#define DSP_DCT23_Q15_4
#define DSP_DCT4_Q31_4
#define DSP_DCT4_Q15_4
#define DSP_CFFT_Q31_8
#define DSP_CFFT_F32_8
#define DSP_CFFT_Q15_8
#define DSP_CIFFT_Q15_8
#define DSP_RFFT_Q15_8
#define DSP_RIFFT_Q15_8
#define DSP_DCT23_Q31_8
#define DSP_DCT23_Q15_8
#define DSP_DCT4_Q31_8
#define DSP_DCT4_Q15_8
#define DSP_CFFT_Q31_16
#define DSP_CFFT_F32_16
#define DSP_CFFT_Q15_16
#define DSP_CIFFT_Q15_16
#define DSP_RFFT_Q15_16
#define DSP_RIFFT_Q15_16
#define DSP_DCT23_Q31_16
#define DSP_DCT23_Q15_16
#define DSP_DCT4_Q31_16
#define DSP_DCT4_Q15_16
#define DSP_CFFT_Q31_32
#define DSP_CFFT_F32_32
#define DSP_CFFT_Q15_32
#define DSP_CIFFT_Q15_32
#define DSP_RFFT_Q15_32
#define DSP_RIFFT_Q15_32
#define DSP_DCT23_Q31_32
#define DSP_DCT23_Q15_32
#define DSP_DCT4_Q31_32
#define DSP_DCT4_Q15_32
#define DSP_CFFT_Q31_64
#define DSP_CFFT_F32_64
#define DSP_CFFT_Q15_64
#define DSP_CIFFT_Q15_64
#define DSP_RFFT_Q15_64
#define DSP_RIFFT_Q15_64
#define DSP_DCT23_Q31_64
#define DSP_DCT23_Q15_64
#define DSP_DCT4_Q31_64
#define DSP_DCT4_Q15_64
#define DSP_CFFT_Q31_128
#define DSP_CFFT_F32_128
#define DSP_CFFT_Q15_128
#define DSP_CIFFT_Q15_128
#define DSP_RFFT_Q15_128
#define DSP_RIFFT_Q15_128
#define DSP_DCT23_Q31_128
#define DSP_DCT23_Q15_128
#define DSP_DCT4_Q31_128
#define DSP_DCT4_Q15_128
#define DSP_CFFT_Q31_256
#define DSP_CFFT_F32_256
#define DSP_CFFT_Q15_256
#define DSP_CIFFT_Q15_256
#define DSP_RFFT_Q15_256
#define DSP_RIFFT_Q15_256
#define DSP_DCT23_Q31_256
#define DSP_DCT23_Q15_256
#define DSP_DCT4_Q31_256
#define DSP_DCT4_Q15_256
#define DSP_CFFT_Q31_512
#define DSP_CFFT_F32_512
#define DSP_CFFT_Q15_512
#define DSP_CIFFT_Q15_512
#define DSP_RFFT_Q15_512
#define DSP_RIFFT_Q15_512
#define DSP_DCT23_Q31_512
#define DSP_DCT23_Q15_512
#define DSP_DCT4_Q31_512
#define DSP_DCT4_Q15_512
#define DSP_CFFT_Q31_1024
#define DSP_CFFT_F32_1024
#define DSP_CFFT_Q15_1024
#define DSP_CIFFT_Q15_1024
#define DSP_RFFT_Q15_1024
#define DSP_RIFFT_Q15_1024
#define DSP_DCT23_Q31_1024
#define DSP_DCT23_Q15_1024
#define DSP_DCT4_Q31_1024
#define DSP_DCT4_Q15_1024
#define DSP_CFFT_Q31_2048
#define DSP_CFFT_F32_2048
#define DSP_CFFT_Q15_2048
#define DSP_CIFFT_Q15_2048
#define DSP_RFFT_Q15_2048
#define DSP_RIFFT_Q15_2048
#define DSP_DCT23_Q31_2048
#define DSP_DCT23_Q15_2048
#define DSP_DCT4_Q31_2048
#define DSP_DCT4_Q15_2048
#else //DSP_SELECTABLE_TRANSFORM_TABLES

#if defined(DSP_DCT2_Q31_4) || defined(DSP_DCT3_Q31_4)
#define DSP_DCT23_Q31_4
#endif
#if defined(DSP_DCT2_Q15_4) || defined(DSP_DCT3_Q15_4)
#define DSP_DCT23_Q15_4
#endif
#if defined(DSP_DCT2_Q31_8) || defined(DSP_DCT3_Q31_8)
#define DSP_DCT23_Q31_8
#endif
#if defined(DSP_DCT2_Q15_8) || defined(DSP_DCT3_Q15_8)
#define DSP_DCT23_Q15_8
#endif
#if defined(DSP_DCT2_Q31_16) || defined(DSP_DCT3_Q31_16)
#define DSP_DCT23_Q31_16
#endif
#if defined(DSP_DCT2_Q15_16) || defined(DSP_DCT3_Q15_16)
#define DSP_DCT23_Q15_16
#endif
#if defined(DSP_DCT2_Q31_32) || defined(DSP_DCT3_Q31_32)
#define DSP_DCT23_Q31_32
#endif
#if defined(DSP_DCT2_Q15_32) || defined(DSP_DCT3_Q15_32)
#define DSP_DCT23_Q15_32
#endif
#if defined(DSP_DCT2_Q31_64) || defined(DSP_DCT3_Q31_64)
#define DSP_DCT23_Q31_64
#endif
#if defined(DSP_DCT2_Q15_64) || defined(DSP_DCT3_Q15_64)
#define DSP_DCT23_Q15_64
#endif
#if defined(DSP_DCT2_Q31_128) || defined(DSP_DCT3_Q31_128)
#define DSP_DCT23_Q31_128
#endif
#if defined(DSP_DCT2_Q15_128) || defined(DSP_DCT3_Q15_128)
#define DSP_DCT23_Q15_128
#endif
#if defined(DSP_DCT2_Q31_256) || defined(DSP_DCT3_Q31_256)
#define DSP_DCT23_Q31_256
#endif
#if defined(DSP_DCT2_Q15_256) || defined(DSP_DCT3_Q15_256)
#define DSP_DCT23_Q15_256
#endif
#if defined(DSP_DCT2_Q31_512) || defined(DSP_DCT3_Q31_512)
#define DSP_DCT23_Q31_512
#endif
#if defined(DSP_DCT2_Q15_512) || defined(DSP_DCT3_Q15_512)
#define DSP_DCT23_Q15_512
#endif
#if defined(DSP_DCT2_Q31_1024) || defined(DSP_DCT3_Q31_1024)
#define DSP_DCT23_Q31_1024
#endif
#if defined(DSP_DCT2_Q15_1024) || defined(DSP_DCT3_Q15_1024)
#define DSP_DCT23_Q15_1024
#endif
#if defined(DSP_DCT2_Q31_2048) || defined(DSP_DCT3_Q31_2048)
#define DSP_DCT23_Q31_2048
#endif
#if defined(DSP_DCT2_Q15_2048) || defined(DSP_DCT3_Q15_2048)
#define DSP_DCT23_Q15_2048
#endif
#if defined(DSP_DCT23_Q31_8) || defined(DSP_RFFT_Q31_8) || defined(DSP_RIFFT_Q31_8) || defined(DSP_DCT4_Q31_8) || defined(DSP_MDCT_Q31_16) || defined(DSP_IMDCT_Q31_8)
#define DSP_CFFT_Q31_4
#endif
#if defined(DSP_DCT23_Q15_8) || defined(DSP_RFFT_Q15_8) || defined(DSP_DCT4_Q15_8) || defined(DSP_MDCT_Q15_16) || defined(DSP_IMDCT_Q15_8)
#define DSP_CFFT_Q15_4
#endif
#if defined(DSP_RIFFT_Q15_8)
#define DSP_CIFFT_Q15_4
#endif //defined(DSP_RIFFT_Q15_8)
#if defined(DSP_DCT23_Q31_16) || defined(DSP_RFFT_Q31_16) || defined(DSP_RIFFT_Q31_16) || defined(DSP_DCT4_Q31_16) || defined(DSP_MDCT_Q31_32) || defined(DSP_IMDCT_Q31_16)
#define DSP_CFFT_Q31_8
#endif
#if defined(DSP_DCT23_Q15_16) || defined(DSP_RFFT_Q15_16) || defined(DSP_DCT4_Q15_16) || defined(DSP_MDCT_Q15_32) || defined(DSP_IMDCT_Q15_16)
#define DSP_CFFT_Q15_8
#endif
#if defined(DSP_RIFFT_Q15_16)
#define DSP_CIFFT_Q15_8
#endif //defined(DSP_RIFFT_Q15_16)
#if defined(DSP_DCT23_Q31_32) || defined(DSP_RFFT_Q31_32) || defined(DSP_RIFFT_Q31_32) || defined(DSP_DCT4_Q31_32) || defined(DSP_MDCT_Q31_64) || defined(DSP_IMDCT_Q31_32)
#define DSP_CFFT_Q31_16
#endif
#if defined(DSP_DCT23_Q15_32) || defined(DSP_RFFT_Q15_32) || defined(DSP_DCT4_Q15_32) || defined(DSP_MDCT_Q15_64) || defined(DSP_IMDCT_Q15_32)
#define DSP_CFFT_Q15_16
#endif
#if defined(DSP_RIFFT_Q15_32)
#define DSP_CIFFT_Q15_16
#endif //defined(DSP_RIFFT_Q15_32)
#if defined(DSP_DCT23_Q31_64) || defined(DSP_RFFT_Q31_64) || defined(DSP_RIFFT_Q31_64) || defined(DSP_DCT4_Q31_64) || defined(DSP_MDCT_Q31_128) || defined(DSP_IMDCT_Q31_64)
#define DSP_CFFT_Q31_32
#endif
#if defined(DSP_DCT23_Q15_64) || defined(DSP_RFFT_Q15_64) || defined(DSP_DCT4_Q15_64) || defined(DSP_MDCT_Q15_128) || defined(DSP_IMDCT_Q15_64)
#define DSP_CFFT_Q15_32
#endif
#if defined(DSP_RIFFT_Q15_64)
#define DSP_CIFFT_Q15_32
#endif //defined(DSP_RIFFT_Q15_64)
#if defined(DSP_DCT23_Q31_128) || defined(DSP_RFFT_Q31_128) || defined(DSP_RIFFT_Q31_128) || defined(DSP_DCT4_Q31_128) || defined(DSP_MDCT_Q31_256) || defined(DSP_IMDCT_Q31_128)
#define DSP_CFFT_Q31_64
#endif
#if defined(DSP_DCT23_Q15_128) || defined(DSP_RFFT_Q15_128) || defined(DSP_DCT4_Q15_128) || defined(DSP_MDCT_Q15_256) || defined(DSP_IMDCT_Q15_128)
#define DSP_CFFT_Q15_64
#endif
#if defined(DSP_RIFFT_Q15_128)
#define DSP_CIFFT_Q15_64
#endif //defined(DSP_RIFFT_Q15_128)
#if defined(DSP_DCT23_Q31_256) || defined(DSP_RFFT_Q31_256) || defined(DSP_RIFFT_Q31_256) || defined(DSP_DCT4_Q31_256) || defined(DSP_MDCT_Q31_512) || defined(DSP_IMDCT_Q31_256)
#define DSP_CFFT_Q31_128
#endif
#if defined(DSP_DCT23_Q15_256) || defined(DSP_RFFT_Q15_256) || defined(DSP_DCT4_Q15_256) || defined(DSP_MDCT_Q15_512) || defined(DSP_IMDCT_Q15_256)
#define DSP_CFFT_Q15_128
#endif
#if defined(DSP_RIFFT_Q15_256)
#define DSP_CIFFT_Q15_128
#endif //defined(DSP_RIFFT_Q15_256)
#if defined(DSP_DCT23_Q31_512) || defined(DSP_RFFT_Q31_512) || defined(DSP_RIFFT_Q31_512) || defined(DSP_DCT4_Q31_512) || defined(DSP_MDCT_Q31_1024) || defined(DSP_IMDCT_Q31_512)
#define DSP_CFFT_Q31_256
#endif
#if defined(DSP_DCT23_Q15_512) || defined(DSP_RFFT_Q15_512) || defined(DSP_DCT4_Q15_512) || defined(DSP_MDCT_Q15_1024) || defined(DSP_IMDCT_Q15_512)
#define DSP_CFFT_Q15_256
#endif
#if defined(DSP_RIFFT_Q15_512)
#define DSP_CIFFT_Q15_256
#endif //defined(DSP_RIFFT_Q15_512)
#if defined(DSP_DCT23_Q31_1024) || defined(DSP_RFFT_Q31_1024) || defined(DSP_RIFFT_Q31_1024) || defined(DSP_DCT4_Q31_1024) || defined(DSP_MDCT_Q31_2048) || defined(DSP_IMDCT_Q31_1024)
#define DSP_CFFT_Q31_512
#endif
#if defined(DSP_DCT23_Q15_1024) || defined(DSP_RFFT_Q15_1024) || defined(DSP_DCT4_Q15_1024) || defined(DSP_MDCT_Q15_2048) || defined(DSP_IMDCT_Q15_1024)
#define DSP_CFFT_Q15_512
#endif
#if defined(DSP_RIFFT_Q15_1024)
#define DSP_CIFFT_Q15_512
#endif //defined(DSP_RIFFT_Q15_1024)
#if defined(DSP_DCT23_Q31_2048) || defined(DSP_RFFT_Q31_2048) || defined(DSP_RIFFT_Q31_2048) || defined(DSP_DCT4_Q31_2048) || defined(DSP_MDCT_Q31_4096) || defined(DSP_IMDCT_Q31_2048)
#define DSP_CFFT_Q31_1024
#endif
#if defined(DSP_DCT23_Q15_2048) || defined(DSP_RFFT_Q15_2048) || defined(DSP_DCT4_Q15_2048) || defined(DSP_MDCT_Q15_4096) || defined(DSP_IMDCT_Q15_2048)
#define DSP_CFFT_Q15_1024
#endif
#if defined(DSP_RIFFT_Q15_2048)
#define DSP_CIFFT_Q15_1024
#endif //defined(DSP_RIFFT_Q15_2048)
#if defined(DSP_DCT23_Q15_4) || defined(DSP_RFFT_Q15_4) || defined(DSP_RIFFT_Q15_4)
#define DSP_CIFFT_Q15_4
#endif
#if defined(DSP_DCT23_Q31_4) || defined(DSP_RFFT_Q31_4) || defined(DSP_RIFFT_Q31_4) || defined(DSP_CIFFT_Q31_4)
#define DSP_CFFT_Q31_4
#endif
#if defined(DSP_MDCT_Q31_8) || defined(DSP_IMDCT_Q31_4)
#define DSP_DCT4_Q31_4
#endif
#if defined(DSP_MDCT_Q15_8) || defined(DSP_IMDCT_Q15_4)
#define DSP_DCT4_Q15_4
#endif
#if defined(DSP_DCT23_Q15_8) || defined(DSP_RFFT_Q15_8) || defined(DSP_RIFFT_Q15_8)
#define DSP_CIFFT_Q15_8
#endif
#if defined(DSP_DCT23_Q31_8) || defined(DSP_RFFT_Q31_8) || defined(DSP_RIFFT_Q31_8) || defined(DSP_CIFFT_Q31_8)
#define DSP_CFFT_Q31_8
#endif
#if defined(DSP_MDCT_Q31_16) || defined(DSP_IMDCT_Q31_8)
#define DSP_DCT4_Q31_8
#endif
#if defined(DSP_MDCT_Q15_16) || defined(DSP_IMDCT_Q15_8)
#define DSP_DCT4_Q15_8
#endif
#if defined(DSP_DCT23_Q15_16) || defined(DSP_RFFT_Q15_16) || defined(DSP_RIFFT_Q15_16)
#define DSP_CIFFT_Q15_16
#endif
#if defined(DSP_DCT23_Q31_16) || defined(DSP_RFFT_Q31_16) || defined(DSP_RIFFT_Q31_16) || defined(DSP_CIFFT_Q31_16)
#define DSP_CFFT_Q31_16
#endif
#if defined(DSP_MDCT_Q31_32) || defined(DSP_IMDCT_Q31_16)
#define DSP_DCT4_Q31_16
#endif
#if defined(DSP_MDCT_Q15_32) || defined(DSP_IMDCT_Q15_16)
#define DSP_DCT4_Q15_16
#endif
#if defined(DSP_DCT23_Q15_32) || defined(DSP_RFFT_Q15_32) || defined(DSP_RIFFT_Q15_32)
#define DSP_CIFFT_Q15_32
#endif
#if defined(DSP_DCT23_Q31_32) || defined(DSP_RFFT_Q31_32) || defined(DSP_RIFFT_Q31_32) || defined(DSP_CIFFT_Q31_32)
#define DSP_CFFT_Q31_32
#endif
#if defined(DSP_MDCT_Q31_64) || defined(DSP_IMDCT_Q31_32)
#define DSP_DCT4_Q31_32
#endif
#if defined(DSP_MDCT_Q15_64) || defined(DSP_IMDCT_Q15_32)
#define DSP_DCT4_Q15_32
#endif
#if defined(DSP_DCT23_Q15_64) || defined(DSP_RFFT_Q15_64) || defined(DSP_RIFFT_Q15_64)
#define DSP_CIFFT_Q15_64
#endif
#if defined(DSP_DCT23_Q31_64) || defined(DSP_RFFT_Q31_64) || defined(DSP_RIFFT_Q31_64) || defined(DSP_CIFFT_Q31_64)
#define DSP_CFFT_Q31_64
#endif
#if defined(DSP_MDCT_Q31_128) || defined(DSP_IMDCT_Q31_64)
#define DSP_DCT4_Q31_64
#endif
#if defined(DSP_MDCT_Q15_128) || defined(DSP_IMDCT_Q15_64)
#define DSP_DCT4_Q15_64
#endif
#if defined(DSP_DCT23_Q15_128) || defined(DSP_RFFT_Q15_128) || defined(DSP_RIFFT_Q15_128)
#define DSP_CIFFT_Q15_128
#endif
#if defined(DSP_DCT23_Q31_128) || defined(DSP_RFFT_Q31_128) || defined(DSP_RIFFT_Q31_128) || defined(DSP_CIFFT_Q31_128)
#define DSP_CFFT_Q31_128
#endif
#if defined(DSP_MDCT_Q31_256) || defined(DSP_IMDCT_Q31_128)
#define DSP_DCT4_Q31_128
#endif
#if defined(DSP_MDCT_Q15_256) || defined(DSP_IMDCT_Q15_128)
#define DSP_DCT4_Q15_128
#endif
#if defined(DSP_DCT23_Q15_256) || defined(DSP_RFFT_Q15_256) || defined(DSP_RIFFT_Q15_256)
#define DSP_CIFFT_Q15_256
#endif
#if defined(DSP_DCT23_Q31_256) || defined(DSP_RFFT_Q31_256) || defined(DSP_RIFFT_Q31_256) || defined(DSP_CIFFT_Q31_256)
#define DSP_CFFT_Q31_256
#endif
#if defined(DSP_MDCT_Q31_512) || defined(DSP_IMDCT_Q31_256)
#define DSP_DCT4_Q31_256
#endif
#if defined(DSP_MDCT_Q15_512) || defined(DSP_IMDCT_Q15_256)
#define DSP_DCT4_Q15_256
#endif
#if defined(DSP_DCT23_Q15_512) || defined(DSP_RFFT_Q15_512) || defined(DSP_RIFFT_Q15_512)
#define DSP_CIFFT_Q15_512
#endif
#if defined(DSP_DCT23_Q31_512) || defined(DSP_RFFT_Q31_512) || defined(DSP_RIFFT_Q31_512) || defined(DSP_CIFFT_Q31_512)
#define DSP_CFFT_Q31_512
#endif
#if defined(DSP_MDCT_Q31_1024) || defined(DSP_IMDCT_Q31_512)
#define DSP_DCT4_Q31_512
#endif
#if defined(DSP_MDCT_Q15_1024) || defined(DSP_IMDCT_Q15_512)
#define DSP_DCT4_Q15_512
#endif
#if defined(DSP_DCT23_Q15_1024) || defined(DSP_RFFT_Q15_1024) || defined(DSP_RIFFT_Q15_1024)
#define DSP_CIFFT_Q15_1024
#endif
#if defined(DSP_DCT23_Q31_1024) || defined(DSP_RFFT_Q31_1024) || defined(DSP_RIFFT_Q31_1024) || defined(DSP_CIFFT_Q31_1024)
#define DSP_CFFT_Q31_1024
#endif
#if defined(DSP_MDCT_Q31_2048) || defined(DSP_IMDCT_Q31_1024)
#define DSP_DCT4_Q31_1024
#endif
#if defined(DSP_MDCT_Q15_2048) || defined(DSP_IMDCT_Q15_1024)
#define DSP_DCT4_Q15_1024
#endif
#if defined(DSP_DCT23_Q15_2048) || defined(DSP_RFFT_Q15_2048) || defined(DSP_RIFFT_Q15_2048)
#define DSP_CIFFT_Q15_2048
#endif
#if defined(DSP_DCT23_Q31_2048) || defined(DSP_RFFT_Q31_2048) || defined(DSP_RIFFT_Q31_2048) || defined(DSP_CIFFT_Q31_2048)
#define DSP_CFFT_Q31_2048
#endif
#if defined(DSP_MDCT_Q31_4096) || defined(DSP_IMDCT_Q31_2048)
#define DSP_DCT4_Q31_2048
#endif
#if defined(DSP_MDCT_Q15_4096) || defined(DSP_IMDCT_Q15_2048)
#define DSP_DCT4_Q15_2048
#endif
#endif //DSP_SELECTABLE_TRANSFORM_TABLES

extern const int32_t* dsp_cfft_q31_twd_tab[];
extern const int32_t* dsp_cfft_q31_twd3_tab[];
extern const f32_t* dsp_cfft_f32_twd_tab[];
extern const f32_t* dsp_cfft_f32_twd3_tab[];
extern const int16_t* dsp_cfft_bitrev_tab[];
extern const int16_t* dsp_cfft_q15_twd_tab[];
extern const int16_t* dsp_cifft_q15_twd_tab[];
extern const int16_t* dsp_rfft_q15_twd_tab[];
extern const int16_t* dsp_rifft_q15_twd_tab[];
extern const int32_t* dsp_dct23_q31_twd_tab[];
extern const int16_t* dsp_dct23_q15_twd_tab[];
extern const int32_t* dsp_dct4_q31_prerot_tab[];
extern const int16_t* dsp_dct4_q15_prerot_tab[];
extern const int32_t* dsp_dct4_q31_postrot_tab[];
extern const int16_t* dsp_dct4_q15_postrot_tab[];
