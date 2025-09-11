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

#include "score/json/json_writer.h"
#include "score/language/safecpp/safe_math/safe_math.h"
#include "score/json/i_json_writer.h"
#include "score/json/internal/model/error.h"
#include "score/json/internal/writer/json_serialize/json_serialize.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <locale>
#include <sstream>
#include <string_view>
#include <type_traits>

namespace
{

template <typename T>
score::ResultBlank ToFileInternal(const T& json_data,
                                const score::cpp::string_view& file_path,
                                score::filesystem::IFileFactory& file_factory)
{
    const std::string file_path_string{file_path.data(), file_path.size()};
    const auto file = file_factory.Open(file_path_string, std::ios::out | std::ios::trunc);
    if (!file.has_value())
    {
        auto error = score::json::MakeError(score::json::Error::kInvalidFilePath, "Failed to open file");
        return score::ResultBlank{score::unexpect, error};
    }

    score::json::JsonSerialize serializer{**file};
    return serializer << json_data;
}

template <class U>
constexpr std::size_t max_dec_digits() noexcept
{
    U v = std::numeric_limits<U>::max();
    std::size_t n = 1U;
    while (v >= 10U)
    {
        v /= 10U;
        ++n;
    }
    return n;
}

template <typename T>
inline constexpr std::size_t kIntBufLen = max_dec_digits<std::make_unsigned_t<std::remove_cv_t<T>>>() + 1U;

template <typename T>
inline auto abs_magnitude_unsigned(T val) noexcept
{
    using U = std::make_unsigned_t<std::remove_cv_t<T>>;
    static_assert(std::is_integral_v<T>, "integral only");
    static_assert(std::is_integral_v<U>, "U must be integral");
    static_assert(std::numeric_limits<U>::digits >= std::numeric_limits<std::remove_cv_t<T>>::digits,
                  "U must represent full magnitude of T");

    if constexpr (std::is_signed_v<T>)
    {
        const auto abs_val = score::safe_math::Abs(val);
        const auto cast_res = score::safe_math::Cast<U>(abs_val);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(cast_res.has_value(), "Safe cast failed in abs_magnitude_unsigned (signed)");
        return cast_res.value();
    }
    else
    {
        const auto cast_res = score::safe_math::Cast<U>(val);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(cast_res.has_value(), "Safe cast failed in abs_magnitude_unsigned (unsigned)");
        return cast_res.value();
    }
}

template <typename T, typename C>
[[nodiscard]] std::string_view integer_to_chars(C& buffer, const T val) noexcept
{
    using U = std::make_unsigned_t<std::remove_cv_t<T>>;
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "integral only");
    static_assert(!std::is_same_v<std::remove_cv_t<T>, bool>, "bool not supported");
    static_assert(std::is_same_v<typename C::value_type, char>, "container must hold char");
    static_assert(std::is_base_of_v<std::bidirectional_iterator_tag,
                                    typename std::iterator_traits<typename C::iterator>::iterator_category>,
                  "iterator must be bidirectional");
    static_assert(std::numeric_limits<U>::digits >= std::numeric_limits<std::remove_cv_t<T>>::digits,
                  "U must represent full magnitude of T");

    const bool is_negative = (std::is_signed_v<T> && (val < 0));
    U x = abs_magnitude_unsigned(val);

    auto it = buffer.end();
    do
    {
        const auto digit = static_cast<U>(x % static_cast<U>(10U));
        *(--it) = static_cast<char>('0' + digit);
        x /= 10U;
    } while (x > static_cast<U>(0U));
    if (is_negative)
        *(--it) = '-';

    return std::string_view(&*it, static_cast<std::size_t>(std::distance(it, buffer.end())));
}

class OptimizedNumPut : public std::num_put<char>
{
  public:
    using std::num_put<char>::num_put;

  protected:
    using std::num_put<char>::do_put;

    iter_type do_put(iter_type out, std::ios_base& s, char_type fill, long v) const override
    {
        return OptimizedPutForInts(out, s, fill, v);
    }
    iter_type do_put(iter_type out, std::ios_base& s, char_type fill, unsigned long v) const override
    {
        return OptimizedPutForInts(out, s, fill, v);
    }
    iter_type do_put(iter_type out, std::ios_base& s, char_type fill, long long v) const override
    {
        return OptimizedPutForInts(out, s, fill, v);
    }
    iter_type do_put(iter_type out, std::ios_base& s, char_type fill, unsigned long long v) const override
    {
        return OptimizedPutForInts(out, s, fill, v);
    }

  private:
    template <typename T>
    iter_type OptimizedPutForInts(iter_type out, std::ios_base& str, char_type fill, T val) const
    {
        std::array<char, kIntBufLen<T>> buf{};
        const auto sv = integer_to_chars<T>(buf, val);

        const auto width = static_cast<std::size_t>(str.width());
        const auto n = sv.size();
        if (width > n)
            out = std::fill_n(out, width - n, fill);
        return std::copy(sv.begin(), sv.end(), out);
    }
};

template <typename T>
score::ResultBlank ToFileInternalAtomic(const T& json_data,
                                      const std::string_view& file_path,
                                      score::filesystem::IFileFactory& file_factory,
                                      const score::filesystem::AtomicUpdateOwnershipFlags atomic_ownership)

{
    return file_factory.AtomicUpdate(std::string{file_path}, std::ios::out | std::ios::trunc, atomic_ownership)
        .transform_error([](auto err) noexcept {
            return score::json::MakeError(score::json::Error::kInvalidFilePath, err.UserMessage());
        })
        .and_then([&json_data](auto filestream) -> score::ResultBlank {
            score::json::JsonSerialize serializer{*filestream};
            auto serializer_result = serializer << json_data;
            return filestream->Close().and_then([serializer_result](auto) noexcept {
                return serializer_result;
            });
        });
}

template <typename T>
score::Result<std::string> ToBufferInternal(const T& json_data)
{
    // This line must be hit when the function is called. Since other parts of this function show line coverage,
    // this line must also be hit. Missing coverage is due to a bug in the coverage tool
    std::ostringstream string_stream{};  // LCOV_EXCL_LINE
    std::locale loc(std::locale(), new OptimizedNumPut());
    string_stream.imbue(loc);

    score::json::JsonSerialize serializer{string_stream};
    serializer << json_data;
    return string_stream.str();
}

}  // namespace

score::json::JsonWriter::JsonWriter(FileSyncMode file_sync_mode,
                                  score::filesystem::AtomicUpdateOwnershipFlags ownership) noexcept
    : file_sync_mode_{file_sync_mode}, atomic_ownership_{ownership}
{
}

score::ResultBlank score::json::JsonWriter::ToFile(const score::json::Object& json_data,
                                               const score::cpp::string_view& file_path,
                                               std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return (file_sync_mode_ == FileSyncMode::kSynced)
               ? ToFileInternalAtomic(
                     json_data, std::string_view{file_path.begin(), file_path.size()}, *file_factory, atomic_ownership_)
               : ToFileInternal(json_data, file_path, *file_factory);
}

score::ResultBlank score::json::JsonWriter::ToFile(const score::json::List& json_data,
                                               const score::cpp::string_view& file_path,
                                               std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return (file_sync_mode_ == FileSyncMode::kSynced)
               ? ToFileInternalAtomic(
                     json_data, std::string_view{file_path.begin(), file_path.size()}, *file_factory, atomic_ownership_)
               : ToFileInternal(json_data, file_path, *file_factory);
}

score::ResultBlank score::json::JsonWriter::ToFile(const score::json::Any& json_data,
                                               const score::cpp::string_view& file_path,
                                               std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return (file_sync_mode_ == FileSyncMode::kSynced)
               ? ToFileInternalAtomic(
                     json_data, std::string_view{file_path.begin(), file_path.size()}, *file_factory, atomic_ownership_)
               : ToFileInternal(json_data, file_path, *file_factory);
}

score::Result<std::string> score::json::JsonWriter::ToBuffer(const score::json::Object& json_data)
{
    return ToBufferInternal(json_data);
}

score::Result<std::string> score::json::JsonWriter::ToBuffer(const score::json::List& json_data)
{
    return ToBufferInternal(json_data);
}

score::Result<std::string> score::json::JsonWriter::ToBuffer(const score::json::Any& json_data)
{
    return ToBufferInternal(json_data);
}
