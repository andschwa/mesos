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

# SOURCE FILES FOR THE PROCESS LIBRARY.
#######################################

set(PRIVATE_PROCESS_HEADERS
  ${PRIVATE_PROCESS_HEADERS}
  authenticator_manager.hpp
  config.hpp
  decoder.hpp
  encoder.hpp
  event_loop.hpp
  gate.hpp
  poll_socket.hpp
  process_reference.hpp
  )

if (WIN32)
  set(PROCESS_ROOT_SRC
    ${PROCESS_ROOT_SRC}
    subprocess_windows.cpp
    )
else (WIN32)
  set(PROCESS_ROOT_SRC
    ${PROCESS_ROOT_SRC}
    subprocess_posix.cpp
    )
endif (WIN32)

set(PROCESS_ROOT_SRC
  ${PROCESS_ROOT_SRC}
  authenticator_manager.cpp
  authenticator.cpp
  clock.cpp
  firewall.cpp
  help.cpp
  http.cpp
  io.cpp
  latch.cpp
  logging.cpp
  mime.cpp
  pid.cpp
  poll_socket.cpp
  profiler.cpp
  process.cpp
  reap.cpp
  socket.cpp
  subprocess.cpp
  time.cpp
  timeseries.cpp
  )

set(PROCESS_METRICS_SRC
  ${PROCESS_METRICS_SRC}
  metrics/metrics.cpp
  )

# TODO(hausdorff): Add this to build after we resolve MESOS-6691.
if (FALSE)
  set(PROCESS_SSL_SRC
    ${PROCESS_SSL_SRC}
    openssl.cpp
    libevent_ssl_socket.cpp
    ssl/utilities.cpp
    )
endif (FALSE)

if (ENABLE_LIBEVENT)
  set(PROCESS_ROOT_SRC
    ${PROCESS_ROOT_SRC}
    libevent.cpp
    libevent_poll.cpp
    )

  set(PRIVATE_PROCESS_HEADERS
    ${PRIVATE_PROCESS_HEADERS}
    libevent.hpp
    )
else (ENABLE_LIBEVENT)
  set(PROCESS_ROOT_SRC
    ${PROCESS_ROOT_SRC}
    libev.cpp
    libev_poll.cpp
    )

  set(PRIVATE_PROCESS_HEADERS
    ${PRIVATE_PROCESS_HEADERS}
    libev.hpp
    )
endif (ENABLE_LIBEVENT)

set(PROCESS_SRC
  ${PROCESS_SRC}
  ${STOUT_HEADERS}
  ${PROCESS_HEADERS}
  ${PRIVATE_PROCESS_HEADERS}
  ${PROCESS_ROOT_SRC}
  ${PROCESS_METRICS_SRC}
  ${PROCESS_SSL_SRC}
  )

# INCLUDE DIRECTIVES FOR PROCESS LIBRARY (generates, e.g., -I/path/to/thing
# on Linux).
###########################################################################
include_directories(${PROCESS_INCLUDE_DIRS})

# LINKING LIBRARIES BY DIRECTORY (might generate, e.g., -L/path/to/thing on
# Linux).
###########################################################################
link_directories(${PROCESS_LIB_DIRS})

# THE PROCESS LIBRARY (generates, e.g., libprocess.so, etc., on Linux).
#######################################################################
if (WIN32)
  add_library(${PROCESS_TARGET} STATIC ${PROCESS_SRC})
else (WIN32)
  add_library(${PROCESS_TARGET} SHARED ${PROCESS_SRC})

  set_target_properties(
    ${PROCESS_TARGET} PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
    )
endif (WIN32)

GROUP_STOUT_HEADERS()
GROUP_PROCESS_HEADERS()

GROUP_SOURCE(
  "src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "*.[ch]*")

set_target_properties(
  ${PROCESS_TARGET} PROPERTIES
  VERSION ${PROCESS_PACKAGE_VERSION}
  SOVERSION ${PROCESS_PACKAGE_SOVERSION}
  )

add_dependencies(${PROCESS_TARGET} ${PROCESS_DEPENDENCIES})

# ADD LINKER FLAGS (generates, e.g., -lglog on Linux).
######################################################
target_link_libraries(${PROCESS_TARGET} ${PROCESS_LIBS})
