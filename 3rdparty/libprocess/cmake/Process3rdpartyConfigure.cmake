# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# DEFINE DIRECTORY STRUCTURE FOR THIRD-PARTY LIBS.
##################################################
EXTERNAL("libev"           ${LIBEV_VERSION}           "${MESOS_3RDPARTY_BIN}")
EXTERNAL("libevent"        ${LIBEVENT_VERSION}        "${MESOS_3RDPARTY_BIN}")
EXTERNAL("libapr"          ${LIBAPR_VERSION}          "${MESOS_3RDPARTY_BIN}")

# Intermediate convenience variables for oddly-structured directories.
set(LIBEV_LIB_ROOT    ${LIBEV_ROOT}-lib/lib)
set(LIBEVENT_LIB_ROOT ${LIBEVENT_ROOT}-lib/lib)

# Convenience variables for include directories of third-party dependencies.
set(PROCESS_INCLUDE_DIR ${MESOS_3RDPARTY_SRC}/libprocess/include)

set(GPERFTOOLS_INCLUDE_DIR      ${GPERFTOOLS}/src)
set(LIBEV_INCLUDE_DIR           ${LIBEV_ROOT})

if (WIN32)
  set(APR_INCLUDE_DIR      ${LIBAPR_ROOT}/include ${LIBAPR_ROOT}-build)
  set(LIBEVENT_INCLUDE_DIR
    ${LIBEVENT_ROOT}/include
    ${LIBEVENT_ROOT}-build/include)
else ()
  set(LIBEVENT_INCLUDE_DIR ${LIBEVENT_LIB_ROOT}/include)
endif ()

# Convenience variables for `lib` directories of built third-party dependencies.
set(LIBEV_LIB_DIR       ${LIBEV_ROOT}-build/.libs)

if (WIN32)
  set(LIBEVENT_LIB_DIR    ${LIBEVENT_ROOT}-build/lib)
else ()
  set(LIBEVENT_LIB_DIR    ${LIBEVENT_LIB_ROOT}/lib)
endif ()

# Convenience variables for "lflags", the symbols we pass to CMake to generate
# things like `-L/path/to/glog` or `-lglog`.
set(LIBEV_LFLAG       ev)
set(LIBEVENT_LFLAG    event)

if (WIN32)

else ()
  set(DL_LFLAG       dl)
  set(SASL_LFLAG     sasl2)
endif ()

# Configure the process library, the last of our third-party libraries.
#######################################################################
include(ProcessConfigure)
