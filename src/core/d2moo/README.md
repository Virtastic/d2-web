# Vendored D2MOO

Source: https://github.com/ThePhrozenKeep/D2MOO @ `5596f5cb6c5251a0a07c6637d26458b06099d516`
`source/D2Common/{include,src}`: `Drlg/*`, `D2Dungeon`, `D2Collision`, `D2Seed`, `D2StatList`,
`D2Inventory`, `D2Skills`, `D2States`, `D2Composit`, `Items/{Items,ItemMods}`, `Units/*`, all
`DataTbls/*`, `Fog/{Calc,BitManip}` and their headers; `source/D2Game`: `src/ITEMS/{Items,ItemsMagic,
ItemMode}.cpp` + `include/*` (headers only beyond ITEMS). `squall/` holds the public-domain Storm
headers D2MOO uses (`.cpp` files dropped). License: MIT (`LICENSE`).

D2MOO describes the 1.10f binaries; this copy is built as 1.13c through `../compat/`. The tree is
kept diffable against upstream: every local change is a `// d2-web:` comment on the changed line or
a `// 1.13c <DLL>+0x<offset>` marker naming the 1.13c function the code corresponds to, where its
behaviour differs from 1.10f.
