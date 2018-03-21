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

#ifndef __STOUT_DECOMPRESS_HPP__
#define __STOUT_DECOMPRESS_HPP__

#include <archive.h>
#include <archive_entry.h>

#include <glog/logging.h>

#include <stout/nothing.hpp>
#include <stout/os/int_fd.hpp>
#include <stout/os/open.hpp>
#include <stout/path.hpp>
#include <stout/try.hpp>

namespace archiver {

// Flags can be any of (or together):
//   ARCHIVE_EXTRACT_ACL
//   ARCHIVE_EXTRACT_FFLAGS
//   ARCHIVE_EXTRACT_PERM
//   ARCHIVE_EXTRACT_TIME

inline Try<Nothing> extract(
    const std::string& source,
    const std::string& destination,
    int flags = ARCHIVE_EXTRACT_TIME)
{
  // TODO(jeffaco): See notes on supporting destination, below

  // Get references to libarchive for reading/handling a compressed file

  std::unique_ptr<struct archive, std::function<void(struct archive*)>> reader(
      archive_read_new(),
      [](struct archive* p) { archive_read_close(p);  archive_read_free(p); });

  archive_read_support_format_all(reader.get());
  archive_read_support_filter_all(reader.get());

  std::unique_ptr<struct archive, std::function<void(struct archive*)>> writer(
      archive_write_disk_new(),
      [](struct archive* p) { archive_write_close(p); archive_write_free(p); });

  archive_write_disk_set_options(writer.get(), flags);
  archive_write_disk_set_standard_lookup(writer.get());

  // Open the compressed file for decompression
  // We do not use libarchive to open the file to insure we have proper
  // Unicode and long path handling on both Linux and Windows.
  Try<int_fd> fd = os::open(source, O_RDONLY | O_CLOEXEC, 0);

  if (fd.isError()) {
    return Error("Failed to open file '" + source + "': " + fd.error());
  }

  // Be certain that the input archive is closed on exit
  int_fd fd_real = fd.get();
  std::unique_ptr<int_fd, std::function<void(int_fd*)>> fd_closer(
      &fd_real,
      [](int_fd* fd) { os::close(*fd); });

  int result = archive_read_open_fd(
      reader.get(),
      fd.get(),
      10240);
  if (result) {
    return Error(archive_error_string(reader.get()));
  }

  while (true) {
    // Read the next header from the input stream
    struct archive_entry *entry;
    result = archive_read_next_header(reader.get(), &entry);
    if (result == ARCHIVE_EOF)
    {
      break;
    }
    if (result != ARCHIVE_OK)
    {
      return Error(archive_error_string(reader.get()));
    }

    // TODO(jeffaco): Supporting destination. Question: Do we want this?
    //
    // If we want to support the destination, then the entry contains the
    // pathname to write (i.e. archive_entry_pathname(entry)).
    //
    // This will take some thought, because the entry might be a symlink,
    // or a hardlink, and we need to investigate how that looks. Right now,
    // archive_entry_pathname() returns raw path, assuming you are in the
    // destination directory. We can certainly use os::join functions to
    // build an absolute path with destination, if that's what we want.

    result = archive_write_header(writer.get(), entry);
    if (result != ARCHIVE_OK)
    {
      return Error(archive_error_string(writer.get()));
    }

    const void *buff;
    size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
    int64_t offset;
#else
    off_t offset;
#endif

    while (true) {
      result = archive_read_data_block(
          reader.get(),
          &buff,
          &size,
          &offset);
      if (result == ARCHIVE_EOF)
      {
          break;
      }
      if (result != ARCHIVE_OK)
      {
        return Error(archive_error_string(reader.get()));
      }

      result = archive_write_data_block(writer.get(), buff, size, offset);
      if (result != ARCHIVE_OK) {
        return Error(archive_error_string(writer.get()));
      }
    }

    result = archive_write_finish_entry(writer.get());
    if (result != ARCHIVE_OK)
    {
      return Error(archive_error_string(writer.get()));
    }
  }

  return Nothing();
}

} // namespace archiver {

#endif // __STOUT_DECOMPRESS_HPP__
