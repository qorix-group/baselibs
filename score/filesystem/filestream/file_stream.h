#ifndef SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_STREAM_H
#define SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_STREAM_H

#include "score/filesystem/filestream/file_buf.h"

#include "score/result/result.h"

#include <score/utility.hpp>

#include <fstream>
#include <iostream>

namespace score::filesystem
{

// NOLINTNEXTLINE(fuchsia-multiple-inheritance): The multiple inheritance comes from iostream. We can't change that.
class FileStream : public std::iostream
{
  public:
    explicit FileStream(details::StdioFileBuf* filebuf) : std::iostream{filebuf} {}
    ~FileStream() override = default;

    ResultBlank Close();
};

namespace details
{

template <typename Buf>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance): The multiple inheritance comes from iostream. We can't change that.
class FileStreamImpl : public FileStream
{
  public:
    template <typename... Args>
    explicit FileStreamImpl(Args&&... args) : FileStream{&buf_}, buf_{std::forward<Args>(args)...}
    {
    }

    FileStreamImpl(const FileStreamImpl&) = delete;

    FileStreamImpl& operator=(FileStreamImpl&& other) noexcept(false)
    {
        buf_ = std::move(other.buf_);
        return *this;
    }

    ~FileStreamImpl() override
    {
        Close();
    }

    FileStreamImpl& operator=(const FileStreamImpl&) = delete;
    FileStreamImpl(FileStreamImpl&& other) noexcept(false) : FileStream{&buf_}, buf_{std::move(other.buf_)} {}

  private:
    Buf buf_;
};

}  // namespace details

}  // namespace score::filesystem

#endif  // SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_STREAM_H
