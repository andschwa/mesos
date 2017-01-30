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

#include <mesos/type_utils.hpp>

#include <process/collect.hpp>
#include <process/delay.hpp>
#include <process/process.hpp>
#include <process/reap.hpp>

#include <stout/unreachable.hpp>

#include "mesos/resources.hpp"

#include "slave/containerizer/mesos/launcher.hpp"
#include "slave/containerizer/mesos/windows_launcher.hpp"

using namespace process;

using std::list;
using std::map;
using std::string;
using std::vector;

using mesos::slave::ContainerState;

namespace mesos {
namespace internal {
namespace slave {

Try<Launcher*> WindowsLauncher::create(const Flags& flags)
{
  // While currently unused, `flags` is sent to retain uniformity with
  // the other launchers, and to reserve its use in the future.
  return new WindowsLauncher();
}


Future<hashset<ContainerID>> WindowsLauncher::recover(
    const list<ContainerState>& states)
{
  foreach (const ContainerState& state, states) {
    const ContainerID& containerId = state.container_id();
    // `ContainerState` is a `protobuf::Message` and so will likely never change,
    // meaning it will not store a job object name.
    // Hence the need to map `pid` to `name`.
    pid_t pid = state.pid();
    Try<std::string> name = os::name_job(pid);
    if (name.isError()) {
      return Failure("WindowsLauncher::recover: Call to `name_job` failed.");
    }

    // TODO(andschwa): Thoroughly investigate this potential scenario on Windows.
    // This came from the PosixLauncher.
    if (jobs.containsValue(name.get())) {
      // This should (almost) never occur. There is the possibility
      // that a new executor is launched with the same pid as one that
      // just exited (highly unlikely) and the slave dies after the
      // new executor is launched but before it hears about the
      // termination of the earlier executor (also unlikely).
      // Regardless, the launcher can't do anything sensible so this
      // is considered an error.
      return Failure("Detected duplicate pid " + stringify(pid) +
                     " for container " + stringify(containerId));
    }

    jobs.put(containerId, name.get());
  }

  return hashset<ContainerID>();
}


Try<pid_t> WindowsLauncher::fork(
    const ContainerID& containerId,
    const string& path,
    const vector<string>& argv,
    const Subprocess::IO& in,
    const Subprocess::IO& out,
    const Subprocess::IO& err,
    const flags::FlagsBase* flags,
    const Option<map<string, string>>& environment,
    const Option<int>& enterNamespaces,
    const Option<int>& cloneNamespaces)
{
  if (enterNamespaces.isSome() && enterNamespaces.get() != 0) {
    return Error("Windows launcher does not support entering namespaces");
  }

  if (cloneNamespaces.isSome() && cloneNamespaces.get() != 0) {
    return Error("Windows launcher does not support cloning namespaces");
  }

  if (jobs.contains(containerId)) {
    return Error("Process has already been forked for container " +
                 stringify(containerId));
  }

  vector<process::Subprocess::ParentHook> parentHooks;

  parentHooks.emplace_back(Subprocess::ParentHook(
      [](pid_t pid) -> Try<Nothing> {
        // NOTE: There are two very important parts to this hook. First,
        // Windows does not have a process hierarchy in the same sense that
        // Unix does, so in order to be able to kill a task, we have to put it
        // in a Job Object. Then, when we terminate the Job Object, it will
        // terminate all the processes in the task (including any processes
        // that were subsequently created by any process in this task).
        //
        // Second, the job handle is not closed here, because the job lifetime
        // is equal or lower than the process lifetime.
        Try<std::string> name = os::name_job(pid);
        if (name.isError()) {
          return Error(name.error());
        }

        // This creates a named job object in the Windows kernel.
        Try<Nothing> result = os::create_job(name.get());
        if (result.isError()) {
          return Error(result.error());
        }

        // This actually assigns the process `pid` to the job object.
        result = os::assign_job(name.get(), pid);
        if (result.isError()) {
          return Error(result.error());
        }

        return Nothing();
      }));

  Try<Subprocess> child = subprocess(
      path,
      argv,
      in,
      out,
      err,
      flags,
      environment,
      None(),
      parentHooks,
      {Subprocess::ChildHook::SETSID()});

  if (child.isError()) {
    return Error("Failed to fork a child process: " + child.error());
  }

  LOG(INFO) << "Forked child with pid '" << child.get().pid()
            << "' for container '" << containerId << "'";

  // Store the job object's name. For the `WindowsLauncher`,
  // our abstraction is the named job object, not the first process
  // that happened to be started for the task.
  Try<std::string> name = os::name_job(child.get().pid());
  if (name.isError()) {
    return Error(name.error());
  }
  jobs.put(containerId, name.get());

  return child.get().pid();
}


// Forward declaration.
Future<Nothing> _destroy(const Future<Option<int>>& future);


Future<Nothing> WindowsLauncher::destroy(const ContainerID& containerId)
{
  LOG(INFO) << "Asked to destroy container " << containerId;

  if (!jobs.contains(containerId)) {
    LOG(WARNING) << "Ignored destroy for unknown container " << containerId;
    return Nothing();
  }

  std::string name = jobs.get(containerId).get();

  // Kill all processes in the job object for the given container.
  Try<Nothing> result = os::kill_job(name);
  if (result.isError()) {
    return Failure("Failed to kill job object: " + result.error());
  }

  jobs.erase(containerId);

  return Nothing();
}


Future<ContainerStatus> WindowsLauncher::status(const ContainerID& containerId)
{
  if (!jobs.contains(containerId)) {
    return Failure("Container does not exist!");
  }

  ContainerStatus status;
  // TODO(andschwa): Fill in more status fields when they become available.
  // For now we return a `ContainerStatus` so that the containerizer
  // can fill in the `ContainerID`.
  return status;
}



} // namespace slave {
} // namespace internal {
} // namespace mesos {
