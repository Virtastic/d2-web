// Pins the DCC decoder's output byte for byte on a few retail player DCCs (one per class family, mixed modes):
// each digest is a SHA-256 over the container header digest and every direction digest (frame headers, palette
// map, stream sizes, indexed and opaque pixels), as tools/dcc-decode-bench.mjs computes them. The pinned
// values come from the decoder before the typed-array rewrite, so any change to any decoded byte fails here.
// Skips when the retail MPQs are absent (set D2_RETAIL_DIR to run it from a worktree).
import test from 'node:test';
import assert from 'node:assert/strict';
import { createHash } from 'node:crypto';
import { decodeDcc } from '../play/assets/unit-formats.mjs';
import { openRetail, dccDigests } from '../tools/dcc-decode-bench.mjs';

const PINNED = {
  'data\\global\\CHARS\\AI\\HD\\AIHDcrnA11HS.DCC': '597d4b79b3ffb73a08a5a7da1a07873beb5121bf4b5548451e8f65e568057515',
  'data\\global\\CHARS\\AM\\S1\\AMS1medA11HS.DCC': '197ce270a73a98b9b8e7b785869cf70d525e275c153849a00fd79475b78f2756',
  'data\\global\\CHARS\\DZ\\RA\\DZRAlitWL2HT.DCC': 'e20494726010ff744506dc79de539e0dac1d73007e3e012f274b3b0750e5a25a',
  'data\\global\\CHARS\\PA\\HD\\PAHDcrnWL1HS.DCC': '8a174f3292ac6a13174f85f7671be486580f511252c1d26c8b290f21114ce44a',
  'data\\global\\CHARS\\SO\\TR\\SOTRmedWLHTH.DCC': '57c0320e6263fedc4b69755fc33f94346f10988ea906f9c6e4d873f8bf1c7ce4',
};

const read = openRetail();

test('DCC decode output is byte-identical on pinned retail player DCCs', { skip: read ? false : 'retail MPQs not found (D2_RETAIL_DIR)' }, () => {
  for (const [path, expected] of Object.entries(PINNED)) {
    const { header, directions } = dccDigests(decodeDcc(read(path)));
    assert.equal(directions.length, 16, path);
    const digest = createHash('sha256').update([header, ...directions].join('\n')).digest('hex');
    assert.equal(digest, expected, path);
  }
});
