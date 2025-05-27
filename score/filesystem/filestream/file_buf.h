/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#ifndef SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_BUF_H
#define SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_BUF_H

#include "score/filesystem/path.h"

#include "score/result/result.h"

#include <iosfwd>
#include <iostream>

// The usage is pre-processor here is imperative to differentiate between
// GCC's libstdc++ and LLVM's libstdcxx.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__GLIBCXX__)  // We're using GCC's libstdc++

#include <ext/stdio_filebuf.h>
// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#define FILE_FACTORY_GLIBCXX

#elif defined(_LIBCPP_VERSION)  // We're using libstdcxx from LLVM

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#include <fstream>
// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#define FILE_FACTORY_LLVMLIBCXX

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#else

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#error Unknown standard C++ library variant

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#endif
namespace score::filesystem::details
{

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#ifdef FILE_FACTORY_GLIBCXX

using StdioFilebufBase = __gnu_cxx::stdio_filebuf<char>;

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#endif

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#ifdef FILE_FACTORY_LLVMLIBCXX

class StdioFilebufBase : public std::filebuf
{
  public:
    StdioFilebufBase(int fd, std::ios::openmode mode) : std::filebuf{}, file_handle_{fd}
    {
        // Use a non-standard extension to associate the file descriptor with the filebuf
        // coverity[autosar_cpp14_a0_1_2_violation]
        __open(fd, mode);  // NOLINT(score-qnx-banned-builtin): This is not a builtin but an extension we need to use
    }
    ~StdioFilebufBase() override = default;
    StdioFilebufBase(const StdioFilebufBase&) = delete;
    StdioFilebufBase& operator=(const StdioFilebufBase&) = delete;
    // Need to be compatible with the GCC counterpart.
    // coverity[autosar_cpp14_a12_8_6_violation]
    StdioFilebufBase(StdioFilebufBase&&) = default;
    // Need to be compatible with the GCC counterpart.
    // coverity[autosar_cpp14_a12_8_6_violation]
    StdioFilebufBase& operator=(StdioFilebufBase&&) = default;

    int fd() const noexcept
    {
        return file_handle_;
    }

  private:
    int file_handle_;
};

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#endif

class StdioFileBuf : public StdioFilebufBase
{
  public:
    using StdioFilebufBase::StdioFilebufBase;
    StdioFileBuf(const StdioFileBuf&) = delete;
    StdioFileBuf& operator=(const StdioFileBuf&) = delete;
    ~StdioFileBuf() override = default;
    // Need to be public as the class is used directly and moving is needed.
    // coverity[autosar_cpp14_a12_8_6_violation]
    StdioFileBuf& operator=(StdioFileBuf&&) = default;
    // Need to be public as the class is used directly and moving is needed.
    // coverity[autosar_cpp14_a12_8_6_violation]
    StdioFileBuf(StdioFileBuf&&) = default;

    int sync() override;
    virtual ResultBlank Close();
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
