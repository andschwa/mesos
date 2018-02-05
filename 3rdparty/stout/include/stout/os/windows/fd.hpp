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
#include <stout/windows.hpp> // For `WinSock2.h`.

namespace os {
namespace internal {

inline bool is_valid(int crt)
{
  return crt >= 0;
}

inline bool is_valid(HANDLE handle)
{
  return handle != nullptr && handle != INVALID_HANDLE_VALUE;
}

inline bool is_valid(SOCKET socket)
{
  return socket != INVALID_SOCKET;
}

} // namespace internal


class CrtFD
{
public:
  CrtFD(int crt = -1)
    : crt(crt),
      handle(
          internal::is_valid(crt)
            ? reinterpret_cast<HANDLE>(::_get_osfhandle(crt))
            : INVALID_HANDLE_VALUE)
  {
    CHECK(internal::is_valid(crt) == internal::is_valid(handle));
  }

  bool is_valid() const
  {
    CHECK(internal::is_valid(crt) == internal::is_valid(handle));

    return internal::is_valid(crt);
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
  HandleFD(HANDLE handle = INVALID_HANDLE_VALUE)
    : crt(internal::is_valid(handle)
            ? ::_open_osfhandle(reinterpret_cast<intptr_t>(handle), O_RDWR)
            : -1),
      handle(handle)
  {
    CHECK(internal::is_valid(crt) == internal::is_valid(handle));
  }

  bool is_valid() const
  {
    CHECK(internal::is_valid(crt) == internal::is_valid(handle));

    return internal::is_valid(handle);
  }

  operator int() const
  {
    if (is_valid()) {
      return crt;
    } else {
      return -1;
    }
  }

  operator HANDLE() const { return handle; }

private:
  int crt;
  HANDLE handle;
};


class SocketFD
{
public:
  SocketFD(SOCKET socket = INVALID_SOCKET) : socket(socket) {}

  // On Windows, libevent's `evutil_socket_t` is set to `intptr_t`.
  SocketFD(intptr_t socket) : socket(static_cast<SOCKET>(socket)) {}

  bool is_valid() const
  {
    return internal::is_valid(socket);
  }

  operator int() const
  {
    if (is_valid()) {
      return static_cast<int>(socket);
    } else {
      return -1;
    }
  }

  operator SOCKET() const { return socket; }

  operator intptr_t() const { return static_cast<intptr_t>(socket); }

private:
  SOCKET socket;
};


// The `WindowsFD` class exists to provide an common interface with the POSIX
// file descriptor. While the bare `int` representation of the POSIX file
// descriptor API is undesirable, we rendezvous there in order to maintain the
// existing code in Mesos.
//
// In the platform-agnostic code paths, the `int_fd` type is aliased to
// `WindowsFD`. The `os::*` functions return a type appropriate to the platform,
// which allows us to write code like this:
//
//   Try<int_fd> fd = os::open(...);
//
// The `WindowsFD` constructs off one of:
//   (1) `int` - from the WinCRT API
//   (2) `HANDLE` - from the Win32 API
//   (3) `SOCKET` - from the WinSock API
//
// The `os::*` functions then take an instance of `WindowsFD`, examines
// the state and dispatches to the appropriate API.
class WindowsFD
{
public:
  enum Type
  {
    FD_INVALID,
    FD_CRT,
    FD_HANDLE,
    FD_SOCKET
  };

  WindowsFD()
    : type_(FD_INVALID), crt_(CrtFD()), handle_(HandleFD()), socket_(SocketFD())
  {}

  WindowsFD(int crt) : type_(FD_CRT), crt_(crt) {}

  // IMPORTANT: The `HANDLE` here is expected to be file handles. Specifically,
  //            `HANDLE`s returned by file API such as `CreateFile`. There are
  //            APIs that return `HANDLE`s with different error values, and
  //            therefore must be handled accordingly. For example, a thread API
  //            such as `CreateThread` returns `NULL` as the error value, rather
  //            than `INVALID_HANDLE_VALUE`.
  // TODO(mpark): Consider adding a second parameter which tells us what the
  //              error values are.
  WindowsFD(HANDLE handle) : type_(FD_HANDLE), handle_(handle) {}

  WindowsFD(SOCKET socket) : type_(FD_SOCKET), socket_(socket) {}

  // On Windows, libevent's `evutil_socket_t` is set to `intptr_t`.
  WindowsFD(intptr_t socket) : type_(FD_SOCKET), socket_(socket) {}

  bool is_valid() const
  {
    switch (type()) {
    case FD_INVALID:
      return false;
    case FD_CRT:
      return crt_.is_valid();
    case FD_HANDLE:
      return handle_.is_valid();
    case FD_SOCKET:
      return socket_.is_valid();
    }

    UNREACHABLE();
  }

  operator int() const
  {
    switch (type()) {
    case FD_INVALID:
      return -1;
    case FD_CRT:
      return crt_;
    case FD_HANDLE:
      return handle_;
    case FD_SOCKET:
      return socket_;
    }

    UNREACHABLE();
  }

  operator HANDLE() const
  {
    CHECK((type() == FD_CRT) || (type() == FD_HANDLE));
    return handle_;
  }

  operator SOCKET() const
  {
    CHECK_EQ(FD_SOCKET, type());
    return socket_;
  }

  operator intptr_t() const
  {
    CHECK_EQ(FD_SOCKET, type());
    return socket_;
  }

  Type type() const { return type_; }

private:
  Type type_;

  CrtFD crt_;
  HandleFD handle_;
  SocketFD socket_;
};


inline std::ostream& operator<<(std::ostream& stream, const WindowsFD& fd)
{
  switch (fd.type()) {
    case WindowsFD::FD_CRT: {
      stream << static_cast<int>(fd);
      break;
    }
    case WindowsFD::FD_HANDLE: {
      stream << static_cast<HANDLE>(fd);
      break;
    }
    case WindowsFD::FD_SOCKET: {
      stream << static_cast<SOCKET>(fd);
      break;
    }
  }
  return stream;
}


// TODO(andschwa): Re-implement (WindowsFD, WindowsFD) comparators to compare
// primitive values instead of `int`.
inline bool operator<(const WindowsFD& left, const WindowsFD& right)
{
  return static_cast<int>(left) < static_cast<int>(right);
}


inline bool operator<(int left, const WindowsFD& right)
{
  return left < static_cast<int>(right);
}


inline bool operator<(const WindowsFD& left, int right)
{
  return static_cast<int>(left) < right;
}


inline bool operator>(const WindowsFD& left, const WindowsFD& right)
{
  return right < left;
}


inline bool operator>(int left, const WindowsFD& right)
{
  return left > static_cast<int>(right);
}


inline bool operator>(const WindowsFD& left, int right)
{
  return static_cast<int>(left) > right;
}


inline bool operator<=(const WindowsFD& left, const WindowsFD& right)
{
  return !(left > right);
}


inline bool operator<=(int left, const WindowsFD& right)
{
  return left <= static_cast<int>(right);
}


inline bool operator<=(const WindowsFD& left, int right)
{
  return static_cast<int>(left) <= right;
}


inline bool operator>=(const WindowsFD& left, const WindowsFD& right)
{
  return !(left < right);
}


inline bool operator>=(int left, const WindowsFD& right)
{
  return left >= static_cast<int>(right);
}


inline bool operator>=(const WindowsFD& left, int right)
{
  return static_cast<int>(left) >= right;
}


inline bool operator==(const WindowsFD& left, const WindowsFD& right)
{
  return static_cast<int>(left) == static_cast<int>(right);
}


inline bool operator==(int left, const WindowsFD& right)
{
  return left == static_cast<int>(right);
}


inline bool operator==(const WindowsFD& left, int right)
{
  return static_cast<int>(left) == right;
}


inline bool operator!=(const WindowsFD& left, const WindowsFD& right)
{
  return !(left == right);
}


inline bool operator!=(int left, const WindowsFD& right)
{
  return left != static_cast<int>(right);
}


inline bool operator!=(const WindowsFD& left, int right)
{
  return static_cast<int>(left) != right;
}

} // namespace os {

namespace std {

template <>
struct hash<os::WindowsFD>
{
  using argument_type = os::WindowsFD;
  using result_type = size_t;

  result_type operator()(const argument_type& fd) const
  {
    switch (fd.type()) {
      case os::WindowsFD::FD_CRT: {
        return static_cast<result_type>(static_cast<int>(fd));
      }
      case os::WindowsFD::FD_HANDLE: {
        return reinterpret_cast<result_type>(static_cast<HANDLE>(fd));
      }
      case os::WindowsFD::FD_SOCKET: {
        return static_cast<result_type>(static_cast<SOCKET>(fd));
      }
    }
    UNREACHABLE();
  }
};

} // namespace std {

#endif // __STOUT_OS_WINDOWS_FD_HPP__
