// Retail player composites are data driven: Armor.txt supplies the body layer's
// light/medium/heavy variant (and the helmet/shield alternate graphic), while
// Weapons.txt supplies both the hand graphic and the animation weapon class.
// The engine owns which records are worn; this module only translates those
// records through the same retail tables the game uses to name its DCC files.

const COMPONENT = ['HD', 'TR', 'LG', 'RA', 'LA', 'RH', 'LH', 'SH', 'S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8'];
const ARMOR_CLASS_COMPONENT = Object.freeze({ rarm: 3, larm: 4, torso: 1, legs: 2, rspad: 8, lspad: 9 });
// D2Items.h: IFLAG_SWITCHIN / IFLAG_SWITCHOUT. The presentation record exposes
// itemFlags directly, so the client need not infer which weapon set is live.
const SWITCHIN = 0x40, SWITCHOUT = 0x80;
const MAIN_HANDS = new Set([4, 5]), SWITCH_HANDS = new Set([11, 12]);

function rows(bytes) {
  const lines = new TextDecoder().decode(bytes).replace(/\r/g, '').split('\n').filter(Boolean);
  const header = lines.shift().split('\t').map((name) => name.trim().toLowerCase());
  return lines.map((line) => {
    const values = line.split('\t'), row = {};
    for (let i = 0; i < header.length; i++) row[header[i]] = (values[i] || '').trim();
    return row;
  });
}

function itemCode(item) {
  if (typeof item?.code === 'string') return item.code.replace(/\0/g, '').trim().toLowerCase();
  const code = Number(item?.code) >>> 0;
  return String.fromCharCode(code & 255, (code >>> 8) & 255, (code >>> 16) & 255, (code >>> 24) & 255).replace(/\0/g, '').trim().toLowerCase();
}

function activeHand(item) {
  const bodyLoc = Number(item?.bodyLoc);
  const flags = Number(item?.itemFlags) >>> 0;
  // Normal equips have neither transition flag. A retail weapon swap puts the
  // active pair in RARM/LARM with SWITCHIN and the inactive pair in SWRARM/
  // SWLARM with SWITCHOUT (ItemMode.cpp); honour both the steady and transition
  // states rather than letting an arbitrary mode-1 record win by list order.
  if (MAIN_HANDS.has(bodyLoc)) return !(flags & SWITCHOUT);
  return SWITCH_HANDS.has(bodyLoc) && Boolean(flags & SWITCHIN);
}

/** Resolve worn presentation records into the exact variant requested per player component. */
export function createPlayerCompositeResolver(read) {
  let tables = null;
  function load() {
    if (tables) return tables;
    const armor = new Map(rows(read('data\\global\\excel\\armor.txt')).map((row) => [row.code.toLowerCase(), row]));
    const weapons = new Map(rows(read('data\\global\\excel\\weapons.txt')).map((row) => [row.code.toLowerCase(), row]));
    // ArmType.txt's row order is the numeric rArm/lArm/Torso/... value in Armor.txt.
    const armorClass = rows(read('data\\global\\excel\\ArmType.txt')).map((row) => row.token.toLowerCase());
    tables = { armor, weapons, armorClass };
    return tables;
  }
  return {
    resolve(items, defaults) {
      const { armor, weapons, armorClass } = load();
      const components = { ...defaults }, source = {};
      let weaponClass = 'hth';
      // Choose the active weapons before assigning composite components.  The
      // Weapons.txt `component` describes an item's normal presentation, not
      // which equipped hand owns it: a Barbarian can wield two entries whose
      // row both says RH.  Retail's body locations are the authority here.
      const hands = new Map();
      for (const item of items || []) {
        if (item?.mode !== 1 || !activeHand(item)) continue;
        const bodyLoc = Number(item.bodyLoc);
        if (bodyLoc === 4 || bodyLoc === 5) hands.set(bodyLoc, item);
      }
      for (const item of items || []) {
        if (item?.mode !== 1) continue;
        const code = itemCode(item), armorRow = armor.get(code), weaponRow = weapons.get(code);
        if (armorRow) {
          // Helmets and shields carry their own alternate DCC token (e.g. fhl, kit).
          const direct = Number(armorRow.component);
          if ([0, 7].includes(direct) && armorRow.alternategfx && (direct !== 7 || activeHand(item))) {
            components[direct] = armorRow.alternategfx.toLowerCase();
            source[direct] = code;
          }
          // A torso item changes all of the body and shoulder-pad layers. Values are
          // ArmType.txt row numbers: 0 lit, 1 med, 2 hvy in retail 1.13c.
          // The six class columns belong to a torso composite (component TR). Shields
          // also carry zeroes in those columns; treating those zeroes as "lit" would
          // incorrectly erase the armour already selected by the torso item.
          if (direct === 1) for (const [field, component] of Object.entries(ARMOR_CLASS_COMPONENT)) {
            const token = armorClass[Number(armorRow[field])];
            if (token) { components[component] = token; source[component] = code; }
          }
        }
      }
      for (const [bodyLoc, item] of hands) {
        const code = itemCode(item), weaponRow = weapons.get(code);
        if (!weaponRow?.alternategfx) continue;
        const component = bodyLoc === 4 ? 5 : 6;
        components[component] = weaponRow.alternategfx.toLowerCase();
        source[component] = code;
        // The equipped right hand selects the COF; the left hand never gets
        // to replace it merely because it followed in the presentation list.
        if (bodyLoc === 4 && weaponRow.wclass) weaponClass = weaponRow.wclass.toLowerCase();
      }
      const layers = COMPONENT.map((component, index) => ({ component, variant: components[index] || '', source: source[index] || null }))
        .filter((layer) => layer.variant);
      return { weaponClass, components, source, layers };
    },
  };
}
