// All sixteen GBA levels, in game order, as lists of the web level builder's calls (see build() in "Super Mario
// Hellbound.html"): the web game's three levels, the GBA-only ones (gba-levels.js), the GBA-only additions to the web
// levels (GBA_EXTRAS) and, since 2026-10-17, each level's second half (gba-halves.js; the user had every level made twice
// as long, with a proportionate number of enemies, embers and mushrooms).
// Each level is recorded as calls ([method, args]) so it can be moved about and written out again as source: build-assets.js
// builds the levels from them, and check-levels.js writes them into a stand-in HTML for the web game's checkers.
const fs = require('fs'), path = require('path');
const WEB = path.resolve(__dirname, '../..');
const html = fs.readFileSync(path.join(WEB, 'Super Mario Hellbound.html'), 'utf8');
const script = html.match(/<script>([\s\S]*)<\/script>/)[1];
const buildSrc = script.slice(script.indexOf('function build(def)'), script.indexOf('return L;\n}', script.indexOf('function build(def)')) + 'return L;\n}'.length);
const lvStart = script.indexOf('const LEVELS'), lvEnd = script.indexOf('return L;\n}', lvStart) + 'return L;\n}'.length;
const mod = { exports: {} };
new Function('module', `const T = 16, ROWS = 17, PH = 24, COINS_PER_BLOCK = 5, BEAST_HP = 6;\n${script.slice(lvStart, lvEnd)}\nmodule.exports = { LEVELS, build };`)(mod);

// GBA-only additions, run after each level's own builder (the web game's levels stay as they are)
const GBA_EXTRAS = {
  // Four more aliens in the Jungle, at the user's request (2026-10-16), on stretches of floor that had room: a second
  // one on Part 1's long floor after the spike pits, one on the high ledge at the top of the elevator, one on the floor
  // under the plank before the second checkpoint, and a guard in front of the exit door
  Jungle: b => { b.enemy(59, 64, 13); b.enemy(157, 161, 7); b.enemy(174, 178, 13); b.enemy(213, 215, 11); },
  // Three more reapers in the Graveyard, spread out (user request, 2026-10-16), each in a stretch that had none: a plank
  // in the middle of Part 1's empty plank hops, the raised step with the mushroom in Part 2, and the last plank hop before
  // the final stretch. Kept off the steps next to checkpoints, so a respawn isn't straight into a rising reaper.
  Graveyard: b => { b.enemy(61, 63, 9); b.enemy(127, 130, 8); b.enemy(172, 174, 9); },
  // A harder Hell (World 4-4), at the user's request (2026-10-16), leaving the Fire Skull's arena (col 352 on) alone:
  // seven more beasts (23 -> 30), one on each of three plank hops that had none (the first plank chain, the first plank
  // of the chain after the shelf, the last plank before the boss checkpoint), one each on the floor just after the start,
  // Part 1's long floor, the floor after the shelf and between the two spike pits of Part 3's spike run; and two more
  // spike pits (on Part 1's long floor and under the plank before the second checkpoint). Checkpoints keep their room.
  Hell: b => {
    b.enemy(15, 19, 13); b.enemy(57, 59, 9); b.enemy(86, 91, 13); b.enemy(126, 130, 13); b.enemy(133, 135, 11);
    b.enemy(228, 230, 13); b.enemy(325, 327, 11);
    b.spikes(82, 83, 13); b.spikes(160, 161, 13);
  },
};

// Builder calls -> list. Methods are only recorded here (coins() and row() expand when the list is replayed).
function record(fn) {
  const calls = [];
  fn(new Proxy({}, { get: (_, m) => (...a) => { calls.push([m, a]); } }));
  return calls;
}
// Which arguments of each builder method are columns
const XS = { ground: [0, 1], block: [0, 1], plat: [0, 1], spikes: [0, 1], coins: [0, 1], row: [0, 1], enemy: [0, 1],
  ember: [0], mush: [0], bluemush: [0], coin: [0], crack: [0], oneup: [0], check: [0], start: [0], door: [0], mover: [0, 3], boss: [0, 1] };
const STEPPED = { coins: 3, row: 3 };   // the argument holding the step, for runs that are split in two
const moved = ([m, a], dx) => [m, a.map((v, i) => XS[m].includes(i) ? v + dx : v)];
// Moves everything from column `at` on `len` columns right (a boss level's checkpoint and arena, to make room for the
// second half), splitting runs of ground, planks, spikes, coins and embers that cross `at`
function openGap(calls, at, len) {
  const out = [];
  for (const c of calls) {
    const [m, a] = c, xs = XS[m];
    if (!xs) throw new Error(`unknown builder call ${m}`);
    const lo = Math.min(...xs.map(i => a[i])), hi = Math.max(...xs.map(i => a[i]));
    if (lo >= at) { out.push(moved(c, len)); continue; }
    if (hi < at) { out.push(c); continue; }
    if (m === 'enemy' || m === 'mover' || m === 'boss') throw new Error(`${m}(${a}) crosses column ${at}`);
    const step = STEPPED[m] !== undefined ? (a[STEPPED[m]] || 1) : 1, x2 = a[0] + Math.ceil((at - a[0]) / step) * step;
    const left = [...a]; left[1] = at - 1; out.push([m, left]);
    if (x2 <= a[1]) { const right = [...a]; right[0] = x2 + len; right[1] = a[1] + len; out.push([m, right]); }
  }
  return out;
}

// Extra 1-up blocks (user request, 2026-10-17), as [col, row]: one more in 4-1..4-3, two more in 4-4, all in the second
// half on a ledge's end (4-2's and 4-3's mirror the first half's 1-up; 4-2's moved off its floating plank, (552, 7) ->
// (566, 13), user, 2026-10-20)
const EXTRA_ONEUPS = { 'Underworld 1': [[656, 11]], 'Underworld 2': [[566, 13]], 'Underworld 3': [[619, 11]], Hell: [[372, 10], [570, 8]] };
// Extra enemies (user request, 2026-10-19: five more in World 4-4, 60 -> 65), as enemy() args in final columns: on
// floor or plank stretches with no beast within 4 columns, no spikes or blocks beside them and no checkpoint, mushroom,
// 1-up, star or flamethrower next to them, spread through the level. Then five more in Worlds 1-2..1-4 and 2-1..2-4 (user
// request, 2026-10-19), picked the same way (each as far as it could be from every other enemy; 3-col patrols, 3+ cols
// from the next enemy, mostly on the planks the existing patrols leave free)
const EXTRA_ENEMIES = {
  'Jungle 2': [[48, 50, 8], [122, 124, 11], [162, 164, 13], [327, 329, 10], [388, 390, 11]],
  'Jungle 3': [[37, 39, 9], [47, 49, 9], [122, 124, 9], [233, 235, 9], [366, 368, 10]],
  'Jungle 4': [[38, 40, 11], [123, 125, 6], [217, 219, 10], [409, 411, 11], [415, 417, 11]],
  Graveyard: [[51, 53, 9], [107, 109, 9], [167, 169, 7], [292, 294, 10], [350, 352, 8]],
  'Graveyard 2': [[40, 42, 13], [105, 107, 8], [112, 114, 9], [269, 271, 10], [371, 373, 9]],
  'Graveyard 3': [[39, 41, 9], [45, 47, 8], [235, 237, 7], [293, 295, 10], [345, 347, 9]],
  'Graveyard 4': [[49, 51, 8], [167, 169, 5], [197, 199, 12], [343, 345, 9], [373, 375, 12]],
  Hell: [[47, 49, 13], [119, 121, 13], [343, 345, 13], [447, 449, 9], [611, 613, 10]],
};
// The flamethrower pickups (user requests, 2026-10-18), as [col, row] (the cell it stands in) or as how far through the
// level, for flameSpot() to find a spot near. World 1-1 had the first one (placed by hand at col 336, row 13) until the
// user had it removed, the same day. The web builder has no such thing, so it isn't one of the calls; main.cpp gets it
// as level_info::flame_item.
const FLAMETHROWERS = { 'Jungle 3': 0.35, 'Graveyard 2': 0.5, 'Graveyard 4': 0.25, City: 0.5, 'City 3': 0.35,
  'Underworld 2': 0.6, Hell: 0.7 };   // 4-2 was 0.35 until 2026-10-19 (user: 0.45, then 0.6)
// The rocket launcher pickups (user request, 2026-10-19), the same way: World 1-2 at about 60%. Found by flameSpot() with
// the level's star block in place (World 1-2 keeps its star), so the two never share a spot; main.cpp gets it as
// level_info::rocket_item.
// Then World 2-1, 3-2 and 4-1 (user request, 2026-10-19: each between 35% and 75%, at a different point in each)
// World 1-2's moved right before its first checkpoint (col 112) on 2026-10-20 (user); col 110 clears the alien patrol (ends col 109)
const ROCKETS = { 'Jungle 2': [110, 9], Graveyard: 0.675, 'City 2': 0.505,
  // World 4-1: on the landing right after the moving platform (184-194) past the second checkpoint: the first tile of the
  // ledge, beside a beast's patrol (198-205) (user, 2026-10-21; was 0.406 = 286, 11, then 180, 13 just past the checkpoint)
  'Underworld 1': [197, 11] };
// A spot for a flamethrower pickup as near `frac` of the way through the level as there is one: standing on the ground or
// a plank (the lowest floor at that column, the main way through), two tiles of air over it, nothing to collect in its
// cell, no spikes under it or beside it, no block within two tiles, and before the exit door (or boss gate)
// clearFoes (the rocket launcher): also no enemy patrol or checkpoint within two tiles of it, so it isn't picked up in an alien's path
function flameSpot(def, calls, frac, clearFoes = false) {
  const L = mod.exports.build({ ...def, build: b => { for (const [m, a] of calls) b[m](...a); } }), g = L.grid, w = def.w;
  const at = (x, y) => x < 0 || x >= w || y < 0 ? -1 : y >= 17 ? 1 : g[y][x];
  const items = new Set([...L.coins, ...L.embers, ...L.mush.map(m => ({ x: m.x + 6, y: m.y + 6 }))].map(o => Math.floor(o.x / 16) + ',' + Math.floor(o.y / 16)));
  const exit = L.boss ? L.boss.gate : L.door ? Math.floor(L.door.x / 16) : w;
  const fits = (c, r) => {
    if (at(c, r) !== 0 || at(c, r - 1) !== 0 || at(c, r - 2) !== 0 || items.has(c + ',' + r)) return false;
    if (at(c, r + 1) !== 1 && at(c, r + 1) !== 2) return false;
    for (let x = c - 1; x <= c + 1; x++) if (at(x, r) === 3 || at(x, r + 1) === 3) return false;
    for (let x = c - 2; x <= c + 2; x++) for (let y = r - 2; y <= r + 1; y++) if (at(x, y) === 4) return false;
    if (clearFoes && L.checks.some(k => Math.abs(k.x / 16 - c) <= 2)) return false;
    if (clearFoes && L.enemies.some(e => e.a / 16 - 2 <= c + 1 && (e.b + e.w) / 16 + 2 >= c && Math.abs((e.y + e.h) / 16 - 1 - r) <= 2)) return false;
    return true;
  };
  const mid = Math.round(frac * w);
  for (let d = 0; d < w; d++)
    for (const c of d ? [mid + d, mid - d] : [mid]) {
      if (c < 2 || c >= exit - 2) continue;
      for (let r = 15; r >= 1; r--) if (fits(c, r)) return [c, r];
    }
  throw new Error(`${def.name}: no spot for the flamethrower`);
}
// Fewer enemies (user request, 2026-10-20: 10 fewer hell beasts in World 4-4, 65 -> 55), as enemy() arguments. Picked from
// the most crowded stretches (each time the beast with its neighbours nearest), leaving the ones the user had added
// (EXTRA_ENEMIES). Dropped after the star, flamethrower and rocket spots are found, so those don't move.
const DROP_ENEMIES = { Hell: [[126, 130, 13], [138, 140, 9], [236, 237, 13], [304, 306, 13], [320, 322, 9], [353, 357, 11],
  [398, 400, 9], [502, 505, 10], [530, 532, 9], [669, 671, 11]] };
// More aliens and mechs (user requests, 2026-10-20: 5 more aliens in World 1-1, 8 more in each of 1-2 to 1-4), as enemy()
// arguments: 2-column patrols on floor or planks, picked one at a time as far as possible from every enemy (max-min
// distance), away from blocks, pickups, checkpoints, movers, spikes, the start and the door. Added after the star,
// flamethrower and rocket spots are found, so those don't move.
const MORE_ENEMIES = {
  Jungle: [[112, 114, 10], [124, 126, 10], [152, 154, 7], [203, 205, 11], [261, 263, 8]],
  'Jungle 2': [[43, 45, 10], [99, 101, 9], [149, 151, 8], [200, 202, 11], [318, 320, 8], [332, 334, 12], [343, 345, 13], [368, 370, 8]],
  'Jungle 3': [[42, 44, 7], [89, 91, 11], [138, 140, 12], [201, 203, 11], [321, 323, 12], [361, 363, 8], [378, 380, 13], [422, 424, 11]],
  'Jungle 4': [[22, 24, 7], [60, 62, 10], [128, 130, 9], [142, 144, 13], [222, 224, 8], [237, 239, 6], [290, 292, 5], [393, 395, 6]],
  // Mechs in World 3 (user request, 2026-10-20: 4-7 more each), the same way, but also at least 3 tiles from any enemy, 12
  // columns from each other and 10 from a checkpoint: as many as fit, 5, 5, 4 and 6
  City: [[31, 33, 10], [54, 56, 13], [163, 165, 13], [324, 326, 10], [395, 397, 13]],
  'City 2': [[128, 130, 11], [238, 240, 6], [309, 311, 8], [367, 369, 5], [394, 396, 13]],
  'City 3': [[52, 54, 7], [84, 86, 6], [123, 125, 10], [303, 305, 5]],
  'City 4': [[104, 106, 9], [195, 197, 12], [217, 219, 12], [312, 314, 9], [403, 405, 12], [421, 423, 13]],
};
// Hard: 12 more aliens in each World 1 level (user request, 2026-10-23), and 12 more mech units in each World 3 level. The floors are nearly all patrolled already, so
// on Hard 12 aliens with long patrols (6+ columns) become two aliens side by side, each walking half the stretch (the
// user's choice). The longest patrols are split first, spread out along the level. hardSplits() returns their enemy()
// arguments; build-assets keeps those aliens Normal-only and adds the halves as Hard-only. Not builder calls: the web
// game and the checkers see the level as Normal has it.
// World 3's mech units the same way (user request, 2026-10-23: about 12 more each; World 3 is as full as World 1)
const HARD_SPLITS = { Jungle: 12, 'Jungle 2': 12, 'Jungle 3': 12, 'Jungle 4': 12, City: 12, 'City 2': 12, 'City 3': 12, 'City 4': 12 };
// Hard: 8 more reapers in each World 2 level (user request, 2026-10-23), as enemy() arguments: World 2 has room, so these
// are new 2-column patrols on floor or planks (scratchpad pick_hard.js: two free rows above, max-min distance, at least 3
// tiles from any reaper's whole patrol, 5 columns from each other and checkpoints, 10 from the start, away from spikes,
// mushrooms and power-ups). Hard-only, like the splits; HARD_FLYERS more flying demons come with them (hardFlyerSpots).
const HARD_ENEMIES = {
  Graveyard: [[29, 31, 8], [66, 68, 11], [90, 92, 7], [102, 104, 11], [197, 199, 13], [259, 261, 10], [307, 309, 9], [345, 347, 10]],
  'Graveyard 2': [[27, 29, 8], [52, 54, 8], [62, 64, 8], [67, 69, 10], [156, 158, 9], [254, 256, 8], [264, 266, 8], [349, 351, 6]],
  'Graveyard 3': [[50, 52, 10], [60, 62, 10], [76, 78, 10], [108, 110, 9], [118, 120, 9], [245, 247, 7], [310, 312, 8], [340, 342, 7]],
  'Graveyard 4': [[101, 103, 9], [130, 132, 5], [155, 157, 9], [204, 206, 10], [216, 218, 4], [259, 261, 5], [301, 303, 10], [357, 359, 8]],
  // World 4-1 to 4-3: 5 more hell beasts each (user request, 2026-10-23), picked the same way but 10 columns from
  // checkpoints and the exit door (beasts fire far)
  // World 4-1: 3 more (user request, 2026-10-24: 5 -> 8), from scratchpad pick_u1.js's legal spots, spread along the level
  'Underworld 1': [[32, 34, 10], [47, 49, 13], [145, 147, 11], [311, 313, 11], [351, 353, 13], [406, 408, 13], [549, 551, 7], [673, 675, 11]],
  'Underworld 2': [[53, 55, 8], [68, 70, 10], [184, 186, 13], [351, 353, 13], [399, 401, 8]],
  'Underworld 3': [[177, 179, 13], [215, 217, 5], [355, 357, 13], [417, 419, 9], [563, 565, 5]],
};
// (and 8 more in each World 4 level, user request, 2026-10-23)
// Hard has one checkpoint fewer in every level (user request, 2026-10-23): the one whose removal leaves the shortest
// longest stretch between the start, the checkpoints and the exit. A boss level keeps the checkpoint by its arena.
// Returns that checkpoint's column; build-assets marks it Normal-only.
function hardDropCheck(def, calls) {
  const L = mod.exports.build({ ...def, build: b => { for (const [m, a] of calls) b[m](...a); } });
  const exit = L.boss ? L.boss.gate : Math.floor(L.door.x / 16), start = Math.floor(L.start.x / 16);
  const cols = L.checks.map(c => c.col).sort((a, b) => a - b);
  let best = null, longest = Infinity;
  cols.forEach((c, i) => {
    if (L.boss && i === cols.length - 1) return;
    const p = [start, ...cols.filter(k => k !== c), exit], m = Math.max(...p.slice(1).map((x, k) => x - p[k]));
    if (m < longest) { longest = m; best = c; }
  });
  return best;
}
// Hard has fewer red mushrooms in World 4 (user request, 2026-10-23): 3 fewer in 4-1 to 4-3, 2 in 4-4. Taken one at a
// time where they're most bunched up: the red one whose removal leaves the shortest longest stretch between the start,
// the mushrooms (blue ones count, and stay, as does Hard's own blue one, extra) and the exit, then the one with the
// closest neighbours. None within 10 columns of a boss arena (the heal before the fight). Returns indexes into the
// level's mushrooms; build-assets marks them Normal-only.
// (and 2 fewer in World 1-1, all of World 2, and World 3-1, 3-2 and 3-4, user request, later the same day)
const HARD_DROP_MUSH = { 'Underworld 1': 3, 'Underworld 2': 3, 'Underworld 3': 3, Hell: 2, Jungle: 2,
  Graveyard: 2, 'Graveyard 2': 2, 'Graveyard 3': 2, 'Graveyard 4': 2, City: 2, 'City 2': 2, 'City 4': 2 };
// Expert has one red mushroom fewer than Hard in every level (user request, 2026-10-24): hardDropMush's next pick after
// Hard's (it picks one at a time, so its first picks are Hard's own); level_info::expert_skip_mushrooms
const EXPERT_DROP_MUSH = 1;
// Except here (user request, 2026-10-31): Expert keeps this level's mushrooms
const EXPERT_KEEP_MUSH = new Set(['Jungle 2']);
// Hard leaves out these levels' star block (none now: World 4-1's came back, user, 2026-11-01); level_info::hard_no_star
const HARD_NO_STAR = new Set([]);
// Hard alone (not Expert) leaves out these levels' star block (Worlds 4-3 and 4-4, user, 2026-11-01);
// level_info::hard_only_no_star
const HARD_ONLY_NO_STAR = new Set(['Underworld 3', 'Hell']);
// Expert leaves out these levels' rocket launcher (World 3-2, user, 2026-10-24); level_info::expert_no_rocket
const EXPERT_NO_ROCKET = new Set(['City 2']);
// Expert leaves out these levels' star block (World 2-3, user, 2026-10-24; World 3-4's too until 2026-10-31, user);
// level_info::expert_no_star
const EXPERT_NO_STAR = new Set(['Graveyard 3']);
// Expert's own star block, [col, row], where the level's (if any) is left out (World 4-1, user, 2026-10-25: "an area
// crowded with enemies" -- 4 beasts and 2 demons within a screen on Expert, the most of any legal block spot, by the
// star block rules); level_info::expert_star. Not a builder call, so the checkers don't see it.
// (World 2-1, user, 2026-10-31: about 25% through, at the end of the floor before the gap, 2 cols past the checkpoint;
// that level has no star of its own)
const EXPERT_STAR = { 'Underworld 1': [423, 9], Graveyard: [99, 13] };
// Patrols Expert leaves out, exact [a, b, row] as they stand on Expert (World 3-4 -5, user, 2026-10-31: from the most
// crowded stretches, by repeatedly taking the mech with the most others within 15 columns)
const EXPERT_DROP_ENEMIES = { 'City 4': [[396, 400, 9], [188, 189, 9], [417, 419, 12], [190, 192, 9], [408, 411, 12]] };
// Expert's own 1-up block, [col, row], besides the level's (World 2-1, user, 2026-10-31); level_info::expert_oneup.
// Not a builder call either, so the checkers don't see it.
const EXPERT_ONEUP = { Graveyard: [296, 10] };
function hardDropMush(def, calls, n, extra = []) {
  const L = mod.exports.build({ ...def, build: b => { for (const [m, a] of calls) b[m](...a); } });
  const exit = L.boss ? L.boss.gate : Math.floor(L.door.x / 16), start = Math.floor(L.start.x / 16);
  const col = i => L.mush[i].x / 16, left = L.mush.map((_, i) => i), out = [];
  for (let k = 0; k < n; k++) {
    let best = null, key = null;
    for (const i of left) {
      if (L.mush[i].blue || col(i) >= exit || (L.boss && col(i) >= exit - 10)) continue;
      const p = [start, ...left.filter(j => j !== i).map(col), ...extra].sort((a, b) => a - b); p.push(exit);
      const longest = Math.max(...p.slice(1).map((x, j) => x - p[j]));
      const near = Math.min(...[...left.filter(j => j !== i).map(col), ...extra].map(x => Math.abs(x - col(i))));
      if (! key || longest < key[0] || (longest === key[0] && near < key[1])) { best = i; key = [longest, near]; }
    }
    out.push(best); left.splice(left.indexOf(best), 1);
  }
  return out.sort((a, b) => a - b);
}
// Hard adds a blue mushroom to every level of Worlds 1-3 (user request, 2026-10-23): in the middle of the longest
// stretch without one (placed before Hard's drops are chosen, so they leave room round it), on the floor under or beside a coin, at most 4 rows below it (the checkers
// have every coin reachable, so Mario gets to that floor), not on a coin, not next to spikes. Returns bluemush() arguments [col, row]; build-assets adds it Hard-only.
const HARD_BLUE = /^(Jungle|Graveyard|City)/;
function hardBlueMush(def, calls) {
  const L = mod.exports.build({ ...def, build: b => { for (const [m, a] of calls) b[m](...a); } }), g = L.grid, w = def.w;
  const exit = L.boss ? L.boss.gate : Math.floor(L.door.x / 16), start = Math.floor(L.start.x / 16);
  const at = (c, r) => (c < 0 || c >= w || r < 0 || r >= 17) ? 1 : g[r][c];
  const p = [start, ...L.mush.map(m => Math.floor(m.x / 16)), exit].sort((a, b) => a - b);
  let gi = 0; p.slice(1).forEach((x, i) => { if (x - p[i] > p[gi + 1] - p[gi]) gi = i; });
  const target = (p[gi] + p[gi + 1]) / 2;
  const coins = new Set(L.coins.map(c => `${Math.floor(c.x / 16)},${Math.floor(c.y / 16)}`));
  let best = null;
  for (const k of coins) {
    const [cc, cr] = k.split(',').map(Number);
    for (const c of [cc - 1, cc, cc + 1]) {
      let r = cr; while (r < cr + 4 && at(c, r + 1) === 0) r++;   // down to the floor
      if (c <= start + 3 || c >= exit - 3 || coins.has(`${c},${r}`)) continue;
      if ([...Array(r - cr + 1).keys()].some(d => at(c, cr + d) !== 0) || at(c, r - 1) !== 0 || ! [1, 2].includes(at(c, r + 1))) continue;
      if ([-2, -1, 0, 1, 2].some(d => at(c + d, r) === 3 || at(c + d, r + 1) === 3)) continue;
      if (! best || Math.abs(c - target) < Math.abs(best[0] - target)) best = [c, r];
    }
  }
  if (! best) throw new Error(`${def.name}: no spot for Hard's blue mushroom`);
  return best;
}
const HARD_FLYERS = { Graveyard: 5, 'Graveyard 2': 5, 'Graveyard 3': 5, 'Graveyard 4': 5,
  'Underworld 1': 10, 'Underworld 2': 8, 'Underworld 3': 8, Hell: 8 };   // World 4-1 8 -> 10 (user request, 2026-10-24)
// Expert (user request, 2026-10-24: 10 more enemies than Hard in every World 1 and World 3 level): more of Hard's patrols
// split the same way, picked from Hard's own list (its halves and extras included) down to 4 columns, so some halves are
// 2 columns wide: an alien or mech with nowhere to walk stands guard (main.cpp). Expert is Hard plus these.
const EXPERT_SPLITS = { Jungle: 10, 'Jungle 2': 10, 'Jungle 3': 10, 'Jungle 4': 10, City: 10, 'City 2': 10, 'City 3': 10, 'City 4': 10,
  // World 2 (user request, 2026-10-24: 7 more reapers each): EXPERT_ENEMIES' new spots first, then splits for the rest,
  // only of patrols 6+ columns long (EXPERT_SPLIT_MIN), so every half still walks
  Graveyard: 4, 'Graveyard 2': 5, 'Graveyard 4': 3 };
const EXPERT_SPLIT_MIN = { Graveyard: 6, 'Graveyard 2': 6, 'Graveyard 4': 6 };
// Expert's new reapers in World 2 (user request, 2026-10-24), as enemy() arguments: free floor or planks, picked like
// HARD_ENEMIES (scratchpad pick_x.js: two free rows above, max-min distance, 3+ tiles from every Normal/Hard reaper's
// whole patrol, 5 columns from each other, checkpoints and the exit, 10 from the start, away from spikes, blocks,
// mushrooms, power-ups and movers). Only these fit those rules; EXPERT_SPLITS makes up the rest to 7.
const EXPERT_ENEMIES = {
  Graveyard: [[133, 135, 8], [202, 204, 11], [212, 214, 11]],
  'Graveyard 2': [[166, 168, 9], [376, 378, 11]],
  'Graveyard 3': [[12, 14, 11], [22, 24, 11], [173, 175, 10], [183, 185, 10], [212, 214, 10], [266, 268, 10], [371, 373, 10]],
  'Graveyard 4': [[54, 56, 6], [111, 113, 9], [249, 251, 9], [323, 325, 9]],
  // World 4 (user request, 2026-10-24: 5 more hell beasts each): the same rules with beast spacing (pick_x.js ... 5 3 10 10:
  // three free rows, 10 columns from checkpoints and the exit); of the spots that fit, the five furthest from Hard's extras
  // and each other
  'Underworld 1': [[65, 67, 11], [373, 375, 7], [498, 500, 9], [513, 515, 11], [599, 601, 11]],
  'Underworld 2': [[134, 136, 11], [287, 289, 9], [484, 486, 7], [550, 552, 7], [667, 669, 11]],
  'Underworld 3': [[40, 42, 11], [279, 281, 11], [503, 505, 11], [608, 610, 6], [671, 673, 8]],
  Hell: [[52, 54, 11], [302, 304, 13], [398, 400, 9], [530, 532, 9], [669, 671, 11]],
};
// Expert's extra flying demons (user request, 2026-10-24: 3 more in each World 2 level), placed like Hard's
// (hardFlyerSpots, away from Normal's and Hard's)
const EXPERT_FLYERS = { Graveyard: 3, 'Graveyard 2': 3, 'Graveyard 3': 3, 'Graveyard 4': 3,
  'Underworld 1': 5, 'Underworld 2': 5, 'Underworld 3': 5, Hell: 5 };   // (World 4: 5 more each, user request, same day)
const half = ([x0, x1, row]) => { const m = x0 + Math.floor((x1 - x0 + 1) / 2); return [[x0, m - 1, row], [m, x1, row]]; };
function hardPatrols(calls, splits, extra) {
  const keys = splits.map(a => a.join()), out = [];
  for (const [m, a] of calls) if (m === 'enemy') out.push(...(keys.includes(a.join()) ? half(a) : [a]));
  return out.concat(extra);
}
function hardSplits(calls, n, min = 6) {
  const long = calls.filter(([m, a]) => m === 'enemy' && a[1] - a[0] + 1 >= min).map(([, a]) => a);
  if (long.length < n) throw new Error(`only ${long.length} alien patrols are long enough to split, not ${n}`);
  const picked = [], mid = a => (a[0] + a[1]) / 2;
  while (picked.length < n) {
    // The longest patrol, then the furthest from those already picked
    let best = null, key = null;
    for (const a of long) {
      if (picked.includes(a)) continue;
      const k = [a[1] - a[0], picked.length ? Math.min(...picked.map(b => Math.abs(mid(a) - mid(b)))) : 0];
      if (!best || k[0] > key[0] || (k[0] === key[0] && k[1] > key[1])) { best = a; key = k; }
    }
    picked.push(best);
  }
  return picked.sort((a, b) => a[0] - b[0]);
}
// Flying demons (user request, 2026-10-20: 7-10 more enemies in each of Worlds 2-1 to 2-4, flying, 2 HP, Gothicvania's
// demon art): how many per level. GBA-only; not builder calls, so the web game and the checkers don't see them. Each is
// [a, b, row]: it patrols columns a to b with its middle on row 'row' (main.cpp FOE_DEMON).
const FLYERS = { Graveyard: 8, 'Graveyard 2': 9, 'Graveyard 3': 9, 'Graveyard 4': 10,
  'Underworld 1': 6, 'Underworld 2': 7, 'Underworld 3': 7, Hell: 8 };   // World 4 too (user request, 2026-10-20: 6-8 each)
// Spread evenly from the start to the exit door (or boss gate): the level is cut into n equal stretches and each gets one,
// as near the stretch's middle as there's room: a 4-column patrol 4 rows above the highest floor under it (so Mario can
// walk under it and has to shoot up at it), open air one tile around it, not over a pit (it could knock Mario in), not
// over a checkpoint, not near the start.
// Where a flying demon can patrol: a 4-column patrol 4 rows above the highest floor under it, open air one tile around it,
// no pit under it, no checkpoint near, between start + 14 and exit - 6. spotAt(a) = [a, a + 4, row] or null.
function flyerRoom(def, calls) {
  const L = mod.exports.build({ ...def, build: b => { for (const [m, a] of calls) b[m](...a); } }), g = L.grid, w = def.w;
  const exit = L.boss ? L.boss.gate : L.door ? Math.floor(L.door.x / 16) : w;
  const top = c => { for (let r = 0; r < 17; r++) if (g[r][c] !== 0) return r; return 17; };   // highest floor in a column
  const start = Math.floor(L.start.x / 16) + 14, end = exit - 6;
  const fits = (a, b, row) => {
    if (row < 4) return false;   // (higher is under the HUD)
    for (let c = a - 1; c <= b + 2; c++) for (let r = row - 1; r <= row + 1; r++) if (c < 0 || c >= w || g[r][c] !== 0) return false;
    return ! L.checks.some(k => k.x / 16 >= a - 4 && k.x / 16 <= b + 5);
  };
  const spotAt = a => {
    let floor = 17, pit = false; for (let c = a; c <= a + 5; c++) { floor = Math.min(floor, top(c)); pit = pit || top(c) === 17; }
    return ! pit && fits(a, a + 4, floor - 4) ? [a, a + 4, floor - 4] : null;
  };
  return { start, end, spotAt };
}
function flyerSpots(def, calls, n) {
  const { start, end, spotAt } = flyerRoom(def, calls), out = [];
  for (let i = 0; i < n; i++) {
    const lo = start + Math.floor((end - start) * i / n), hi = start + Math.floor((end - start) * (i + 1) / n) - 5, mid = Math.floor((lo + hi) / 2);
    let spot = null;
    for (let d = 0; ! spot && mid - d >= lo; d++) for (const a of d ? [mid + d, mid - d] : [mid]) {
      if (a < lo || a > hi) continue;
      if ((spot = spotAt(a))) break;
    }
    if (! spot) throw new Error(`${def.name}: no room for flying demon ${i + 1} between columns ${lo} and ${hi}`);
    out.push(spot);
  }
  return out;
}
// Hard's extra demons (user request, 2026-10-23: 5 more in each World 2 level): one at a time where there's room, as far
// as possible from every demon already placed (max-min distance between patrol middles), at least 6 columns from any
function hardFlyerSpots(def, calls, flyers, n) {
  const { start, end, spotAt } = flyerRoom(def, calls), out = [], mids = flyers.map(([a, b]) => (a + b) / 2);
  for (let i = 0; i < n; i++) {
    let best = null, bd = -1;
    for (let a = start; a <= end - 4; a++) {
      const spot = spotAt(a); if (! spot) continue;
      const d = Math.min(...mids.map(m => Math.abs(m - (a + 2))));
      if (d > bd) { bd = d; best = spot; }
    }
    if (! best || bd < 6) throw new Error(`${def.name}: no room for Hard's flying demon ${i + 1}`);
    out.push(best); mids.push(best[0] + 2);
  }
  return out.sort((x, y) => x[0] - y[0]);
}
// Fewer checkpoints (user requests, 2026-10-17): a third fewer in World 4 (9 -> 6 each), two fewer in each level of Worlds
// 1-3; by column. Those kept are the most evenly spaced from the start to the door (a boss level: to its arena, always
// keeping the checkpoint before it).
const DROP_CHECKS = {
  Jungle: [216, 392], 'Jungle 2': [58, 216], 'Jungle 3': [58, 216], 'Jungle 4': [60, 410],
  Graveyard: [44, 196], 'Graveyard 2': [43, 196], 'Graveyard 3': [66, 196], 'Graveyard 4': [42, 368],
  City: [55, 216], 'City 2': [54, 216], 'City 3': [63, 216], 'City 4': [108, 316],
  'Underworld 1': [47, 402, 550], 'Underworld 2': [46, 181, 392], 'Underworld 3': [40, 176, 398], Hell: [48, 178, 430],
};
// Coins per world (user request, 2026-10-17): Worlds 1-3 have 40% fewer, World 4 has 20% more, both the loose coins and
// the coins in each cracked block (5 in the web game; blockCoins: 3 and 6). By internal name prefix. World 1 then got
// 20% more in all (user request, 2026-10-18): blocks hold 4 (3 * 1.2 rounded up), so the loose coins only go 0.6 -> 0.66
// to keep the total at +20%. Then 10% fewer in World 1 (user request, 2026-10-19): blocks stay at 4, so the loose coins
// go 0.66 -> 0.54 to take the 10% off the total. An optional third entry sets the coins per block (else round(5 * scale)).
const COIN_SCALE = [['Jungle', 0.54, 4], ['Graveyard', 0.6], ['City', 0.6], ['Underworld', 1.2], ['Hell', 1.2]];
// Extra loose coins on top of the scaled count, by exact internal name (user request, 2026-10-20: World 1-1..1-4 +10 each).
// They're spread over the level's coin groups like the scaling (error diffusion), so each group grows a little.
const COIN_EXTRA = { Jungle: 10, 'Jungle 2': 10, 'Jungle 3': 10, 'Jungle 4': 10 };
// Rewrites a level's coin() and coins() calls as single coin() calls, with round(scale * n) (+ extra) coins. Coins are taken in
// groups (each coins() run, and coin() calls next to each other in a line), and each group's share is spread along the
// level by error diffusion, so the change is even. A thinned group keeps its ends and evenly spaced coins between; a
// grown group gets coins on its ends in line with it, in empty cells (out of a boss arena), alternately right then left.
function scaleCoins(def, calls, scale, extra = 0) {
  const groups = [], rest = [];
  for (const c of calls) {
    const [m, a] = c;
    if (m === 'coins') { const s = a[3] || 1, g = []; for (let x = a[0]; x <= a[1]; x += s) g.push([x, a[2]]); groups.push({ cells: g, dx: s, dy: 0 }); }
    else if (m === 'coin') {
      const [x, y] = a, g = groups.find(g => g.single && g.cells.some(([cx, cy]) => (cx === x || cy === y) && Math.abs(cx - x) <= 2 && Math.abs(cy - y) <= 2));
      if (g) g.cells.push([x, y]); else groups.push({ cells: [[x, y]], single: true });
    } else rest.push(c);
  }
  for (const g of groups) if (g.single) {
    g.cells.sort((p, q) => p[0] - q[0] || p[1] - q[1]);
    const [a, b] = [g.cells[0], g.cells[g.cells.length - 1]], n = g.cells.length;
    g.dx = n > 1 ? Math.round((b[0] - a[0]) / (n - 1)) : 1; g.dy = n > 1 ? Math.round((b[1] - a[1]) / (n - 1)) : 0;
  }
  const N = groups.reduce((n, g) => n + g.cells.length, 0);
  if (extra) scale = (Math.round(scale * N) + extra) / N;   // exactly extra more coins in all
  groups.sort((p, q) => p.cells[0][0] - q.cells[0][0]);
  const L = mod.exports.build({ ...def, build: b => { for (const [m, a] of rest) b[m](...a); } });
  const taken = new Set(groups.flatMap(g => g.cells.map(c => c + ''))), gate = L.boss ? L.boss.gate : def.w;
  const cell = (x, y) => taken.add(Math.floor(x / 16) + ',' + Math.floor(y / 16));   // embers, mushrooms, posts and the door keep their cells
  for (const e of L.embers) cell(e.x, e.y);
  for (const m of L.mush) cell(m.x, m.y);
  for (const c of L.checks) { cell(c.x, c.y); cell(c.x, c.y - 16); }
  if (L.door) { cell(L.door.x, L.door.y); cell(L.door.x, L.door.y + 16); }
  const free = ([x, y]) => x >= 0 && x < gate && y >= 0 && y < 17 && L.grid[y][x] === 0 && ! taken.has(x + ',' + y);
  // Grows a group's cells to k coins where there's room; returns how many it couldn't place
  const grow = (g, cells, k) => {
    for (let i = 0, misses = 0; cells.length < k && misses < 2; i++) {
      const end = i % 2 ? cells[0] : cells[cells.length - 1], sgn = i % 2 ? -1 : 1, c = [end[0] + sgn * g.dx, end[1] + sgn * g.dy];
      if (free(c)) { taken.add(c + ''); if (sgn > 0) cells.push(c); else cells.unshift(c); misses = 0; } else misses++;
    }
    return k - cells.length;
  };
  let acc = 0, owed = 0;
  const kept = groups.map(g => {
    const n = g.cells.length, k = Math.round(acc + scale * n) - Math.round(acc) + owed;
    acc += scale * n; owed = 0;
    if (k < n) return k ? Array.from({ length: k }, (_, i) => g.cells[Math.round((i + 0.5) * n / k - 0.5)]) : [];
    const cells = [...g.cells];
    owed = grow(g, cells, k);
    return cells;
  });
  // Coins the last groups had no room for go to the nearest earlier group that has
  for (let i = groups.length - 1; owed && i >= 0; i--) owed = grow(groups[i], kept[i], kept[i].length + owed);
  if (owed) throw new Error(`${def.name}: no room for ${owed} more coins`);
  return [...rest, ...kept.flat().map(c => ['coin', c])];
}

// The star block (user request, 2026-10-18): one more cracked block per level, in the middle third of the level, that
// holds a star (10 s of invincibility; see main.cpp) instead of coins. Picked here, as near the level's centre as there's
// a spot where it can't get in the way: on the ground or a plank with a tile of floor each side and two tiles of open air
// over all three (room to jump onto it or over it), no moving platform, checkpoint or door within two tiles, no enemy
// patrol, spikes or other block next to it, and no coin, ember or mushroom in its cell. The star pops up and lands back where the block
// was, so it's never lost down a pit. STAR_AT moves a level's target off the centre, as a fraction of its width.
const STAR_AT = { 'City 2': 0.57,   // World 3-2: ~7% later (user request, 2026-10-19)
  'Underworld 1': 0.7,               // World 4-1: 70% (user request, 2026-10-19)
  // A [col, row] is used as it is: World 3-4's star sits right after the moving platform at cols 239-245, on the landing
  // ledge's first tile (user request, 2026-10-20; it was at (223, 10), just before it). No spot there passes the rules
  // above: the ledge starts at a pit and a mech patrols the rest of it (250-257).
  'City 4': [249, 7] };
function starBlock(def, calls) {
  const L = mod.exports.build({ ...def, build: b => { for (const [m, a] of calls) b[m](...a); } }), g = L.grid, w = def.w;
  const at = (x, y) => x < 0 || x >= w || y < 0 ? -1 : y >= 17 ? 1 : g[y][x];
  // Cells to keep two tiles from; one tile from; cells the block can't take
  const near = new Set(), beside = new Set(), items = new Set();
  const mark = (set, px, py, pw = 1, ph = 1) => {
    for (let x = Math.floor(px / 16); x <= Math.floor((px + pw - 1) / 16); x++) for (let y = Math.floor(py / 16); y <= Math.floor((py + ph - 1) / 16); y++) set.add(x + ',' + y);
  };
  for (const e of L.enemies) mark(beside, e.a, e.y, e.b + e.w - e.a, e.h);   // the whole patrol
  for (const m of L.movers) mark(near, Math.min(m.x0, m.x1), Math.min(m.y0, m.y1) - 32, Math.abs(m.x1 - m.x0) + m.w, Math.abs(m.y1 - m.y0) + 48);
  for (const c of L.checks) mark(near, c.x, c.y - 16, 16, 32);
  if (L.door) mark(near, L.door.x, L.door.y, 16, 32);
  for (const c of L.coins) mark(items, c.x, c.y);
  for (const e of L.embers) mark(items, e.x, e.y);
  for (const m of L.mush) mark(items, m.x, m.y, 12, 12);
  // Why a spot won't do, or '' if it will
  const misfit = (c, r) => {
    for (let x = c - 1; x <= c + 1; x++) {
      if (at(x, r + 1) !== 1 && at(x, r + 1) !== 2) return 'floor';
      for (let y = r - 2; y <= r; y++) if (at(x, y) !== 0) return 'room';
    }
    if (items.has(c + ',' + r)) return 'items';
    for (let x = c - 2; x <= c + 2; x++) for (let y = r - 2; y <= r + 1; y++) if (near.has(x + ',' + y)) return 'near';
    for (let x = c - 1; x <= c + 1; x++) for (let y = r - 1; y <= r + 1; y++) {
      if (beside.has(x + ',' + y)) return 'enemies';
      if (at(x, y) === 3 || at(x, y) === 4) return 'spikes/blocks';
    }
    return '';
  };
  // (a STAR_AT target is searched within a sixth of the level either side of it instead of the middle third)
  if (Array.isArray(STAR_AT[def.name])) return STAR_AT[def.name];
  const at_ = STAR_AT[def.name], mid = Math.round((at_ ?? 0.5) * w), why = {};
  const lo = at_ == null ? Math.ceil(w / 3) : Math.max(0, mid - Math.floor(w / 6)), hi = at_ == null ? Math.floor(2 * w / 3) : Math.min(w - 1, mid + Math.floor(w / 6));
  for (let d = 0; mid - d >= lo || mid + d <= hi; d++)
    for (const c of d ? [mid + d, mid - d] : [mid]) {
      if (c < lo || c > hi) continue;
      for (let r = 1; r < 16; r++) { const m = misfit(c, r); if (! m) return [c, r]; if (m !== 'floor') why[m] = (why[m] || 0) + 1; }
    }
  throw new Error(`${def.name}: no room for the star block between columns ${lo} and ${hi} (${JSON.stringify(why)})`);
}

// The levels in game order. The GBA-only levels go straight after the web level they follow on from: World 1-2, 1-3 and
// 1-4 ("Jungle n") after the Jungle (World 1-1), World 2-2, 2-3 and 2-4 ("Graveyard n") after the Graveyard (World 2-1),
// then the levels that name it in `after`, in file order (World 3-1 to 3-4, "City" to "City 4", then World 4-1 to 4-3,
// "Underworld 1" to "Underworld 3", after the Graveyard's group)
const webDefs = mod.exports.LEVELS, gbaDefs = require('./gba-levels.js'), HALVES = require('./gba-halves.js');
const ordered = webDefs.flatMap(d => [d, ...gbaDefs.filter(g => g.name.startsWith(d.name + ' ')), ...gbaDefs.filter(g => g.after === d.name)]);
const LEVELS = ordered.map(d => {
  let calls = record(b => { d.build(b); if (GBA_EXTRAS[d.name]) GBA_EXTRAS[d.name](b); });
  let w = d.w;
  const half = process.env.SMHB_NO_HALVES ? null : HALVES[d.name];
  if (half) {
    // The second half goes in at column `at`: the end of a level with a door (its door becomes a checkpoint, "where the
    // door used to be", as the web game did when its levels grew), or the start of a boss level's run-up to its arena
    const boss = calls.some(([m]) => m === 'boss');
    if (boss) calls = openGap(calls, half.at, half.len);
    else {
      if (half.at !== d.w) throw new Error(`${d.name}: a level with a door gets its second half at its end (${d.w})`);
      // (unless the half says dropDoor: then it just goes)
      calls = half.dropDoor ? calls.filter(([m]) => m !== 'door') : calls.map(([m, a]) => m === 'door' ? ['check', a] : [m, a]);
    }
    calls.push(...record(half.build).map(c => moved(c, half.at)));
    w += half.len;
  }
  // Nothing to collect past the exit door (user request, 2026-10-17): coins, embers, cracked and 1-up blocks beyond its
  // column are dropped (it was a cracked block just right of the door in the levels that end in one)
  const exit = calls.find(([m]) => m === 'door' || m === 'boss'), doorCol = exit[0] === 'door' ? exit[1][0] : exit[1][1];
  calls = calls.filter(([m, a]) => ! (['coin', 'coins', 'ember', 'row', 'crack', 'oneup'].includes(m) && a[0] > doorCol));
  if (DROP_CHECKS[d.name]) calls = calls.filter(([m, a]) => ! (m === 'check' && DROP_CHECKS[d.name].includes(a[0])));
  const [, scale, perBlock] = COIN_SCALE.find(([pre]) => d.name.startsWith(pre));
  calls = scaleCoins({ ...d, w }, calls, scale, COIN_EXTRA[d.name] || 0);
  // Moving platforms 20% faster (user request, 2026-10-17), then 20% faster again (user, 2026-10-23): each cycle takes
  // 1/1.44 of the web game's frames
  calls = calls.map(([m, a]) => m === 'mover' ? [m, [...a.slice(0, 5), Math.round(a[5] / 1.44)]] : [m, a]);
  // The extra 1-up blocks go in after scaleCoins, so no coin moves
  calls.push(...(EXTRA_ONEUPS[d.name] || []).map(a => ['oneup', a]));
  calls.push(...(EXTRA_ENEMIES[d.name] || []).map(a => ['enemy', a]));
  // The star block goes in last, as a crack() to the web builder and the checkers (its coins are left out of the game's
  // coin total: main.cpp knows it by level_info::star_block)
  let star = starBlock({ ...d, w }, calls);
  const starCall = ['crack', star];
  calls.push(starCall);
  // (the flamethrower's spot is found with every star block in place, as it was before some were removed, so it stays put)
  const fl = FLAMETHROWERS[d.name], flame = Array.isArray(fl) ? fl : fl ? flameSpot({ ...d, w }, calls, fl) : null;
  // No star in the levels with a flamethrower, except World 4-4 (user request, 2026-10-18): its block goes too
  if (flame && d.name !== 'Hell') { calls.pop(); star = null; }
  const rk = ROCKETS[d.name], rocket = Array.isArray(rk) ? rk : rk ? flameSpot({ ...d, w }, calls, rk, true) : null;
  // No star in the levels with a rocket launcher either, except World 4-1 (user request, 2026-10-19). It goes after the
  // launcher's spot is found (with the block still there), so the launchers stay put.
  if (rocket && star && d.name !== 'Underworld 1') { calls = calls.filter(c => c !== starCall); star = null; }
  const drop = DROP_ENEMIES[d.name] || [];
  calls = calls.filter(([m, a]) => ! (m === 'enemy' && drop.some(x => x.join() === a.join())));
  calls.push(...(MORE_ENEMIES[d.name] || []).map(a => ['enemy', a]));
  const flyers = FLYERS[d.name] ? flyerSpots({ ...d, w }, calls, FLYERS[d.name]) : [];
  const hardBlue = HARD_BLUE.test(d.name) ? hardBlueMush({ ...d, w }, calls) : null;
  const build = b => { for (const [m, a] of calls) b[m](...a); };
  const hardFlyers = HARD_FLYERS[d.name] ? hardFlyerSpots({ ...d, w }, calls, flyers, HARD_FLYERS[d.name]) : [];
  return { ...d, w, calls, build, star, flame, rocket, flyers, hardSplits: HARD_SPLITS[d.name] ? hardSplits(calls, HARD_SPLITS[d.name]) : [], hardEnemies: HARD_ENEMIES[d.name] || [], hardDropCheck: hardDropCheck({ ...d, w }, calls),
    hardBlue, hardNoStar: HARD_NO_STAR.has(d.name), hardOnlyNoStar: HARD_ONLY_NO_STAR.has(d.name), expertNoRocket: EXPERT_NO_ROCKET.has(d.name), expertNoStar: EXPERT_NO_STAR.has(d.name), expertStar: EXPERT_STAR[d.name] || null, expertOneup: EXPERT_ONEUP[d.name] || null, hardDropMush: HARD_DROP_MUSH[d.name] ? hardDropMush({ ...d, w }, calls, HARD_DROP_MUSH[d.name], hardBlue ? [hardBlue[0]] : []) : [],
    expertDropMush: hardDropMush({ ...d, w }, calls, (HARD_DROP_MUSH[d.name] || 0) + (EXPERT_KEEP_MUSH.has(d.name) ? 0 : EXPERT_DROP_MUSH), hardBlue ? [hardBlue[0]] : []),
    expertSplits: EXPERT_SPLITS[d.name] ? hardSplits(hardPatrols(calls, HARD_SPLITS[d.name] ? hardSplits(calls, HARD_SPLITS[d.name]) : [], HARD_ENEMIES[d.name] || []).map(a => ['enemy', a]), EXPERT_SPLITS[d.name], EXPERT_SPLIT_MIN[d.name] || 4) : [],
    expertEnemies: EXPERT_ENEMIES[d.name] || [], expertDropEnemies: EXPERT_DROP_ENEMIES[d.name] || [],
    hardFlyers, expertFlyers: EXPERT_FLYERS[d.name] ? hardFlyerSpots({ ...d, w }, calls, [...flyers, ...hardFlyers], EXPERT_FLYERS[d.name]) : [], blockCoins: perBlock ?? Math.round(5 * scale), half: half ? { at: half.at, len: half.len } : null };
});

// One level as source for a stand-in LEVELS array (check-levels.js), with its build() spelled out call by call
function defSource(def, name = def.name, prelude = '') {
  const body = def.calls.map(([m, a]) => `b.${m}(${a.join(', ')});`).join(' ');
  return `{ name: ${JSON.stringify(name)}, w: ${def.w}, foe: ${JSON.stringify(def.foe)}, build: function (b) { ${prelude}${body} } }`;
}

// The embers two rows over spikes in World 4 and 2-1 (just the pair at cols 120-121 there; the others are three
// rows up) sit EMBER_LIFT px higher (user requests, 2026-10-31): at their old height a full jump over the spikes passed
// above them. From 20 px up, every full jump that clears the spikes takes both.
// Only the game's data changes: the placement and reachability checks still see them on their tile rows.
const EMBER_LIFT = 20, EMBER_LIFT_LEVELS = new Set(['Underworld 1', 'Underworld 2', 'Underworld 3', 'Hell', 'Graveyard']);
function build(def) {
  const L = mod.exports.build(def);
  if (EMBER_LIFT_LEVELS.has(def.name)) for (const e of L.embers) {
    const col = Math.floor(e.x / 16), row = Math.floor(e.y / 16);
    if (L.grid[row + 2] && L.grid[row + 2][col] === 3) e.y -= EMBER_LIFT;
  }
  return L;
}

module.exports = { LEVELS, build, buildSrc, defSource, WEB };
