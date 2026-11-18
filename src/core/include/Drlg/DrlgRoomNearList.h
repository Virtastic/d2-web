#pragma once

#include <cstddef>
#include <cstdint>

namespace d2 {
namespace drlg {

template <typename T, std::size_t Capacity>
inline bool appendNearRoom(T (&rooms)[Capacity], int32_t& count, const T& room) noexcept
{
	if (count < 0 || static_cast<std::size_t>(count) >= Capacity)
	{
		return false;
	}

	rooms[count++] = room;
	return true;
}

} // namespace drlg
} // namespace d2
