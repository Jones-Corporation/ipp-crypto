/*************************************************************************
* Copyright (C) 2024 Intel Corporation
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

#include <crypto_mb/status.h>

#include <internal/common/ifma_defs.h>
#include <internal/common/ifma_cvt52.h>
#include <internal/ecnist/ifma_ecpoint_p384.h>
#include <internal/rsa/ifma_rsa_arith.h>

#if (_MBX>=_MBX_K1)

#ifndef BN_OPENSSL_DISABLE

mbx_status internal_avx512_nistp384_ecpublic_key_ssl_mb8(BIGNUM* pa_pubx[8],
                                                         BIGNUM* pa_puby[8],
                                                         BIGNUM* pa_pubz[8],
                                                   const BIGNUM* const pa_skey[8],
                                                   int8u* pBuffer, int use_jproj_coords)
{
   mbx_status status = 0;
   int buf_no = 0;
   /* zero padded keys */
   U64 scalarz[P384_LEN64+1];
   ifma_BN_transpose_copy((int64u (*)[8])scalarz, pa_skey, P384_BITSIZE);
   scalarz[P384_LEN64] = get_zero64();

   status |= MBX_SET_STS_BY_MASK(status, is_zero(scalarz, P384_LEN64+1), MBX_STATUS_MISMATCH_PARAM_ERR);

   /* do not need to clear copy of secret keys before this return - all of them is NULL or zero */
   if(!MBX_IS_ANY_OK_STS(status))
      return status;
      
   /* public key */
   P384_POINT P;

   /* compute public keys */
   MB_FUNC_NAME(ifma_ec_nistp384_mul_pointbase_)(&P, scalarz);
   /* clear copy of secret */
   MB_FUNC_NAME(zero_)((int64u (*)[8])scalarz, sizeof(scalarz)/sizeof(U64));

   if(!use_jproj_coords)
      MB_FUNC_NAME(get_nistp384_ec_affine_coords_)(P.X, P.Y, &P);

   /* convert P coordinates to regular domain */
   MB_FUNC_NAME(ifma_frommont52_p384_)(P.X, P.X);
   MB_FUNC_NAME(ifma_frommont52_p384_)(P.Y, P.Y);
   if(use_jproj_coords)
      MB_FUNC_NAME(ifma_frommont52_p384_)(P.Z, P.Z);

   /* convert public key and store BIGNUM result */
   int8u tmp[8][NUMBER_OF_DIGITS(P384_BITSIZE,8)];
   int8u* const pa_tmp[8] = {tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6],tmp[7]};

   /* X */
   ifma_mb8_to_HexStr8(pa_tmp, (const int64u (*)[8])P.X, P384_BITSIZE);
   for(buf_no=0; (buf_no<8) && (0==MBX_GET_STS(status, buf_no)); buf_no++) {
      BN_bin2bn(pa_tmp[buf_no], NUMBER_OF_DIGITS(P384_BITSIZE,8), pa_pubx[buf_no]);
   }

   /* Y */
   ifma_mb8_to_HexStr8(pa_tmp, (const int64u (*)[8])P.Y, P384_BITSIZE);
   for(buf_no=0; (buf_no<8) && (0==MBX_GET_STS(status, buf_no)); buf_no++) {
      BN_bin2bn(pa_tmp[buf_no], NUMBER_OF_DIGITS(P384_BITSIZE,8), pa_puby[buf_no]);
   }

   /* Z */
   if(use_jproj_coords) {
      ifma_mb8_to_HexStr8(pa_tmp, (const int64u (*)[8])P.Z, P384_BITSIZE);
      for(buf_no=0; (buf_no<8) && (0==MBX_GET_STS(status, buf_no)); buf_no++) {
         BN_bin2bn(pa_tmp[buf_no], NUMBER_OF_DIGITS(P384_BITSIZE,8), pa_pubz[buf_no]);
      }
   }

   return status;
}

#endif // BN_OPENSSL_DISABLE

mbx_status internal_avx512_nistp384_ecpublic_key_mb8(int64u* pa_pubx[8],
                                                     int64u* pa_puby[8],
                                                     int64u* pa_pubz[8],
                                               const int64u* const pa_skey[8],
                                               int8u* pBuffer, int use_jproj_coords)
{
   mbx_status status = 0;
   /* zero padded keys */
   U64 scalarz[P384_LEN64+1];
   ifma_BNU_transpose_copy((int64u (*)[8])scalarz, pa_skey, P384_BITSIZE);
   scalarz[P384_LEN64] = get_zero64();

   status |= MBX_SET_STS_BY_MASK(status, is_zero(scalarz, P384_LEN64+1), MBX_STATUS_MISMATCH_PARAM_ERR);

   /* do not need to clear copy of secret keys before this return - all of them is NULL or zero */
   if(!MBX_IS_ANY_OK_STS(status))
      return status;

   /* public key */
   P384_POINT P;

   /* compute public keys */
   MB_FUNC_NAME(ifma_ec_nistp384_mul_pointbase_)(&P, scalarz);
   /* clear copy of secret */
   MB_FUNC_NAME(zero_)((int64u (*)[8])scalarz, sizeof(scalarz)/sizeof(U64));

   if(!use_jproj_coords)
      MB_FUNC_NAME(get_nistp384_ec_affine_coords_)(P.X, P.Y, &P);

   /* convert P coordinates to regular domain */
   MB_FUNC_NAME(ifma_frommont52_p384_)(P.X, P.X);
   MB_FUNC_NAME(ifma_frommont52_p384_)(P.Y, P.Y);
   if(use_jproj_coords)
      MB_FUNC_NAME(ifma_frommont52_p384_)(P.Z, P.Z);

   /* store result */
   ifma_mb8_to_BNU(pa_pubx, (const int64u (*)[8])P.X, P384_BITSIZE);
   ifma_mb8_to_BNU(pa_puby, (const int64u (*)[8])P.Y, P384_BITSIZE);
   if(use_jproj_coords)
      ifma_mb8_to_BNU(pa_pubz, (const int64u (*)[8])P.Z, P384_BITSIZE);

   return status;
}

#endif /* #if (_MBX>=_MBX_K1) */
