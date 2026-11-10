#pragma once
#include <cstdint>
namespace d2::client {
// Native UCS-2 buffers, NUL terminated. The caller supplies sufficient destination
// capacity. Retail's temporary limits are 400 and 100 code units respectively.
void sub_8E600(char16_t* text, std::uint32_t token, const char16_t* replacement);
// Extraction starts four units after a marker and ends at '[' (or NUL); extracted
// content is shorter than 100 units, as required by retail's local array.
void sub_8E6D0(char16_t* text, const char16_t* marker);
// Fill a "<selectors>:<text>" format with n arguments (at most ten, each under 200 units); out holds the result.
void formatSelectors(char16_t* out, const char16_t* format, const char16_t* const* args, std::int32_t count);
}
