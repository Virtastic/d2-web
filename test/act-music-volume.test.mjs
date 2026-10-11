import test from 'node:test';
import assert from 'node:assert/strict';
import { ACT_MUSIC, resolveActMusicSourceVolume, retailMusicChannelGain } from '../play/assets/act-music.mjs';

test('town music gain includes the native Sounds.txt base volume before slider gain', () => {
  const table = [
    'Sound\tIndex\tFileName\tVolume\tMusic Vol',
    'music_town_1\t4673\tact1\\town1.wav\t110\t1',
    'music_town_2\t4674\tact2\\town2.wav\t110\t1',
    'music_town_3\t4675\tact3\\town3.wav\t110\t1',
    'music_town_4\t4676\tact4\\town4.wav\t110\t1',
    'music_xtown\t4684\tact5\\xtown.wav\t110\t1',
  ].join('\n');
  for (const path of ACT_MUSIC) assert.equal(resolveActMusicSourceVolume(table, path), 110);
  assert.equal(retailMusicChannelGain(110, 0.5, 1), 55 / 255);
  // D2Client truncates after Music and again after Master: 110*.55 -> 60, 60*.8 -> 48.
  assert.equal(retailMusicChannelGain(110, 0.55, 0.8), 48 / 255);
  assert.notEqual(retailMusicChannelGain(110, 0.55, 0.8), (110 / 255) * 0.55 * 0.8);
});

test('town music gain rejects a non-music row and an ambiguous Sounds.txt match', () => {
  const header = 'Sound\tIndex\tFileName\tVolume\tMusic Vol';
  assert.throws(() => resolveActMusicSourceVolume(`${header}\nambient\t1\tact1\\town1.wav\t110\t0`, ACT_MUSIC[0]), /found 0/);
  assert.throws(() => resolveActMusicSourceVolume(`${header}\nmusic_a\t1\tact1\\town1.wav\t110\t1\nmusic_b\t2\tact1\\town1.wav\t110\t1`, ACT_MUSIC[0]), /found 2/);
});
