// 1.13c D2Common implementation batch (rows 0x24840 and 0x24A50).
// Prior art: 's item-generation row; D2MOO's 1.10f
// ITEMS_CalculateTransactionCost is the closest named twin. The 1.13c call frames retain retail
// EAX and stack inputs as uint32_t values. Retail pointers in records are 32-bit handles and pass
// through resolveAddress; no retail layout field is read at wasm64 pointer width.
// This is the serialized-record adapter; native D2UnitStrc pointers require a separate typed adapter.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace
{
	struct ItemTxtView
	{
		uint8_t bytes[0x1A8];
	};
	static_assert(sizeof(ItemTxtView) == 0x1A8, "1.13c items.txt row size");
	static_assert(offsetof(ItemTxtView, bytes) == 0, "item row byte view starts at row base");

	int32_t itemI32(const ItemTxtView* row, size_t offset)
	{
		int32_t value;
		std::memcpy(&value, row->bytes + offset, sizeof(value));
		return value;
	}
	uint32_t itemU32(const ItemTxtView* row, size_t offset)
	{
		uint32_t value;
		std::memcpy(&value, row->bytes + offset, sizeof(value));
		return value;
	}
	uint8_t itemU8(const ItemTxtView* row, size_t offset)
	{
		return row->bytes[offset];
	}
	uint32_t retailU32(const void* base, size_t offset)
	{
		uint32_t value;
		std::memcpy(&value, static_cast<const uint8_t*>(base) + offset, sizeof(value));
		return value;
	}
	int32_t retailI32(const void* base, size_t offset)
	{
		int32_t value;
		std::memcpy(&value, static_cast<const uint8_t*>(base) + offset, sizeof(value));
		return value;
	}
	void retailWriteI32(void* base, int32_t value)
	{
		std::memcpy(base, &value, sizeof(value));
	}
	[[noreturn]] void fatalFallback()
	{
		__builtin_trap();
	}
}

// The decompile sees the input item in EAX and the item level in the caller's stack frame. Keep
// those locations explicit; environment callbacks model the globals and ordinal helpers used by
// retail. resolveAddress translates a 32-bit retail address into wasm memory.
struct D2Common24840Frame
{
	uint32_t eaxItem;
	int32_t stackItemLevel;
};
struct D2Common24840Env
{
	void* userData;
	uint32_t itemRecordCount;
	const uint8_t* itemsTxt;
	void* (*resolveAddress)(void* userData, uint32_t retailAddress);
	uint32_t (*getItemIndexFromCode)(void* userData, uint32_t itemCode);
	const uint8_t* (*getItemRecordFromCode)(void* userData, uint32_t itemCode);
	void (*fatal)(void* userData, uint32_t errorCode);
};

// 1.13c D2Common+0x24840: estimate the base gamble cost. 1.13c items.txt rows have stride 0x1A8;
// code links are +0x80/+0x88/+0x8C, level +0xFD, average-stack fields +0xE4/+0xE8, cost +0xE0,
// and the ring/amulet gamble cost is +0xD4. The caller's item level is a stack input, not an
// inferred wasm argument. Null global tables and out-of-range base records follow retail's fatal
// errors 0x1EF and 0xB1B respectively.
extern "C" int32_t D2Common_24840_CalculateGambleBaseCost(
	const D2Common24840Frame* frame, const D2Common24840Env* env)
{
	if (!frame || !env || !env->resolveAddress || !env->getItemIndexFromCode ||
		!env->getItemRecordFromCode)
		fatalFallback();
	auto fatal = [&](uint32_t code) -> void {
		if (env->fatal) env->fatal(env->userData, code);
		fatalFallback();
	};
	if (!frame->eaxItem) fatal(0x798);
	const void* item = env->resolveAddress(env->userData, frame->eaxItem);
	if (!item) fatalFallback();
	if (retailU32(item, 0) != 4) fatal(0x799);
	const uint32_t classId = retailU32(item, 4);
	// GetItemsTxtRecord returns null for an invalid class; GetNormalCode then faults.
	if (classId >= env->itemRecordCount) fatalFallback();
	if (!env->itemsTxt) fatal(0x1EF);
	const auto* input = reinterpret_cast<const ItemTxtView*>(env->itemsTxt + classId * 0x1A8u);
	const uint32_t normalCode = itemU32(input, 0x84);
	const uint32_t itemIndex = env->getItemIndexFromCode(env->userData,
		normalCode ? normalCode : itemU32(input, 0x80));
	if (itemIndex >= env->itemRecordCount) fatal(0xB1B);

	const auto* base = reinterpret_cast<const ItemTxtView*>(env->itemsTxt + itemIndex * 0x1A8u);
	const uint32_t baseCode = itemU32(base, 0x80);
	if (baseCode == 0x206e6972u || baseCode == 0x20756d61u)
		return itemI32(base, 0xD4);

	int32_t averageStack = static_cast<int32_t>(itemU32(base, 0xE8) + itemU32(base, 0xE4)) / 2;
	if (averageStack < 1)
		averageStack = 1;

	int32_t exceptionalWeight = 0;
	int32_t eliteWeight = 0;
	int32_t exceptionalCost = 0;
	int32_t eliteCost = 0;
	const uint32_t exceptionalCode = itemU32(base, 0x88);
	if (exceptionalCode != 0 && exceptionalCode != 0x20202030u)
	{
		const uint8_t* exceptional = env->getItemRecordFromCode(env->userData, exceptionalCode);
		if (exceptional)
		{
			const auto* row = reinterpret_cast<const ItemTxtView*>(exceptional);
			exceptionalWeight = static_cast<int32_t>((static_cast<uint32_t>(frame->stackItemLevel) - itemU8(row, 0xFD)) * 100u) / 2 + 1;
			if (exceptionalWeight < 0)
				exceptionalWeight = 0;
			exceptionalCost = itemI32(row, 0xE0);
		}
	}

	const uint32_t eliteCode = itemU32(base, 0x8C);
	if (eliteCode != 0 && eliteCode != 0x20202030u)
	{
		const uint8_t* elite = env->getItemRecordFromCode(env->userData, eliteCode);
		if (elite)
		{
			const auto* row = reinterpret_cast<const ItemTxtView*>(elite);
			const int32_t scaledLevel = static_cast<int32_t>((static_cast<uint32_t>(frame->stackItemLevel) - itemU8(row, 0xFD)) * 100u);
			eliteWeight = ((scaledLevel + (scaledLevel >> 31 & 3)) >> 2) + 1;
			if (eliteWeight < 0)
				eliteWeight = 0;
			eliteCost = itemI32(row, 0xE0);
		}
	}

	int32_t itemLevel = frame->stackItemLevel;
	if (itemLevel < 5)
		itemLevel = 5;
	const int32_t weightedCost =
		static_cast<int32_t>((10000u - static_cast<uint32_t>(eliteWeight) - static_cast<uint32_t>(exceptionalWeight)) *
		 itemU32(base, 0xE0) * static_cast<uint32_t>(averageStack) +
		 static_cast<uint32_t>(eliteCost) * static_cast<uint32_t>(eliteWeight) +
		 static_cast<uint32_t>(exceptionalCost) * static_cast<uint32_t>(exceptionalWeight)) / 10000;
	int32_t levelDelta = static_cast<int32_t>(itemU8(base, 0xFD)) - 45;
	if (levelDelta < 0)
		levelDelta = 0;
	const int32_t levelTerm =
		static_cast<int32_t>((static_cast<uint32_t>(levelDelta - itemU8(base, 0xFD) / 2) +
		 static_cast<uint32_t>(itemLevel)) * 250u) / 3;
	const int32_t levelFactor = static_cast<int32_t>(static_cast<uint32_t>(itemLevel) * 2u + 1u) / 3;
	return static_cast<int32_t>((static_cast<uint32_t>(levelTerm) + static_cast<uint32_t>(weightedCost)) *
		(static_cast<uint32_t>(levelFactor) + 20u)) / 15;
}

// The decompile receives the unit in EAX and three output pointers on the stack. The frame stores
// all four retail addresses at 32 bits; resolveAddress is the only bridge to wasm memory. The item
// and item-data link fields are read as uint32_t slots at +0x14 and +0x64.
struct D2Common24A50Frame
{
	uint32_t eaxUnit;
	uint32_t stackSellCost;
	uint32_t stackBuyCost;
	uint32_t stackRepairCost;
};
struct D2Common24A50Env
{
	void* userData;
	const uint8_t* itemsTxt;
	uint32_t itemRecordCount;
	void* (*resolveAddress)(void* userData, uint32_t retailAddress);
	void (*fatal)(void* userData, uint32_t errorCode);
};

// 1.13c D2Common+0x24A50: add half of each contained item's table cost to all three output totals.
// The inventory signature is unit+0x60 -> +0x0; first item is inventory+0x0C; item type/class id
// are +0x0/+0x4; item data is +0x14; its next link is +0x64. Retail stops when item data is null
// or the exact 32-bit sentinel -0x5C, and takes the fatal 0x1EF path when a valid class id needs a
// missing items.txt table.
extern "C" void D2Common_24A50_AddContainedItemCosts(
	const D2Common24A50Frame* frame, const D2Common24A50Env* env)
{
	if (!frame || !env || !env->resolveAddress)
		fatalFallback();
	auto* unit = static_cast<uint8_t*>(env->resolveAddress(env->userData, frame->eaxUnit));
	if (!unit) fatalFallback();
	const uint32_t inventoryAddress = retailU32(unit, 0x60);
	if (!inventoryAddress) return;
	auto* inventory = static_cast<uint8_t*>(env->resolveAddress(env->userData, inventoryAddress));
	if (!inventory) fatalFallback();
	uint32_t containedAddress = retailU32(inventory, 0) == 0x01020304u
		? retailU32(inventory, 0x0C) : 0;
	int32_t total = 0;
	while (containedAddress != 0)
	{
		auto* contained = static_cast<uint8_t*>(env->resolveAddress(env->userData, containedAddress));
		if (!contained) fatalFallback();
		const int32_t unitType = retailI32(contained, 0);
		uint32_t nextAddress = 0;
		if (unitType == 4)
		{
			const uint32_t itemDataAddress = retailU32(contained, 0x14);
			if (itemDataAddress != 0 && itemDataAddress != 0xFFFFFFA4u)
			{
				auto* itemData = static_cast<uint8_t*>(env->resolveAddress(env->userData, itemDataAddress));
				if (!itemData) fatalFallback();
				nextAddress = retailU32(itemData, 0x64);
			}
			const uint32_t classId = retailU32(contained, 0x04);
			if (classId >= env->itemRecordCount) fatalFallback();
			if (!env->itemsTxt)
			{
				if (env->fatal) env->fatal(env->userData, 0x01EF);
				fatalFallback();
			}
			const auto* row = reinterpret_cast<const ItemTxtView*>(env->itemsTxt + classId * 0x1A8u);
			const int32_t halfCost = itemI32(row, 0xE0) / 2;
			total = static_cast<int32_t>(static_cast<uint32_t>(total) + static_cast<uint32_t>(halfCost));
		}
		containedAddress = nextAddress;
	}

	// Keep read/add/write order: the three required output pointers may alias.
	for (uint32_t address : {frame->stackSellCost, frame->stackBuyCost, frame->stackRepairCost})
	{
		void* output = address ? env->resolveAddress(env->userData, address) : nullptr;
		if (!output) fatalFallback();
		retailWriteI32(output, static_cast<int32_t>(static_cast<uint32_t>(retailI32(output, 0)) +
			static_cast<uint32_t>(total)));
	}
}
