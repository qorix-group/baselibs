#ifndef SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_BUF_H
#define SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_BUF_H

#include "score/filesystem/path.h"

#include "score/result/result.h"

#include <iosfwd>
#include <iostream>

#if defined(__GLIBCXX__)  // We're using GCC's libstdc++

#include <ext/stdio_filebuf.h>
#define FILE_FACTORY_GLIBCXX

#elif defined(_LIBCPP_VERSION)  // We're using libstdcxx from LLVM

#include <fstream>
#define FILE_FACTORY_LLVMLIBCXX

#else

#error Unknown standard C++ library variant

#endif

namespace score::filesystem::details
{

#ifdef FILE_FACTORY_GLIBCXX

using StdioFilebufBase = __gnu_cxx::stdio_filebuf<char>;

#endif

#ifdef FILE_FACTORY_LLVMLIBCXX

class StdioFilebufBase : public std::filebuf
{
  public:
    StdioFilebufBase(int fd, std::ios::openmode mode) : std::filebuf{}
    {
        // Use a non-standard extension to associate the file descriptor with the filebuf
        __open(fd, mode);  // NOLINT(score-qnx-banned-builtin): This is not a builtin but an extension we need to use
    }
    ~StdioFilebufBase() override = default;
    StdioFilebufBase(StdioFilebufBase&&) = default;
    StdioFilebufBase& operator=(StdioFilebufBase&&) = default;
    StdioFilebufBase(const StdioFilebufBase&) = delete;
    StdioFilebufBase& operator=(const StdioFilebufBase&) = delete;
};

#endif

class StdioFileBuf : public StdioFilebufBase
{
  public:
    StdioFileBuf(int fd, std::ios::openmode mode) : StdioFilebufBase{fd, mode}, file_handle_{fd} {}
    StdioFileBuf& operator=(StdioFileBuf&&) = default;
    StdioFileBuf(StdioFileBuf&&) = default;
    StdioFileBuf(const StdioFileBuf&) = delete;
    StdioFileBuf& operator=(const StdioFileBuf&) = delete;
    ~StdioFileBuf() override = default;

    int sync() override;
    virtual ResultBlank Close();

  private:
    int file_handle_;
};

class AtomicFileBuf : public StdioFileBuf
{
  public:
    AtomicFileBuf(int fd, std::ios::openmode mode, Path from_path, Path to_path);
    AtomicFileBuf& operator=(AtomicFileBuf&&) = default;
    AtomicFileBuf(AtomicFileBuf&&) = default;
    AtomicFileBuf(const AtomicFileBuf&) = delete;
    AtomicFileBuf& operator=(const AtomicFileBuf&) = delete;
    ~AtomicFileBuf() override = default;

    ResultBlank Close() override;

  private:
    Path from_path_;
    Path to_path_;
};

}  // namespace score::filesystem::details

#endif  // SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_BUF_H
