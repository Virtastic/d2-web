//#region src/bik-constants.ts
const createArrayOfLen = (length) => Array.from({ length });
const unpackValues = (n, s) => {
	const len = s.length / n;
	return createArrayOfLen(len).fill(0).map((_, index) => parseInt(createArrayOfLen(n).fill(0).map((_2, i) => s[i * len + index]).join(""), 36));
};
const PACKED_BIK_TREE_LENS = "4444444444444444145555555555555522445555555555552334445555555555333344445555555533444444444455552444444444555555133555666666666612556666666666661344555566666666223445555566666614444555556666662225556666666666133366667777777713335677777777772233366666777777";
const PACKED_BIK_TREE_CODES = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000101010101000001010101010100000000010101010000000000010101000000000001010100000001010101010000010102130213000000130213021300000010101302130123456789abcdef013579bdfjlnprtv02195ldt3jbr7nfv026195dt3jbr7nfv0426195d3jbr7nfv042a6e195d3b7nfv02a6e195d3br7nfv0153jbrn73njfbvr013jb7rn73njfbvr015d3jbr73njfbvr0215d3jbr7njfbvr0195d3jbr7njfbvr0213jbrn73njfbvr015373njf7b3vnrj01537njbf7b3vnrj0215373njfb3vnrj";
const PACKED_BIK_PATTERNS = "000001111110000000000111111000000000111111110000000011111111000011111111111100000000000000000000000000000000000011111111111111110000000000000000011111111111100000000000000000001111111111111111000000000000000001010101111111111111111111111111000000000000000000000000000000000000000000000000111111111111111100111111111111000000000000000000000011111111111100000000000000001111111111111111000000000000000000000000000000001111111101010101111111111111111100000000000000000000000000000000000011111111111111111111111111110000000000000000011000111111111111111111111111110000000000000000000000000000000000000000000000001111111111111111001111111111110000000000000000000101010111111111111111111111111100000000000000000000000000000000000000000000000011111111111111110111111110001111000000000000000000111001111111111111111111111111000000000000000000000000000000000000000000000000111111111111111111100001111111110000000000000000000000000000000011111111111111111111111110000111000000000011111111111100000000000000000011111111111111110000000008gow4ckld5xph912aiqy6emnf7zrjb34cks08goph91tld56emu2aiqrjb3vnf7nmlkcdef7654wxyzrqpoghijba9801234567fedcklmnvuts0123ba98ghijrqpophiqrjb32a9108gow4ckld56emnf7zyxtlmuvnf76ed54cks08goph9aiqrjb3213b2a1908gohpiqjrz7y6x5w4ckdlemfnogphqirjb3a29180sktlumvnf7e6d5c4opgh890123abijqrstklcd4567efmnuv32bajirqpohg9810zy76fenmlkdc54xw012389abghijopqrwxyz4567cdefklmn4567cdefklmnstuv012389abghijopqr67fed5c43b2a9108gohpiqjrksltmunv3b2a1908z7y6x5w4dcklmenfogphijrq01234567fedcba98ghijklmnvutsrqpowxyz0123ba987654cdefghijrqponmlk089123baijrqphgow45xyz76edcklmnfgopqrjih98012ab3vnmutskldc456ef7opgh890123abijqrstklcd4567efmnuv23abijqropgh8901yz67efmnklcd45wx08192a3bjriqhpgow4x5y6z7fnemdlckogphqirjb3a29180vnumtlskc4d5e6f708gopqrjb3219hia4ckstuvnf765dlme08gopqrjb3219hiaw4cklmnf7zyx5de6089123bajrqihgopxw456yz7fnmedlkcgophiqrjb32a9801tsklmuvnef765dc4og80123bjrqpha9iskc4567fnvutledmog80123bjrqpha9ikc4wxyz7fnmld65e08912aihgopqrjb376efnmld54ckstuvrqijba9hpog80123kcdlme654wxyz7fn0189ghopwx45cdklmnef67yzqrijab2345cdklst0189ghopqrijab23uvmnef67";
const PACKED_AUDIO_CRITICAL_FREQS = "00000000001111222344579b258behlpuz5bjs3fue3xxb9yskc46iek0a4skg0is88swwck";
const PACKED_AUDIO_RLE_LENGTH_TABLE = "000111222223337egow4cs08gow4ck48";
const PACKED_BIK_SCAN = "0000000000000000000000000000001100111111111100001111111111111111018923ab45cd67efklstmnuvghopwx45yz67cdklefmnijqr018923abghopijqr";
const buildQuantTables = () => {
	const tables = [];
	const baseQuant = unpackValues(4, "11231143113210102232101034232322322312112211333332211010111110001223113211210010212110102323221222221100110032222111100011000000eyopuy4feb531jgwzmirsxwt3asvjj7r3sm2nm4jh8ulwin58cjwmrbnsfcb8mwje2b7yu6vk8fwwgbpezdvdpanec950snamd506soxoiyu5tyo3nse5lyha0qlpde7k5fr2lxpk6zrm67nd3c9t1dqhnc6n4gvhc3yvw4meg2fa83mxfva5w1az2t9fnkykiaj7wj3wpovhkgepdh5529kpareotlnzhvzr2evca6rayt5ulkq1sxt6a58x7mxg1xpjz2ogi6vom0m47tz5ftgr9aekmnura7g6c0w48u30wpasfmbljblwgxtlh8vguh077we04awyg6dtl5pzcczt3djwqhv557xe1y58ygdvvo0oh96usw9k9rl3pot");
	for (let level = 0; level < 32; level++) {
		const factor = [
			1,
			4 / 3,
			5 / 3,
			2,
			8 / 3,
			7 / 2,
			4,
			5,
			6,
			8,
			12,
			17,
			22,
			28,
			34,
			44
		][level % 16];
		const subTableOffset = level > 15 ? 64 : 0;
		for (let i = 0; i < 64; i++) tables.push(Math.round(baseQuant[i + subTableOffset] * factor));
	}
	return tables;
};
//#endregion
//#region src/bik-decoder-utils.ts
var BitReader = class {
	#buffer;
	#bufferBitLength;
	#bitPos = 0;
	constructor(buffer) {
		this.#buffer = buffer;
		this.#bufferBitLength = buffer.length << 3;
	}
	l(buffer) {
		this.#buffer = buffer;
		this.#bufferBitLength = buffer.length << 3;
		this.#bitPos = 0;
	}
	/**
	* Read the specified number of bits from the bit-stream.
	*
	* @param n Number of bits to read. Must be less than or equal to 32.
	* @param peek When `true`, do not update the bit position in the bit-stream, otherwise do update
	*   it.
	* @returns The bits read from the bit-stream.
	*/
	a(n, peek = false) {
		let result = 0;
		let bitPos = this.#bitPos;
		let bitsRemaining = n;
		do {
			const mask = (1 << bitsRemaining) - 1 & 255;
			const bits = (this.#buffer[bitPos >>> 3] ?? 0) >>> (bitPos & 7) & mask;
			result |= bits << n - bitsRemaining;
			if ((bitPos & 7) + bitsRemaining > 7) {
				const posChange = 8 - (bitPos & 7);
				bitsRemaining -= posChange;
				bitPos += posChange;
			} else {
				bitPos += bitsRemaining;
				break;
			}
		} while (bitsRemaining);
		if (!peek) this.#bitPos = bitPos;
		return result;
	}
	c() {
		return (this.#buffer[this.#bitPos >>> 3] ?? 0) >>> (this.#bitPos++ & 7) & 1;
	}
	i(n) {
		if (n < 1) return;
		this.#bitPos += n;
	}
	m() {
		const n = 32 - (this.#bitPos & 31) & 31;
		this.i(n);
	}
	get n() {
		return this.#bufferBitLength - this.#bitPos;
	}
	e(v) {
		return this.c() ? -v : v;
	}
};
//#endregion
//#region src/transforms.ts
function* genIDxT(nBits, useDCT) {
	const nBase = 1 << nBits;
	const nDiv4 = nBase >>> 2;
	const theta = 2 * Math.PI / nBase;
	const tempBuf = new Float32Array(nBase);
	let reciprocalCosTables;
	let cosTable;
	let sinTable;
	let fft;
	let i;
	if (useDCT) {
		reciprocalCosTables = [];
		for (let tableIndex = 0; tableIndex <= nBits; tableIndex++) {
			const n = 1 << tableIndex;
			const reciprocalCosTable = new Float32Array(n);
			for (i = 0; i < n; i++) reciprocalCosTable[i] = .5 / Math.cos((i + .5) * Math.PI / n);
			reciprocalCosTables.push(reciprocalCosTable);
		}
	} else {
		cosTable = new Float32Array(nDiv4);
		sinTable = new Float32Array(nDiv4);
		for (i = 0; i < nDiv4; i++) {
			cosTable[i] = Math.cos(i * theta);
			sinTable[i] = Math.sin(i * theta);
		}
		fft = genFFT(nBits - 1);
		fft.next();
	}
	const inverseTransformInternal = (data2, off, n, nBits2, temp) => {
		if (n < 2) return;
		const reciprocalCosTable = reciprocalCosTables[nBits2];
		const halfLen = n >>> 1;
		temp[off + 0] = data2[off];
		temp[off + halfLen] = data2[off + 1];
		for (i = 1; i < halfLen; i++) {
			const iDoubled = i << 1;
			temp[off + i] = data2[off + iDoubled];
			temp[off + i + halfLen] = data2[off - 1 + iDoubled] + data2[off + 1 + iDoubled];
		}
		inverseTransformInternal(temp, off, halfLen, nBits2 - 1, data2);
		inverseTransformInternal(temp, off + halfLen, halfLen, nBits2 - 1, data2);
		for (i = 0; i < halfLen; i++) {
			const x = temp[off + i];
			const y = temp[off + i + halfLen] * reciprocalCosTable[i];
			data2[off + i] = x + y;
			data2[off + n - 1 - i] = x - y;
		}
	};
	let data = yield;
	while (data) {
		if (data.length < nBase) return;
		if (useDCT) inverseTransformInternal(data, 0, nBase, nBits, tempBuf);
		else {
			const dc = data[0];
			const nyquist = data[1];
			data[0] = .5 * (dc + nyquist);
			data[1] = .5 * (dc - nyquist);
			for (i = 1; i < nDiv4; i++) {
				const i1 = i << 1;
				const i2 = nBase - i1;
				const d01 = data[i1] ?? 0;
				const d02 = data[i2] ?? 0;
				const d11 = data[i1 + 1] ?? 0;
				const d12 = data[i2 + 1] ?? 0;
				const evenRe = .5 * (d01 + d02);
				const oddIm = .5 * (d01 - d02);
				const evenIm = .5 * (d11 - d12);
				const oddRe = -.5 * (d11 + d12);
				const cosVal = cosTable[i];
				const sinVal = sinTable[i];
				data[i1] = evenRe + oddRe * cosVal - oddIm * sinVal;
				data[i1 + 1] = evenIm + oddIm * cosVal + oddRe * sinVal;
				data[i2] = evenRe - oddRe * cosVal + oddIm * sinVal;
				data[i2 + 1] = -evenIm + oddIm * cosVal + oddRe * sinVal;
			}
			fft.next(data);
		}
		data = yield;
	}
}
function* genFFT(nBits) {
	const n = 1 << nBits;
	let i;
	let j;
	const revTable = new Uint16Array(n);
	for (i = 0; i < n; i++) {
		let iCopy = i;
		let value = 0;
		for (j = 0; j < nBits; j++) {
			value = value << 1 | iCopy & 1;
			iCopy >>= 1;
		}
		revTable[i] = value;
	}
	const twiddle = new Float32Array(n);
	for (i = 0; i < n >>> 1; i++) {
		const twiddleIndex = i << 1;
		const angle = -2 * Math.PI * i / n;
		twiddle[twiddleIndex] = Math.cos(angle);
		twiddle[twiddleIndex + 1] = Math.sin(angle);
	}
	let data = yield;
	while (data) {
		if (data.length < n) return;
		for (i = 0; i < n; i++) {
			const iDouble = i << 1;
			j = revTable[i];
			if (j > i) {
				const jDouble = j << 1;
				const tr = data[iDouble];
				const ti = data[iDouble + 1];
				data[iDouble] = data[jDouble];
				data[iDouble + 1] = data[jDouble + 1];
				data[jDouble] = tr;
				data[jDouble + 1] = ti;
			}
		}
		for (let size = 2; size <= n; size <<= 1) {
			const halfSize = size >>> 1;
			const step = ~~(n / size);
			for (i = 0; i < n; i += size) {
				let twiddleIdx = 0;
				for (j = 0; j < halfSize; j++) {
					const wr = twiddle[twiddleIdx << 1];
					const wi = twiddle[(twiddleIdx << 1) + 1];
					const evenIdx = i + j << 1;
					const oddIdx = i + j + halfSize << 1;
					const er = data[evenIdx];
					const ei = data[evenIdx + 1];
					const or = data[oddIdx];
					const oi = data[oddIdx + 1];
					const tr = wr * or - wi * oi;
					const ti = wr * oi + wi * or;
					data[evenIdx] = er + tr;
					data[evenIdx + 1] = ei + ti;
					data[oddIdx] = er - tr;
					data[oddIdx + 1] = ei - ti;
					twiddleIdx += step;
				}
			}
		}
		data = yield;
	}
}
const idct = (src, srcOffset, rawDest, destOffset, column) => {
	const indexShift = column ? 3 : 0;
	const constantToAdd = column ? 0 : 127;
	const destShift = column ? 0 : 8;
	const dest = rawDest ? rawDest : src;
	let a0 = (src[srcOffset] ?? 0) + constantToAdd;
	let b0 = src[srcOffset + (1 << indexShift)] ?? 0;
	let a2 = src[srcOffset + (2 << indexShift)] ?? 0;
	const x3 = src[srcOffset + (3 << indexShift)] ?? 0;
	const x4 = src[srcOffset + (4 << indexShift)] ?? 0;
	let a4 = src[srcOffset + (5 << indexShift)] ?? 0;
	const x6 = src[srcOffset + (6 << indexShift)] ?? 0;
	const x7 = src[srcOffset + (7 << indexShift)] ?? 0;
	const a1 = a0 - x4;
	const a3 = 2896 * (a2 - x6) >> 11;
	const a5 = a4 - x3;
	const a7 = b0 - x7;
	a0 += x4;
	a2 += x6;
	a4 += x3;
	b0 += x7;
	const a0pa2 = a0 + a2;
	const a0ma2 = a0 - a2;
	const a1pa3ma3 = a1 + a3 - a2;
	const a1ma3pa2 = a1 - a3 + a2;
	const b1 = 3784 * (a5 + a7) >> 11;
	let b3 = 2896 * (b0 - a4) >> 11;
	b0 += a4;
	const b2 = (-5352 * a5 >> 11) - b0 + b1;
	b3 -= b2;
	const b4 = (2217 * a7 >> 11) + b3 - b1;
	dest[destOffset] = a0pa2 + b0 >> destShift;
	dest[destOffset + (1 << indexShift)] = a1pa3ma3 + b2 >> destShift;
	dest[destOffset + (2 << indexShift)] = a1ma3pa2 + b3 >> destShift;
	dest[destOffset + (3 << indexShift)] = a0ma2 - b4 >> destShift;
	dest[destOffset + (4 << indexShift)] = a0ma2 + b4 >> destShift;
	dest[destOffset + (5 << indexShift)] = a1ma3pa2 - b3 >> destShift;
	dest[destOffset + (6 << indexShift)] = a1pa3ma3 - b2 >> destShift;
	dest[destOffset + (7 << indexShift)] = a0pa2 - b0 >> destShift;
};
const idctPut = (block, dest, destOffset, stride) => {
	let i = 0;
	for (; i < 8; i++) idct(block, i, null, i, true);
	for (i = 0; i < 64; i += 8) {
		if (destOffset >= dest.length) break;
		idct(block, i, dest, destOffset, false);
		destOffset += stride;
	}
};
const idctAdd = (block, dest, destOffset, stride) => {
	let i = 0;
	for (; i < 8; i++) idct(block, i, null, i, true);
	for (i = 0; i < 64; i += 8) idct(block, i, null, i, false);
	addBlock8x8(block, dest, destOffset, stride);
};
const addBlock8x8 = (block, dest, destOffset, stride) => {
	stride -= 8;
	let i = -1;
	let iMax = 6;
	while (i++ < 63) {
		dest[destOffset + i] += block[i];
		if (i > iMax) {
			destOffset += stride;
			iMax += 8;
		}
	}
};
//#endregion
//#region src/bik-audio-decoder.ts
let AUDIO_CRITICAL_FREQS;
let AUDIO_RLE_LENGTH_TABLE;
let constantsInitialized$1 = false;
function* genBikAudioDecoder(sampleRate, numChannels, useDCT) {
	if (!constantsInitialized$1) {
		AUDIO_CRITICAL_FREQS = unpackValues(3, PACKED_AUDIO_CRITICAL_FREQS);
		AUDIO_RLE_LENGTH_TABLE = unpackValues(2, PACKED_AUDIO_RLE_LENGTH_TABLE);
		constantsInitialized$1 = true;
	}
	let frameLenBits;
	if (sampleRate < 22050) frameLenBits = 9;
	else if (sampleRate < 44100) frameLenBits = 10;
	else frameLenBits = 11;
	let numInternalChannels = numChannels;
	if (!useDCT) {
		sampleRate *= numChannels;
		frameLenBits += Math.ceil(Math.log2(numChannels)) & 3;
		numInternalChannels = 1;
	}
	const frameLen = 1 << frameLenBits;
	const overlapLen = frameLen >>> 4;
	const blockSize = (frameLen - overlapLen) * numInternalChannels;
	const sampleRateHalf = sampleRate + 1 >>> 1;
	const baseQuant = (useDCT ? frameLen : 2) / (Math.sqrt(frameLen) * 32768);
	const expMultiplier = .0664 / Math.log10(Math.E);
	const quantTable = /* @__PURE__ */ new Float32Array(96);
	for (let i = 0; i < 96; i++) quantTable[i] = Math.exp(i * expMultiplier) * baseQuant;
	let numBands = AUDIO_CRITICAL_FREQS.findIndex((x) => sampleRateHalf <= x);
	numBands = numBands < 0 ? AUDIO_CRITICAL_FREQS.length + 1 : numBands + 1;
	const bands = new Uint32Array(numBands + 1);
	bands[0] = 2;
	for (let i = 1; i < numBands; i++) bands[i] = AUDIO_CRITICAL_FREQS[i - 1] * frameLen / sampleRateHalf & -2;
	bands[numBands] = frameLen;
	const output = createArrayOfLen(numInternalChannels);
	for (const [index] of output.entries()) output[index] = new Float32Array(frameLen);
	const overlapWindows = [];
	for (let i = 0; i < numInternalChannels; i++) overlapWindows.push(new Float32Array(overlapLen));
	const idt = genIDxT(frameLenBits, useDCT);
	idt.next();
	let first = true;
	let allOutput = [];
	while (true) {
		const data = yield allOutput;
		allOutput = [];
		if (!data) {
			first = true;
			continue;
		}
		const reader = new BitReader(data);
		const readDequantFloat29 = () => {
			const power = reader.a(5);
			return reader.e(reader.a(23) * 2 ** (power - 23)) * baseQuant;
		};
		while (reader.n > 0) {
			if (useDCT) reader.i(2);
			for (let ch = 0; ch < numInternalChannels; ch++) {
				const coeffs = output[ch];
				coeffs[0] = readDequantFloat29();
				coeffs[1] = readDequantFloat29();
				const quant = new Float32Array(numBands);
				for (let i2 = 0; i2 < quant.length; i2++) quant[i2] = quantTable[Math.min(reader.a(8), 95)];
				let k = 0;
				let q = quant[0];
				let i = 2;
				while (i < frameLen) {
					let j;
					if (reader.c()) {
						const v = reader.a(4);
						j = i + AUDIO_RLE_LENGTH_TABLE[v];
					} else j = i + 8;
					j = ~~Math.min(j, frameLen);
					const width = reader.a(4);
					if (width === 0) {
						coeffs.fill(0, i, j);
						i = j;
						while ((bands[k] ?? 0) < i) q = quant[k++] ?? 0;
					} else while (i < j) {
						if (bands[k] === i) q = quant[k++] ?? 0;
						const coeff = reader.a(width);
						if (coeff) {
							const sign = reader.c();
							coeffs[i] = sign ? -q * coeff : q * coeff;
						} else coeffs[i] = 0;
						i++;
					}
				}
				idt.next(coeffs);
				if (useDCT) {
					const dctModifier = 4 * baseQuant;
					for (let i2 = 0; i2 < coeffs.length; i2++) coeffs[i2] *= dctModifier;
				}
			}
			for (let ch = 0; ch < numInternalChannels; ch++) {
				const coeffs = output[ch];
				const overlapWindow = overlapWindows[ch];
				if (!first) {
					const count = overlapLen * numInternalChannels;
					for (let i = 0, j = ch; i < overlapLen; i++, j += numInternalChannels) coeffs[i] = ((overlapWindow[i] ?? 0) * (count - j) + (coeffs[i] ?? 0) * j) / count;
				}
				overlapWindow.set(coeffs.subarray(frameLen - overlapLen));
			}
			const outputBlock = [];
			const stride = Math.ceil(numChannels / numInternalChannels);
			if (stride > 1) {
				let intCh = 0;
				while (intCh < numInternalChannels) {
					const internalBlock = output[intCh];
					let ch = 0;
					while (ch < numChannels) {
						for (let strideCount = 0; strideCount < stride; strideCount++) {
							const deinterleavedBlock = new Float32Array(~~(blockSize / stride));
							for (let i = 0; i < deinterleavedBlock.length; i++) deinterleavedBlock[i] = internalBlock[i * stride + strideCount];
							outputBlock.push(deinterleavedBlock);
						}
						ch += stride;
					}
					intCh++;
				}
			} else {
				const actualOutputBlockSize = ~~(blockSize / numInternalChannels);
				for (let ch = 0; ch < numInternalChannels; ch++) outputBlock.push(output[ch].slice(0, actualOutputBlockSize));
			}
			allOutput.push(outputBlock);
			first = false;
			reader.m();
		}
	}
}
//#endregion
//#region src/bik-video-decoder.ts
const EMPTY_UINT8_ARRAY = /* @__PURE__ */ new Uint8Array();
let BIK_PATTERNS;
let BIK_SCAN;
let BIK_TREE_CODES;
let BIK_TREE_LENS;
let BIK_QUANT;
let PREDEFINED_HUFF_TABLES;
let constantsInitialized = false;
var HuffTable = class {
	#symbolsLens;
	#maxBits;
	constructor(bikTreeIndex) {
		const referenceTableOffset = bikTreeIndex << 4;
		const maxBits = BIK_TREE_LENS[referenceTableOffset + 15];
		this.#maxBits = maxBits;
		const tableSize = 1 << maxBits;
		this.#symbolsLens = new Uint8Array(tableSize);
		for (let i = 0; i < 16; i++) {
			const code = BIK_TREE_CODES[referenceTableOffset + i];
			const len = BIK_TREE_LENS[referenceTableOffset + i];
			const numEntries = 1 << maxBits - len;
			for (let j = 0; j < numEntries; j++) {
				const index = j << len | code;
				if (index < tableSize) this.#symbolsLens[index] = i << 4 | len;
			}
		}
	}
	/**
	* Decode the next Huffman symbol in a bit-stream.
	*
	* @param reader Bit-stream to read from.
	* @param tree Huffman tree to use for decoding.
	* @returns 4-bit value, Huffman-decoded from the bit-stream.
	*/
	static f(reader, tree) {
		const table = tree.k;
		const peek = reader.a(table.#maxBits, true);
		const entry = table.#symbolsLens[peek];
		reader.i(entry & 15);
		return tree.g[entry >>> 4];
	}
};
function* genBikVideoDecoder(width, height, version, hasAlpha, hasSwappedUVPlanes) {
	if (!constantsInitialized) {
		BIK_PATTERNS = unpackValues(2, PACKED_BIK_PATTERNS);
		BIK_SCAN = unpackValues(2, PACKED_BIK_SCAN);
		BIK_TREE_CODES = unpackValues(2, PACKED_BIK_TREE_CODES);
		BIK_TREE_LENS = unpackValues(1, PACKED_BIK_TREE_LENS);
		BIK_QUANT = buildQuantTables();
		PREDEFINED_HUFF_TABLES = createArrayOfLen(16).fill(null).map((_, bikTreeIndex) => new HuffTable(bikTreeIndex));
		constantsInitialized = true;
	}
	const inputTreeBuf = /* @__PURE__ */ new Uint8Array(16);
	const outputTreeBuf = /* @__PURE__ */ new Uint8Array(16);
	const coeffIndex = /* @__PURE__ */ new Int32Array(64);
	const coeffList = /* @__PURE__ */ new Int32Array(128);
	const modeList = /* @__PURE__ */ new Uint8Array(128);
	const tmpScalingBuf = /* @__PURE__ */ new Uint8Array(64);
	const tmpDCTBuf = /* @__PURE__ */ new Int32Array(64);
	const numPixels = width * height;
	const uvSize = (width + 1 >>> 1) * (height + 1 >>> 1);
	const frameSize = (numPixels << (hasAlpha ? 1 : 0)) + (uvSize << 1);
	let stride = 0;
	const prevFrameData = new Uint8Array(frameSize);
	let planeData = EMPTY_UINT8_ARRAY;
	let prevPlaneData = EMPTY_UINT8_ARRAY;
	let planeDataPtr = 0;
	const reader = new BitReader(EMPTY_UINT8_ARRAY);
	const blockParams = Array.from({ length: 9 });
	const colHigh = createArrayOfLen(16);
	let colLastValue = 0;
	const numBlockPixels = numPixels + 63 >>> 6 << 6;
	for (let i = 0; i < 9; i++) blockParams[i] = {
		o: 0,
		h: {
			k: PREDEFINED_HUFF_TABLES[0],
			g: /* @__PURE__ */ new Uint8Array(16)
		},
		d: i > 3 && i < 8 ? new Int16Array(numBlockPixels) : new Uint8Array(numBlockPixels),
		b: 0,
		j: 0
	};
	for (let i = 0; i < 16; i++) colHigh[i] = {
		k: PREDEFINED_HUFF_TABLES[0],
		g: /* @__PURE__ */ new Uint8Array(16)
	};
	const decodePlane = (frame2, planeIndex) => {
		const isChroma = planeIndex === 1 || planeIndex === 2;
		const blockWidth = isChroma ? Math.ceil(width / 16) : Math.ceil(width / 8);
		const blockHeight = isChroma ? Math.ceil(height / 16) : Math.ceil(height / 8);
		readPlaneTrees(blockWidth);
		stride = frame2.lineSize[planeIndex] ?? 0;
		const planeOffset = planeIndex === 0 ? 0 : planeIndex === 1 ? numPixels : planeIndex === 2 ? numPixels + uvSize : numPixels + uvSize + uvSize;
		const blockLineIncr = stride * 7;
		const planeSize = isChroma ? uvSize : numPixels;
		planeData = new Uint8Array(frame2.yuv.buffer, planeOffset, planeSize);
		planeDataPtr = 0;
		prevPlaneData = new Uint8Array(prevFrameData.buffer, planeOffset, planeSize);
		const blockTypeParam = blockParams[0];
		const blockTypes = blockTypeParam.d;
		let blockYPos = 0;
		while (blockYPos++ < blockHeight) {
			readBlockTypes(blockTypeParam);
			readBlockTypes(blockParams[1]);
			readColors(blockParams[2]);
			readPatterns(blockParams[3]);
			readMotionValues(blockParams[4]);
			readMotionValues(blockParams[5]);
			readDCs(blockParams[6], false);
			readDCs(blockParams[7], true);
			readRuns(blockParams[8]);
			let blockXPos = 0;
			while (blockXPos++ < blockWidth) {
				const blockType = blockTypes[blockTypeParam.j++];
				switch (blockType) {
					case 0: break;
					case 1:
						if (blockYPos & 1) decodeScaledBlock();
						blockXPos++;
						planeDataPtr += 16;
						continue;
					case 2:
						decodeMotionBlock();
						break;
					case 3:
						decodeRunBlock(planeData, planeDataPtr, stride);
						break;
					case 4:
						decodeResidueBlock();
						break;
					case 5:
						decodeIntraBlock(planeData, planeDataPtr, stride);
						break;
					case 6:
						decodeFillBlock();
						break;
					case 7:
						decodeInterBlock();
						break;
					case 8:
						decodePatternBlock(planeData, planeDataPtr, stride);
						break;
					case 9:
						decodeRawBlock();
						break;
					default: throw new Error(`Invalid block type ${blockType}`);
				}
				planeDataPtr += 8;
			}
			planeDataPtr += blockLineIncr;
		}
		reader.m();
		planeData = EMPTY_UINT8_ARRAY;
		prevPlaneData = EMPTY_UINT8_ARRAY;
	};
	const copyBlock = (srcOffset) => {
		const indexDiff = planeDataPtr - srcOffset;
		if (!indexDiff) return;
		const strideMinusBlock = stride - 8;
		let lineCount = 8;
		while (lineCount--) {
			const srcOffsetMax = srcOffset + 8;
			while (srcOffset < srcOffsetMax) {
				planeData[srcOffset + indexDiff] = prevPlaneData[srcOffset];
				srcOffset++;
			}
			srcOffset += strideMinusBlock;
		}
	};
	const decodeMotionBlock = () => {
		const xOffset = getValue(4);
		const yOffset = getValue(5);
		const ref = planeDataPtr + xOffset + yOffset * stride;
		copyBlock(ref);
	};
	const decodeRunBlock = (block, offset = 0, stride2 = 8) => {
		let i = 0;
		let scanIndex = reader.a(4) << 6;
		do {
			const run = getValue(8) + 1;
			i += run;
			if (reader.c()) {
				const v = getValue(2);
				for (let j = 0; j < run; j++) {
					const pos = BIK_PATTERNS[scanIndex++] ?? 0;
					block[offset + (pos >>> 3) * stride2 + (pos & 7)] = v;
				}
			} else for (let j = 0; j < run; j++) {
				const pos = BIK_PATTERNS[scanIndex++] ?? 0;
				block[offset + (pos >>> 3) * stride2 + (pos & 7)] = getValue(2);
			}
		} while (i < 63);
		if (i === 63) {
			const pos = BIK_PATTERNS[scanIndex] ?? 0;
			block[offset + (pos >>> 3) * stride2 + (pos & 7)] = getValue(2);
		}
	};
	const decodeResidueBlock = () => {
		const xOffset = getValue(4);
		const yOffset = getValue(5);
		const srcPos = planeDataPtr + xOffset + yOffset * stride;
		tmpDCTBuf.fill(0);
		readCoeffsOrResidue(tmpDCTBuf);
		copyBlock(srcPos);
		addBlock8x8(tmpDCTBuf, planeData, planeDataPtr, stride);
	};
	const decodeIntraBlock = (block, offset = 0, stride2 = 8) => {
		tmpDCTBuf[0] = getValue(6);
		tmpDCTBuf.fill(0, 1);
		readCoeffsOrResidue(tmpDCTBuf, 0);
		idctPut(tmpDCTBuf, block, offset, stride2);
	};
	const decodeFillBlock = (size = 8) => {
		const v = getValue(2);
		for (let y = 0; y < size; y++) for (let x = 0; x < size; x++) planeData[planeDataPtr + y * stride + x] = v;
	};
	const decodeInterBlock = () => {
		const xOffset = getValue(4);
		const yOffset = getValue(5);
		const ref = planeDataPtr + xOffset + yOffset * stride;
		copyBlock(ref);
		tmpDCTBuf[0] = getValue(7);
		tmpDCTBuf.fill(0, 1);
		readCoeffsOrResidue(tmpDCTBuf, 1024);
		idctAdd(tmpDCTBuf, planeData, planeDataPtr, stride);
	};
	const decodePatternBlock = (block, offset = 0, stride2 = 8) => {
		const col = [getValue(2), getValue(2)];
		for (let i = 0; i < 8; i++) {
			let v = getValue(3);
			for (let j = 0; j < 8; j++) {
				block[offset + i * stride2 + j] = col[v & 1];
				v >>= 1;
			}
		}
	};
	const decodeRawBlock = () => {
		const blockParamValues = blockParams[2];
		for (let y = 0; y < 8; y++) for (let x = 0; x < 8; x++) planeData[planeDataPtr + y * stride + x] = blockParamValues.d[blockParamValues.j++];
	};
	const decodeScaledBlock = () => {
		const subBlk = getValue(1);
		switch (subBlk) {
			case 9:
				for (let i = 0; i < 64; i++) tmpScalingBuf[i] = getValue(2);
				break;
			case 5:
				decodeIntraBlock(tmpScalingBuf);
				break;
			case 6:
				decodeFillBlock(16);
				return;
			case 3:
				decodeRunBlock(tmpScalingBuf);
				break;
			case 8:
				decodePatternBlock(tmpScalingBuf);
				break;
			default: throw new Error(`Invalid sub-block type ${subBlk}`);
		}
		const dest = planeData;
		let srcPos = 0;
		let destPosLine = planeDataPtr;
		const maxDestPosLine = destPosLine + (stride << 4);
		const lineIncrement = (stride << 1) - 15;
		while (destPosLine < maxDestPosLine) {
			const value = tmpScalingBuf[srcPos++];
			dest[destPosLine] = dest[stride + destPosLine++] = value;
			dest[destPosLine] = dest[stride + destPosLine] = value;
			destPosLine += srcPos & 7 ? 1 : lineIncrement;
		}
	};
	const readTree = (tree) => {
		const tableNum = reader.a(4);
		tree.k = PREDEFINED_HUFF_TABLES[tableNum];
		if (!tableNum) {
			for (let i = 0; i < 16; i++) tree.g[i] = i;
			return;
		}
		if (reader.c()) {
			let len = reader.a(3);
			const tmp = inputTreeBuf.fill(0);
			for (let i = 0; i <= len; i++) {
				tree.g[i] = reader.a(4);
				tmp[tree.g[i]] = 1;
			}
			for (let i = 0; i < 16; i++) if (!tmp[i]) tree.g[++len] = i;
		} else {
			const len = reader.a(2);
			let input = inputTreeBuf;
			let output = outputTreeBuf;
			for (let i = 0; i < 16; i += 2) {
				const bit = reader.c();
				input[i] = i + bit;
				input[i + 1] = i + (bit ^ 1);
			}
			for (let i = 1; i <= len; i++) {
				const size = 1 << i;
				for (let t = 0; t < 16; t += size << 1) mergeTreeData(output, input, t, size);
				[input, output] = [output, input];
			}
			tree.g.set(input);
		}
	};
	const mergeTreeData = (dest, src, src1Index, size1) => {
		let src2Index = src1Index + size1;
		let size2 = size1;
		let destIndex = src1Index;
		while (size1 && size2) if (reader.c()) {
			dest[destIndex++] = src[src2Index++];
			size2--;
		} else {
			dest[destIndex++] = src[src1Index++];
			size1--;
		}
		while (size1--) dest[destIndex++] = src[src1Index++];
		while (size2--) dest[destIndex++] = src[src2Index++];
	};
	const readPlaneTrees = (blockWidth) => {
		const extraLen = 511;
		const commonLen = blockWidth + extraLen;
		for (const [blockParamNum, blockParamValues] of blockParams.entries()) {
			blockParamValues.o = ~~Math.log2([
				commonLen,
				(blockWidth >>> 1) + extraLen,
				(blockWidth << 6) + extraLen,
				(blockWidth << 3) + extraLen,
				commonLen,
				commonLen,
				commonLen,
				commonLen,
				blockWidth * 48 + extraLen
			][blockParamNum]) + 1;
			if (blockParamNum === 2) {
				for (const tree of colHigh) readTree(tree);
				colLastValue = 0;
			}
			if (blockParamNum < 6 || blockParamNum > 7) readTree(blockParamValues.h);
			blockParamValues.b = blockParamValues.j = 0;
		}
	};
	const readCodedDataCount = (blockParamValues) => {
		if (blockParamValues.b < 0 || blockParamValues.b > blockParamValues.j) return 0;
		const count = reader.a(blockParamValues.o);
		if (count === 0) blockParamValues.b = -1;
		return count;
	};
	const readBlockTypes = (blockParamValues) => {
		const count = readCodedDataCount(blockParamValues);
		if (!count) return;
		if (reader.c()) {
			const v = reader.a(4);
			for (let i = 0; i < count; i++) blockParamValues.d[blockParamValues.b++] = v;
		} else {
			let prevValue = 0;
			for (let i = 0; i < count; i++) {
				const v = HuffTable.f(reader, blockParamValues.h);
				if (v < 12) {
					prevValue = v;
					blockParamValues.d[blockParamValues.b++] = v;
				} else {
					const runLength = [
						4,
						8,
						12,
						32
					][v - 12];
					for (let j = 0; j < runLength; j++) blockParamValues.d[blockParamValues.b++] = prevValue;
					i += runLength - 1;
				}
			}
		}
	};
	const readColors = (blockParamValues) => {
		const count = readCodedDataCount(blockParamValues);
		if (!count) return;
		const isRun = reader.c();
		let loopCount = isRun ? 1 : count;
		do {
			const colHighValue = HuffTable.f(reader, colHigh[colLastValue]);
			let v = HuffTable.f(reader, blockParamValues.h) | colHighValue << 4;
			colLastValue = colHighValue;
			if (version < 105) v = v > 127 ? 256 - v : v + 128;
			if (isRun) {
				blockParamValues.d.fill(v, blockParamValues.b, blockParamValues.b + count);
				blockParamValues.b += count;
			} else blockParamValues.d[blockParamValues.b++] = v;
		} while (--loopCount);
	};
	const readPatterns = (blockParamValues) => {
		const count = readCodedDataCount(blockParamValues);
		if (!count) return;
		for (let i = 0; i < count; i++) {
			const v = HuffTable.f(reader, blockParamValues.h) | HuffTable.f(reader, blockParamValues.h) << 4;
			blockParamValues.d[blockParamValues.b++] = v;
		}
	};
	const readMotionValues = (blockParamValues) => {
		const count = readCodedDataCount(blockParamValues);
		if (!count) return;
		const items = blockParamValues.d;
		const maxDec = blockParamValues.b + count;
		let v;
		if (reader.c()) {
			v = reader.a(4);
			if (v) v = reader.e(v);
			v = v << 24 >> 24;
			items.fill(v, blockParamValues.b, maxDec);
		} else while (blockParamValues.b < maxDec) {
			v = HuffTable.f(reader, blockParamValues.h);
			if (v) v = reader.e(v);
			v = v << 24 >> 24;
			items[blockParamValues.b++] = v;
		}
		blockParamValues.b = maxDec;
	};
	const readDCs = (blockParamValues, hasSign) => {
		const count = readCodedDataCount(blockParamValues);
		if (!count) return;
		let v = reader.a(hasSign ? 10 : 11);
		if (v && hasSign) v = reader.e(v);
		const items = blockParamValues.d;
		items[blockParamValues.b++] = v;
		let i = 1;
		while (i < count) {
			const len = Math.min(count - i, 8);
			const v2Size = reader.a(4);
			if (v2Size) for (let j = 0; j < len; j++) {
				let v2 = reader.a(v2Size);
				if (v2) v2 = reader.e(v2);
				v += v2;
				items[blockParamValues.b++] = v;
			}
			else for (let j = 0; j < len; j++) items[blockParamValues.b++] = v;
			i += len;
		}
	};
	const readRuns = (blockParamValues) => {
		const count = readCodedDataCount(blockParamValues);
		if (!count) return;
		if (reader.c()) {
			const v = reader.a(4);
			for (let i = 0; i < count; i++) blockParamValues.d[blockParamValues.b++] = v;
		} else for (let i = 0; i < count; i++) blockParamValues.d[blockParamValues.b++] = HuffTable.f(reader, blockParamValues.h);
	};
	const getValue = (blockParamNum) => {
		const blockParamValues = blockParams[blockParamNum];
		return blockParamValues.d[blockParamValues.j++] ?? 0;
	};
	const readCoeffsOrResidue = (block, quantStartIndex = -1) => {
		const isResidue = quantStartIndex < 0;
		let listStart = 64;
		let listEnd = 70;
		let masksCount = 0;
		let coeffCount = 0;
		let i = 0;
		coeffList[64] = 4;
		coeffList[65] = 24;
		coeffList[66] = 44;
		modeList[64] = 0;
		modeList[65] = 0;
		modeList[66] = 0;
		if (isResidue) {
			listEnd = 68;
			masksCount = reader.a(7);
			coeffList[67] = 0;
			modeList[67] = 2;
		} else {
			coeffList[67] = 1;
			coeffList[68] = 2;
			coeffList[69] = 3;
			modeList[67] = 3;
			modeList[68] = 3;
			modeList[69] = 3;
		}
		let bits = isResidue ? 1 << reader.a(3) : reader.a(4) - 1;
		while (isResidue ? bits : bits >= 0) {
			if (isResidue) {
				for (i = 0; i < coeffCount; i++) if (reader.c()) {
					const curNzCoeff = coeffIndex[i] ?? 0;
					const value = block[curNzCoeff] ?? 0;
					block[curNzCoeff] = value < 0 ? value - bits : value + bits;
					if (!masksCount--) return;
				}
			}
			let listPos = listStart;
			while (listPos < listEnd) {
				const ccoeff = coeffList[listPos] ?? 0;
				const mode = modeList[listPos] ?? 0;
				if (!(mode | ccoeff) || !reader.c()) {
					listPos++;
					continue;
				}
				switch (mode) {
					case 0:
					case 2:
						if (mode === 0) {
							coeffList[listPos] = ccoeff + 4;
							modeList[listPos] = 1;
						} else {
							coeffList[listPos] = 0;
							modeList[listPos++] = 0;
						}
						for (i = ccoeff; i < ccoeff + 4; i++) if (reader.c()) {
							coeffList[--listStart] = i;
							modeList[listStart] = 3;
						} else if (isResidue) {
							const offset = BIK_SCAN[i] ?? 0;
							coeffIndex[coeffCount++] = offset;
							block[offset] = reader.e(bits);
							if (!masksCount--) return;
						} else {
							const offset = bits ? reader.e(reader.a(bits) | 1 << bits) : 1 - (reader.c() << 1);
							block[BIK_SCAN[i] ?? 0] = offset;
							coeffIndex[coeffCount++] = i;
						}
						break;
					case 1:
						modeList[listPos] = 2;
						for (i = ccoeff + 4; i < ccoeff + 16; i += 4) {
							coeffList[listEnd] = i;
							modeList[listEnd++] = 2;
						}
						break;
					case 3:
						coeffList[listPos] = 0;
						modeList[listPos++] = 0;
						if (isResidue) {
							const offset = BIK_SCAN[ccoeff] ?? 0;
							coeffIndex[coeffCount++] = offset;
							block[offset] = reader.e(bits);
							if (!masksCount--) return;
						} else {
							const offset = bits ? reader.e(reader.a(bits) | 1 << bits) : 1 - (reader.c() << 1);
							block[BIK_SCAN[ccoeff] ?? 0] = offset;
							coeffIndex[coeffCount++] = ccoeff;
						}
				}
			}
			bits = isResidue ? bits >>> 1 : bits - 1;
		}
		if (!isResidue) {
			const quantOffset = (reader.a(4) << 6) + quantStartIndex;
			block[0] = (block[0] ?? 0) * (BIK_QUANT[quantOffset] ?? 0) >> 11;
			while (coeffCount--) {
				const index = coeffIndex[coeffCount] ?? 0;
				const blockIndex = BIK_SCAN[index] ?? 0;
				block[blockIndex] = (block[blockIndex] ?? 0) * (BIK_QUANT[quantOffset + index] ?? 0) >> 11;
			}
		}
	};
	let frame = null;
	while (true) {
		const payload = yield frame;
		if (!payload) continue;
		reader.l(payload.p);
		const existingFrame = payload.q;
		if (existingFrame) {
			frame = existingFrame;
			frame.yuv.set(prevFrameData);
		} else frame = {
			width,
			height,
			yuv: new Uint8Array(prevFrameData),
			lineSize: [
				width,
				width >>> 1,
				width >>> 1,
				width
			]
		};
		if (hasAlpha) {
			if (version > 104) reader.i(32);
			decodePlane(frame, 3);
		}
		if (version > 104) reader.i(32);
		for (let plane = 0; plane < 3; plane++) {
			const planeIndex = plane && hasSwappedUVPlanes ? plane ^ 3 : plane;
			decodePlane(frame, planeIndex);
			if (reader.n < 1) break;
		}
		prevFrameData.set(frame.yuv);
		reader.l(EMPTY_UINT8_ARRAY);
	}
}
const createBikDecoder = class BikDecoder {
	/** Source of the video. */
	#dataSource;
	/** A {@link ReadableStreamDefaultReader} for the current position in the encoded video data. */
	#streamReader = null;
	/** Buffer of encoded video data being processed by the decoder. */
	#bufBytes = null;
	/** Start of the {@link #bufBytes} buffer relative to the start of the encoded video data. */
	#bufPos = 0;
	#curFrame = -1;
	#audioTrackDecoders = [];
	#videoDecoder = null;
	#header = null;
	#isSupported = false;
	constructor(source) {
		this.#dataSource = source;
	}
	/**
	* Set the current video data read position to the specified position, updating the read buffer
	* and potentially creating a new readable stream if the read position actually changes.
	*
	* @param pos Position in the video to seek to. Specified in bytes from the start of the video.
	*/
	async #seek(pos) {
		if (pos < 0) return;
		if (this.#bufBytes) {
			if (pos === this.#bufPos) return;
			if (pos > this.#bufPos && pos < this.#bufPos + this.#bufBytes.byteLength) {
				this.#bufBytes = this.#bufBytes.subarray(pos - this.#bufPos);
				this.#bufPos = pos;
				return;
			}
		}
		if (this.#streamReader) {
			await this.#streamReader.cancel();
			this.#streamReader.releaseLock();
			this.#streamReader = null;
		}
		const dataSource = this.#dataSource;
		let streamReader;
		if (dataSource instanceof Blob) streamReader = dataSource.slice(pos).stream()?.getReader();
		else {
			const isRequest = dataSource instanceof Request;
			if (dataSource instanceof URL || isRequest) {
				const request = isRequest ? dataSource : new Request(dataSource);
				const headers = request.headers;
				headers.delete("range");
				if (pos) headers.set("range", `bytes=${pos}-`);
				streamReader = (await fetch(request))?.body?.getReader();
			}
		}
		if (!streamReader) throw new Error("Invalid stream reader");
		this.#streamReader = streamReader;
		this.#bufBytes = null;
		this.#bufPos = pos;
	}
	/**
	* Read data from the readable stream.
	*
	* @param len Number of bytes to read.
	* @returns The requested bytes of data. Will be at most `len` bytes, but may be fewer.
	* @throws {@link Error} Thrown if the stream ends before the requested number of bytes have been
	*   read.
	*/
	async #readBytes(len) {
		if (!this.#streamReader || len < 1) return null;
		if (!this.#bufBytes) {
			const { value, done } = await this.#streamReader.read();
			if (!value || done) return null;
			this.#bufBytes = value;
		}
		let bufLen = this.#bufBytes.byteLength;
		while (len > bufLen) {
			const { value, done } = await this.#streamReader.read();
			if (!value || done) {
				this.#bufPos += bufLen;
				const bufBytes2 = this.#bufBytes;
				this.#bufBytes = null;
				return bufBytes2;
			}
			const newBufLen = bufLen + value.byteLength;
			const newBufBytes = new Uint8Array(newBufLen);
			newBufBytes.set(this.#bufBytes);
			newBufBytes.set(value, bufLen);
			this.#bufBytes = newBufBytes;
			bufLen = this.#bufBytes.byteLength;
		}
		if (bufLen === len) {
			this.#bufPos += bufLen;
			const bufBytes2 = this.#bufBytes;
			this.#bufBytes = null;
			return bufBytes2;
		}
		this.#bufPos += len;
		const bufBytes = this.#bufBytes.subarray(0, len);
		this.#bufBytes = this.#bufBytes.subarray(len);
		return bufBytes;
	}
	/**
	* Read data from the readable stream.
	*
	* @param len Number of bytes to read.
	* @returns The requested bytes of data.
	* @throws {@link Error} Thrown if the stream ends before the requested number of bytes have been
	*   read.
	*/
	async #ensureReadBytes(len) {
		const bufBytes = await this.#readBytes(len);
		if (bufBytes?.byteLength !== len) throw new Error(`Read ${bufBytes?.byteLength} bytes but expected ${len}`);
		return bufBytes;
	}
	/**
	* Initialize the decoder by parsing the header from the readable stream to get the required
	* information about the encoded video, including the offsets of all the frames.
	*/
	async #init() {
		await this.#seek(0);
		const headerBytes = await this.#ensureReadBytes(44);
		const headerDataView = new DataView(headerBytes.buffer, headerBytes.byteOffset + 0, 44);
		const headerWords = (/* @__PURE__ */ new Uint32Array(11)).fill(0).map((_, index) => headerDataView.getUint32(index << 2, true));
		const magicUint = headerWords[0];
		const version = [4933954, 3293771].indexOf(magicUint & 16777215) + 1;
		if (!version) throw new Error(`Invalid format`);
		const subVersion = magicUint >>> 24;
		const numFrames = headerWords[2];
		const width = headerWords[5];
		const height = headerWords[6];
		const videoFlags = headerWords[9] >>> 16;
		const hasAlpha = !!(videoFlags & 16);
		const hasSwappedUVPlanes = version === 1 && subVersion > 103;
		const isGrayscale = !!(videoFlags & 2);
		const scaling = videoFlags >>> 12;
		const numAudioTracks = headerWords[10];
		const audioTrackHeaderSize = numAudioTracks * 12;
		const frameListSize = (numFrames + 1) * 4;
		let audioTracks = [];
		if (numAudioTracks > 0) {
			const audioHeaderBytes = await this.#ensureReadBytes(audioTrackHeaderSize);
			const audioHeaderDataView = new DataView(audioHeaderBytes.buffer, audioHeaderBytes.byteOffset, audioTrackHeaderSize);
			audioTracks = createArrayOfLen(numAudioTracks).fill(null).map((_, index) => {
				const flags = audioHeaderDataView.getUint16((numAudioTracks << 2) + (index << 2) + 2, true);
				const isStereo = !!(flags & 8192);
				return {
					trackId: audioHeaderDataView.getUint32((numAudioTracks << 3) + (index << 2), true),
					numChannels: isStereo ? 2 : 1,
					sampleRate: audioHeaderDataView.getUint16((numAudioTracks << 2) + (index << 2), true),
					flags: {
						stereo: isStereo,
						usesDCT: !!(flags & 4096)
					}
				};
			});
			audioTracks.forEach((audioTrack) => {
				const audioTrackDecoder = genBikAudioDecoder(audioTrack.sampleRate, audioTrack.numChannels, audioTrack.flags.usesDCT);
				audioTrackDecoder.next();
				this.#audioTrackDecoders.push(audioTrackDecoder);
			});
		}
		const videoHeaderBytes = await this.#ensureReadBytes(frameListSize);
		const frameListDataView = new DataView(videoHeaderBytes.buffer, videoHeaderBytes.byteOffset, frameListSize);
		const frames = createArrayOfLen(numFrames).fill(null).map((_, index) => {
			const rawOffset = frameListDataView.getUint32(index << 2, true);
			const offset = rawOffset & 4294967294;
			return {
				offset,
				size: (frameListDataView.getUint32(index + 1 << 2, true) & 4294967294) - offset,
				keyframe: !!(rawOffset & 1)
			};
		});
		this.#videoDecoder = genBikVideoDecoder(width, height, subVersion, hasAlpha, hasSwappedUVPlanes);
		this.#videoDecoder.next();
		this.#isSupported = version === 1 && subVersion > 99 && subVersion < 106 && subVersion !== 101;
		this.#header = {
			version,
			subVersion,
			totalSize: headerWords[1] + 8,
			numFrames,
			largestFrameSize: headerWords[3],
			width,
			height,
			fps: headerWords[7] / headerWords[8],
			videoFlags: {
				hasAlpha,
				hasSwappedUVPlanes,
				isGrayscale,
				scaling
			},
			numAudioTracks,
			audioTracks,
			frames
		};
	}
	/**
	* Decoded header of the video.
	*
	* @returns Decoded header.
	*/
	get header() {
		return this.#header;
	}
	/**
	* Whether the audio/images in the video can be processed by the decoder or not.
	*
	* @returns `true` when the audio/images can be processed by the decoder, otherwise `false`.
	*/
	get isSupported() {
		return this.#isSupported;
	}
	/**
	* Get the next frame of the video and decode it.
	*
	* @param prevFrame Optional data structure for a previously decoded frame to re-use (to reduce
	*   garbage collection).
	* @returns Next decoded frame (audio and video).
	*/
	async getNextFrame(prevFrame = null) {
		if (!this.#isSupported) return null;
		const frameHeader = this.#header?.frames[++this.#curFrame];
		if (!frameHeader) {
			this.#curFrame--;
			return null;
		}
		await this.#seek(frameHeader.offset);
		const frameBytes = await this.#ensureReadBytes(frameHeader.size);
		const frameDataView = new DataView(frameBytes.buffer, frameBytes.byteOffset, frameHeader.size);
		const audioTracksFrame = [];
		let audioFrameSize = 0;
		let audioFramePos = 0;
		for (const header of this.#header?.audioTracks ?? []) {
			const size = frameDataView.getUint32(audioFramePos, true);
			const numSamples = size > 3 ? frameDataView.getUint32(audioFramePos + 4, true) : 0;
			audioFrameSize += size;
			const bytes = frameBytes.subarray(audioFramePos + 8, audioFramePos + size + 4);
			audioTracksFrame.push({
				header,
				size,
				numSamples,
				bytes
			});
			audioFramePos += size + 4;
		}
		const audioTracks = [];
		for (const [audioTrackIndex, audioTrackFrame] of audioTracksFrame.entries()) {
			const audioDecoder = this.#audioTrackDecoders[audioTrackIndex];
			if (audioDecoder) {
				const audioHeader = audioTrackFrame.header;
				audioTracks.push({
					header: audioHeader,
					size: audioTrackFrame.size,
					numSamples: audioTrackFrame.numSamples,
					blocks: audioDecoder.next(audioTrackFrame.bytes).value
				});
			}
		}
		const videoFramePos = audioFramePos;
		const videoFrameSize = frameHeader.size - audioFrameSize;
		const videoFrameBytes = frameBytes.subarray(videoFramePos, videoFramePos + videoFrameSize);
		return {
			audioTracks,
			videoFrame: this.#videoDecoder?.next({
				p: videoFrameBytes,
				q: prevFrame?.videoFrame
			}).value ?? null
		};
	}
	/**
	* Skip the specified number of frames of the video. They will still be decoded as decoding a
	* frame can effectively require data from any number of earlier frames.
	*
	* @param numFrames Number of frames to skip, but still decode.
	*/
	async skipFrames(numFrames) {
		let frame = null;
		for (let i = 0; i < numFrames; i++) {
			frame = await this.getNextFrame(frame);
			if (!frame) return;
		}
	}
	/**
	* Reset the state of the decoder so it is ready to start decoding the video from the beginning.
	* Note that this will result in the decoder streaming the video again from the data source.
	*/
	reset() {
		this.#curFrame = -1;
		for (const audioTrackDecoder of this.#audioTrackDecoders) audioTrackDecoder.next();
	}
	/**
	* Read and parse the headers of a BIK video. If successful, return an instance of
	* {@link BikDecoder} for decoding the rest of the video from the data source.
	*
	* @param source Data source that will provide the encoded video data.
	* @returns Decoder instance. Use {@link BikDecoder.header} to access the parsed headers.
	*/
	static async r(source) {
		const decoder = new BikDecoder(source);
		await decoder.#init();
		return decoder;
	}
}.r;
//#endregion
export { createBikDecoder };
