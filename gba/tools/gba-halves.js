// The second half of every level (the user's request, 2026-10-17: each level twice as long, with a proportionate number
// of enemies, embers and mushrooms). Each is a new stretch of course in its world's style, with the same counts as the
// level's own course (enemies, embers, mushrooms, coins, cracked blocks, checkpoints and moving platforms); a level keeps
// its one hidden 1-up.
// Columns here are relative to `at`, where the half goes in: the end of a level with a door (its old door becomes a
// checkpoint, and the half ends at a new door), or a boss level's run-up, before its checkpoint and arena (which move
// `len` columns right). Same builder as gba-levels.js: ground(x0, x1, top) fills from `top` down, and a thing "on" ground
// whose top is row t sits on row t - 1. Mario's jump reaches about 3.8 tiles up and 4.5 across.
// Checked by tools/check-levels.js (placements, every coin and ember reachable, start to door).
module.exports = {
  // ---------- World 1 (jungle, aliens) ----------
  Jungle: { at: 220, len: 220, build(b) {
    // World 1-1, Part 3: a log ride to a guarded floor, an elevator to a high ledge, a drop to the checkpoint, a log to a
    // floor under a guarded branch; Part 4: plank hops to the last checkpoint, an elevator to the canopy, a spike floor,
    // a step up, a last log ride and the door
    b.mover(1, 11, 3, 10, 11, 300); b.ember(5, 8); b.ember(9, 8);
    b.ground(14, 34, 14); b.crack(17, 13); b.enemy(18, 23, 13); b.spikes(25, 26, 13); b.ember(25, 10); b.ember(26, 10);
    b.enemy(28, 33, 13); b.plat(28, 32, 11); b.enemy(29, 32, 10); b.crack(34, 13);
    b.mover(36, 13, 3, 36, 8, 300); b.ember(37, 5);
    b.ground(40, 52, 9); b.mush(41, 8); b.enemy(42, 50, 8); b.row(43, 49, 6, 3); b.crack(52, 8);
    b.ground(55, 70, 13); b.check(56, 12); b.enemy(59, 64, 12); b.spikes(66, 67, 12); b.ember(66, 9); b.ember(67, 9); b.crack(70, 12);
    b.mover(72, 11, 3, 81, 11, 300); b.ember(76, 8);
    b.ground(85, 100, 11); b.enemy(87, 93, 10); b.plat(89, 92, 8); b.enemy(89, 92, 7); b.ember(90, 5); b.enemy(94, 99, 10); b.mush(96, 10); b.ember(98, 7); b.crack(100, 10);
    // Part 4
    b.plat(103, 105, 12); b.ember(104, 10); b.plat(108, 110, 10); b.enemy(108, 110, 9); b.ember(109, 7);
    b.ground(113, 128, 14); b.crack(113, 13); b.check(114, 13); b.enemy(117, 122, 13); b.spikes(124, 125, 13); b.ember(124, 10); b.ember(125, 10); b.enemy(126, 128, 13);
    b.mover(130, 13, 3, 130, 7, 300); b.ember(131, 4);
    b.ground(134, 150, 8); b.crack(134, 7); b.mush(135, 7); b.enemy(136, 142, 7); b.enemy(144, 149, 7); b.row(137, 145, 5, 4); b.crack(150, 7);
    b.ground(153, 170, 14); b.enemy(155, 160, 13); b.plat(156, 160, 11); b.enemy(157, 160, 10); b.ember(158, 8); b.spikes(163, 164, 13); b.ember(163, 10); b.ember(164, 10);
    b.enemy(166, 169, 13); b.crack(170, 13);
    b.ground(171, 186, 12); b.check(172, 11); b.mush(174, 11); b.enemy(176, 182, 11); b.ember(179, 8); b.crack(186, 11);
    b.mover(188, 10, 3, 197, 10, 300); b.ember(192, 7); b.ember(196, 7);
    b.ground(201, 219, 12); b.crack(201, 11); b.enemy(203, 208, 11); b.plat(205, 208, 9); b.enemy(205, 208, 8); b.ember(206, 6);
    b.spikes(211, 212, 11); b.ember(211, 8); b.ember(212, 8); b.enemy(213, 215, 11); b.crack(219, 11);
    b.door(216, 11);
    // Coins
    b.coins(2, 10, 7, 2); b.coins(19, 23, 12); b.coins(28, 32, 12); b.coin(37, 10); b.coin(37, 8); b.coins(43, 49, 7, 2);
    b.coins(58, 64, 11, 2); b.coins(74, 83, 9, 3); b.coins(86, 88, 9); b.coins(97, 99, 9); b.coins(103, 105, 11); b.coins(108, 110, 8);
    b.coins(116, 122, 12, 2); b.coin(131, 10); b.coin(131, 8); b.coins(137, 149, 6, 4); b.coins(154, 160, 12, 2); b.coins(165, 169, 12);
    b.coins(175, 182, 10); b.coins(189, 198, 8, 3); b.coins(203, 208, 10); b.coins(213, 215, 10);
  }},
  'Jungle 2': { at: 220, len: 220, build(b) {
    // World 1-2, Part 3: plank hops, three terraces up, a drop to a spike floor under a guarded branch, a step to the
    // checkpoint, a log ride, a guarded floor and a high step; Part 4: plank drops to a long floor, an elevator to a high
    // floor, a step down to the last checkpoint, planks, a last log ride and the door
    b.plat(2, 4, 10); b.ember(3, 8); b.plat(7, 9, 8); b.enemy(7, 9, 7); b.ember(8, 5); b.plat(12, 14, 10);
    b.ground(17, 28, 12); b.crack(17, 11); b.enemy(18, 23, 11); b.ember(20, 8); b.crack(28, 11);
    b.ground(29, 34, 10); b.enemy(29, 34, 9); b.ember(31, 6);
    b.ground(35, 40, 8); b.ember(37, 4); b.crack(40, 7);
    b.ground(43, 58, 13); b.crack(43, 12); b.spikes(46, 47, 12); b.ember(46, 9); b.ember(47, 9); b.enemy(50, 56, 12); b.plat(51, 55, 10); b.enemy(52, 55, 9);
    b.ground(59, 66, 11); b.check(60, 10); b.ember(63, 7); b.crack(66, 10);
    b.mover(68, 10, 3, 77, 10, 300); b.ember(72, 7); b.ember(76, 7);
    b.ground(81, 96, 12); b.crack(81, 11); b.enemy(83, 89, 11); b.ember(87, 8); b.spikes(91, 92, 11); b.ember(91, 8); b.ember(92, 8); b.enemy(93, 96, 11);
    b.ground(97, 104, 9); b.mush(98, 8); b.ember(101, 6); b.crack(104, 8);
    // Part 4
    b.plat(107, 109, 11); b.ember(108, 9); b.plat(112, 114, 13); b.ember(113, 10);
    b.ground(117, 134, 14); b.enemy(118, 123, 13); b.plat(120, 123, 11); b.enemy(121, 123, 10); b.row(125, 131, 10, 3); b.spikes(127, 128, 13);
    b.enemy(130, 133, 13); b.crack(134, 13);
    b.mover(136, 13, 3, 136, 8, 300); b.ember(137, 5);
    b.ground(140, 156, 9); b.enemy(142, 148, 8); b.ember(145, 5); b.enemy(150, 155, 8); b.ember(152, 5); b.crack(156, 8);
    b.ground(159, 172, 12); b.check(160, 11); b.spikes(165, 166, 11); b.ember(165, 8); b.ember(166, 8); b.crack(172, 11);
    b.plat(175, 177, 10); b.enemy(175, 177, 9); b.ember(176, 7); b.plat(180, 182, 12);
    b.mover(184, 12, 3, 193, 12, 280); b.ember(188, 9); b.ember(192, 9);
    b.ground(197, 219, 12); b.crack(197, 11); b.mush(198, 11); b.enemy(200, 206, 11); b.plat(202, 205, 9); b.enemy(202, 205, 8); b.ember(203, 6);
    b.spikes(208, 209, 11); b.ember(208, 8); b.ember(209, 8); b.enemy(211, 214, 11); b.crack(219, 11);
    b.door(216, 11);
    // Coins
    b.coins(2, 4, 9); b.coins(12, 14, 9); b.coins(19, 27, 10, 2); b.coins(30, 34, 8); b.coins(35, 39, 6); b.coins(50, 56, 11, 2);
    b.coins(60, 65, 9); b.coins(69, 78, 8, 3); b.coins(83, 89, 10, 2); b.coins(97, 103, 7); b.coins(107, 109, 10); b.coins(112, 114, 12);
    b.coins(118, 123, 12); b.coin(137, 10); b.coin(137, 8); b.coins(142, 148, 7, 2); b.coins(150, 154, 7, 2); b.coins(161, 164, 10);
    b.coins(167, 169, 10); b.coins(180, 182, 11); b.coins(185, 193, 10, 2); b.coins(212, 214, 10);
  }},
  'Jungle 3': { at: 220, len: 220, build(b) {
    // World 1-3, Part 3: a log ride, planks over the river, a guarded floor, a long log ride to the checkpoint, a guarded
    // bank, planks and a log to a guarded floor; Part 4: a high bank to the last checkpoint, a log to a guarded plank
    // chain, a spike floor, a bank with a mushroom, a last log ride and the door
    b.mover(1, 11, 3, 9, 11, 300); b.ember(5, 8); b.ember(11, 8);
    b.plat(13, 15, 10); b.ember(14, 8); b.plat(18, 20, 12); b.ember(19, 10);
    b.ground(23, 36, 13); b.crack(23, 12); b.enemy(25, 30, 12); b.plat(27, 30, 10); b.enemy(27, 30, 9); b.ember(28, 7);
    b.spikes(32, 33, 12); b.ember(32, 9); b.ember(33, 9); b.enemy(34, 36, 12);
    b.mover(38, 11, 3, 47, 11, 280); b.ember(42, 8); b.ember(46, 8);
    b.ground(51, 62, 12); b.check(52, 11); b.mush(54, 11); b.ember(56, 8); b.enemy(57, 61, 11); b.crack(62, 11);
    b.ground(63, 70, 10); b.enemy(64, 69, 9); b.ember(66, 6); b.crack(70, 9);
    b.plat(73, 75, 9); b.ember(74, 7); b.plat(78, 80, 11); b.enemy(78, 80, 10);
    b.mover(82, 11, 3, 90, 11, 260); b.ember(86, 8);
    b.ground(94, 112, 13); b.crack(94, 12); b.enemy(96, 102, 12); b.plat(98, 101, 10); b.enemy(98, 101, 9); b.ember(100, 7);
    b.spikes(105, 106, 12); b.ember(105, 9); b.ember(106, 9); b.enemy(108, 111, 12); b.crack(112, 12);
    // Part 4
    b.ground(113, 122, 10); b.crack(113, 9); b.check(114, 9); b.ember(118, 6); b.crack(122, 9);
    b.mover(124, 10, 3, 132, 10, 280); b.ember(128, 7);
    b.plat(136, 138, 11); b.enemy(136, 138, 10); b.plat(141, 143, 9); b.ember(142, 7); b.plat(146, 148, 11); b.ember(147, 9);
    b.ground(151, 170, 14); b.crack(151, 13); b.enemy(153, 159, 13); b.plat(155, 159, 11); b.enemy(156, 159, 10); b.ember(157, 8);
    b.spikes(162, 163, 13); b.ember(162, 10); b.ember(163, 10); b.enemy(165, 169, 13); b.crack(170, 13);
    b.ground(171, 180, 12); b.mush(172, 11); b.enemy(174, 179, 11); b.ember(176, 8); b.crack(180, 11);
    b.mover(182, 11, 3, 191, 11, 300); b.ember(186, 8); b.ember(190, 8);
    b.ground(195, 219, 12); b.crack(195, 11); b.enemy(197, 203, 11); b.ember(200, 8); b.spikes(206, 207, 11); b.ember(206, 8); b.ember(207, 8);
    b.enemy(209, 212, 11); b.ember(214, 8); b.crack(219, 11);
    b.door(216, 11);
    // Coins
    b.coins(2, 10, 7, 2); b.coins(13, 15, 9); b.coins(18, 20, 11); b.coins(25, 30, 11); b.coins(39, 48, 9, 3); b.coins(55, 61, 10);
    b.coins(64, 69, 8); b.coins(73, 75, 8); b.coins(78, 80, 9); b.coins(83, 91, 9, 2); b.coins(96, 101, 11); b.coins(114, 121, 8);
    b.coins(125, 133, 8, 2); b.coins(141, 143, 8); b.coins(153, 159, 12); b.coins(173, 179, 10); b.coins(183, 191, 9, 2);
  }},
  'Jungle 4': { at: 220, len: 220, build(b) {
    // World 1-4, Part 3 (before the boss): planks up to the canopy, a drop to a guarded floor and the checkpoint, an
    // elevator to the treetops, planks down to a guarded floor and a step to the second checkpoint; Part 4: a log ride to a
    // high floor, a spike floor, an elevator to the canopy, a drop to the last checkpoint and a log down to the arena
    b.plat(2, 4, 9); b.ember(3, 7); b.plat(7, 9, 7); b.enemy(7, 9, 6); b.plat(12, 14, 5); b.ember(13, 3);
    b.ground(17, 30, 7); b.enemy(19, 25, 6); b.ember(22, 3); b.ember(28, 3); b.crack(30, 6);
    b.ground(33, 48, 12); b.crack(33, 11); b.spikes(36, 37, 11); b.ember(36, 8); b.ember(37, 8); b.enemy(39, 45, 11); b.plat(40, 44, 9); b.enemy(41, 44, 8);
    b.ember(42, 6); b.crack(48, 11);
    b.ground(49, 56, 10); b.check(50, 9); b.ember(53, 7); b.crack(56, 9);
    b.mover(58, 10, 3, 58, 5, 300); b.ember(59, 3);
    b.ground(62, 76, 6); b.enemy(64, 70, 5); b.row(65, 73, 3, 4); b.crack(76, 5);
    b.plat(79, 81, 8); b.ember(80, 6); b.plat(84, 86, 10); b.enemy(84, 86, 9); b.plat(89, 91, 12); b.enemy(89, 91, 11); b.ember(90, 8);
    b.ground(94, 110, 14); b.crack(94, 13); b.enemy(96, 102, 13); b.plat(97, 101, 11); b.enemy(98, 101, 10); b.ember(99, 8);
    b.spikes(104, 105, 13); b.ember(104, 10); b.ember(105, 10); b.enemy(107, 110, 13);
    b.ground(111, 118, 11); b.check(112, 10); b.mush(114, 10); b.ember(115, 7); b.crack(118, 10);
    // Part 4
    b.mover(120, 11, 3, 129, 11, 300); b.ember(124, 8); b.ember(128, 8);
    b.ground(133, 148, 9); b.enemy(135, 141, 8); b.ember(138, 5); b.enemy(143, 147, 8); b.ember(145, 5); b.crack(148, 8);
    b.ground(151, 166, 13); b.spikes(154, 155, 12); b.ember(154, 9); b.ember(155, 9); b.enemy(157, 163, 12); b.plat(158, 162, 10); b.enemy(159, 162, 9);
    b.ember(160, 7); b.crack(166, 12);
    b.mover(168, 13, 3, 168, 7, 300); b.ember(169, 4);
    b.ground(172, 186, 7); b.mush(173, 6); b.enemy(175, 181, 6); b.ember(178, 3); b.ember(183, 3); b.crack(186, 6);
    b.ground(189, 199, 12); b.check(190, 11); b.ember(194, 8); b.crack(199, 11);
    b.mover(201, 11, 3, 208, 11, 280); b.ember(205, 8);
    b.ground(212, 219, 14); b.enemy(213, 218, 13);
    // Coins
    b.coins(2, 4, 8); b.coins(12, 14, 4); b.coins(19, 27, 5, 2); b.coins(39, 45, 10, 2); b.coins(50, 55, 8); b.coin(59, 7);
    b.coins(63, 73, 4, 2); b.coins(79, 81, 7); b.coins(84, 86, 8); b.coins(96, 102, 12); b.coins(111, 117, 9);
    b.coins(121, 130, 9, 3); b.coins(135, 141, 7, 2); b.coins(143, 147, 7); b.coins(157, 163, 11, 2); b.coin(169, 10); b.coin(169, 8);
    b.coins(174, 181, 5); b.coins(190, 198, 10, 2); b.coins(202, 210, 9, 2);
  }},

  // ---------- World 2 (graveyard, reapers; no moving platforms) ----------
  Graveyard: { at: 200, len: 200, build(b) {
    // World 2-1, Part 3: beam hops, a guarded lawn under a beam, three steps up, a drop to the checkpoint and a spike pit,
    // a beam chain to a long lawn; Part 4: steps up, beams down to the last checkpoint, a raised plot, a beam chain, a
    // spike lawn and the last lawn to the door
    b.plat(2, 4, 12); b.ember(3, 10); b.plat(7, 9, 10); b.enemy(7, 9, 9); b.plat(12, 14, 12); b.ember(13, 10);
    b.ground(17, 30, 13); b.crack(17, 12); b.enemy(19, 25, 12); b.plat(21, 24, 10); b.enemy(21, 24, 9); b.ember(22, 7);
    b.spikes(27, 28, 12); b.ember(27, 9); b.ember(28, 9); b.crack(30, 12);
    b.ground(31, 35, 10); b.mush(32, 9); b.ember(33, 7);
    b.ground(36, 40, 8); b.enemy(36, 40, 7); b.ember(38, 5);
    b.ground(43, 56, 14); b.crack(43, 13); b.check(44, 13); b.spikes(48, 49, 13); b.ember(48, 10); b.ember(49, 10); b.enemy(51, 55, 13); b.ember(53, 10); b.crack(56, 13);
    b.plat(59, 61, 11); b.ember(60, 9); b.plat(64, 66, 9); b.enemy(64, 66, 8); b.plat(69, 71, 11); b.ember(70, 9);
    b.ground(74, 90, 14); b.crack(74, 13); b.enemy(76, 83, 13); b.ember(79, 10); b.mush(80, 13); b.plat(84, 87, 11); b.enemy(84, 87, 10);
    b.spikes(88, 89, 13); b.ember(88, 10); b.ember(89, 10);
    // Part 4
    b.ground(91, 98, 11); b.ember(94, 8); b.crack(98, 10);
    b.ground(99, 104, 8); b.enemy(99, 104, 7); b.ember(101, 5);
    b.plat(107, 109, 10); b.ember(108, 8); b.plat(112, 114, 12); b.ember(115, 10);
    b.ground(117, 132, 13); b.crack(117, 12); b.check(118, 12); b.spikes(121, 122, 12); b.ember(121, 9); b.ember(122, 9); b.enemy(124, 130, 12); b.ember(127, 9); b.crack(132, 12);
    b.ground(133, 137, 10); b.mush(134, 9); b.ember(136, 7);
    b.plat(140, 142, 9); b.enemy(140, 142, 8); b.plat(145, 147, 11); b.ember(146, 9); b.plat(150, 152, 9); b.ember(151, 7);
    b.ground(155, 172, 14); b.crack(155, 13); b.enemy(157, 163, 13); b.plat(159, 162, 11); b.enemy(159, 162, 10); b.spikes(165, 166, 13); b.ember(165, 10); b.ember(166, 10);
    b.enemy(168, 171, 13); b.crack(172, 13);
    b.ground(173, 199, 12); b.crack(173, 11); b.mush(175, 11); b.enemy(178, 185, 11); b.ember(181, 8); b.spikes(188, 189, 11); b.ember(188, 8); b.ember(189, 8);
    b.plat(191, 194, 9); b.enemy(191, 194, 8); b.crack(199, 11);
    b.door(196, 11);
    // Coins
    b.coins(2, 4, 11); b.coins(12, 14, 11); b.coins(19, 25, 11, 2); b.coins(33, 35, 9); b.coins(36, 40, 6); b.coins(45, 47, 12);
    b.coins(59, 61, 10); b.coins(64, 66, 7); b.coins(69, 71, 10); b.coins(76, 82, 12, 2); b.coins(91, 97, 9); b.coins(99, 103, 6);
    b.coins(107, 109, 9); b.coins(112, 114, 11); b.coins(124, 130, 11, 2); b.coins(133, 137, 8); b.coins(145, 147, 10);
    b.coins(157, 163, 12, 2); b.coins(174, 186, 10, 2); b.coins(190, 195, 10);
  }},
  'Graveyard 2': { at: 200, len: 200, build(b) {
    // World 2-2, Part 3: steps up three plots to a high one, a drop to a guarded lawn under a beam and the checkpoint, beam
    // hops and a long lawn; Part 4: steps up, a beam stair, a lawn to the last checkpoint, steps up, a guarded lawn, beams
    // and the last lawn to the door
    b.ground(0, 10, 14); b.row(3, 9, 11, 3);
    b.ground(11, 15, 12); b.enemy(11, 14, 11); b.ember(13, 8); b.crack(15, 11);
    b.ground(16, 21, 9); b.mush(17, 8); b.ember(18, 6);
    b.ground(22, 27, 6); b.enemy(22, 26, 5); b.ember(25, 3); b.crack(27, 5);
    b.ground(30, 46, 14); b.crack(30, 13); b.enemy(32, 38, 13); b.plat(34, 37, 11); b.enemy(34, 37, 10); b.ember(35, 8);
    b.spikes(40, 41, 13); b.ember(40, 10); b.ember(41, 10); b.check(45, 13); b.crack(46, 13);
    b.ground(47, 50, 11); b.mush(48, 10);
    b.plat(54, 56, 9); b.ember(55, 7); b.plat(59, 61, 11); b.enemy(59, 61, 10); b.ember(62, 8); b.plat(64, 66, 9); b.ember(65, 7); b.plat(69, 71, 11);
    b.ground(74, 96, 14); b.enemy(76, 84, 13); b.ember(79, 10); b.spikes(87, 88, 13); b.ember(87, 10); b.ember(88, 10);
    b.plat(90, 93, 11); b.enemy(90, 93, 10); b.row(90, 93, 9, 3); b.crack(96, 13);
    // Part 4
    b.ground(97, 101, 12); b.enemy(97, 100, 11); b.ember(99, 9); b.crack(101, 11);
    b.ground(102, 105, 9); b.mush(103, 8);
    b.plat(109, 111, 10); b.enemy(109, 111, 9); b.ember(110, 8); b.plat(114, 116, 8); b.enemy(114, 116, 7); b.ember(115, 6); b.plat(119, 121, 10);
    b.ground(124, 140, 13); b.crack(124, 12); b.check(125, 12); b.spikes(130, 131, 12); b.ember(130, 9); b.ember(131, 9); b.enemy(133, 139, 12); b.crack(140, 12);
    b.ground(141, 146, 10); b.enemy(142, 146, 9); b.ember(144, 7);
    b.ground(147, 152, 7); b.ember(149, 4); b.crack(152, 6);
    b.ground(155, 168, 12); b.enemy(157, 163, 11); b.plat(159, 162, 9); b.enemy(159, 162, 8); b.ember(160, 7); b.ember(165, 8); b.mush(166, 11); b.crack(168, 11);
    b.plat(171, 173, 10); b.ember(172, 8); b.plat(176, 178, 12);
    b.ground(181, 199, 14); b.crack(181, 13); b.enemy(183, 189, 13); b.ember(186, 10); b.spikes(191, 192, 13); b.ember(191, 10); b.ember(192, 10);
    b.enemy(193, 195, 13); b.crack(199, 13);
    b.door(196, 13);
    // Coins
    b.coins(1, 10, 12, 3); b.coins(16, 21, 7); b.coins(22, 26, 4); b.coins(32, 38, 12, 2); b.coins(47, 50, 9); b.coins(54, 56, 8);
    b.coins(64, 66, 8); b.coins(69, 71, 10); b.coins(76, 84, 12, 2); b.coins(102, 105, 7); b.coins(119, 121, 9); b.coins(126, 129, 11);
    b.coins(133, 139, 11, 2); b.coins(147, 151, 5); b.coins(155, 158, 10); b.coins(171, 173, 9); b.coins(176, 178, 11); b.coins(182, 189, 12);
  }},
  'Graveyard 3': { at: 200, len: 200, build(b) {
    // World 2-3, Part 3: a beam chain to a guarded lawn under a beam, a raised plot, a beam chain to the checkpoint, more
    // beams and a guarded lawn; Part 4: a beam chain to the last checkpoint, beams, a spike lawn, a raised plot, a last
    // beam chain and the last lawn to the door
    b.plat(2, 4, 11); b.ember(3, 9); b.plat(7, 9, 9); b.enemy(7, 9, 8); b.ember(10, 7); b.plat(12, 14, 11); b.ember(13, 9);
    b.ground(17, 28, 13); b.crack(17, 12); b.enemy(19, 25, 12); b.plat(21, 24, 10); b.enemy(21, 24, 9); b.ember(22, 7); b.crack(28, 12);
    b.ground(29, 32, 10); b.mush(30, 9); b.ember(31, 7);
    b.plat(35, 37, 8); b.ember(36, 6); b.plat(40, 42, 10); b.enemy(40, 42, 9); b.ember(43, 8); b.plat(45, 47, 8); b.ember(46, 6); b.ember(48, 6); b.plat(50, 52, 10);
    b.ground(55, 63, 13); b.crack(55, 12); b.check(56, 12); b.mush(58, 12); b.row(57, 62, 10, 5); b.crack(63, 12);
    b.plat(66, 68, 11); b.ember(67, 9); b.plat(71, 73, 9); b.enemy(71, 73, 8); b.ember(74, 7); b.plat(76, 78, 11); b.enemy(76, 78, 10);
    b.ground(81, 92, 14); b.crack(81, 13); b.enemy(83, 89, 13); b.ember(86, 11); b.crack(92, 13);
    b.ground(93, 97, 11); b.ember(95, 8); b.ember(98, 7);
    // Part 4
    b.plat(100, 102, 9); b.enemy(100, 102, 8); b.ember(101, 6); b.plat(105, 107, 11); b.enemy(105, 107, 10); b.ember(108, 7); b.plat(110, 112, 9); b.ember(111, 7);
    b.plat(115, 117, 11); b.ember(118, 8);
    b.ground(120, 132, 12); b.crack(120, 11); b.check(121, 11); b.mush(123, 11); b.enemy(127, 131, 11); b.ember(129, 8); b.crack(132, 11);
    b.plat(135, 137, 10); b.enemy(135, 137, 9); b.plat(140, 142, 8); b.ember(141, 6); b.plat(145, 147, 10); b.ember(146, 8);
    b.ground(150, 162, 13); b.enemy(152, 157, 12); b.plat(153, 156, 10); b.enemy(153, 156, 9); b.spikes(159, 160, 12); b.ember(159, 9); b.ember(160, 9);
    b.ember(167, 9); b.crack(162, 12);
    b.ground(163, 168, 10); b.mush(164, 9); b.ember(166, 7); b.crack(168, 9);
    b.plat(171, 173, 11); b.ember(172, 9); b.plat(176, 178, 9); b.enemy(176, 178, 8); b.plat(181, 183, 11); b.ember(182, 9);
    b.ground(186, 199, 13); b.crack(186, 12); b.enemy(188, 192, 12); b.ember(190, 9); b.enemy(193, 195, 12); b.crack(199, 12);
    b.door(196, 12);
    // Coins
    b.coins(2, 4, 10); b.coins(12, 14, 10); b.coins(19, 25, 11, 2); b.coins(32, 32, 8); b.coins(29, 29, 8); b.coins(35, 37, 7);
    b.coins(45, 47, 7); b.coins(50, 52, 9); b.coins(59, 61, 11); b.coins(66, 68, 10); b.coins(76, 78, 9); b.coins(83, 89, 12, 2);
    b.coins(93, 97, 9); b.coins(110, 112, 8); b.coins(115, 117, 10); b.coins(124, 131, 10); b.coins(140, 142, 7); b.coins(145, 147, 9);
    b.coins(152, 157, 11); b.coins(163, 167, 8); b.coins(171, 173, 10); b.coins(181, 183, 10);
  }},
  'Graveyard 4': { at: 200, len: 200, build(b) {
    // World 2-4, Part 3 (before the boss): three steps up to a high plot, a drop to a guarded lawn and the checkpoint, a
    // beam stair to a high guarded plot, a drop to a guarded lawn and a step up to the second checkpoint; Part 4: beams, a
    // guarded lawn under a beam, a climb, beams down to the last checkpoint and a guarded lawn to the arena
    b.ground(0, 7, 11); b.ember(3, 8); b.crack(7, 10);
    b.ground(8, 13, 8); b.enemy(8, 13, 7); b.ember(11, 5);
    b.ground(14, 19, 5); b.ember(16, 2); b.crack(19, 4);
    b.ground(22, 38, 14); b.crack(22, 13); b.enemy(24, 30, 13); b.plat(26, 29, 11); b.enemy(26, 29, 10); b.ember(28, 8);
    b.spikes(33, 34, 13); b.ember(33, 10); b.ember(34, 10); b.enemy(35, 37, 13); b.crack(38, 13);
    b.ground(39, 46, 12); b.check(40, 11); b.mush(42, 11); b.ember(44, 8); b.crack(46, 11);
    b.plat(49, 51, 10); b.ember(50, 8); b.plat(54, 56, 8); b.enemy(54, 56, 7); b.ember(57, 6); b.plat(59, 61, 6); b.ember(60, 4);
    b.ground(64, 74, 7); b.enemy(66, 72, 6); b.ember(69, 3); b.crack(74, 6);
    b.ground(77, 95, 14); b.crack(77, 13); b.enemy(79, 85, 13); b.mush(82, 13); b.plat(81, 84, 11); b.enemy(81, 84, 10);
    b.spikes(88, 89, 13); b.ember(88, 10); b.ember(89, 10); b.enemy(91, 95, 13); b.ember(92, 10);
    b.ground(96, 104, 11); b.check(97, 10); b.ember(101, 7); b.crack(104, 10);
    // Part 4
    b.ground(105, 110, 8); b.mush(106, 7); b.ember(108, 5);
    b.plat(113, 115, 10); b.enemy(113, 115, 9); b.plat(118, 120, 12); b.enemy(118, 120, 11); b.ember(119, 9); b.plat(123, 125, 10); b.ember(124, 8);
    b.ground(128, 142, 13); b.crack(128, 12); b.enemy(130, 136, 12); b.plat(132, 136, 10); b.enemy(133, 136, 9); b.spikes(138, 139, 12); b.ember(138, 9); b.ember(139, 9); b.crack(142, 12);
    b.ground(143, 148, 10); b.ember(145, 7);
    b.ground(149, 154, 7); b.enemy(149, 154, 6); b.ember(152, 3);
    b.plat(157, 159, 9); b.ember(158, 7); b.plat(162, 164, 11); b.enemy(162, 164, 10); b.ember(165, 8);
    b.ground(167, 180, 13); b.check(168, 12); b.mush(170, 12); b.row(172, 178, 9, 3); b.crack(180, 12);
    b.ground(181, 199, 14); b.enemy(183, 190, 13); b.ember(186, 10); b.spikes(193, 194, 13); b.ember(193, 10); b.ember(194, 10);
    // Coins
    b.coins(0, 6, 9); b.coins(8, 13, 6); b.coins(14, 18, 3); b.coins(24, 30, 12, 2); b.coins(41, 45, 10); b.coins(49, 51, 9);
    b.coins(59, 61, 5); b.coins(65, 72, 5); b.coins(79, 85, 12, 2); b.coins(97, 103, 9); b.coins(123, 125, 9); b.coins(130, 136, 11, 2);
    b.coins(143, 148, 8); b.coins(157, 159, 8); b.coins(169, 179, 11, 2); b.coins(183, 190, 12);
  }},

  // ---------- World 3 (city, mechs) ----------
  City: { at: 220, len: 220, build(b) {
    // World 3-1, Part 3: a slide to a guarded roof under a beam, a spike street, an elevator to a high roof, a drop to the
    // checkpoint and a spike street, a slide to a guarded roof; Part 4: beams round a slide, a spike street, a step to the
    // last checkpoint, an elevator to a high roof, a guarded street and a last roof to the door
    b.mover(1, 11, 3, 9, 11, 300); b.ember(5, 8); b.ember(9, 8);
    b.ground(13, 26, 12); b.crack(13, 11); b.enemy(15, 21, 11); b.plat(17, 20, 9); b.enemy(17, 20, 8); b.ember(18, 6); b.enemy(22, 25, 11); b.crack(26, 11);
    b.ground(29, 44, 14); b.spikes(31, 32, 13); b.ember(31, 10); b.ember(32, 10); b.enemy(34, 40, 13); b.ember(38, 10); b.mush(42, 13); b.crack(44, 13);
    b.mover(46, 13, 3, 46, 7, 300); b.ember(47, 4);
    b.ground(50, 64, 8); b.enemy(51, 56, 7); b.enemy(58, 63, 7); b.row(52, 62, 5, 5); b.crack(64, 7);
    b.ground(67, 82, 13); b.crack(67, 12); b.check(68, 12); b.spikes(73, 74, 12); b.ember(73, 9); b.ember(74, 9); b.enemy(76, 81, 12); b.ember(79, 9); b.crack(82, 12);
    b.mover(84, 11, 3, 92, 11, 280); b.ember(88, 8);
    b.ground(96, 110, 11); b.enemy(98, 103, 10); b.plat(100, 103, 8); b.enemy(100, 103, 7); b.ember(101, 5); b.mush(106, 10); b.crack(110, 10);
    // Part 4
    b.plat(113, 115, 9); b.ember(114, 7); b.mover(117, 9, 3, 121, 9, 260); b.ember(122, 6); b.plat(124, 126, 9); b.enemy(124, 126, 8);
    b.ground(129, 144, 14); b.crack(129, 13); b.enemy(131, 137, 13); b.spikes(139, 140, 13); b.ember(139, 10); b.ember(140, 10); b.enemy(141, 144, 13);
    b.ground(145, 152, 11); b.check(146, 10); b.mush(148, 10); b.crack(152, 10);
    b.mover(154, 11, 3, 154, 6, 300); b.ember(155, 3);
    b.ground(158, 172, 7); b.enemy(159, 165, 6); b.enemy(167, 171, 6); b.row(160, 170, 3, 5); b.crack(172, 6);
    b.ground(175, 192, 14); b.enemy(177, 182, 13); b.plat(178, 182, 11); b.enemy(179, 182, 10); b.ember(180, 8);
    b.spikes(185, 186, 13); b.ember(185, 10); b.ember(186, 10); b.enemy(188, 191, 13); b.crack(192, 13);
    b.ground(193, 219, 12); b.mush(194, 11); b.enemy(197, 203, 11); b.plat(199, 203, 9); b.enemy(199, 203, 8); b.ember(201, 6);
    b.spikes(206, 207, 11); b.ember(206, 8); b.ember(207, 8); b.enemy(209, 213, 11); b.ember(212, 8); b.crack(219, 11);
    b.door(216, 11);
    // Coins
    b.coins(2, 10, 7, 2); b.coins(14, 21, 10); b.coins(34, 40, 12, 2); b.coin(47, 10); b.coin(47, 8); b.coins(51, 63, 6, 2);
    b.coins(69, 72, 11); b.coins(85, 93, 9, 2); b.coins(97, 103, 9); b.coins(113, 115, 8); b.coins(117, 121, 8, 2); b.coins(131, 137, 12);
    b.coins(147, 151, 9); b.coin(155, 9); b.coin(155, 7); b.coins(159, 171, 5, 3); b.coins(177, 182, 12); b.coins(195, 203, 10, 2); b.coins(209, 213, 10);
  }},
  'City 2': { at: 220, len: 220, build(b) {
    // World 3-2, Part 3: an elevator to a guarded high roof, a drop to a spike street and a step up to the checkpoint,
    // girders round a slide, a guarded street under a beam and a step up; Part 4: a slide to a guarded street, a step to the
    // last checkpoint, an elevator to the highest roof, guarded girders, a spike street, a slide and the door
    b.ground(0, 6, 12); b.ember(3, 9);
    b.mover(8, 12, 3, 8, 6, 300); b.ember(9, 3);
    b.ground(12, 25, 7); b.enemy(13, 18, 6); b.enemy(20, 24, 6); b.row(14, 24, 4, 5); b.crack(25, 6);
    b.ground(28, 42, 13); b.crack(28, 12); b.spikes(31, 32, 12); b.ember(31, 9); b.ember(32, 9); b.enemy(34, 40, 12); b.ember(37, 9); b.crack(42, 12);
    b.ground(43, 50, 10); b.check(44, 9); b.mush(46, 9); b.crack(50, 9);
    b.plat(53, 55, 8); b.ember(54, 6); b.mover(58, 8, 3, 64, 8, 300); b.ember(61, 5); b.ember(66, 5); b.plat(68, 70, 8); b.enemy(68, 70, 7);
    b.ground(73, 88, 12); b.crack(73, 11); b.enemy(75, 80, 11); b.plat(77, 80, 9); b.enemy(77, 80, 8); b.ember(78, 6);
    b.spikes(83, 84, 11); b.ember(83, 8); b.ember(84, 8); b.enemy(85, 88, 11);
    b.ground(89, 96, 9); b.mush(90, 8); b.enemy(91, 95, 8); b.ember(93, 5); b.crack(96, 8);
    // Part 4
    b.mover(98, 9, 3, 106, 9, 280); b.ember(102, 6);
    b.ground(110, 125, 13); b.crack(110, 12); b.enemy(112, 118, 12); b.plat(114, 118, 10); b.enemy(115, 118, 9); b.ember(116, 7);
    b.spikes(121, 122, 12); b.ember(121, 9); b.ember(122, 9); b.enemy(123, 125, 12);
    b.ground(126, 135, 11); b.check(127, 10); b.ember(131, 7); b.crack(135, 10);
    b.mover(137, 11, 3, 137, 5, 300); b.ember(138, 2);
    b.ground(141, 154, 6); b.enemy(142, 147, 5); b.ember(145, 3); b.mush(150, 5); b.enemy(151, 153, 5); b.crack(154, 5);
    b.plat(157, 159, 8); b.enemy(157, 159, 7); b.plat(162, 164, 10); b.enemy(162, 164, 9); b.ember(163, 7);
    b.ground(167, 182, 14); b.crack(167, 13); b.enemy(169, 175, 13); b.ember(172, 10); b.spikes(178, 179, 13); b.ember(178, 10); b.ember(179, 10); b.enemy(180, 182, 13);
    b.mover(184, 12, 3, 192, 12, 280); b.ember(188, 9);
    b.ground(196, 219, 12); b.crack(196, 11); b.mush(197, 11); b.enemy(199, 204, 11); b.plat(201, 204, 9); b.enemy(201, 204, 8); b.ember(202, 6);
    b.spikes(207, 208, 11); b.ember(207, 8); b.ember(208, 8); b.enemy(210, 214, 11); b.crack(219, 11);
    b.door(216, 11);
    // Coins
    b.coins(1, 6, 10); b.coin(9, 10); b.coin(9, 7); b.coins(13, 23, 5, 2); b.coins(34, 40, 11, 2); b.coins(45, 49, 8); b.coins(53, 55, 7);
    b.coins(59, 65, 7, 2); b.coins(75, 80, 10); b.coins(89, 95, 7); b.coins(99, 107, 7, 2); b.coins(112, 118, 11); b.coins(128, 134, 9);
    b.coins(142, 149, 4); b.coins(169, 175, 12, 2); b.coins(185, 193, 10, 2); b.coins(199, 204, 10);
  }},
  'City 3': { at: 220, len: 220, build(b) {
    // World 3-3, Part 3: guarded girder hops, a slide to a guarded roof under a beam, girder hops to the checkpoint and a
    // spike roof, an elevator to the highest roof, a drop to a spike street; Part 4: girders round a slide, a guarded roof
    // under a beam to the last checkpoint, a slide, a spike street, guarded girders, a last slide and the door
    b.plat(2, 4, 10); b.ember(3, 8); b.plat(7, 9, 8); b.enemy(7, 9, 7); b.ember(10, 6); b.plat(12, 14, 10);
    b.mover(16, 10, 3, 23, 10, 280); b.ember(20, 7); b.ember(25, 7);
    b.ground(27, 40, 12); b.crack(27, 11); b.enemy(29, 34, 11); b.plat(31, 34, 9); b.enemy(31, 34, 8); b.ember(32, 6); b.enemy(36, 39, 11); b.crack(40, 11);
    b.plat(43, 45, 10); b.ember(44, 8); b.ember(47, 6); b.plat(48, 50, 8); b.enemy(48, 50, 7); b.plat(53, 55, 10); b.enemy(53, 55, 9); b.ember(54, 7);
    b.ground(58, 70, 13); b.crack(58, 12); b.check(59, 12); b.mush(61, 12); b.ember(63, 9); b.spikes(65, 66, 12); b.ember(65, 9); b.ember(66, 9); b.enemy(68, 70, 12);
    b.mover(72, 12, 3, 72, 6, 280); b.ember(73, 3);
    b.ground(76, 92, 6); b.crack(76, 5); b.enemy(77, 83, 5); b.enemy(85, 90, 5); b.row(78, 88, 2, 5); b.crack(92, 5);
    b.ground(95, 112, 13); b.crack(95, 12); b.enemy(97, 102, 12); b.ember(100, 9); b.spikes(104, 105, 12); b.ember(104, 9); b.ember(105, 9); b.enemy(107, 111, 12); b.crack(112, 12);
    // Part 4
    b.plat(115, 117, 10); b.ember(116, 8); b.mover(119, 10, 3, 125, 10, 260); b.ember(122, 7); b.plat(129, 131, 10); b.enemy(129, 131, 9);
    b.ground(134, 148, 12); b.crack(134, 11); b.check(135, 11); b.mush(137, 11); b.enemy(139, 145, 11); b.plat(141, 145, 9); b.enemy(142, 145, 8); b.ember(143, 6); b.crack(148, 11);
    b.mover(150, 11, 3, 158, 11, 280); b.ember(154, 8);
    b.ground(162, 176, 13); b.crack(162, 12); b.enemy(164, 170, 12); b.ember(167, 9); b.spikes(172, 173, 12); b.ember(172, 9); b.ember(173, 9); b.mush(175, 12);
    b.plat(179, 181, 10); b.enemy(179, 181, 9); b.plat(184, 186, 8); b.enemy(184, 186, 7); b.ember(185, 5); b.plat(189, 191, 10);
    b.mover(193, 10, 3, 198, 10, 260); b.ember(196, 7);
    b.ground(202, 219, 12); b.crack(202, 11); b.enemy(203, 208, 11); b.plat(205, 208, 9); b.enemy(205, 208, 8); b.ember(206, 6);
    b.spikes(210, 211, 11); b.ember(210, 8); b.ember(211, 8); b.enemy(212, 214, 11); b.crack(219, 11);
    b.door(216, 11);
    // Coins
    b.coins(2, 4, 9); b.coins(12, 14, 9); b.coins(17, 25, 8, 2); b.coins(29, 34, 10); b.coins(43, 45, 9); b.coins(59, 64, 11);
    b.coin(73, 9); b.coin(73, 6); b.coins(77, 89, 4, 3); b.coins(97, 102, 11); b.coins(107, 111, 11); b.coins(115, 117, 9);
    b.coins(120, 126, 8, 2); b.coins(136, 140, 10); b.coins(151, 159, 9, 2); b.coins(164, 170, 11); b.coins(189, 191, 9);
    b.coins(194, 200, 8, 2); b.coins(203, 208, 10);
  }},
  'City 4': { at: 220, len: 220, build(b) {
    // World 3-4, Part 3 (before the boss): steps up to a guarded girder and a slide to a guarded roof, a drop to a spike
    // street and the checkpoint, an elevator to a guarded high roof, a street and a step up to the second checkpoint; Part
    // 4: a rising slide and a slide to a spike street, steps up, a slide to the last checkpoint, guarded steps and a drop
    // to a guarded street before the arena
    b.ground(0, 5, 11); b.ember(2, 8); b.crack(5, 10);
    b.ground(6, 11, 8); b.enemy(6, 11, 7); b.ember(9, 5);
    b.plat(14, 16, 6); b.enemy(14, 16, 5); b.ember(15, 3); b.mover(19, 6, 3, 25, 6, 300); b.ember(22, 3);
    b.ground(29, 38, 8); b.enemy(30, 36, 7); b.ember(33, 4); b.crack(38, 7);
    b.ground(41, 56, 14); b.crack(41, 13); b.spikes(44, 45, 13); b.ember(44, 10); b.ember(45, 10); b.enemy(47, 52, 13); b.plat(48, 52, 11); b.enemy(48, 52, 10); b.ember(50, 8);
    b.check(54, 13); b.mush(55, 13); b.crack(56, 13);
    b.mover(58, 13, 3, 58, 7, 300); b.ember(59, 4);
    b.ground(62, 72, 7); b.enemy(63, 67, 6); b.enemy(68, 71, 6); b.row(64, 70, 3, 3); b.crack(72, 6);
    b.ground(75, 85, 13); b.crack(75, 12); b.enemy(76, 82, 12); b.ember(79, 9); b.crack(85, 12);
    b.ground(86, 98, 10); b.enemy(87, 92, 9); b.ember(90, 6); b.mush(94, 9); b.check(96, 9); b.crack(98, 9);
    // Part 4
    b.mover(100, 9, 3, 103, 7, 240); b.ember(102, 6); b.plat(106, 108, 10); b.enemy(106, 108, 9); b.mover(111, 10, 3, 117, 10, 280); b.ember(114, 7);
    b.ground(121, 135, 14); b.crack(121, 13); b.enemy(122, 127, 13); b.spikes(129, 130, 13); b.ember(129, 10); b.ember(130, 10); b.enemy(132, 135, 13);
    b.ground(136, 140, 11); b.mush(137, 10); b.ember(138, 8); b.crack(140, 10);
    b.ground(141, 145, 8); b.enemy(141, 144, 7); b.ember(143, 5);
    b.mover(147, 8, 3, 153, 8, 280); b.ember(150, 5);
    b.ground(157, 174, 13); b.check(158, 12); b.mush(160, 12); b.enemy(163, 169, 12); b.plat(164, 168, 10); b.enemy(164, 168, 9); b.ember(166, 7);
    b.spikes(171, 172, 12); b.ember(171, 9); b.ember(172, 9); b.crack(174, 12);
    b.ground(175, 180, 10); b.enemy(176, 180, 9); b.ember(178, 6);
    b.ground(183, 199, 13); b.enemy(185, 191, 12); b.plat(187, 191, 10); b.enemy(187, 191, 9); b.ember(189, 7); b.spikes(194, 195, 12); b.ember(194, 9); b.ember(195, 9);
    b.enemy(197, 199, 12);
    b.ground(200, 219, 14); b.enemy(203, 209, 13); b.ember(206, 10); b.ember(212, 10);
    // Coins
    b.coins(0, 4, 9); b.coins(6, 11, 6); b.coins(14, 16, 4); b.coins(20, 26, 4, 2); b.coins(30, 37, 6); b.coins(47, 52, 12);
    b.coin(59, 10); b.coin(59, 8); b.coins(63, 71, 5, 2); b.coins(76, 82, 11, 2); b.coins(87, 93, 8); b.coins(106, 108, 8);
    b.coins(112, 118, 8, 2); b.coins(122, 127, 12); b.coins(141, 145, 6); b.coins(148, 154, 6, 2); b.coins(159, 162, 11);
    b.coins(176, 180, 8); b.coins(185, 191, 11, 2);
  }},

  // ---------- World 4 (hell, beasts) ----------
  // dropDoor: as World 4-2's (user, 2026-10-17)
  'Underworld 1': { at: 352, len: 352, dropDoor: true, build(b) {
    // World 4-1, Part 5: a guarded plank stair up to a guarded step, a spike floor to the checkpoint, a climb to a high
    // floor; Part 6: a long ride, a guarded floor, a spike floor to the checkpoint, a guarded plank chain; Part 7: a spike
    // floor, an elevator to a guarded ledge with the checkpoint, a guarded step, a spike floor and a plank climb to the
    // blue mushroom; Part 8: a guarded ledge with the last checkpoint, a long ride, guarded floors, a guarded plank chain
    // and the door
    b.ground(0, 8, 14); b.row(2, 8, 12, 3);
    b.plat(11, 13, 12); b.ember(12, 10); b.plat(16, 18, 10); b.enemy(16, 18, 9); b.plat(21, 23, 8); b.ember(22, 6);
    b.ground(26, 33, 10); b.crack(26, 9); b.enemy(28, 33, 9);
    b.ground(34, 47, 14); b.mush(35, 13); b.spikes(39, 40, 13); b.ember(39, 11); b.ember(40, 11); b.enemy(43, 47, 13);
    b.ground(48, 57, 14); b.check(50, 13); b.crack(57, 13);
    b.ground(58, 61, 12); b.ember(60, 10); b.ground(62, 65, 10); b.enemy(62, 65, 9);
    b.ground(69, 87, 10); b.mush(70, 9); b.enemy(72, 79, 9); b.row(73, 79, 7, 3); b.enemy(83, 87, 9);
    // Part 6
    b.mover(89, 10, 3, 99, 10, 300); b.row(91, 99, 8, 4);
    b.ground(103, 116, 12); b.enemy(105, 113, 11); b.ember(115, 10);
    b.ground(117, 130, 14); b.crack(117, 13); b.spikes(120, 121, 13); b.ember(120, 11); b.ember(121, 11); b.enemy(124, 129, 13); b.crack(130, 13);
    b.ground(131, 138, 14); b.mush(132, 13); b.check(135, 13);
    b.plat(141, 143, 12); b.enemy(141, 143, 11); b.plat(146, 148, 10); b.ember(147, 9); b.plat(151, 153, 8); b.ember(152, 7); b.plat(156, 158, 10); b.enemy(156, 158, 9);
    b.plat(161, 163, 12);
    b.ground(166, 175, 14); b.enemy(169, 174, 13);
    // Part 7
    b.ground(176, 190, 14); b.mush(177, 13); b.spikes(179, 180, 13); b.ember(179, 11); b.ember(180, 11); b.enemy(184, 189, 13);
    b.mover(192, 13, 3, 192, 7, 300); b.ember(193, 5);
    b.ground(197, 210, 8); b.check(198, 7); b.row(199, 203, 6, 2); b.enemy(203, 209, 7);
    b.ground(211, 224, 11); b.enemy(214, 220, 10); b.crack(224, 10);
    b.ground(225, 244, 14); b.mush(226, 13); b.enemy(229, 233, 13); b.spikes(236, 237, 13); b.ember(236, 11); b.ember(237, 11); b.enemy(240, 244, 13);
    b.plat(247, 249, 12); b.ember(250, 9); b.plat(252, 254, 10); b.enemy(252, 254, 9); b.plat(257, 259, 8); b.bluemush(258, 7);
    // Part 8
    b.ground(262, 275, 9); b.check(263, 8); b.ember(266, 7); b.enemy(269, 274, 8);
    b.mover(277, 10, 3, 289, 10, 300); b.row(279, 289, 8, 5);
    b.ground(293, 304, 12); b.mush(293, 11); b.enemy(296, 303, 11);
    b.ground(305, 318, 14); b.enemy(308, 310, 13); b.spikes(313, 314, 13); b.ember(313, 11); b.ember(314, 11); b.enemy(316, 318, 13);
    b.plat(321, 323, 12); b.plat(326, 328, 10); b.enemy(326, 328, 9); b.ember(329, 9); b.plat(331, 333, 12); b.ember(332, 10);
    b.ground(336, 351, 14); b.mush(337, 13); b.enemy(340, 345, 13); b.crack(351, 13);
    b.door(348, 13);
    // Coins
    b.coins(3, 7, 13, 2); b.coins(11, 13, 11); b.coins(21, 23, 7); b.coins(58, 61, 11); b.coins(146, 148, 8); b.coins(161, 163, 11);
    b.coin(193, 10); b.coin(193, 8); b.coins(211, 213, 9); b.coins(247, 249, 11); b.coins(257, 259, 6); b.coins(294, 296, 10);
    b.coins(321, 323, 11); b.coins(331, 333, 11); b.coins(346, 347, 12);
  }},
  // dropDoor: the old door doesn't become a checkpoint, as one already stands 7 columns before it (user, 2026-10-17)
  'Underworld 2': { at: 352, len: 352, dropDoor: true, build(b) {
    // World 4-2, Part 5: guarded plank hops, a spike floor, a step to the checkpoint, a guarded plank chain and a guarded
    // floor under a guarded plank; Part 6: the checkpoint, a long ride, a guarded floor, a guarded plank chain and two spike
    // pits; Part 7: a guarded plank climb, a spike floor, an elevator to the checkpoint's ledge and the blue mushroom, a
    // guarded step; Part 8: a long ride, guarded planks, a spike floor, guarded planks and a step up to the door
    b.ground(0, 6, 14); b.row(1, 5, 12, 2);
    b.plat(9, 11, 12); b.plat(14, 16, 10); b.enemy(14, 16, 9); b.plat(19, 21, 12); b.ember(20, 10);
    b.ground(24, 38, 14); b.enemy(26, 31, 13); b.spikes(34, 35, 13); b.ember(34, 11); b.ember(35, 11); b.crack(38, 13);
    b.ground(39, 44, 11); b.check(40, 10); b.mush(42, 10);
    b.plat(47, 49, 9); b.ember(48, 7); b.plat(52, 54, 11); b.enemy(52, 54, 10); b.plat(57, 59, 9); b.ember(58, 7); b.plat(62, 64, 11); b.enemy(62, 64, 10);
    b.ground(67, 87, 14); b.mush(68, 13); b.enemy(71, 77, 13); b.plat(72, 75, 11); b.row(73, 74, 10); b.enemy(72, 75, 10); b.spikes(80, 81, 13); b.enemy(84, 87, 13);
    // Part 6
    b.ground(88, 95, 14); b.mush(89, 13); b.check(90, 13); b.ember(94, 11);
    b.mover(97, 12, 3, 106, 12, 300); b.row(99, 107, 10, 4);
    b.ground(111, 124, 12); b.crack(111, 11); b.enemy(113, 121, 11); b.crack(124, 11);
    b.plat(127, 129, 10); b.enemy(127, 129, 9); b.plat(132, 134, 8); b.ember(133, 7); b.plat(137, 139, 10); b.plat(142, 144, 12); b.enemy(142, 144, 11);
    b.ground(147, 168, 14); b.mush(148, 13); b.spikes(151, 152, 13); b.ember(151, 11); b.ember(152, 11); b.enemy(155, 161, 13);
    b.spikes(164, 165, 13); b.ember(164, 11); b.ember(165, 11);
    b.ground(169, 175, 11); b.enemy(169, 175, 10);
    // Part 7
    b.ground(176, 185, 14); b.mush(177, 13); b.check(180, 13);
    b.plat(188, 190, 12); b.plat(193, 195, 10); b.enemy(193, 195, 9); b.ember(196, 8); b.plat(198, 200, 8); b.ember(199, 7); b.plat(203, 205, 10); b.enemy(203, 205, 9);
    b.plat(208, 210, 12);
    b.ground(213, 230, 14); b.crack(213, 13); b.enemy(215, 220, 13); b.spikes(223, 224, 13); b.ember(223, 11); b.ember(224, 11); b.enemy(227, 230, 13);
    b.mover(232, 13, 3, 232, 7, 300); b.ember(233, 5);
    b.ground(237, 250, 8); b.check(238, 7); b.row(240, 246, 6, 3); b.bluemush(250, 7);
    b.ground(251, 263, 10); b.crack(252, 9); b.enemy(257, 262, 9);
    // Part 8
    b.mover(265, 11, 3, 276, 11, 300); b.row(267, 277, 9, 5);
    b.plat(281, 283, 12); b.enemy(281, 283, 11); b.plat(286, 288, 10); b.ember(287, 8); b.plat(291, 293, 12); b.enemy(291, 293, 11);
    b.ground(296, 312, 14); b.mush(297, 13); b.enemy(299, 304, 13); b.spikes(307, 308, 13); b.ember(307, 11); b.ember(308, 11); b.enemy(310, 312, 13);
    b.plat(315, 317, 12); b.ember(318, 9); b.plat(320, 322, 10); b.enemy(320, 322, 9); b.plat(325, 327, 12); b.enemy(325, 327, 11);
    b.ground(330, 339, 14); b.enemy(332, 337, 13);
    b.ground(340, 351, 12); b.mush(341, 11); b.ember(343, 9); b.ember(346, 9);
    b.door(351, 11);
    // Coins
    b.coins(9, 11, 11); b.coins(19, 21, 11); b.coins(47, 49, 8); b.coins(57, 59, 8); b.coins(98, 106, 9, 2); b.coins(132, 134, 6);
    b.coins(137, 139, 9); b.coins(188, 190, 11); b.coins(208, 210, 11); b.coin(233, 10); b.coin(233, 8); b.coins(286, 288, 9);
    b.coins(315, 317, 11); b.coins(340, 342, 10);
  }},
  // dropDoor: as World 4-2's (user, 2026-10-17)
  'Underworld 3': { at: 352, len: 352, dropDoor: true, build(b) {
    // World 4-3, Part 5: steps up to a guarded high step, a drop to a spike floor and the checkpoint, a guarded plank
    // chain and a guarded floor under a guarded plank; Part 6: the checkpoint, an elevator to a guarded high floor, a
    // guarded step, a spike floor, a guarded plank chain and the checkpoint; Part 7: a long ride, a guarded climb, a spike
    // run under a guarded plank to the last checkpoint and the blue mushroom, guarded planks; Part 8: a guarded floor under
    // a guarded plank, a long ride, a spike floor, a guarded step, planks and the door
    b.ground(0, 9, 14); b.row(2, 8, 12, 3);
    b.ground(10, 15, 12); b.enemy(10, 15, 11);
    b.ground(16, 21, 9); b.crack(16, 8); b.ember(20, 7);
    b.ground(22, 27, 6); b.enemy(22, 27, 5);
    b.ground(30, 44, 14); b.crack(30, 13); b.enemy(32, 38, 13); b.spikes(41, 42, 13); b.ember(41, 11); b.ember(42, 11);
    b.ground(45, 52, 12); b.check(46, 11); b.mush(48, 11);
    b.plat(55, 57, 10); b.enemy(55, 57, 9); b.plat(60, 62, 8); b.ember(61, 7); b.plat(65, 67, 10);
    b.ground(70, 87, 13); b.mush(71, 12); b.enemy(73, 79, 12); b.plat(74, 78, 10); b.row(75, 77, 9, 2); b.enemy(74, 78, 9); b.spikes(83, 84, 12);
    // Part 6
    b.ground(88, 95, 13); b.check(90, 12); b.ember(94, 10);
    b.mover(97, 12, 3, 97, 6, 300);
    b.ground(101, 116, 6); b.row(102, 104, 4, 2); b.enemy(106, 112, 5); b.ember(115, 4); b.crack(116, 5);
    b.ground(119, 132, 10); b.enemy(122, 127, 9); b.ember(129, 7);
    b.ground(133, 148, 14); b.mush(134, 13); b.spikes(137, 138, 13); b.ember(137, 11); b.ember(138, 11); b.enemy(141, 147, 13); b.crack(148, 13);
    b.plat(151, 153, 12); b.ember(152, 10); b.plat(156, 158, 10); b.enemy(156, 158, 9); b.plat(161, 163, 12); b.enemy(161, 163, 11);
    b.ground(166, 175, 14); b.mush(166, 13); b.check(169, 13);
    // Part 7
    b.mover(177, 12, 3, 186, 12, 320); b.row(179, 187, 10, 4);
    b.ground(191, 202, 12); b.mush(191, 11); b.enemy(193, 201, 11);
    b.ground(203, 208, 9); b.enemy(203, 208, 8); b.ground(209, 214, 6); b.ember(210, 5); b.ember(213, 5); b.crack(214, 5);
    b.ground(217, 236, 14); b.spikes(220, 221, 13); b.ember(220, 11); b.ember(221, 11); b.enemy(224, 229, 13); b.plat(225, 228, 11); b.row(226, 227, 10);
    b.enemy(225, 228, 10); b.spikes(232, 233, 13); b.enemy(234, 236, 13);
    b.ground(237, 248, 11); b.check(238, 10); b.bluemush(247, 10);
    b.plat(251, 253, 9); b.enemy(251, 253, 8); b.plat(256, 258, 7); b.ember(257, 6); b.plat(261, 263, 9); b.enemy(261, 263, 8);
    // Part 8
    b.ground(266, 280, 12); b.enemy(268, 274, 11); b.plat(269, 273, 9); b.row(270, 272, 8, 2); b.enemy(269, 273, 8);
    b.mover(282, 12, 3, 294, 12, 300); b.row(285, 293, 10, 4);
    b.ground(298, 310, 14); b.mush(299, 13); b.enemy(300, 304, 13); b.spikes(306, 307, 13); b.ember(305, 13); b.ember(311, 10);
    b.ground(311, 316, 11); b.enemy(312, 316, 10);
    b.plat(319, 321, 9); b.ember(320, 8); b.plat(324, 326, 11); b.enemy(324, 326, 10);
    b.ground(329, 351, 14); b.enemy(331, 336, 13); b.ember(337, 11); b.mush(342, 13); b.crack(350, 13);
    b.door(347, 13);
    // Coins
    b.coins(3, 7, 13, 2); b.coins(17, 19, 7); b.coins(22, 26, 4); b.coins(55, 57, 8); b.coins(65, 67, 9); b.coin(98, 10); b.coin(98, 8);
    b.coins(119, 121, 8); b.coins(151, 153, 11); b.coins(209, 213, 4); b.coins(256, 258, 5); b.coins(319, 321, 7); b.coins(343, 346, 13);
  }},
  Hell: { at: 342, len: 342, build(b) {
    // World 4-4, the run-up to the Fire Skull, Parts 5 to 8 (before the blue mushroom, the boss checkpoint and the arena):
    // guarded steps, a spike floor, the checkpoint, a guarded plank chain and a guarded spike floor; a ride to a guarded
    // floor, a spike floor, a guarded plank chain and a guarded floor under a guarded plank; the checkpoint, a guarded plank
    // stair up to a guarded high floor, a guarded step, a spike run between beasts and an elevator to the checkpoint and
    // the blue mushroom; a guarded step, a long ride, a guarded floor, a spike floor and guarded planks
    b.ground(0, 10, 14); b.row(1, 5, 12, 2); b.enemy(7, 10, 13);
    b.ground(11, 15, 12); b.enemy(11, 15, 11);
    b.ground(16, 20, 10); b.enemy(16, 20, 9); b.ember(22, 8); b.ember(23, 10);
    b.ground(24, 38, 14); b.crack(24, 13); b.enemy(26, 32, 13); b.plat(27, 30, 11); b.row(28, 29, 10); b.spikes(35, 36, 13); b.ember(35, 11); b.ember(36, 11);
    b.ground(39, 48, 14); b.mush(40, 13); b.check(44, 13); b.crack(48, 13);
    b.plat(51, 53, 12); b.enemy(51, 53, 11); b.plat(56, 58, 10); b.enemy(56, 58, 9); b.ember(59, 8); b.plat(61, 63, 12); b.enemy(61, 63, 11);
    b.ground(66, 85, 14); b.mush(67, 13); b.enemy(69, 75, 13); b.row(70, 74, 11, 2); b.spikes(77, 78, 13); b.enemy(80, 85, 13);
    // Part 6
    b.ground(86, 93, 14); b.check(88, 13); b.mush(91, 13);
    b.mover(95, 12, 3, 103, 12, 300);
    b.ground(105, 118, 10); b.row(106, 110, 7, 2); b.enemy(107, 116, 9);
    b.ground(119, 130, 14); b.crack(119, 13); b.spikes(122, 123, 13); b.ember(122, 11); b.ember(123, 11); b.enemy(126, 130, 13);
    b.plat(133, 135, 12); b.enemy(133, 135, 11); b.plat(138, 140, 10); b.ember(139, 9); b.plat(143, 145, 12); b.enemy(143, 145, 11);
    b.ground(148, 171, 14); b.mush(149, 13); b.enemy(152, 158, 13); b.plat(160, 163, 11); b.row(161, 162, 10); b.enemy(160, 163, 10);
    b.spikes(164, 165, 13); b.enemy(167, 171, 13);
    // Part 7
    b.ground(172, 180, 14); b.mush(173, 13); b.check(174, 13);
    b.plat(183, 185, 12); b.enemy(183, 185, 11); b.plat(188, 190, 10); b.enemy(188, 190, 9); b.plat(193, 195, 8); b.enemy(193, 195, 7); b.ember(196, 6); b.plat(198, 200, 6); b.ember(199, 5);
    b.ground(203, 216, 6); b.enemy(206, 212, 5); b.crack(216, 5);
    b.ground(219, 228, 9); b.enemy(221, 227, 8);
    b.ground(229, 248, 14); b.crack(229, 13); b.mush(230, 13); b.spikes(233, 234, 13); b.ember(233, 11); b.ember(234, 11); b.enemy(237, 239, 13);
    b.spikes(242, 243, 13); b.ember(242, 11); b.ember(243, 11); b.enemy(246, 248, 13);
    b.mover(250, 13, 3, 250, 8, 300);
    // Part 8
    b.ground(254, 268, 9); b.check(255, 8); b.row(257, 261, 7, 2); b.bluemush(265, 8);
    b.ground(269, 280, 11); b.enemy(272, 279, 10);
    b.mover(282, 11, 3, 293, 11, 300); b.row(284, 292, 9, 4);
    b.ground(297, 308, 13); b.mush(297, 12); b.enemy(300, 307, 12);
    b.ground(309, 324, 14); b.enemy(311, 313, 13); b.spikes(316, 317, 13); b.ember(316, 11); b.ember(317, 11); b.enemy(320, 324, 13);
    b.plat(327, 329, 12); b.enemy(327, 329, 11); b.plat(332, 334, 10); b.enemy(332, 334, 9); b.ember(335, 8);
    b.ground(337, 341, 14); b.enemy(338, 341, 13);
    // Coins
    b.coins(1, 5, 13, 2); b.coins(27, 30, 12); b.coins(54, 55, 9); b.coins(96, 104, 10, 2); b.coins(138, 140, 8); b.coins(193, 195, 6);
    b.coins(198, 200, 4); b.coins(203, 205, 4); b.coins(219, 220, 7); b.coin(251, 11); b.coin(251, 9); b.coins(262, 264, 7);
    b.coins(269, 271, 9); b.coins(298, 299, 11); b.coins(335, 336, 10);
  }},
};
