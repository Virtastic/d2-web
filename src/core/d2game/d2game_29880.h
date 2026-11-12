// 1.13c D2Game+0x29880: formats a number into a caller buffer (table entry). ecx = the buffer, edx = the value;
// one C-runtime sprintf call (0x2A6D, caller cleans: buffer, the module's format string, value), whose result is
// the result. `fmt` is the format string's address as the caller's environment names it.
#pragma once
#include <cstdint>

namespace g29880 {
template <class E> uint32_t Format(E& e, uint32_t buf, uint32_t value, uint32_t fmt)
{
    return e.h(0x2A6D, 0, 0, 0, 0, 0, buf, fmt, value);
}
}
