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
#include <stout/path.hpp>
#include <stout/try.hpp>

namespace decompress {

enum Format
{
  // These are a subset of formats supported by libarchive, but this
  // is likely all that Mesos cares about.
  FORMAT_ANY = 0,
  FORMAT_BZIP2,
  FORMAT_COMPRESS,
  FORMAT_CPIO,
  FORMAT_GZIP,
  FORMAT_TAR,
  FORMAT_ZIP
};


// Flags can be any of (or together):
//   ARCHIVE_EXTRACT_ACL
//   ARCHIVE_EXTRACT_FFLAGS
//   ARCHIVE_EXTRACT_PERM
//   ARCHIVE_EXTRACT_TIME

inline Try<Nothing> extract(
    const std::string& source,
    const std::string& destination,
    int format = Format::FORMAT_ANY,
    int flags = ARCHIVE_EXTRACT_TIME)
{
  // TODO(jeffaco): Determine how to tell libarchive where to put destination

  // libdecompress, if no filename is specified, will try to read from stdin.
  // This is one quick way to avoid that.
  // TODO(jeffaco): Can stdin be disabled from libarchive programmatically?
  if (source.empty())
  {
    return Error("No filename specified to decompress");
  }

  // Get references to libarchive for reading/handling a compressed file
  // TODO(jeffaco): We only support FORMAT_ANY for now. Consider if this
  //     should be restricted to specific formats or return an error if
  //     format doesn't match. By default, libarchive will examine the
  //     data stream to figure out what the format is and then just use it.

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
  // TODO(jeffaco): A fair amount of work was done to support Unicode
  //     and long paths. Callback for opening file? Or some way to pass
  //     a handle? If not, investigate if this will work properly on
  //     all of our platforms.
  int result;
  if (result = archive_read_open_filename(
      reader.get(),
      source.c_str(),
      10240))
  {
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

} // namespace decompress {

#endif // __STOUT_DECOMPRESS_HPP__
