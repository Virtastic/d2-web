// The widescreen HUD art ships in git (play/freeres/). Every DC6 play/assets/wide-hud.mjs fetches must be
// there, decode, and have the frame sizes its SIZE table names, or the bottom bar draws only the 800 px centre.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync, statSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';
import { decodeDc6 } from '../play/assets/d2-formats.mjs';
import { FREERES_FILES, SIZE } from '../play/assets/wide-hud.mjs';

const FREERES = join(dirname(fileURLToPath(import.meta.url)), '..', 'play', 'freeres');

test('the art set is the 16 SGD2FreeRes panel files, every one a SIZE entry', () => {
  assert.equal(FREERES_FILES.length, 16);
  assert.deepEqual(FREERES_FILES, Object.keys(SIZE));
});

test('every DC6 wide-hud.mjs needs is tracked in play/freeres/ and is not empty', () => {
  for (const name of FREERES_FILES) {
    const path = join(FREERES, `${name}.dc6`);
    assert.ok(existsSync(path), `missing ${name}.dc6 in play/freeres/`);
    assert.ok(statSync(path).size > 0, `${name}.dc6 is empty`);
  }
});

test('the licence and the provenance README ship beside the art', () => {
  const licence = readFileSync(join(FREERES, 'LICENSE.md'), 'utf8');
  assert.match(licence, /GNU Affero General Public License/i);
  assert.ok(existsSync(join(FREERES, 'README.md')));
});

test('every DC6 decodes to exactly the frame sizes its SIZE entry names', () => {
  for (const name of FREERES_FILES) {
    const dc6 = decodeDc6(new Uint8Array(readFileSync(join(FREERES, `${name}.dc6`))));
    const want = SIZE[name];
    assert.equal(dc6.frames.length, want.length, `${name}: frame count`);
    dc6.frames.forEach((frame, i) => {
      assert.deepEqual([frame.width, frame.height], want[i], `${name} frame ${i}`);
    });
  }
});
