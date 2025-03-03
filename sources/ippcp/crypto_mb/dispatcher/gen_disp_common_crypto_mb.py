#=========================================================================
# Copyright (C) 2024 Intel Corporation
#
# Licensed under the Apache License,  Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# 	http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law  or agreed  to  in  writing,  software
# distributed under  the License  is  distributed  on  an  "AS IS"  BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the  specific  language  governing  permissions  and
# limitations under the License.
#=========================================================================

#
# Crypto_MB APIs parser. Parses strings such as: MBXAPI( type,name,arg ) 
#

import re
import sys
import os
import hashlib

def readNextFunction(header, curLine, headerID):    ## read next function with arguments
  ## find header ID macros
  FunName  = ''
  FunArg   = ''
  FunType  = ''
  FunArgCall = ''
  success = False
  while (curLine < len(header) and success == False):
    if not headerID and re.match( '\s*#\s*if\s*!\s*defined\s*\(\s*__IPP', header[curLine]):
      headerID= re.sub( '.*__IPP', '__IPP', header[curLine] )
      headerID= re.sub( "\)", '', headerID)
      headerID= re.sub( '[\n\s]', '', headerID )
    
    if re.match( '^\s*MBXAPI\s*\(.*', header[curLine] ) :
      FunStr= header[curLine]
      FunStr= re.sub('\n','',FunStr)   ## remove EOL symbols
  
      while not re.match('.*\)\s*\)\s*$', FunStr):   ## concatenate string if string is not completed
        curLine= curLine+1
        FunStr= FunStr+header[curLine]
        FunStr= re.sub('\n','',FunStr)   ## remove EOL symbols
    
      FunStr= re.sub('\s+', ' ', FunStr)
    
      s= FunStr.split(',')
    
      ## Extract function name
      FunName= s[1]
      FunName= re.sub('\s', '', FunName)
    
      ## Extract function type
      FunType= re.sub( '.*\(', '', s[0] )
    
      ## Extract function arguments
      FunArg= re.sub('.*\(.*,.+,\s*\(', '(', FunStr)
      FunArg= re.sub('\)\s*\)', ')', FunArg)

      ## Extract function arguments for call
      s= FunArg.split(',')
      for i in s:
        l= i.split(' ')
        FunArgCall+= l[-1] + ', '
        FunArgCall= re.sub('\[\w*\]', '', FunArgCall)
        FunArgCall= re.sub('\*', '', FunArgCall)
      FunArgCall= FunArgCall[:-2]
      FunArgCall= '(' + FunArgCall

      if (FunArg == '(void)'):
        FunArg= '()'
        FunArgCall= '()'

      success = True

    curLine = curLine + 1

  return {'curLine':curLine, 'FunType':FunType, 'FunName':FunName, 'FunArg':FunArg, 'FunArgCall':FunArgCall, 'success':success }
