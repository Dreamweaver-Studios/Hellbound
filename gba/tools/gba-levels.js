// GBA-only levels. First, between World 1-1 (the web game's Jungle) and World 2-1 (its Graveyard): World 1-2, 1-3 and 1-4, added
// at the user's request (2026-10-16). Same look, music and enemies as World 1-1 (the jungle tileset and backdrops, its
// music and aliens), and about its length (220 tiles) and difficulty. Written with the web game's own level builder
// (see build() in "Super Mario Hellbound.html"): columns and rows are tiles, ground(x0, x1, top) fills from `top` down,
// and a thing "on" ground whose top is row t sits on row t - 1. Mario's jump reaches about 3.8 tiles up and 4.5 across.
// Checked by tools/check-levels.js: placements, every coin reachable, and start-to-door reachable.
module.exports = [
  { name: 'Jungle 2', w: 220, foe: 'alien', build(b) {
    // World 1-2, "Tree Steps": up the terraces, down a plank stair, a log ride, a high step to the midway checkpoint;
    // then plank drops, a beetle floor, an elevator to a high ledge, a spike run and a last log ride to the door
    b.start(2, 13);
    b.oneup(0, 13);   // behind the start, as in Hell
    b.ground(0, 12, 14); b.crack(10, 13); b.row(4, 10, 12, 3);
    b.ground(13, 16, 12); b.crack(16, 11); b.ember(14, 10);
    b.ground(17, 20, 10); b.enemy(17, 20, 9);
    b.ember(22, 5);
    b.ground(24, 30, 10); b.enemy(25, 30, 9);
    b.ground(31, 40, 13); b.crack(31, 12); b.spikes(34, 35, 12); b.ember(34, 9); b.ember(35, 9); b.enemy(37, 40, 12);
    b.plat(43, 45, 11); b.plat(48, 50, 9); b.ember(49, 6); b.plat(53, 55, 11);
    b.ground(57, 70, 13); b.crack(57, 12); b.check(58, 12); b.mush(60, 12); b.enemy(62, 67, 12); b.enemy(64, 68, 12); b.row(62, 67, 9, 5); b.crack(69, 12);
    b.mover(72, 12, 3, 81, 12, 300); b.ember(77, 8);
    b.ground(85, 96, 12); b.enemy(86, 90, 11); b.ember(88, 9); b.spikes(92, 93, 11); b.ember(92, 8); b.ember(93, 8); b.crack(95, 11);
    b.ground(97, 114, 10); b.enemy(101, 108, 9); b.row(99, 111, 7, 6); b.check(112, 9); b.crack(114, 9);
    // Part 2
    b.plat(117, 119, 11); b.ember(118, 10); b.plat(122, 124, 12);
    b.ground(127, 142, 14); b.enemy(128, 132, 13); b.plat(130, 133, 11); b.enemy(130, 133, 10); b.spikes(134, 135, 13); b.row(133, 136, 10, 3); b.enemy(137, 141, 13); b.ember(140, 10); b.crack(142, 13);
    b.mover(144, 13, 3, 144, 8, 300); b.ember(147, 6);
    b.ground(148, 160, 9); b.mush(150, 8); b.enemy(151, 158, 8); b.ember(155, 5); b.crack(160, 8);
    b.ground(162, 184, 14); b.spikes(166, 167, 13); b.ember(166, 10); b.ember(167, 10);
    b.enemy(169, 175, 13); b.plat(170, 175, 11); b.enemy(171, 174, 10); b.row(171, 174, 9, 3); b.crack(177, 13);
    b.spikes(178, 179, 13); b.check(182, 13);
    b.mover(186, 12, 3, 195, 12, 280); b.ember(190, 9); b.ember(193, 9);
    b.ground(199, 219, 12); b.enemy(201, 207, 11); b.plat(204, 207, 9); b.enemy(204, 207, 8); b.crack(210, 11); b.enemy(211, 215, 11); b.ember(213, 9); b.crack(219, 11);
    b.door(216, 11);
    // Coins
    b.coins(3, 9, 11, 2); b.coins(13, 15, 11); b.coins(21, 23, 7); b.coins(24, 30, 8, 2);
    b.coins(41, 42, 9); b.coins(43, 45, 10); b.coins(48, 50, 8); b.coins(53, 55, 10); b.coins(59, 67, 11, 2);
    b.coins(73, 82, 10, 3); b.coins(86, 90, 10); b.coins(98, 110, 8, 2); b.coins(117, 119, 9); b.coins(122, 124, 11);
    b.coins(128, 132, 12); b.coins(137, 141, 12); b.coins(149, 158, 7, 3); b.coins(163, 165, 13); b.coins(180, 181, 13);
    b.coins(187, 197, 10, 2); b.coins(211, 214, 11);
  }},
  { name: 'Jungle 3', w: 220, foe: 'alien', build(b) {
    // World 1-3, "Log River": a log ride from the start, a plank chain over the river, a long log ride to a guarded
    // floor and the midway checkpoint; then two logs in a row, a spike floor, a high bank and a last log to the door
    b.start(2, 12);
    b.ground(0, 9, 13); b.crack(5, 12); b.crack(8, 12); b.ember(3, 9);
    b.mover(11, 11, 3, 20, 11, 320); b.row(14, 20, 8, 3);
    b.ground(24, 34, 12); b.crack(25, 11); b.enemy(27, 32, 11); b.enemy(29, 33, 11); b.ember(29, 8); b.crack(34, 11);
    b.plat(37, 39, 10); b.plat(42, 44, 8); b.ember(41, 6); b.plat(47, 49, 10); b.plat(52, 54, 12); b.ember(55, 9);
    b.ground(57, 72, 14); b.crack(57, 13); b.check(58, 13); b.mush(60, 13); b.enemy(61, 66, 13); b.plat(62, 65, 11); b.enemy(62, 65, 10);
    b.spikes(68, 69, 13); b.ember(68, 10); b.ember(69, 10); b.enemy(70, 72, 13);
    b.mover(74, 12, 3, 84, 12, 300); b.ember(78, 8); b.ember(82, 8);
    b.ground(88, 100, 12); b.enemy(90, 96, 11); b.plat(92, 95, 9); b.enemy(92, 95, 8); b.ember(94, 6); b.crack(99, 11);
    b.ground(101, 110, 10); b.enemy(102, 106, 9); b.ember(105, 6); b.check(109, 9);
    // Part 2
    b.mover(111, 9, 3, 118, 9, 260); b.ember(115, 6); b.ember(119, 6);
    b.plat(122, 124, 10); b.ember(125, 7);
    b.mover(126, 10, 3, 134, 10, 260); b.ember(130, 7); b.ember(133, 7);
    b.ground(138, 160, 13); b.enemy(140, 146, 12); b.plat(142, 146, 10); b.enemy(142, 146, 9); b.crack(147, 12);
    b.spikes(148, 149, 12); b.ember(148, 9); b.ember(149, 9); b.enemy(152, 158, 12); b.ember(155, 9); b.crack(160, 12);
    b.ground(161, 170, 10); b.mush(163, 9); b.enemy(164, 169, 9); b.ember(166, 6); b.crack(170, 9);
    b.ground(172, 186, 14); b.check(173, 13); b.spikes(176, 177, 13); b.ember(176, 10); b.ember(177, 10);
    b.enemy(179, 184, 13); b.plat(180, 184, 11); b.enemy(181, 184, 10); b.ember(182, 8); b.crack(186, 13);
    b.mover(188, 12, 3, 196, 12, 280); b.ember(192, 8); b.ember(196, 8);
    b.ground(200, 219, 12); b.enemy(202, 208, 11); b.ember(205, 8); b.spikes(210, 211, 11); b.ember(210, 8); b.ember(211, 8);
    b.crack(212, 11); b.enemy(213, 215, 11); b.oneup(218, 11); b.crack(219, 11);   // the 1-up: just past the door
    b.door(216, 11);
    // Coins
    b.coins(3, 7, 11, 2); b.coins(12, 22, 9, 2); b.coins(26, 32, 10, 2);
    b.coins(37, 39, 9); b.coins(42, 44, 7); b.coins(47, 49, 9); b.coins(52, 54, 11);
    b.coins(60, 66, 12, 2); b.coins(75, 86, 10, 3); b.coins(89, 97, 10, 2); b.coins(92, 95, 7); b.coins(102, 108, 8, 2);
    b.coins(112, 120, 7, 2); b.coins(122, 124, 9); b.coins(127, 135, 8, 2);
    b.coins(140, 146, 11, 2); b.coins(152, 158, 11, 2); b.coins(162, 169, 8, 2); b.coins(179, 184, 12);
    b.coins(189, 197, 10, 2); b.coins(203, 207, 10, 2);
  }},
  { name: 'Jungle 4', w: 254, foe: 'alien', build(b) {
    // World 1-4, "Canopy Climb": an elevator to a high ledge, a plank stair down, a guarded floor, two steps up to the
    // midway checkpoint and a log down to a second floor; then a plank tower, an elevator to the canopy, a long drop
    // and a last log to the door
    // The door became a boss arena (the user's request, 2026-10-16), like World 4-4's: a drop to a checkpoint, then a
    // one-screen arena (cols 224-253) sealed by a stone gate at col 224, where the flying alien boss waits; its door
    // only appears once the boss is beaten
    b.start(2, 13);
    b.ground(0, 14, 14); b.crack(6, 13); b.crack(12, 13); b.row(4, 10, 11, 3); b.ember(14, 10);
    b.mover(16, 13, 3, 16, 8, 300); b.ember(17, 5);
    b.ground(20, 30, 8); b.enemy(23, 29, 7); b.crack(30, 7); b.ember(26, 4);
    b.plat(33, 35, 10); b.plat(38, 40, 12); b.ember(36, 8);
    b.ground(43, 58, 14); b.crack(44, 13); b.enemy(45, 50, 13); b.plat(45, 49, 11); b.enemy(46, 49, 10); b.ember(47, 8);
    b.spikes(52, 53, 13); b.ember(52, 10); b.ember(53, 10); b.enemy(55, 58, 13);
    b.ground(59, 66, 11); b.check(60, 10); b.mush(62, 10); b.crack(66, 10); b.ember(64, 7);
    b.ground(67, 72, 8); b.enemy(68, 71, 7); b.crack(72, 7); b.ember(70, 4);
    b.mover(74, 10, 3, 80, 10, 280); b.ember(77, 7);
    b.ground(84, 98, 12); b.enemy(86, 92, 11); b.plat(88, 92, 9); b.enemy(89, 92, 8); b.spikes(94, 95, 11); b.ember(94, 8); b.ember(95, 8);
    b.enemy(96, 98, 11);
    b.ground(99, 110, 9); b.enemy(100, 104, 8); b.ember(105, 5); b.check(108, 8); b.crack(110, 8);
    // Part 2
    b.plat(113, 115, 7); b.ember(114, 5); b.plat(118, 120, 5); b.plat(123, 125, 7); b.ember(124, 5);
    b.ground(128, 140, 10); b.enemy(130, 138, 9); b.oneup(139, 9); b.ember(134, 6); b.crack(140, 9);
    // (the 1-up was at (120, 4) on the plank until the user moved it onto this grass, 2026-10-20; 128-138 are alien patrols)
    b.ground(142, 156, 14); b.spikes(146, 147, 13); b.ember(146, 10); b.ember(147, 10);
    b.enemy(149, 155, 13); b.plat(150, 154, 11); b.enemy(151, 154, 10); b.ember(152, 8);
    b.mover(158, 13, 3, 158, 7, 300); b.ember(159, 4);
    b.ground(162, 176, 7); b.mush(163, 6); b.enemy(164, 170, 6); b.enemy(172, 175, 6); b.crack(176, 6); b.ember(168, 3);
    b.ground(178, 188, 13); b.check(179, 12); b.spikes(183, 184, 12); b.ember(183, 9); b.ember(184, 9); b.enemy(185, 188, 12);
    b.mover(190, 11, 3, 198, 11, 280); b.ember(194, 8); b.ember(200, 8);
    b.ground(202, 219, 11); b.enemy(204, 210, 10); b.crack(211, 10); b.spikes(212, 213, 10); b.ember(212, 7); b.ember(213, 7);
    b.crack(215, 10);
    // Boss arena: a flat floor (it had World 4-4's three planks until the user had them removed, 2026-10-18)
    b.ground(220, 253, 14); b.mush(221, 13); b.check(222, 13);
    b.boss(224, 251, 13);
    // Coins
    b.coins(3, 9, 12, 2); b.coins(16, 18, 10); b.coins(21, 29, 6, 2); b.coins(31, 32, 7); b.coins(33, 35, 9); b.coins(38, 40, 11);
    b.coins(46, 49, 12); b.coins(56, 58, 12); b.coins(60, 65, 9); b.coins(67, 71, 6, 2); b.coins(75, 81, 9, 2);
    b.coins(86, 92, 10, 2); b.coins(100, 106, 7, 2); b.coins(111, 112, 6); b.coins(113, 115, 6); b.coins(118, 120, 3, 2);
    b.coins(123, 125, 6); b.coins(129, 138, 8, 3); b.coins(149, 155, 12, 2); b.coins(163, 175, 5, 3);
    b.coins(180, 182, 12); b.coins(191, 199, 9, 2); b.coins(204, 210, 9, 2);
  }},

  // World 2-2, 2-3 and 2-4, between World 2-1 (the Graveyard) and World 4-4 (Hell), added at the user's request
  // (2026-10-16): the Graveyard's look, music and reapers, its length (200 tiles) and its difficulty: 16 reapers (13 +
  // the GBA's 3 extras), 30 embers, about 80 coins, 12 cracked blocks, a hidden 1-up, 4 mushrooms, 3 checkpoints and,
  // like it, no moving platforms. Reapers stay off the steps right after checkpoints, as in the Graveyard.
  { name: 'Graveyard 2', w: 200, foe: 'death', build(b) {
    // World 2-2, "Grave Steps": up two raised plots, a guarded lawn under a beam, a spike pit to the first checkpoint,
    // beam hops and a long lawn; then steps up to a high plot, a drop to a guarded lawn, a beam chain and the last lawn
    b.start(2, 13);
    b.ground(0, 11, 14); b.crack(9, 13); b.row(4, 10, 12, 3);
    b.ground(12, 16, 12); b.enemy(13, 15, 11); b.crack(16, 11);
    b.ground(17, 21, 10); b.enemy(18, 21, 9); b.ember(19, 7);
    b.ground(24, 33, 12); b.enemy(25, 28, 11); b.enemy(30, 33, 11); b.plat(27, 30, 9); b.ember(28, 7); b.ember(29, 7);
    b.ground(34, 44, 14); b.crack(35, 13); b.spikes(37, 38, 13); b.ember(37, 10); b.ember(38, 10); b.check(43, 13); b.crack(44, 13);
    b.ground(45, 48, 11); b.mush(46, 10);
    b.plat(52, 54, 9); b.ember(53, 7); b.plat(57, 59, 11); b.enemy(57, 59, 10); b.plat(62, 64, 9); b.ember(63, 7); b.plat(67, 69, 11);
    b.ground(71, 98, 14); b.enemy(72, 80, 13); b.mush(78, 13); b.crack(81, 13); b.spikes(83, 84, 13); b.ember(83, 10); b.ember(84, 10);
    b.plat(86, 89, 11); b.enemy(86, 89, 10); b.enemy(90, 95, 13); b.check(97, 13); b.crack(98, 13);
    // Part 2
    b.ground(100, 104, 12); b.ember(102, 9); b.crack(103, 11);
    b.ground(105, 108, 9); b.ember(106, 6);
    b.plat(112, 114, 10); b.ember(113, 8);
    b.ground(117, 130, 13); b.enemy(118, 124, 12); b.plat(120, 123, 10); b.enemy(120, 123, 9); b.spikes(126, 127, 12); b.ember(126, 9); b.ember(127, 9); b.crack(129, 12);
    b.ground(131, 134, 10); b.mush(132, 9); b.ember(133, 7);
    b.ground(135, 138, 7); b.ember(136, 4); b.oneup(137, 6); b.crack(138, 6);
    b.ground(141, 152, 12); b.enemy(142, 146, 11); b.enemy(147, 150, 11); b.row(144, 150, 8, 3); b.check(151, 11); b.crack(152, 11);
    b.plat(156, 158, 10); b.ember(157, 8); b.plat(161, 163, 8); b.enemy(161, 163, 7); b.ember(162, 6); b.plat(166, 168, 10); b.ember(167, 8);
    b.ground(171, 199, 14); b.enemy(173, 180, 13); b.ember(176, 10); b.mush(176, 13); b.crack(181, 13); b.spikes(183, 184, 13); b.ember(183, 10); b.ember(184, 10);
    b.plat(186, 189, 11); b.enemy(186, 189, 10); b.row(186, 189, 9, 3); b.enemy(191, 194, 13); b.crack(199, 13);
    b.door(196, 13);
    // Coins
    b.coins(3, 11, 11, 2); b.coins(13, 16, 10); b.coins(18, 21, 8); b.coin(22, 7); b.coin(23, 7); b.coins(24, 33, 10, 3);
    b.coins(39, 42, 12); b.coins(45, 48, 9); b.coins(52, 54, 8); b.coins(57, 59, 10); b.coins(62, 64, 8); b.coins(67, 69, 10);
    b.coins(72, 80, 12, 2); b.coins(91, 95, 12, 2);
    b.coins(100, 103, 10); b.coins(105, 108, 7); b.coins(112, 114, 9); b.coins(118, 124, 11, 2); b.coins(131, 134, 8);
    b.coins(141, 150, 10, 3); b.coins(156, 158, 9); b.coins(166, 168, 9); b.coins(174, 180, 12, 2); b.coins(191, 195, 12, 2);
  }},
  { name: 'Graveyard 3', w: 200, foe: 'death', build(b) {
    // World 2-3, "Bone Bridges": mostly beam chains over open graves, with a guarded lawn between each chain
    b.start(2, 13);
    b.ground(0, 9, 14); b.crack(7, 13); b.row(3, 9, 12, 3);
    b.plat(12, 14, 12); b.plat(17, 19, 10); b.enemy(17, 19, 9); b.ember(18, 8); b.plat(22, 24, 12);
    b.ground(27, 38, 13); b.crack(27, 12); b.enemy(28, 34, 12); b.plat(30, 33, 10); b.enemy(30, 33, 9); b.crack(36, 12); b.ember(37, 9);
    b.ground(39, 42, 10); b.ember(41, 7);
    b.plat(45, 47, 9); b.ember(47, 6); b.plat(50, 52, 11); b.ember(51, 9); b.plat(55, 57, 9); b.enemy(55, 57, 8); b.plat(60, 62, 11); b.ember(61, 9);
    b.ground(65, 72, 13); b.check(66, 12); b.mush(68, 12); b.row(66, 71, 10, 5); b.crack(72, 12);
    b.plat(76, 78, 11); b.ember(77, 9); b.plat(81, 83, 9); b.enemy(81, 83, 8); b.ember(82, 7); b.plat(86, 88, 11); b.enemy(86, 88, 10); b.ember(87, 9);
    b.ground(91, 99, 14); b.crack(91, 13); b.enemy(92, 97, 13); b.ember(95, 11); b.check(98, 13); b.crack(99, 13);
    // Part 2
    b.ground(101, 105, 12); b.ember(103, 9); b.mush(103, 11); b.crack(105, 11);
    b.plat(108, 110, 10); b.ember(109, 8); b.plat(113, 115, 12); b.enemy(113, 115, 11); b.plat(118, 120, 10); b.ember(119, 8);
    b.plat(123, 125, 8); b.ember(124, 6);
    b.ground(128, 140, 11); b.oneup(128, 10); b.enemy(129, 134, 10); b.ember(131, 8); b.crack(135, 10); b.spikes(136, 137, 10); b.ember(136, 7); b.ember(137, 7); b.enemy(138, 140, 10);
    b.ground(141, 146, 14); b.enemy(141, 145, 13); b.ember(144, 11); b.mush(143, 13); b.crack(146, 13);
    b.plat(149, 151, 12); b.enemy(149, 151, 11); b.ember(152, 9); b.plat(154, 156, 10); b.enemy(154, 156, 9); b.plat(159, 161, 12); b.ember(160, 10);
    b.ground(164, 170, 13); b.check(165, 12); b.mush(167, 12); b.crack(170, 12);
    b.plat(173, 175, 11); b.ember(174, 9); b.plat(178, 180, 9); b.enemy(178, 180, 8); b.ember(179, 7); b.plat(183, 185, 11); b.ember(184, 9);
    b.ground(188, 199, 13); b.crack(188, 12); b.enemy(189, 191, 12); b.ember(191, 9); b.enemy(192, 195, 12); b.ember(194, 9); b.crack(199, 12);
    b.door(196, 12);
    // Coins
    b.coins(3, 9, 11, 2); b.coins(12, 14, 11); b.coins(17, 19, 9); b.coins(22, 24, 11); b.coins(28, 30, 11); b.coins(35, 38, 11);
    b.coins(39, 42, 8); b.coins(45, 47, 8); b.coins(50, 52, 10); b.coins(55, 57, 8); b.coins(60, 62, 10); b.coins(67, 71, 11, 2);
    b.coins(76, 78, 10); b.coins(81, 83, 8); b.coins(86, 88, 10); b.coins(92, 97, 12, 2);
    b.coins(101, 105, 10); b.coins(108, 110, 9); b.coins(118, 120, 9); b.coins(123, 124, 7); b.coins(130, 134, 9, 2);
    b.coins(141, 145, 12, 2); b.coins(159, 161, 11); b.coins(173, 175, 10); b.coins(188, 192, 11, 2);
  }},
  { name: 'Graveyard 4', w: 234, foe: 'death', build(b) {
    // World 2-4, "Mausoleum Rise": up two plots to a high ledge, a drop to a guarded lawn and a beam stair to the
    // highest ledge; then beam hops, a climb to a hidden 1-up, a drop to a guarded lawn, a beam stair and the last lawn
    // The door became a boss arena (the user's request, 2026-10-16), like World 1-4's: a step down to a checkpoint, then a
    // one-screen arena (cols 204-233) sealed by a stone gate at col 204, where the Eye Demon waits; its door only appears
    // once the boss is beaten
    b.start(2, 13);
    b.ground(0, 10, 14); b.crack(8, 13); b.row(4, 10, 11, 3);
    b.ground(11, 15, 11); b.enemy(12, 15, 10);
    b.ground(16, 22, 8); b.enemy(17, 21, 7); b.ember(19, 4); b.crack(22, 7);
    b.ground(25, 40, 14); b.crack(25, 13); b.enemy(27, 33, 13); b.plat(29, 32, 11); b.enemy(29, 32, 10);
    b.spikes(35, 36, 13); b.ember(35, 10); b.ember(36, 10); b.enemy(37, 40, 13);
    b.ground(41, 46, 11); b.check(42, 10); b.mush(44, 10); b.crack(46, 10);
    b.plat(49, 51, 9); b.ember(50, 7); b.plat(54, 56, 7); b.ember(55, 5);
    b.ground(59, 66, 6); b.enemy(60, 66, 5); b.ember(63, 2);
    b.ground(69, 84, 14); b.enemy(70, 76, 13); b.mush(72, 13); b.plat(73, 76, 11); b.enemy(73, 76, 10); b.spikes(79, 80, 13); b.ember(79, 10); b.ember(80, 10); b.crack(83, 13);
    b.ground(85, 98, 12); b.enemy(87, 94, 11); b.row(87, 93, 8, 3); b.check(97, 11); b.crack(98, 11);
    // Part 2
    b.plat(101, 103, 10); b.ember(102, 8); b.plat(106, 108, 8); b.enemy(106, 108, 7); b.plat(111, 113, 10); b.ember(112, 8);
    b.ground(116, 124, 12); b.crack(116, 11); b.enemy(117, 123, 11); b.ember(120, 8);
    b.ground(125, 129, 9); b.mush(126, 8); b.ember(128, 6);
    b.ground(130, 134, 6); b.ember(132, 3); b.oneup(133, 5); b.crack(134, 5);
    b.ground(137, 152, 13); b.enemy(139, 145, 12); b.plat(141, 145, 10); b.enemy(142, 145, 9); b.spikes(147, 148, 12); b.ember(147, 9); b.ember(148, 9);
    b.check(151, 12); b.crack(152, 12);
    b.ground(153, 158, 10); b.ember(156, 7); b.crack(158, 9);
    b.plat(161, 163, 8); b.enemy(161, 163, 7); b.ember(165, 5); b.plat(167, 169, 6); b.ember(168, 4);
    b.ground(172, 182, 10); b.enemy(174, 181, 9); b.mush(178, 9); b.row(175, 181, 6, 3);
    b.ground(183, 199, 13); b.crack(183, 12); b.enemy(185, 190, 12); b.spikes(192, 193, 12); b.ember(192, 9); b.ember(193, 9); b.ember(187, 9);
    // Boss arena: the same planks as World 1-4's and 4-4's
    b.ground(200, 233, 14); b.mush(201, 13); b.check(202, 13);
    b.plat(208, 211, 11); b.plat(213, 224, 8); b.plat(226, 229, 11);
    b.boss(204, 231, 13);
    // Coins
    b.coins(3, 9, 12, 2); b.coins(11, 15, 9); b.coins(17, 21, 6, 2); b.coins(25, 28, 12);
    b.coins(49, 51, 8); b.coins(54, 56, 6); b.coins(59, 65, 4, 2); b.coins(70, 76, 12, 2); b.coins(85, 95, 10, 2);
    b.coins(101, 103, 9); b.coins(106, 108, 7); b.coins(111, 113, 9); b.coins(117, 123, 10, 2); b.coins(125, 128, 7); b.coins(130, 133, 4);
    b.coins(137, 140, 11); b.coins(153, 157, 8); b.coins(161, 163, 6); b.coins(167, 169, 5); b.coins(172, 182, 8, 2); b.coins(184, 190, 11, 2);
  }},

  // World 3-1, between World 2-4 and World 4-4, added at the user's request (2026-10-16): Synth City's skyline as the
  // backdrop and the Warped "mech-unit" as the enemy, with World 1-1's size and difficulty: 220 tiles, 20 enemies, 5 moving
  // platforms, 30 embers, about 80 coins, 12 cracked blocks, a hidden 1-up, 4 mushrooms and 3 checkpoints. The mechs
  // behave exactly like World 1-1's aliens (same hitbox, speed, health and shots), so the web builder and the checkers see
  // aliens (foe: 'alien'); `gbaFoe` makes the GBA draw them as mechs. `after` puts the level after World 2-1's group.
  { name: 'City', w: 220, foe: 'alien', gbaFoe: 'mech', after: 'Graveyard', build(b) {
    // "Neon Rooftops": a guarded roof, a sliding platform, a guarded roof under a beam, a spike pit to the first
    // checkpoint, an elevator to a high roof, a long street and a step up to the midway checkpoint; then beam hops round a
    // sliding platform, a spike run, an elevator to a high roof, a long street to the last checkpoint and a last slide
    b.start(2, 13);
    b.ground(0, 11, 14); b.crack(9, 13); b.row(4, 10, 12, 3);
    b.ground(12, 18, 12); b.enemy(13, 17, 11); b.ember(15, 9); b.crack(18, 11);
    b.mover(20, 12, 3, 26, 12, 320); b.ember(24, 9);
    b.ground(31, 42, 11); b.enemy(32, 38, 10); b.plat(34, 37, 8); b.ember(35, 6); b.oneup(37, 7); b.enemy(39, 41, 10); b.crack(42, 10);
    b.ground(43, 60, 14); b.spikes(45, 46, 13); b.ember(45, 10); b.ember(46, 10); b.enemy(48, 53, 13); b.check(55, 13); b.mush(57, 13); b.crack(60, 13);
    b.mover(62, 13, 3, 62, 8, 300); b.ember(63, 5);
    b.ground(66, 76, 8); b.enemy(67, 71, 7); b.enemy(72, 75, 7); b.row(68, 74, 5, 3); b.crack(76, 7);
    b.ground(79, 96, 14); b.enemy(80, 85, 13); b.spikes(87, 88, 13); b.ember(87, 10); b.ember(88, 10);
    b.enemy(90, 95, 13); b.plat(90, 94, 11); b.enemy(91, 94, 10); b.ember(92, 8); b.crack(96, 13);
    b.ground(97, 110, 12); b.enemy(99, 104, 11); b.ember(102, 8); b.mush(106, 11); b.check(108, 11); b.crack(110, 11);
    // Part 2
    b.plat(113, 115, 10); b.ember(114, 8); b.mover(117, 9, 3, 121, 9, 260); b.plat(123, 125, 10);
    b.ground(128, 142, 14); b.crack(128, 13); b.enemy(129, 134, 13); b.spikes(136, 137, 13); b.ember(136, 10); b.ember(137, 10); b.enemy(139, 141, 13); b.crack(142, 13);
    b.mover(144, 13, 3, 144, 7, 300); b.ember(145, 4);
    b.ground(148, 160, 7); b.enemy(149, 155, 6); b.enemy(156, 159, 6); b.row(150, 158, 3, 4); b.mush(152, 6); b.crack(160, 6);
    b.ground(163, 184, 14); b.enemy(165, 170, 13); b.spikes(172, 173, 13); b.ember(172, 10); b.ember(173, 10);
    b.enemy(175, 180, 13); b.plat(175, 179, 11); b.enemy(176, 179, 10); b.check(183, 13); b.crack(184, 13);
    b.mover(186, 12, 3, 196, 12, 300); b.ember(190, 9); b.ember(194, 9);
    b.ground(200, 219, 12); b.mush(201, 11); b.enemy(202, 207, 11); b.plat(204, 207, 9); b.enemy(204, 207, 8); b.ember(205, 6);
    b.spikes(209, 210, 11); b.ember(209, 8); b.ember(210, 8); b.enemy(211, 214, 11); b.crack(219, 11);
    b.door(216, 11);
    // Coins
    b.coins(3, 11, 11, 2); b.coins(13, 17, 10); b.coins(20, 28, 10, 2); b.coins(31, 33, 9); b.coins(34, 36, 7);
    b.coins(48, 53, 12); b.coins(62, 64, 6); b.coins(67, 75, 6, 2); b.coins(80, 85, 12); b.coins(98, 104, 10, 2);
    b.coins(113, 115, 9); b.coins(117, 121, 8, 2); b.coins(123, 125, 9); b.coins(129, 134, 12); b.coins(145, 146, 5);
    b.coins(149, 159, 5, 2); b.coins(165, 170, 12); b.coins(187, 196, 10, 3); b.coins(212, 214, 10);
  }},

  // World 3-2, 3-3 and 3-4, between World 3-1 and World 4-4, added at the user's request (2026-10-16): World 3-1's look,
  // music and mechs, and its length and difficulty (each 220 tiles, 20 mechs, 5 moving platforms, 30 embers, about 80
  // coins, 12 cracked blocks, a hidden 1-up, 4 mushrooms and 3 checkpoints). They follow World 3-1 in this file's order.
  { name: 'City 2', w: 220, foe: 'alien', gbaFoe: 'mech', after: 'Graveyard', build(b) {
    // World 3-2, "Neon Alley": an elevator to a guarded high roof, a drop to a spike street and a step up to the first
    // checkpoint, girder hops round a sliding platform, a guarded street and a roof to the midway checkpoint; then a
    // slide to a guarded street, a spike street, an elevator to a high roof with the 1-up, and a last slide to the door
    b.start(2, 13);
    b.ground(0, 10, 14); b.crack(8, 13); b.row(4, 10, 12, 3);
    b.ground(11, 17, 11); b.enemy(12, 16, 10); b.ember(14, 8); b.crack(17, 10);
    b.mover(19, 11, 3, 19, 6, 300); b.ember(20, 4);
    b.ground(23, 34, 6); b.enemy(24, 29, 5); b.enemy(30, 33, 5); b.row(25, 33, 3, 4); b.crack(34, 5);
    b.ground(37, 52, 14); b.enemy(38, 43, 13); b.spikes(45, 46, 13); b.ember(45, 10); b.ember(46, 10); b.enemy(48, 51, 13); b.crack(52, 13);
    b.ground(53, 60, 11); b.check(54, 10); b.mush(56, 10); b.crack(60, 10);
    b.plat(63, 65, 9); b.ember(64, 7); b.mover(68, 9, 3, 74, 9, 300); b.ember(71, 6); b.plat(78, 80, 9); b.enemy(78, 80, 8);
    b.ground(83, 98, 13); b.enemy(84, 89, 12); b.plat(86, 89, 10); b.enemy(86, 89, 9); b.spikes(91, 92, 12); b.ember(91, 9); b.ember(92, 9);
    b.enemy(94, 97, 12); b.crack(98, 12);
    b.ground(99, 110, 10); b.enemy(100, 104, 9); b.ember(102, 6); b.check(108, 9); b.crack(110, 9);
    // Part 2
    b.mover(112, 10, 3, 118, 10, 280); b.ember(115, 7);
    b.ground(122, 135, 12); b.crack(122, 11); b.enemy(123, 128, 11); b.plat(125, 128, 9); b.enemy(125, 128, 8); b.ember(131, 8); b.enemy(130, 134, 11); b.crack(135, 11);
    b.ground(136, 139, 9); b.mush(137, 8); b.ember(138, 6);
    b.ground(142, 156, 14); b.crack(143, 13); b.spikes(145, 146, 13); b.ember(145, 10); b.ember(146, 10); b.enemy(148, 155, 13);
    b.mover(158, 13, 3, 158, 7, 300); b.ember(159, 4);
    b.ground(162, 175, 7); b.enemy(163, 168, 6); b.mush(166, 6); b.enemy(170, 174, 6); b.row(164, 174, 3, 5); b.oneup(175, 6);
    b.ground(178, 190, 13); b.check(179, 12); b.enemy(182, 189, 12); b.ember(185, 9); b.crack(190, 12);
    b.mover(192, 12, 3, 198, 12, 280); b.ember(195, 9);
    b.ground(202, 219, 12); b.mush(202, 11); b.enemy(203, 207, 11); b.plat(205, 208, 9); b.enemy(205, 208, 8); b.ember(206, 6);
    b.spikes(210, 211, 11); b.ember(210, 8); b.ember(211, 8); b.enemy(212, 214, 11); b.ember(213, 8); b.crack(219, 11);
    b.door(216, 11);
    // Coins
    b.coins(3, 9, 11, 2); b.coins(11, 16, 9); b.coins(23, 33, 4, 2); b.coins(37, 43, 12, 2); b.coins(55, 59, 9);
    b.coins(63, 65, 8); b.coins(68, 76, 7, 2); b.coins(78, 80, 7); b.coins(84, 89, 11); b.coins(100, 106, 8, 2);
    b.coins(112, 120, 8, 2); b.coins(130, 134, 10); b.coins(136, 139, 7); b.coins(148, 154, 12, 2); b.coins(163, 173, 5, 2);
    b.coins(180, 189, 11, 3); b.coins(192, 200, 10, 2); b.coins(203, 207, 10);
  }},
  { name: 'City 3', w: 220, foe: 'alien', gbaFoe: 'mech', after: 'Graveyard', build(b) {
    // World 3-3, "Skybridge": girder hops and a slide to a guarded roof, girder hops to the first checkpoint, an elevator
    // to a high roof and a spike street to the midway checkpoint; then girder hops round two slides, a guarded roof, a
    // step up to the 1-up, girders to the last checkpoint and a last slide to the door
    b.start(2, 13);
    b.ground(0, 9, 14); b.crack(7, 13); b.row(3, 9, 12, 3);
    b.plat(12, 14, 12); b.enemy(12, 14, 11); b.plat(17, 19, 10); b.ember(18, 8); b.mover(22, 10, 3, 28, 10, 300); b.ember(25, 7);
    b.ground(32, 44, 12); b.crack(32, 11); b.enemy(33, 38, 11); b.plat(35, 38, 9); b.enemy(35, 38, 8); b.ember(36, 6); b.ember(41, 8); b.enemy(40, 43, 11); b.crack(44, 11);
    b.plat(47, 49, 10); b.plat(52, 54, 8); b.ember(53, 6); b.plat(57, 59, 10); b.enemy(57, 59, 9); b.ember(58, 8);
    b.ground(62, 72, 13); b.crack(62, 12); b.check(63, 12); b.mush(65, 12); b.spikes(68, 69, 12); b.ember(68, 9); b.ember(69, 9); b.enemy(70, 72, 12);
    b.mover(74, 12, 3, 74, 7, 280); b.ember(75, 4);
    b.ground(78, 92, 7); b.crack(78, 6); b.enemy(79, 85, 6); b.enemy(87, 91, 6); b.row(80, 90, 3, 5); b.crack(92, 6);
    b.ground(95, 110, 13); b.crack(95, 12); b.enemy(96, 101, 12); b.spikes(103, 104, 12); b.ember(103, 9); b.ember(104, 9); b.enemy(105, 108, 12); b.check(110, 12);
    // Part 2
    b.plat(113, 115, 11); b.ember(114, 9); b.plat(118, 120, 9); b.enemy(118, 120, 8); b.plat(123, 125, 11); b.ember(124, 9);
    b.mover(128, 11, 3, 134, 11, 280); b.ember(131, 8);
    b.ground(138, 152, 12); b.crack(138, 11); b.enemy(139, 144, 11); b.plat(141, 144, 9); b.enemy(141, 144, 8);
    b.spikes(147, 148, 11); b.ember(147, 8); b.ember(148, 8); b.enemy(149, 151, 11); b.crack(152, 11);
    b.ground(153, 158, 9); b.mush(154, 8); b.ember(156, 6); b.oneup(158, 8);
    b.plat(161, 163, 10); b.enemy(161, 163, 9); b.mover(165, 8, 3, 169, 8, 260); b.ember(167, 6); b.plat(171, 173, 10); b.enemy(171, 173, 9);
    b.ground(176, 188, 13); b.crack(176, 12); b.check(177, 12); b.enemy(180, 187, 12); b.ember(183, 9); b.crack(188, 12);
    b.mover(190, 12, 3, 197, 12, 300); b.ember(193, 9); b.ember(197, 9);
    b.ground(201, 219, 12); b.mush(202, 11); b.enemy(203, 208, 11); b.plat(205, 208, 9); b.enemy(205, 208, 8); b.ember(206, 6);
    b.spikes(210, 211, 11); b.ember(210, 8); b.ember(211, 8); b.enemy(212, 214, 11); b.crack(219, 11);
    b.door(216, 11);
    // Coins
    b.coins(3, 9, 11, 2); b.coins(12, 14, 11); b.coins(17, 19, 9); b.coins(22, 30, 8, 2); b.coins(39, 43, 10);
    b.coins(47, 49, 9); b.coins(52, 54, 7); b.coins(57, 59, 9); b.coins(63, 67, 11); b.coins(79, 91, 4, 3); b.coins(96, 101, 11);
    b.coins(113, 115, 10); b.coins(123, 125, 10); b.coins(128, 136, 9, 2); b.coins(139, 143, 10, 2); b.coins(153, 157, 7);
    b.coins(161, 163, 8); b.coins(171, 173, 8); b.coins(180, 187, 11, 2); b.coins(190, 199, 10, 3); b.coins(212, 214, 10);
  }},
  { name: 'City 4', w: 254, foe: 'alien', gbaFoe: 'mech', after: 'Graveyard', build(b) {
    // World 3-4, "Tower Climb": up two roofs, a girder and a slide to a guarded roof, a drop to a spike street and the
    // first checkpoint, an elevator to a high roof, a street and a step up to the midway checkpoint; then a rising
    // platform and a slide to a spike street, two steps up to the 1-up, a slide down to the last checkpoint, a guarded
    // step and a last street to the door
    // The door became a boss arena (the user's request, 2026-10-16), like World 1-4's: a drop to a checkpoint, then a
    // one-screen arena (cols 224-253) sealed by a stone gate at col 224, where the Mech Walker waits; its door only
    // appears once the boss is beaten
    b.start(2, 13);
    b.ground(0, 12, 14); b.crack(10, 13); b.row(4, 10, 12, 3);
    b.ground(13, 18, 11); b.enemy(14, 18, 10); b.ember(16, 8);
    b.ground(19, 24, 8); b.enemy(20, 23, 7); b.ember(22, 5); b.crack(24, 7);
    b.plat(27, 29, 6); b.enemy(27, 29, 5); b.ember(28, 4); b.mover(32, 6, 3, 38, 6, 300); b.ember(35, 3);
    b.ground(42, 50, 8); b.enemy(43, 49, 7); b.ember(46, 4); b.crack(50, 7);
    b.ground(53, 68, 14); b.spikes(56, 57, 13); b.ember(56, 10); b.ember(57, 10); b.enemy(59, 64, 13); b.plat(60, 64, 11); b.enemy(60, 64, 10);
    b.check(66, 13); b.mush(67, 13); b.crack(68, 13);
    b.mover(70, 13, 3, 70, 7, 300); b.ember(71, 4);
    b.ground(74, 84, 7); b.enemy(75, 79, 6); b.enemy(80, 83, 6); b.row(76, 82, 3, 3); b.crack(84, 6);
    b.ground(87, 97, 13); b.crack(87, 12); b.enemy(88, 94, 12); b.ember(91, 9); b.crack(97, 12);
    b.ground(98, 110, 10); b.enemy(99, 104, 9); b.ember(102, 6); b.mush(106, 9); b.check(108, 9); b.crack(110, 9);
    // Part 2
    b.mover(112, 9, 3, 115, 7, 240); b.ember(114, 6); b.plat(118, 120, 10); b.enemy(118, 120, 9); b.mover(123, 10, 3, 129, 10, 280); b.ember(126, 7);
    b.ground(133, 147, 14); b.crack(133, 13); b.enemy(134, 139, 13); b.spikes(141, 142, 13); b.ember(141, 10); b.ember(142, 10); b.enemy(144, 147, 13);
    b.ground(148, 152, 11); b.mush(149, 10); b.ember(150, 8); b.crack(152, 10);
    b.ground(153, 157, 8); b.enemy(153, 156, 7); b.ember(155, 5); b.oneup(157, 7);
    b.mover(159, 8, 3, 165, 8, 280); b.ember(162, 5);
    b.ground(169, 186, 13); b.check(170, 12); b.mush(172, 12); b.enemy(175, 181, 12); b.plat(176, 180, 10); b.enemy(176, 180, 9);
    b.spikes(183, 184, 12); b.ember(183, 9); b.ember(184, 9); b.crack(186, 12);
    b.ground(187, 192, 10); b.enemy(188, 192, 9); b.ember(190, 6);
    b.ground(195, 219, 13); b.enemy(197, 203, 12); b.plat(199, 203, 10); b.enemy(199, 203, 9); b.ember(201, 7);
    b.spikes(206, 207, 12); b.ember(206, 9); b.ember(207, 9); b.enemy(209, 213, 12); b.ember(214, 9);
    // Boss arena: the same planks as the other boss arenas
    b.ground(220, 253, 14); b.mush(221, 13); b.check(222, 13);
    b.plat(228, 231, 11); b.plat(233, 244, 8); b.plat(246, 249, 11);
    b.boss(224, 251, 13);
    // Coins
    b.coins(3, 11, 11, 2); b.coins(13, 17, 9); b.coins(19, 23, 6); b.coins(32, 40, 4, 2); b.coins(43, 49, 6, 2);
    b.coins(53, 55, 12); b.coins(59, 63, 12, 2); b.coins(70, 72, 5); b.coins(75, 83, 4, 2); b.coins(88, 94, 11, 2); b.coins(99, 105, 8, 2);
    b.coins(123, 131, 8, 2); b.coins(134, 139, 12); b.coins(148, 152, 9); b.coins(153, 156, 6); b.coins(159, 167, 6, 2);
    b.coins(170, 174, 11); b.coins(188, 192, 8); b.coins(197, 203, 11, 2);
  }},

  // World 4-1, 4-2 and 4-3, between World 3-4 and World 4-4, added at the user's request (2026-10-16): World 4-4's look,
  // music and hell beasts, about its length and difficulty, and no boss. World 4-4 is 384 tiles with a 32-tile boss arena,
  // so each is 352 tiles ending at a door, with World 4-4's (harder, GBA) mix before its arena: 27-28 beasts, 3 moving
  // platforms, 36 embers, about 40 coins, 6 cracked blocks, a hidden 1-up, 8 mushrooms (one blue) and 5 checkpoints.
  // The user then had the beasts cut by 15% (27-28 -> 23-24), dropping repeats: pairs, back-to-back guarded planks.
  // Named "Underworld n" ("Hell n" would be placed after Hell); they follow World 3-4 in this file's order.
  { name: 'Underworld 1', w: 352, foe: 'beast', after: 'Graveyard', build(b) {
    // World 4-1, "Ash Steps": a guarded step, a guarded floor under a plank and a spike pit to the first checkpoint,
    // guarded plank hops and a long floor; a staircase to a guarded shelf, a plank chain and a long floor; a moving
    // platform, a climb, a spike run and an elevator to a high ledge; a guarded walkway, a long ride, a guarded floor,
    // a last guarded plank chain and the door
    b.start(2, 13);
    b.ground(0, 14, 14); b.row(5, 11, 13, 2); b.crack(13, 13);
    b.ground(15, 19, 14); b.enemy(15, 19, 13);
    b.ground(22, 26, 12); b.enemy(22, 26, 11); b.row(23, 25, 10, 2);
    b.ground(29, 42, 14); b.enemy(31, 37, 13); b.plat(32, 35, 11); b.row(33, 34, 10); b.spikes(39, 40, 13); b.ember(39, 11); b.ember(40, 11);
    b.ground(43, 52, 14); b.mush(44, 13); b.check(47, 13); b.crack(52, 13);
    b.plat(55, 57, 12); b.plat(60, 62, 10); b.ember(61, 9); b.enemy(60, 62, 9); b.plat(65, 67, 12);
    // (its mushroom at 71, 13 was removed at the user's request, 2026-10-21)
    b.ground(70, 97, 14); b.enemy(72, 77, 13); b.row(73, 77, 11, 2); b.spikes(79, 80, 13); b.enemy(82, 87, 13); b.check(92, 13);
    // Part 2
    b.ground(98, 101, 12); b.mush(98, 11); b.ground(102, 105, 10); b.enemy(102, 105, 9);
    b.ground(109, 120, 10); b.enemy(110, 119, 9); b.row(111, 117, 8, 3); b.crack(120, 9);
    b.ground(121, 132, 14); b.spikes(125, 126, 13); b.enemy(129, 132, 13);
    b.plat(135, 137, 12); b.plat(140, 142, 10); b.ember(141, 9); b.enemy(140, 142, 9); b.plat(145, 147, 12);
    b.ground(150, 175, 14); b.mush(151, 13); b.enemy(154, 160, 13); b.plat(162, 165, 11); b.row(163, 164, 10); b.enemy(162, 165, 10);
    b.spikes(167, 168, 13); b.enemy(170, 175, 13);
    // Part 3
    b.ground(176, 182, 14); b.check(178, 13);
    b.mover(184, 12, 3, 192, 12, 320);
    b.ground(197, 206, 12); b.enemy(198, 205, 11); b.row(199, 203, 10, 2);
    b.ground(207, 211, 10); b.enemy(207, 211, 9); b.ground(212, 216, 8); b.ember(213, 7); b.ember(215, 7); b.crack(216, 7);
    b.ground(217, 236, 14); b.mush(218, 13); b.spikes(221, 222, 13); b.enemy(224, 226, 13); b.spikes(228, 229, 13); b.enemy(232, 233, 13);
    b.mover(238, 13, 3, 238, 7, 300);
    b.ground(243, 252, 8); b.check(244, 7); b.ember(246, 6); b.ember(249, 6); b.bluemush(252, 7);
    // Part 4
    b.ground(253, 266, 9); b.crack(255, 8); b.enemy(261, 263, 8);
    b.mover(268, 11, 3, 280, 11, 300); b.row(270, 280, 9, 5);
    b.ground(285, 296, 12); b.mush(285, 11); b.enemy(286, 295, 11);
    b.ground(297, 308, 14); b.enemy(299, 301, 13); b.spikes(303, 304, 13); b.enemy(305, 308, 13);
    b.plat(311, 313, 12); b.plat(316, 318, 10); b.ember(317, 9); b.enemy(316, 318, 9); b.plat(321, 323, 12);
    // 1-up on the floor (user request, 2026-10-21; it was on the plank at 323, 11)
    b.oneup(327, 13);
    b.ground(325, 351, 14); b.mush(326, 13); b.enemy(329, 337, 13); b.row(330, 336, 11, 3); b.check(340, 13);
    b.spikes(343, 344, 13); b.ember(343, 11); b.ember(344, 11); b.crack(350, 13);
    b.door(348, 13);
    // Coins
    b.coins(4, 12, 12, 2); b.coins(22, 26, 9, 2); b.coins(55, 57, 11); b.coins(65, 67, 11); b.coins(98, 101, 10); b.coins(145, 147, 11);
    b.coins(212, 215, 5); b.coins(219, 220, 13); b.coin(239, 11); b.coin(239, 9); b.coins(272, 278, 8, 2); b.coins(321, 322, 11);
    b.coins(326, 328, 12); b.coins(345, 347, 12);
  }},
  { name: 'Underworld 2', w: 352, foe: 'beast', after: 'Graveyard', build(b) {
    // World 4-2, "Brimstone Bridges": guarded plank hops and a guarded floor to the first checkpoint, more plank hops and
    // a floor under a guarded plank; a long ride, a guarded floor, a guarded plank chain and two spike pits; a plank
    // climb to the 1-up, a guarded floor and an elevator to a high ledge; a guarded step, a long ride, guarded planks,
    // a guarded floor, a last guarded plank chain and the door
    b.start(2, 13);
    b.ground(0, 12, 14); b.row(4, 10, 13, 2); b.crack(11, 13);
    b.plat(15, 17, 12); b.plat(20, 22, 10); b.enemy(20, 22, 9); b.plat(25, 27, 12); b.ember(26, 9);
    b.ground(30, 44, 14); b.enemy(32, 36, 13); b.spikes(38, 39, 13); b.ember(38, 11); b.ember(39, 11); b.enemy(41, 44, 13);
    b.ground(45, 50, 11); b.check(46, 10); b.mush(48, 10); b.crack(50, 10);
    b.plat(53, 55, 9); b.ember(54, 6); b.plat(58, 60, 11); b.enemy(58, 60, 10); b.plat(63, 65, 9); b.ember(64, 6); b.plat(68, 70, 11);
    b.ground(73, 92, 14); b.mush(74, 13); b.enemy(77, 82, 13); b.plat(78, 81, 11); b.row(79, 80, 10); b.enemy(78, 81, 10);
    b.spikes(85, 86, 13); b.enemy(88, 92, 13);
    // Part 2
    b.ground(93, 100, 14); b.mush(93, 13); b.check(96, 13);
    b.mover(102, 12, 3, 110, 12, 300); b.row(104, 110, 10, 3);
    b.ground(115, 126, 12); b.enemy(116, 125, 11); b.row(117, 123, 9, 3); b.crack(126, 11);
    b.plat(129, 131, 10); b.enemy(129, 131, 9); b.plat(134, 136, 12); b.ember(135, 9); b.plat(139, 141, 10); b.enemy(139, 141, 9); b.plat(144, 146, 12);
    b.ground(149, 170, 14); b.mush(150, 13); b.spikes(153, 154, 13); b.enemy(157, 163, 13); b.spikes(166, 167, 13); b.ember(166, 11); b.ember(167, 11); b.crack(170, 13);
    b.ground(171, 176, 11); b.enemy(171, 176, 10);
    // Part 3
    b.ground(177, 186, 14); b.mush(178, 13); b.check(181, 13);
    b.plat(189, 191, 12); b.plat(194, 196, 10); b.ember(195, 9); b.enemy(194, 196, 9); b.plat(199, 201, 8); b.ember(200, 7);
    b.plat(204, 206, 10); b.enemy(204, 206, 9); b.plat(209, 211, 12);
    // The 1-up: on this ground's left end (it was at (201, 7) on the floating plank until the user moved it, 2026-10-20)
    b.ground(214, 232, 14); b.oneup(215, 13); b.enemy(216, 221, 13); b.spikes(224, 225, 13); b.ember(224, 11); b.ember(225, 11); b.enemy(228, 232, 13);
    b.mover(234, 13, 3, 234, 7, 300);
    b.ground(239, 250, 8); b.check(240, 7); b.row(242, 248, 6, 3); b.bluemush(250, 7);
    // Part 4
    b.ground(251, 262, 10); b.crack(253, 9); b.enemy(258, 262, 9);
    b.mover(265, 11, 3, 277, 11, 300); b.row(267, 277, 9, 5);
    b.plat(282, 284, 12); b.enemy(282, 284, 11); b.plat(287, 289, 10); b.ember(288, 9); b.plat(292, 294, 12); b.enemy(292, 294, 11);
    b.ground(297, 312, 14); b.mush(298, 13); b.enemy(300, 305, 13); b.spikes(308, 309, 13); b.ember(308, 11); b.ember(309, 11); b.enemy(311, 312, 13);
    b.plat(315, 317, 12); b.enemy(315, 317, 11); b.plat(320, 322, 10); b.ember(321, 9); b.plat(325, 327, 12); b.enemy(325, 327, 11);
    b.ground(330, 351, 14); b.enemy(332, 338, 13); b.row(333, 337, 11, 4); b.check(341, 13); b.mush(342, 13); b.spikes(344, 345, 13); b.crack(350, 13);
    b.door(348, 13);
    // Coins
    b.coins(4, 10, 12, 2); b.coins(25, 27, 11); b.coins(53, 55, 8); b.coins(63, 65, 8); b.coins(115, 118, 11); b.coins(134, 136, 11);
    b.coins(189, 191, 11); b.coins(242, 248, 7, 3); b.coins(268, 276, 8, 4); b.coins(287, 289, 8); b.coins(320, 322, 8);
    b.coins(330, 331, 13); b.coins(346, 347, 13);
  }},
  { name: 'Underworld 3', w: 352, foe: 'beast', after: 'Graveyard', build(b) {
    // World 4-3, "Molten Climb": up two steps, a drop to a spike pit and a guarded step to the first checkpoint, a climb
    // to a guarded ledge, plank hops and a guarded floor under a plank; an elevator to a high ledge, two drops, a spike
    // pit and guarded planks; a long ride, a climb, a spike run between beasts; guarded planks up to the 1-up, a guarded
    // floor under a guarded plank, a long ride, a guarded step, a last plank chain and the door
    b.start(2, 13);
    b.ground(0, 10, 14); b.row(4, 10, 12, 3); b.crack(9, 13);
    b.ground(11, 16, 11); b.enemy(11, 16, 10);
    b.ground(17, 22, 8); b.enemy(18, 21, 7); b.ember(18, 6); b.ember(21, 6); b.crack(22, 7);
    b.ground(25, 38, 14); b.enemy(27, 32, 13); b.spikes(34, 35, 13); b.ember(34, 11); b.ember(35, 11); b.mush(37, 13);
    b.ground(39, 48, 12); b.check(40, 11); b.enemy(45, 48, 11);
    b.ground(49, 54, 9); b.ember(51, 8); b.ground(55, 60, 6); b.enemy(55, 60, 5);
    b.plat(63, 65, 8); b.ember(64, 7); b.plat(68, 70, 10); b.enemy(68, 70, 9); b.plat(73, 75, 12);
    b.ground(78, 96, 14); b.mush(79, 13); b.enemy(82, 88, 13); b.plat(83, 87, 11); b.row(84, 86, 10, 2); b.enemy(83, 87, 10);
    b.spikes(91, 92, 13);
    // Part 2
    b.ground(97, 104, 14); b.check(103, 13);
    b.mover(106, 13, 3, 106, 7, 300);
    b.ground(110, 124, 7); b.enemy(115, 121, 6); b.row(112, 122, 5, 5); b.crack(124, 6);
    b.ground(127, 140, 10); b.enemy(130, 135, 9); b.ember(136, 7);
    b.ground(141, 156, 14); b.mush(142, 13); b.spikes(145, 146, 13); b.ember(145, 11); b.ember(146, 11); b.enemy(149, 155, 13); b.crack(156, 13);
    b.plat(159, 161, 12); b.enemy(159, 161, 11); b.plat(164, 166, 10); b.ember(165, 8); b.plat(169, 171, 12);
    b.ground(174, 180, 14); b.mush(174, 13); b.check(176, 13);
    // Part 3
    b.mover(182, 12, 3, 190, 12, 320); b.row(184, 190, 10, 3);
    b.ground(195, 206, 12); b.mush(195, 11); b.enemy(196, 205, 11);
    b.ground(207, 212, 9); b.enemy(207, 212, 8); b.ground(213, 218, 6); b.ember(214, 5); b.ember(217, 5); b.crack(218, 5);
    b.ground(221, 240, 14); b.spikes(224, 225, 13); b.enemy(228, 230, 13); b.spikes(233, 234, 13); b.ember(233, 11); b.ember(234, 11); b.enemy(237, 240, 13);
    b.ground(241, 250, 11); b.check(242, 10); b.bluemush(249, 10);
    // Part 4
    b.plat(253, 255, 9); b.enemy(253, 255, 8); b.plat(258, 260, 7); b.ember(259, 6); b.oneup(269, 11); b.plat(263, 265, 9); b.enemy(263, 265, 8);
    b.ground(268, 282, 12); b.enemy(270, 276, 11); b.plat(271, 275, 9); b.row(272, 274, 8, 2); b.enemy(271, 275, 8);
    b.mover(284, 12, 3, 296, 12, 300); b.row(287, 295, 10, 4);
    b.ground(300, 312, 14); b.mush(301, 13); b.enemy(303, 308, 13); b.spikes(310, 311, 13); b.ember(310, 11); b.ember(311, 11);
    b.ground(313, 318, 11); b.enemy(314, 318, 10);
    b.plat(321, 323, 9); b.ember(322, 8); b.plat(326, 328, 11); b.enemy(326, 328, 10);
    b.ground(331, 351, 14); b.enemy(333, 338, 13); b.ember(336, 11); b.ember(339, 11); b.check(341, 13); b.mush(343, 13); b.crack(350, 13);
    b.door(348, 13);
    // Coins
    b.coins(5, 9, 11, 2); b.coins(17, 21, 5, 2); b.coins(52, 54, 8); b.coins(63, 65, 6); b.coins(73, 75, 11); b.coins(110, 113, 6);
    b.coins(143, 144, 13); b.coins(169, 171, 11); b.coins(196, 199, 10); b.coins(302, 303, 13); b.coins(321, 323, 7);
    b.coins(331, 332, 13); b.coins(344, 347, 13);
  }},
];
