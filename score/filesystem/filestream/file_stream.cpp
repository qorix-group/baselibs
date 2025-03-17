#include "score/filesystem/filestream/file_stream.h"

score::ResultBlank score::filesystem::FileStream::Close()
{
    auto buf = rdbuf();
    if (buf != nullptr)
    {
        auto stdio_filebuf = dynamic_cast<details::StdioFileBuf*>(buf);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(stdio_filebuf != nullptr,
                               "FileStream must not be used with a streambuf that's not derived from StdioFileBuf");
        return stdio_filebuf->Close();
    }
    else
    {
        return {};
    }
}
