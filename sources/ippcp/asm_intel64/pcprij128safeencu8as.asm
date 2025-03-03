;=========================================================================
; Copyright (C) 2015 Intel Corporation
;
; Licensed under the Apache License,  Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
; 	http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law  or agreed  to  in  writing,  software
; distributed under  the License  is  distributed  on  an  "AS IS"  BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the  specific  language  governing  permissions  and
; limitations under the License.
;=========================================================================

;
;
;     Purpose:  Cryptography Primitive.
;               Rijndael-128 (AES) cipher functions.
;               (It's the special free from Sbox/tables implementation)
;
;     Content:
;        SafeEncrypt_RIJ128()
;
;     History:
;
;   Notes.
;   The implementation is based on
;   isomorphism between native GF(2^8) and composite GF((2^4)^2).
;
;
%include "asmdefs.inc"
%include "ia_32e.inc"
%include "ia_32e_regs.inc"
%include "pcpvariant.inc"

%if (_AES_NI_ENABLING_ == _FEATURE_OFF_) || (_AES_NI_ENABLING_ == _FEATURE_TICKTOCK_)
%if (_IPP32E >= _IPP32E_U8)

%macro PTRANSFORM 6.nolist
  %xdefine %%dst %1
  %xdefine %%src %2
  %xdefine %%memTransLO %3
  %xdefine %%memTransHI %4
  %xdefine %%tmp %5
  %xdefine %%srcLO %6

   movdqa   %%dst, oword [rel %%memTransLO]     ;; LO transformation
   movdqa   %%tmp, oword [rel %%memTransHI]     ;; HI transformation

   movdqa   %%srcLO, %%src     ;; split src:
   psrlw    %%src, 4         ;;
   pand     %%srcLO, xmm7    ;; low 4 bits -> srcLO
   pand     %%src, xmm7      ;; upper 4 bits -> src

   pshufb   %%dst, %%srcLO     ;; transformation
   pshufb   %%tmp, %%src
   pxor     %%dst, %%tmp
%endmacro

%macro PLOOKUP_MEM 3.nolist
  %xdefine %%dst %1
  %xdefine %%src %2
  %xdefine %%Table %3

   movdqa   %%dst, OWORD [rel %%Table]
   pshufb   %%dst,%%src
%endmacro

%macro PREDUCE_MOD15 2.nolist
  %xdefine %%dst %1
  %xdefine %%src %2

   movdqa   %%dst, %%src
   pcmpgtb  %%src, xmm7
   psubb    %%dst, %%src
%endmacro

%macro PINVERSE_GF16_FWD 6.nolist
  %xdefine %%xmmB %1
  %xdefine %%xmmC %2
  %xdefine %%xmmP %3
  %xdefine %%xmmQ %4
  %xdefine %%xmmD %5
  %xdefine %%xmmT %6

   PLOOKUP_MEM    %%xmmT, %%xmmC, GF16_logTbl    ;; xmmT = index_of(c)
   pxor           %%xmmC, %%xmmB
   PLOOKUP_MEM    %%xmmQ, %%xmmC, GF16_logTbl    ;; xmmQ = index_of(b xor c)

   PLOOKUP_MEM    %%xmmD, %%xmmB, GF16_sqr1      ;; xmmD = sqr(b)*beta^14
   PLOOKUP_MEM    %%xmmP, %%xmmB, GF16_logTbl    ;; xmmP = index_of(b)

   paddb          %%xmmT, %%xmmQ                                     ;; xmmT = index_of(c) + index_of(b xor c)
   PREDUCE_MOD15  %%xmmC, %%xmmT                                     ;;
   PLOOKUP_MEM    %%xmmT, %%xmmC, GF16_expTbl    ;; c*(b xor c)

   pxor           %%xmmD, %%xmmT                                     ;; xmmD = delta = (c*(b xor c)) xor (sqr(b)*beta^14)
   PLOOKUP_MEM    %%xmmT, %%xmmD, GF16_invLog    ;; xmmT = index_of( inv(delta) )

   paddb          %%xmmQ, %%xmmT  ;; xmmQ = index_of((b xor c) * inv(delta))
   paddb          %%xmmP, %%xmmT  ;; xmmP = index_of(b * inv(delta))
   PREDUCE_MOD15  %%xmmT, %%xmmQ
   PLOOKUP_MEM    %%xmmC, %%xmmT, GF16_expTbl
   PREDUCE_MOD15  %%xmmT, %%xmmP
   PLOOKUP_MEM    %%xmmB, %%xmmT, GF16_expTbl
%endmacro

segment .data align=IPP_ALIGN_FACTOR


align IPP_ALIGN_FACTOR

ENCODE_DATA:

;; (forward) native GF(2^8) to composite GF((2^4)^2) transformation : {0x01,0x2E,0x49,0x43,0x35,0xD0,0x3D,0xE9}
TransFwdLO \
      DB    000h  ;; 000h                             ;; 0
      DB    001h  ;; 001h                             ;; 1
      DB    02Eh  ;; 02Eh                             ;; 2
      DB    02Fh  ;; 02Eh XOR 001h                    ;; 3
      DB    049h  ;; 049h                             ;; 4
      DB    048h  ;; 049h XOR 001h                    ;; 5
      DB    067h  ;; 049h XOR 02Eh                    ;; 6
      DB    066h  ;; 049h XOR 02Eh XOR 001h           ;; 7
      DB    043h  ;; 043h                             ;; 8
      DB    042h  ;; 043h XOR 001h                    ;; 9
      DB    06Dh  ;; 043h XOR 02Eh                    ;; a
      DB    06Ch  ;; 043h XOR 02Eh XOR 001h           ;; b
      DB    00Ah  ;; 043h XOR 049h                    ;; c
      DB    00Bh  ;; 043h XOR 049h XOR 001h           ;; d
      DB    024h  ;; 043h XOR 049h XOR 02Eh           ;; e
      DB    025h  ;; 043h XOR 049h XOR 02Eh XOR 001h  ;; f
TransFwdHI \
      DB    000h  ;; 000h                             ;; 0
      DB    035h  ;; 035h                             ;; 1
      DB    0D0h  ;; 0D0h                             ;; 2
      DB    0E5h  ;; 0D0h XOR 035h                    ;; 3
      DB    03Dh  ;; 03Dh                             ;; 4
      DB    008h  ;; 03Dh XOR 035h                    ;; 5
      DB    0EDh  ;; 03Dh XOR 0D0h                    ;; 6
      DB    0D8h  ;; 03Dh XOR 0D0h XOR 035h           ;; 7
      DB    0E9h  ;; 0E9h                             ;; 8
      DB    0DCh  ;; 0E9h XOR 035h                    ;; 9
      DB    039h  ;; 0E9h XOR 0D0h                    ;; a
      DB    00Ch  ;; 0E9h XOR 0D0h XOR 035h           ;; b
      DB    0D4h  ;; 0E9h XOR 03Dh                    ;; c
      DB    0E1h  ;; 0E9h XOR 03Dh XOR 035h           ;; d
      DB    004h  ;; 0E9h XOR 03Dh XOR 0D0h           ;; e
      DB    031h  ;; 0E9h XOR 03Dh XOR 0D0h XOR 035h  ;; f

;; (inverse) composite GF((2^4)^2) to native GF(2^8) transformation : {0x01,0x5C,0xE0,0x50,0x1F,0xEE,0x55,0x6A}
TransInvLO \
      DB    000h  ;; 000h                             ;; 0
      DB    001h  ;; 001h                             ;; 1
      DB    05Ch  ;; 05Ch                             ;; 2
      DB    05Dh  ;; 05Ch XOR 001h                    ;; 3
      DB    0E0h  ;; 0E0h                             ;; 4
      DB    0E1h  ;; 0E0h XOR 001h                    ;; 5
      DB    0BCh  ;; 0E0h XOR 05Ch                    ;; 6
      DB    0BDh  ;; 0E0h XOR 05Ch XOR 001h           ;; 7
      DB    050h  ;; 050h                             ;; 8
      DB    051h  ;; 050h XOR 001h                    ;; 9
      DB    00Ch  ;; 050h XOR 05Ch                    ;; a
      DB    00Dh  ;; 050h XOR 05Ch XOR 001h           ;; b
      DB    0B0h  ;; 050h XOR 0E0h                    ;; c
      DB    0B1h  ;; 050h XOR 0E0h XOR 001h           ;; d
      DB    0ECh  ;; 050h XOR 0E0h XOR 05Ch           ;; e
      DB    0EDh  ;; 050h XOR 0E0h XOR 05Ch XOR 001h  ;; f
TransInvHI \
      DB    000h  ;; 000h                             ;; 0
      DB    01Fh  ;; 01Fh                             ;; 1
      DB    0EEh  ;; 0EEh                             ;; 2
      DB    0F1h  ;; 0EEh XOR 01Fh                    ;; 3
      DB    055h  ;; 055h                             ;; 4
      DB    04Ah  ;; 055h XOR 01Fh                    ;; 5
      DB    0BBh  ;; 055h XOR 0EEh                    ;; 6
      DB    0A4h  ;; 055h XOR 0EEh XOR 01Fh           ;; 7
      DB    06Ah  ;; 06Ah                             ;; 8
      DB    075h  ;; 06Ah XOR 01Fh                    ;; 9
      DB    084h  ;; 06Ah XOR 0EEh                    ;; a
      DB    09Bh  ;; 06Ah XOR 0EEh XOR 01Fh           ;; b
      DB    03Fh  ;; 06Ah XOR 055h                    ;; c
      DB    020h  ;; 06Ah XOR 055h XOR 01Fh           ;; d
      DB    0D1h  ;; 06Ah XOR 055h XOR 0EEh           ;; e
      DB    0CEh  ;; 06Ah XOR 055h XOR 0EEh XOR 01Fh  ;; f


GF16_csize  DB 00Fh,0Fh,0Fh,0Fh,0Fh,0Fh,0Fh,0Fh,0Fh,0Fh,0Fh,0Fh,0Fh,0Fh,0Fh,0Fh

;; GF16 elements:
;;         0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
GF16_logTbl \
      DB 0C0h,00h,01h,04h,02h,08h,05h,0Ah,03h,0Eh,09h,07h,06h,0Dh,0Bh,0Ch
GF16_expTbl \
      DB 001h,02h,04h,08h,03h,06h,0Ch,0Bh,05h,0Ah,07h,0Eh,0Fh,0Dh,09h,01h
GF16_sqr1   \
      DB 000h,09h,02h,0Bh,08h,01h,0Ah,03h,06h,0Fh,04h,0Dh,0Eh,07h,0Ch,05h ;; sqr(GF16_element) * beta^14
GF16_invLog \
      DB 0C0h,00h,0Eh,0Bh,0Dh,07h,0Ah,05h,0Ch,01h,06h,08h,09h,02h,04h,03h
GF16_expTbl_shift \
      DB 010h,020h,040h,080h,030h,060h,0C0h,0B0h,050h,0A0h,070h,0E0h,0F0h,0D0h,090h,010h

;; affine transformation matrix (forward cipher) : {0x10,0x22,0x55,0x82,0x41,0x34,0x40,0x2A}
FwdAffineLO \
      DB    000h  ;; 000h                             ;; 0
      DB    010h  ;; 010h                             ;; 1
      DB    022h  ;; 022h                             ;; 2
      DB    032h  ;; 022h XOR 010h                    ;; 3
      DB    055h  ;; 055h                             ;; 4
      DB    045h  ;; 055h XOR 010h                    ;; 5
      DB    077h  ;; 055h XOR 022h                    ;; 6
      DB    067h  ;; 055h XOR 022h XOR 010h           ;; 7
      DB    082h  ;; 082h                             ;; 8
      DB    092h  ;; 082h XOR 010h                    ;; 9
      DB    0A0h  ;; 082h XOR 022h                    ;; a
      DB    0B0h  ;; 082h XOR 022h XOR 010h           ;; b
      DB    0D7h  ;; 082h XOR 055h                    ;; c
      DB    0C7h  ;; 082h XOR 055h XOR 010h           ;; d
      DB    0F5h  ;; 082h XOR 055h XOR 022h           ;; e
      DB    0E5h  ;; 082h XOR 055h XOR 022h XOR 010h  ;; f
FwdAffineHI \
      DB    000h  ;; 000h                             ;; 0
      DB    041h  ;; 041h                             ;; 1
      DB    034h  ;; 034h                             ;; 2
      DB    075h  ;; 034h XOR 041h                    ;; 3
      DB    040h  ;; 040h                             ;; 4
      DB    001h  ;; 040h XOR 041h                    ;; 5
      DB    074h  ;; 040h XOR 034h                    ;; 6
      DB    035h  ;; 040h XOR 034h XOR 041h           ;; 7
      DB    02Ah  ;; 02Ah                             ;; 8
      DB    06Bh  ;; 02Ah XOR 041h                    ;; 9
      DB    01Eh  ;; 02Ah XOR 034h                    ;; a
      DB    05Fh  ;; 02Ah XOR 034h XOR 041h           ;; b
      DB    06Ah  ;; 02Ah XOR 040h                    ;; c
      DB    02Bh  ;; 02Ah XOR 040h XOR 041h           ;; d
      DB    05Eh  ;; 02Ah XOR 040h XOR 034h           ;; e
      DB    01Fh  ;; 02Ah XOR 040h XOR 034h XOR 041h  ;; f

;; affine transformation constant (forward cipher)
FwdAffineCnt \
      DQ    0C2C2C2C2C2C2C2C2h,0C2C2C2C2C2C2C2C2h

;; shift rows transformation (forward cipher)
FwdShiftRows \
      DB    0,5,10,15,4,9,14,3,8,13,2,7,12,1,6,11

;; mix columns transformation (forward cipher)
GF16mul_E_2x \
            DB 000h,02Eh,04Fh,061h,08Dh,0A3h,0C2h,0ECh,039h,017h,076h,058h,0B4h,09Ah,0FBh,0D5h
GF16mul_1_Cx \
            DB 000h,0C1h,0B2h,073h,054h,095h,0E6h,027h,0A8h,069h,01Ah,0DBh,0FCh,03Dh,04Eh,08Fh

ColumnROR    \
   DB 1,2,3,0,5,6,7,4,9,10,11,8,13,14,15,12


segment .text align=IPP_ALIGN_FACTOR
;*************************************************************
; convert GF(2^128) -> GF((2^4)^2)
;*************************************************************
align IPP_ALIGN_FACTOR
IPPASM TransformNative2Composite,PUBLIC
%assign LOCAL_FRAME 0
        USES_GPR rsi,rdi
        USES_XMM xmm7
        COMP_ABI 2
;; rdi:  pOutBlk:  DWORD,    ; output block address
;; rsi:  pInpBlk:  DWORD,    ; input  block address

   movdqu   xmm0, oword [rsi] ; input block
   movdqa   xmm7, oword [rel GF16_csize]

   ;; convert input into the composite GF((2^4)^2)
   PTRANSFORM  xmm1, xmm0, TransFwdLO,TransFwdHI, xmm2, xmm3

   movdqu   oword [rdi], xmm1 ; output block

   REST_XMM
   REST_GPR
   ret
ENDFUNC TransformNative2Composite

;*************************************************************
;* void SafeEncrypt_RIJ128(const Ipp8u* pInpBlk,
;*                               Ipp8u* pOutBlk,
;*                               int    nr,
;*                        const Ipp8u* pKeys,
;*                        const void*  Tables)
;*************************************************************
align IPP_ALIGN_FACTOR
;;
;; Lib = U8
;;
IPPASM SafeEncrypt_RIJ128,PUBLIC
%assign LOCAL_FRAME 0
        USES_GPR rsi,rdi
        USES_XMM xmm6,xmm7
        COMP_ABI 5
;; rdi:  pInpBlk:  DWORD,    ; input  block address
;; rsi:  pOutBlk:  DWORD,    ; output block address
;; rdx:  nr:           DWORD,    ; number of rounds
;; rcx:  pKey:     DWORD     ; key material address

%assign RSIZE  sizeof(dword)         ; size of row
%assign SC  4                        ; columns in STATE
%assign SSIZE  RSIZE*SC              ; size of state

   movdqu   xmm1, oword [rdi] ; input block

   movdqa   xmm7, oword [rel GF16_csize]

   ;; convert input into the composite GF((2^4)^2)
   PTRANSFORM  xmm0, xmm1, TransFwdLO,TransFwdHI, xmm2, xmm3

   ;; initial whitening
   pxor     xmm0, oword [rcx]
   add      rcx, SSIZE

   ;; (nr-1) regular rounds
   sub      rdx,1

.encode_round:
   ;; SubByte() Transformation:

   ;; split input by low and upper parts
   movdqa      xmm1, xmm0
   pand        xmm0, xmm7  ; low parts (4 bits)
   psrlw       xmm1, 4
   pand        xmm1, xmm7  ; upper parts (4 bits)

   ;; compute multiplicative inverse
   PINVERSE_GF16_FWD xmm1,xmm0, xmm3,xmm2,xmm4,xmm5

   ;; affine transformation
   movdqa      xmm3, oword [rel FwdAffineLO]
   movdqa      xmm2, oword [rel FwdAffineHI]
   movdqa      xmm4, oword [rel FwdAffineCnt] ; H(c), c=0x63
   pshufb      xmm3, xmm0
   pshufb      xmm2, xmm1
   pxor        xmm3, xmm4
   pxor        xmm3, xmm2

   ;; ShiftRows() Transformation:
   pshufb      xmm3, [rel FwdShiftRows]

   ;; MixColumn() Transformation:
   movdqa      xmm1, xmm3
   movdqa      xmm2, xmm3
   pxor        xmm4, xmm4
   psrlw       xmm2, 4

   pand        xmm1, xmm7           ;; a0*(0xE + 0x2*t)
   PLOOKUP_MEM xmm0, xmm1, GF16mul_E_2x

   pand        xmm2, xmm7           ;; a1*(0x2*0x9 + (0x2+0xE)*t)
   PLOOKUP_MEM xmm1, xmm2, GF16mul_1_Cx

   pxor        xmm0, xmm1

   pshufb      xmm3, [rel ColumnROR]
   pxor        xmm4, xmm3

   pshufb      xmm3, [rel ColumnROR]
   pxor        xmm4, xmm3

   movdqa      xmm2, xmm0
   pshufb      xmm2, [rel ColumnROR]
   pxor        xmm0, xmm2

   pshufb      xmm3, [rel ColumnROR]
   pxor        xmm4, xmm3

   pxor        xmm0, xmm4

   ;; AddRoundKey() Transformation:
   pxor        xmm0, oword [rcx]
   add         rcx, SSIZE

   sub         rdx,1
   jg          .encode_round


   ;;
   ;; the last one is irregular
   ;;

   ;; SubByte() Transformation:
   movdqa      xmm1, xmm0
   pand        xmm0, xmm7  ; low parts (4 bits)
   psrlw       xmm1, 4
   pand        xmm1, xmm7  ; upper parts (4 bits)

   ;; compute multiplicative inverse
   PINVERSE_GF16_FWD xmm1,xmm0, xmm3,xmm2,xmm4,xmm5

   ;; affine transformation
   movdqa      xmm3, oword [rel FwdAffineLO]
   movdqa      xmm2, oword [rel FwdAffineHI]
   movdqa      xmm4, oword [rel FwdAffineCnt] ; H(c), c=0x63
   pshufb      xmm3, xmm0
   pshufb      xmm2, xmm1
   pxor        xmm3, xmm4
   pxor        xmm3, xmm2

   ;; ShiftRows() Transformation:
   pshufb      xmm3, [rel FwdShiftRows]

   ;; AddRoundKey() Transformation:
   pxor        xmm3, oword [rcx]

   ;; convert output into the native GF(2^8)
   PTRANSFORM  xmm0,xmm3, TransInvLO,TransInvHI, xmm2,xmm1

   movdqu      oword [rsi], xmm0

   REST_XMM
   REST_GPR
   ret
ENDFUNC SafeEncrypt_RIJ128

%endif

%endif ;; _AES_NI_ENABLING_

