import { test } from 'node:test';
import assert from 'node:assert/strict';
import { auditSessionActiveNearFeed, parseSessionActiveNearFeed, SESSION_ACTIVE_NEAR_FEED_BYTES,
  orderFloorsByActiveNearFeed } from '../play/assets/session-active-near-feed.mjs';
import { weatherWaterAnchors } from '../play/assets/weather-water-anchors.mjs';

function feedRecord(kind, roomOrdinal, levelId, tileX, tileY, floorOrdinal = 0, selector = 0) {
  return { kind, roomOrdinal, levelId, tileX, tileY, floorOrdinal, selector };
}

test('active-near feed parser preserves room and floor identities in the 28-byte engine record', () => {
  const bytes = new Uint8Array(SESSION_ACTIVE_NEAR_FEED_BYTES * 2);
  const view = new DataView(bytes.buffer);
  const values = [
    [0, 7, 76, 1040, 992, 0, 0],
    [1, 7, 76, 1040, 992, 12, 2],
  ];
  values.forEach((record, i) => record.forEach((value, field) =>
    view.setInt32(i * SESSION_ACTIVE_NEAR_FEED_BYTES + field * 4, value, true)));

  assert.deepEqual(parseSessionActiveNearFeed(bytes, 0, 2), [
    feedRecord(0, 7, 76, 1040, 992),
    feedRecord(1, 7, 76, 1040, 992, 12, 2),
  ]);
  assert.deepEqual(parseSessionActiveNearFeed(bytes, 0, 0), []);
});

test('water candidates follow engine room, selector, and source-tile order by identity', () => {
  const floor = (room, ordinal, selector, worldX) => ({
    tile: { room, ordinal, flags: selector << 14, worldX, worldY: 2, orientation: 0 },
    dt1Tile: { materialFlags: 2, orientation: 0 },
  });
  const firstInEngineOrder = floor(9, 3, 1, 13);
  const secondInEngineOrder = floor(9, 8, 2, 12);
  const outOfFeed = floor(10, 1, 1, 11);
  const earlierRoom = floor(4, 6, 1, 10);
  const feed = [
    feedRecord(0, 4, 76, 1032, 992),
    feedRecord(1, 4, 76, 1032, 992, 6, 1),
    feedRecord(0, 9, 76, 1040, 992),
    feedRecord(1, 9, 76, 1040, 992, 8, 2),
    feedRecord(1, 9, 76, 1040, 992, 3, 1),
  ];

  assert.deepEqual(orderFloorsByActiveNearFeed([firstInEngineOrder, outOfFeed, secondInEngineOrder, earlierRoom], feed),
    [earlierRoom, secondInEngineOrder, firstInEngineOrder]);
  assert.deepEqual(weatherWaterAnchors([firstInEngineOrder, outOfFeed, secondInEngineOrder, earlierRoom], feed).map(({ x }) => x),
    [50, 60, 65]);
  assert.deepEqual(weatherWaterAnchors([firstInEngineOrder], []), [],
    'an available empty native active-room feed cannot fall back to JS-wide room order');
});

test('feed audit exposes malformed identities and selector/source order', () => {
  const valid = [feedRecord(0, 7, 76, 1040, 992), feedRecord(1, 7, 76, 1040, 992, 2, 1),
    feedRecord(1, 7, 76, 1040, 992, 9, 2)];
  assert.deepEqual(auditSessionActiveNearFeed(valid), { available: true, roomCount: 1, floorCount: 2, errors: [] });
  const invalid = [feedRecord(0, 7, 76, 1040, 992), feedRecord(1, 8, 76, 1040, 992, 9, 2),
    feedRecord(1, 7, 76, 1040, 992, 2, 1)];
  const audit = auditSessionActiveNearFeed(invalid);
  assert.ok(audit.errors.includes('floor-1-room-identity-mismatch'));
  assert.ok(audit.errors.includes('floor-2-out-of-order'));
});
