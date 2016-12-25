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
set(AGENT_SRC
  ${AGENT_SRC}
  ${MESOS_SRC_DIR}/slave/constants.cpp
  ${MESOS_SRC_DIR}/slave/constants.hpp
  ${MESOS_SRC_DIR}/slave/container_logger.cpp
  ${MESOS_SRC_DIR}/slave/flags.cpp
  ${MESOS_SRC_DIR}/slave/flags.hpp
  ${MESOS_SRC_DIR}/slave/gc.cpp
  ${MESOS_SRC_DIR}/slave/gc.hpp
  ${MESOS_SRC_DIR}/slave/http.cpp
  ${MESOS_SRC_DIR}/slave/metrics.cpp
  ${MESOS_SRC_DIR}/slave/metrics.hpp
  ${MESOS_SRC_DIR}/slave/paths.cpp
  ${MESOS_SRC_DIR}/slave/paths.hpp
  ${MESOS_SRC_DIR}/slave/posix_signalhandler.hpp
  ${MESOS_SRC_DIR}/slave/qos_controller.cpp
  ${MESOS_SRC_DIR}/slave/qos_controllers/noop.cpp
  ${MESOS_SRC_DIR}/slave/qos_controllers/noop.hpp
  ${MESOS_SRC_DIR}/slave/resource_estimator.cpp
  ${MESOS_SRC_DIR}/slave/slave.cpp
  ${MESOS_SRC_DIR}/slave/slave.hpp
  ${MESOS_SRC_DIR}/slave/state.cpp
  ${MESOS_SRC_DIR}/slave/state.hpp
  ${MESOS_SRC_DIR}/slave/status_update_manager.cpp
  ${MESOS_SRC_DIR}/slave/status_update_manager.hpp
  ${MESOS_SRC_DIR}/slave/validation.cpp
  ${MESOS_SRC_DIR}/slave/validation.hpp
  ${MESOS_SRC_DIR}/slave/windows_ctrlhandler.hpp
  ${MESOS_SRC_DIR}/slave/container_loggers/sandbox.cpp
  ${MESOS_SRC_DIR}/slave/container_loggers/sandbox.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/composing.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/composing.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/containerizer.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/containerizer.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/docker.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/docker.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/fetcher.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/fetcher.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/constants.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/containerizer.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/containerizer.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolator.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolator.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/launch.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/launch.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/launcher.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/launcher.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/mount.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/mount.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/paths.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/paths.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/utils.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/utils.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/io/switchboard.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/io/switchboard.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/posix.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/windows.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/backend.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/backend.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/paths.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/paths.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/provisioner.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/provisioner.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/store.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/store.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/utils.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/utils.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/appc/cache.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/appc/cache.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/appc/fetcher.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/appc/fetcher.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/appc/paths.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/appc/paths.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/appc/store.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/appc/store.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/backends/copy.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/backends/copy.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/local_puller.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/local_puller.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/message.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/message.proto
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/metadata_manager.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/metadata_manager.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/paths.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/paths.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/puller.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/puller.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/registry_puller.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/registry_puller.hpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/store.cpp
  ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/docker/store.hpp
  ${MESOS_SRC_DIR}/slave/resource_estimators/noop.cpp
  ${MESOS_SRC_DIR}/slave/resource_estimators/noop.hpp
  )

if (NOT WIN32)
  set(AGENT_SRC
    ${AGENT_SRC}
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/docker/volume/driver.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/docker/volume/driver.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/docker/volume/paths.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/docker/volume/paths.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/docker/volume/state.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/docker/volume/state.proto
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/filesystem/posix.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/filesystem/posix.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/network/cni/paths.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/network/cni/paths.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/network/cni/spec.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/network/cni/spec.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/network/cni/spec.proto
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/posix/disk.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/posix/disk.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/posix/rlimits.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/posix/rlimits.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/volume/sandbox_path.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/volume/sandbox_path.hpp
    )
else (NOT WIN32)
  set(AGENT_SRC
    ${AGENT_SRC}
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/filesystem/posix.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/filesystem/posix.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/filesystem/windows.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/filesystem/windows.hpp
    )
endif (NOT WIN32)

if (LINUX)
  set(AGENT_SRC
    ${AGENT_SRC}
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/linux_launcher.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/linux_launcher.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/appc/runtime.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/appc/runtime.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/cgroups.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/cgroups.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/constants.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystem.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystem.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/blkio.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/blkio.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/cpu.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/cpu.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/cpuacct.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/cpuacct.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/cpuset.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/cpuset.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/devices.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/devices.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/hugetlb.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/hugetlb.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/memory.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/memory.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/net_cls.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/net_cls.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/net_prio.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/net_prio.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/perf_event.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/perf_event.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/pids.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/cgroups/subsystems/pids.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/docker/runtime.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/docker/runtime.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/docker/volume/isolator.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/docker/volume/isolator.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/filesystem/linux.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/filesystem/linux.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/filesystem/shared.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/filesystem/shared.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/gpu/allocator.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/gpu/allocator.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/gpu/components.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/gpu/isolator.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/gpu/isolator.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/gpu/nvidia.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/gpu/nvml.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/gpu/nvml.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/gpu/volume.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/gpu/volume.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/linux/capabilities.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/linux/capabilities.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/namespaces/ipc.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/namespaces/ipc.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/namespaces/pid.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/namespaces/pid.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/network/cni/cni.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/network/cni/cni.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/network/cni/plugins/port_mapper/port_mapper.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/network/cni/plugins/port_mapper/port_mapper.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/volume/image.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/isolators/volume/image.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/backends/aufs.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/backends/aufs.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/backends/bind.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/backends/bind.hpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/backends/overlay.cpp
    ${MESOS_SRC_DIR}/slave/containerizer/mesos/provisioner/backends/overlay.hpp
    )
endif (LINUX)

# SOURCE GROUPS FOR AGENT.
##########################
GROUP_SOURCE(
    "Agent Source Files"
    "${MESOS_SRC_DIR}/slave"
    "${MESOS_SRC_DIR}/slave"
    "*.[ch]*")
