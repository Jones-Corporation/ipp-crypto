#===============================================================================
# Copyright (C) 2024 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the 'License');
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an 'AS IS' BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions
# and limitations under the License.
#
#===============================================================================

# Security Linker flags

set(LINK_FLAG_SECURITY "")

# Specifies whether to generate an executable image that can be randomly rebased at load time.
set(LINK_FLAG_SECURITY "${LINK_FLAG_SECURITY} /DYNAMICBASE")
# This option modifies the header of an executable image, a .dll file or .exe file, to indicate whether ASLR with 64-bit addresses is supported.
set(LINK_FLAG_SECURITY "${LINK_FLAG_SECURITY} /HIGHENTROPYVA")
# The /LARGEADDRESSAWARE option tells the linker that the application can handle addresses larger than 2 gigabytes.
set(LINK_FLAG_SECURITY "${LINK_FLAG_SECURITY} /LARGEADDRESSAWARE")
# Indicates that an executable is compatible with the Windows Data Execution Prevention (DEP) feature
set(LINK_FLAG_SECURITY "${LINK_FLAG_SECURITY} /NXCOMPAT")

# Security Compiler flags

set(CMAKE_C_FLAGS_SECURITY "")
# Detect some buffer overruns.
set(CMAKE_C_FLAGS_SECURITY "${CMAKE_C_FLAGS_SECURITY} /GS")
# Warning level = 3
set(CMAKE_C_FLAGS_SECURITY "${CMAKE_C_FLAGS_SECURITY} /W3")
# Changes all warnings to errors.
set(CMAKE_C_FLAGS_SECURITY "${CMAKE_C_FLAGS_SECURITY} /WX")
# Enable Intel® Control-Flow Enforcement Technology (Intel® CET) protection
set(CMAKE_C_FLAGS_SECURITY "${CMAKE_C_FLAGS_SECURITY} -fcf-protection:full")
# Changes all warnings to errors.
set(CMAKE_C_FLAGS_SECURITY "${CMAKE_C_FLAGS_SECURITY} /WX")

# Linker flags

# Add export files
if(MBX_FIPS_MODE)
  set(LINK_FLAGS_DYNAMIC "/DEF:${CRYPTO_MB_SOURCES_DIR}/cmake/dll_export/crypto_mb_fips_selftests.defs")
else()
  set(LINK_FLAGS_DYNAMIC "/DEF:${CRYPTO_MB_SOURCES_DIR}/cmake/dll_export/crypto_mb.defs")
endif()

# Disables linking to Intel® libraries
set(LINK_FLAG_DYNAMIC_WINDOWS "${LINK_FLAG_DYNAMIC_WINDOWS} /Qno-intel-lib")

# Compiler flags

# Tells the compiler to align functions and loops
# set(CMAKE_C_FLAGS "/Qfnalign:32 /Qalign-loops:32")
# Suppress warning #10120: overriding '/O2' with '/O3'
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -wd10120 -Wno-unused-command-line-argument -Wno-unused-parameter -Wno-pointer-sign -Wno-sign-compare -Wno-static-in-inline /Qno-intel-lib")
# Ensures that compilation takes place in a freestanding environment
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Qfreestanding")

if(CODE_COVERAGE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Qrof-gen:srcpos /Qprof-dir:${PROF_DATA_DIR}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

# Tells the compiler to conform to a specific language standard.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Qstd=c99")

# Causes the application to use the multithread, static version of the run-time library
set(CMAKE_C_FLAGS_RELEASE "/MT")
# Optimization level = 3
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O3")
# No-debug macro
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

# Causes the application to use the multithread, static version of the run-time library (debug version).
set(CMAKE_C_FLAGS_DEBUG "/MTd")
# The /Zi option produces a separate PDB file that contains all the symbolic debugging information for use with the debugger.
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Zi")
# Turns off all optimizations.
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od")
# Debug macro
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

# Optimisation dependent flags
set(AVX512_CFLAGS "-march=icelake-server -mavx512dq -mavx512ifma -mavx512f -mavx512vbmi2 -mavx512cd -mavx512bw -mbmi2")
