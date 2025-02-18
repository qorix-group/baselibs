#ifndef SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_STREAM_H
#define SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_STREAM_H

#include "score/filesystem/filestream/file_buf.h"

#include "score/result/result.h"

#include <score/utility.hpp>

#include <fstream>
#include <iostream>

namespace score::filesystem
{

class FileStream : public std::iostream
{
  public:
    explicit FileStream(details::StdioFileBuf* filebuf) : std::iostream{filebuf} {}
    ~FileStream() override = default;

    ResultBlank Close()
    {
        auto buf = std::ostream::rdbuf();
        auto stdio_filebuf = dynamic_cast<details::StdioFileBuf*>(buf);
        return stdio_filebuf->Close();
    }
};

template <typename Buf>
class FileStreamImpl : public FileStream
{
  public:
    ~FileStreamImpl() override
    {
        FileStream::Close();
    }

    template <typename... Args>
    explicit FileStreamImpl(Args&&... args) : FileStream{&buf_}, buf_{std::forward<Args>(args)...}
    {
    }

  private:
    Buf buf_;
};

}  // namespace score::filesystem

#endif  // SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_STREAM_H
