const fourcc = (bytes, offset) => String.fromCharCode(bytes[offset], bytes[offset + 1], bytes[offset + 2], bytes[offset + 3]);

/** Parse a RIFF/WAVE PCM stream after StormLib has applied MPQ-sector decompression. */
export function decodeWav(bytes) {
  if (!(bytes instanceof Uint8Array) || bytes.length < 12 || fourcc(bytes, 0) !== 'RIFF' || fourcc(bytes, 8) !== 'WAVE') {
    throw new TypeError('WAV must begin with RIFF/WAVE');
  }
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const riffLength = view.getUint32(4, true);
  if (riffLength + 8 > bytes.length) throw new RangeError('WAV RIFF length exceeds input');
  let format = null, dataOffset = -1, dataLength = 0;
  for (let offset = 12; offset + 8 <= riffLength + 8;) {
    const id = fourcc(bytes, offset), length = view.getUint32(offset + 4, true), payload = offset + 8;
    if (payload + length > riffLength + 8) throw new RangeError(`WAV ${id} chunk exceeds RIFF extent`);
    if (id === 'fmt ') {
      if (format || length < 16) throw new RangeError('WAV has invalid fmt chunk');
      format = { encoding: view.getUint16(payload, true), channels: view.getUint16(payload + 2, true),
        sampleRate: view.getUint32(payload + 4, true), byteRate: view.getUint32(payload + 8, true),
        blockAlign: view.getUint16(payload + 12, true), bitsPerSample: view.getUint16(payload + 14, true) };
    } else if (id === 'data' && dataOffset < 0) { dataOffset = payload; dataLength = length; }
    offset = payload + length + (length & 1);
  }
  if (!format || dataOffset < 0 || !format.channels || !format.sampleRate || !format.blockAlign || !format.bitsPerSample ||
      format.byteRate !== format.sampleRate * format.blockAlign || format.blockAlign !== format.channels * Math.ceil(format.bitsPerSample / 8) ||
      dataLength % format.blockAlign) throw new RangeError('WAV format/data invariant failed');
  if (format.encoding !== 1 || format.bitsPerSample !== 16) throw new RangeError(`unsupported WAV format ${format.encoding}/${format.bitsPerSample}`);
  return { ...format, dataOffset, dataLength, frames: dataLength / format.blockAlign, duration: dataLength / format.byteRate };
}
