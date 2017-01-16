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

# SOURCE FILES FOR THE AGENT LIBRARY.
#####################################
set(MASTER_SRC
  ${MASTER_SRC}
  ${MESOS_SRC_DIR}/master/constants.hpp
  ${MESOS_SRC_DIR}/master/flags.cpp
  ${MESOS_SRC_DIR}/master/flags.hpp
  ${MESOS_SRC_DIR}/master/http.cpp
  ${MESOS_SRC_DIR}/master/machine.hpp
  ${MESOS_SRC_DIR}/master/maintenance.cpp
  ${MESOS_SRC_DIR}/master/maintenance.hpp
  ${MESOS_SRC_DIR}/master/master.cpp
  ${MESOS_SRC_DIR}/master/master.hpp
  ${MESOS_SRC_DIR}/master/metrics.cpp
  ${MESOS_SRC_DIR}/master/metrics.hpp
  ${MESOS_SRC_DIR}/master/quota.cpp
  ${MESOS_SRC_DIR}/master/quota.hpp
  ${MESOS_SRC_DIR}/master/quota_handler.cpp
  ${MESOS_SRC_DIR}/master/registrar.cpp
  ${MESOS_SRC_DIR}/master/registrar.hpp
  ${MESOS_SRC_DIR}/master/registry.hpp
  ${MESOS_SRC_DIR}/master/registry.proto
  ${MESOS_SRC_DIR}/master/validation.cpp
  ${MESOS_SRC_DIR}/master/validation.hpp
  ${MESOS_SRC_DIR}/master/weights.cpp
  ${MESOS_SRC_DIR}/master/weights.hpp
  ${MESOS_SRC_DIR}/master/weights_handler.cpp
  ${MESOS_SRC_DIR}/master/allocator/allocator.cpp
  ${MESOS_SRC_DIR}/master/allocator/mesos/allocator.hpp
  ${MESOS_SRC_DIR}/master/allocator/mesos/hierarchical.cpp
  ${MESOS_SRC_DIR}/master/allocator/mesos/hierarchical.hpp
  ${MESOS_SRC_DIR}/master/allocator/mesos/metrics.cpp
  ${MESOS_SRC_DIR}/master/allocator/mesos/metrics.hpp
  ${MESOS_SRC_DIR}/master/allocator/sorter/sorter.hpp
  ${MESOS_SRC_DIR}/master/allocator/sorter/drf/metrics.cpp
  ${MESOS_SRC_DIR}/master/allocator/sorter/drf/metrics.hpp
  ${MESOS_SRC_DIR}/master/allocator/sorter/drf/sorter.cpp
  ${MESOS_SRC_DIR}/master/allocator/sorter/drf/sorter.hpp
  ${MESOS_SRC_DIR}/master/contender/contender.cpp
  ${MESOS_SRC_DIR}/master/contender/standalone.cpp
  ${MESOS_SRC_DIR}/master/contender/standalone.hpp
  ${MESOS_SRC_DIR}/master/contender/zookeeper.cpp
  ${MESOS_SRC_DIR}/master/contender/zookeeper.hpp
  ${MESOS_SRC_DIR}/master/detector/detector.cpp
  ${MESOS_SRC_DIR}/master/detector/standalone.cpp
  ${MESOS_SRC_DIR}/master/detector/standalone.hpp
  ${MESOS_SRC_DIR}/master/detector/zookeeper.cpp
  ${MESOS_SRC_DIR}/master/detector/zookeeper.hpp
  )

# SOURCE GROUPS FOR AGENT.
##########################
GROUP_SOURCE(
  "Master Source Files"
  "${MESOS_SRC_DIR}/master"
  "${MESOS_SRC_DIR}/master"
  "*.[ch]*")
