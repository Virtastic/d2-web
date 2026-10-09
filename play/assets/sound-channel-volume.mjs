/**
 * Re-derive D2Client 1.13c's per-source channel value before it reaches D2Sound.
 * D2Client truncates after each integer division by 100, then D2Sound's default
 * global scale maps the 0..255 channel to amplitude by dividing by 255.
 */
export function retailSoundChannelVolume(sourceVolume, masterPercent, musicPercent = 100, isMusic = false) {
  const source = Number(sourceVolume), master = Number(masterPercent), music = Number(musicPercent);
  if (!Number.isInteger(source) || source < 0 || source > 255) throw new RangeError('source volume must be an integer from 0 to 255');
  if (!Number.isInteger(master) || master < 0 || master > 100) throw new RangeError('master volume must be an integer from 0 to 100');
  if (!Number.isInteger(music) || music < 0 || music > 100) throw new RangeError('music volume must be an integer from 0 to 100');
  let channel = source;
  if (isMusic) channel = Math.trunc(channel * music / 100);
  channel = Math.trunc(channel * master / 100);
  return channel;
}

export function soundSourceGain(sourceVolume, masterPercent, musicPercent = 100, isMusic = false) {
  return retailSoundChannelVolume(sourceVolume, masterPercent, musicPercent, isMusic) / 255;
}

export function sliderPercent(value) {
  const n = Number(value);
  return Math.max(0, Math.min(100, Math.trunc((Number.isFinite(n) ? n : 0) * 100 + 1e-7)));
}
