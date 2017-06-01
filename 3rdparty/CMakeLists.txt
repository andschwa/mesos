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

include(ExternalProject)

# Define sources of third-party dependencies.
#############################################
if (REBUNDLED)
  set(FETCH_URL ${CMAKE_CURRENT_SOURCE_DIR})
else (REBUNDLED)
  set(FETCH_URL ${3RDPARTY_DEPENDENCIES})
endif (REBUNDLED)


# DEPENDENCIES FOR THE PROCESS LIBRARY AND STOUT.
#
# Downloads, configures, and compiles the third-party libraries for the process
# library (located in `3rdparty/`, i.e. not the `mesos-3rdparty` repository).
###############################################################################
set(BOOST_URL       ${FETCH_URL}/boost-${BOOST_VERSION}.tar.gz)
set(ELFIO_URL       ${FETCH_URL}/elfio-${ELFIO_VERSION}.tar.gz)
set(GLOG_URL        ${FETCH_URL}/glog-${GLOG_VERSION}.tar.gz)
set(HTTP_PARSER_URL ${FETCH_URL}/http-parser-${HTTP_PARSER_VERSION}.tar.gz)
set(PICOJSON_URL    ${FETCH_URL}/picojson-${PICOJSON_VERSION}.tar.gz)
set(NVML_URL        ${FETCH_URL}/nvml-${NVML_VERSION}.tar.gz)
set(LIBEV_URL       ${FETCH_URL}/libev-${LIBEV_VERSION}.tar.gz)

# NOTE: libevent doesn't come rebundled, so this URL is always the same. But,
# it's only downloaded if `ENABLE_LIBEVENT` is set.
set(LIBEVENT_URL ${3RDPARTY_DEPENDENCIES}/libevent-release-${LIBEVENT_VERSION}.tar.gz)

if (WIN32)
  # NOTE: These dependencies are only rebundled on Windows because they
  # are available as installable packages on Linux; so they live
  # exclusively in the 3rdparty repo.
  set(CURL_URL   ${3RDPARTY_DEPENDENCIES}/curl-${CURL_VERSION}.tar.gz)
  set(LIBAPR_URL ${3RDPARTY_DEPENDENCIES}/libapr-${LIBAPR_VERSION}.tar.gz)
  set(ZLIB_URL   ${3RDPARTY_DEPENDENCIES}/zlib-${ZLIB_VERSION}.tar.gz)

  # NOTE: The Windows version of Glog is patched and only available in the
  # 3rdparty repo, not the local repo.
  set(GLOG_URL   ${3RDPARTY_DEPENDENCIES}/glog-${GLOG_VERSION}.tar.gz)
endif (WIN32)

# CMake arguments that need to be sent to all 3rdparty dependencies.
# NOTE: The CMAKE_GENERATOR_TOOLSET is impliticly set by `ExternalProject_Add`,
# and cannot be included twice.
list(APPEND CMAKE_FORWARD_ARGS
  # TODO(andschwa): Set the CMAKE_GENERATOR explicitly as an argmuent to
  # `ExternalProject_Add`.
  -G${CMAKE_GENERATOR}
  # This only matters for single-configuration generators.
  # E.g. Makefile, but not Visual Studio.
  -D${CMAKE_BUILD_TYPE}=${CMAKE_BUILD_TYPE}
  -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS})
foreach (l C CXX)
  list(APPEND CMAKE_FORWARD_ARGS -DCMAKE_${l}_FLAGS=${CMAKE_${l}_FLAGS})
  foreach (c DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    list(APPEND CMAKE_FORWARD_ARGS -DCMAKE_${l}_FLAGS_${c}=${CMAKE_${l}_FLAGS_${c}})
  endforeach ()
endforeach ()
# This `CMAKE_FORWARD_ARGS` variable is sent as the `CMAKE_ARGS` argument to
# the `ExternalProject_Add` macro (along with any per-project arguments), and
# is used in conjunction with the implicit `CONFIGURE_COMMAND` and
# `BUILD_COMMAND` arguments. That is, if these two arguments are either not
# specified, or are set to an empty string (or an empty variable), the macro
# automatically invokes CMake to configure and build the project. This is in
# contrast to the arguments being set to `CMAKE_NOOP`, a Mesos variable defined
# in `Common.cmake`, which explicitly will not invoke CMake. When the macro
# invokes CMake, it will configure it using the `CMAKE_ARGS`; otherwise the
# arguments are ignored. Additionally, if the project's CMake files are not in
# the root of the project's source directory, the `SUB_DIR` argument can be
# specified to account for this.

# Define build/patch/configure commands for third-party libs.
#############################################################
# NOTE: (fix for MESOS-3250) A few third-party libraries (libev, gmock) do not
# have `make install` commands available, so below we have to add our own
# "install" commands.
#
# The reason is: if we do not, we get runtime library load problems on OS X. In
# particular, `dydl` will look for these libraries at the prefix we passed to
# `configure` (or in `/usr/local` if we did not pass a prefix in), but since
# they don't have a `make install` step, they never get placed in the prefix
# folder.
#
# Our solution is to:
#   (1) make a lib directory inside the Mesos folder for each of the libraries
#       that has no install step, and
#   (2) copy all such libraries into their respective directories.
#
# (Note that step (1) is not only convenient, but important: make will add a
# `lib` to the end of your prefix path when linking, and since the built
# libraries end up in a `.libs` folder, it's not enough to simply pass the
# build directory into `configure` as a prefix; so if we're going to move the
# libraries, we might as well move them to a library folder.)
if (NOT WIN32)
  set(GLOG_CONFIG_CMD  ${GLOG_ROOT}/src/../configure --prefix=${GLOG_LIB_ROOT})
  set(GLOG_BUILD_CMD   make libglog.la)
  set(GLOG_INSTALL_CMD make install)
  # Patch glog to deal with a problem that appears when compiling on clang
  # under the C++11 standard. cf. MESOS-860, MESOS-966.
  PATCH_CMD(${MESOS_3RDPARTY_SRC}/glog-${GLOG_VERSION}.patch GLOG_PATCH_CMD)

  # NOTE: `libev` is "installed" into a lib directory, see "NOTE: (fix for
  # MESOS-3250)" comment above for explanation.
  set(LIBEV_CONFIG_CMD  ${LIBEV_ROOT}/configure --prefix=${LIBEV_ROOT}-lib)
  set(LIBEV_BUILD_CMD   make)
  set(LIBEV_INSTALL_CMD mkdir -p ${LIBEV_LIB_ROOT} && cp -r ${LIBEV_ROOT}-build/.libs/. ${LIBEV_LIB_ROOT})
  # Patch libev to keep it from reaping child processes.
  PATCH_CMD(${MESOS_3RDPARTY_SRC}/libev-4.22.patch LIBEV_PATCH_CMD)

  set(LIBEVENT_INSTALL_CMD mkdir -p ${LIBEVENT_LIB_ROOT} && cp -r ${LIBEVENT_ROOT}-build/lib/. ${LIBEVENT_LIB_DIR} && cp -r ${LIBEVENT_ROOT}-build/include/. ${LIBEVENT_INCLUDE_DIR} && cp -r ${LIBEVENT_ROOT}/include/. ${LIBEVENT_INCLUDE_DIR})
elseif (WIN32)
  set(GLOG_INSTALL_CMD ${CMAKE_NOOP})
  set(GLOG_PATCH_CMD ${PATCHEXE_LOCATION} -p1 < ${MESOS_3RDPARTY_SRC}/glog-${GLOG_VERSION}.patch)

  set(LIBEVENT_INSTALL_CMD ${CMAKE_NOOP})

  set(LIBAPR_INSTALL_CMD ${CMAKE_NOOP})

  set(LIBEVENT_CMAKE_ARGS
    ${LIBEVENT_CMAKE_ARGS}
    -DEVENT__DISABLE_OPENSSL=ON
    )
endif (NOT WIN32)

set(HTTP_PARSER_UPDATE_CMD ${CMAKE_COMMAND} -E copy ${MESOS_3RDPARTY_SRC}/http-parser/CMakeLists.txt.template ${HTTP_PARSER_ROOT}/CMakeLists.txt)

if (NOT WIN32)
  PATCH_CMD(${MESOS_3RDPARTY_SRC}/http-parser-${HTTP_PARSER_VERSION}.patch HTTP_PARSER_PATCH_CMD)
elseif (WIN32)
  # Set the patch command which will utilize patch.exe under
  # `\Users\<user>\AppData\Local\Temp with no elevation prompt`
  set(HTTP_PARSER_PATCH_CMD ${PATCHEXE_LOCATION} --binary -p1 < ${MESOS_3RDPARTY_SRC}/http-parser-${HTTP_PARSER_VERSION}.patch)
endif (NOT WIN32)

set(LIBEVENT_CMAKE_ARGS
  ${LIBEVENT_CMAKE_ARGS}
  # NOTE: libevent does not respect BUILD_SHARED_LIBS
  -DEVENT__BUILD_SHARED_LIBRARIES=${BUILD_SHARED_LIBS}
  -DEVENT__DISABLE_BENCHMARK=ON
  -DEVENT__DISABLE_TESTS=ON
  -DEVENT__DISABLE_SAMPLES=ON
  )

if (ENABLE_SSL)
  set(LIBEVENT_CMAKE_ARGS
    ${LIBEVENT_CMAKE_ARGS}
    -DEVENT__DISABLE_OPENSSL=OFF
    )
else (ENABLE_SSL)
  set(LIBEVENT_CMAKE_ARGS
    ${LIBEVENT_CMAKE_ARGS}
    -DEVENT__DISABLE_OPENSSL=ON
    )
endif (ENABLE_SSL)

if (NOT WIN32)
  set(LIBEVENT_CMAKE_ARGS
    ${LIBEVENT_CMAKE_ARGS}
    -DCMAKE_C_FLAGS=-fPIC
    )
endif (NOT WIN32)


# Third-party libraries. Tell the build system how to pull in and build third-
# party libraries at compile time, using the ExternalProject_Add macro.
##############################################################################
ExternalProject_Add(
  ${BOOST_TARGET}
  PREFIX            ${BOOST_CMAKE_ROOT}
  CONFIGURE_COMMAND ${CMAKE_NOOP}
  BUILD_COMMAND     ${CMAKE_NOOP}
  INSTALL_COMMAND   ${CMAKE_NOOP}
  URL               ${BOOST_URL}
  URL_HASH          ${BOOST_HASH}
  )

ExternalProject_Add(
  ${ELFIO_TARGET}
  PREFIX            ${ELFIO_CMAKE_ROOT}
  CONFIGURE_COMMAND ${CMAKE_NOOP}
  BUILD_COMMAND     ${CMAKE_NOOP}
  INSTALL_COMMAND   ${CMAKE_NOOP}
  URL               ${ELFIO_URL}
  URL_HASH          ${ELFIO_HASH}
  )

# The patch, configure, build, and install commands are stubbed out on Windows
# builds so that it defaults to build using CMake. This is for the same reason
# as the GMock code library build, see the call to `ExternalProject_Add` for
# the GMock project below for more details.
ExternalProject_Add(
  ${GLOG_TARGET}
  PREFIX            ${GLOG_CMAKE_ROOT}
  PATCH_COMMAND     ${GLOG_PATCH_CMD}
  CMAKE_ARGS        ${CMAKE_FORWARD_ARGS};-DBUILD_TESTING=OFF
  CONFIGURE_COMMAND ${GLOG_CONFIG_CMD}
  BUILD_COMMAND     ${GLOG_BUILD_CMD}
  INSTALL_COMMAND   ${GLOG_INSTALL_CMD}
  URL               ${GLOG_URL}
  URL_HASH          ${GLOG_HASH}
  )

ExternalProject_Add(
  ${PICOJSON_TARGET}
  PREFIX            ${PICOJSON_CMAKE_ROOT}
  CONFIGURE_COMMAND ${CMAKE_NOOP}
  BUILD_COMMAND     ${CMAKE_NOOP}
  INSTALL_COMMAND   ${CMAKE_NOOP}
  URL               ${PICOJSON_URL}
  URL_HASH          ${PICOJSON_HASH}
  )

ExternalProject_Add(
  ${NVML_TARGET}
  PREFIX            ${NVML_CMAKE_ROOT}
  CONFIGURE_COMMAND ${CMAKE_NOOP}
  BUILD_COMMAND     ${CMAKE_NOOP}
  INSTALL_COMMAND   ${CMAKE_NOOP}
  URL               ${NVML_URL}
  URL_HASH          ${NVML_HASH}
  )

ExternalProject_Add(
  ${HTTP_PARSER_TARGET}
  PREFIX            ${HTTP_PARSER_CMAKE_ROOT}
  UPDATE_COMMAND    ${HTTP_PARSER_UPDATE_CMD}
  PATCH_COMMAND     ${HTTP_PARSER_PATCH_CMD}
  CMAKE_ARGS        ${CMAKE_FORWARD_ARGS}
  INSTALL_COMMAND   ${CMAKE_NOOP}
  URL               ${HTTP_PARSER_URL}
  URL_HASH          ${HTTP_PARSER_HASH}
  )

if (NOT ENABLE_LIBEVENT)
  ExternalProject_Add(
    ${LIBEV_TARGET}
    PREFIX            ${LIBEV_CMAKE_ROOT}
    PATCH_COMMAND     ${LIBEV_PATCH_CMD}
    CMAKE_ARGS        ${CMAKE_FORWARD_ARGS}
    CONFIGURE_COMMAND ${LIBEV_CONFIG_CMD}
    BUILD_COMMAND     ${LIBEV_BUILD_CMD}
    INSTALL_COMMAND   ${LIBEV_INSTALL_CMD}
    URL               ${LIBEV_URL}
    URL_HASH          ${LIBEV_HASH}
    )
elseif (ENABLE_LIBEVENT)
  ExternalProject_Add(
    ${LIBEVENT_TARGET}
    PREFIX            ${LIBEVENT_CMAKE_ROOT}
    CMAKE_ARGS        ${CMAKE_FORWARD_ARGS};${LIBEVENT_CMAKE_ARGS}
    INSTALL_COMMAND   ${LIBEVENT_INSTALL_CMD}
    URL               ${LIBEVENT_URL}
    URL_HASH          ${LIBEVENT_HASH}
    )
endif (NOT ENABLE_LIBEVENT)

if (WIN32)
  ExternalProject_Add(
    ${LIBAPR_TARGET}
    PREFIX            ${LIBAPR_CMAKE_ROOT}
    CMAKE_ARGS        ${CMAKE_FORWARD_ARGS}
    INSTALL_COMMAND   ${LIBAPR_INSTALL_CMD}
    URL               ${LIBAPR_URL}
    URL_HASH          ${LIBAPR_HASH}
    )
endif (WIN32)


# Windows third-party libraries. Windows has no package manager, so we download
# them here.
###############################################################################
if (WIN32)
  set(CURL_CMAKE_ARGS ${CMAKE_FORWARD_ARGS} -DBUILD_CURL_TESTS=OFF)
  # NOTE: curl does not respect BUILD_SHARED_LIBS.
  if (NOT ${BUILD_SHARED_LIBS})
    # This is both a CMake argument, and a pre-processor definition.
    list(APPEND CURL_CMAKE_ARGS -DCURL_STATICLIB=ON)
    # TODO(andschwa): Define this closer to its usage; anything that includes
    # `curl.h` has to set this so that the declspec is correct.
    string(APPEND CMAKE_CXX_FLAGS " -DCURL_STATICLIB")
  endif ()
  # TODO(hausdorff): maybe try to incorporate this into findpackage for Windows
  ExternalProject_Add(
    ${CURL_TARGET}
    PREFIX            ${CURL_CMAKE_ROOT}
    PATCH_COMMAND     ${CMAKE_NOOP}
    CMAKE_ARGS        ${CURL_CMAKE_ARGS}
    INSTALL_COMMAND   ${CMAKE_NOOP}
    URL               ${CURL_URL}
    URL_HASH          ${CURL_HASH}
  )

  ExternalProject_Add(
    ${ZLIB_TARGET}
    PREFIX            ${ZLIB_CMAKE_ROOT}
    CMAKE_ARGS        ${CMAKE_FORWARD_ARGS}
    INSTALL_COMMAND   ${CMAKE_NOOP}
    URL               ${ZLIB_URL}
    URL_HASH          ${ZLIB_HASH}
  )
endif (WIN32)


# DEPENDENCIES FOR THE PROCESS LIBRARY TESTS AND STOUT TESTS.
#
# Downloads, configures, and compiles the third-party libraries for the process
# library tests (located in `3rdparty/`).
###############################################################################
list(
  APPEND CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/3rdparty/stout/cmake
  )
include(StoutConfigure)
include(StoutTestsConfigure)

set(GOOGLETEST_URL ${FETCH_URL}/googletest-release-${GOOGLETEST_VERSION}.tar.gz)
set(PROTOBUF_URL ${FETCH_URL}/protobuf-${PROTOBUF_VERSION}.tar.gz)

# NOTE: `googletest` is "installed" into a lib directory, see "NOTE: (fix for
# MESOS-3250)" comment above for explanation.
if (NOT WIN32)
  set(GOOGLETEST_INSTALL_CMD mkdir -p ${GOOGLETEST_ROOT}-lib/lib && cp -r ${GOOGLETEST_ROOT}-build/googlemock/. ${GOOGLETEST_ROOT}-lib/lib)
elseif (WIN32)
  set(GOOGLETEST_INSTALL_CMD ${CMAKE_NOOP})
endif (NOT WIN32)

if (NOT WIN32)
  set(PROTOBUF_CONFIG_CMD  ${PROTOBUF_ROOT}/src/../configure --prefix=${PROTOBUF_LIB_ROOT})
  set(PROTOBUF_BUILD_CMD   make)
  set(PROTOBUF_INSTALL_CMD make install)
elseif (WIN32)
  set(PROTOBUF_INSTALL_CMD ${CMAKE_NOOP})
endif (NOT WIN32)

ExternalProject_Add(
  ${GOOGLETEST_TARGET}
  PREFIX            ${GOOGLETEST_CMAKE_ROOT}
  CMAKE_ARGS        ${CMAKE_FORWARD_ARGS}
  INSTALL_COMMAND   ${GOOGLETEST_INSTALL_CMD}
  URL               ${GOOGLETEST_URL}
  URL_HASH          ${GOOGLETEST_HASH}
  )

ExternalProject_Add(
  ${PROTOBUF_TARGET}
  PREFIX            ${PROTOBUF_CMAKE_ROOT}
  SOURCE_SUBDIR     cmake
  CMAKE_ARGS        ${CMAKE_FORWARD_ARGS};-Dprotobuf_BUILD_TESTS=OFF
  CONFIGURE_COMMAND ${PROTOBUF_CONFIG_CMD}
  BUILD_COMMAND     ${PROTOBUF_BUILD_CMD}
  INSTALL_COMMAND   ${PROTOBUF_INSTALL_CMD}
  URL               ${PROTOBUF_URL}
  URL_HASH          ${PROTOBUF_HASH}
  )


# BUILDING THE STOUT TESTS.
#
# Builds, configures, and compiles the Stout tests.
###################################################
add_subdirectory(stout/tests)


# BUILDING THE PROCESS LIBRARY AND ITS TESTS.
#
# Builds, configures, and compiles the process library and assorted tests.
##########################################################################
add_subdirectory(libprocess)


# BUILDING THE MESOS DEPENDENCIES.
#
# Downloads, configures, and compiles the third-party libraries for the mesos.
###################################################
set(LEVELDB_URL   ${FETCH_URL}/leveldb-${LEVELDB_VERSION}.tar.gz)
# NOTE: The Windows version of ZooKeeper is patched and only available in the
# 3rdparty repo, not the local repo.
if (NOT WIN32)
  set(ZOOKEEPER_URL ${FETCH_URL}/zookeeper-${ZOOKEEPER_VERSION}.tar.gz)
elseif (WIN32)
  set(ZOOKEEPER_URL ${3RDPARTY_DEPENDENCIES}/zookeeper-${ZOOKEEPER_VERSION}.tar.gz)
endif (NOT WIN32)


# Third-party libraries. Tell the build system how to pull in and build third-
# party libraries at compile time, using the ExternalProject_Add macro.
##############################################################################
if (NOT WIN32)
  # NOTE: `ExternalProject_Add` is used separately because older versions
  # of CMake used on Linux do not understand the `SOURCE_SUBDIR` command.
  PATCH_CMD(
    ${MESOS_3RDPARTY_SRC}/zookeeper-${ZOOKEEPER_VERSION}.patch
    ZOOKEEPER_PATCH_CMD)

  ExternalProject_Add(
    ${ZOOKEEPER_TARGET}
    PREFIX            ${ZOOKEEPER_CMAKE_ROOT}
    PATCH_COMMAND     ${ZOOKEEPER_PATCH_CMD}
    CONFIGURE_COMMAND cd ${ZOOKEEPER_C_ROOT} && ./configure --enable-shared=no --with-pic --srcdir=. --prefix=${ZOOKEEPER_LIB}
    BUILD_COMMAND     cd ${ZOOKEEPER_C_ROOT} && make
    INSTALL_COMMAND   cd ${ZOOKEEPER_C_ROOT} && make install
    URL               ${ZOOKEEPER_URL}
    URL_HASH          ${ZOOKEEPER_HASH}
    )
elseif (WIN32)
  ExternalProject_Add(
    ${ZOOKEEPER_TARGET}
    PREFIX            ${ZOOKEEPER_CMAKE_ROOT}
    # Set the patch command which will utilize patch.exe in temp location for no elevation prompt
    # NOTE: We do not specify the `--binary` patch option here because the
    # files being modified are extracted with CRLF (Windows) line endings
    # already. The `--binary` option will instead fail to apply the patch.
    PATCH_COMMAND     ${PATCHEXE_LOCATION} -p1 < ${MESOS_3RDPARTY_SRC}/zookeeper-${ZOOKEEPER_VERSION}.patch
    SOURCE_SUBDIR     src/c
    CMAKE_ARGS        ${CMAKE_FORWARD_ARGS};-DWANT_CPPUNIT=OFF
    INSTALL_COMMAND   ${CMAKE_NOOP}
    URL               ${ZOOKEEPER_URL}
    URL_HASH          ${ZOOKEEPER_HASH}
    )
endif (NOT WIN32)

if (NOT WIN32)
  set(LEVELDB_CONFIG_CMD  cd ${LEVELDB_ROOT} && ./configure --prefix=${LEVELDB_ROOT}-lib)
  set(LEVELDB_BUILD_CMD   cd ${LEVELDB_ROOT} && make OPT=-O2\ -g\ -fPIC)
  set(LEVELDB_INSTALL_CMD cd ${LEVELDB_ROOT} && make install)
  PATCH_CMD(
    ${MESOS_3RDPARTY_SRC}/leveldb-${LEVELDB_VERSION}.patch
    LEVELDB_PATCH_CMD)

  ExternalProject_Add(
    ${LEVELDB_TARGET}
    PREFIX            ${LEVELDB_TARGET}
    PATCH_COMMAND     ${LEVELDB_PATCH_CMD}
    CONFIGURE_COMMAND ${CMAKE_NOOP}
    BUILD_COMMAND     ${LEVELDB_BUILD_CMD}
    INSTALL_COMMAND   ${CMAKE_NOOP}
    URL               ${LEVELDB_URL}
    URL_HASH          ${LEVELDB_HASH}
    )
endif (NOT WIN32)
