// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __WINDOWS_LAUNCHER_HPP__
#define __WINDOWS_LAUNCHER_HPP__

#include <sys/types.h>

#include <list>
#include <map>
#include <string>

#include <mesos/mesos.hpp>

#include <mesos/slave/isolator.hpp>

#include <process/future.hpp>
#include <process/subprocess.hpp>

#include <stout/flags.hpp>
#include <stout/hashmap.hpp>
#include <stout/hashset.hpp>
#include <stout/lambda.hpp>
#include <stout/option.hpp>
#include <stout/try.hpp>

#include "slave/flags.hpp"

#include "slave/containerizer/mesos/launcher.hpp"

namespace mesos {
namespace internal {
namespace slave {

// TODO(andschwa): Update comment for Windows.
// Launcher suitable for any POSIX compliant system. Uses process
// groups and sessions to track processes in a container. POSIX states
// that process groups cannot migrate between sessions so all
// processes for a container will be contained in a session.

// Minimal implementation of a `Launcher` for the Windows platform. Does not
// take into account process groups (jobs) or sessions.
class WindowsLauncher : public Launcher
{
public:
  static Try<Launcher*> create(const Flags& flags);

  virtual ~WindowsLauncher() {}

  virtual process::Future<hashset<ContainerID>> recover(
      const std::list<mesos::slave::ContainerState>& states);

  virtual Try<pid_t> fork(
      const ContainerID& containerId,
      const std::string& path,
      const std::vector<std::string>& argv,
      const process::Subprocess::IO& in,
      const process::Subprocess::IO& out,
      const process::Subprocess::IO& err,
      const flags::FlagsBase* flags,
      const Option<std::map<std::string, std::string>>& environment,
      const Option<int>& enterNamespaces,
      const Option<int>& cloneNamespaces);

  virtual process::Future<Nothing> destroy(const ContainerID& containerId);

  virtual process::Future<ContainerStatus> status(
      const ContainerID& containerId);

protected:
  WindowsLauncher() {}

  // The `std::string name` is the identifier for the job object
  // which owns the set of processes for the container.
  hashmap<ContainerID, std::string> jobs;
};


} // namespace slave {
} // namespace internal {
} // namespace mesos {

#endif // __WINDOWS_LAUNCHER_HPP__
