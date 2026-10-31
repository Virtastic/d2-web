// D2Game+0x1020 -- the player's quest-flag block for the game's difficulty.
// eax = player, stack game (ret 4): PlayerData(player) via #10920, then the dword at +0x10 + difficulty*4,
// difficulty being the byte at game+0x6d.
// The one definition, d2game_1020_probe, lives in d2game_batch_1480.cpp (kept from main); this file
// carries the spec only so the port has a single owner.
