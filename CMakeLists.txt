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

# THE MESOS PROJECT.
####################
cmake_minimum_required(VERSION 2.8.10)

project(Mesos)
set(MESOS_MAJOR_VERSION 1)
set(MESOS_MINOR_VERSION 2)
set(MESOS_PATCH_VERSION 0)
set(PACKAGE_VERSION
  ${MESOS_MAJOR_VERSION}.${MESOS_MINOR_VERSION}.${MESOS_PATCH_VERSION})

set(MESOS_PACKAGE_VERSION ${PACKAGE_VERSION})
set(MESOS_PACKAGE_SOVERSION 0)


# CMAKE MODULE SETUP.
#####################
# Paths that are searched when `include(...)` is called.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/3rdparty/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/3rdparty/libprocess/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/3rdparty/stout/cmake)
list(
  APPEND
  CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/3rdparty/libprocess/cmake/macros)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/src/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/src/examples/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/src/master/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/src/slave/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/src/tests/cmake)

# Macros.
include(Common)
include(External)
include(PatchCommand)
include(Versions)
include(VsBuildCommand)

# Configuration.
include(MesosConfigure)


# SUBDIRECTORIES.
#################
add_subdirectory(3rdparty)
add_subdirectory(src)


# TESTS.
########
add_custom_target(
  check ${STOUT_TESTS_TARGET}
  COMMAND ${PROCESS_TESTS_TARGET}
  COMMAND ${MESOS_TESTS_TARGET}
  DEPENDS ${STOUT_TESTS_TARGET} ${PROCESS_TESTS_TARGET} ${MESOS_TESTS_TARGET}
  )

# Declare the `test` target. This is somewhat complicated because CMake tries
# to reserve the `test` target and warns you if you use it. We disable the
# warning, make sure that `include(CTest)` wasn't called, and then set our own
# test target.
cmake_policy(SET CMP0037 OLD)

if (BUILD_TESTING OR BUILD_TESTING_STATIC OR BUILD_TESTING_SHARED)
  message(
    FATAL_ERROR
    "CTest is enabled, which will conflict with our `make test` target; "
    "please disable before continuing")
endif (BUILD_TESTING OR BUILD_TESTING_STATIC OR BUILD_TESTING_SHARED)

add_custom_target(
  test
  DEPENDS ${STOUT_TESTS_TARGET} ${PROCESS_TESTS_TARGET} ${MESOS_TESTS_TARGET}
  )
