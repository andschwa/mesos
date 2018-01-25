// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __STOUT_OS_WINDOWS_FCNTL_HPP__
#define __STOUT_OS_WINDOWS_FCNTL_HPP__

#include <glog/logging.h>

#include <stout/nothing.hpp>
#include <stout/try.hpp>
#include <stout/windows.hpp>

#include <stout/os/socket.hpp>
#include <stout/os/windows/fd.hpp>

#define O_RDONLY _O_RDONLY
#define O_WRONLY _O_WRONLY
#define O_RDWR _O_RDWR
#define O_CREAT _O_CREAT
#define O_TRUNC _O_TRUNC
#define O_APPEND _O_APPEND
// NOTE: The semantics of close-on-exec are the same as disabling inheritance.
#define O_CLOEXEC _O_NOINHERIT

namespace os {

inline Try<Nothing> cloexec(const WindowsFD& fd)
{
  if (::SetHandleInformation(fd, HANDLE_FLAG_INHERIT, 0)) {
    return Nothing();
  } else {
    return WindowsError("`os::cloexec` failed");
  }
}


inline Try<Nothing> unsetCloexec(const WindowsFD& fd)
{
  if (::SetHandleInformation(fd, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT)) {
    return Nothing();
  } else {
    return WindowsError("`os::unsetCloexec` failed for CRT or HANDLE");
  }
}


inline Try<bool> isCloexec(const WindowsFD& fd)
{
  // This works for all cases of `fd.type()`.
  DWORD flags;
  if (::GetHandleInformation(fd, &flags)) {
    return flags != HANDLE_FLAG_INHERIT;
  } else {
    return WindowsError("`os::isCloexec` failed");
  }
}


inline Try<Nothing> nonblock(const WindowsFD& fd)
{
  switch (fd.type()) {
    case WindowsFD::FD_CRT:
    case WindowsFD::FD_HANDLE: {
      /* Do nothing. */
      break;
    }
    case WindowsFD::FD_SOCKET: {
      const u_long non_block_mode = 1;
      u_long mode = non_block_mode;

      int result = ::ioctlsocket(fd, FIONBIO, &mode);
      if (result != NO_ERROR) {
        return WindowsSocketError();
      }
      break;
    }
  }
  return Nothing();
}


// NOTE: This is not supported on Windows.
inline Try<bool> isNonblock(const WindowsFD& fd)
{
  VLOG(2) << "`os::isNonblock` has been called, but is a stub on Windows";
  return true;
}

} // namespace os {

#endif // __STOUT_OS_WINDOWS_FCNTL_HPP__
