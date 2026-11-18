// Numeric-field splitter of the treasure-class parser, moved from the stand-alone d2common tiny batch into the game tree.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2Common+0x535D0: consume the next numeric field of a mutable string cursor. The cursor sits
// one past the token after the separator is overwritten with a NUL; a missing separator leaves the
// cursor null and the whole remaining string is parsed. The parsed integer is the return value.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Common_535D0_SplitAndAtoi(char** ppCursor, char cSeparator,
    int32_t (*retailAtol)(const char*))
{
	char* const pStart = *ppCursor;
	char* const pSeparator = std::strchr(pStart, cSeparator);
	*ppCursor = pSeparator;
	if (pSeparator != nullptr) {
		*pSeparator = '\0';
		*ppCursor = pSeparator + 1;
	}
	// The retail CRT parser is required: host long width and overflow rules differ.
	return retailAtol(pStart);
}
