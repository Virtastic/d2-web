import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';

// Packet 0x35 bytes 9-12 carry the item's dwMode low word, zero-extended, ahead of the durability dword.
test('single-item repair packet stores the item mode low word at byte 9', () => {
  const src = readFileSync('src/core/monspawn/monspawn_check.cpp', 'utf8');
  const fn = src.slice(src.indexOf('d2_session_command_vendor_repair('));
  const body = fn.slice(0, fn.indexOf('d2_session_command_vendor_repair_all'));
  assert.match(body, /itemMode = \(uint16_t\)item->dwAnimMode/);
  assert.match(body, /memcpy\(packet \+ 9, &itemMode, sizeof itemMode\)/);
  assert.ok(body.indexOf('packet + 9') < body.indexOf('packet + 13'));
});
