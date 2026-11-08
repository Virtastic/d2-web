import test from 'node:test';
import assert from 'node:assert/strict';
import { createLobbyFigure } from './lobby-figure.mjs';

const stat = (cls) => 'PX2Drealm,Ama,\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' + String.fromCharCode(cls) + '\xff'.repeat(11) + '\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00';

test('no character data in the statstring draws no figure', () => {
  const f = createLobbyFigure({ read: () => { throw new Error('unused'); }, decodeDatPalette: () => new Uint8Array(1024) });
  assert.equal(f.figure('garbage'), null);
  assert.equal(f.figure('PX2Drealm,Ama,\x00\x00\x00'), null);
});

test('art the player\'s assets lack is reported, never thrown into the lobby draw', () => {
  const asked = [];
  const f = createLobbyFigure({ read: (p) => { asked.push(p); if (/pal\.(dat|pl2)$/.test(p)) return new Uint8Array(1 << 20); throw new Error('no such file'); }, decodeDatPalette: () => new Uint8Array(1024) });
  assert.equal(f.figure(stat(1)), null);
  assert.ok(asked.some((p) => /CHARS.AM.COF.AMTNHTH\.COF$/.test(p)), asked.join());
  assert.equal(f.missing.length, 1);
});
