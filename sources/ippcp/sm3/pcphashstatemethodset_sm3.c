/*************************************************************************
* Copyright (C) 2022 Intel Corporation
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
//  Purpose:
//     Cryptography Primitive.
//     Digesting message according to SM3
// 
//  Contents:
//        ippsHashStateMethodSet_SM3()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcphash.h"
#include "pcphash_rmf.h"
#include "pcptool.h"
#include "pcpsm3stuff.h"

/*F*
//    Name: ippsHashStateMethodSet_SM3
//
// Purpose: Setup SM3 method inside the hash state.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pMethod == NULL or pState == NULL
//    ippStsNoErr             no errors
//
*F*/
IPPFUN( IppStatus, ippsHashStateMethodSet_SM3, (IppsHashState_rmf* pState, IppsHashMethod* pMethod) )
{
   /* test pointers */
   IPP_BAD_PTR2_RET(pState, pMethod);

   HASH_METHOD(pState) = pMethod;

   pMethod->hashAlgId     = ippHashAlg_SM3;
   pMethod->hashLen       = IPP_SM3_DIGEST_BITSIZE/8;
   pMethod->msgBlkSize    = MBS_SM3;
   pMethod->msgLenRepSize = MLR_SM3;

   pMethod->hashInit      = sm3_hashInit;
#if (_IPP32E >= _IPP32E_L9)
   if (IsFeatureEnabled(ippCPUID_AVX2SM3)) {
      pMethod->hashUpdate = sm3_hashUpdate_ni;
   }
   else
#endif
   {
      pMethod->hashUpdate = sm3_hashUpdate;
   }
   pMethod->hashOctStr    = sm3_hashOctString;
   pMethod->msgLenRep     = sm3_msgRep;

   return ippStsNoErr;
}
