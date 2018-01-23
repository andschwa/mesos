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

#ifndef __STOUT_OS_WINDOWS_DUP_HPP__
#define __STOUT_OS_WINDOWS_DUP_HPP__

#include <stout/error.hpp>
#include <stout/try.hpp>
#include <stout/unreachable.hpp>
#include <stout/windows.hpp> // For `WinSock2.h`.

#include <stout/os/windows/fd.hpp>

namespace os {

inline Try<WindowsFD> dup(const WindowsFD& fd)
{
  switch (fd.type()) {
    case WindowsFD::FD_CRT:
    case WindowsFD::FD_HANDLE: {
      HANDLE duplicate = INVALID_HANDLE_VALUE;
      const BOOL result = ::DuplicateHandle(
          // Source process == current.
          ::GetCurrentProcess(),
          // Handle to duplicate.
          static_cast<HANDLE>(fd),
          // Target process == current.
          ::GetCurrentProcess(),
          // Duplicated handle.
          &duplicate,
          // Ignored (DUPLICATE_SAME_ACCESS).
          0,
          // We mark the new (duplicated) handle as inheritable because
          // the `dup` function on Linux does not have the close-on-exec flag
          // (meaning, it is inheritable).
          TRUE,
          // Same access level as source.
          DUPLICATE_SAME_ACCESS);

      if (!result) {
        return WindowsError("Failed to duplicate handle of stdin file");
      }

      return duplicate;
    }
    case WindowsFD::FD_SOCKET: {
      WSAPROTOCOL_INFOW info;
      const int result = ::WSADuplicateSocketW(fd, ::GetCurrentProcessId(), &info);
      if (result != 0) {
        return SocketError();
      }
      return ::WSASocketW(0, 0, 0, &info, 0, 0);
    }
  }
  UNREACHABLE();
}

} // namespace os {

#endif // __STOUT_OS_WINDOWS_DUP_HPP__
