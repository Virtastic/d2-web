import { readFileSync } from 'node:fs';
import assert from 'node:assert';
const s = readFileSync(new URL('../src/core/d2game/d2game_batch_5a8d.cpp', import.meta.url), 'utf8');
const f = s.slice(s.indexOf('d2game_malloc(size_t'), s.indexOf('D2Game+0x069DC'));
assert(f.includes('size > 0xFFFFFFE0u) return nullptr'), 'oversize must fail');
assert(f.includes('size ? size : 1'), 'zero size rounds to 1');
