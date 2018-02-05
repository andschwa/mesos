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

#ifndef __STOUT_OS_WINDOWS_FD_HPP__
#define __STOUT_OS_WINDOWS_FD_HPP__

#include <array>
#include <memory>
#include <ostream>

#include <stout/check.hpp>
#include <stout/nothing.hpp>
#include <stout/try.hpp>
#include <stout/unreachable.hpp>
#include <stout/variant.hpp>
#include <stout/windows.hpp> // For `WinSock2.h`.

namespace os {
namespace internal {

inline bool is_valid(const int crt)
{
  return crt >= 0;
}

inline bool is_valid(const HANDLE handle)
{
  return handle != nullptr && handle != INVALID_HANDLE_VALUE;
}

inline bool is_valid(const SOCKET socket)
{
  return socket != INVALID_SOCKET;
}

} // namespace internal

class IntFD
{
public:
  IntFD(int crt)
    : crt(crt),
      handle(
          internal::is_valid(crt)
            ? reinterpret_cast<HANDLE>(::_get_osfhandle(crt))
            : INVALID_HANDLE_VALUE)
  {
    CHECK(internal::is_valid(crt) == internal::is_valid(handle));
  }

  operator int() const { return crt; }

  operator HANDLE() const { return handle; }

private:
  int crt;
  HANDLE handle;
};


class HandleFD
{
public:
  HandleFD(HANDLE handle)
    : crt(internal::is_valid(handle)
            ? ::_open_osfhandle(reinterpret_cast<intptr_t>(handle), O_RDWR)
            : -1),
      handle(handle)
  {
    CHECK(internal::is_valid(crt) == internal::is_valid(handle));
  }

  operator int() const { return crt; }

  operator HANDLE() const { return handle; }

private:
  int crt;
  HANDLE handle;
};


class SocketFD
{
public:
  SocketFD(SOCKET socket) : socket(socket) {}
  // On Windows, libevent's `evutil_socket_t` is set to `intptr_t`.
  SocketFD(intptr_t socket) : socket(static_cast<SOCKET>(socket)) {}

  operator int() const
  {
    if (internal::is_valid(socket)) {
      return static_cast<int>(socket);
    } else {
      return -1;
    }
  }

  operator SOCKET() const { return socket; }

  operator intptr_t() const { return static_cast<intptr_t>(socket); }

  operator HANDLE() const { return reinterpret_cast<HANDLE>(socket); }

private:
  SOCKET socket;
};

using VariantFD = Variant<os::IntFD, os::HandleFD, os::SocketFD>;
struct WindowsFD : VariantFD
{
  using VariantFD::VariantFD;

  operator int() const
  {
    return visit(
        [](const os::IntFD& fd) { return boost::get<os::IntFD>(fd); },
        [](const os::HandleFD& fd) { return boost::get<os::HandleFD>(fd); },
        [](const os::SocketFD& fd) { return boost::get<os::SocketFD>(fd); });
  }

  operator HANDLE() const
  {
    return visit(
        [](const os::IntFD& fd) { return boost::get<os::IntFD>(fd); },
        [](const os::HandleFD& fd) { return boost::get<os::HandleFD>(fd); },
        [](const os::SocketFD& fd) { return boost::get<os::SocketFD>(fd); });
  }

  operator SOCKET() const { return boost::get<SocketFD>(*this); }

  operator intptr_t() const { return static_cast<intptr_t>(boost::get<SocketFD>(*this)); }
};

} // namespace os {

#endif // __STOUT_OS_WINDOWS_FD_HPP__
