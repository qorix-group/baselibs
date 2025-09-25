#ifndef SCORE_LIB_UTILS_BASE64_H
#define SCORE_LIB_UTILS_BASE64_H

#include <cstdint>
#include <string>
#include <vector>

namespace score::utils
{

std::string EncodeBase64(const std::vector<std::uint8_t>& buffer);
std::vector<std::uint8_t> DecodeBase64(const std::string& encoded_string);

}  // namespace score::utils

#endif  // SCORE_LIB_UTILS_BASE64_H
