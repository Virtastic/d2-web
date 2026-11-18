#pragma once
// A call through D2_CALL runs the function itself unless a test has installed a stand-in for it:
//   D2Seam<&AIUTIL_GetDistanceToCoordinates>::hook = [](D2UnitStrc* u, int32_t x, int32_t y) -> uint32_t { ... };
// The slot is empty in the game, so the call is one null check and the same direct call as before.
template <auto Fn>
struct D2Seam
{
	static inline decltype(Fn) hook = nullptr;
};

#define D2_CALL(fn, ...) (D2Seam<&fn>::hook ? D2Seam<&fn>::hook(__VA_ARGS__) : fn(__VA_ARGS__))
// The function's address for a table of callbacks: the stand-in when one is installed.
#define D2_FN(fn) (D2Seam<&fn>::hook ? D2Seam<&fn>::hook : &fn)
