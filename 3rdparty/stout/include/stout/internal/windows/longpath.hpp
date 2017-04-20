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

#ifndef __STOUT_INTERNAL_WINDOWS_LONGPATH_HPP__
#define __STOUT_INTERNAL_WINDOWS_LONGPATH_HPP__

#include <string>

#include <assert.h>

#include <stout/path.hpp>
#include <stout/stringify.hpp>

#include <stout/os/constants.hpp>


namespace internal {
namespace windows {

// This function idempotently prepends "\\?\" to the given path (if it's
// absolute), resolves the path, and converts it to UTF-16, appropriate for use
// in Uniode versions of Windows filesystem APIs which support lengths greater
// than MAX_PATH.
inline std::wstring longpath(const std::string& path)
{
  // Only add the prefix if necessary.
  if (path::absolute(path) && !strings::startsWith(path, os::LONGPATH_PREFIX)) {
      return wide_stringify(os::LONGPATH_PREFIX + path);
  } else {
      return wide_stringify(path);
  }
}

inline std::wstring longpath(const std::wstring& path)
{
    return longpath(stringify(path));
}

} // namespace windows {
} // namespace internal {

#endif // __STOUT_INTERNAL_WINDOWS_LONGPATH_HPP__
