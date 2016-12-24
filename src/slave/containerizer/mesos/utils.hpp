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

#ifndef __MESOS_CONTAINERIZER_UTILS_HPP__
#define __MESOS_CONTAINERIZER_UTILS_HPP__

#include <string>

#include <mesos/mesos.hpp>

#include <stout/option.hpp>
#include <stout/os.hpp>
#include <stout/path.hpp>
#include <stout/strings.hpp>
#include <stout/try.hpp>

#include <stout/os/getenv.hpp>

namespace mesos {
namespace internal {
namespace slave {

ContainerID getRootContainerId(const ContainerID& containerId);


#ifdef __linux__
Try<pid_t> getMountNamespaceTarget(pid_t parent);
#endif // __linux__


// Returns a platform-specific default for the `PATH` environment
// variable.
// NOTE: This function can return different things on different
// hosts. We place it here (instead of, say, Stout) to make sure that
// it is called close to the place where we are calling `exec`, to
// minimize the possibility of accidentally calling this function on
// a separate host from the one calling `exec` below.
inline std::string defaultPath()
{
#ifndef __WINDOWS__
  return "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
#else
  // NOTE: On Windows, this code must run on the host where we are
  // expecting to `exec` the task, because the value of
  // `%SYSTEMROOT%` is not identical on all platforms.
  const Option<std::string> systemRootEnv = os::getenv("SYSTEMROOT");
  const std::string systemRoot = systemRootEnv.isSome()
    ? systemRootEnv.get()
    : "C:\\WINDOWS";

  return strings::join(";", systemRoot, path::join(systemRoot, "system32"));
#endif // !__WINDOWS__
}

} // namespace slave {
} // namespace internal {
} // namespace mesos {

#endif // __MESOS_CONTAINERIZER_UTILS_HPP__
