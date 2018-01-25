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
    FD_CRT,
    FD_HANDLE,
    FD_SOCKET
  };

  WindowsFD() = default;

  WindowsFD(int crt)
    : type_(FD_CRT),
      crt_(crt),
      handle_(
          crt < 0 ? INVALID_HANDLE_VALUE
                  : reinterpret_cast<HANDLE>(::_get_osfhandle(crt))) {}

  // IMPORTANT: The `HANDLE` here is expected to be file handles. Specifically,
  //            `HANDLE`s returned by file API such as `CreateFile`. There are
  //            APIs that return `HANDLE`s with different error values, and
  //            therefore must be handled accordingly. For example, a thread API
  //            such as `CreateThread` returns `NULL` as the error value, rather
  //            than `INVALID_HANDLE_VALUE`.
  // TODO(mpark): Consider adding a second parameter which tells us what the
  //              error values are.
  WindowsFD(HANDLE handle)
    : type_(FD_HANDLE),
      crt_(
          handle == INVALID_HANDLE_VALUE
            ? -1
            : ::_open_osfhandle(reinterpret_cast<intptr_t>(handle), O_RDWR)),
      handle_(handle) {}

  WindowsFD(SOCKET socket) : type_(FD_SOCKET), socket_(socket) {}

  // On Windows, libevent's `evutil_socket_t` is set to `intptr_t`.
  WindowsFD(intptr_t socket)
    : type_(FD_SOCKET),
      socket_(static_cast<SOCKET>(socket)) {}

  WindowsFD(const WindowsFD&) = default;
  WindowsFD(WindowsFD&&) = default;

  ~WindowsFD() = default;

  WindowsFD& operator=(const WindowsFD&) = default;
  WindowsFD& operator=(WindowsFD&&) = default;

  int crt() const
  {
    CHECK((type() == FD_CRT) || (type() == FD_HANDLE));
    return crt_;
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
    return static_cast<intptr_t>(socket_);
  }

  operator int() const
  {
    switch (type()) {
      case FD_CRT:
      case FD_HANDLE: {
        return crt_;
      }
      case FD_SOCKET: {
        // Users of this class expect invalid sockets to be < 0 (like in POSIX
        // environments); however, on Windows the value is `INVALID_SOCKET`. So
        // we check for that, and then behave "as expected" instead. This is
        // significantly easier than fixing all socket code to check for two
        // different error behaviors.
        if (socket_ == INVALID_SOCKET) {
          return -1;
        }
        return static_cast<int>(socket_);
      }
    }
    UNREACHABLE();
  }

  Type type() const { return type_; }

private:
  Type type_;

  union
  {
    // We keep both a CRT FD as well as a `HANDLE`
    // regardless of whether we were constructed
    // from a file or a handle.
    //
    // This is because once we request for a CRT FD
    // from a `HANDLE`, we're required to close it
    // via `_close`. If we were to do the conversion
    // lazily upon request, the resulting CRT FD
    // would be dangling.
    struct
    {
      int crt_;
      HANDLE handle_;
    };
    SOCKET socket_;
  };
};


inline std::ostream& operator<<(std::ostream& stream, const WindowsFD& fd)
{
  switch (fd.type()) {
    case WindowsFD::FD_CRT: {
      stream << fd.crt();
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
        return static_cast<result_type>(fd.crt());
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
