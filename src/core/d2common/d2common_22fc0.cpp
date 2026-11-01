// 1.13c D2Common+0x22FC0: running maximum with one row's level byte. eax = running value, ecx = a
// struct (byte +0x67 is the slot, +0x65 the base byte, +0x68 the alternate byte), edx = a struct whose
// dword +4 names the slot. The alternate byte is taken when the slot is not 0xFF and matches edx+4;
// the result is the larger of the running value and the chosen byte. A null ecx returns the running value.
#include <cstdint>
#include <cstring>

extern "C" int32_t D2Common_22FC0_RunningMax(int32_t running, const uint8_t* pRow, const uint8_t* pKey)
{
	if (!pRow)
		return running;

	const uint8_t slot = pRow[0x67];
	bool useAlternate = false;
	if (slot != 0xFF && pKey) {
		uint32_t keySlot = 0;
		std::memcpy(&keySlot, pKey + 4, sizeof(keySlot));
		useAlternate = (keySlot == slot);
	}

	const int32_t value = useAlternate ? pRow[0x68] : pRow[0x65];
	return (running > value) ? running : value;
}
