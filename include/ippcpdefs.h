/*************************************************************************
* Copyright (C) 2012 Intel Corporation
*
* Licensed under the Apache License,  Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* 	http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law  or agreed  to  in  writing,  software
* distributed under  the License  is  distributed  on  an  "AS IS"  BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the  specific  language  governing  permissions  and
* limitations under the License.
*************************************************************************/

/*
//
//              Intel® Integrated Performance Primitives Cryptography (Intel® IPP Cryptography)
//
//              Purpose: Basic Types and Macro Definitions
//
*/


#ifndef IPPBASE_H__
#define IPPBASE_H__

#ifdef __cplusplus
extern "C" {
#endif
#if defined (_WIN64)
#define IPPCP_INTEL_LIBS_DIR "lib/"
#elif defined (_WIN32)
#define IPPCP_INTEL_LIBS_DIR "lib32/"
#endif

#if !defined( IPPAPI )

  /* Specify explicit calling convention for public functions */
  #if defined( IPP_W32DLL ) && (defined( _WIN32 ) || defined( _WIN64 ))
    #if defined( _MSC_VER ) || defined( __ICL )
      #define IPPAPI( type,name,arg ) \
                     __declspec(dllimport)   type IPP_CALL name arg;
    #else
      #define IPPAPI( type,name,arg )        type IPP_CALL name arg;
    #endif
  #else
    #define   IPPAPI( type,name,arg )        type IPP_CALL name arg;
  #endif

#endif

/* icc 2021 supports short float data type, icx supports _Float16 data type */
#define _FLOAT_16    2
#define _SHORT_FLOAT 1
#define _NO_FLOAT_16 0
#if defined(__INTEL_LLVM_COMPILER) && defined(__AVX512FP16__)
#    define COMPILER_SUPPORT_SHORT_FLOAT _FLOAT_16
#else
#    if defined(__INTEL_COMPILER)
#        if(__INTEL_COMPILER >= 2021)
#            define COMPILER_SUPPORT_SHORT_FLOAT _SHORT_FLOAT
#        endif
#    endif
#endif
#if !(defined(COMPILER_SUPPORT_SHORT_FLOAT))
#    define COMPILER_SUPPORT_SHORT_FLOAT _NO_FLOAT_16
#endif

#if !defined(_NO_IPP_DEPRECATED)
   #if (defined( __ICL ) || defined( __ECL ) || defined(_MSC_VER)) && !defined( _PCS ) && !defined( _PCS_GENSTUBS )
     #if( __INTEL_COMPILER >= 1100 ) /* icl 11.0 supports additional comment */
       #if( _MSC_VER >= 1400 )
         #define IPP_DEPRECATED( comment ) __declspec( deprecated ( comment ))
       #else
         #pragma message ("your icl version supports additional comment for deprecated functions but it can't be displayed")
         #pragma message ("because internal _MSC_VER macro variable setting requires compatibility with MSVC7.1")
         #pragma message ("use -Qvc8 switch for icl command line to see these additional comments")
         #define IPP_DEPRECATED( comment ) __declspec( deprecated )
       #endif
     #elif( _MSC_FULL_VER >= 140050727 )&&( !defined( __INTEL_COMPILER ))&&( !defined(__INTEL_LLVM_COMPILER)) /* VS2005 supports additional comment */
       #define IPP_DEPRECATED( comment ) __declspec( deprecated ( comment ))
     #elif( _MSC_VER <= 1200 )&&( !defined( __INTEL_COMPILER ))&&( !defined(__INTEL_LLVM_COMPILER)) /* VS 6 doesn't support deprecation */
       #define IPP_DEPRECATED( comment )
     #else
       #define IPP_DEPRECATED( comment ) __declspec( deprecated )
     #endif
   #elif (defined(__ICC) || defined(__ECC) || defined( __GNUC__ )) && !defined( _PCS ) && !defined( _PCS_GENSTUBS )
     #if defined( __GNUC__ )
       #if (__GNUC__ * 100 + __GNUC_MINOR__) >= 405
         #define IPP_DEPRECATED( message ) __attribute__(( deprecated( message )))
       #else
         #define IPP_DEPRECATED( message ) __attribute__(( deprecated ))
       #endif
     #else
       #define IPP_DEPRECATED( comment ) __attribute__(( deprecated ))
     #endif
   #else
     #define IPP_DEPRECATED( comment )
   #endif
#else
   #define IPP_DEPRECATED( comment )
#endif

#if (defined( __ICL ) || defined( __ECL ) || defined(_MSC_VER))
  #if !defined( IPP_NO_DEFAULT_LIB )
    #if  ((defined( _IPP_SEQUENTIAL_DYNAMIC ) && !defined( _IPP_SEQUENTIAL_STATIC )) || \
          (!defined( _IPP_SEQUENTIAL_DYNAMIC ) &&  defined( _IPP_SEQUENTIAL_STATIC )))
    #elif (!defined( _IPP_SEQUENTIAL_DYNAMIC ) && !defined( _IPP_SEQUENTIAL_STATIC ))
      #define IPP_NO_DEFAULT_LIB
    #else
      #error Illegal combination of _IPP_SEQUENTIAL_DYNAMIC/_IPP_SEQUENTIAL_STATIC, only one definition can be defined
    #endif
  #endif
#else
  #define IPP_NO_DEFAULT_LIB
  #if (defined(_IPP_SEQUENTIAL_DYNAMIC) || defined(_IPP_SEQUENTIAL_STATIC))
    #pragma message ("defines _IPP_SEQUENTIAL_DYNAMIC/_IPP_SEQUENTIAL_STATIC do not have any effect in current configuration")
  #endif
#endif


#if defined (_MSC_VER)
  #define IPP_CDECL    __cdecl
#elif (defined (__INTEL_COMPILER) || defined (__INTEL_LLVM_COMPILER) || defined (__GNUC__ ) || defined (__clang__)) && defined (_ARCH_IA32)
  #define IPP_CDECL    __attribute((cdecl))
#else
  #define IPP_CDECL
#endif

#if defined( _WIN32 ) || defined( _WIN64 )
  #define IPP_STDCALL  __stdcall
  #define IPP_CALL     IPP_STDCALL
  #define IPP_INT64    __int64
  #define IPP_UINT64    unsigned __int64
#else
  #define IPP_STDCALL
  #define IPP_CALL     IPP_CDECL
  #define IPP_INT64    long long
  #define IPP_UINT64   unsigned long long
#endif

#define IPP_COUNT_OF( obj )  (sizeof(obj)/sizeof(obj[0]))

#define IPP_PI    ( 3.14159265358979323846 )  /* ANSI C does not support M_PI */
#define IPP_2PI   ( 6.28318530717958647692 )  /* 2*pi                         */
#define IPP_PI2   ( 1.57079632679489661923 )  /* pi/2                         */
#define IPP_PI4   ( 0.78539816339744830961 )  /* pi/4                         */
#define IPP_PI180 ( 0.01745329251994329577 )  /* pi/180                       */
#define IPP_RPI   ( 0.31830988618379067154 )  /* 1/pi                         */
#define IPP_SQRT2 ( 1.41421356237309504880 )  /* sqrt(2)                      */
#define IPP_SQRT3 ( 1.73205080756887729353 )  /* sqrt(3)                      */
#define IPP_LN2   ( 0.69314718055994530942 )  /* ln(2)                        */
#define IPP_LN3   ( 1.09861228866810969139 )  /* ln(3)                        */
#define IPP_E     ( 2.71828182845904523536 )  /* e                            */
#define IPP_RE    ( 0.36787944117144232159 )  /* 1/e                          */
#define IPP_EPS23 ( 1.19209289e-07f )
#define IPP_EPS52 ( 2.2204460492503131e-016 )

#define IPP_MAX_8U     ( 0xFF )
#define IPP_MAX_16U    ( 0xFFFF )
#define IPP_MAX_32U    ( 0xFFFFFFFF )
#define IPP_MIN_8U     ( 0 )
#define IPP_MIN_16U    ( 0 )
#define IPP_MIN_32U    ( 0 )
#define IPP_MIN_8S     (-128 )
#define IPP_MAX_8S     ( 127 )
#define IPP_MIN_16S    (-32768 )
#define IPP_MAX_16S    ( 32767 )
#define IPP_MIN_32S    (-2147483647 - 1 )
#define IPP_MAX_32S    ( 2147483647 )
#define IPP_MIN_64U    ( 0 )

#if defined( _WIN32 ) || defined ( _WIN64 )
  #define IPP_MAX_64S  ( 9223372036854775807i64 )
  #define IPP_MIN_64S  (-9223372036854775807i64 - 1 )
  #define IPP_MAX_64U  ( 0xffffffffffffffffL ) /* 18446744073709551615 */
#else
  #define IPP_MAX_64S  ( 9223372036854775807LL )
  #define IPP_MIN_64S  (-9223372036854775807LL - 1 )
  #define IPP_MAX_64U  ( 0xffffffffffffffffLL ) /* 18446744073709551615 */
#endif

#define IPP_MINABS_32F ( 1.175494351e-38f )
#define IPP_MAXABS_32F ( 3.402823466e+38f )
#define IPP_EPS_32F    ( 1.192092890e-07f )
#define IPP_MINABS_64F ( 2.2250738585072014e-308 )
#define IPP_MAXABS_64F ( 1.7976931348623158e+308 )
#define IPP_EPS_64F    ( 2.2204460492503131e-016 )

#define IPP_MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define IPP_MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )

#define IPP_ABS( a ) ( ((a) < 0) ? (-(a)) : (a) )

typedef struct {
    int          major;         /* e.g. 1                               */
    int          minor;         /* e.g. 2                               */
    int          majorBuild;    /* e.g. 3                               */
    unsigned int revision;      /* e.g. 0xf6f5e5bc                      */
    char         targetCpu[4];  /* corresponding to Intel® processor    */
    const char*  Name;          /* e.g. "ippsw7"                        */
    const char*  Version;       /* e.g. "v1.2 Beta"                     */
    const char*  BuildDate;     /* e.g. "Jul 20 99"                     */
} IppLibraryVersion;

typedef unsigned char  Ipp8u;
typedef unsigned short Ipp16u;
typedef unsigned int   Ipp32u;
typedef signed char    Ipp8s;
typedef signed short   Ipp16s;
typedef signed int     Ipp32s;
typedef float          Ipp32f;
typedef IPP_INT64      Ipp64s;
typedef IPP_UINT64     Ipp64u;
typedef double         Ipp64f;

#if (COMPILER_SUPPORT_SHORT_FLOAT == _FLOAT_16)
    typedef _Float16       Ipp16f;
#endif
#if (COMPILER_SUPPORT_SHORT_FLOAT == _SHORT_FLOAT)
    typedef short float    Ipp16f;
#endif
#if (COMPILER_SUPPORT_SHORT_FLOAT == _NO_FLOAT_16)
    typedef Ipp16s         Ipp16f;
#endif

typedef struct {
    Ipp8s  re;
    Ipp8s  im;
} Ipp8sc;

typedef struct {
    Ipp16s  re;
    Ipp16s  im;
} Ipp16sc;

typedef struct {
    Ipp16u  re;
    Ipp16u  im;
} Ipp16uc;

typedef struct {
    Ipp32s  re;
    Ipp32s  im;
} Ipp32sc;

typedef struct {
    Ipp32f  re;
    Ipp32f  im;
} Ipp32fc;

typedef struct {
    Ipp64s  re;
    Ipp64s  im;
} Ipp64sc;

typedef struct {
    Ipp64f  re;
    Ipp64f  im;
} Ipp64fc;

typedef struct {
    Ipp16f  re;
    Ipp16f  im;
} Ipp16fc;

typedef enum {
   ippUndef = -1,
   ipp1u    =  0,
   ipp8u    =  1,
   ipp8uc   =  2,
   ipp8s    =  3,
   ipp8sc   =  4,
   ipp16u   =  5,
   ipp16uc  =  6,
   ipp16s   =  7,
   ipp16sc  =  8,
   ipp32u   =  9,
   ipp32uc  = 10,
   ipp32s   = 11,
   ipp32sc  = 12,
   ipp32f   = 13,
   ipp32fc  = 14,
   ipp64u   = 15,
   ipp64uc  = 16,
   ipp64s   = 17,
   ipp64sc  = 18,
   ipp64f   = 19,
   ipp64fc  = 20,
   ipp16fc  = 21 /* This is necessary for TS */
} IppDataType;

typedef enum {
    ippFalse = 0,
    ippTrue = 1
} IppBool;

#ifdef __cplusplus
}
#endif

#endif /* IPPBASE_H__ */

#ifndef IPP_CPU_FEATURES__
#define IPP_CPU_FEATURES__

#define   ippCPUID_MMX             0x00000001   /* Intel® architecture with MMX(TM) technology supported                     */
#define   ippCPUID_SSE             0x00000002   /* Intel® Streaming SIMD Extensions (Intel® SSE) instruction set             */
#define   ippCPUID_SSE2            0x00000004   /* Intel® Streaming SIMD Extensions 2 (Intel® SSE2) instruction set          */
#define   ippCPUID_SSE3            0x00000008   /* Intel® Streaming SIMD Extensions 3 (Intel® SSE3) instruction set          */
#define   ippCPUID_SSSE3           0x00000010   /* Supplemental Streaming SIMD Extensions 3 (SSSE3) instruction set          */
#define   ippCPUID_MOVBE           0x00000020   /* Intel® instruction MOVBE                                                  */
#define   ippCPUID_SSE41           0x00000040   /* Intel® Streaming SIMD Extensions 4.1 (Intel® SSE4.1) instruction set      */
#define   ippCPUID_SSE42           0x00000080   /* Intel® Streaming SIMD Extensions 4.2 (Intel® SSE4.2) instruction set      */
#define   ippCPUID_AVX             0x00000100   /* Intel® Advanced Vector Extensions instruction set                         */
#define   ippAVX_ENABLEDBYOS       0x00000200   /* Intel® Advanced Vector Extensions instruction set is supported by OS      */
#define   ippCPUID_AES             0x00000400   /*                                                                           */
#define   ippCPUID_CLMUL           0x00000800   /* Intel® instruction PCLMULQDQ                                              */
#define   ippCPUID_ABR             0x00001000   /* Reserved                                                                  */
#define   ippCPUID_RDRAND          0x00002000   /* Intel® instruction RDRAND                                                 */
#define   ippCPUID_F16C            0x00004000   /* Intel® instruction F16C                                                   */
#define   ippCPUID_AVX2            0x00008000   /* Intel® Advanced Vector Extensions 2                                       */
#define   ippCPUID_ADCOX           0x00010000   /* Intel® instructions ADOX/ADCX                                             */
#define   ippCPUID_RDSEED          0x00020000   /* Intel® instruction RDSEED                                                 */
#define   ippCPUID_PREFETCHW       0x00040000   /* Intel® instruction PREFETCHW                                              */
#define   ippCPUID_SHA             0x00080000   /* Intel® Secure Hash Algorithm Extensions                                   */
#define   ippCPUID_AVX512F         0x00100000   /* Intel® Advanced Vector Extensions 512 Foundation instruction set          */
#define   ippCPUID_AVX512CD        0x00200000   /* Intel® Advanced Vector Extensions 512 CD instruction set                  */
#define   ippCPUID_AVX512ER        0x00400000   /* Intel® Advanced Vector Extensions 512 ER instruction set                  */
#define   ippCPUID_AVX512PF        0x00800000   /* Intel® Advanced Vector Extensions 512 PF instruction set                  */
#define   ippCPUID_AVX512BW        0x01000000   /* Intel® Advanced Vector Extensions 512 BW instruction set                  */
#define   ippCPUID_AVX512DQ        0x02000000   /* Intel® Advanced Vector Extensions 512 DQ instruction set                  */
#define   ippCPUID_AVX512VL        0x04000000   /* Intel® Advanced Vector Extensions 512 VL instruction set                  */
#define   ippCPUID_AVX512VBMI      0x08000000   /* Intel® Advanced Vector Extensions 512 Bit Manipulation instructions       */
#define   ippCPUID_MPX             0x10000000   /* Intel® Memory Protection Extensions                                       */
#define   ippCPUID_AVX512_4FMADDPS 0x20000000   /* Intel® Advanced Vector Extensions 512 DL floating-point single precision  */
#define   ippCPUID_AVX512_4VNNIW   0x40000000   /* Intel® Advanced Vector Extensions 512 DL enhanced word variable precision */
#define   ippCPUID_KNC             0x80000000   /* Intel® Xeon Phi(TM) coprocessor                                           */
#if defined( _WIN32 ) || defined ( _WIN64 )
  #define INT64_SUFFIX(name) name##L
#else
  #define INT64_SUFFIX(name) name##LL
#endif
 #define   ippCPUID_AVX512IFMA      INT64_SUFFIX(0x100000000)        /* Intel® Advanced Vector Extensions 512 IFMA (PMADD52) instruction set             */
 #define   ippCPUID_NOCHECK         INT64_SUFFIX(0x8000000000000000) /* Force ippSetCpuFeatures to set CPU features without check                        */
 #define   ippCPUID_GETINFO_A       INT64_SUFFIX(0x616f666e69746567) /* Force ippGetCpuFeatures to work as cpuid instruction                             */
 #define   ippAVX512_ENABLEDBYOS    INT64_SUFFIX(0x200000000)        /* Intel® Advanced Vector Extensions 512 is supported by OS                         */

 #define   ippCPUID_AVX512GFNI   INT64_SUFFIX(0x400000000)        /*                                                                                      */
 #define   ippCPUID_AVX512VAES   INT64_SUFFIX(0x800000000)        /*                                                                                      */
 #define   ippCPUID_AVX512VCLMUL INT64_SUFFIX(0x1000000000)       /*                                                                                      */
 #define   ippCPUID_AVX512VBMI2  INT64_SUFFIX(0x2000000000)       /* Intel® Advanced Vector Extensions 512 Bit Manipulation instructions 2                */
 #define   ippCPUID_AVX512_FP16  INT64_SUFFIX(0x1000000000)       /* Intel(R) Advanced Vector Extensions 512 16-bit floating point (FP16) instruction set */

 #define   ippCPUID_AVX2VAES    INT64_SUFFIX(0x4000000000)        /* Intel® Advanced Vector Extensions 256 Bit Vector AES instructions                    */
 #define   ippCPUID_AVX2VCLMUL  INT64_SUFFIX(0x8000000000)        /* Intel® instruction VPCLMULQDQ                                                        */
 #define   ippCPUID_AVX2SM3     INT64_SUFFIX(0x10000000000)       /* Intel® Advanced Vector Extensions 256 Bit SM3_NI instructions                        */
 #define   ippCPUID_AVX2SM4     INT64_SUFFIX(0x20000000000)       /* Intel® Advanced Vector Extensions 256 Bit SM4_NI instructions                        */
 #define   ippCPUID_AVX2SHA512  INT64_SUFFIX(0x40000000000)       /* Intel® Advanced Vector Extensions 256 Bit SHA512_NI instructions                     */
#endif /* IPP_CPU_FEATURES__ */

/* Macros are necessary to build custom Intel® IPP Cryptography static 1cpu library (enable specific features at compile-time) */
#if (!defined(_MERGED_BLD) && defined(IPPCP_CUSTOM_BUILD))

#ifndef IPP_CUSTOM_CPU_FEATURES__
#define IPP_CUSTOM_CPU_FEATURES__

#ifndef IPPCP_AES_ON
#define IPPCP_AES_ON (0)
#endif
#ifndef IPPCP_CLMUL_ON
#define IPPCP_CLMUL_ON (0)
#endif
#ifndef IPPCP_VAES_ON
#define IPPCP_VAES_ON (0)
#endif
#ifndef IPPCP_VCLMUL_ON
#define IPPCP_VCLMUL_ON (0)
#endif
#define IPP_CUSTOM_ENABLED_FEATURES (ippCPUID_AES*IPPCP_AES_ON | ippCPUID_CLMUL*IPPCP_CLMUL_ON | ippCPUID_AVX512VAES*IPPCP_VAES_ON | ippCPUID_AVX512VCLMUL*IPPCP_VCLMUL_ON \
                                      | ippCPUID_AVX2VAES*IPPCP_VAES_ON | ippCPUID_AVX2VCLMUL*IPPCP_VCLMUL_ON)

#endif /* IPP_CUSTOM_CPU_FEATURES__ */
#endif /* !defined(_MERGED_BLD) && defined(IPPCP_CUSTOM_BUILD) */

#ifndef IPPSTATUS_H__
#define IPPSTATUS_H__

#ifdef __cplusplus
extern "C" {
#endif
typedef signed int IppStatus;

    /* start of common with ippCrypto part - any changes MUST be done in both repositories - IPP & ippCrypto                    */
#define ippStsCpuNotSupportedErr         -9999 /* The target CPU is not supported.                                              */
#define ippStsUnknownStatusCodeErr        -216 /* Unknown status code.                                                          */
#define ippStsLoadDynErr                  -221 /* Error when loading the dynamic library.                                       */
#define ippStsLengthErr                    -15 /* Incorrect value for string length.                                            */
#define ippStsNotSupportedModeErr          -14 /* The requested mode is currently not supported.                                */
#define ippStsContextMatchErr              -13 /* Context parameter does not match the operation.                               */
#define ippStsScaleRangeErr                -12 /* Scale bounds are out of range.                                                */
#define ippStsOutOfRangeErr                -11 /* Argument is out of range, or point is outside the image.                      */
#define ippStsDivByZeroErr                 -10 /* An attempt to divide by zero.                                                 */
#define ippStsMemAllocErr                   -9 /* Memory allocated for the operation is not enough.                             */
#define ippStsNullPtrErr                    -8 /* Null pointer error.                                                           */
#define ippStsRangeErr                      -7 /* Incorrect values for bounds: the lower bound is greater than the upper bound. */
#define ippStsSizeErr                       -6 /* Incorrect value for data size.                                                */
#define ippStsBadArgErr                     -5 /* Incorrect arg/param of the function.                                          */
#define ippStsNoMemErr                      -4 /* Not enough memory for the operation.                                          */
#define ippStsErr                           -2 /* Unknown/unspecified error                                                     */
     /* no errors */
#define ippStsNoErr                          0 /* No errors. */
     /* warnings  */
#define ippStsNoOperation                    1 /* No operation has been executed. */
#define ippStsDivByZero                      2 /* Zero value(s) for the divisor in the Div function. */
#define ippStsWaterfall                     43 /* Cannot load required library, waterfall is used. */
#define ippStsFeaturesCombination           51 /* Wrong combination of features. */
    /* end of common with ippCrypto part */

#ifdef __cplusplus
}
#endif

#endif /* IPPSTATUS_H__ */

     /* ippCrypto specific statuses - any changes MUST be done in both repositories - IPP & ippCrypto */
#define ippStsInvalidPoint               -1017 /* ippStsInvalidPoint ECC: Invalid point (out of EC).*/
#define ippStsQuadraticNonResidueErr     -1016 /* SQRT operation on quadratic non-residue value. */
#define ippStsPointAtInfinity            -1015 /* Point at infinity is detected. */
#define ippStsOFBSizeErr                 -1014 /* Incorrect value for crypto OFB block size. */
#define ippStsIncompleteContextErr       -1013 /* Crypto: set up of context is not complete. */
#define ippStsCTRSizeErr                 -1012 /* Incorrect value for crypto CTR block size. */
#define ippStsEphemeralKeyErr            -1011 /* ECC: Invalid ephemeral key. */
#define ippStsMessageErr                 -1010 /* ECC: Invalid message digest. */
#define ippStsShareKeyErr                -1009 /* ECC: Invalid share key. */
#define ippStsInvalidPrivateKey          -1008 /* ECC: Invalid private key. */
#define ippStsOutOfECErr                 -1007 /* ECC: Point out of EC. */
#define ippStsECCInvalidFlagErr          -1006 /* ECC: Invalid Flag. */
#define ippStsUnderRunErr                -1005 /* Error in data under run. */
#define ippStsPaddingErr                 -1004 /* Detected padding error indicates the possible data corruption. */
#define ippStsCFBSizeErr                 -1003 /* Incorrect value for crypto CFB block size. */
#define ippStsPaddingSchemeErr           -1002 /* Invalid padding scheme. */
#define ippStsBadModulusErr              -1001 /* Bad modulus caused a failure in module inversion. */
#define ippStsInsufficientEntropy           25 /* Generation of the prime/key failed due to insufficient entropy in the random seed and stimulus bit string. */
#define ippStsNotSupportedCpu               36 /* The CPU is not supported. */
#define ippStsMbWarning                     53 /* Error(s) in statuses array. */
     /* end of ippCrypto specific statuses - any changes MUST be done in both repositories - IPP & ippCrypto */

#if (!defined IPPCPDEFS_H__) || defined( _OWN_BLDPCS )
#define IPPCPDEFS_H__

#ifdef __cplusplus
extern "C" {
#endif



#if !defined( _OWN_BLDPCS )

typedef Ipp32u IppAlgId;

/*
// =========================================================
// Symmetric Ciphers
// =========================================================
*/
typedef enum {
   ippPaddingNONE  = 0, /*NONE  = 0,*/ IppsCPPaddingNONE  = 0,
   ippPaddingPKCS7 = 1, /*PKCS7 = 1,*/ IppsCPPaddingPKCS7 = 1,
   ippPaddingZEROS = 2, /*ZEROS = 2,*/ IppsCPPaddingZEROS = 2
} IppsPadding, IppsCPPadding;

typedef struct _cpDES         IppsDESSpec;
typedef struct _cpRijndael128 IppsAESSpec;
typedef struct _cpRijndael128 IppsRijndael128Spec;
typedef struct _cpSMS4        IppsSMS4Spec;

/* TDES */
#define  DES_BLOCKSIZE  (64)  /* cipher blocksize (bits) */
#define TDES_BLOCKSIZE  DES_BLOCKSIZE

#define  DES_KEYSIZE    (64) /*     cipher keysize (bits) */
#define TDES_KEYSIZE    DES_KEYSIZE

/* AES */
#define IPP_AES_BLOCK_BITSIZE (128)    /* cipher blocksizes (bits) */

/* Rijndael */
typedef enum {
   ippRijndaelKey128 = 128, IppsRijndaelKey128 = 128, /* 128-bit key */
   ippRijndaelKey192 = 192, IppsRijndaelKey192 = 192, /* 192-bit key */
   ippRijndaelKey256 = 256, IppsRijndaelKey256 = 256  /* 256-bit key */
} IppsRijndaelKeyLength;

/* AES-CCM (authentication & confidence) */
typedef struct _cpAES_CCM        IppsAES_CCMState;
/* AES-GCM (authentication & confidence) */
typedef struct _cpAES_GCM        IppsAES_GCMState;
/* AES-XTS (confidence) */
typedef struct _cpAES_XTS        IppsAES_XTSSpec;

/* SMS4-CCM (authentication & confidence) */
typedef struct _cpSMS4_CCM       IppsSMS4_CCMState;

/*
// =========================================================
// ARCFOUR Stream Cipher
// =========================================================
*/
typedef struct _cpARCfour  IppsARCFourState;

#define IPP_ARCFOUR_KEYMAX_SIZE  (256)  /* max key length (bytes) */
#define MAX_ARCFOUR_KEY_LEN   IPP_ARCFOUR_KEYMAX_SIZE /* obsolete */

/*
// =========================================================
// One-Way Hash Functions
// =========================================================
*/
typedef enum {
   ippHashAlg_Unknown,
   ippHashAlg_SHA1,
   ippHashAlg_SHA256,
   ippHashAlg_SHA224,
   ippHashAlg_SHA512,
   ippHashAlg_SHA384,
   ippHashAlg_MD5,
   ippHashAlg_SM3,
   ippHashAlg_SHA512_224,
   ippHashAlg_SHA512_256,
   ippHashAlg_MaxNo
} IppHashAlgId;

#define IPP_ALG_HASH_UNKNOWN     (ippHashAlg_Unknown) /* unknown  */
#define IPP_ALG_HASH_SHA1        (ippHashAlg_SHA1)    /* SHA1     */
#define IPP_ALG_HASH_SHA256      (ippHashAlg_SHA256)  /* SHA256   */
#define IPP_ALG_HASH_SHA224      (ippHashAlg_SHA224)  /* SHA224 or SHA256/224 */
#define IPP_ALG_HASH_SHA512      (ippHashAlg_SHA512)  /* SHA512   */
#define IPP_ALG_HASH_SHA384      (ippHashAlg_SHA384)  /* SHA384 or SHA512/384 */
#define IPP_ALG_HASH_MD5         (ippHashAlg_MD5)     /* MD5      */
#define IPP_ALG_HASH_SM3         (ippHashAlg_SM3)     /* SM3      */
#define IPP_ALG_HASH_SHA512_224  (ippHashAlg_SHA512_224) /* SHA512/224 */
#define IPP_ALG_HASH_SHA512_256  (ippHashAlg_SHA512_256) /* SHA512/256 */
#define IPP_ALG_HASH_LIMIT       (ippHashAlg_MaxNo)   /* hash alg limiter*/

typedef struct _cpSHA1     IppsSHA1State;
typedef struct _cpSHA256   IppsSHA256State;
typedef struct _cpSHA256   IppsSHA224State;
typedef struct _cpSHA512   IppsSHA512State;
typedef struct _cpSHA512   IppsSHA384State;
typedef struct _cpMD5      IppsMD5State;
typedef struct _cpSM3      IppsSM3State;
typedef struct _cpHashCtx  IppsHashState;

typedef struct _cpHashMethod_rmf IppsHashMethod;
typedef struct _cpHashCtx_rmf    IppsHashState_rmf;

#define   IPP_SHA1_DIGEST_BITSIZE  160   /* digest size (bits) */
#define IPP_SHA256_DIGEST_BITSIZE  256
#define IPP_SHA224_DIGEST_BITSIZE  224
#define IPP_SHA384_DIGEST_BITSIZE  384
#define IPP_SHA512_DIGEST_BITSIZE  512
#define    IPP_MD5_DIGEST_BITSIZE  128
#define    IPP_SM3_DIGEST_BITSIZE  256
#define IPP_SHA512_224_DIGEST_BITSIZE  224
#define IPP_SHA512_256_DIGEST_BITSIZE  256

/*
// =========================================================
// Keyed-Hash Message Authentication Codes
// =========================================================
*/
typedef struct _cpHMAC  IppsHMACState;
typedef struct _cpHMAC  IppsHMACSHA1State;
typedef struct _cpHMAC  IppsHMACSHA256State;
typedef struct _cpHMAC  IppsHMACSHA224State;
typedef struct _cpHMAC  IppsHMACSHA384State;
typedef struct _cpHMAC  IppsHMACSHA512State;
typedef struct _cpHMAC  IppsHMACMD5State;
typedef struct _cpHMAC_rmf       IppsHMACState_rmf;

/*
// =========================================================
// Data Authentication Codes
// =========================================================
*/
typedef struct _cpAES_CMAC          IppsAES_CMACState;

/*
// =========================================================
// Big Number Integer Arithmetic
// =========================================================
*/
#define BN_MAXBITSIZE      (16*1024)   /* bn max size (bits) */


typedef enum {
   ippBigNumNEG = 0, IppsBigNumNEG = 0,
   ippBigNumPOS = 1, IppsBigNumPOS = 1
} IppsBigNumSGN;

typedef enum {
   ippBinaryMethod   = 0, IppsBinaryMethod = 0,
   ippSlidingWindows = 1, IppsSlidingWindows = 1
} IppsExpMethod;

typedef struct _cpBigNum      IppsBigNumState;
typedef struct _cpMontgomery  IppsMontState;
typedef struct _cpPRNG        IppsPRNGState;
typedef struct _cpPrime       IppsPrimeState;

/*  External Bit Supplier */
typedef IppStatus (IPP_CALL *IppBitSupplier)(Ipp32u* pRand, int nBits, void* pEbsParams);

#define IPP_IS_EQ (0)
#define IPP_IS_GT (1)
#define IPP_IS_LT (2)
#define IPP_IS_NE (3)
#define IPP_IS_NA (4)

#define IPP_IS_PRIME       (5)
#define IPP_IS_COMPOSITE   (6)

#define IPP_IS_VALID       (7)
#define IPP_IS_INVALID     (8)
#define IPP_IS_INCOMPLETE  (9)
#define IPP_IS_ATINFINITY  (10)

#define IS_ZERO            IPP_IS_EQ
#define GREATER_THAN_ZERO  IPP_IS_GT
#define LESS_THAN_ZERO     IPP_IS_LT
#define IS_PRIME           IPP_IS_PRIME
#define IS_COMPOSITE       IPP_IS_COMPOSITE
#define IS_VALID_KEY       IPP_IS_VALID
#define IS_INVALID_KEY     IPP_IS_INVALID
#define IS_INCOMPLETED_KEY IPP_IS_INCOMPLETE

/*
// =========================================================
// RSA Cryptography
// =========================================================
*/
#define MIN_RSA_SIZE (8)
#define MAX_RSA_SIZE (16*1024)

typedef struct _cpRSA IppsRSAState;

/* key types */
typedef enum {
   ippRSApublic  = 0x20000000, IppRSApublic  = 0x20000000,
   ippRSAprivate = 0x40000000, IppRSAprivate = 0x40000000
} IppRSAKeyType;

/* key component's tag */
typedef enum {
   ippRSAkeyN    = 0x01,  IppRSAkeyN    = 0x01,
   ippRSAkeyE    = 0x02,  IppRSAkeyE    = 0x02,
   ippRSAkeyD    = 0x04,  IppRSAkeyD    = 0x04,
   ippRSAkeyP    = 0x08,  IppRSAkeyP    = 0x08,
   ippRSAkeyQ    = 0x10,  IppRSAkeyQ    = 0x10,
   ippRSAkeyDp   = 0x20,  IppRSAkeyDp   = 0x20,
   ippRSAkeyDq   = 0x40,  IppRSAkeyDq   = 0x40,
   ippRSAkeyQinv = 0x80,  IppRSAkeyQinv = 0x80
} IppRSAKeyTag;

typedef struct _cpRSA_public_key   IppsRSAPublicKeyState;
typedef struct _cpRSA_private_key  IppsRSAPrivateKeyState;


/*
// =========================================================
// DL Cryptography
// =========================================================
*/
#define MIN_DLP_BITSIZE      (512)
#define MIN_DLP_BITSIZER     (160)

#define MIN_DLPDH_BITSIZE    (512)
#define MIN_DLPDH_BITSIZER   (160)
#define DEF_DLPDH_BITSIZER   (160)

#define MIN_DLPDSA_BITSIZE   (512)
#define MAX_DLPDSA_BITSIZE  (1024)
#define MIN_DLPDSA_BITSIZER  (160)
#define DEF_DLPDSA_BITSIZER  (160)
#define MAX_DLPDSA_BITSIZER  (160)
#define MIN_DLPDSA_SEEDSIZE  (160)

typedef struct _cpDLP IppsDLPState;

/* domain parameter tags */
typedef enum {
   ippDLPkeyP = 0x01, IppDLPkeyP = 0x01,
   ippDLPkeyR = 0x02, IppDLPkeyR = 0x02,
   ippDLPkeyG = 0x04, IppDLPkeyG = 0x04
} IppDLPKeyTag;

typedef enum {
   ippDLValid,                /* validation pass successfully  */

   ippDLBaseIsEven,           /* !(P is odd)                   */
   ippDLOrderIsEven,          /* !(R is odd)                   */
   ippDLInvalidBaseRange,     /* !(2^(L-1) < P < 2^L)          */
   ippDLInvalidOrderRange,    /* !(2^(M-1) < R < 2^M)          */
   ippDLCompositeBase,
   ippDLCompositeOrder,
   ippDLInvalidCofactor,      /* !( R|(P-1) )                  */
   ippDLInvalidGenerator,     /* !( G^R == 1 (mod P) )         */
                              /* !(1 < G < (P-1))              */
   ippDLInvalidPrivateKey,    /* !(1 < private < (R-1))        */
   ippDLInvalidPublicKey,     /* !(1 < public  <=(P-1))        */
   ippDLInvalidKeyPair,       /* !(G^private == public         */

   ippDLInvalidSignature       /* invalid signature            */
} IppDLResult;

/*
// =========================================================
// EC Cryptography
// =========================================================
*/
#define EC_GFP_MAXBITSIZE   (1024)

/* operation result */
typedef enum {
   ippECValid,             /* validation pass successfully                                 */

   ippECCompositeBase,     /* field based on composite                                     */
   ippECComplicatedBase,   /* number of non-zero terms in the polynomial (> PRIME_ARR_MAX) */
   ippECIsZeroDiscriminant,/* zero discriminant                                            */
   ippECCompositeOrder,    /* composite order of base point                                */
   ippECInvalidOrder,      /* invalid base point order                                     */
   ippECIsWeakMOV,         /* weak Meneze-Okamoto-Vanstone  reduction attack               */
   ippECIsWeakSSSA,        /* weak Semaev-Smart,Satoh-Araki reduction attack               */
   ippECIsSupersingular,   /* supersingular curve                                          */

   ippECInvalidPrivateKey, /* !(0 < Private < order)                                       */
   ippECInvalidPublicKey,  /* (order*PublicKey != Infinity)                                */
   ippECInvalidKeyPair,    /* (Private*BasePoint != PublicKey)                             */

   ippECPointOutOfGroup,   /* out of group (order*P != Infinity)                           */
   ippECPointIsAtInfinite, /* point (P=(Px,Py)) at Infinity                                */
   ippECPointIsNotValid,   /* point (P=(Px,Py)) out-of EC                                  */

   ippECPointIsEqual,      /* compared points are equal                                    */
   ippECPointIsNotEqual,   /* compared points are different                                */

   ippECInvalidSignature   /* invalid signature                                            */
} IppECResult;

/* domain parameter set/get flags */
typedef enum {
   ippECarbitrary =0x00000,        IppECCArbitrary = 0x00000,       /* arbitrary ECC */

   ippECPstd      = 0x10000,       IppECCPStd      = 0x10000,       /* random (recommended) EC over FG(p): */
   ippECPstd112r1 = ippECPstd,     IppECCPStd112r1 = IppECCPStd,    /* secp112r1 curve */
   ippECPstd112r2 = ippECPstd+1,   IppECCPStd112r2 = IppECCPStd+1,  /* secp112r2 curve */
   ippECPstd128r1 = ippECPstd+2,   IppECCPStd128r1 = IppECCPStd+2,  /* secp128r1 curve */
   ippECPstd128r2 = ippECPstd+3,   IppECCPStd128r2 = IppECCPStd+3,  /* secp128r2 curve */
   ippECPstd160r1 = ippECPstd+4,   IppECCPStd160r1 = IppECCPStd+4,  /* secp160r1 curve */
   ippECPstd160r2 = ippECPstd+5,   IppECCPStd160r2 = IppECCPStd+5,  /* secp160r2 curve */
   ippECPstd192r1 = ippECPstd+6,   IppECCPStd192r1 = IppECCPStd+6,  /* secp192r1 curve */
   ippECPstd224r1 = ippECPstd+7,   IppECCPStd224r1 = IppECCPStd+7,  /* secp224r1 curve */
   ippECPstd256r1 = ippECPstd+8,   IppECCPStd256r1 = IppECCPStd+8,  /* secp256r1 curve */
   ippECPstd384r1 = ippECPstd+9,   IppECCPStd384r1 = IppECCPStd+9,  /* secp384r1 curve */
   ippECPstd521r1 = ippECPstd+10,  IppECCPStd521r1 = IppECCPStd+10, /* secp521r1 curve */
   ippECPstdSM2   = ippECPstd+11,  IppECCPStdSM2   = IppECCPStd+11, /* TMP SM2   curve */
   ippEC_TPM_SM2_P256= ippECPstd+11,
   ippEC_TPM_BN_P256 = ippECPstd+12,                                /* TPM BN_P256 curve */

   /* curves over binary finit fields are not supported in Intel® IPP 9.0 */
   IppECCBStd      = 0x20000,       /* random (recommended) EC over FG(2^m): */
   IppECCBStd113r1 = IppECCBStd,    /* sect113r1 curve */
   IppECCBStd113r2 = IppECCBStd+1,  /* sect113r2 curve */
   IppECCBStd131r1 = IppECCBStd+2,  /* sect131r1 curve */
   IppECCBStd131r2 = IppECCBStd+3,  /* sect131r2 curve */
   IppECCBStd163r1 = IppECCBStd+4,  /* sect163r1 curve */
   IppECCBStd163r2 = IppECCBStd+5,  /* sect163r2 curve */
   IppECCBStd193r1 = IppECCBStd+6,  /* sect193r1 curve */
   IppECCBStd193r2 = IppECCBStd+7,  /* sect193r2 curve */
   IppECCBStd233r1 = IppECCBStd+8,  /* sect233r1 curve */
   IppECCBStd283r1 = IppECCBStd+9,  /* sect283r1 curve */
   IppECCBStd409r1 = IppECCBStd+10, /* sect409r1 curve */
   IppECCBStd571r1 = IppECCBStd+11, /* sect571r1 curve */

   IppECCKStd      = 0x40000,       /* Koblitz (recommended) EC over FG(2^m): */
   IppECCBStd163k1 = IppECCKStd,    /* Koblitz 163 curve */
   IppECCBStd233k1 = IppECCKStd+1,  /* Koblitz 233 curve */
   IppECCBStd239k1 = IppECCKStd+2,  /* Koblitz 239 curve */
   IppECCBStd283k1 = IppECCKStd+3,  /* Koblitz 283 curve */
   IppECCBStd409k1 = IppECCKStd+4,  /* Koblitz 409 curve */
   IppECCBStd571k1 = IppECCKStd+5   /* Koblitz 571 curve */
} IppsECType, IppECCType;

/*
// GF over prime and its extension
*/
#define IPP_MIN_GF_CHAR         (3)  /* min characteristic of GF  */

#define IPP_MIN_GF_BITSIZE      (2)  /* min bitsize of element over prime GF */
#define IPP_MAX_GF_BITSIZE   (1024)  /* max bitsize of element over prime GF */

#define IPP_MIN_GF_EXTDEG       (2)  /* min GF extension degree */
#define IPP_MAX_GF_EXTDEG       (8)  /* max GF extension degree */

#define IPP_MAX_EXPONENT_NUM    (6)  /* max number of exponents, equals to LOG_CACHE_LINE_SIZE */

typedef struct _cpGFpMethod   IppsGFpMethod;

typedef struct _cpGFp         IppsGFpState;
typedef struct _cpGFpElement  IppsGFpElement;

typedef struct _cpGFpEC       IppsGFpECState;
typedef struct _cpGFpECPoint  IppsGFpECPoint;

typedef struct _cpGFpEC       IppsECCPState;
typedef struct _cpGFpECPoint  IppsECCPPointState;

typedef struct {
   int   hashSize;
   int   msgBlockSize;
} IppsHashInfo;

typedef struct {
 //const IppsGFpState* pBasicGF;
 //const IppsGFpState* pGroundGF;
   int   parentGFdegree;
   int   basicGFdegree;
   int   basicElmBitSize;
} IppsGFpInfo;

/* SM3 Digest Bytes Size */
#define IPP_SM3_DIGEST_BYTESIZE ((IPP_SM3_DIGEST_BITSIZE + 7) / 8)

typedef struct _cpStateECES_SM2 IppsECESState_SM2;

typedef enum {
   ippKESM2Requester = 0xF, /* corresponds to A user/participant */
   ippKESM2Responder        /* corresponds to B user/participant */
} IppsKeyExchangeRoleSM2;

typedef struct _GFpECKeyExchangeSM2 IppsGFpECKeyExchangeSM2State;

#endif /* !defined( _OWN_BLDPCS ) */

IPPAPI( IppStatus, ippcpGetCpuFeatures, ( Ipp64u* pFeaturesMask ))
IPPAPI( IppStatus, ippcpSetCpuFeatures, ( Ipp64u features ))
IPPAPI( Ipp64u, ippcpGetEnabledCpuFeatures, ( void ) )
IPPAPI( IppStatus, ippcpSetNumThreads, ( int numThr ))
IPPAPI( IppStatus, ippcpInit,( void ))
IPPAPI( IppStatus, ippcpGetNumThreads, (int* pNumThr) )
IPPAPI( const char*, ippcpGetStatusString, ( IppStatus StsCode ))
IPPAPI( int, ippcpGetEnabledNumThreads, ( void ) )
IPPAPI( Ipp64u, ippcpGetCpuClocks, (void) )

/* Defines related to experimental features enabling */
#ifdef IPPCP_PREVIEW_ALL
    #ifndef IPPCP_PREVIEW_XMSS
    #define IPPCP_PREVIEW_XMSS (1)
    #endif
    #ifndef IPPCP_PREVIEW_LMS
    #define IPPCP_PREVIEW_LMS  (1)
    #endif
#endif

/*
// =========================================================
// XMSS Algo
// =========================================================
*/
#ifdef IPPCP_PREVIEW_XMSS
    typedef enum
    {
        reserved = 0,
        XMSS_SHA2_10_256 = 1,
        XMSS_SHA2_16_256 = 2,
        XMSS_SHA2_20_256 = 3,
        XMSS_SHA2_10_512 = 4,
        XMSS_SHA2_16_512 = 5,
        XMSS_SHA2_20_512 = 6
    } IppsXMSSAlgo;

    typedef struct _cpXMSSSignatureState IppsXMSSSignatureState;
    typedef struct _cpXMSSPublicKeyState IppsXMSSPublicKeyState;

#endif // IPPCP_PREVIEW_XMSS


/*
// =========================================================
// LMS Algo
// =========================================================
*/
#ifdef IPPCP_PREVIEW_LMS
    /* Parameters set is based on two articles:
     *  RFC8554 (https://datatracker.ietf.org/doc/html/rfc8554)
     *  https://datatracker.ietf.org/doc/html/draft-fluhrer-lms-more-parm-sets-00
     */
    typedef enum
    {
        LMOTS_SHA256_N32_W1 = 1,
        LMOTS_SHA256_N32_W2 = 2,
        LMOTS_SHA256_N32_W4 = 3,
        LMOTS_SHA256_N32_W8 = 4,
        LMOTS_SHA256_N24_W1 = 5,
        LMOTS_SHA256_N24_W2 = 6,
        LMOTS_SHA256_N24_W4 = 7,
        LMOTS_SHA256_N24_W8 = 8
    } IppsLMOTSAlgo;

    typedef enum
    {
        LMS_SHA256_M32_H5  = 5,
        LMS_SHA256_M32_H10 = 6,
        LMS_SHA256_M32_H15 = 7,
        LMS_SHA256_M32_H20 = 8,
        LMS_SHA256_M32_H25 = 9,
        LMS_SHA256_M24_H5  = 10,
        LMS_SHA256_M24_H10 = 11,
        LMS_SHA256_M24_H15 = 12,
        LMS_SHA256_M24_H20 = 13,
        LMS_SHA256_M24_H25 = 14
    } IppsLMSAlgo;

    typedef struct {
        IppsLMOTSAlgo lmotsOIDAlgo;
        IppsLMSAlgo   lmsOIDAlgo;
    } IppsLMSAlgoType;

    typedef struct _cpLMSSignatureState IppsLMSSignatureState;
    typedef struct _cpLMSPublicKeyState IppsLMSPublicKeyState;
#endif // IPPCP_PREVIEW_LMS


#ifdef __cplusplus
}
#endif

#endif /* !defined IPPCPDEFS_H__ || defined( _OWN_BLDPCS ) */
