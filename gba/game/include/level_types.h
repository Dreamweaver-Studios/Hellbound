#ifndef LEVEL_TYPES_H
#define LEVEL_TYPES_H

#include <stdint.h>

// Shapes of the generated level data (world pixels, origin at the level's top-left)
struct point { int x; int y; };
struct mush_def { int x; int y; bool blue; };
struct enemy_def { int x; int y; int w; int h; int a; int b; int kind; int modes; };  // a..b = patrol range of e.x; kind: -1 = the level's foe, else a FOE_ (main.cpp); modes: bit 1 Normal, 2 Hard, 4 Expert
struct mover_def { int x0; int y0; int x1; int y1; int w; int period; };
struct check_def { int x; int y; int row; };

// Everything about one level. foe: 0 alien, 1 reaper, 2 hell beast. theme: 0 jungle, 1 graveyard, 2 hell (backdrops,
// music, darkness; the GBA-only World 1-2 to 1-4 are jungle). A boss level has no door until the boss dies.
struct level_info
{
    const char* name;
    int width, bg_width, foe, theme;
    const uint8_t* grid;
    const point* coins; int coins_n;
    const point* embers; int embers_n;
    const mush_def* mushrooms; int mushrooms_n;
    const point* oneup_blocks; int oneup_blocks_n;
    const enemy_def* enemies; int enemies_n;
    const mover_def* movers; int movers_n;
    const check_def* checks; int checks_n;
    point start;
    bool has_door; point door;
    bool has_boss; int boss_gate; point boss_door;
    int block_coins;   // coins in each cracked block
    point star_block;  // the cracked block holding the level's star (tiles; one per level, 2026-10-18)
    point flame_item;  // where the flamethrower pickup stands (tiles), or {-1, -1}: World 1-1 only (2026-10-18)
    point rocket_item; // where the rocket launcher pickup stands (tiles), or {-1, -1}: World 1-2 only (2026-10-19)
    int enemies_on[3];     // how many of enemies[] each difficulty has (Normal, Hard, Expert; enemy_def::modes, 2026-10-24)
    int hard_skip_check;   // the checkpoint (index into checks[]) Hard leaves out, or -1 (2026-10-23)
    uint32_t hard_skip_mushrooms;   // bit i: Hard leaves out mushrooms[i] (2026-10-23)
    int hard_extra_mushrooms;       // the last this many of mushrooms[] are Hard's own (a blue one in Worlds 1-3)
    bool hard_no_star;              // Hard leaves out star_block (World 4-1, 2026-10-24)
    uint32_t expert_skip_mushrooms; // bit i: Expert leaves out mushrooms[i] (Hard's and one more red, 2026-10-24)
    bool expert_no_rocket;          // Expert leaves out rocket_item (World 3-2, 2026-10-24)
    bool expert_no_star;            // Expert leaves out star_block (Worlds 2-3 and 3-4, 2026-10-24)
    point expert_star;              // Expert's own star block in place of star_block, or {-1, -1} (World 4-1, 2026-10-25)
};

#endif
