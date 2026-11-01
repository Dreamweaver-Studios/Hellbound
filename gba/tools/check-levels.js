// Checks every GBA level as the game gets it (tools/levels.js: the web game's levels with the GBA's additions, the
// GBA-only levels, and since 2026-10-17 every level's second half):
//   1. placements and 2. coin reachability, with the web game's own checkers (levelcheck.js and coinreach.js in the
//      project root), run on a stand-in HTML file holding these levels and the web game's build();
//   3. that the exit door can be reached from the start: a search over standing spots and moving platforms, using the
//      game's player physics (the same model as coinreach.js), that never lands Mario on spikes.
// Run: node check-levels.js [level name ...]
const fs = require('fs'), path = require('path'), os = require('os'), { execFileSync } = require('child_process');
const levels = require('./levels.js'), WEB = levels.WEB, buildSrc = levels.buildSrc;
const only = process.argv.slice(2);
const defs = levels.LEVELS.filter(d => !only.length || only.includes(d.name));

// 1 and 2: the web game's checkers on a stand-in HTML
const src = `const LEVELS = [\n${defs.map(d => levels.defSource(d)).join(',\n')}\n];\n${buildSrc}`;
const tmp = path.join(os.tmpdir(), `smhb-gba-levels-${process.pid}.html`);
fs.writeFileSync(tmp, `<script>${src}</script>`);
// SMHB_PLACE_ONLY=1: just the placement check and each level's counts (quick, while designing)
if (process.env.SMHB_PLACE_ONLY) {
  process.stdout.write(execFileSync(process.execPath, [path.join(WEB, 'levelcheck.js'), tmp], { encoding: 'utf8' }));
  for (const d of defs) {
    const L = levels.build(d);
    console.log(`${d.name}: w ${L.w}, enemies ${L.enemies.length}, embers ${L.embers.length}, mush ${L.mush.length} (blue ${L.mush.filter(m => m.blue).length}), ` +
      `coins ${L.coins.length}, cracks ${L.cracks}, 1-ups ${L.oneupBlocks.size}, checks ${L.checks.length}, movers ${L.movers.length}`);
  }
  process.exit(0);
}
if (!process.env.SMHB_PATH_ONLY) {
  for (const tool of ['levelcheck.js', 'coinreach.js']) process.stdout.write(execFileSync(process.execPath, [path.join(WEB, tool), tmp], { encoding: 'utf8' }));
  // Embers too: coinreach.js again, on a copy where every ember is a coin (and the real coins are left out)
  const emberSrc = `const LEVELS = [\n${defs.map(d => levels.defSource(d, d.name + ' embers', 'b.coin = b.coins = () => {}; b.ember = (x, y) => { b.ember2(x, y); }; ')).join(',\n')}\n];\n${buildSrc.replace('ember(x, y) {', 'ember2(x, y) { L.coins.push({ x: x * T + 8, y: y * T + 8 }); },\n    ember(x, y) {')}`;
  const tmpE = path.join(os.tmpdir(), `smhb-gba-embers-${process.pid}.html`);
  fs.writeFileSync(tmpE, `<script>${emberSrc}</script>`);
  process.stdout.write(execFileSync(process.execPath, [path.join(WEB, 'coinreach.js'), tmpE], { encoding: 'utf8' }).replace(/visible coins/g, 'embers'));
}
// The flamethrower and rocket launcher pickups (GBA-only, 2026-10-18/19): coinreach.js again, on copies where the pickup
// is the only coin
const flames = [...defs.filter(d => d.flame).map(d => [d, 'flamethrower', d.flame]), ...defs.filter(d => d.rocket).map(d => [d, 'rocket launcher', d.rocket])];
if (flames.length && ! process.env.SMHB_PATH_ONLY) {
  const flameSrc = `const LEVELS = [\n${flames.map(([d, what, at]) => levels.defSource(d, d.name + ' ' + what, `const add = b.coin; b.coin = b.coins = () => {}; add(${at}); `)).join(',\n')}\n];\n${buildSrc}`;
  const tmpF = path.join(os.tmpdir(), `smhb-gba-flames-${process.pid}.html`);
  fs.writeFileSync(tmpF, `<script>${flameSrc}</script>`);
  process.stdout.write(execFileSync(process.execPath, [path.join(WEB, 'coinreach.js'), tmpF], { encoding: 'utf8' }).replace(/visible coins/g, 'pickup'));
}

// 3: start to door
// PH: the GBA hero's standing hitbox, 36 since 2026-10-29 (was 24, then 31; main.cpp's PH); SMHB_PH overrides it
const T = 16, ROWS = 17, PW = +(process.env.SMHB_PW || 26), PH = +(process.env.SMHB_PH || 40);
const mod = { exports: {} };
new Function('module', `const T = 16, ROWS = 17, PH = 24, COINS_PER_BLOCK = 5, BEAST_HP = 6;\n${src}\nmodule.exports = { LEVELS, build };`)(mod);
for (const def of mod.exports.LEVELS) {
  if (process.env.SMHB_ONLY && def.name !== process.env.SMHB_ONLY) continue;   // one level, by internal name
  const L = mod.exports.build(def);
  // blocksGone: every cracked block shot away (the second search below)
  let blocksGone = false;
  const tile = (tx, ty) => { if (tx < 0 || tx >= L.w) return 1; if (ty < 0 || ty >= ROWS) return 0; const t = L.grid[ty][tx]; return blocksGone && t === 4 ? 0 : t; };
  const solid = t => t === 1 || t === 4;
  // Cracked blocks are CRACK_H 27 px tall on the GBA (2026-10-29; main.cpp crack_rise): the bottom RISE px of the empty
  // tile above one count as the block. tileAt: a point's tile, as [tile, tx, ty]; top: a tile's top (a block's is higher)
  const RISE = 11;
  const cap = (tx, ty) => tile(tx, ty) === 0 && tile(tx, ty + 1) === 4;
  const capTop = ty => (ty + 1) * T - RISE;
  const tileAt = (x, y) => { const tx = Math.floor(x / T), ty = Math.floor(y / T); return cap(tx, ty) && y >= capTop(ty) ? [4, tx, ty + 1] : [tile(tx, ty), tx, ty]; };
  const top = (tx, ty) => ty * T - (tile(tx, ty) === 4 ? RISE : 0);
  const wall = (tx, ty, bottom) => solid(tile(tx, ty)) || (cap(tx, ty) && bottom > capTop(ty));
  const onSpikes = (x, y) => { for (let ty = Math.floor(y / T); ty <= Math.floor((y + PH - 0.01) / T); ty++) for (let tx = Math.floor(x / T); tx <= Math.floor((x + PW - 0.01) / T); tx++) if (tile(tx, ty) === 3) return true; return false; };
  // The hero mustn't begin inside a wall or spikes at the level start or a checkpoint (main.cpp respawn_x / respawn_y)
  const embedded = (x, y) => { for (let ty = Math.floor(y / T); ty <= Math.floor((y + PH - 0.01) / T); ty++) for (let tx = Math.floor(x / T); tx <= Math.floor((x + PW - 0.01) / T); tx++) if (wall(tx, ty, y + PH) || tile(tx, ty) === 3) return true; return false; };
  if (embedded(L.start.x, L.start.y + 24 - PH)) console.log(`${def.name}: start is inside a wall or spikes at PW ${PW}`);
  for (const c of L.checks) if ([c.x + Math.trunc((T - PW) / 2), c.x + T - PW, c.x].every(x => embedded(x, (c.row + 1) * T - PH)))
    console.log(`${def.name}: checkpoint ${c.x / T} has no clear respawn spot at PW ${PW}`);
  const phases = L.movers.map(mv => { const out = []; for (let t = 0; t < mv.period; t += 6) { const k = (1 - Math.cos(t / mv.period * Math.PI * 2)) / 2; out.push({ x: mv.x0 + (mv.x1 - mv.x0) * k, y: mv.y0 + (mv.y1 - mv.y0) * k, w: mv.w }); } return out; });

  // One trajectory from (x0, y0); `plats` are moving platforms frozen at one moment (Mario can wait for them).
  // Calls land(x, y) for a static landing, or landMover(i) for landing on plats[i].
  function sim(x0, y0, pol, plats, land, landMover) {
    let x = x0, y = y0, vx = pol.v0, vy = 0, ground = true, coyote = 0, jumpBuf = 0, jumped = false, air = false;
    for (let t = 0; t < 150; t++) {
      const dir = t < pol.m ? pol.d1 : pol.d2, held = pol.jb >= 0 && t >= pol.jb && t < pol.k;
      if (t === pol.jb) jumpBuf = 8;
      const acc = ground ? 0.38 : 0.26;
      if (dir) { vx += dir * acc; if (Math.abs(vx) > 2.3) vx = Math.sign(vx) * 2.3; } else { vx *= ground ? 0.7 : 0.9; if (Math.abs(vx) < 0.05) vx = 0; }
      const ox = x;
      coyote = ground ? 6 : Math.max(0, coyote - 1);
      if (jumpBuf > 0) jumpBuf--;
      if (jumpBuf > 0 && coyote > 0) { vy = -7.2; ground = false; coyote = 0; jumpBuf = 0; jumped = true; }
      if (!held && vy < -2.6) vy = -2.6;
      vy = Math.min(vy + 0.42, 8.5);
      x += vx;
      const mvx = x - ox, row0 = Math.floor(y / T), bot = Math.floor((y + PH - 0.01) / T);
      if (mvx > 0) { const tx = Math.floor((x + PW - 0.01) / T); for (let ty = row0; ty <= bot; ty++) if (wall(tx, ty, y + PH)) { x = tx * T - PW; vx = 0; break; } }
      else if (mvx < 0) { const tx = Math.floor(x / T); for (let ty = row0; ty <= bot; ty++) if (wall(tx, ty, y + PH)) { x = (tx + 1) * T; vx = 0; break; } }
      const pb = y + PH;
      y += vy; ground = false;
      const l = Math.floor(x / T), r = Math.floor((x + PW - 0.01) / T);
      if (vy > 0) {
        for (let i = 0; i < plats.length; i++) { const p = plats[i]; if (p && pb <= p.y + 0.01 && y + PH >= p.y && x + PW > p.x && x < p.x + p.w) { landMover(i); return; } }
        for (let c = l; c <= r; c++) { const [tt, tx, ty] = tileAt(c * T, y + PH); if (solid(tt) || (tt === 2 && pb <= ty * T + 0.01)) { y = top(tx, ty) - PH; vy = 0; ground = true; break; } }
      } else if (vy < 0) {
        const ty = Math.floor(y / T);
        for (let tx = l; tx <= r; tx++) if (solid(tile(tx, ty))) { y = (ty + 1) * T; vy = 0; break; }
      }
      if (!ground) air = true;
      if (y > ROWS * T + 10) return;
      if (ground && (air || t > 10)) { land(x, y); return; }
    }
  }
  const V0 = [-2.3, 0, 2.3], DIRS = [-1, 0, 1], MS = [0, 8, 16, 30], KS = [3, 8, 99], JBS = [0, 4];
  function launch(x, y, plats, land, landMover) {
    for (const v0 of V0) for (const d1 of DIRS) for (const d2 of DIRS) for (const mm of MS) {
      if (mm === 0 && d1 !== 0) continue;
      sim(x, y, { v0, d1, d2, m: mm, jb: -1, k: 0 }, plats, land, landMover);
      for (const jb of JBS) for (const k of KS) sim(x, y, { v0, d1, d2, m: mm, jb, k: jb + k }, plats, land, landMover);
    }
  }

  let seen, queue, moverSeen, moverQueue, far, doorReached, shot = false;
  const door = L.door || (L.boss && L.boss.door), doorRow = door ? (door.y + door.h) / T - 1 : -1;
  const visit = (x, y) => {
    // (spots are told apart to 4 px but searched from the exact x: rounding it let him drift into walls)
    y = Math.round(y);
    if (onSpikes(x, y)) return;
    const k = Math.round(x / 4) * 4 + ',' + y; if (seen.has(k)) return; seen.add(k); queue.push([x, y]);
    far = Math.max(far, x);
    if (door && Math.round((y + PH) / T) - 1 === doorRow && x + PW > door.x && x < door.x + door.w) doorReached = true;
  };
  const moverVisit = i => { if (!moverSeen.has(i)) { moverSeen.add(i); moverQueue.push(i); } };
  // Without shooting anything first; if that fails, with the blocks gone (some ways on go through one)
  for (const gone of [false, true]) {
  blocksGone = gone; shot = gone; seen = new Set(); queue = []; moverSeen = new Set(); moverQueue = []; far = 0; doorReached = false;
  // (the builder's start is for the web game's 24 px player: put this one's feet in the same place, as the GBA does)
  visit(L.start.x, L.start.y + 24 - PH);
  while ((queue.length || moverQueue.length) && !doorReached) {
    while (queue.length && !doorReached) {
      const [x, y] = queue.shift();
      if (process.env.SMHB_PATH_TRACE && seen.size % 200 === 0) console.log(`  ${def.name}: ${seen.size} spots, queue ${queue.length}, at x ${x} y ${y}`);
      // Walk both ways along the floor (to the next drop or wall), then jump from here, with each mover frozen in turn
      launch(x, y, [], visit, () => {});
      // (only the moments when the platform is within a jump of here: up to 5 tiles across and 4 up)
      phases.forEach((ph, i) => {
        if (moverSeen.has(i)) return;
        for (const p of ph) {
          if (p.x > x + 6 * T || p.x + p.w < x - 5 * T || p.y < y + PH - 4 * T || p.y > y + PH + 8 * T) continue;
          const plats = []; plats[i] = p; launch(x, y, plats, visit, moverVisit);
          if (moverSeen.has(i)) break;
        }
      });
    }
    while (moverQueue.length) {
      const i = moverQueue.shift();
      // Ride the mover anywhere along its path and jump (or step) off
      for (const p of phases[i]) for (let x = p.x - PW + 2; x < p.x + p.w - 2; x += 8) launch(x, p.y - PH, [], visit, () => {});
    }
  }
  if (doorReached) break;
  }
  blocksGone = false;
  const half = defs.find(d => d.name === def.name).half;
  console.log(`${def.name}: w ${L.w}${half ? ` (2nd half ${half.at}-${half.at + half.len - 1})` : ''}, enemies ${L.enemies.length}, embers ${L.embers.length}, ` +
    `mush ${L.mush.length}, coins ${L.coins.length}, cracks ${L.cracks}, checks ${L.checks.length}, movers ${L.movers.length}`);
  console.log(`${def.name}: start to door ${doorReached ? `REACHABLE${shot ? ' by shooting blocks' : ''}` : 'NOT reachable (furthest x ' + far + ' = col ' + Math.floor(far / T) + ')'}; ${seen.size} spots, movers ridden ${moverSeen.size}/${L.movers.length}`);
}
