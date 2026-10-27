import { test } from 'node:test';
import assert from 'node:assert/strict';
import { readSessionTerrain, SESSION_TILE_BYTES } from '../play/assets/session-terrain.mjs';

// A fake engine: 3 tiles at heap 1024, entries at 2048, coordlists at 4096, one DT1 path at 8192.
function fakeModule() {
  const HEAPU8 = new Uint8Array(1 << 16), dv = new DataView(HEAPU8.buffer);
  const st = { x: 100, y: 200, calls: 0 };
  const tile = (i, wx) => { const b = 1024 + i * SESSION_TILE_BYTES; dv.setUint32(b, 7, true); dv.setInt32(b + 40, wx, true); dv.setUint32(b + 64, 0, true); dv.setUint32(b + 80, 0, true); };
  for (let i = 0; i < 3; i++) tile(i, i * 5);
  for (let i = 0; i < 3; i++) dv.setUint32(2048 + i * 4, i + 1, true);
  const path = 'data\\a.dt1'; for (let i = 0; i < path.length; i++) HEAPU8[8192 + i] = path.charCodeAt(i);
  const table = { d2_session_tile_count: () => 3, d2_session_tiles: () => 1024, d2_session_tile_entries: () => 2048, d2_session_tile_coordlists: () => 4096,
    d2_session_player_x: () => st.x, d2_session_player_y: () => st.y, d2_session_player_coordlist: () => 0, d2_session_hold_world: () => 0 };
  const Module = { HEAPU8, _d2_session_tile_entries: 1, _d2_session_player_coordlist: 1, _d2_session_hold_world: 1, _malloc: () => 8192, _free: () => {}, st, dv,
    ccall(name, ret, types, args) {
      if (name === 'd2_session_level_tile_files') { dv.setUint8(8192 + path.length, 0); return 1; }
      st.calls++; return table[name]();
    } };
  return Module;
}

test('readSessionTerrain returns the previous parse only while every raw byte is unchanged', () => {
  const M = fakeModule();
  const a = readSessionTerrain(M), b = readSessionTerrain(M);
  assert.equal(a.tiles.length, 3);
  assert.equal(b, a, 'unchanged world: same result object');
  M.dv.setInt32(1024 + 84 + 40, 999, true);   // tile 1 moves
  const c = readSessionTerrain(M);
  assert.notEqual(c, a); assert.equal(c.tiles[1].worldX, 999);
  M.st.x = 101;                                // player moved, tiles the same
  assert.notEqual(readSessionTerrain(M), c);
  M.dv.setUint32(2048 + 8, 77, true);          // an entry index changes
  assert.equal(readSessionTerrain(M).tiles[2].entry, 77);
});
