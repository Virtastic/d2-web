// Tooltip properties are read from the engine on first use, then reused while an item remains
// among the most recently inspected IDs. Item IDs accumulate during long sessions, so retain a
// bounded working set instead of one entry for every item ever seen.
export function createItemPropertyCache(limit = 512) {
  if (!Number.isInteger(limit) || limit < 1) throw new RangeError('item property cache limit must be positive');
  const entries = new Map();
  return {
    get(id) {
      if (!entries.has(id)) return undefined;
      const value = entries.get(id);
      entries.delete(id);
      entries.set(id, value);
      return value;
    },
    getOrRead(id, read, shouldCache = () => true) {
      const cached = this.get(id);
      if (cached !== undefined) return cached;
      const value = read();
      if (shouldCache(value)) this.set(id, value);
      return value;
    },
    set(id, value) {
      if (entries.has(id)) entries.delete(id);
      entries.set(id, value);
      if (entries.size > limit) entries.delete(entries.keys().next().value);
    },
    delete(id) { return entries.delete(id); },
    clear() { entries.clear(); },
    get size() { return entries.size; },
  };
}

// A magic/set/rare/unique item that reads back with no name is not identified yet (or the read raced
// the engine): caching it would pin the base name and white colour after identification.
export const itemPropsCacheable = (p) => p.lines.length > 0 && !(p.quality >= 4 && !p.title);
