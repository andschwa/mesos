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

#ifndef __STOUT_OS_WINDOWS_ENVIRONMENT_HPP__
#define __STOUT_OS_WINDOWS_ENVIRONMENT_HPP__

#include <map>
#include <string>


namespace os {

inline std::map<std::string, std::string> environment()
{
  char* env = GetEnvironmentStrings();
  std::map<std::string, std::string> result;

  int prev = 0;
  for(int i = 0; env[i] != '\0' && env[i+1] != '\0';) {
    std::string entry(env + i);
    i = i + entry.size() + 1;
    size_t position = entry.find_first_of('=');
    if (position == std::string::npos) {
      continue; // Skip malformed environment entries.
    }

    result[entry.substr(0, position)] = entry.substr(position + 1);
  }

  return result;
}

} // namespace os {

#endif // __STOUT_OS_WINDOWS_ENVIRONMENT_HPP__
