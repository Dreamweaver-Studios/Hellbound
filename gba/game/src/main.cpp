// Hellbound (until 2026-10-29 "Super Mario Hellbound") - Game Boy Advance port (all three levels: Jungle, Graveyard, Hell and the Fire Skull).
// A line-by-line port of the web game's update code (updatePlayer, updateWorld, updateBullets, updateFoes, updateBoss, ...):
// one world pixel is one GBA pixel, so every physics constant is unchanged. The GBA screen (240x160) shows
// 15x10 tiles, so the camera also follows Mario vertically.

#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_fixed.h"
#include "bn_math.h"
#include "bn_random.h"
#include "bn_optional.h"
#include "bn_vector.h"
#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_affine_mat_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_memory.h"
#include "bn_camera_ptr.h"
#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_sprite_text_generator.h"
#include "bn_sram.h"
#include "bn_sound_items.h"
#include "bn_sound_handle.h"
#include "bn_music.h"
#include "bn_music_items.h"
#include "bn_blending.h"
#include "bn_sprites.h"

#include "bn_sprite_items_hero.h"
#include "bn_sprite_items_hero_flame.h"
#include "bn_sprite_items_hero_rocket.h"
#include "bn_sprite_items_hero_grenade.h"
#include "bn_sprite_items_hero_prone.h"
#include "bn_sprite_items_hero_flame_prone.h"
#include "bn_sprite_items_hero_rocket_prone.h"
#include "hero_frames.h"
#include "bn_sprite_items_alien.h"
#include "bn_sprite_palette_items_alien_flash.h"
#include "bn_sprite_items_reaper.h"
#include "bn_sprite_palette_items_reaper_flash.h"
#include "bn_sprite_items_demon.h"
#include "bn_sprite_palette_items_demon_flash.h"
#include "bn_sprite_items_mech.h"
#include "bn_sprite_palette_items_mech_flash.h"
#include "bn_sprite_items_beast.h"
#include "bn_sprite_palette_items_beast_flash.h"
#include "bn_sprite_items_fireball.h"
#include "bn_sprite_items_skull.h"
#include "bn_sprite_palette_items_skull_flash.h"
#include "bn_sprite_items_wasp.h"
#include "bn_sprite_palette_items_wasp_flash.h"
#include "bn_sprite_items_eye_demon.h"
#include "bn_sprite_palette_items_eye_demon_flash.h"
#include "bn_sprite_items_mech_walker.h"
#include "bn_sprite_palette_items_mech_walker_flash.h"
#include "bn_sprite_items_coin.h"
#include "bn_sprite_items_oneup.h"
#include "bn_sprite_items_star.h"
#include "bn_sprite_items_flame_beam.h"
#include "bn_sprite_items_flame_pickup.h"
#include "bn_sprite_items_rocket.h"
#include "bn_sprite_items_rocket_pickup.h"
#include "bn_sprite_items_boom.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_items_ember.h"
#include "bn_sprite_items_mush.h"
#include "bn_sprite_items_bullet.h"
#include "bn_sprite_items_glob.h"
#include "bn_sprite_items_muzzle.h"
#include "bn_sprite_items_bullet_up.h"
#include "bn_sprite_items_rocket_up.h"
#include "bn_sprite_items_bullet_diag.h"
#include "bn_sprite_items_rocket_diag.h"
#include "bn_sprite_items_grenade.h"
#include "bn_sprite_items_flame_diag.h"
#include "bn_sprite_items_hp.h"
#include "bn_sprite_items_bossbar.h"
#include "bn_sprite_items_checkpoint.h"
#include "bn_sprite_items_door.h"
#include "bn_sprite_items_platform.h"
#include "bn_sprite_items_gate.h"
#include "bn_sprite_items_crack.h"
#include "bn_sprite_items_particle.h"
#include "bn_regular_bg_items_jungle_level.h"
#include "bn_regular_bg_items_jungle_far.h"
#include "bn_regular_bg_items_jungle_mid.h"
#include "bn_regular_bg_items_jungle_near.h"
#include "bn_regular_bg_items_grave_level.h"
#include "bn_regular_bg_items_grave_far.h"
#include "bn_regular_bg_items_grave_near.h"
#include "bn_regular_bg_items_hell_level.h"
#include "bn_regular_bg_items_hell_far.h"
#include "bn_regular_bg_items_hell_mid.h"
#include "bn_regular_bg_items_hell_near.h"
#include "bn_regular_bg_items_city_far.h"
#include "bn_regular_bg_items_city_mid.h"
#include "bn_regular_bg_items_city_near.h"
#include "bn_regular_bg_items_hell_arena.h"
#include "bn_regular_bg_items_jungle4_arena.h"
#include "bn_regular_bg_items_grave4_arena.h"
#include "bn_regular_bg_items_city4_arena.h"
#include "bn_regular_bg_items_dw_logo.h"
#include "bn_regular_bg_items_title_art.h"

#include "common_variable_8x8_sprite_font.h"
#include "common_variable_8x16_sprite_font.h"

#include "dmg_sfx.h"
#include "level_data.h"
#include "level_bgs.h"
#include "sfx_volumes.h"

namespace
{

using bn::fixed;

// ---------- Constants (the web game's values) ----------
// The hero's hitbox: PH standing, CH crouching, PRONE_H lying prone (the rifle's crouch; user, 2026-10-29). 2026-10-29 (user): first 30% taller to match the taller sprite (were 24
// and 18), then standing = the standing sprite's full 36 px (taller than two tiles; every level still checks out)
constexpr int WEB_PH = 24;   // the web game's player height, which the level data's start positions are for
constexpr int T = 16, ROWS = 17, PW = 26, PH = 40, CH = 23, PRONE_H = 23, MAX_HP = 5, MAX_LIVES = 5;
constexpr int COINS_PER_LIFE = 100;   // (the coins in a cracked block are per level: level_info::block_coins)
constexpr int SCREEN_W = 240, SCREEN_H = 160;
// SPIKE_DMG: 1 since 2026-10-17 (user; the web game takes 2)
constexpr int SPIKE_DMG = 1, MUSH_HEAL = 2, BLUE_MUSH_HEAL = 4, HURT_INV = 50, FOE_RANGE = 220, FOE_WINDUP = 20;
// Hits each kind of enemy takes (user, 2026-10-17: reapers 1 -> 3, mech units 3 -> 4, hell beasts 6 -> 5)
constexpr int ALIEN_HP = 4, REAPER_HP = 4, MECH_UNIT_HP = 5;   // 3, 3, 4 until 2026-10-19 (user)
// BEAST_HP: 6, 5 (2026-10-17), 6 (2026-10-19), 5 again since 2026-10-21 (user)
// Hard (user, 2026-10-23): one more each
constexpr int HARD_ALIEN_HP = 5, HARD_REAPER_HP = 5, HARD_MECH_UNIT_HP = 6, HARD_BEAST_HP = 6;
constexpr int EXPERT_REAPER_HP = 6;   // Expert: World 2's reapers take one more than Hard (user, 2026-10-24); others as Hard
constexpr int BEAST_HP = 5, BEAST_RANGE = 240, RISE_END = 11 * 5, RISE_RANGE = 170, DEATH_SINK = 33, BEAST_BURN_END = 6 * 6 + 30;
constexpr int BOSS_HP = 80, BOSS_MUSH_EVERY = 600, BOSS_MUSH_MAX = 3;
constexpr int EXPERT_EYE_MUSH_EVERY = 1200;   // the Eye Demon's fight (World 2-4) on Expert: one every 20 s (user, 2026-10-24)
// A rocket launcher falls into the Fire Skull's arena once it's down to a quarter of its health (user requests, 2026-10-19:
// a fifth at first)
constexpr int BOSS_ROCKET_AT = 4;   // drops when hp * 4 <= max hp (20 of 80)
// A flying demon joins the Fire Skull's fight every BOSS_DEMON_EVERY frames (7 s), at most BOSS_DEMON_MAX alive at a time
// (user, 2026-10-23). They always chase Mario, and the ones left vanish when the skull dies or Mario does.
constexpr int BOSS_DEMON_EVERY = 420, BOSS_DEMON_MAX = 2;   // max 3 until later on 2026-10-23 (user)
// The enemy list holds a level's enemies on one difficulty plus the Fire Skull's demons (72 until Hard's extras, 2026-10-23;
// 80 until Expert's World 4 extras, 2026-10-24: World 4-4 on Expert is 81 + 2)
constexpr int MAX_ENEMIES = 88;
constexpr int WASP_HP = 25, EYE_HP = 50, MECH_HP = 65;   // World 1-4's, 2-4's and 3-4's bosses (the user's calls, 2026-10-16; raised 2026-10-19 and 2026-10-23)
// Hard (user, 2026-10-23; eye, mech and skull raised 2026-10-24): the wasp, eye demon, mech walker and Fire Skull
constexpr int HARD_WASP_HP = 35, HARD_EYE_HP = 70, HARD_MECH_HP = 85, HARD_BOSS_HP = 110;
constexpr int EXPERT_WASP_HP = 45, EXPERT_EYE_HP = 80, EXPERT_MECH_HP = 95, EXPERT_BOSS_HP = 130;   // Expert (user, 2026-10-24)
constexpr double EYE_HARD_SPEED = 1.2;   // Hard: the eye demon floats and dives 20% faster (user, 2026-10-24)
constexpr int BOSS_SKULL = 0, BOSS_WASP = 1, BOSS_EYE = 2, BOSS_MECH = 3;
constexpr int SCORE_COIN = 10, SCORE_EMBER = 30, SCORE_ONEUP = 100, SCORE_ENEMY = 200, SCORE_BEAST = 300;
// The star (user request, 2026-10-18): 10 s of invincibility, flashing, and enemies die at a touch
constexpr int STAR_TIME = 780, STAR_WARN = 120;   // frames (13 s; 10 s until the user added 3 s, 2026-10-20); for the last 2 s Mario flashes slower, as a warning
constexpr int STAR_TIME_HELL = STAR_TIME + 180;   // World 4-4 only: 3 s longer (user request, 2026-10-20)
// The flamethrower (user requests, 2026-10-18; World 1-1's pickup): for 15 s, holding B shoots one continuous flame
// instead of bullets, 40% of the screen's width (96 px; it was 60% until the user had it shortened) from the muzzle, stopped by walls. Everything in it takes
// FLAME_DMG each time the gun would have fired (FIRE_DELAY), double the gun's damage. Drawn as FLAME_PIECES 16x16 pieces.
constexpr int FLAME_TIME = 1200, FLAME_DMG = 2, FLAME_RANGE = 96, FLAME_PIECES = FLAME_RANGE / 16;
// The rocket launcher (user requests, 2026-10-19; World 1-2's pickup): for 20 s, B fires rockets instead of bullets, one
// every 0.7 s (ROCKET_DELAY; one a second until the user sped it up). Each does ROCKET_DMG (6; it was 2, then 5), and explodes on whatever it hits (enemy, boss, spit, wall,
// cracked block, spikes) in a 64x64 fireball. Spikes it hits are blown away, SPIKE_BLAST tiles each side of the hit.
// Rockets are 21x10 (32x16 until the user had them made ~35% smaller) and hit with their nose, ROCKET_NOSE px ahead of their centre.
constexpr int SPIKE_BLAST = 2;
constexpr int ROCKET_SPLASH = 32, ROCKET_SPLASH_DMG = 3;   // the blast's reach (px, the fireball's radius) and damage to enemies in it
// On Hard one every 0.5 s instead, on Expert every 0.5 s too (HARD_/EXPERT_ROCKET_DELAY; user, 2026-10-24; Expert was 0.3 s until 2026-10-25).
constexpr int ROCKET_TIME = 1200, ROCKET_DMG = 6, ROCKET_DELAY = 42, HARD_ROCKET_DELAY = 30, EXPERT_ROCKET_DELAY = 30,
    ROCKET_NOSE = 9, BOOM_FRAMES = 6, BOOM_STEP = 4;
const fixed ROCKET_SPEED = fixed(5);
// Grenades: GRENADE_START of them through World 1, and a fresh GRENADE_START through each of Worlds 2, 3 and 4, on every
// difficulty (user, 2026-10-29; World 4 was Expert-only before), and on
// Expert again (user request, 2026-10-25): a card after the "WORLD 4" card says Mario found some, and he has
// GRENADE_START of them from World 4-1 on (they carry over to the next levels; a restart or a game-over retry gives back
// the ones that attempt threw). R throws one in an arc the way he faces (higher when aiming up), one every GRENADE_DELAY
// frames; it goes off on whatever it touches (floor, wall, plank from above, block, spikes, enemy, boss) or after
// GRENADE_FUSE frames, in the rocket's fireball: GRENADE_DMG to every enemy, and the boss, within GRENADE_RADIUS px, and
// it breaks the cracked blocks beside it. It doesn't hurt Mario.
// (On Expert: EXPERT_GRENADES in Worlds 1-3, user 2026-10-30, and EXPERT_W4_GRENADES in World 4, user 2026-10-29)
// He throws them by hand (user, 2026-10-29: the "grenade animation" art, replacing the launcher): R starts the 3-frame
// animation (THROW_STEP frames each), the grenade leaves his hand as the last frame (arm out) comes up, which then holds
// until THROW_END
constexpr int THROW_STEP = 4, THROW_RELEASE = (HERO_THROW_N - 1) * THROW_STEP + 1, THROW_END = 24;
constexpr int GRENADE_START = 5, EXPERT_GRENADES = 5, EXPERT_W4_GRENADES = 8, GRENADE_DELAY = 30, GRENADE_FUSE = 150, GRENADE_DMG = 8, GRENADE_RADIUS = 40;
const fixed GRENADE_GRAVITY = fixed(0.2);
constexpr int SCORE_FLAWLESS = 1000, SCORE_UNTOUCHED = 5000, SCORE_ALL_EMBERS = 500;   // 200 and 500 until 2026-10-23 (user)
constexpr int SCORE_ALL_ENEMIES = 1000;   // Hard only: every enemy in the level defeated (user, 2026-10-24)
constexpr int FOE_ALIEN = 0, FOE_REAPER = 1, FOE_BEAST = 2, FOE_MECH = 3;   // mechs (World 3) act like aliens, with one more HP
constexpr fixed FOE_FIRE_H = 24.5;   // World 1's aliens and World 2's reapers fire from the height of the hero's rifle (user, 2026-10-31; was 17, 17.5)
// Flying demons (user request, 2026-10-20): Worlds 2-1 to 2-4 have 8-10 besides their reapers (enemy_def.kind, from
// levels.js FLYERS). They fly back and forth, bobbing; they don't shoot (user request, same day) but fly straight at
// Mario when he's within DEMON_RANGE px, up to DEMON_LEASH px past their patrol, and only touching them hurts.
constexpr int FOE_DEMON = 4, DEMON_HP = 2, DEMON_STOMP_DMG = 2, DEMON_RANGE = 120, DEMON_LEASH = 128;
constexpr int DEMON_BACK_OFF = 45;   // frames a demon flies away from Mario after touching him, before it comes at him again
constexpr fixed DEMON_SPEED = fixed(1.3);   // px a frame when chasing (Mario runs at 2.3, so he can get away)
const fixed FIRE_DELAY = fixed(14) / fixed(1.69);   // two 30% speed-ups on the original 14 frames
const fixed BEAST_SHOT_SPEED = fixed(3.3);
const fixed EPS = fixed::from_data(1);               // the web game's "- 0.01" nudge at tile edges
constexpr int MAX_W = max_level_width;               // widest level (level_data.h)

// Particle colours (indices into the particle sprite sheet)
enum pc { P_DUST, P_EMBER, P_GOLD, P_FLAME, P_COIN, P_PALE, P_STONE, P_STONE_DARK, P_ALIEN, P_ALIEN_LIGHT, P_ALIEN_RED,
          P_WHITE, P_RED, P_TEAL, P_GREEN };

// ---------- Small helpers ----------
bn::random rng;

int tdiv(fixed v) { return v.floor_integer() >> 4; }   // world px -> tile, flooring negatives

fixed sin_deg(fixed d)
{
    int whole = d.floor_integer() % 360;
    if(whole < 0) whole += 360;
    return bn::degrees_lut_sin(fixed(whole) + (d - d.floor_integer()));
}

fixed cos_deg(fixed d) { return sin_deg(d + 90); }

// Direction in degrees (0 = right, 90 = down) from a vector
fixed angle_of(fixed dx, fixed dy) { return bn::degrees_atan2(dy.round_integer(), dx.round_integer()); }

struct rect { fixed x, y, w, h; };
bool overlap(const rect& a, const rect& b) { return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y; }

// ---------- Sound ----------
namespace sfx
{
    using dmg_sfx::wave;
    fixed vol(int permille) { return bn::min(fixed(1), fixed(permille) / 1000); }

    // Butano stops the game ("No more sound handles available") if more sound effects play at once than it allows
    // (10, set by BN_CFG_AUDIO_MAX_SOUND_CHANNELS in the Makefile; 8 until the flamethrower's sound took two of its own). Grabbing a row of coins while jumping and firing can
    // get there, so every sample goes through here: it reuses a finished slot, or cuts off the oldest sound still playing.
    // One slot is kept spare, because a sound stopped this frame still counts until the next bn::core::update().
    constexpr int slots = 7;
    BN_DATA_EWRAM bn::optional<bn::sound_handle> handles[slots];
    int started[slots];
    int plays = 0;
    void play(const bn::sound_item& item, fixed volume)
    {
#ifdef SMHB_NO_SFX
        return;   // diagnostic ROM (build.ps1 -Variant): music only
#endif
        int slot = -1, oldest = 0;
        for(int i = 0; i < slots; ++i)
        {
            if(! handles[i] || ! handles[i]->active()) { slot = i; break; }
            if(started[i] < started[oldest]) oldest = i;
        }
        if(slot < 0) { slot = oldest; handles[slot]->stop(); }
        // Every sample at 3/4, at the user's request (2026-10-16), and then doubled: the mixer's output is played at half
        // strength (see main()), so mixing at double level keeps the loudness with half the 8-bit quantisation noise
        handles[slot] = item.play(volume * 3 / 2);
        started[slot] = ++plays;
    }

    // 3x the web mix at the user's request, then 20% louder (user, 2026-10-17): 3.6x
    void jump() { play(bn::sound_items::jump, vol(sfx_volume_jump * 655 / 100)); }   // 3.6x until 2026-10-31 (user: +30%, then +40%)
    // The gunshot would need 1.5x full volume after doubling, so its sample is made 1.5x hotter (limited) and it plays at
    // 2/3 of its old setting (1.0 after play()'s 3/2)
    void shoot() { play(bn::sound_items::shoot, vol(sfx_volume_shoot * 2 / 3)); }
    void coin() { play(bn::sound_items::coin, vol(sfx_volume_coin * 3)); }   // 3x the web mix, at the user's request
    void die() { play(bn::sound_items::death, vol(sfx_volume_death)); }
    void over() { play(bn::sound_items::game_over, vol(sfx_volume_game_over)); }
    void course_clear() { play(bn::sound_items::course_clear, vol(sfx_volume_course_clear)); }
    // The rest are the web game's synth tones: tone(freq, seconds -> frames, type, slide-to freq, gain x 1000, delay frames).
    // The bullet-impact "ping" and the landing thud were removed at the user's request.
    void hurt() { dmg_sfx::tone(420, 10, wave::square, 140, 60); dmg_sfx::tone(300, 7, wave::square, 120, 40, 4); }
    void oneup() { const int f[] = { 523, 659, 784, 1047, 1319 }; for(int i = 0; i < 5; ++i) dmg_sfx::tone(f[i], 7, wave::square, 0, 40, i * 5); }
    void crack() { dmg_sfx::tone(180, 10, wave::sawtooth, 60, 60); dmg_sfx::tone(90, 12, wave::square, 40, 40); }
    void spit() { dmg_sfx::tone(640, 7, wave::sawtooth, 260, 35); }
    void rise() { dmg_sfx::tone(70, 30, wave::sawtooth, 45, 50); dmg_sfx::tone(110, 21, wave::triangle, 60, 30, 9); }
    void drop() { const int f[] = { 660, 880, 1100 }; for(int i = 0; i < 3; ++i) dmg_sfx::tone(f[i], 5, wave::triangle, 0, 35, i * 4); }
    void fire() { dmg_sfx::tone(260, 12, wave::sawtooth, 90, 40); dmg_sfx::tone(130, 14, wave::square, 60, 25); }
    void burn() { dmg_sfx::tone(160, 30, wave::sawtooth, 40, 50); dmg_sfx::tone(90, 24, wave::square, 30, 35, 7); }
    void star_out() { const int f[] = { 392, 494, 587, 784 }; for(int i = 0; i < 4; ++i) dmg_sfx::tone(f[i], 4, wave::square, 0, 35, i * 3); }
    void star() { const int f[] = { 523, 784, 1047, 1568 }; for(int i = 0; i < 4; ++i) dmg_sfx::tone(f[i], 6, wave::square, 0, 45, i * 4); }
    void flame_get() { const int f[] = { 262, 330, 392, 523, 659 }; for(int i = 0; i < 5; ++i) dmg_sfx::tone(f[i], 5, wave::sawtooth, 0, 40, i * 3); }
    void flame_out() { dmg_sfx::tone(300, 14, wave::sawtooth, 80, 40); }
    void rocket_get() { const int f[] = { 196, 262, 330, 392, 523 }; for(int i = 0; i < 5; ++i) dmg_sfx::tone(f[i], 5, wave::square, 0, 40, i * 3); }
    void rocket_out() { dmg_sfx::tone(240, 14, wave::square, 70, 40); }
    void launch() { dmg_sfx::tone(180, 10, wave::sawtooth, 520, 35); }   // a rising whoosh as a rocket leaves (no sample given)
    // The user's explosion sample, 1.5x hot like the gunshot and played the same way (1.0 after play()'s 3/2)
    void rocket_hit() { play(bn::sound_items::rocket_hit, vol(sfx_volume_rocket_hit * 2 / 3)); }
    // The flame burning something (user request, 2026-10-21). The flame hits every FIRE_DELAY frames, so the crackle
    // isn't restarted while the last one is still playing (it would also fill every slot)
    int crackle_slot = -1, crackle_id = 0;
    void fire_crackle()
    {
        if(crackle_slot >= 0 && handles[crackle_slot] && handles[crackle_slot]->active() && started[crackle_slot] == crackle_id) return;
        play(bn::sound_items::fire_crackle, vol(sfx_volume_fire_crackle * 2 / 3));   // full volume after play()'s 3/2
        crackle_id = plays;
        for(int i = 0; i < slots; ++i) if(started[i] == plays) crackle_slot = i;
    }
    void mush() { const int f[] = { 392, 523, 659, 784, 1047 }; for(int i = 0; i < 5; ++i) dmg_sfx::tone(f[i], 5, wave::square, 0, 35, i * 3); }
    // Quarter volume (the wave channel's quietest), at the user's request
    void ember() { dmg_sfx::tone(880, 4, wave::triangle, 1320, 12); dmg_sfx::tone(1320, 6, wave::triangle, 1760, 12, 3); }
    void stomp() { dmg_sfx::tone(220, 8, wave::square, 70, 60); }
    void check() { dmg_sfx::tone(440, 12, wave::triangle, 880, 60); dmg_sfx::tone(660, 15, wave::triangle, 1320, 50, 5); }
    void clear() { const int f[] = { 523, 659, 784, 1047 }; for(int i = 0; i < 4; ++i) dmg_sfx::tone(f[i], 12, wave::triangle, 0, 60, i * 7); }
    void pop() { dmg_sfx::tone(900 + rng.get_int(600), 11, wave::triangle, 200, 20); }
    void letter() { dmg_sfx::tone(660, 3, wave::square, 0, 30); }   // a letter changing on the initials screen
}

// ---------- World state ----------
// The big containers and state live in EWRAM (BN_DATA_EWRAM): in IWRAM (32KB, shared with the stack) they left so little
// room that deep call chains, such as collecting a 1-up, overflowed the stack into Mario's state and the score.

// A world sprite that exists only while near the screen, remembering its frame and palette so they're only set on change
struct wspr
{
    bn::optional<bn::sprite_ptr> s;
    int gfx = -1, pal = -1;
    int mode = 0;   // 0 normal (on the camera), 1 zoomed, 2 zoomed and flipped
    void reset() { s.reset(); gfx = -1; pal = -1; mode = 0; }
};

struct coin_t { fixed x, y, vx, vy; bool rest, got; int t, age; wspr spr; };
struct ember_t { fixed x, y; bool got; int t; wspr spr; };
struct mush_t { fixed x, y, vy; bool blue, got, fall, boss; int t; wspr spr; };
struct oneup_t { fixed x, y, vx, vy; bool rest, got; int t, age, block; wspr spr; };
struct crack_t { int tx, ty, oneup; wspr spr; };   // oneup: index into the level's oneup_blocks, -1 (coins) or CRACK_STAR
constexpr int CRACK_STAR = -2;                       // the level's star block (level_info::star_block)
struct flame_item_t { fixed x, y, vy; bool live = false, fall = false; int t = 0; wspr spr; };   // x, y: the pickup's centre; fall: dropping into a boss fight
struct star_t { fixed x, y, vy; bool live = false, rest = false, got = false; int t = 0, age = 0; wspr spr; };
struct enemy_t
{
    fixed x, y, vx; int w, h; fixed a, b;
    fixed y0;   // a flying demon's height, which it bobs about
    int kind; bool alive, alert; int squish, dir, cool, windup, hp, hit, t, rise, volley;
    bool minion = false;   // a flying demon the Fire Skull's fight sent in (after the level's own enemies)
    wspr spr, spr2;   // hell beasts are two sprites tall
};
struct mover_t { fixed x0, y0, x1, y1, x, y, dx, dy; int w, period, t; wspr spr; };
struct check_t { fixed x, y; int row; bool lit; wspr spr; };
struct bullet_t { fixed x, y, vx, vy; int life; bool rocket = false; wspr spr; };
struct glob_t { fixed x, y, vx, vy; int life, t, dir; bool fire; wspr spr; };
struct boom_t { fixed x, y; int t; wspr spr; };   // a rocket's explosion
struct grenade_t { fixed x, y, vx, vy; int t; wspr spr; };   // t < 0: gone off
struct part_t { fixed x, y, vx, vy, g; int life, max, colour, size; wspr spr; };
struct floater_t { fixed x, y; int life; bn::vector<bn::sprite_ptr, 6> spr; };

struct boss_t
{
    int gate, kind; rect door; fixed x0, x1, cx, x, y, home_y;   // kind: BOSS_SKULL (World 4-4), BOSS_WASP (1-4), BOSS_EYE (2-4), BOSS_MECH (3-4)
    enum st { idle, intro, fight, dying, dead } state;
    enum md { m_float, m_volley, m_rain, m_dive, m_recover } mode;
    int hp, mush_t_, demon_t, t, hit, face, cool, attack, mt, shots;
    bool rocket_dropped;   // the Fire Skull's rocket launcher has fallen (see BOSS_ROCKET_AT)
    bool blue_dropped;     // Hard: the Alien Wasp's blue mushroom has fallen (at a quarter health)
    fixed shown, tx, ty;
    fixed vx, vy; int walk;   // the mech walker's leap and its patrol direction
    wspr q[4];
};

enum { AIM_AHEAD, AIM_DIAG, AIM_UP };
constexpr fixed DIAG = fixed(0.7071);   // each axis's share of a 45-degree move
struct player_t
{
    fixed x, y, vx, vy, pb, step, cool;
    int w = PW, h = PH;
    bool ground = false, crouch = false;
    int aim = 0;   // AIM_AHEAD, AIM_DIAG (UP with LEFT or RIGHT: 45 degrees) or AIM_UP (UP alone) (user requests, 2026-10-20)
    int coyote = 0, face = 1, dead = 0, on_mover = -1, flash = 0, hp = MAX_HP, inv = 0;
    int star = 0;   // frames of star power left
    int flame = 0;  // frames of the flamethrower left
    int rocket = 0; // frames of the rocket launcher left
    int grenade_cool = 0;   // frames until R can throw another grenade
    int throw_t = 0;        // frames into his throwing animation (0: not throwing; see THROW_STEP)
};

struct run_t
{
    int time = 0, deaths = 0, embers = 0, total = 0, lives = MAX_LIVES, coins = 0, score = 0;
    int life_coins = 0;   // coins toward the next 1-up (the HUD counter); a restarted level sets it back to 0 (user, 2026-10-24)
    bool ember_bonus[level_count] = {};
    uint8_t oneup_got[level_count] = {};   // per level, a bit for each 1-up block already claimed
    // Grenades in hand, whether he has the launcher now, and the world that gave them (0: none yet)
    int grenades = 0; bool got_grenades = false; int grenade_world = 0;
};
struct lvl_t { int time = 0, deaths = 0, embers = 0, coins = 0, score = 0, coin_lives = 0, grenades = 0; bool hurt = false; };   // coin_lives: 1-ups from coins this attempt; grenades: thrown this attempt

BN_DATA_EWRAM uint8_t grid[ROWS * MAX_W];
// A level with a rocket launcher gets its tile background's map copied here (from ROM) at each load, so spikes the rockets
// destroy can be erased from it; level_map_ram_on says the level BG uses it. To fit World 4-1's, only tile rows 1-16 are
// copied (MAP_RAM_ROWS cell rows from MAP_RAM_SKIP; row 0 is empty in every level), so the BG starts 16 px down and the
// camera stops 16 px short of the level's top there (clamp_cam_y). World 4-1's copy is 1408x32 cells, 88 KB.
constexpr int MAP_RAM_ROWS = 32, MAP_RAM_SKIP = 2;
BN_DATA_EWRAM bn::regular_bg_map_cell level_map_ram[rocket_map_cells];
bool level_map_ram_on = false;
// Sized for the levels doubled in length (2026-10-17): World 4-4 has 60 enemies, World 1-3 171 coins
BN_DATA_EWRAM bn::vector<coin_t, 320> coins;     // a level's coins plus those its cracked blocks spill
BN_DATA_EWRAM bn::vector<ember_t, 80> embers;
BN_DATA_EWRAM bn::vector<mush_t, 32> mushes;     // a level's mushrooms plus those that fall during a boss fight
BN_DATA_EWRAM bn::vector<oneup_t, 4> oneups;   // World 4-4 has 3 1-up blocks
BN_DATA_EWRAM flame_item_t flame_item;          // the flamethrower pickup (World 1-1 only)
BN_DATA_EWRAM flame_item_t rocket_item;         // the rocket launcher pickup (World 1-2, 2-1, 3-2, 4-1; or dropped in the Fire Skull fight)
int flame_len = 0;                               // the flame's length this frame (px from the muzzle; 0 = off)
BN_DATA_EWRAM star_t star_item;                 // the level's star, once its block is broken
BN_DATA_EWRAM bn::vector<crack_t, 32> cracks;
BN_DATA_EWRAM bn::vector<enemy_t, MAX_ENEMIES> enemies;
// A level with more enemies, movers or embers than these hold crashed on load ("Vector is full"): catch it when building
constexpr bool level_lists_fit()
{
    for(const level_info& l : levels)
    {
        if(bn::max(l.enemies_on[0], bn::max(l.enemies_on[1], l.enemies_on[2])) + BOSS_DEMON_MAX > MAX_ENEMIES || l.movers_n > 12 || l.embers_n > 80 || l.mushrooms_n > 24 || l.checks_n > 12) return false;
        if(l.coins_n > 192 || l.oneup_blocks_n + (l.expert_oneup.x >= 0) > 8) return false;
        int blocks = (l.expert_star.x >= 0) + (l.expert_oneup.x >= 0);
        for(int i = 0; i < ROWS * l.width; ++i) if(l.grid[i] == 4) ++blocks;
        if(blocks > 32) return false;
    }
    return true;
}
static_assert(level_lists_fit(), "a level has more enemies, movers, embers, mushrooms, checkpoints, coins or blocks than the game's lists hold");
BN_DATA_EWRAM bn::vector<mover_t, 12> movers;
BN_DATA_EWRAM bn::vector<check_t, 12> checks;
BN_DATA_EWRAM bn::vector<bullet_t, 12> bullets;
BN_DATA_EWRAM bn::vector<glob_t, 32> globs;
BN_DATA_EWRAM bn::vector<part_t, 28> parts;
BN_DATA_EWRAM bn::vector<boom_t, 4> booms;
BN_DATA_EWRAM bn::vector<grenade_t, 4> grenades;
BN_DATA_EWRAM bn::vector<floater_t, 3> floaters;
BN_DATA_EWRAM bn::optional<boss_t> boss;
rect door;
bool has_door = false;
fixed respawn_x, respawn_y;
int ember_total = 0, coin_total = 0;

BN_DATA_EWRAM player_t p;
BN_DATA_EWRAM run_t run;
BN_DATA_EWRAM lvl_t lvl;
int li = 0;            // current level index
int jump_buf = 0;
fixed shake = 0;
fixed cam_x = 0, cam_y = 0;
int frame = 0;

enum class state_t { title, difficulty, controls, credits, play, pause, clear, over, congrats, win, world_card, initials, scores, ta_select };
state_t state = state_t::title;
int title_t = 0;
// Difficulty (user, 2026-10-23), picked on the Select Difficulty menu after PRESS START. "Normal" is the game as it was;
// in "Hard" every enemy and boss fires 40% more often (hard_frames). Each keeps its own high scores and best time.
// "Expert" (user, 2026-10-24) is Hard (hard is true for it too) plus more enemies in Worlds 1 and 3 (enemy_def::modes).
#if defined(SMHB_EXPERT)
int difficulty = 2;
#elif defined(SMHB_HARD)
int difficulty = 1;
#else
int difficulty = 0;
#endif
bool hard = difficulty >= 1;
constexpr int DIFFICULTIES = 3;
// How many grenades he gets on coming into a world
int world_grenades(int world)
{
    if(difficulty != 2) return GRENADE_START;
    return world == 4 ? EXPERT_W4_GRENADES : EXPERT_GRENADES;
}
constexpr const char* difficulty_names[DIFFICULTIES] = { "Easy", "Normal", "Hard" };   // shown names (user, 2026-10-25: were Normal/Hard/Expert; code names unchanged)
void set_difficulty(int d) { difficulty = d; hard = d >= 1; }

// Test builds can start the game at a later level (build.ps1 -StartLevel n)
#ifndef SMHB_START_LEVEL
#define SMHB_START_LEVEL 0
#endif
// A wait between shots in this difficulty: 1/1.4 of it in Hard, so shots come 40% faster
int hard_frames(int n) { return hard ? (n * 5 + 3) / 7 : n; }
// A boss's wait between attacks and shots: Hard's, and on Expert 1/1.1 of that, so bosses fire 10% faster (user, 2026-10-24)
int boss_frames(int n) { return difficulty == 2 ? (n * 50 + 38) / 77 : hard_frames(n); }
bool scores_from_title = false;
// Time Attack (user, 2026-10-25): any level, played as on Normal, for the fastest clear (see Time Attack below)
bool time_attack = false;
bool hs_then_stats = false;   // after the high-score table: the end-of-game stats (true) or the title
int ta_level = 0;               // the level picked on its screen
int ta_prev_difficulty = 0;     // the difficulty picked before, given back on leaving Time Attack
// The title and the screens its menu opens, over the same panning picture and title music
bool title_screens()
{
    return state == state_t::title || state == state_t::difficulty || state == state_t::controls || state == state_t::credits ||
           (state == state_t::scores && scores_from_title) || state == state_t::ta_select;
}   // the high-score table opened with SELECT on the title (title music and pan carry on)

struct keys_t { bool left, right, jump, fire, grenade, down, up, a_pressed, start_pressed, select_pressed, b_pressed, left_pressed, right_pressed, up_pressed, down_pressed; } K;

const level_info& LV() { return levels[li]; }
int level_w() { return LV().width; }
int tile(int tx, int ty)
{
    if(tx < 0 || tx >= level_w()) return 1;
    if(ty < 0 || ty >= ROWS) return 0;
    return grid[ty * level_w() + tx];
}
bool solid(int t) { return t == 1 || t == 4; }
// Cracked blocks are 27 px tall (user, 2026-10-29, so standing shots hit them): one tile plus the bottom crack_rise px of
// the (always empty) tile above, which counts as the block
bool crack_cap(int tx, int ty) { return tile(tx, ty) == 0 && tile(tx, ty + 1) == 4; }
fixed cap_top(int ty) { return (ty + 1) * T - crack_rise; }   // (the top of a block whose cap is in row ty)
// The tile a point is in (tx, ty), a point in a block's raised top giving the block's tile
int tile_px(fixed x, fixed y, int& tx, int& ty)
{
    tx = tdiv(x); ty = tdiv(y);
    if(crack_cap(tx, ty) && y >= cap_top(ty)) { ++ty; return 4; }
    return tile(tx, ty);
}
// The top of tile tx, ty: a cracked block's is crack_rise px higher
fixed tile_top(int tx, int ty) { return ty * T - (tile(tx, ty) == 4 ? crack_rise : 0); }
// Whether something whose bottom is at `bottom` and whose side is in tile tx, ty runs into it (a wall, or a block's raised top)
bool wall_at(int tx, int ty, fixed bottom) { return solid(tile(tx, ty)) || (crack_cap(tx, ty) && bottom > cap_top(ty)); }
fixed max_cam_x() { return level_w() * T - SCREEN_W; }
fixed max_cam_y() { return ROWS * T - SCREEN_H; }
fixed clamp_cam_x(fixed x) { return bn::max(fixed(0), bn::min(max_cam_x(), x)); }
fixed clamp_cam_y(fixed y) { return bn::max(fixed(level_map_ram_on ? MAP_RAM_SKIP * 8 : 0), bn::min(max_cam_y(), y)); }

// ---------- Per-theme look (jungle, graveyard, hell, city; each level's own tile background is level_bgs[li]) ----------
struct parallax_look { const bn::regular_bg_item* item; int period, map_w; fixed fx, fy; };
struct level_look
{
    parallax_look far, mid, near;
    int crack_frame, platform_frame;
    const bn::music_item* music;
};
const level_look& look()
{
    static const level_look looks[4] = {
        { { &bn::regular_bg_items::jungle_far, jungle_far_period, jungle_far_map_w, fixed(0.08), fixed(0.1) },
          { &bn::regular_bg_items::jungle_mid, jungle_mid_period, jungle_mid_map_w, fixed(0.18), fixed(0.2) },
          { &bn::regular_bg_items::jungle_near, jungle_near_period, jungle_near_map_w, fixed(0.38), fixed(0.35) },
          0, 0, &bn::music_items::jungle_music },
        // The sky and the tree/mansion layer are one background here (two need more tile memory than the GBA has left)
        { { &bn::regular_bg_items::grave_far, grave_far_period, grave_far_map_w, fixed(0.06), fixed(0.1) },
          { nullptr, 0, 0, 0, 0 },
          { &bn::regular_bg_items::grave_near, grave_near_period, grave_near_map_w, fixed(0.3), fixed(0.3) },
          1, 1, &bn::music_items::grave_music },
        { { &bn::regular_bg_items::hell_far, hell_far_period, hell_far_map_w, fixed(0.15), fixed(0.15) },
          { &bn::regular_bg_items::hell_mid, hell_mid_period, hell_mid_map_w, fixed(0.36), fixed(0.3) },
          { &bn::regular_bg_items::hell_near, hell_near_period, hell_near_map_w, fixed(0.5), fixed(0.4) },
          2, 1, &bn::music_items::hell_music },
        // World 3-1: Synth City's sky, towers and street; Bit By Bit's "Heavy Air" (the user's choice)
        { { &bn::regular_bg_items::city_far, city_far_period, city_far_map_w, fixed(0.08), fixed(0.1) },
          { &bn::regular_bg_items::city_mid, city_mid_period, city_mid_map_w, fixed(0.18), fixed(0.2) },
          { &bn::regular_bg_items::city_near, city_near_period, city_near_map_w, fixed(0.38), fixed(0.35) },
          3, 2, &bn::music_items::city_music },
    };
    return looks[LV().theme];
}

// ---------- Music: the level's track while playing, the boss track during the fight (like the web game's syncMusic) ----------
const bn::music_item* current_music = nullptr;
bool boss_active();
void sync_music()
{
    const bn::music_item* want = nullptr;
    if(title_screens()) want = &bn::music_items::title_music;   // Heatley Bros' "Title Screen Music" (the user's choice)
    else if(state == state_t::play || state == state_t::pause)
    {
        // The World 1-4 and 2-4 fights play "Final Showdown" (Bit By Bit's OGG, the user's choice); the Fire Skull its own
        want = ! boss_active() ? look().music : boss->kind == BOSS_SKULL ? &bn::music_items::boss_music : &bn::music_items::showdown_music;
        if(p.star > 0) want = &bn::music_items::star_music;   // the star's music replaces the level's or the boss's while it lasts
    }
    // The user's "beat the game music" (2026-10-29): from the Congratulations screen through the stats (user, same day),
    // including a new high score's initials and table in between
    else if(state == state_t::congrats || state == state_t::win ||
            ((state == state_t::initials || state == state_t::scores) && hs_then_stats && ! time_attack))
        want = &bn::music_items::win_music;
    if(want != current_music)
    {
        current_music = want;
        if(bn::music::playing()) bn::music::stop();
        // Louder than the web mix, at the user's request: jungle 2.34x, graveyard 3.52x, hell 1.69x, boss 2.55x
        // (on 2026-10-16 jungle and graveyard went up a quarter and hell down a tenth, then graveyard up another quarter;
        // the boss stayed put). The graveyard track is now converted from a better source (it sounded "crunchy"; see
        // build-assets.js). Since replaced (the user's choice) by "I've Got Your Back!" (its main loop), at 155/64: the same RMS
        // level on the GBA as the track before it. Every track's XM plays its
        // sample on two channels at once, doubling it, because the mixer's output is played at half strength (see main()).
        // The Fire Skull's "Battle Mage" (the user's choice, replacing the web game's "Final Showdown"): 79/32, the same RMS
        // level on the GBA as the track it replaced at 51/20
        if(want) want->play(sfx::vol(want == &bn::music_items::boss_music ? sfx_volume_music_boss_music * 79 / 32 :
            // World 1's "World 1 Music.mp3" (the user's choice, 2026-10-21): the same RMS level on the GBA as "Battle of
            // the Kingdom" before it (109/64), which matched the web track at 75/32
            want == &bn::music_items::jungle_music ? sfx_volume_music_jungle_music * 158 / 64 :
            // World 2's "World 2 Music.mp3" (the user's, 2026-10-21): first at the level "I've Got Your Back!" had (39.5%),
            // then set to 35% at the user's request (same day)
            want == &bn::music_items::grave_music ? 350 :
            // World 3-1's "Heavy Air": the same RMS level on the GBA as World 1-1's track, which it played before
            want == &bn::music_items::city_music ? sfx_volume_music_city_music * 181 / 64 :
            // "Final Showdown" (World 1-4 and 2-4 bosses): the same RMS level on the GBA as the Fire Skull's old copy of it
            want == &bn::music_items::showdown_music ? sfx_volume_music_showdown_music * 83 / 32 :
            // The title music: the same RMS level on the GBA as World 1-1's track
            want == &bn::music_items::title_music ? sfx_volume_music_title_music * 152 / 64 :   // 155/64 until the user's 2026-10-21 edit
            // The star's music: the user's "invincibility music" since 2026-10-29, first at World 1-1's RMS level (194/64,
            // as loud as the old star clip at 842/64), then 40% louder (user, same day)
            want == &bn::music_items::star_music ? sfx_volume_music_star_music * 272 / 64 :
            // The Congratulations music: the same RMS level on the GBA as World 1's track
            want == &bn::music_items::win_music ? sfx_volume_music_win_music * 94 / 64 :
            // World 4's "World 4 Music.mp3" (the user's, 2026-10-21): first at the level Spider's Grotto had (18.8%), then
            // set to 25% at the user's request (same day)
            250));
    }
    if(want)
    {
        if(state == state_t::pause && ! bn::music::paused()) bn::music::pause();
        else if(state == state_t::play && bn::music::paused()) bn::music::resume();
    }
}
// The flamethrower's sound (user request, 2026-10-18), for as long as the flame is on: flame_start (its ignition) and then
// flame_loop over and over, each restarted on a frame count (build-assets.js cuts both to whole frames and makes the loop
// seamless). It has its own handle, outside sfx::play()'s slots: two sound channels (one finishing as the next starts).
BN_DATA_EWRAM bn::optional<bn::sound_handle> flame_snd;
int flame_snd_t = -1;      // frames into the clip playing, or -1 when off
bool flame_snd_loop = false;
void flame_audio(bool on)
{
#ifdef SMHB_NO_SFX
    on = false;
#endif
    if(! on)
    {
        if(flame_snd_t >= 0 && flame_snd && flame_snd->active()) flame_snd->stop();
        flame_snd.reset(); flame_snd_t = -1;
        return;
    }
    if(flame_snd_t >= 0 && ++flame_snd_t < (flame_snd_loop ? sfx_frames_flame_loop : sfx_frames_flame_start)) return;
    if(flame_snd && flame_snd->active()) flame_snd->stop();
    bool loop = flame_snd_t >= 0;   // the ignition first, then the roar
    flame_snd = (loop ? bn::sound_items::flame_loop : bn::sound_items::flame_start).play(sfx::vol(sfx_volume_flame * 3 / 2));   // capped at full volume
    flame_snd_t = 0; flame_snd_loop = loop;
}
void music_stop() { current_music = nullptr; if(bn::music::playing()) bn::music::stop(); }

// ---------- Display objects ----------
bn::optional<bn::camera_ptr> camera;
BN_DATA_EWRAM bn::optional<bn::sprite_text_generator> text8, text16;
wspr mario_spr, muzzle_spr, prone_spr[2];
BN_DATA_EWRAM wspr flame_spr[FLAME_PIECES];   // the flamethrower's flame, nozzle to tip
bool world_visible = true;   // world sprites are hidden behind the title, pause, clear and game-over text
bool parts_visible = true;   // particles also show on the Congratulations screen (the fireworks)

// bright: kept out of the level's darkness (see Darkness): everything that had a light around it in the web game (the
// user didn't want the glow on the GBA); the rest of the world is darkened
void attach(bn::sprite_ptr& s, int z, bool bright = false)
{
    s.set_camera(*camera);
    s.set_bg_priority(2);
    s.set_z_order(z);
    s.set_blending_enabled(! bright);
}

// The zoomed boss fight: the arena (480x272 world px, one web-game screen) shown at 75%, the zoom the user asked for
// (50% and then 62.5% felt too far out). Sprites leave the camera and share one 75% affine matrix (and a mirrored twin
// for flipped sprites); the scrolling level background is swapped for a pre-drawn 75% picture of the arena (360x204).
// The view (320x213 world px) follows Mario across the arena, clamped to its edges.
bool zoom = false;
const fixed ZOOM = fixed(0.75);
const fixed ZOOM_VIEW_W = fixed(SCREEN_W) / ZOOM, ZOOM_VIEW_H = fixed(SCREEN_H) / ZOOM;
fixed zoom_x0 = 0;               // the arena's left edge (the gate column)
fixed zoom_vx = 0, zoom_vy = 0;  // world position of the view's top-left
bn::optional<bn::sprite_affine_mat_ptr> zoom_mat, zoom_mat_flip;
fixed zoom_sx(fixed x) { return (x - zoom_vx) * ZOOM - SCREEN_W / 2; }
fixed zoom_sy(fixed y) { return (y - zoom_vy) * ZOOM - SCREEN_H / 2; }

bool near_view(fixed x, fixed y, int w, int h)
{
    if(zoom) return x + w > zoom_vx - 16 && x < zoom_vx + ZOOM_VIEW_W + 16 && y + h > zoom_vy - 16 && y < zoom_vy + ZOOM_VIEW_H + 16;
    return x + w > cam_x - 24 && x < cam_x + SCREEN_W + 24 && y + h > cam_y - 24 && y < cam_y + SCREEN_H + 24;
}

// ---------- Active window ----------
// Only what's near the view is updated, collided with and drawn. Walking every coin, ember and enemy each frame (the code
// runs from ROM) took the CPU from about 50% to 100% in World 4-4's boss fight once the levels doubled in length
// (2026-10-17). The level's coins, embers, mushrooms, blocks and enemies are loaded sorted by x (enemies by the left end of
// their patrol), so each frame a binary search finds the stretch [lo, hi) within ACTIVE_MARGIN of the view. Coins and
// mushrooms added later (from blocks, or falling in a boss fight) sit after coins_static / mushes_static and always count.
constexpr int ACTIVE_MARGIN = 288;   // past the view's edges: more than any foe's attack range (240) beyond Mario
struct span { int lo = 0, hi = 0, plo = 0, phi = 0; };   // plo/phi: last frame's, so drawing also hides what just left
span coin_span, ember_span, mush_span, crack_span, enemy_span;
int coins_static = 0, mushes_static = 0, enemy_reach = 0;   // enemy_reach: the longest patrol (px)

// Indexes 0..n-1 ordered by key(i): an insertion sort, once per level load
template<class K> const int* by_x(int n, K key)
{
    static int idx[256];
    BN_ASSERT(n <= 256, "Too many things to sort: ", n);
    for(int i = 0; i < n; ++i)
    {
        int v = key(i), j = i;
        while(j > 0 && key(idx[j - 1]) > v) { idx[j] = idx[j - 1]; --j; }
        idx[j] = i;
    }
    return idx;
}
// The first of the first n items whose key is at least x (the items are sorted by key)
template<class V, class K> int first_at(const V& v, int n, fixed x, K key)
{
    int lo = 0, hi = n;
    while(lo < hi) { int m = (lo + hi) / 2; if(key(v[m]) < x) lo = m + 1; else hi = m; }
    return lo;
}
void update_active()
{
    fixed x0 = (zoom ? zoom_vx : cam_x) - ACTIVE_MARGIN, x1 = (zoom ? zoom_vx + ZOOM_VIEW_W : cam_x + SCREEN_W) + ACTIVE_MARGIN;
    auto set = [](span& s, int lo, int hi) { s.plo = s.lo; s.phi = s.hi; s.lo = lo; s.hi = hi; };
    auto cx = [](const coin_t& c) { return c.x; };
    auto ex = [](const ember_t& e) { return e.x; };
    auto mx = [](const mush_t& m) { return m.x; };
    auto kx = [](const crack_t& c) { return fixed(c.tx * T); };
    auto ax = [](const enemy_t& e) { return e.a; };
    set(coin_span, first_at(coins, coins_static, x0, cx), first_at(coins, coins_static, x1, cx));
    set(ember_span, first_at(embers, embers.size(), x0, ex), first_at(embers, embers.size(), x1, ex));
    set(mush_span, first_at(mushes, mushes_static, x0, mx), first_at(mushes, mushes_static, x1, mx));
    set(crack_span, first_at(cracks, cracks.size(), x0, kx), first_at(cracks, cracks.size(), x1, kx));
    set(enemy_span, first_at(enemies, enemies.size(), x0 - enemy_reach, ax), first_at(enemies, enemies.size(), x1, ax));
}
// A new level: no last frame to tidy up after
void reset_active()
{
    coin_span = ember_span = mush_span = crack_span = enemy_span = span();
    update_active();
    for(span* s : { &coin_span, &ember_span, &mush_span, &crack_span, &enemy_span }) { s->plo = s->lo; s->phi = s->hi; }
}
// Loops over items [lo, hi) and then [tail, size): for(coin_t& c : active(coins, coin_span, coins_static))
template<class V> struct active_range
{
    V& v; int lo, hi, tail;
    struct iter
    {
        V* v; int i, hi, tail;
        auto& operator*() const { return (*v)[i]; }
        iter& operator++() { if(++i == hi) i = tail; return *this; }
        bool operator!=(const iter& o) const { return i != o.i; }
    };
    iter begin() const { return { &v, lo == hi ? tail : lo, hi, tail }; }
    iter end() const { return { &v, int(v.size()), hi, tail }; }
};
template<class V> active_range<V> active(V& v, const span& s, int tail) { return { v, s.lo, s.hi, tail }; }
template<class V> active_range<V> active(V& v, const span& s) { return { v, s.lo, s.hi, int(v.size()) }; }
// Drawing covers this frame's stretch and last frame's, so things that just left it get their sprites dropped
template<class V> active_range<V> drawn(V& v, const span& s, int tail) { return { v, bn::min(s.lo, s.plo), bn::max(s.hi, s.phi), tail }; }
template<class V> active_range<V> drawn(V& v, const span& s) { return drawn(v, s, int(v.size())); }

// Creates or drops a world sprite depending on whether it should show
bn::sprite_ptr* ensure(wspr& w, bool show, const bn::sprite_item& item, int gfx, int z, bool is_particle = false, bool bright = false)
{
    if(! show || ! (is_particle ? parts_visible : world_visible))
    {
        w.reset();
        return nullptr;
    }
    if(! w.s)
    {
        w.s = bn::sprite_ptr::create_optional(item, gfx);
        if(! w.s) return nullptr;
        attach(*w.s, z, bright);
        w.gfx = gfx; w.pal = 0; w.mode = 0;
    }
    else if(w.gfx != gfx)
    {
        w.s->set_tiles(item.tiles_item(), gfx);
        w.gfx = gfx;
    }
    return &*w.s;
}

// Positions a world sprite at world coordinates (its centre), in the normal view or the zoomed-out boss view
void place(wspr& w, fixed x, fixed y, bool flip = false)
{
    if(! w.s) return;
    bn::sprite_ptr& s = *w.s;
    if(! zoom)
    {
        if(w.mode > 0) { s.remove_affine_mat(); s.set_camera(*camera); }
        w.mode = 0;
        s.set_position(x, y);
        s.set_horizontal_flip(flip);
        return;
    }
    int m = flip ? 2 : 1;
    if(w.mode != m)
    {
        if(w.mode == 0) { s.remove_camera(); s.set_horizontal_flip(false); }
        s.set_affine_mat(flip ? *zoom_mat_flip : *zoom_mat);
        w.mode = m;
    }
    s.set_position(zoom_sx(x), zoom_sy(y));
}

// Swaps between an item's own palette (0) and a flash palette (1) only when it changes
void set_flash(wspr& w, bool on, const bn::sprite_item& item, const bn::sprite_palette_item& flash)
{
    if(! w.s || int(on) == w.pal) return;
    if(on) w.s->set_palette(flash); else w.s->set_palette(item.palette_item());
    w.pal = on;
}

// The star's flashing (the star itself, and Mario while it lasts): its palette cycles through four looks, k = 0 its own
// colours, 1 and 2 a third and two thirds of the way round the colour wheel, 3 washed halfway to white. Kept in w.pal, so
// the palette is only touched when the look changes (a new sprite starts at 0, its own colours).
void star_flash(wspr& w, int k)
{
    if(! w.s || w.pal == k) return;
    bn::sprite_palette_ptr pal = w.s->palette();
    pal.set_hue_shift_intensity(k == 1 ? fixed(0.33) : k == 2 ? fixed(0.67) : fixed(0));
    pal.set_fade(bn::color(31, 31, 31), k == 3 ? fixed(0.5) : fixed(0));
    w.pal = k;
}

// ---------- Particles & floating text ----------
void burst(fixed x, fixed y, int n, const int* cols, int ncols, fixed spd = 2, fixed grav = fixed(0.08), int life = 40)
{
    for(int i = 0; i < n && ! parts.full(); ++i)
    {
        fixed a = rng.get_int(360), s = rng.get_fixed(1) * spd;
        part_t q;
        q.x = x; q.y = y;
        q.vx = cos_deg(a) * s;
        q.vy = sin_deg(a) * s - spd * fixed(0.3);
        q.g = grav;
        q.max = life;
        q.life = (life * (60 + rng.get_int(40))) / 100;
        q.colour = cols[rng.get_int(ncols)];
        q.size = rng.get_int(10) < 3 ? 2 : 1;
        parts.push_back(bn::move(q));
    }
}
#define BURST(x, y, n, spd, grav, life, ...) do { static constexpr int c_[] = { __VA_ARGS__ }; burst(x, y, n, c_, int(sizeof(c_) / sizeof(int)), spd, grav, life); } while(0)

void float_text(fixed x, fixed y, const bn::string_view& text)
{
    if(floaters.full()) floaters.erase(floaters.begin());
    floaters.push_back(floater_t());
    floater_t& f = floaters.back();
    f.x = x; f.y = y; f.life = 60;
    text8->set_center_alignment();
    text8->set_bg_priority(1);
    if(zoom)
    {
        text8->generate(zoom_sx(x), zoom_sy(y), text, f.spr);
        for(bn::sprite_ptr& s : f.spr) s.set_z_order(-20);
        return;
    }
    text8->generate(x, y, text, f.spr);
    for(bn::sprite_ptr& s : f.spr) { s.set_camera(*camera); s.set_z_order(-20); }
}

void add_score(int n, bool show = false, fixed x = 0, fixed y = 0)
{
    run.score += n; lvl.score += n;
    if(show)
    {
        bn::string<12> s = "+";
        s += bn::to_string<8>(n);
        float_text(x, y, s);
    }
}

// ---------- The bosses ----------
// The Fire Skull: a huge flaming skull (Gothicvania, by ansimuz) guarding the end of Hell (World 4-4). Crossing into the
// arena seals a stone gate behind Mario; the skull drifts down, then cycles through fireball fans, a rain of fire from
// above, and a dive that sends flames skidding along the floor. Below half health it rages. Losing a life resets the fight.
// Two smaller fights in the same kind of arena were added at the user's request (2026-10-16), each with Bit By Bit's
// "Final Showdown":
// - World 1-4: the Alien Wasp (Warped's flying alien, by ansimuz), 20 HP. It spits fans of acid globs and dives at
//   Mario, with no rain and nothing along the floor unless it's raging, and rests longer between attacks.
// - World 2-4: the Eye Demon (Gothicvania's flying eye demon, by ansimuz), 30 HP. Between the two: fans of fireballs, a
//   light rain of fire and a dive (flames along the floor only when raging).
// - World 3-4: the Mech Walker (Warped's bipedal unit, by ansimuz), 40 HP. It walks the arena floor instead of flying:
//   fans of plasma from its cannon, a mortar barrage falling from above, and a leap at Mario whose landing sends plasma
//   skidding both ways along the floor. Its body is low enough to stand over from the side planks.
struct boss_spec
{
    const char* name;
    const char* defeated;
    int score;               // for destroying it (the user's values, 2026-10-16: 1000, 2000, 5000, 10000 by world)
    int hp, home_y;          // home_y: the lowest dip of its figure-eight reaches as far down as the skull's
    int box_w, box_h;        // its body, centred on x, y
    int land;                // a dive lands with x, y this far above the floor (its body on the floor)
    int cool, cool_rage;     // rest between attacks
    const bn::sprite_item* item;
    const bn::sprite_palette_item* flash;
    bool faces_right;        // the art's own facing
    int spr_dx, spr_dy;      // the sprite's centre from x, y, with the art facing its own way
    int anim;                // frames per animation frame (the mech walker's steps follow its x instead)
    const bn::regular_bg_item* arena;
};
const boss_spec& spec(int kind)
{
    static const boss_spec specs[4] = {
        { "FIRE SKULL", "FIRE SKULL DESTROYED", 10000, BOSS_HP, 140, 52, 56, 30, 75, 40,
          &bn::sprite_items::skull, &bn::sprite_palette_items::skull_flash, false, 0, 0, 6, &bn::regular_bg_items::hell_arena },
        // The wasp's x, y is its body's centre, which is off-centre in its frames (see build-assets.js)
        { "ALIEN WASP", "ALIEN WASP DESTROYED", 1000, WASP_HP, 147, 40, 42, 21, 90, 55,
          &bn::sprite_items::wasp, &bn::sprite_palette_items::wasp_flash, false, 13, -8, 4, &bn::regular_bg_items::jungle4_arena },
        { "EYE DEMON", "EYE DEMON DESTROYED", 2000, EYE_HP, 148, 40, 40, 20, 80, 48,
          &bn::sprite_items::eye_demon, &bn::sprite_palette_items::eye_demon_flash, true, 0, 0, 5, &bn::regular_bg_items::grave4_arena },
        // x, y is the middle of its lower body (44x46 from the floor up, under the side planks); the art stands on the
        // bottom of its 128x128 box (see build-assets.js). home_y is where it stands.
        { "MECH WALKER", "MECH WALKER DESTROYED", 5000, MECH_HP, 14 * 16 - 23, 44, 46, 23, 70, 42,
          &bn::sprite_items::mech_walker, &bn::sprite_palette_items::mech_walker_flash, true, 4, -40, 5, &bn::regular_bg_items::city4_arena },
    };
    return specs[kind];
}

// The boss's full health: its spec's (Normal), Hard's or Expert's
int boss_max_hp(const boss_t& B)
{
    if(! hard) return spec(B.kind).hp;
    if(difficulty == 2) return B.kind == BOSS_SKULL ? EXPERT_BOSS_HP : B.kind == BOSS_WASP ? EXPERT_WASP_HP : B.kind == BOSS_EYE ? EXPERT_EYE_HP : EXPERT_MECH_HP;
    return B.kind == BOSS_SKULL ? HARD_BOSS_HP : B.kind == BOSS_WASP ? HARD_WASP_HP : B.kind == BOSS_EYE ? HARD_EYE_HP : HARD_MECH_HP;
}
void make_boss()
{
    boss.reset();
    if(! LV().has_boss) return;
    boss.emplace();
    boss_t& B = *boss;
    B.gate = LV().boss_gate;
    B.kind = LV().theme == 2 ? BOSS_SKULL : LV().theme == 1 ? BOSS_EYE : LV().theme == 3 ? BOSS_MECH : BOSS_WASP;
    B.door = { fixed(LV().boss_door.x), fixed(LV().boss_door.y), 16, 32 };
    B.x0 = (B.gate + 1) * T; B.x1 = level_w() * T; B.cx = (B.x0 + B.x1) / 2;
    B.x = B.cx + 90; B.y = -80;
    B.home_y = spec(B.kind).home_y;
    B.state = boss_t::idle; B.mode = boss_t::m_float;
    B.rocket_dropped = false; B.blue_dropped = false;
    B.hp = boss_max_hp(B); B.mush_t_ = 0; B.demon_t = 0; B.t = 0; B.hit = 0; B.face = -1; B.cool = 0; B.attack = 0; B.mt = 0; B.shots = 0;
    B.shown = 0; B.tx = 0; B.ty = 0; B.vx = 0; B.vy = 0; B.walk = -1;
}
rect boss_box(const boss_t& B)
{
    const boss_spec& S = spec(B.kind);
    return { B.x - S.box_w / 2, B.y - S.box_h / 2, S.box_w, S.box_h };
}
bool boss_active() { return boss && boss->state != boss_t::idle && boss->state != boss_t::dead; }
void set_gate(bool on)
{
    for(int y = 0; y < 14; ++y) grid[y * level_w() + boss->gate] = on ? 1 : 0;
    BURST(boss->gate * T + 8, 13 * T, 14, fixed(1.6), fixed(0.1), 30, P_DUST, P_STONE, P_EMBER);
}
bool in_wall(fixed x, fixed y, int w, int h);
// The Fire Skull's demons: dead ones keep their slots at the end of the enemy list and are reused, so the list never
// shrinks under the active spans
void clear_boss_demons(bool pop)
{
    for(enemy_t& e : enemies)
    {
        if(! e.minion || ! e.alive) continue;
        if(pop) BURST(e.x + e.w / 2, e.y + e.h / 2, 16, fixed(2.2), fixed(0.1), 30, P_RED, P_ALIEN_RED, P_TEAL, P_WHITE);
        e.alive = false; e.squish = 1000;   // gone at once, no death flicker
    }
}
void spawn_boss_demon(const boss_t& B)
{
    int alive = 0;
    enemy_t* slot = nullptr;
    for(enemy_t& e : enemies) if(e.minion) { if(e.alive) ++alive; else if(! slot) slot = &e; }
    if(alive >= BOSS_DEMON_MAX) return;
    if(! slot)
    {
        if(enemies.full()) return;
        enemies.push_back(enemy_t());
        slot = &enemies.back();
    }
    enemy_t& k = *slot;
    k = enemy_t();
    k.minion = true; k.kind = FOE_DEMON; k.w = demon_box_w; k.h = demon_box_h;
    k.a = B.x0; k.b = B.x1 - k.w;
    // It flies in high up on the side of the arena away from Mario
    bool left = p.x + p.w / 2 > B.cx;
    k.x = left ? B.x0 + 8 : B.x1 - 8 - k.w; k.y = k.y0 = 4 * T;
    if(in_wall(k.x, k.y, k.w, k.h)) k.x = left ? B.x1 - 8 - k.w : B.x0 + 8;
    k.alive = true; k.alert = false; k.squish = 0; k.dir = left ? 1 : -1; k.cool = 0; k.windup = 0; k.hit = 0;
    k.t = rng.get_int(48); k.volley = 1; k.rise = RISE_END; k.vx = fixed(0.7); k.hp = DEMON_HP;
    BURST(k.x + k.w / 2, k.y + k.h / 2, 12, fixed(1.8), fixed(0.05), 26, P_RED, P_ALIEN_RED, P_EMBER);
    // (no chime as it comes in, on any difficulty: the user had it removed on Expert, then on Normal and Hard, 2026-10-25)
}
// Whether this level has a rocket launcher placed in it in this difficulty (Expert leaves World 3-2's out)
bool has_rocket_item(const level_info& L) { return L.rocket_item.x >= 0 && ! (difficulty == 2 && L.expert_no_rocket); }
void reset_boss()
{
    if(! boss || boss->state == boss_t::dead) return;
    if(boss->state != boss_t::idle) set_gate(false);
    make_boss(); globs.clear(); clear_boss_demons(false);
    for(int i = 0; i < mushes.size(); ) { if(mushes[i].boss) mushes.erase(mushes.begin() + i); else ++i; }
    if(! has_rocket_item(LV())) rocket_item = flame_item_t();   // a boss-dropped launcher goes with the fight
}

// Fireballs remember which of the 16 pre-rotated frames to show
int fire_dir(fixed vx, fixed vy)
{
    int a = (angle_of(vx * 16, vy * 16) * 16 / 360).round_integer();   // 22.5 degree steps; 0 = moving right
    return ((a % 16) + 16) % 16;
}
void push_glob(fixed x, fixed y, fixed vx, fixed vy, int life, bool fire, int t = 0)
{
    if(globs.full()) return;
    glob_t g; g.x = x; g.y = y; g.vx = vx; g.vy = vy; g.life = life; g.fire = fire; g.t = t;
    g.dir = fire ? fire_dir(vx, vy) : 0;
    globs.push_back(bn::move(g));
}
void boss_fire(fixed x, fixed y, fixed deg, fixed s) { push_glob(x, y, cos_deg(deg) * s, sin_deg(deg) * s, 220, true, rng.get_int(15)); }
// The wasp's acid glob (the aliens' spit, which bullets can also knock out of the air)
void boss_spit(fixed x, fixed y, fixed deg, fixed s) { push_glob(x, y, cos_deg(deg) * s, sin_deg(deg) * s, 220, false); }

// Sparks in each boss's colours: flame for the skull, alien green for the wasp, red for the eye demon. `final`: the
// burst when it's destroyed (a fourth colour)
void boss_burst(int kind, fixed x, fixed y, int n, fixed spd, fixed grav, int life, bool final = false)
{
    if(kind == BOSS_MECH)
    {
        if(final) BURST(x, y, n, spd, grav, life, P_GOLD, P_TEAL, P_WHITE, P_STONE);
        else BURST(x, y, n, spd, grav, life, P_GOLD, P_TEAL, P_WHITE);
    }
    else if(kind == BOSS_WASP)
    {
        if(final) BURST(x, y, n, spd, grav, life, P_GOLD, P_ALIEN, P_ALIEN_LIGHT, P_TEAL);
        else BURST(x, y, n, spd, grav, life, P_ALIEN, P_ALIEN_LIGHT, P_GOLD);
    }
    else if(kind == BOSS_EYE)
    {
        if(final) BURST(x, y, n, spd, grav, life, P_GOLD, P_RED, P_FLAME, P_EMBER);
        else BURST(x, y, n, spd, grav, life, P_RED, P_FLAME, P_EMBER);
    }
    else
    {
        if(final) BURST(x, y, n, spd, grav, life, P_GOLD, P_EMBER, P_FLAME, P_PALE);
        else BURST(x, y, n, spd, grav, life, P_FLAME, P_EMBER, P_GOLD);
    }
}

void hurt(fixed src_x, bool spike = false);
void hit_boss(int n)
{
    boss_t& B = *boss; if(B.state != boss_t::fight) return;
    B.hp = bn::max(0, B.hp - n); B.hit = 6;
    if(B.hp == 0) { B.state = boss_t::dying; B.t = 0; globs.clear(); clear_boss_demons(true); shake = 8; sfx::burn(); add_score(spec(B.kind).score, true, B.x, B.y - 50); }
}
void end_attack(boss_t& B, bool rage)
{
    B.mode = boss_t::m_float; B.mt = 0;
    B.cool = boss_frames(rage ? spec(B.kind).cool_rage : spec(B.kind).cool);
}

void update_boss()
{
    if(! boss || boss->state == boss_t::dead) return;
    boss_t& B = *boss;
    if(B.state == boss_t::idle)
    {
        if(! p.dead && p.x > (B.gate + 3) * T)
        {
            B.state = boss_t::intro; B.t = 0; set_gate(true); shake = 6; sfx::burn();
            sync_music();   // the showdown track starts as the gate slams shut
        }
        return;
    }
    ++B.t; if(B.hit > 0) --B.hit;
    const bool skull = B.kind == BOSS_SKULL, wasp = B.kind == BOSS_WASP, mech = B.kind == BOSS_MECH;
    if(skull && rng.get_int(10) < 3) BURST(B.x + rng.get_int(50) - 25, B.y + rng.get_int(40) - 20, 1, fixed(0.6), fixed(-0.04), 30, P_FLAME, P_EMBER, P_GOLD);
    if(B.state == boss_t::intro)
    {
        if(mech)
        {
            // The mech walker drops out of the sky and lands with a thud
            if(B.y < B.home_y)
            {
                B.vy += fixed(0.25); B.y += B.vy;
                if(B.y >= B.home_y)
                {
                    B.y = B.home_y; B.vy = 0; shake = 8; sfx::stomp();
                    BURST(B.x, 14 * T, 14, fixed(2.6), fixed(0.1), 34, P_DUST, P_STONE, P_GOLD);
                }
            }
        }
        else B.y += (B.home_y - B.y) * fixed(0.04);
        B.shown = bn::min(fixed(1), fixed(B.t) / 90);
        if(B.t > 110) { B.state = boss_t::fight; B.t = 0; B.cool = boss_frames(50); }
        return;
    }
    if(B.state == boss_t::dying)
    {
        if(! mech) B.y += fixed(0.3);   // the fliers sink; the mech walker stays on its feet
        if(B.t % 6 == 0)
        {
            boss_burst(B.kind, B.x + rng.get_int(60) - 30, B.y + rng.get_int(60) - 30, 8, fixed(2.4), fixed(0.05), 36);
            shake = bn::max(shake, fixed(4));
        }
        if(B.t % 24 == 0) sfx::stomp();
        if(B.t >= 120)
        {
            B.state = boss_t::dead; set_gate(false); door = B.door; has_door = true;
            sync_music();   // back to the level track
            boss_burst(B.kind, B.x, B.y, 24, fixed(3.2), fixed(0.05), 60, true);
            BURST(B.door.x + 8, B.door.y + 16, 10, fixed(1.6), fixed(-0.02), 50, P_EMBER, P_GOLD);
            shake = 10; sfx::burn(); sfx::check();
            float_text(B.cx, 110, spec(B.kind).defeated);
        }
        return;
    }

    bool rage = B.hp <= boss_max_hp(B) / 2;
    fixed px = p.x + p.w / 2, py = p.y + p.h / 2;
    B.face = px > B.x ? 1 : -1;
    // Every BOSS_MUSH_EVERY frames a healing mushroom drops from the sky somewhere in the arena (at most 3 waiting); none
    // in World 1-4's fight with the wasp (the user's call, 2026-10-16)
    if(! wasp && ++B.mush_t_ >= (B.kind == BOSS_EYE && difficulty == 2 ? EXPERT_EYE_MUSH_EVERY : BOSS_MUSH_EVERY))
    {
        B.mush_t_ = 0;
        int waiting = 0;
        for(const mush_t& m : mushes) if(m.boss && ! m.got) ++waiting;
        if(waiting < BOSS_MUSH_MAX && ! mushes.full())
        {
            mush_t m; m.x = B.x0 + 24 + rng.get_fixed(1) * (B.x1 - B.x0 - 60); m.y = -14; m.vy = 0;
            m.blue = false; m.got = false; m.fall = true; m.boss = true; m.t = 0;
            mushes.push_back(bn::move(m));
            sfx::drop();
        }
    }
    // Hard: a blue mushroom drops into the middle of the Alien Wasp's arena at a quarter health, once (user, 2026-10-23;
    // the wasp's fight has no other falling mushrooms). It falls and lands like the other fights' mushrooms.
    // On Expert it drops at 40% health instead (18 of 45; user, 2026-10-24).
    bool blue_due = difficulty == 2 ? B.hp * 5 <= boss_max_hp(B) * 2 : B.hp * BOSS_ROCKET_AT <= boss_max_hp(B);
    if(hard && wasp && ! B.blue_dropped && blue_due && ! mushes.full())
    {
        B.blue_dropped = true;
        mush_t m; m.x = B.cx - 6; m.y = -14; m.vy = 0;
        m.blue = true; m.got = false; m.fall = true; m.boss = true; m.t = 0;
        mushes.push_back(bn::move(m));
        sfx::drop();
    }
    // The rocket launcher drops into the middle of the arena at a quarter health, once (Fire Skull only)
    if(B.kind == BOSS_SKULL && ! B.rocket_dropped && B.hp * BOSS_ROCKET_AT <= boss_max_hp(B))
    {
        B.rocket_dropped = true;
        rocket_item = flame_item_t();
        rocket_item.x = B.cx; rocket_item.y = -14; rocket_item.vy = 0; rocket_item.live = true; rocket_item.fall = true;
        sfx::drop();
    }
    // (and on Expert the Eye Demon's, the same way: user, 2026-10-24)
    const bool summons = B.kind == BOSS_SKULL || (difficulty == 2 && B.kind == BOSS_EYE);
    if(summons && ++B.demon_t >= BOSS_DEMON_EVERY) { B.demon_t = 0; spawn_boss_demon(B); }
    ++B.mt;
    if(mech && (B.mode == boss_t::m_float || B.mode == boss_t::m_rain))
    {
        // The mech walker patrols the floor from end to end
        fixed lo = B.x0 + 40, hi = B.x1 - 40, sp = rage ? fixed(1.3) : fixed(0.9);
        B.x += sp * B.walk;
        if(B.x <= lo) { B.x = lo; B.walk = 1; }
        else if(B.x >= hi) { B.x = hi; B.walk = -1; }
    }
    else if(B.mode == boss_t::m_float || B.mode == boss_t::m_rain)
    {
        // Lazy figure-eight across the arena, dipping low enough to be shot from the floor. Hard: the eye demon flies it
        // 20% faster (the path and the follow rate both, so the same shape; user, 2026-10-24)
        fixed sp = hard && B.kind == BOSS_EYE ? fixed(EYE_HARD_SPEED) : fixed(1);
        fixed tx = B.cx + sin_deg(fixed(B.t) * fixed(0.63) * sp) * 150, ty = B.home_y + sin_deg(fixed(B.t) * fixed(1.547) * sp) * 38;
        B.x += (tx - B.x) * fixed(0.04) * sp; B.y += (ty - B.y) * fixed(0.05) * sp;
    }
    if(B.mode == boss_t::m_float)
    {
        if(--B.cool <= 0 && ! p.dead)
        {
            static constexpr boss_t::md order[] = { boss_t::m_volley, boss_t::m_rain, boss_t::m_volley, boss_t::m_dive };
            static constexpr boss_t::md wasp_order[] = { boss_t::m_volley, boss_t::m_volley, boss_t::m_dive };
            static constexpr boss_t::md eye_order[] = { boss_t::m_volley, boss_t::m_dive, boss_t::m_volley, boss_t::m_rain };
            // The mech walker's leap is its m_dive
            B.mode = wasp ? wasp_order[B.attack++ % 3] : skull ? order[B.attack++ % 4] : eye_order[B.attack++ % 4];
            B.mt = 0; B.shots = 0;
        }
    }
    else if(B.mode == boss_t::m_volley)
    {
        // Flicker for half a second, then fan shots at Mario
        if(mech)
        {
            // The mech walker: plasma from its cannon, 3 shots (two fans of 5 when raging)
            if(B.mt == 30 || (rage && B.mt == 30 + boss_frames(20)))
            {
                int n = rage ? 5 : 3;
                fixed gx = B.x + B.face * 27, gy = B.y - 15, a = angle_of(px - gx, py - gy);
                for(int i = 0; i < n; ++i) boss_spit(gx, gy, a + fixed(i * 2 - (n - 1)) / 2 * fixed(11.46), rage ? fixed(3) : fixed(2.6));
                sfx::fire(); shake = bn::max(shake, fixed(2));
            }
            if(B.mt >= 30 + boss_frames(rage ? 40 : 20)) end_attack(B, rage);
        }
        else if(! skull)
        {
            // The wasp and the eye demon: one fan, 3 shots (5 when raging); acid from the wasp, fire from the eye demon
            if(B.mt == 30)
            {
                int n = rage ? 5 : 3;
                fixed a = angle_of(px - B.x, py - B.y);
                for(int i = 0; i < n; ++i)
                {
                    fixed d = a + fixed(i * 2 - (n - 1)) / 2 * fixed(11.46);
                    if(wasp) boss_spit(B.x + B.face * 12, B.y + 4, d, rage ? fixed(2.6) : fixed(2.2));
                    else boss_fire(B.x + B.face * 14, B.y + 6, d, rage ? fixed(2.7) : fixed(2.4));
                }
                if(wasp) sfx::spit(); else sfx::fire();
                shake = bn::max(shake, fixed(2));
            }
            if(B.mt >= 30 + boss_frames(20)) end_attack(B, rage);
        }
        else
        {
            // The skull: a fan of fireballs (a second fan when raging)
            if(B.mt == 30 || (rage && B.mt == 30 + boss_frames(22)))
            {
                int n = rage ? 7 : 5;
                fixed a = angle_of(px - B.x, py - B.y), s = rage ? fixed(2.9) : fixed(2.5);
                for(int i = 0; i < n; ++i) boss_fire(B.x + B.face * 20, B.y + 8, a + fixed(i * 2 - (n - 1)) / 2 * fixed(11.46), s);
                sfx::fire(); shake = bn::max(shake, fixed(3));
            }
            if(B.mt >= 30 + boss_frames(rage ? 40 : 20)) end_attack(B, rage);
        }
    }
    else if(B.mode == boss_t::m_rain)
    {
        // Fire falling from above the arena: the skull's 7 (10 raging), the eye demon's lighter 5 (7); the mech walker's
        // mortar barrage is 6 (8) plasma shots
        int n = skull ? (rage ? 10 : 7) : mech ? (rage ? 8 : 6) : (rage ? 7 : 5);
        if(B.shots < n && B.mt >= boss_frames((B.shots + 1) * 12))   // every 12 frames (every 1/1.4 of that in Hard, 1/1.54 in Expert)
        {
            ++B.shots;
            fixed s = skull ? (rage ? fixed(2.6) : fixed(2.2)) : rage ? fixed(2.4) : fixed(2);
            fixed rx = B.x0 + 16 + rng.get_fixed(1) * (B.x1 - B.x0 - 32), rd = 90 + (rng.get_fixed(1) - fixed(0.5)) * fixed(11.46);
            if(mech) boss_spit(rx, -12, rd, s + fixed(0.2)); else boss_fire(rx, -12, rd, s);
            sfx::spit();
        }
        if(B.shots >= n && B.mt > boss_frames(n * 12) + 20) end_attack(B, rage);
    }
    else if(B.mode == boss_t::m_dive && mech)
    {
        // The mech walker's leap: shudder for half a second, then jump in an arc to where Mario stands
        if(B.mt <= 30)
        {
            B.x += rng.get_fixed(2) - 1;
            if(B.mt == 30)
            {
                fixed tx = bn::max(B.x0 + 30, bn::min(B.x1 - 30, px));
                B.vx = (tx - B.x) / 48; B.vy = -6; sfx::burn();   // 48 frames up and back down at 0.25 px/frame^2
            }
        }
        else
        {
            B.vy += fixed(0.25); B.x += B.vx; B.y += B.vy;
            if(B.vy > 0 && B.y >= B.home_y)
            {
                B.y = B.home_y; B.mode = boss_t::m_recover; B.mt = 0; shake = 9; sfx::stomp();
                // A shockwave of plasma skidding each way along the floor (a slower second pair when raging)
                fixed gy = 14 * T - 7;
                BURST(B.x, 14 * T, 14, fixed(2.6), fixed(0.1), 34, P_DUST, P_STONE, P_GOLD);
                boss_spit(B.x - 24, gy, 180, fixed(2.4)); boss_spit(B.x + 24, gy, 0, fixed(2.4));
                if(rage) { boss_spit(B.x - 24, gy, 180, fixed(1.6)); boss_spit(B.x + 24, gy, 0, fixed(1.6)); }
            }
        }
    }
    else if(B.mode == boss_t::m_dive)
    {
        if(B.mt <= 40)
        {
            // Wind-up: rise and shudder, then lock on to where Mario stands
            B.y += (B.home_y - 40 - B.y) * fixed(0.08); B.x += rng.get_fixed(2) - 1;
            if(B.mt == 40) { B.tx = bn::max(B.x0 + 30, bn::min(B.x1 - 30, px)); B.ty = 14 * T - spec(B.kind).land; sfx::burn(); }
        }
        else
        {
            fixed dx = B.tx - B.x, dy = B.ty - B.y;
            fixed s = wasp ? (rage ? fixed(7) : fixed(5.5)) : skull ? (rage ? fixed(8) : fixed(6.5)) : rage ? fixed(7.5) : fixed(6);
            if(hard && B.kind == BOSS_EYE) s *= fixed(EYE_HARD_SPEED);   // the eye demon dives 20% faster in Hard
            int d = bn::sqrt(dx.round_integer() * dx.round_integer() + dy.round_integer() * dy.round_integer());
            if(d <= s)
            {
                B.x = B.tx; B.y = B.ty; B.mode = boss_t::m_recover; B.mt = 0; shake = 9; sfx::stomp();
                fixed gy = 14 * T - 7;
                if(wasp)
                {
                    // Dust from the floor, and only when raging a glob of acid skidding each way
                    BURST(B.x, 14 * T, 14, fixed(2.6), fixed(0.1), 34, P_DUST, P_STONE, P_ALIEN_LIGHT);
                    if(rage) { boss_spit(B.x - 24, gy, 180, fixed(2.2)); boss_spit(B.x + 24, gy, 0, fixed(2.2)); }
                }
                else if(! skull)
                {
                    // The eye demon: dust and embers, and only when raging a flame skidding each way
                    BURST(B.x, 14 * T, 14, fixed(2.6), fixed(0.1), 34, P_DUST, P_STONE, P_EMBER);
                    if(rage) { boss_fire(B.x - 24, gy, 180, fixed(2.4)); boss_fire(B.x + 24, gy, 0, fixed(2.4)); }
                }
                else
                {
                    BURST(B.x, 14 * T, 14, fixed(2.6), fixed(0.1), 34, P_FLAME, P_EMBER, P_GOLD);
                    boss_fire(B.x - 24, gy, 180, fixed(2.6)); boss_fire(B.x + 24, gy, 0, fixed(2.6));
                    if(rage) { boss_fire(B.x - 24, gy, 180, fixed(1.6)); boss_fire(B.x + 24, gy, 0, fixed(1.6)); }
                }
            }
            else { B.x += dx * s / d; B.y += dy * s / d; }
        }
    }
    else if(B.mode == boss_t::m_recover)
    {
        // Stunned on the floor for a moment: the best chance to unload on it
        if(B.mt > 45) end_attack(B, rage);
    }
    if(! p.dead && overlap({ p.x, p.y, fixed(p.w), fixed(p.h) }, boss_box(B))) hurt(B.x);   // only bullets damage it
}

// ---------- Level setup ----------
void spawn(fixed x, fixed y)
{
    p = player_t();
    p.x = x; p.y = y;
}

bn::optional<bn::regular_bg_ptr> bg_level, bg_far, bg_mid, bg_near, bg_arena;
int bg_level_index = -1;
bn::regular_bg_ptr make_level_bg();

void set_zoom(bool on)
{
    zoom = on;
    if(on)
    {
        if(! zoom_mat)
        {
            zoom_mat = bn::sprite_affine_mat_ptr::create(); zoom_mat->set_scale(ZOOM);
            zoom_mat_flip = bn::sprite_affine_mat_ptr::create(); zoom_mat_flip->set_scale(ZOOM); zoom_mat_flip->set_horizontal_flip(true);
        }
        zoom_x0 = boss->gate * T;
        bg_level.reset();   // the GBA has 4 background layers and Hell uses them all, so the arena picture takes the level's
        zoom_vx = -1;   // update_zoom_view snaps the view onto Mario the first time
        // The 512x256 picture holds the 300x170 arena in its top-left corner (placed by update_zoom_view)
        bg_arena = spec(boss->kind).arena->create_bg(0, 0);
        bg_arena->set_priority(2);
        bg_arena->set_blending_enabled(true);   // darkened like the rest (see Darkness)
    }
    else
    {
        bg_arena.reset();
        bg_level = make_level_bg();
        bg_level->set_priority(2);
        bg_level->set_blending_enabled(true);
        bg_level->set_camera(*camera);
    }
}

// The level's tile background: from ROM, or from level_map_ram for a level with a rocket launcher (see load_level)
bn::regular_bg_ptr make_level_bg()
{
    const bn::regular_bg_item& item = *level_bgs[li];
    if(! level_map_ram_on) return item.create_bg(LV().bg_width / 2, 256);
    bn::size dims(item.map_item().dimensions().width(), MAP_RAM_ROWS);
    bn::regular_bg_item ram(item.tiles_item(), item.palette_item(), bn::regular_bg_map_item(level_map_ram[0], dims));
    return ram.create_bg(LV().bg_width / 2, MAP_RAM_SKIP * 8 + MAP_RAM_ROWS * 4);   // (centre) top at y 16
}

void create_backgrounds()
{
    if(zoom) set_zoom(false);
    if(bg_level_index == li) return;
    bool had = bg_far.has_value();
    bg_level.reset(); bg_far.reset(); bg_mid.reset(); bg_near.reset();
    // Butano only hands freed BG VRAM back at the end of a frame (until then a new BG can reuse a freed block only of the
    // exact same size), so let a frame pass before creating the new level's layers. Without it "Play again" after the
    // Fire Skull crashed in bn_bg_blocks_manager: the arena swaps leave Hell's VRAM fragmented, and World 1-1's tiles
    // didn't fit in what was left.
    if(had) bn::core::update();
    const level_look& lk = look();
    bg_far = lk.far.item->create_bg(0, 0);
    if(lk.mid.item) bg_mid = lk.mid.item->create_bg(0, 0);
    bg_near = lk.near.item->create_bg(0, 0);
    bg_level = make_level_bg();
    bg_far->set_priority(3); bg_near->set_priority(3); bg_level->set_priority(2);
    bg_far->set_z_order(3); bg_near->set_z_order(1);
    if(bg_mid) { bg_mid->set_priority(3); bg_mid->set_z_order(2); }
    bg_level->set_camera(*camera);
    // Every layer takes the darkness's fade (see Darkness)
    bg_far->set_blending_enabled(true); bg_near->set_blending_enabled(true); bg_level->set_blending_enabled(true);
    if(bg_mid) bg_mid->set_blending_enabled(true);
    const int* sky = level_sky_rgb[LV().theme];
    bn::bg_palettes::set_transparent_color(bn::color(sky[0] >> 3, sky[1] >> 3, sky[2] >> 3));
    bg_level_index = li;
}

void load_level(int index)
{
    li = index;
    const level_info& L = LV();
    for(int i = 0; i < ROWS * L.width; ++i) grid[i] = L.grid[i];
    // A fresh copy of the map, with every spike back (create_backgrounds() makes the BG from it, or, when the level's BG
    // is already up, it's reloaded below)
    bool map_was_ram = level_map_ram_on && bg_level_index == li;
    level_map_ram_on = L.rocket_item.x >= 0;
    if(level_map_ram_on)
    {
        const bn::regular_bg_map_item& m = level_bgs[li]->map_item();
        int mw = m.dimensions().width();
        BN_ASSERT(mw * MAP_RAM_ROWS <= rocket_map_cells, "Level map too big for its RAM copy: ", mw);
        bn::memory::copy(m.cells_ptr()[MAP_RAM_SKIP * mw], mw * MAP_RAM_ROWS, level_map_ram[0]);
    }
    else if(map_was_ram) bg_level_index = -1;   // the BG still points at the RAM copy: make it again from ROM
    coins.clear(); embers.clear(); mushes.clear(); oneups.clear(); cracks.clear(); enemies.clear(); movers.clear(); checks.clear();
    star_item = star_t();
    flame_item = flame_item_t(); flame_len = 0;
    if(L.flame_item.x >= 0)
    {
        flame_item.x = L.flame_item.x * T + 8; flame_item.y = (L.flame_item.y + 1) * T - 9; flame_item.live = true;
    }
    rocket_item = flame_item_t();
    if(has_rocket_item(L))
    {
        rocket_item.x = L.rocket_item.x * T + 8; rocket_item.y = (L.rocket_item.y + 1) * T - 9; rocket_item.live = true;
    }
    bullets.clear(); globs.clear(); parts.clear(); floaters.clear(); booms.clear(); grenades.clear();
    // Grenades, by world: a fresh GRENADE_START on first coming into each world (Worlds 1-3 on every difficulty, user
    // 2026-10-29; World 4 on Expert since 2026-10-25 and on Easy and Normal too since 2026-10-29), at its first level (x-1,
    // or wherever a Time Attack or test ROM starts in it); carried through that world, never into the next
    const bn::string_view name = L.name;   // "World x-y"
    const int world = name.size() > 8 ? name[6] - '0' : 0;
    if(world >= 1 && world <= 4)
    {
        if(run.grenade_world != world)
        {
            run.grenades = world_grenades(world);
            run.got_grenades = true; run.grenade_world = world;
        }
    }
    else if(run.got_grenades) { run.grenades = 0; run.got_grenades = false; run.grenade_world = 0; }

    // Coins, embers, mushrooms, blocks and enemies go in left to right, for the active window (see Active window)
    const int* order = by_x(L.coins_n, [&](int i) { return L.coins[i].x; });
    for(int n = 0; n < L.coins_n; ++n)
    {
        int i = order[n];
        coin_t k; k.x = L.coins[i].x; k.y = L.coins[i].y; k.vx = 0; k.vy = 0; k.rest = true; k.got = false; k.t = rng.get_int(100); k.age = 99;
        coins.push_back(bn::move(k));
    }
    order = by_x(L.embers_n, [&](int i) { return L.embers[i].x; });
    for(int n = 0; n < L.embers_n; ++n)
    {
        int i = order[n];
        ember_t k; k.x = L.embers[i].x; k.y = L.embers[i].y; k.got = false; k.t = rng.get_int(100);
        embers.push_back(bn::move(k));
    }
    const int mushrooms_n = L.mushrooms_n - (hard ? 0 : L.hard_extra_mushrooms);
    order = by_x(mushrooms_n, [&](int i) { return L.mushrooms[i].x; });
    for(int n = 0; n < mushrooms_n; ++n)
    {
        int i = order[n];
        // Hard has fewer red mushrooms (2026-10-23), Expert one fewer still (2026-10-24)
        if((difficulty == 2 ? L.expert_skip_mushrooms : hard ? L.hard_skip_mushrooms : 0u) >> i & 1) continue;
        mush_t k; k.x = L.mushrooms[i].x; k.y = L.mushrooms[i].y; k.vy = 0; k.blue = L.mushrooms[i].blue;
        k.got = false; k.fall = false; k.boss = false; k.t = rng.get_int(100);
        mushes.push_back(bn::move(k));
    }
    int crack_count = 0;
    // Expert's own star block (World 4-1, 2026-10-25) goes in, and the level's own star block (if any) goes
    const bool own_star = difficulty == 2 && L.expert_star.x >= 0;
    const point star_at = own_star ? L.expert_star : L.star_block;
    if(own_star) grid[star_at.y * L.width + star_at.x] = 4;
    // Expert's own 1-up block (World 2-1, 2026-10-31) goes in, numbered after the level's for run.oneup_got
    const bool own_oneup = difficulty == 2 && L.expert_oneup.x >= 0;
    if(own_oneup) grid[L.expert_oneup.y * L.width + L.expert_oneup.x] = 4;
    for(int tx = 0; tx < L.width; ++tx) for(int ty = 0; ty < ROWS; ++ty) if(grid[ty * L.width + tx] == 4)
    {
        crack_t k; k.tx = tx; k.ty = ty; k.oneup = -1;
        for(int i = 0; i < L.oneup_blocks_n; ++i) if(L.oneup_blocks[i].x == tx && L.oneup_blocks[i].y == ty) k.oneup = i;
        if(own_oneup && L.expert_oneup.x == tx && L.expert_oneup.y == ty) k.oneup = L.oneup_blocks_n;
        if(star_at.x == tx && star_at.y == ty) k.oneup = CRACK_STAR;
        const bool old_star = own_star && L.star_block.x == tx && L.star_block.y == ty;
        if(old_star || (k.oneup >= 0 && (run.oneup_got[li] >> k.oneup & 1)) ||
           (k.oneup == CRACK_STAR && ! own_star && ((hard && L.hard_no_star) || (difficulty == 1 && L.hard_only_no_star) || (difficulty == 2 && L.expert_no_star))))
        {
            grid[ty * L.width + tx] = 0;   // each hidden 1-up can be claimed once per run
            continue;
        }
        if(k.oneup == -1) ++crack_count;   // (1-up and star blocks hold no coins)
        cracks.push_back(bn::move(k));
    }
    // The level's enemies on this difficulty (on Hard some long-patrolling aliens are two aliens each walking half the
    // stretch, 2026-10-23; Expert splits more, 2026-10-24): enemy_def::modes says which difficulties have each
    static int mine[256];
    int enemies_n = 0;
    for(int i = 0; i < L.enemies_n; ++i) if(L.enemies[i].modes >> difficulty & 1) mine[enemies_n++] = i;
    auto pick = [&](int i) -> const enemy_def& { return L.enemies[mine[i]]; };
    order = by_x(enemies_n, [&](int i) { return pick(i).a; });
    enemy_reach = 0;
    for(int n = 0; n < enemies_n; ++n)
    {
        const enemy_def& d = pick(order[n]);
        enemy_reach = bn::max(enemy_reach, d.b - d.a);
        enemy_t k; k.x = d.x; k.y = k.y0 = d.y; k.w = d.w; k.h = d.h; k.a = d.a; k.b = d.b; k.kind = d.kind >= 0 ? d.kind : L.foe;
        k.alive = true; k.alert = false; k.squish = 0; k.dir = 1; k.cool = 30; k.windup = 0; k.hit = 0; k.t = rng.get_int(48); k.volley = 0;
        k.rise = RISE_END;   // only reapers start buried
        if(k.kind == FOE_ALIEN || k.kind == FOE_MECH) { k.vx = fixed(0.9); k.hp = k.kind == FOE_MECH ? (hard ? HARD_MECH_UNIT_HP : MECH_UNIT_HP) : hard ? HARD_ALIEN_HP : ALIEN_HP; }
        else if(k.kind == FOE_REAPER) { k.vx = fixed(0.6); k.hp = difficulty == 2 ? EXPERT_REAPER_HP : hard ? HARD_REAPER_HP : REAPER_HP; k.rise = 0; }
        else if(k.kind == FOE_DEMON) { k.vx = fixed(0.7); k.hp = DEMON_HP; k.cool = 0; }   // (cool: its back-off time)
        else { k.vx = 0; k.hp = hard ? HARD_BEAST_HP : BEAST_HP; k.dir = -1; }
        enemies.push_back(bn::move(k));
    }
    for(int i = 0; i < L.movers_n; ++i)
    {
        const mover_def& m = L.movers[i];
        mover_t k; k.x0 = m.x0; k.y0 = m.y0; k.x1 = m.x1; k.y1 = m.y1; k.w = m.w; k.period = m.period; k.t = 0;
        k.x = k.x0; k.y = k.y0; k.dx = 0; k.dy = 0;
        movers.push_back(bn::move(k));
    }
    for(int i = 0; i < L.checks_n; ++i)
    {
        if(hard && i == L.hard_skip_check) continue;   // Hard has one checkpoint fewer (2026-10-23)
        check_t k; k.x = L.checks[i].x; k.y = L.checks[i].y; k.row = L.checks[i].row; k.lit = false;
        checks.push_back(bn::move(k));
    }
    has_door = L.has_door;
    door = { fixed(L.door.x), fixed(L.door.y), 16, 32 };
    make_boss();
    // The level data's start is the web game's, for its 24 px tall player (WEB_PH): his feet go where that player's were
    // (2026-10-29: used as is, the taller hero started with his feet 16 px into the ground)
    respawn_x = L.start.x; respawn_y = L.start.y + WEB_PH - PH;
    ember_total = embers.size();
    coin_total = coins.size() + crack_count * L.block_coins;
    lvl = lvl_t();
    jump_buf = 0;
    spawn(respawn_x, respawn_y);
    cam_x = clamp_cam_x(p.x - SCREEN_W / 2);
    cam_y = clamp_cam_y(p.y + p.h / 2 - SCREEN_H / 2);
    coins_static = coins.size(); mushes_static = mushes.size();
    create_backgrounds();
    if(level_map_ram_on && map_was_ram && bg_level) bn::regular_bg_map_ptr(bg_level->map()).reload_cells_ref();
    reset_active();
    current_music = nullptr;   // restart the level's track from the top
    if(bn::music::playing()) bn::music::stop();
}

// ---------- Player ----------
void die()
{
    if(p.dead) return;
    flame_len = 0;
    p.dead = 1; ++lvl.deaths; lvl.hurt = true; shake = 8;
    if(! time_attack) --run.lives;   // (unlimited lives in Time Attack)
    p.star = 0;
    // A flamethrower or rocket launcher still in hand goes back to its spot (the Fire Skull's dropped launcher
    // comes back with the fight instead, see reset_boss)
    const level_info& L = LV();
    if(p.flame > 0 && L.flame_item.x >= 0)
    {
        flame_item = flame_item_t();
        flame_item.x = L.flame_item.x * T + 8; flame_item.y = (L.flame_item.y + 1) * T - 9; flame_item.live = true;
    }
    if(p.rocket > 0 && has_rocket_item(L))
    {
        rocket_item = flame_item_t();
        rocket_item.x = L.rocket_item.x * T + 8; rocket_item.y = (L.rocket_item.y + 1) * T - 9; rocket_item.live = true;
    }
    if(run.lives <= 0) music_stop();   // clear the air for the death sound and the game-over music
    BURST(p.x + p.w / 2, p.y + 6, 28, fixed(2.6), fixed(0.1), 45, P_WHITE, P_EMBER, P_FLAME);
    sfx::die();
}

// Down: he lies prone, PRONE_H tall (user, 2026-10-29: the prone sprite's height, 10 px, 15, then 20; 23 since 2026-10-30): with the rifle, and
// since later that day with the flamethrower and the rocket launcher too (they crouched, CH tall, before). The picture is
// his main weapon's (weapon_set), not the brief grenade throw's.
int weapon_set() { return p.flame > 0 ? HERO_FLAME : p.rocket > 0 ? HERO_ROCKET : HERO_RIFLE; }
int crouch_h() { return PRONE_H; }

// Is there room for him to be h tall, feet where they are? (Checks every tile row the taller box would add.)
bool room_for(int h)
{
    fixed feet = p.y + p.h;
    if(h <= p.h) return true;
    int t0 = tdiv(feet - h), t1 = tdiv(p.y - EPS), l = tdiv(p.x), r = tdiv(p.x + p.w - EPS);
    for(int ty = t0; ty <= t1; ++ty) for(int tx = l; tx <= r; ++tx) if(solid(tile(tx, ty))) return false;
    return true;
}
bool can_stand() { return room_for(PH); }

// Sets his height, feet where they are
void set_height(int h) { p.y += p.h - h; p.h = h; }

void set_crouch(bool on)
{
    if(p.crouch == on) return;
    p.crouch = on;
    set_height(on ? crouch_h() : PH);
}

// Lying prone, his weapon and boots reach well past his hitbox (user, 2026-10-29: the rifle went into cracked blocks), so
// the whole prone picture (hero_prone_x0..x1 of his weapon from his middle, mirrored facing left like the draw) has to be clear of
// walls and blocks; and he has to be able to stand up where he ends up
bool prone_now() { return p.crouch && p.h == PRONE_H; }
bool prone_clear(fixed x, int face)
{
    fixed cx = x + p.w / 2, feet = p.y + p.h;
    const int ws = weapon_set(), x0 = hero_prone_x0[ws], x1 = hero_prone_x1[ws];
    fixed l = face > 0 ? cx + x0 : cx + 1 - x1;
    return ! in_wall(l, feet - PRONE_H, x1 - x0, PRONE_H) && ! in_wall(x, feet - PH, p.w, PH);
}
// Where he comes back at a checkpoint in tile column x: on its middle, or against whichever side keeps him (wider than a
// tile since 2026-10-30) out of a wall or spikes beside it
fixed check_respawn_x(fixed x, fixed y)
{
    const fixed spots[3] = { x + (T - PW) / 2, x + T - PW, x };
    for(fixed s : spots)
    {
        bool clear = ! in_wall(s, y, PW, PH);
        for(int ty = tdiv(y); ty <= tdiv(y + PH - 1); ++ty)
            for(int tx = tdiv(s); tx <= tdiv(s + PW - 1); ++tx) if(tile(tx, ty) == 3) clear = false;
        if(clear) return s;
    }
    return spots[0];
}
// Whether sliding him along the floor to x is blocked: by a wall (at his standing height), spikes, or the floor ending
bool slide_blocked(fixed x)
{
    fixed feet = p.y + p.h;
    if(in_wall(x, feet - PH, p.w, PH)) return true;
    int l = tdiv(x), r = tdiv(x + p.w - 1), fy = tdiv(feet - 1) + 1;
    bool floor = false;
    for(int tx = l; tx <= r; ++tx)
    {
        for(int ty = tdiv(feet - p.h); ty < fy; ++ty) if(tile(tx, ty) == 3) return true;
        int t = tile(tx, fy);
        if(solid(t) || t == 2) floor = true;
    }
    return ! floor;
}
// Moves him the least distance (backing away from what's ahead first) to where it's clear; false if nowhere within reach.
// He only slides along the floor: the search each way stops at the first wall, spikes or drop.
bool fit_prone(int face)
{
    bool back = true, ahead = true;
    const int ws = weapon_set();
    for(int d = 0; d <= hero_prone_x1[ws] - hero_prone_x0[ws] && (back || ahead); ++d)
    {
        if(back && d && slide_blocked(p.x - d * face)) back = false;
        if(back && prone_clear(p.x - d * face, face)) { p.x -= d * face; return true; }
        if(ahead && d && slide_blocked(p.x + d * face)) ahead = false;
        if(ahead && d && prone_clear(p.x + d * face, face)) { p.x += d * face; return true; }
    }
    return false;
}

void hurt(fixed src_x, bool spike)
{
    if(p.dead || p.inv > 0 || p.star > 0) return;   // nothing hurts Mario with the star (a pit still kills)
    lvl.hurt = true;
    p.hp -= spike ? SPIKE_DMG : 1;
    if(p.hp <= 0) { die(); return; }
    p.inv = HURT_INV;
    // No knockback away from what hit him and no hop (user, 2026-10-23): he keeps his own speed. Spikes still bounce
    // him up and off them.
    (void) src_x;
    if(spike) { p.vy = fixed(-6.5); p.ground = false; p.on_mover = -1; }
    shake = bn::max(shake, fixed(4));
    BURST(p.x + p.w / 2, p.y + p.h / 2, 10, fixed(1.8), fixed(0.1), 24, P_RED, P_EMBER, P_WHITE);
    sfx::hurt();
}

void break_block(int tx, int ty)
{
    grid[ty * level_w() + tx] = 0;
    fixed cx = tx * T + 8, cy = ty * T + 8;
    BURST(cx, cy - crack_rise / 2, 16, fixed(2.2), fixed(0.15), 30, P_STONE, P_STONE, P_STONE_DARK);   // (the middle of the tall block)
    for(crack_t& c : cracks)
    {
        if(c.tx != tx || c.ty != ty) continue;
        c.spr.reset();
        if(c.oneup == CRACK_STAR)
        {
            // The star pops up out of the block and lands back where it was
            star_item.x = cx - 8; star_item.y = cy - 8; star_item.vy = fixed(-3.5); star_item.live = true;
            shake = bn::max(shake, fixed(2)); sfx::crack(); sfx::star_out();
            return;
        }
        if(c.oneup >= 0 && ! oneups.full())
        {
            oneup_t u; u.x = cx - 7; u.y = cy - 7; u.vx = 0; u.vy = fixed(-3.5); u.rest = false; u.got = false; u.age = 0; u.t = 0; u.block = c.oneup;
            // A 1-up block on a ledge edge would drop its mushroom right beside the pit, and grabbing it on the run sends
            // Mario over the edge. So it hops about 3.5 tiles away from the drop instead, leaving room to stop.
            auto footing = [](int t) { return solid(t) || t == 2; };
            bool drop_l = ! footing(tile(tx - 1, ty + 1)), drop_r = ! footing(tile(tx + 1, ty + 1));
            if(drop_l && ! drop_r) u.vx = fixed(1.6);
            else if(drop_r && ! drop_l) u.vx = fixed(-1.6);
            oneups.push_back(bn::move(u));
            shake = bn::max(shake, fixed(2)); sfx::crack();
            return;
        }
    }
    const int n = LV().block_coins;
    for(int i = 0; i < n && ! coins.full(); ++i)
    {
        coin_t k; k.x = cx; k.y = cy; k.vx = fixed(2 * i - (n - 1)) * fixed(0.06); k.vy = fixed(-3.2) - fixed(i % 2) * fixed(0.8);
        k.rest = false; k.got = false; k.t = i * 20; k.age = 0;
        coins.push_back(bn::move(k));
    }
    shake = bn::max(shake, fixed(2)); sfx::crack();
}

// Where the barrel's tip is in Mario's aim-up and 45-degree frames (the user's art; build-assets.js logs both): straight up,
// right over his middle 41 px above his feet; at 45 degrees, 8.5 px ahead and 36 px up
// The hero's frame (the user's soldier since 2026-10-29; see hero_frames.h): aiming up or at 45 degrees has one pose
// each, then crouching, in the air (rising, near the top, falling), walking (8 frames) and standing still
int hero_gfx()
{
    if(p.throw_t > 0) return bn::min(HERO_THROW_N - 1, (p.throw_t - 1) / THROW_STEP);   // (hero_grenade's frames)
    if(p.aim == AIM_UP) return HERO_UP;
    if(p.aim == AIM_DIAG) return HERO_DIAG;
    if(p.crouch) return HERO_CROUCH;
    if(! p.ground) return HERO_AIR + (p.vy < fixed(-1.5) ? 0 : p.vy < fixed(1.5) ? 1 : 2);
    if(bn::abs(p.vx) > fixed(0.4)) return HERO_WALK + p.step.floor_integer() % HERO_WALK_N;
    return HERO_IDLE;
}
// The weapon he's drawn with (user, 2026-10-29): the flamethrower or the rocket launcher while he has one, and throwing a
// grenade while he does (hero_grenade; it was the grenade launcher until the user's throw art, same day); the rifle otherwise
int hero_set()
{
    if(p.throw_t > 0) return HERO_GRENADE;
    if(p.flame > 0) return HERO_FLAME;
    if(p.rocket > 0) return HERO_ROCKET;
    return HERO_RIFLE;
}
// Shots leave from the weapon's tip in the frame being shown (body centre = p.x + p.w / 2, feet = p.y + p.h)
fixed tip_x() { return fixed(hero_tip_x2[hero_set()][hero_gfx()]) / 2; }
fixed tip_y() { return fixed(hero_tip_y2[hero_set()][hero_gfx()]) / 2; }
fixed muzzle_x() { return p.x + p.w / 2 + p.face * tip_x(); }
fixed muzzle_y() { return p.y + p.h - tip_y(); }

// The way the gun points, per axis (both at 45 degrees, each scaled by DIAG)
int aim_dx() { return p.aim == AIM_UP ? 0 : p.face; }
int aim_dy() { return p.aim == AIM_AHEAD ? 0 : -1; }

// A rocket hitting spikes (user request, 2026-10-19): those within SPIKE_BLAST tiles of the hit, along its row, are gone,
// from the grid (no more harm) and from the level's map (level_map_ram; see load_level)
void blast_spikes(int tx, int ty)
{
    bool any = false;
    for(int x = tx - SPIKE_BLAST; x <= tx + SPIKE_BLAST; ++x)
    {
        if(tile(x, ty) != 3) continue;
        grid[ty * level_w() + x] = 0;
        any = true;
        if(! level_map_ram_on) continue;
        int mw = level_bgs[li]->map_item().dimensions().width();
        bn::regular_bg_map_cell empty = level_map_ram[0];   // the top-left corner (tile row 1) is open sky: a blank tile
        for(int cy = ty * 2 - MAP_RAM_SKIP; cy < ty * 2 + 2 - MAP_RAM_SKIP; ++cy)
            for(int cx = x * 2; cx < x * 2 + 2; ++cx) level_map_ram[cy * mw + cx] = empty;
    }
    if(any && level_map_ram_on && bg_level) bn::regular_bg_map_ptr(bg_level->map()).reload_cells_ref();
}

void shot_hit(enemy_t& e, int dmg = 1);

// A rocket going off: the user's explosion sound, a 64x64 fireball, sparks flying out of it and a shake. The blast also
// hurts every enemy within ROCKET_SPLASH px of it (user request, 2026-10-21) for ROCKET_SPLASH_DMG; direct = the enemy
// the rocket itself hit, which already took ROCKET_DMG
void explode(fixed x, fixed y, const enemy_t* direct = nullptr, fixed radius = ROCKET_SPLASH, int dmg = ROCKET_SPLASH_DMG)
{
    for(enemy_t& e : active(enemies, enemy_span))
    {
        if(! e.alive || e.rise < RISE_END || &e == direct) continue;
        // distance from the blast to the nearest point of the enemy's box
        fixed nx = bn::max(e.x, bn::min(x, e.x + e.w)) - x, ny = bn::max(e.y, bn::min(y, e.y + e.h)) - y;
        if(bn::abs(nx) > radius || bn::abs(ny) > radius) continue;
        if(nx * nx + ny * ny <= radius * radius) shot_hit(e, dmg);
    }
    sfx::rocket_hit();
    if(booms.full()) booms.erase(booms.begin());
    boom_t b; b.x = x; b.y = y; b.t = 0;
    booms.push_back(bn::move(b));
    BURST(x, y, 6, fixed(2.6), fixed(0.05), 24, P_FLAME, P_EMBER, P_GOLD, P_WHITE);   // few sparks: each is a sprite (CPU)
    shake = bn::max(shake, fixed(5));
}

// A grenade out of his hand, thrown the way he faces (see GRENADE_START), as his throwing animation lets go of it
void throw_grenade()
{
    --run.grenades; ++lvl.grenades;
    if(grenades.full()) grenades.erase(grenades.begin());
    // From his hand in the throw's last frame (one in a wall just goes off there)
    grenade_t g; g.x = muzzle_x(); g.y = muzzle_y(); g.t = 0;
    g.vx = p.face * (p.aim == AIM_UP ? fixed(0.8) : p.aim == AIM_DIAG ? fixed(2.6) : fixed(3.2)) + p.vx / 2;
    g.vy = p.aim == AIM_UP ? fixed(-5.5) : p.aim == AIM_DIAG ? fixed(-4.5) : fixed(-3);
    grenades.push_back(bn::move(g));
    sfx::launch();
}

// A grenade going off: the rocket's explosion with a wider, harder blast that also reaches the boss and the cracked
// blocks beside it
void grenade_blast(fixed x, fixed y)
{
    explode(x, y, nullptr, GRENADE_RADIUS, GRENADE_DMG);
    if(boss_active() && boss->state == boss_t::fight)
    {
        rect b = boss_box(*boss);
        fixed nx = bn::max(b.x, bn::min(x, b.x + b.w)) - x, ny = bn::max(b.y, bn::min(y, b.y + b.h)) - y;
        if(bn::abs(nx) <= GRENADE_RADIUS && bn::abs(ny) <= GRENADE_RADIUS && nx * nx + ny * ny <= GRENADE_RADIUS * GRENADE_RADIUS)
        {
            hit_boss(GRENADE_DMG);
            boss_burst(boss->kind, x, y, 6, fixed(1.4), fixed(0.05), 16);
        }
    }
    int cx = tdiv(x), cy = tdiv(y);
    for(int ty = cy - 1; ty <= cy + 1; ++ty) for(int tx = cx - 1; tx <= cx + 1; ++tx) if(tile(tx, ty) == 4) break_block(tx, ty);
}

void update_grenades()
{
    if(p.grenade_cool > 0) --p.grenade_cool;
    for(grenade_t& g : grenades)
    {
        ++g.t;
        g.vy = bn::min(g.vy + GRENADE_GRAVITY, fixed(6));
        fixed oy = g.y;
        g.x += g.vx; g.y += g.vy;
        int tx, ty, t = tile_px(g.x, g.y, tx, ty);
        bool boom = g.t >= GRENADE_FUSE || t == 1 || t == 3 || t == 4 || (t == 2 && g.vy > 0 && oy < ty * T + 1);
        if(! boom && boss_active() && boss->state == boss_t::fight && overlap(rect{ g.x - 3, g.y - 3, 6, 6 }, boss_box(*boss))) boom = true;
        if(! boom)
            for(enemy_t& e : active(enemies, enemy_span))
                if(e.alive && e.rise >= RISE_END && g.x > e.x - 3 && g.x < e.x + e.w + 3 && g.y > e.y - 3 && g.y < e.y + e.h + 3) { boom = true; break; }
        if(boom)
        {
            // (backed out of the wall or floor it hit, so the blast is in the open)
            if(t != 0) { g.x -= g.vx; g.y -= g.vy; }
            grenade_blast(g.x, g.y);
            g.t = -1;
        }
        else if(g.y > ROWS * T + 16) g.t = -1;   // down a pit
    }
    for(int i = 0; i < grenades.size(); ) { if(grenades[i].t < 0) grenades.erase(grenades.begin() + i); else ++i; }
}

void shoot()
{
    bool rocket = p.rocket > 0;
    p.cool += rocket ? fixed(difficulty == 2 ? EXPERT_ROCKET_DELAY : difficulty == 1 ? HARD_ROCKET_DELAY : ROCKET_DELAY)
                   : FIRE_DELAY;
    p.flash = 4;
    fixed mx = muzzle_x(), my = muzzle_y();
    int dx = aim_dx(), dy = aim_dy();
    fixed k = dx && dy ? DIAG : fixed(1);
    if(rocket) sfx::launch(); else sfx::shoot();
    // The barrel reaches past the hero's body, so check the tiles between body and muzzle first: from his middle, or from
    // the top of his hitbox when aiming up
    // (a rocket starts with its nose 7 + ROCKET_NOSE px past the muzzle, so for one the check goes on to there: lying prone
    // with the launcher's tip against a cracked block, the rocket would otherwise start beyond it; 2026-10-29)
    fixed reach = p.aim == AIM_UP ? bn::max(fixed(0), tip_y() - p.h) : p.aim == AIM_DIAG ? tip_x() / DIAG : tip_x();
    fixed bx = mx - dx * k * reach, by = my - dy * k * reach;
    if(rocket) reach += 7 + ROCKET_NOSE;
    for(fixed s = 0; ; s = bn::min(reach, s + 4))
    {
        fixed sx = bx + dx * k * s, sy = by + dy * k * s;
        int tx, ty, t = tile_px(sx, sy, tx, ty);
        if(t == 4) { break_block(tx, ty); if(rocket) explode(sx, sy); return; }
        if(t == 3 && rocket) { blast_spikes(tx, ty); explode(sx, sy); return; }
        if(t == 1)
        {
            if(rocket) explode(sx, sy);
            else BURST(sx, sy, 5, fixed(1.2), fixed(0.05), 14, P_GOLD, P_FLAME, P_WHITE);
            return;
        }
        if(s == reach) break;
    }
    if(! bullets.full())
    {
        bullet_t b; b.x = rocket ? mx + dx * k * 7 : mx; b.y = rocket ? my + dy * k * 7 : my; b.rocket = rocket;   // a rocket's tail at the muzzle
        fixed speed = (rocket ? ROCKET_SPEED : fixed(6.5)) * k;
        b.vx = dx * speed; b.vy = dy * speed; b.life = rocket ? 90 : 70;
        bullets.push_back(bn::move(b));
    }
    BURST(mx, my, 3, fixed(0.8), 0, 8, P_GOLD, P_PALE);
}

// How far the flame reaches from the muzzle (px), up to FLAME_RANGE: it stops at a wall or a cracked block (whose tile
// goes in btx, bty, else btx = -1). Scanned from Mario's middle, as the barrel itself can be in a wall.
int flame_reach(int& btx, int& bty)
{
    btx = -1;
    if(p.aim == AIM_DIAG)
    {
        // At 45 degrees, scanned from Mario's middle (14 px back along the barrel); it stops just short of what it meets
        fixed mx = muzzle_x(), my = muzzle_y();
        for(fixed s = 0; s < fixed(14) + FLAME_RANGE; s += 3)
        {
            int tx, ty, t = tile_px(mx + p.face * (s - 14) * DIAG, my - (s - 14) * DIAG, tx, ty);
            if(t != 1 && t != 4) continue;
            if(t == 4) { btx = tx; bty = ty; }
            return bn::max(0, bn::min(FLAME_RANGE, (s - 16).floor_integer()));
        }
        return FLAME_RANGE;
    }
    if(p.aim == AIM_UP)
    {
        // Straight up, scanned from Mario's head
        fixed my = muzzle_y();
        int tx = tdiv(muzzle_x());
        for(fixed s = 0; s < fixed(10) + FLAME_RANGE; s += 4)
        {
            int ty = tdiv(my + 10 - s), t = tile(tx, ty);
            if(t != 1 && t != 4) continue;
            if(t == 4) { btx = tx; bty = ty; }
            return bn::max(0, bn::min(FLAME_RANGE, (my - (ty + 1) * T).floor_integer()));
        }
        return FLAME_RANGE;
    }
    fixed cx = p.x + p.w / 2, mx = muzzle_x(), my = muzzle_y();
    for(fixed s = 0; s < fixed(20.5) + FLAME_RANGE; s += 4)
    {
        int tx, ty, t = tile_px(cx + p.face * s, my, tx, ty);
        if(t != 1 && t != 4) continue;
        if(t == 4) { btx = tx; bty = ty; }
        fixed edge = p.face > 0 ? fixed(tx * T) - mx : mx - (tx + 1) * T;
        return bn::max(0, bn::min(FLAME_RANGE, edge.floor_integer()));
    }
    return FLAME_RANGE;
}

// Whether o is in the flame: the rect r when it points ahead or up; at 45 degrees, a chain of 12x12 squares along it
bool in_flame(const rect& o, const rect& r, fixed mx, fixed my)
{
    if(p.aim != AIM_DIAG) return overlap(r, o);
    for(int d = 6; d < flame_len + 6; d += 8)
    {
        fixed cx = mx + p.face * d * DIAG, cy = my - d * DIAG;
        if(overlap({ cx - 6, cy - 6, 12, 12 }, o)) return true;
    }
    return false;
}

// One "shot" of the flame: everything in it is hit at once
void flame_tick(int btx, int bty)
{
    p.cool += FIRE_DELAY;
    if(btx >= 0) { break_block(btx, bty); return; }   // a cracked block in the way burns first
    if(flame_len <= 0) return;
    fixed mx = muzzle_x(), my = muzzle_y();
    rect r = p.aim == AIM_UP ? rect{ mx - 7, my - flame_len, 14, fixed(flame_len) } : rect{ p.face > 0 ? mx : mx - flame_len, my - 7, fixed(flame_len), 14 };
    fixed k = p.aim == AIM_DIAG ? DIAG : fixed(1), ex = mx + aim_dx() * k * flame_len, ey = my + aim_dy() * k * flame_len;   // the flame's far end
    bool burned = false;
    for(enemy_t& e : active(enemies, enemy_span))
        if(e.alive && e.rise >= RISE_END && in_flame({ e.x, e.y, fixed(e.w), fixed(e.h) }, r, mx, my)) { shot_hit(e, FLAME_DMG); burned = true; }
    if(boss_active() && boss->state == boss_t::fight && in_flame(boss_box(*boss), r, mx, my))
    {
        hit_boss(FLAME_DMG); burned = true;
        boss_burst(boss->kind, ex, ey, 4, fixed(1.2), fixed(0.05), 14);
    }
    if(burned) sfx::fire_crackle();
    for(glob_t& g : globs)
    {
        if(g.life <= 0 || ! in_flame({ g.x - 3, g.y - 3, 6, 6 }, r, mx, my)) continue;
        g.life = 0;
        BURST(g.x, g.y, 6, fixed(1.2), fixed(0.05), 16, P_FLAME, P_EMBER, P_GOLD);
    }
    // Sparks where the flame meets a wall
    if(flame_len < FLAME_RANGE) BURST(ex, ey, 3, fixed(1), fixed(0.03), 12, P_FLAME, P_EMBER, P_GOLD);
}

void collect_coin(coin_t& c)
{
    c.got = true; ++lvl.coins; ++run.coins; add_score(SCORE_COIN);
    BURST(c.x, c.y, 6, 1, fixed(0.02), 16, P_COIN, P_PALE);
    sfx::coin();
    if(++run.life_coins >= COINS_PER_LIFE)
    {
        run.life_coins = 0; ++run.lives; ++lvl.coin_lives; sfx::oneup();
        float_text(p.x + p.w / 2, p.y - 6, "1UP");
    }
}

void clear_level();
void game_over();
void kill(enemy_t& e);

void update_player()
{
    if(p.dead)
    {
        ++p.dead;
        if(time_attack) ++lvl.time;   // in Time Attack a death costs the time it takes to come back
        // On the last life, let the death sound finish before the game-over screen
        int wait = run.lives <= 0 ? 80 : 50;
        if(p.dead > wait)
        {
            if(run.lives <= 0) { game_over(); return; }
            spawn(respawn_x, respawn_y); reset_boss();
            cam_x = clamp_cam_x(p.x - SCREEN_W / 2);
            cam_y = clamp_cam_y(p.y + p.h / 2 - SCREEN_H / 2);
        }
        return;
    }
    ++lvl.time;
    // Crouch: hold down on the ground to duck under shots
    bool want_crouch = K.down && p.ground;
    // (going prone he backs off a wall in front of his rifle; with no room anywhere near he stays up)
    if(want_crouch && ! p.crouch && (crouch_h() != PRONE_H || fit_prone(p.face))) set_crouch(true);
    else if(! want_crouch && p.crouch && can_stand()) set_crouch(false);
    // His weapon changed while he's down (prone <-> crouching): shrink, or grow if there's room
    if(p.crouch && p.h != crouch_h() && room_for(crouch_h()))
    {
        if(crouch_h() != PRONE_H || fit_prone(p.face)) set_height(crouch_h());
        else if(can_stand()) set_crouch(false);
    }
    p.aim = ! K.up || p.crouch ? AIM_AHEAD : K.left || K.right ? AIM_DIAG : AIM_UP;   // standing, running or jumping; crouching aims ahead
    int dir = (K.right ? 1 : 0) - (K.left ? 1 : 0);
    if(dir && (dir == p.face || ! prone_now() || fit_prone(dir))) p.face = dir;   // prone, he turns only if his rifle fits
    fixed acc = p.ground ? fixed(0.38) : fixed(0.26), max = fixed(2.3);
    if(p.crouch) { p.vx *= fixed(0.82); if(bn::abs(p.vx) < fixed(0.05)) p.vx = 0; }
    else if(dir) { p.vx += dir * acc; if(bn::abs(p.vx) > max) p.vx = p.vx > 0 ? max : -max; }
    else { p.vx *= p.ground ? fixed(0.7) : fixed(0.9); if(bn::abs(p.vx) < fixed(0.05)) p.vx = 0; }

    fixed ox = p.x;
    if(p.on_mover >= 0) { p.x += movers[p.on_mover].dx; p.y = movers[p.on_mover].y - p.h; }

    p.coyote = p.ground ? 6 : bn::max(0, p.coyote - 1);
    if(jump_buf > 0) --jump_buf;
    if(jump_buf > 0 && p.coyote > 0 && (! p.crouch || can_stand()))
    {
        if(p.crouch) set_crouch(false);
        p.vy = fixed(-7.2); p.ground = false; p.coyote = 0; jump_buf = 0; p.on_mover = -1;
        BURST(p.x + p.w / 2, p.y + p.h, 6, 1, fixed(0.05), 18, P_DUST, P_STONE);
        sfx::jump();
    }
    if(! K.jump && p.vy < fixed(-2.6)) p.vy = fixed(-2.6);
    p.vy = bn::min(p.vy + fixed(0.42), fixed(8.5));

    // Horizontal
    p.x += p.vx;
    fixed mvx = p.x - ox;
    // While down he still collides as tall as when standing, so his slide can't take him under a low ledge he couldn't
    // stand up from (lying prone he's shorter than a tile, and he can't move while down)
    fixed feet = p.y + p.h;
    int top = tdiv(p.crouch ? feet - PH : p.y), bot = tdiv(feet - EPS);
    if(mvx > 0)
    {
        int tx = tdiv(p.x + p.w - EPS);
        for(int ty = top; ty <= bot; ++ty) if(wall_at(tx, ty, feet)) { p.x = tx * T - p.w; p.vx = 0; break; }
    }
    else if(mvx < 0)
    {
        int tx = tdiv(p.x);
        for(int ty = top; ty <= bot; ++ty) if(wall_at(tx, ty, feet)) { p.x = (tx + 1) * T; p.vx = 0; break; }
    }
    // (his slide or a moving platform can't push his prone rifle or boots into a wall either; standing, the weapon may overlap walls so he can walk right up to them -- user, 2026-10-29)
    fixed slid_x = p.x;
    if(prone_now() && fit_prone(p.face) && p.x != slid_x) p.vx = 0;

    // Vertical
    bool was_ground = p.ground;
    p.pb = p.y + p.h;
    p.y += p.vy; p.ground = false; p.on_mover = -1;
    int l = tdiv(p.x), r = tdiv(p.x + p.w - EPS);
    if(p.vy > 0)
    {
        // (a cracked block's raised top is solid too: tile_px gives the block, tile_top its top)
        for(int x = l; x <= r; ++x)
        {
            int tx, ty, t = tile_px(x * T, p.y + p.h, tx, ty);
            if(solid(t) || (t == 2 && p.pb <= ty * T + fixed(0.01))) { p.y = tile_top(tx, ty) - p.h; p.vy = 0; p.ground = true; break; }
        }
    }
    else if(p.vy < 0)
    {
        int ty = tdiv(p.y);
        for(int tx = l; tx <= r; ++tx) if(solid(tile(tx, ty))) { p.y = (ty + 1) * T; p.vy = 0; break; }
    }
    if(p.vy >= 0)
    {
        for(int i = 0; i < movers.size(); ++i)
        {
            const mover_t& m = movers[i];
            if(p.x + p.w > m.x && p.x < m.x + m.w && p.pb <= m.y - m.dy + fixed(1.5) && p.y + p.h >= m.y)
            {
                p.y = m.y - p.h; p.vy = 0; p.ground = true; p.on_mover = i; break;
            }
        }
    }
    if(p.ground && ! was_ground) BURST(p.x + p.w / 2, p.y + p.h, 4, fixed(0.8), fixed(0.04), 14, P_DUST);
    // The walk cycle: the soldier's 8 frames (two strides) over about 44 px (Mario's 3 frames took 43)
    if(p.ground && bn::abs(p.vx) > fixed(0.4)) { p.step += bn::abs(p.vx) * fixed(0.135); if(p.step >= HERO_WALK_N) p.step -= HERO_WALK_N; }

    // Gun
    if(p.cool > 0) p.cool -= 1;
    if(p.flash > 0) --p.flash;
    if(p.inv > 0) --p.inv;
    if(p.star > 0) --p.star;
    if(p.flame > 0 && --p.flame == 0) sfx::flame_out();
    if(p.rocket > 0 && --p.rocket == 0) sfx::rocket_out();
    flame_len = 0;
    // (no shooting while he throws a grenade: his hands are busy, and the throw frames have no gun to shoot from)
    if(p.throw_t > 0) {}
    else if(p.flame > 0)
    {
        if(K.fire)
        {
            int btx, bty;
            flame_len = flame_reach(btx, bty);
            if(p.cool <= 0) flame_tick(btx, bty);
        }
    }
    else if(K.fire && p.cool <= 0) shoot();
    // R starts a throw (not while crouching or lying prone, on any difficulty: user, 2026-10-29); the grenade leaves his
    // hand at THROW_RELEASE
    if(K.grenade && run.grenades > 0 && p.grenade_cool <= 0 && p.throw_t == 0 && ! p.crouch) { p.throw_t = 1; p.grenade_cool = GRENADE_DELAY; }
    else if(p.throw_t > 0)
    {
        ++p.throw_t;
        if(p.throw_t == THROW_RELEASE && run.grenades > 0) throw_grenade();
        if(p.throw_t > THROW_END) p.throw_t = 0;
    }

    // Hazards
    rect hb = { p.x + 2, p.y + 2, fixed(p.w - 4), fixed(p.h - 2) };
    for(int ty = tdiv(hb.y); ty <= tdiv(hb.y + hb.h); ++ty)
        for(int tx = tdiv(hb.x); tx <= tdiv(hb.x + hb.w); ++tx)
            if(tile(tx, ty) == 3 && hb.y + hb.h > ty * T + 7) { hurt(tx * T + 8, true); if(p.dead) return; }
    if(p.y > ROWS * T + 10) { die(); return; }

    rect pr = { p.x, p.y, fixed(p.w), fixed(p.h) };
    for(enemy_t& e : active(enemies, enemy_span))
    {
        // Only flying demons can be stomped: Mario coming down onto one deals DEMON_STOMP_DMG and bounces him off it (user,
        // 2026-10-20). Otherwise only gunfire destroys enemies, and touching one from any side hurts Mario. With the star,
        // touching one (or landing on it) kills it instead.
        if(! e.alive || e.rise < RISE_END || ! overlap(pr, { e.x, e.y, fixed(e.w), fixed(e.h) })) continue;
        if(p.star > 0) { kill(e); continue; }
        if(e.kind == FOE_DEMON && p.y + p.h > p.pb && p.pb <= e.y + 6)
        {
            shot_hit(e, DEMON_STOMP_DMG);
            if(e.alive) { e.cool = DEMON_BACK_OFF; sfx::stomp(); }
            p.y = e.y - p.h; p.vy = K.jump ? fixed(-6.5) : fixed(-4.5); p.ground = false; p.on_mover = -1;
            continue;
        }
        if(e.kind == FOE_DEMON) e.cool = DEMON_BACK_OFF;   // it backs off, so it doesn't sit on Mario where he can't shoot it
        hurt(e.x + e.w / 2); if(p.dead) return;
    }
    for(ember_t& em : active(embers, ember_span))
    {
        if(em.got) continue;
        fixed dx = p.x + p.w / 2 - em.x, dy = p.y + p.h / 2 - em.y;
        if(bn::abs(dx) < 11 && bn::abs(dy) < 16)
        {
            em.got = true; em.spr.reset(); ++lvl.embers; add_score(SCORE_EMBER);
            BURST(em.x, em.y, 12, fixed(1.6), fixed(0.02), 26, P_EMBER, P_GOLD, P_FLAME);
            sfx::ember();
            if(lvl.embers == ember_total)
            {
                add_score(SCORE_ALL_EMBERS, true, p.x + p.w / 2, p.y - 18);
                if(! run.ember_bonus[li])
                {
                    run.ember_bonus[li] = true; ++run.lives; sfx::oneup();
                    float_text(p.x + p.w / 2, p.y - 6, "ALL EMBERS 1UP");
                }
            }
        }
    }
    for(coin_t& c : active(coins, coin_span, coins_static))
    {
        if(c.got || c.age < 10) continue;
        if(c.x + 4 > p.x && c.x - 4 < p.x + p.w && c.y + 5 > p.y && c.y - 5 < p.y + p.h) { collect_coin(c); c.spr.reset(); }
    }
    for(oneup_t& u : oneups)
    {
        if(u.got || u.age < 10 || ! overlap(pr, { u.x, u.y, 14, 14 })) continue;
        u.got = true; u.spr.reset(); ++run.lives; sfx::oneup(); add_score(SCORE_ONEUP);
        run.oneup_got[li] |= 1 << u.block;
        BURST(u.x + 7, u.y + 7, 18, 2, fixed(0.02), 30, P_GREEN, P_WHITE, P_GREEN);
        float_text(p.x + p.w / 2, p.y - 6, "1UP");
    }
    if(flame_item.live && overlap(pr, { flame_item.x - 14, flame_item.y - 7, 28, 14 }))
    {
        flame_item.live = false; flame_item.spr.reset(); p.flame = FLAME_TIME; p.rocket = 0;
        sfx::flame_get();
        BURST(flame_item.x, flame_item.y, 20, 2, fixed(0.02), 30, P_FLAME, P_EMBER, P_GOLD);
        float_text(p.x + p.w / 2, p.y - 6, "FLAMETHROWER");
    }
    if(rocket_item.live && overlap(pr, { rocket_item.x - 14, rocket_item.y - 7, 28, 14 }))
    {
        rocket_item.live = false; rocket_item.spr.reset(); p.rocket = ROCKET_TIME; p.flame = 0;
        sfx::rocket_get();
        BURST(rocket_item.x, rocket_item.y, 20, 2, fixed(0.02), 30, P_GREEN, P_GOLD, P_WHITE);
        float_text(p.x + p.w / 2, p.y - 6, "ROCKET LAUNCHER");
    }
    if(star_item.live && ! star_item.got && star_item.age >= 10 && overlap(pr, { star_item.x + 1, star_item.y + 1, 14, 14 }))
    {
        star_item.got = true; star_item.spr.reset(); p.star = li == title_level ? STAR_TIME_HELL : STAR_TIME;   // (title_level: World 4-4)
        sfx::star();
        BURST(star_item.x + 8, star_item.y + 8, 20, 2, fixed(0.02), 30, P_GOLD, P_WHITE, P_EMBER);
    }
    // Mushrooms heal; at full health Mario leaves them for later
    for(mush_t& m : active(mushes, mush_span, mushes_static))
    {
        if(m.got || p.hp >= MAX_HP || ! overlap(pr, { m.x, m.y, 12, 12 })) continue;
        m.got = true; m.spr.reset(); p.hp = bn::min(MAX_HP, p.hp + (m.blue ? BLUE_MUSH_HEAL : MUSH_HEAL));
        if(m.blue) BURST(m.x + 6, m.y + 6, 20, fixed(1.8), fixed(0.02), 28, P_TEAL, P_WHITE, P_TEAL);
        else BURST(m.x + 6, m.y + 6, 16, fixed(1.8), fixed(0.02), 28, P_RED, P_WHITE, P_GOLD);
        sfx::mush();
    }
    for(check_t& c : checks)
    {
        if(! c.lit && overlap(pr, { c.x, c.y - 16, 16, 32 }))
        {
            c.lit = true; respawn_y = (c.row + 1) * T - PH; respawn_x = check_respawn_x(c.x, respawn_y);
            BURST(c.x + 8, c.y, 20, 2, fixed(0.03), 34, P_EMBER, P_GOLD, P_FLAME);
            sfx::check();
        }
    }
    if(has_door && overlap(pr, door)) clear_level();   // walking into the door goes through it (user, 2026-10-20; it took UP from 2026-10-17)
}

// ---------- World ----------
void kill(enemy_t& e)
{
    e.alive = false;
    add_score(e.kind == FOE_BEAST ? SCORE_BEAST : SCORE_ENEMY, true, e.x + e.w / 2, e.y - 4);
    if(e.kind == FOE_BEAST)
    {
        BURST(e.x + e.w / 2, e.y + e.h / 2, 24, fixed(2.8), fixed(0.06), 40, P_FLAME, P_EMBER, P_GOLD);
        shake = bn::max(shake, fixed(5)); sfx::stomp(); sfx::burn();
    }
    else if(e.kind == FOE_DEMON)
    {
        BURST(e.x + e.w / 2, e.y + e.h / 2, 24, fixed(2.6), fixed(0.1), 34, P_RED, P_ALIEN_RED, P_TEAL, P_WHITE);
        shake = bn::max(shake, fixed(3)); sfx::stomp();
    }
    else if(e.kind == FOE_REAPER)
    {
        BURST(e.x + e.w / 2, e.y + e.h / 2, 24, fixed(2.8), fixed(0.1), 38, P_RED, P_ALIEN_RED, P_WHITE, P_EMBER);
        shake = bn::max(shake, fixed(5)); sfx::stomp();
    }
    else if(e.kind == FOE_MECH)
    {
        BURST(e.x + e.w / 2, e.y + e.h / 2, 24, fixed(2.6), fixed(0.1), 34, P_EMBER, P_GOLD, P_STONE_DARK, P_TEAL);
        shake = bn::max(shake, fixed(3)); sfx::stomp();
    }
    else
    {
        BURST(e.x + e.w / 2, e.y + e.h / 2, 24, fixed(2.6), fixed(0.1), 34, P_ALIEN, P_ALIEN_LIGHT, P_ALIEN, P_ALIEN_RED);
        shake = bn::max(shake, fixed(3)); sfx::stomp();
    }
}

// Bullets are the only way to destroy enemies (flying demons can also be stomped): aliens and reapers take four gun hits, mech units and hell beasts five; one more each in Hard (see ALIEN_HP etc.)
void shot_hit(enemy_t& e, int dmg)
{
    if((e.hp -= dmg) > 0)
    {
        e.hit = 10;
        if(e.kind == FOE_BEAST) BURST(e.x + e.w / 2, e.y + 8, 6, fixed(1.2), fixed(0.05), 16, P_FLAME, P_EMBER, P_GOLD);
        else if(e.kind == FOE_MECH) BURST(e.x + e.w / 2, e.y + 8, 6, fixed(1.2), fixed(0.05), 16, P_GOLD, P_EMBER, P_WHITE);
        else if(e.kind == FOE_REAPER) BURST(e.x + e.w / 2, e.y + 8, 6, fixed(1.2), fixed(0.05), 16, P_RED, P_ALIEN_RED, P_WHITE);
        else if(e.kind == FOE_DEMON) BURST(e.x + e.w / 2, e.y + 8, 6, fixed(1.2), fixed(0.05), 16, P_RED, P_TEAL, P_WHITE);
        else BURST(e.x + e.w / 2, e.y + 8, 6, fixed(1.2), fixed(0.05), 16, P_ALIEN, P_ALIEN_LIGHT, P_ALIEN_RED);
        return;
    }
    kill(e);
}

// Whether a w x h box at x, y overlaps a wall or block (planks don't count)
bool in_wall(fixed x, fixed y, int w, int h)
{
    for(int tx = tdiv(x); tx <= tdiv(x + w - 1); ++tx)
        for(int ty = tdiv(y); ty <= tdiv(y + h - 1); ++ty)
            if(wall_at(tx, ty, ty == tdiv(y + h - 1) ? y + h : fixed((ty + 1) * T))) return true;
    return false;
}

void update_world()
{
    for(mover_t& m : movers)
    {
        // The count wraps each cycle: t * 360 overflows fixed (max 524287) after 1457 frames, which made platforms jump
        // about erratically from about 24 s into a level
        if(++m.t >= m.period) m.t = 0;
        fixed k = (1 - cos_deg(fixed(m.t) * 360 / m.period)) / 2;
        fixed nx = m.x0 + (m.x1 - m.x0) * k, ny = m.y0 + (m.y1 - m.y0) * k;
        m.dx = nx - m.x; m.dy = ny - m.y; m.x = nx; m.y = ny;
    }
    for(enemy_t& e : active(enemies, enemy_span))
    {
        if(! e.alive) { ++e.squish; continue; }
        if(e.kind == FOE_BEAST) { ++e.t; if(e.hit > 0) --e.hit; continue; }
        ++e.t; if(e.hit > 0) --e.hit;
        if(e.rise < RISE_END)
        {
            // Reapers lie buried until Mario comes close, then climb out of the grave
            bool near = state == state_t::play && ! p.dead && bn::abs(p.x - e.x) < RISE_RANGE;
            if(e.rise == 0 && near)
            {
                sfx::rise(); shake = bn::max(shake, fixed(3));
                BURST(e.x + e.w / 2, e.y + e.h, 14, fixed(2.2), fixed(0.1), 34, P_ALIEN, P_STONE_DARK, P_ALIEN_LIGHT);
            }
            if(e.rise > 0 || near) ++e.rise;
            continue;
        }
        if(e.kind == FOE_DEMON)
        {
            // Flying: it bobs 4 px up and down on patrol. When Mario comes near it flies straight at him (alert), and once
            // he's gone it flies back to its patrol (volley = 1 until it's there). It can't fly through walls.
            fixed bob = sin_deg(fixed((e.t * 4) % 360)) * 4;
            fixed mx = p.x + p.w / 2 - e.w / 2, my = p.y + p.h / 2 - e.h / 2;
            e.alert = state == state_t::play && ! p.dead && (e.minion || (bn::abs(mx - e.x) < DEMON_RANGE && bn::abs(my - e.y) < DEMON_RANGE));
            if(e.alert) e.volley = 1;
            if(e.cool > 0)
            {
                // Backing off after touching Mario: away from him and up
                --e.cool;
                fixed dx = e.x < mx ? -DEMON_SPEED : DEMON_SPEED, dy = -DEMON_SPEED / 2;
                e.dir = dx > 0 ? -1 : 1;   // still facing him
                if(e.x + dx >= e.a - DEMON_LEASH && e.x + dx <= e.b + DEMON_LEASH && ! in_wall(e.x + dx, e.y, e.w, e.h)) e.x += dx;
                if(e.y + dy > 3 * T && ! in_wall(e.x, e.y + dy, e.w, e.h)) e.y += dy;
                continue;
            }
            if(e.volley)
            {
                fixed tx = e.alert ? bn::max(e.a - DEMON_LEASH, bn::min(e.b + DEMON_LEASH, mx)) : bn::max(e.a, bn::min(e.b, e.x));
                fixed ty = e.alert ? my : e.y0 + bob;
                fixed dx = tx - e.x, dy = ty - e.y, d = bn::max(bn::abs(dx), bn::abs(dy));
                if(! e.alert && d < 2) { e.volley = 0; e.y = ty; continue; }
                if(d > DEMON_SPEED) { dx = dx * DEMON_SPEED / d; dy = dy * DEMON_SPEED / d; }
                if(dx != 0) e.dir = dx > 0 ? 1 : -1;
                if(! in_wall(e.x + dx, e.y, e.w, e.h)) e.x += dx;
                if(! in_wall(e.x, e.y + dy, e.w, e.h)) e.y += dy;
                continue;
            }
            e.y = e.y0 + bob;
        }
        if(e.alert || e.a == e.b) continue;   // stands its ground while attacking (a guard with no room to walk always)
        e.x += e.vx;
        if(e.x < e.a) { e.x = e.a; e.vx = -e.vx; }
        if(e.x > e.b) { e.x = e.b; e.vx = -e.vx; }
    }
    for(ember_t& em : active(embers, ember_span)) ++em.t;
    // A boss-dropped rocket launcher falls like the boss-fight mushrooms, until it stands on the floor or a plank
    if(rocket_item.live && rocket_item.fall)
    {
        rocket_item.vy = bn::min(rocket_item.vy + fixed(0.05), fixed(1.6)); rocket_item.y += rocket_item.vy;
        int tx, ty, t = tile_px(rocket_item.x, rocket_item.y + 9, tx, ty);
        if(rocket_item.vy > 0 && (solid(t) || t == 2) && rocket_item.y + 9 - rocket_item.vy <= tile_top(tx, ty) + 1) { rocket_item.y = tile_top(tx, ty) - 9; rocket_item.fall = false; }
        if(rocket_item.y > ROWS * T + 20) rocket_item.live = false;
    }
    for(mush_t& m : active(mushes, mush_span, mushes_static))
    {
        ++m.t;
        // Boss-fight mushrooms drift down from the sky until they land on the floor or a plank
        if(! m.fall || m.got) continue;
        m.vy = bn::min(m.vy + fixed(0.05), fixed(1.6)); m.y += m.vy;
        int tx, ty, t = tile_px(m.x + 6, m.y + 12, tx, ty);
        if(m.vy > 0 && (solid(t) || t == 2) && m.y + 12 - m.vy <= tile_top(tx, ty) + 1) { m.y = tile_top(tx, ty) - 12; m.fall = false; }
        if(m.y > ROWS * T + 20) m.got = true;
    }
    for(coin_t& c : active(coins, coin_span, coins_static))
    {
        if(c.got) continue;
        ++c.t;
        if(c.rest) continue;
        ++c.age; c.vy = bn::min(c.vy + fixed(0.2), fixed(6));
        c.x += c.vx;
        int tx, ty;
        if(solid(tile_px(c.x, c.y, tx, ty))) { c.x -= c.vx; c.vx = 0; }
        c.y += c.vy;
        if(c.vy > 0)
        {
            int t = tile_px(c.x, c.y + 5, tx, ty);
            if(solid(t) || t == 2) { c.y = tile_top(tx, ty) - 5; c.vy = 0; c.vx = 0; c.rest = true; }
        }
        if(c.y > ROWS * T + 20) c.got = true;
    }
    for(oneup_t& u : oneups)
    {
        if(u.got) continue;
        ++u.t;
        if(u.rest) continue;
        ++u.age; u.vy = bn::min(u.vy + fixed(0.2), fixed(6));
        u.x += u.vx;
        int tx, ty;
        if(solid(tile_px(u.x + (u.vx > 0 ? 14 : 0), u.y + 7, tx, ty))) { u.x -= u.vx; u.vx = 0; }
        u.y += u.vy;
        if(u.vy > 0)
        {
            int t = tile_px(u.x + 7, u.y + 14, tx, ty);
            if(solid(t) || t == 2) { u.y = tile_top(tx, ty) - 14; u.vy = 0; u.vx = 0; u.rest = true; }
        }
        if(u.y > ROWS * T + 20) u.got = true;
    }
    if(flame_item.live) ++flame_item.t;
    if(rocket_item.live) ++rocket_item.t;
    if(star_item.live && ! star_item.got)
    {
        star_t& st = star_item;
        ++st.t;
        if(! st.rest)
        {
            ++st.age; st.vy = bn::min(st.vy + fixed(0.2), fixed(6));
            st.y += st.vy;
            int tx, ty, t = tile_px(st.x + 8, st.y + 16, tx, ty);
            if(st.vy > 0 && (solid(t) || t == 2)) { st.y = tile_top(tx, ty) - 16; st.vy = 0; st.rest = true; }
            if(st.y > ROWS * T + 20) st.got = true;
        }
        else if(st.age < 10) ++st.age;
    }
    for(int i = 0; i < floaters.size(); )
    {
        floater_t& f = floaters[i];
        f.y -= fixed(0.4); --f.life;
        for(bn::sprite_ptr& s : f.spr) { s.set_y(s.y() - fixed(0.4)); s.set_visible(world_visible && (f.life > 10 || (f.life & 2))); }
        if(f.life <= 0) floaters.erase(floaters.begin() + i); else ++i;
    }
    if(state == state_t::play)
    {
        for(check_t& c : checks)
            if(c.lit && near_view(c.x, c.y, 16, 16) && rng.get_int(10) < 2) BURST(c.x + 8, c.y + 2, 1, fixed(0.5), fixed(-0.03), 30, P_EMBER, P_FLAME, P_GOLD);
        if(has_door && near_view(door.x, door.y, 16, 32) && rng.get_int(10) < 1) BURST(door.x + 8, door.y + 20, 1, fixed(0.4), fixed(-0.02), 40, P_EMBER, P_GOLD);
    }
    for(int i = 0; i < parts.size(); )
    {
        part_t& q = parts[i];
        q.x += q.vx; q.y += q.vy; q.vy += q.g; q.vx *= fixed(0.98); --q.life;
        if(q.life <= 0) parts.erase(parts.begin() + i); else ++i;
    }
    if(shake > 0) shake *= fixed(0.85);
    if(shake < fixed(0.3)) shake = 0;
}

void update_bullets()
{
    for(bullet_t& b : bullets)
    {
        b.x += b.vx; b.y += b.vy; --b.life;
        // hx, hy: the point that hits (a rocket's nose); dx, dy: the way it flies (right/left, up, or both at 45 degrees)
        int dx = b.vx > 0 ? 1 : b.vx < 0 ? -1 : 0, dy = b.vy > 0 ? 1 : b.vy < 0 ? -1 : 0;
        fixed k = dx && dy ? DIAG : fixed(1);
        fixed hx = b.rocket ? b.x + dx * k * ROCKET_NOSE : b.x, hy = b.rocket ? b.y + dy * k * ROCKET_NOSE : b.y;
        int btx, bty, bt = tile_px(hx, hy, btx, bty);
        int dmg = b.rocket ? ROCKET_DMG : 1;
        if(bt == 4) { break_block(btx, bty); b.life = 0; if(b.rocket) explode(hx, hy); continue; }
        if(bt == 3 && b.rocket) { blast_spikes(btx, bty); b.life = 0; explode(hx, hy); continue; }
        if(bt == 1)
        {
            b.life = 0;
            if(b.rocket) explode(hx - dx * k * 4, hy - dy * k * 4);
            else BURST(b.x - dx * 2, b.y - dy * 2, 5, fixed(1.2), fixed(0.05), 14, P_GOLD, P_FLAME, P_WHITE);
            continue;
        }
        // Bullets knock spit globs and fireballs out of the air
        bool hit_glob = false;
        for(glob_t& g : globs)
        {
            if(g.life > 0 && bn::abs(g.x - hx) < (b.rocket ? 7 : 6) && bn::abs(g.y - hy) < (b.rocket ? 7 : 6))
            {
                g.life = 0; b.life = 0; hit_glob = true;
                if(b.rocket) explode(g.x, g.y);
                if(g.fire) BURST(g.x, g.y, 8, fixed(1.4), fixed(0.05), 18, P_FLAME, P_EMBER, P_GOLD);
                else BURST(g.x, g.y, 8, fixed(1.4), fixed(0.05), 18, P_TEAL, P_TEAL, P_WHITE);
                break;
            }
        }
        if(hit_glob) continue;
        if(boss_active() && overlap(! b.rocket ? rect{ b.x - 1, b.y - 1, 2, 2 } : dx && dy ? rect{ hx - 3, hy - 3, 6, 6 } : dy ? rect{ hx - 4, hy - 2, 8, 4 } : rect{ hx - 2, b.y - 4, 4, 8 }, boss_box(*boss)))
        {
            b.life = 0;
            if(b.rocket) explode(hx, hy);
            if(boss->state == boss_t::fight)
            {
                hit_boss(dmg);
                boss_burst(boss->kind, b.x, b.y, 4, fixed(1.2), fixed(0.05), 14);
            }
            continue;
        }
        for(enemy_t& e : active(enemies, enemy_span))
        {
            if(e.alive && e.rise >= RISE_END && hx > e.x - 2 && hx < e.x + e.w + 2 && hy > e.y - (b.rocket ? 8 : 6) && hy < e.y + e.h + (b.rocket ? 2 : 0))
            {
                shot_hit(e, dmg); b.life = 0;
                if(b.rocket) explode(hx, hy, &e);
                break;
            }
        }
    }
    for(int i = 0; i < bullets.size(); ) { if(bullets[i].life <= 0) bullets.erase(bullets.begin() + i); else ++i; }
    for(boom_t& b : booms) ++b.t;
    for(int i = 0; i < booms.size(); ) { if(booms[i].t >= BOOM_FRAMES * BOOM_STEP) booms.erase(booms.begin() + i); else ++i; }
}

// Foes that see Mario nearby face him and wind up, then fire: aliens and reapers spit a glob, hell beasts breathe fireballs
void update_foes()
{
    fixed px = p.x + p.w / 2, py = p.y + p.h / 2;
    for(enemy_t& e : active(enemies, enemy_span))
    {
        if(! e.alive || e.rise < RISE_END) continue;   // a reaper still climbing out of its grave can't attack yet
        if(e.kind == FOE_DEMON) continue;   // flying demons only fly into Mario (update_world)
        bool beast = e.kind == FOE_BEAST;
        // A beast's mouth is near the top of its body; aliens and reapers fire from FOE_FIRE_H above their feet; mechs spit
        // from their middle (the shot starts 1 px above ey)
        fixed ex = e.x + e.w / 2, ey = beast ? e.y + 6 : e.kind == FOE_MECH ? e.y + e.h / 2 : e.y + e.h - FOE_FIRE_H + 1, dx = px - ex, dy = py - ey;
        e.alert = ! p.dead && bn::abs(dx) < (beast ? BEAST_RANGE : FOE_RANGE) && bn::abs(dy) < 90;
        if(! e.alert) { e.windup = 0; continue; }
        e.dir = dx > 0 ? 1 : -1;
        if(e.cool > 0) { --e.cool; continue; }
        if(++e.windup >= FOE_WINDUP)
        {
            // Beasts alternate an aimed three-way spread with a wide five-way fan; the others spit single globs more often
            // (Hard: the whole wind-up-and-fire cycle is 1/1.4 as long; Expert: 1/1.25 of Hard's, user 2026-10-24; the
            // wind-up flash keeps its length)
            const int cycle = FOE_WINDUP + (beast ? 55 : 40) + rng.get_int(25);
            e.windup = 0; e.cool = (difficulty == 2 ? (cycle * 4 + 3) / 7 : hard_frames(cycle)) - FOE_WINDUP;
            fixed a = angle_of(dx, dy), s = beast ? BEAST_SHOT_SPEED : fixed(2.8);
            if(beast)
            {
                bool fan = (++e.volley) % 2 == 0;
                static constexpr int spread[] = { -13, 0, 13 };          // 0.22 rad
                static constexpr int fan5[] = { -23, -11, 0, 11, 23 };   // 0.2 rad steps
                const int* offs = fan ? fan5 : spread; int n = fan ? 5 : 3;
                for(int i = 0; i < n; ++i) push_glob(ex + e.dir * 14, ey - 1, cos_deg(a + offs[i]) * s, sin_deg(a + offs[i]) * s, 140, true);
                sfx::fire();
            }
            else
            {
                push_glob(ex + e.dir * 6, ey - 1, cos_deg(a) * s, sin_deg(a) * s, 140, false);
                sfx::spit();
            }
        }
    }
    for(glob_t& g : globs)
    {
        g.x += g.vx; g.y += g.vy; --g.life; ++g.t;
        int gtx, gty;
        if(solid(tile_px(g.x, g.y, gtx, gty)))
        {
            g.life = 0;
            if(g.fire) BURST(g.x, g.y, 5, 1, fixed(0.05), 14, P_FLAME, P_EMBER, P_GOLD);
            else BURST(g.x, g.y, 5, 1, fixed(0.05), 14, P_TEAL, P_TEAL, P_WHITE);
            continue;
        }
        if(! p.dead && g.x > p.x - 1 && g.x < p.x + p.w + 1 && g.y > p.y && g.y < p.y + p.h)
        {
            g.life = 0;
            if(g.fire) BURST(g.x, g.y, 8, fixed(1.4), fixed(0.05), 18, P_FLAME, P_EMBER, P_GOLD);
            else BURST(g.x, g.y, 8, fixed(1.4), fixed(0.05), 18, P_TEAL, P_TEAL, P_WHITE);
            hurt(g.x);
        }
    }
    for(int i = 0; i < globs.size(); ) { if(globs[i].life <= 0) globs.erase(globs.begin() + i); else ++i; }
}

// ---------- Screens & HUD ----------
BN_DATA_EWRAM bn::vector<bn::sprite_ptr, 64> overlay_spr;
BN_DATA_EWRAM bn::vector<bn::sprite_ptr, 8> hud_icons;
#ifdef SMHB_AUTOPLAY
// Test build's CPU meter: average and worst of the last half second, worst frame and number of overrun (dropped)
// frames since play started, and sprites in use
int peak_cpu = 0, avg_cpu = 0, peak_all = 0, overruns = 0;
// Output meter: frames whose 8-bit mix buffer had a clamped sample (127 / -128) since play started, and the
// loudest sample seen in the last half second
int clip_frames = 0, out_peak = 0;
extern "C" { extern void* mp_writepos; extern unsigned mm_mixlen; extern unsigned char mp_mix_seg; void mmSetVBlankHandler(void (*)()); }
// Mixer timing probe, called by maxmod's VBlank handler right after it swaps buffer halves: exactly one frame's mix
// (mm_mixlen bytes) should have been written since the last swap, and the interrupt should run at line 160 (later
// and the sound DMA has already read past its buffer)
volatile int mix_bad = 0, irq_late = 0, irq_late_max = 0;
uintptr_t mix_base = 0;
void mix_probe()
{
    int line = *reinterpret_cast<volatile uint16_t*>(0x04000006);
    int late = line >= 160 ? line - 160 : line + 68;
    bool counted = state == state_t::play && frame > 160;
    if(counted && late > 1) ++irq_late;
    if(counted && late > irq_late_max) irq_late_max = late;
    if(mp_mix_seg == 0) mix_base = uintptr_t(mp_writepos);
    else if(counted && mix_base && uintptr_t(mp_writepos) - mix_base != mm_mixlen) ++mix_bad;
}
void measure_output()
{
    static uintptr_t base = ~uintptr_t(0);
    static int worst = 0, n = 0;
    base = bn::min(base, uintptr_t(mp_writepos));   // the write position goes back to the buffer's start every other frame
    const int8_t* s = reinterpret_cast<const int8_t*>(base);
    int peak = 0; bool clip = false;
    for(int i = 0, e = int(mm_mixlen) * 4; i < e; ++i)
    {
        int v = s[i];
        if(v == 127 || v == -128) clip = true;
        peak = bn::max(peak, v < 0 ? -v : v);
    }
    if(clip && state == state_t::play && frame > 160) ++clip_frames;
    worst = bn::max(worst, peak);
    if(++n == 30) { out_peak = worst; worst = 0; n = 0; }
}
#endif

bn::string<16> fmt_time(int f)
{
    int tenths = f / 6, s = tenths / 10, m = s / 60;
    bn::string<16> out = bn::to_string<4>(m);
    out += ':';
    int ss = s % 60;
    if(ss < 10) out += '0';
    out += bn::to_string<4>(ss);
    out += '.';
    out += bn::to_string<2>(tenths % 10);
    return out;
}

bn::string<8> pad6(int v)
{
    bn::string<8> s = bn::to_string<8>(v < 0 ? 0 : bn::min(v, 9999999));
    bn::string<8> out;
    for(int i = s.size(); i < 6; ++i) out += '0';
    out += s;
    return out;
}

void clear_overlay() { overlay_spr.clear(); }

void overlay_line(int y, const bn::string_view& text, bool big = false)
{
    bn::sprite_text_generator& g = big ? *text16 : *text8;
    g.set_center_alignment();
    g.set_bg_priority(0);
    g.generate(0, y, text, overlay_spr);
}

void set_dim(bool on)
{
    bn::bg_palettes::set_fade(bn::color(1, 1, 3), on ? fixed(0.55) : fixed(0));
}

// One piece of HUD text that is only regenerated when its value changes
struct hud_text
{
    bn::vector<bn::sprite_ptr, 10> spr;   // room for the test build's long debug lines
    int value = -1;

    void set(int v, int x, int y, bool right, const bn::string_view& text)
    {
        if(v == value) return;
        value = v;
        spr.clear();
        text8->set_bg_priority(0);
        if(right) text8->set_right_alignment(); else text8->set_left_alignment();
        text8->generate(x, y, text, spr);
    }
    void clear() { spr.clear(); value = -1; }
};
BN_DATA_EWRAM hud_text hud_embers, hud_coins, hud_score, hud_lives, hud_time, hud_cpu, hud_boss, hud_grenades;
BN_DATA_EWRAM bn::optional<bn::sprite_ptr> hud_grenade_icon;
int hud_hp = -1;
BN_DATA_EWRAM wspr boss_bar[5];

void clear_hud()
{
    hud_icons.clear(); hud_hp = -1;
    hud_embers.clear(); hud_coins.clear(); hud_score.clear(); hud_lives.clear(); hud_time.clear(); hud_cpu.clear(); hud_boss.clear();
    hud_grenades.clear(); hud_grenade_icon.reset();
    for(wspr& w : boss_bar) w.reset();
}

void update_hud()
{
    if(title_screens() || state == state_t::congrats || state == state_t::win || state == state_t::world_card ||
       state == state_t::initials || state == state_t::scores) { clear_hud(); return; }
    // Row 1: health pips, embers, coins ............ score
    int hp = p.dead ? 0 : p.hp;
    if(hp != hud_hp)
    {
        hud_hp = hp;
        hud_icons.clear();
        for(int i = 0; i < MAX_HP; ++i)
        {
            bn::sprite_ptr s = bn::sprite_items::hp.create_sprite(-114 + i * 9, -72, i < hp ? 0 : 1);
            s.set_bg_priority(0);
            hud_icons.push_back(bn::move(s));
        }
        bn::sprite_ptr e = bn::sprite_items::ember.create_sprite(-62, -72); e.set_bg_priority(0); hud_icons.push_back(bn::move(e));
        // The coin icon and count sit clear of a "72/72" ember count (moved 10 px right, user 2026-10-17)
        bn::sprite_ptr c = bn::sprite_items::coin.create_sprite(-8, -72, 0); c.set_bg_priority(0); hud_icons.push_back(bn::move(c));
    }
    if(lvl.embers != hud_embers.value)
    {
        bn::string<16> em = bn::to_string<4>(lvl.embers); em += '/'; em += bn::to_string<4>(ember_total);
        hud_embers.set(lvl.embers, -56, -72, false, em);
    }
    // The counter goes back to 0 at every 100 coins, as each hundred buys a life (user, 2026-10-17), and when the level is
    // restarted (2026-10-24); the stats screens still show the whole run's coins
    const int shown_coins = run.life_coins;
    if(shown_coins != hud_coins.value)
    {
        bn::string<12> co = "x"; co += bn::to_string<6>(shown_coins);
        hud_coins.set(shown_coins, -2, -72, false, co);
    }
    if(run.score != hud_score.value) hud_score.set(run.score, 118, -72, true, pad6(run.score));
    // Row 2: lives ............ time (tenths of a second)
    int lives = bn::max(0, run.lives);
    if(lives != hud_lives.value)
    {
        bn::string<16> lv = "HELLFIRE x"; lv += bn::to_string<4>(lives);   // the hero's name since 2026-10-29 (user)
        hud_lives.set(lives, -118, -62, false, lv);
    }
    int tenths = lvl.time / 6;
    if(tenths != hud_time.value) hud_time.set(tenths, 118, -62, true, fmt_time(lvl.time));
    // While he has the grenade launcher (World 1; Expert's World 4): grenades left, after the lives (8 px further right
    // since the lives read "HELLFIRE", 2026-10-29, so two-digit lives still fit). Gone again from World 2.
    if(run.got_grenades)
    {
        if(! hud_grenade_icon) { hud_grenade_icon = bn::sprite_items::grenade.create_sprite(-30, -62, 0); hud_grenade_icon->set_bg_priority(0); }
        if(run.grenades != hud_grenades.value) { bn::string<8> gr = "x"; gr += bn::to_string<4>(run.grenades); hud_grenades.set(run.grenades, -24, -62, false, gr); }
    }
    else if(hud_grenade_icon) { hud_grenade_icon.reset(); hud_grenades.clear(); }
    // Boss health bar under the HUD while the fight is on (the web game has it at the bottom, but on the GBA's smaller
    // screen the arena floor, and Mario, are right there)
    bool bar = boss_active() && state != state_t::title;
    if(bar)
    {
        if(hud_boss.value < 0)
        {
            hud_boss.value = 1;
            text8->set_bg_priority(0); text8->set_center_alignment();
            text8->generate(0, -50, spec(boss->kind).name, hud_boss.spr);
        }
        int fill = boss->state == boss_t::intro ? (boss->shown * 160).round_integer() : boss->hp * 160 / boss_max_hp(*boss);
        for(int i = 0; i < 5; ++i)
        {
            int f = bn::max(0, bn::min(32, fill - i * 32));
            wspr& w = boss_bar[i];
            if(! w.s) { w.s = bn::sprite_items::bossbar.create_sprite(-64 + i * 32, -40, f); w.s->set_bg_priority(0); w.gfx = f; }
            else if(w.gfx != f) { w.s->set_tiles(bn::sprite_items::bossbar.tiles_item(), f); w.gfx = f; }
        }
    }
    else if(hud_boss.value >= 0 || boss_bar[0].s)
    {
        hud_boss.clear();
        for(wspr& w : boss_bar) w.reset();
    }
#ifdef SMHB_AUTOPLAY
    // Test build: average | worst CPU use over the last half second (over 100% = a dropped frame)
    if(peak_cpu != hud_cpu.value)
    {
        bn::string<48> cpu = "CPU "; cpu += bn::to_string<4>(avg_cpu); cpu += "% / "; cpu += bn::to_string<4>(peak_cpu); cpu += "% D"; cpu += bn::to_string<6>(overruns); cpu += " X"; cpu += bn::to_string<6>(mix_bad); cpu += " L"; cpu += bn::to_string<6>(irq_late); cpu += "/"; cpu += bn::to_string<4>(irq_late_max);
        hud_cpu.set(peak_cpu, 118, -52, true, cpu);
    }
#endif
}

// ---------- Screens ----------
// The tag's 5th character is the number of levels in a full run: a best time saved by the old 3-level game ("SMHB",
// no count) can't be compared with a run through all 6, so it's ignored
// Hard's best time is kept apart (at BEST_HARD_OFFSET, with an 'H' after the count) from Normal's, which stays at 0
// (Expert's at BEST_EXPERT_OFFSET with an 'E')
struct save_data { char tag[8]; int best; };
constexpr int BEST_HARD_OFFSET = 32, BEST_EXPERT_OFFSET = 48;
constexpr char difficulty_mark[DIFFICULTIES] = { char(0), 'H', 'E' };
int best_offset() { return difficulty == 2 ? BEST_EXPERT_OFFSET : difficulty == 1 ? BEST_HARD_OFFSET : 0; }
int read_best()
{
    save_data s; bn::sram::read_offset(s, best_offset());
    return (s.tag[0] == 'S' && s.tag[1] == 'M' && s.tag[2] == 'H' && s.tag[3] == 'B' && s.tag[4] == '0' + level_count &&
            (! difficulty || s.tag[5] == difficulty_mark[difficulty]) && s.best > 0) ? s.best : -1;
}
void write_best(int v)
{
    save_data s = { { 'S', 'M', 'H', 'B', char('0' + level_count), difficulty_mark[difficulty], 0, 0 }, v };
    bn::sram::write_offset(s, best_offset());
}
static_assert(BEST_EXPERT_OFFSET + sizeof(save_data) <= 64, "the best times overlap the high scores");

// Expert (shown as "Hard") is locked until the game has been beaten on Hard (shown as "Normal") (user, 2026-10-26).
// Saved in the gap after Normal's best time (tag "SMUL"). Test builds (-StartLevel, autoplay) have it unlocked.
struct unlock_data { char tag[8]; int expert; };
constexpr int UNLOCK_OFFSET = 16;
static_assert(sizeof(save_data) <= UNLOCK_OFFSET && UNLOCK_OFFSET + sizeof(unlock_data) <= BEST_HARD_OFFSET, "the unlock record overlaps the best times");
int expert_unlocked = -1;   // -1 = not read yet
bool hs_beaten_on(int d);
bool expert_open()
{
#if SMHB_START_LEVEL != 0 || (defined(SMHB_AUTOPLAY) && SMHB_SCENE != 60)   // (scene 60 tests the lock)
    return true;
#else
    if(expert_unlocked < 0)
    {
        unlock_data u; bn::sram::read_offset(u, UNLOCK_OFFSET);
        expert_unlocked = u.tag[0] == 'S' && u.tag[1] == 'M' && u.tag[2] == 'U' && u.tag[3] == 'L' && u.expert == 1;
        // A save from before the lock: a Hard or Expert high score that beat the game counts
        if(! expert_unlocked && (hs_beaten_on(1) || hs_beaten_on(2))) expert_unlocked = 1;
    }
    return expert_unlocked == 1;
#endif
}
void unlock_expert()
{
    if(expert_unlocked == 1) return;
    expert_unlocked = 1;
    unlock_data u = { { 'S', 'M', 'U', 'L', 0, 0, 0, 0 }, 1 };
    bn::sram::write_offset(u, UNLOCK_OFFSET);
}
bool difficulty_open(int d) { return d < 2 || expert_open(); }

// High scores (user, 2026-10-22): the top 5 runs with arcade-style initials and how far each got, saved after the best
// time (which stays where it was). A run ends at the Congratulations screen or by leaving game over for the title;
// retrying a level after game over carries the run on, so it isn't recorded until then.
// Normal and Hard keep separate tables (user, 2026-10-23): Normal's where the one table always was, Hard's (tag "SMHS"
// + count + 'H') at HS_HARD_OFFSET. Both remember the last initials typed.
// Expert's ('E') at HS_EXPERT_OFFSET (2026-10-24).
constexpr int HS_COUNT = 5, HS_OFFSET = 64, HS_HARD_OFFSET = 128, HS_EXPERT_OFFSET = 192;
struct hs_entry { char name[3]; uint8_t level; int score; };   // level = index reached, level_count = beat the game
struct hs_table { char tag[8]; hs_entry e[HS_COUNT]; char last[3]; };
static_assert(sizeof(hs_table) <= HS_HARD_OFFSET - HS_OFFSET, "the high-score tables overlap");
BN_DATA_EWRAM hs_table hs_tables[DIFFICULTIES];   // [0] Normal, [1] Hard, [2] Expert
bool hs_loaded = false;
int hs_offset(int d) { return d == 2 ? HS_EXPERT_OFFSET : d ? HS_HARD_OFFSET : HS_OFFSET; }
void hs_load()
{
    if(hs_loaded) return;
    hs_loaded = true;
    for(int d = 0; d < DIFFICULTIES; ++d)
    {
        hs_table& t = hs_tables[d];
        bn::sram::read_offset(t, hs_offset(d));
        const char mark = difficulty_mark[d];
        if(t.tag[0] != 'S' || t.tag[1] != 'M' || t.tag[2] != 'H' || t.tag[3] != 'S' || t.tag[4] != '0' + level_count || (d && t.tag[5] != mark))
        {
            t = hs_table();
            t.tag[0] = 'S'; t.tag[1] = 'M'; t.tag[2] = 'H'; t.tag[3] = 'S'; t.tag[4] = char('0' + level_count); t.tag[5] = mark;
            // A new table suggests the initials last typed on the other one (Normal's, for a first Hard table)
            for(int i = 0; i < 3; ++i) t.last[i] = d ? hs_tables[0].last[i] : 'A';
        }
    }
}
// This run's table (the difficulty being played)
hs_table& hs_now() { hs_load(); return hs_tables[difficulty]; }
// Whether a run on difficulty d that beat the game made its table
bool hs_beaten_on(int d)
{
    hs_load();
    for(const hs_entry& e : hs_tables[d].e) if(e.score > 0 && e.level == level_count) return true;
    return false;
}
// The place a score would take (0-based), or -1 if it doesn't make the table; ties go below the older score
int hs_rank(int score)
{
    hs_table& hs = hs_now();
    if(score <= 0) return -1;
    for(int i = 0; i < HS_COUNT; ++i) if(score > hs.e[i].score) return i;
    return -1;
}
void hs_insert(int rank, const char name[3], int score, int level)
{
    hs_table& hs = hs_now();
    for(int i = HS_COUNT - 1; i > rank; --i) hs.e[i] = hs.e[i - 1];
    hs_entry& e = hs.e[rank];
    for(int i = 0; i < 3; ++i) e.name[i] = name[i];
    e.level = uint8_t(level); e.score = score;
    for(int d = 0; d < DIFFICULTIES; ++d)
    {
        for(int i = 0; i < 3; ++i) hs_tables[d].last[i] = name[i];
        bn::sram::write_offset(hs_tables[d], hs_offset(d));
    }
}

// Time Attack's best times (user, 2026-10-25): per level, the 5 fastest clears with initials, after the high scores.
// Each level's table has its own tag ("SMTA" + level count + 'A' + level index), so a table that doesn't match starts empty.
constexpr int TA_OFFSET = 256;
struct ta_entry { char name[3]; uint8_t used; int time; };   // time in frames (the level timer)
struct ta_table { char tag[8]; ta_entry e[HS_COUNT]; };
static_assert(HS_EXPERT_OFFSET + sizeof(hs_table) <= TA_OFFSET, "the Time Attack tables overlap the high scores");
static_assert(TA_OFFSET + level_count * sizeof(ta_table) <= 32768, "the Time Attack tables don't fit in SRAM");
BN_DATA_EWRAM ta_table ta_tables[level_count];
bool ta_loaded = false;
int ta_offset(int l) { return TA_OFFSET + l * int(sizeof(ta_table)); }
void ta_load()
{
    if(ta_loaded) return;
    ta_loaded = true;
    for(int l = 0; l < level_count; ++l)
    {
        ta_table& t = ta_tables[l];
        bn::sram::read_offset(t, ta_offset(l));
        const char tag[6] = { 'S', 'M', 'T', 'A', char('0' + level_count), char('A' + l) };
        bool ok = true;
        for(int i = 0; i < 6; ++i) if(t.tag[i] != tag[i]) ok = false;
        if(! ok) { t = ta_table(); for(int i = 0; i < 6; ++i) t.tag[i] = tag[i]; }
    }
}
// The place a time would take on a level's table (0-based), or -1; ties go below the older time
int ta_rank(int l, int time)
{
    ta_load();
    for(int i = 0; i < HS_COUNT; ++i) if(! ta_tables[l].e[i].used || time < ta_tables[l].e[i].time) return i;
    return -1;
}
void ta_insert(int l, int rank, const char name[3], int time)
{
    ta_table& t = ta_tables[l];
    for(int i = HS_COUNT - 1; i > rank; --i) t.e[i] = t.e[i - 1];
    ta_entry& e = t.e[rank];
    for(int i = 0; i < 3; ++i) e.name[i] = name[i];
    e.used = 1; e.time = time;
    bn::sram::write_offset(t, ta_offset(l));
    // the initials are remembered for the next entry, here or on the high scores
    hs_load();
    for(int d = 0; d < DIFFICULTIES; ++d)
    {
        for(int i = 0; i < 3; ++i) hs_tables[d].last[i] = name[i];
        bn::sram::write_offset(hs_tables[d], hs_offset(d));
    }
}

// The title screen's menu (user, 2026-10-23: the controls and the credits moved to screens of their own)
enum { MENU_START, MENU_TIME_ATTACK, MENU_CONTROLS, MENU_SCORES, MENU_CREDITS, MENU_ITEMS };   // (High Scores before Credits, user; Time Attack 2026-10-25)
int title_sel = MENU_START;
void draw_title()
{
    static constexpr const char* items[MENU_ITEMS] = { "Start Game", "Time Attack", "Controls", "High Scores", "Credits" };
    clear_overlay();
    overlay_line(-38, "HELLBOUND", true);   // the game's name since 2026-10-29 (user; "SUPER MARIO" was a line above it)
    for(int i = 0; i < MENU_ITEMS; ++i)
    {
        bn::string<24> t;
        if(i == title_sel) { t = "> "; t += items[i]; t += " <"; } else t = items[i];
        overlay_line(-4 + i * 12, t);   // (14 apart until Time Attack made it five)
    }
    overlay_line(60, "UP/DOWN: choose     A: select");
}
void show_title(bool keep_pan = false)
{
    state = state_t::title; if(! keep_pan) title_t = 0;
    sync_music();   // the title music carries on from the title picture
    if(li != title_level) { load_level(title_level); }   // the title screen pans across Hell, like the web game
    set_dim(true);
    draw_title();
}
void show_controls()
{
    state = state_t::controls;
    clear_overlay();
    overlay_line(-50, "CONTROLS", true);
    overlay_line(-22, "Shoot aliens, reapers and hell beasts.");
    overlay_line(-4, "A jump   B shoot   DOWN crouch");
    overlay_line(8, "UP aim up   UP+LEFT/RIGHT diagonal");   // (user, 2026-10-20)
    overlay_line(20, "R toss grenades");   // (user, 2026-10-29)
    overlay_line(56, "B: back");
}
void show_credits()
{
    state = state_t::credits;
    clear_overlay();
    overlay_line(-50, "CREDITS", true);
    overlay_line(-22, "Dreamweaver Studios");
    overlay_line(-4, "Sprites: ansimuz,");   // Goldenfrank0 removed at the user's request, 2026-10-29
    overlay_line(6, "Jesse Munguia, Anokolisa");
    // Heatley Bros: the title music. Panda Beats (World 1's old track) removed at the user's request, 2026-10-21
    // Pixel 8 Beats and The Grump Sounds added, bitbybitsound.com removed (user, 2026-10-21); two lines, like the sprites
    overlay_line(24, "Music: Bert Cole, Heatley Bros,");
    overlay_line(34, "Pixel 8 Beats, The Grump Sounds");
    overlay_line(56, "B: back");
}
void show_difficulty();
void show_scores(bool from_title, int highlight);
void show_ta_select(int highlight);
void update_title()
{
    if(K.up_pressed || K.down_pressed)
    {
        title_sel = (title_sel + (K.up_pressed ? MENU_ITEMS - 1 : 1)) % MENU_ITEMS;
        sfx::letter(); draw_title();
    }
    else if(K.start_pressed || K.a_pressed)
    {
        if(title_sel == MENU_START) show_difficulty();
        else if(title_sel == MENU_TIME_ATTACK) { ta_prev_difficulty = difficulty; ta_level = 0; show_ta_select(-1); }
        else if(title_sel == MENU_CONTROLS) show_controls();
        else if(title_sel == MENU_SCORES) show_scores(true, -1);
        else show_credits();
    }
    else if(K.select_pressed) show_scores(true, -1);   // the old shortcut
}

// Select Difficulty (user, 2026-10-23), after PRESS START: UP/DOWN picks, A or START starts, B goes back to the title.
// The cursor starts on the last difficulty played.
void draw_difficulty()
{
    clear_overlay();
    overlay_line(-44, "SELECT DIFFICULTY", true);
    for(int d = 0; d < DIFFICULTIES; ++d)
    {
        bn::string<24> item = difficulty_names[d];
        if(! difficulty_open(d)) item += " (locked)";
        if(d == difficulty) { item = "> "; item += difficulty_names[d]; item += " <"; }
        overlay_line(-22 + d * 16, item);
    }
    if(! difficulty_open(2)) overlay_line(30, "Beat the game on Normal to unlock Hard.");
    overlay_line(54, "UP/DOWN: choose     A: start     B: back");
}
void show_difficulty()
{
    state = state_t::difficulty;
    if(! difficulty_open(difficulty)) set_difficulty(1);
    set_dim(true);
    draw_difficulty();
}
void update_difficulty()
{
    if(K.up_pressed || K.down_pressed)
    {
        // (the cursor skips a locked difficulty)
        int d = difficulty;
        do d = (d + (K.down_pressed ? 1 : DIFFICULTIES - 1)) % DIFFICULTIES; while(! difficulty_open(d));
        set_difficulty(d);
        sfx::letter(); draw_difficulty();
    }
    else if(K.b_pressed) show_title(true);
}

void start_play()
{
    state = state_t::play; clear_overlay(); set_dim(false);
    sync_music();
}

void start_game(int level = 0)
{
    run = run_t();
    load_level(level);
    start_play();
}

void ta_clear();
void clear_level()
{
    state = state_t::clear;
    music_stop();
    if(time_attack) { ta_clear(); return; }
    bool last = li == level_count - 1;
    run.time += lvl.time; run.deaths += lvl.deaths; run.embers += lvl.embers; run.total += ember_total;
    // Clearing a level without losing a life earns a bonus, and clearing it without taking any damage earns another
    bool flawless = lvl.deaths == 0, untouched = ! lvl.hurt;
    if(flawless) add_score(SCORE_FLAWLESS);
    if(untouched) add_score(SCORE_UNTOUCHED);
    // Hard: defeating every one of the level's enemies earns a bonus too (the Fire Skull's summoned demons don't count;
    // enemies stay defeated when Mario dies)
    bool all_foes = hard && ! enemies.empty();
    for(const enemy_t& e : enemies) if(e.alive && ! e.minion) { all_foes = false; break; }
    if(all_foes) add_score(SCORE_ALL_ENEMIES);
    // The bonuses get two lines: all three on one ran off the screen's sides
    bn::string<48> bonus, bonus2;
    auto add = [](bn::string<48>& line, const char* name, int n)
    {
        if(! line.empty()) line += "  ";
        line += name; line += " +"; line += bn::to_string<6>(n);
    };
    if(ember_total && lvl.embers == ember_total) add(bonus, "All embers", SCORE_ALL_EMBERS);
    if(flawless) add(bonus, "No-death", SCORE_FLAWLESS);
    if(untouched) add(bonus2, "No-damage", SCORE_UNTOUCHED);
    if(all_foes) add(bonus2, "All enemies", SCORE_ALL_ENEMIES);
    clear_overlay();
    if(last)
    {
        // Beating the last level shows the Congratulations screen, with ember fireworks and its own music (sync_music;
        // it replaced the course-clear tune at the user's request, 2026-10-29)
        state = state_t::congrats; set_dim(true);
        if(difficulty >= 1) unlock_expert();   // beating it on Hard (shown "Normal") opens Expert (shown "Hard")
        overlay_line(-52, "WORLD 4 CLEARED");   // the user's wording (2026-10-16): the last level is World 4-4
        overlay_line(-36, "CONGRATULATIONS!", true);
        overlay_line(-4, "YOU BEAT THE GAME!");   // the user's wording (2026-10-16), replacing the web game's paragraph
        bn::string<32> s = "Final score "; s += pad6(run.score);
        overlay_line(24, s);
        overlay_line(38, "Thank you for playing Hellbound");
        overlay_line(60, "A: continue");
        return;
    }
    sfx::clear();
    set_dim(true);
    bn::string<32> title = LV().name; title += " cleared";
    overlay_line(-54, title, true);
    bn::string<48> a = "Embers "; a += bn::to_string<4>(lvl.embers); a += '/'; a += bn::to_string<4>(ember_total);
    // Coins are drawn as gems since 2026-10-29 (user)
    a += "   Gems "; a += bn::to_string<4>(lvl.coins); a += '/'; a += bn::to_string<4>(coin_total);
    overlay_line(-26, a);
    bn::string<48> b = "Time "; b += fmt_time(lvl.time); b += "   Falls "; b += bn::to_string<4>(lvl.deaths);
    overlay_line(-14, b);
    bn::string<32> s = "Score "; s += pad6(run.score);
    overlay_line(0, s);
    if(bonus.empty()) { bonus = bonus2; bonus2.clear(); }
    if(! bonus.empty()) overlay_line(14, bonus);
    if(! bonus2.empty()) overlay_line(26, bonus2);
    overlay_line(46, "A: next level     SELECT: replay");
}

void show_win_stats()
{
    state = state_t::win;
    clear_overlay(); set_dim(true);
    overlay_line(-56, "HELLBOUND", true);
    bn::string<48> a = "Embers "; a += bn::to_string<4>(run.embers); a += '/'; a += bn::to_string<4>(run.total);
    a += "   Gems "; a += bn::to_string<6>(run.coins);
    overlay_line(-28, a);
    bn::string<48> b = "Total time "; b += fmt_time(run.time); b += "   Falls "; b += bn::to_string<4>(run.deaths);
    overlay_line(-16, b);
    bn::string<32> s = "Score "; s += pad6(run.score); s += "   "; s += difficulty_names[difficulty];
    overlay_line(-2, s);
    int best = read_best();
    bn::string<48> bt;
    if(best < 0 || run.time < best) { write_best(run.time); if(best < 0) bt = "First full run recorded"; else { bt = "New best time (was "; bt += fmt_time(best); bt += ")"; } }
    else { bt = "Best time "; bt += fmt_time(best); }
    overlay_line(16, bt);
    overlay_line(46, "A: play again");
}

void game_over()
{
    state = state_t::over;
    music_stop();
    sfx::over();
    clear_overlay(); set_dim(true);
    overlay_line(-40, "GAME OVER", true);
    bn::string<48> m = "Hellfire ran out of lives in "; m += LV().name; m += '.';
    overlay_line(-14, m);
    // Was "Level 2 of 3   Score ..."; the line above already names the world (World 2-1 and so on), so just the score
    bn::string<32> l = "Score "; l += pad6(run.score);
    overlay_line(2, l);
    if(hs_rank(run.score) >= 0) overlay_line(16, "High score! START to enter your initials");
    overlay_line(30, "A: retry level     START: title");
}

// ---------- High scores: initials entry and the table ----------
constexpr char HS_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.- ";
constexpr int HS_NCHARS = sizeof(HS_CHARS) - 1;
BN_DATA_EWRAM bn::vector<bn::sprite_ptr, 4> hs_letter_spr[3];
BN_DATA_EWRAM bn::vector<bn::sprite_ptr, 2> hs_mark_spr;
BN_DATA_EWRAM bn::vector<bn::sprite_ptr, 16> hs_row_spr;   // the new entry's row on the table, which blinks
int hs_letter[3], hs_pos = 0, hs_new_rank = -1, hs_reached = 0, hs_repeat = 0, hs_blink = 0;

int hs_char_index(char c)
{
    for(int i = 0; i < HS_NCHARS; ++i) if(HS_CHARS[i] == c) return i;
    return 0;
}
void hs_draw_letter(int i)
{
    hs_letter_spr[i].clear();
    char t[2] = { HS_CHARS[hs_letter[i]], 0 };
    text16->set_center_alignment(); text16->set_bg_priority(0);
    text16->generate(-20 + i * 20, -6, t, hs_letter_spr[i]);
}
void hs_draw_mark()
{
    hs_mark_spr.clear();
    text8->set_center_alignment(); text8->set_bg_priority(0);
    text8->generate(-20 + hs_pos * 20, 8, "^", hs_mark_spr);
}
void hs_clear_sprites()
{
    for(auto& v : hs_letter_spr) v.clear();
    hs_mark_spr.clear(); hs_row_spr.clear();
}

void show_scores(bool from_title, int highlight);

void begin_initials(const bn::string_view& head, const bn::string_view& line);
// Ends a run: the initials screen if the score made the table, else straight on to the stats or the title
void end_run(bool won)
{
    hs_then_stats = won;
    hs_reached = won ? level_count : li;
    hs_new_rank = hs_rank(run.score);
    if(hs_new_rank < 0) { if(won) show_win_stats(); else show_title(); return; }
    bn::string<48> s = "Score "; s += pad6(run.score); s += "     Rank "; s += bn::to_string<4>(hs_new_rank + 1);
    begin_initials("NEW HIGH SCORE!", s);
}

// The initials screen, for a high score or a Time Attack time
bool ta_entering = false;   // the initials are for a Time Attack time (ta_new_rank on this level's table)
int ta_new_rank = -1;
void begin_initials(const bn::string_view& head, const bn::string_view& line)
{
    state = state_t::initials; music_stop();
    clear_overlay(); set_dim(true);
    overlay_line(-56, head, true);
    overlay_line(-34, line);
    overlay_line(26, "UP/DOWN: letter     LEFT/RIGHT: move");
    overlay_line(38, "A: next     B: back     START: done");
    for(int i = 0; i < 3; ++i) { hs_letter[i] = hs_char_index(hs_now().last[i]); hs_draw_letter(i); }
    hs_pos = 0; hs_repeat = 0; hs_blink = 0;
    hs_draw_mark();
    sfx::oneup();
}

void finish_initials()
{
    char name[3];
    for(int i = 0; i < 3; ++i) name[i] = HS_CHARS[hs_letter[i]];
    if(ta_entering)
    {
        ta_entering = false;
        ta_insert(li, ta_new_rank, name, lvl.time);
        sfx::check();
        show_ta_select(ta_new_rank);
        return;
    }
    hs_insert(hs_new_rank, name, run.score, hs_reached);
    sfx::check();
    show_scores(false, hs_new_rank);
}

void update_initials()
{
    // UP/DOWN step through the letters, repeating while held
    int step = 0;
    if(K.up || K.down)
    {
        if(hs_repeat == 0 || (hs_repeat >= 16 && (hs_repeat - 16) % 5 == 0)) step = K.up ? 1 : -1;
        ++hs_repeat;
    }
    else hs_repeat = 0;
    if(step)
    {
        hs_letter[hs_pos] = (hs_letter[hs_pos] + step + HS_NCHARS) % HS_NCHARS;
        hs_draw_letter(hs_pos); sfx::letter();
    }
    if(K.start_pressed || (K.a_pressed && hs_pos == 2)) { finish_initials(); return; }
    int move = K.a_pressed || K.right_pressed ? 1 : K.b_pressed || K.left_pressed ? -1 : 0;
    if(move && hs_pos + move >= 0 && hs_pos + move <= 2) { hs_pos += move; hs_draw_mark(); hs_blink = 0; sfx::letter(); }
    // The letter being chosen blinks
    ++hs_blink;
    for(int i = 0; i < 3; ++i) for(bn::sprite_ptr& s : hs_letter_spr[i]) s.set_visible(i != hs_pos || hs_blink % 40 < 28);
}

int hs_view = 0;   // the table on show: 0 Normal, 1 Hard, 2 Expert (LEFT/RIGHT switch)
void draw_scores(int highlight)
{
    const hs_table& hs = hs_tables[hs_view];
    hs_clear_sprites(); clear_overlay();
    overlay_line(-58, "HIGH SCORES", true);
    static constexpr const char* heads[DIFFICULTIES] = { "<  EASY  >", "<  NORMAL  >", "<  HARD  >" };
    overlay_line(-42, heads[hs_view]);
    for(int i = 0; i < HS_COUNT; ++i)
    {
        const hs_entry& e = hs.e[i];
        const int y = -30 + i * 14;
        bn::ivector<bn::sprite_ptr>& out = i == highlight ? static_cast<bn::ivector<bn::sprite_ptr>&>(hs_row_spr) : overlay_spr;
        text8->set_bg_priority(0);
        bn::string<4> rank = bn::to_string<2>(i + 1); rank += '.';
        text8->set_left_alignment(); text8->generate(-92, y, rank, out);
        if(e.score <= 0) { text8->generate(-70, y, "---", out); continue; }
        char name[4] = { e.name[0], e.name[1], e.name[2], 0 };
        text8->generate(-70, y, name, out);
        text8->set_right_alignment(); text8->generate(14, y, pad6(e.score), out);
        text8->set_left_alignment();
        text8->generate(28, y, e.level < level_count ? bn::string_view(levels[e.level].name) : bn::string_view("Game beaten"), out);
    }
    overlay_line(54, scores_from_title ? "LEFT/RIGHT: difficulty     B: back" : "LEFT/RIGHT: difficulty     A: continue");
}
// The table. highlight = the row just entered (it blinks), or -1. It opens on this run's difficulty (Normal from the title)
void show_scores(bool from_title, int highlight)
{
    hs_load();
    state = state_t::scores; scores_from_title = from_title;
    set_dim(true);
    hs_view = from_title ? 0 : difficulty;
    hs_new_rank = highlight; hs_blink = 0;
    draw_scores(highlight);
}

void update_scores()
{
    ++hs_blink;
    for(bn::sprite_ptr& s : hs_row_spr) s.set_visible(hs_blink % 40 < 28);
    if(K.left_pressed || K.right_pressed)
    {
        hs_view = (hs_view + (K.right_pressed ? 1 : DIFFICULTIES - 1)) % DIFFICULTIES; sfx::letter();
        // The new entry's row only blinks on its own table
        draw_scores(! scores_from_title && hs_view == difficulty ? hs_new_rank : -1);
    }
    if(scores_from_title)
    {
        if(K.a_pressed || K.b_pressed || K.start_pressed || K.select_pressed) { hs_clear_sprites(); show_title(true); }
    }
    else if(K.a_pressed || K.start_pressed)
    {
        hs_clear_sprites();
        if(hs_then_stats) show_win_stats(); else show_title();
    }
}

// ---------- Time Attack (user, 2026-10-25) ----------
// From the title menu: a screen to pick any of the 16 levels (LEFT/RIGHT), showing that level's 5 best times. A plays it as
// on Normal, with unlimited lives; the level timer (the HUD's) is the time, and it keeps running while Mario is dead. The
// clear screen shows the time; one fast enough for the table goes to the initials, then back here with the new row
// blinking. SELECT on the clear screen (or in the pause menu) retries; B in the pause menu quits back here.
void draw_ta(int highlight)
{
    const ta_table& t = ta_tables[ta_level];
    hs_clear_sprites(); clear_overlay();
    overlay_line(-58, "TIME ATTACK", true);
    bn::string<32> h = "<   "; h += levels[ta_level].name; h += "   >";
    overlay_line(-38, h);
    for(int i = 0; i < HS_COUNT; ++i)
    {
        const ta_entry& e = t.e[i];
        const int y = -22 + i * 13;
        bn::ivector<bn::sprite_ptr>& out = i == highlight ? static_cast<bn::ivector<bn::sprite_ptr>&>(hs_row_spr) : overlay_spr;
        text8->set_bg_priority(0);
        bn::string<4> rank = bn::to_string<2>(i + 1); rank += '.';
        text8->set_left_alignment(); text8->generate(-56, y, rank, out);
        if(! e.used) { text8->generate(-34, y, "---", out); continue; }
        char name[4] = { e.name[0], e.name[1], e.name[2], 0 };
        text8->generate(-34, y, name, out);
        text8->set_right_alignment(); text8->generate(56, y, fmt_time(e.time), out);
        text8->set_left_alignment();
    }
    overlay_line(46, "LEFT/RIGHT: level");   // (two lines: one ran off the screen)
    overlay_line(58, "A: play     B: back");
}
void show_ta_select(int highlight)
{
    state = state_t::ta_select; time_attack = false;
    ta_load();
    if(li != title_level) load_level(title_level);   // over the title's panning Hell, with its music
    set_dim(true);
    ta_new_rank = highlight; hs_blink = 0;
    draw_ta(highlight);
}
void update_ta_select()
{
    ++hs_blink;
    for(bn::sprite_ptr& s : hs_row_spr) s.set_visible(hs_blink % 40 < 28);
    if(K.left_pressed || K.right_pressed || K.up_pressed || K.down_pressed)
    {
        ta_level = (ta_level + (K.right_pressed || K.down_pressed ? 1 : level_count - 1)) % level_count;
        sfx::letter(); draw_ta(-1);
    }
    else if(K.a_pressed || K.start_pressed)
    {
        hs_clear_sprites();
        set_difficulty(0); time_attack = true;
        start_game(ta_level);
    }
    else if(K.b_pressed) { hs_clear_sprites(); set_difficulty(ta_prev_difficulty); show_title(true); }
}
// The clear screen in Time Attack: the time, and where it ranks
void ta_clear()
{
    sfx::clear();
    ta_new_rank = ta_rank(li, lvl.time);
    clear_overlay(); set_dim(true);
    bn::string<32> title = LV().name; title += " cleared";
    overlay_line(-44, title, true);
    bn::string<32> t = "Time "; t += fmt_time(lvl.time);
    overlay_line(-16, t, true);
    const ta_entry& best = ta_tables[li].e[0];
    if(ta_new_rank == 0 && best.used) overlay_line(8, "New record!");
    else if(ta_new_rank == 0) overlay_line(8, "First time recorded!");
    else if(ta_new_rank > 0) { bn::string<32> r = "Rank "; r += bn::to_string<4>(ta_new_rank + 1); r += " of 5!"; overlay_line(8, r); }
    if(best.used && ta_new_rank != 0) { bn::string<32> b = "Best "; b += fmt_time(best.time); overlay_line(20, b); }
    overlay_line(46, ta_new_rank >= 0 ? "A: enter initials     SELECT: retry" : "A: continue     SELECT: retry");
}
// A on the Time Attack clear screen
void ta_continue()
{
    if(ta_new_rank < 0) { show_ta_select(-1); return; }
    ta_entering = true;
    bn::string<48> s = LV().name; s += "     Time "; s += fmt_time(lvl.time); s += "     Rank "; s += bn::to_string<4>(ta_new_rank + 1);
    begin_initials("NEW BEST TIME!", s);
}

void retry_level()
{
    run.lives = MAX_LIVES; run.coins -= lvl.coins; run.score -= lvl.score; run.life_coins = 0;
    run.grenades += lvl.grenades;   // the grenades this attempt threw come back
    load_level(li); start_play();
}

void replay_level()
{
    // A restart loses the coins toward the next 1-up, and the 1-ups this attempt's coins bought (or the same coins
    // could be collected again for more lives)
    run.coins -= lvl.coins; run.score -= lvl.score; run.life_coins = 0;
    run.lives = bn::max(1, run.lives - lvl.coin_lives);
    run.grenades += lvl.grenades;   // (and the grenades it threw come back)
    if(state == state_t::clear)
    {
        run.time -= lvl.time; run.deaths -= lvl.deaths; run.embers -= lvl.embers; run.total -= ember_total;
    }
    load_level(li); start_play();
}

void next_level() { load_level(li + 1); start_play(); }

// After World 1-4, a plain black "WORLD 2" screen (the user's request, 2026-10-16) before World 2-1 starts. The level
// sprites and HUD hide outside play, and the background fade at full strength turns every layer and the backdrop black.
constexpr int WORLD_CARD_FRAMES = 150, WORLD_CARD_SKIP = 30;   // shows for 2.5 s; A or START skips it after half a second
int world_card_t = 0, world_card_frames = WORLD_CARD_FRAMES;

void show_world_card(const bn::string_view& text)
{
    state = state_t::world_card; world_card_t = 0; world_card_frames = WORLD_CARD_FRAMES;
    clear_overlay();
    bn::bg_palettes::set_fade(bn::color(0, 0, 0), 1);
    if(! text.empty()) overlay_line(-8, text, true);
}

// After a "WORLD n" card, another one about grenades (grenade_card_next = n): Worlds 1, 2 and 3's (every difficulty,
// user 2026-10-29) and World 4's: on Easy and Normal like World 2's (user, 2026-10-29), on Expert its own about the
// grenade launcher (user request, 2026-10-25)
int grenade_card_next = 0;
int world_card_level = -1;   // the level a card leads to (a new game's World 1-1), or -1: the next one

void show_grenade_card(int world)
{
    show_world_card("");
    world_card_frames = WORLD_CARD_FRAMES * 2;   // more to read: 5 s
    // The grenade (2x) and how many he gets, "x8" ("x12" in Expert's World 4, user 2026-10-29), side by side, centred
    const bool expert_w4 = world == 4 && difficulty == 2;
    const int count = world_grenades(world), shift = count >= 10 ? -3 : 0;
    bn::sprite_ptr icon = bn::sprite_items::grenade.create_sprite(-10 + shift, -28, 0);
    icon.set_scale(2); icon.set_bg_priority(0); overlay_spr.push_back(bn::move(icon));
    bn::string<8> n = "x"; n += bn::to_string<4>(count);
    text8->set_left_alignment(); text8->set_bg_priority(0); text8->generate(2 + shift, -28, n, overlay_spr);
    // (the user's wording for each, 2026-10-29; Expert's World 4 card was "Hellfire has found a grenade launcher!" from
    // 2026-10-25, "some grenades!" before that, and keeps its full stop)
    overlay_line(-8, world == 1 ? "Hellfire has found some grenades!" : "Hellfire found more grenades!");
    overlay_line(6, expert_w4 ? "Toss grenades with R." : "Toss grenades with R");
}

void leave_level()
{
    if(LV().name == bn::string_view("World 1-4")) { show_world_card("WORLD 2"); grenade_card_next = 2; }
    else if(LV().name == bn::string_view("World 2-4")) { show_world_card("WORLD 3"); grenade_card_next = 3; }   // the user's request, 2026-10-16
    else if(LV().name == bn::string_view("World 3-4")) { show_world_card("WORLD 4"); grenade_card_next = 4; }   // the user's request, 2026-10-16
    else next_level();
}

// A new game from the menu: from World 1-1 it opens with a black "WORLD 1" card, then the grenade card
// (user, 2026-10-29); a test ROM starting further on goes straight in. (Time Attack and the test scripts use start_game.)
void new_game(int level = 0)
{
    if(level != 0) { start_game(level); return; }
    run = run_t();
    world_card_level = 0; grenade_card_next = 1;
    show_world_card("WORLD 1");
}

// When a card's time is up (or A/START skips it): the grenade card if one is due, else the level
void end_world_card()
{
    if(grenade_card_next) { int w = grenade_card_next; grenade_card_next = 0; show_grenade_card(w); return; }
    if(world_card_level >= 0) { int l = world_card_level; world_card_level = -1; load_level(l); start_play(); }
    else next_level();
}

// ---------- Drawing ----------
BN_DATA_EWRAM wspr door_spr;
BN_DATA_EWRAM wspr gate_spr[7];

void draw_parallax(bn::regular_bg_ptr& bg, const parallax_look& pl)
{
    int off = (cam_x * pl.fx).floor_integer() % pl.period;
    fixed top = -96 + (max_cam_y() - cam_y) * pl.fy;
    bg.set_position(pl.map_w / 2 - SCREEN_W / 2 - off, top + 128 - SCREEN_H / 2);
}

void draw_boss()
{
    if(! boss) return;
    boss_t& B = *boss;
    bool show = B.state != boss_t::idle && B.state != boss_t::dead && (B.state != boss_t::dying || (B.t / 3) % 2 == 0 || B.t < 60);
    const boss_spec& S = spec(B.kind);
    int f = B.kind == BOSS_MECH ? (B.x.integer() / S.anim) % 7 : (B.t / S.anim) % 8;   // the mech's 7 steps follow its x
    bool flip = S.faces_right ? B.face < 0 : B.face > 0;   // face Mario
    const bn::sprite_item& item = *S.item;
    fixed sx = B.x + (flip ? -S.spr_dx : S.spr_dx), sy = B.y + S.spr_dy;
    bool tell = (B.mode == boss_t::m_volley && B.mt < 30) || (B.mode == boss_t::m_dive && B.mt <= (B.kind == BOSS_MECH ? 30 : 40));
    bool flash = (B.hit > 0 && B.hit % 4 < 2) || (B.state == boss_t::fight && tell && (B.mt / 4) % 2) || B.state == boss_t::dying;
    for(int i = 0; i < 4; ++i)
    {
        bn::sprite_ptr* s = ensure(B.q[i], show, item, f * 4 + i, 4, false, true);
        if(! s) continue;
        int col = i % 2, row = i / 2;
        int side = flip ? 1 - col : col;
        place(B.q[i], sx - 32 + side * 64, sy - 32 + row * 64, flip);
        set_flash(B.q[i], flash, item, *S.flash);
    }
}

// The zoomed view follows Mario (the user's call), clamped to the arena: 480 px wide from the gate, 272 px tall
void update_zoom_view()
{
    fixed tx = bn::max(zoom_x0, bn::min(zoom_x0 + 2 * SCREEN_W - ZOOM_VIEW_W, p.x + p.w / 2 - ZOOM_VIEW_W / 2));
    fixed ty = bn::max(fixed(0), bn::min(fixed(ROWS * T) - ZOOM_VIEW_H, p.y + p.h / 2 - ZOOM_VIEW_H / 2));
    if(zoom_vx < 0) { zoom_vx = tx; zoom_vy = ty; }
    else if(state == state_t::play) { zoom_vx += (tx - zoom_vx) * fixed(0.12); zoom_vy += (ty - zoom_vy) * fixed(0.12); }
    // The picture's top-left is the arena's top-left (the gate column, world y 0)
    bg_arena->set_position(zoom_sx(zoom_x0) + 256, zoom_sy(0) + 128);
}

// ---------- Darkness ----------
// The web game lays a darkness over each level and cuts soft holes of light in it. Here the darkness is the hardware's
// fade-to-black blend over the scenery: the backgrounds, platforms, particles and sparks. The light holes were ported
// too, but the user had them all removed (their dithered edges looked grainy). Instead, everything that had a light
// (Mario, enemies and their shots, coins, embers, bullets, the door, checkpoints, the 1-up and mushrooms) stays out of
// the darkness (see attach()).
constexpr int DARK16[4] = { 5, 7, 7, 5 };   // darkness in 16ths: the web game's 0.34 and 0.42; Hell is lighter than
                                            // the web's 0.55 at the user's request; World 3-1's city as World 1-1
bool dark_on = false;

void update_darkness()
{
    bool on = world_visible;   // in play only: the title, pause and end screens have their own dimming
    if(on) bn::blending::set_fade_alpha(fixed::from_data(DARK16[LV().theme] << 8));   // n/16
    else if(dark_on) bn::blending::set_fade_alpha(0);
    dark_on = on;
}

// Hell's sparks drift up the screen at their own parallax, flickering as they go (the web game's drawHellFire). They're
// particle dots in screen space, in front of the parallax layers and behind the level.
constexpr int SPARKS = 12;
struct spark_t { bn::optional<bn::sprite_ptr> s; fixed y, speed; int par, phase, at; bool shown; };
BN_DATA_EWRAM spark_t sparks[SPARKS];
fixed spark_wobble = 0;   // the shared wobble angle, in degrees

void draw_sparks()
{
    bool on = LV().theme == 2 && world_visible;
    if(! on)
    {
        if(sparks[0].s) for(spark_t& sp : sparks) sp.s.reset();
        return;
    }
    if(! (frame & 1) && sparks[0].s) return;   // moved 30 times a second, which is plenty for them and halves their cost
    spark_wobble += fixed(3.82);   // the web game's sin(now * 0.002): 1.91 degrees a frame
    if(spark_wobble >= 360) spark_wobble -= 360;
    int cx = cam_x.round_integer(), blink = (frame / 6) % 6;
    for(int i = 0; i < SPARKS; ++i)
    {
        spark_t& sp = sparks[i];
        if(! sp.s)
        {
            // Particle frames: colour 1 (#ffb347) or 2 (#ffd98a), 1 or 2 px
            sp.s = bn::sprite_items::particle.create_sprite_optional(0, 0, (i % 3 ? 2 : 4) + (i % 4 ? 0 : 1));
            if(! sp.s) continue;
            sp.s->set_bg_priority(3);
            sp.s->set_z_order(20);
            sp.s->set_blending_enabled(true);
            sp.speed = fixed(12 + 4 * (i % 7)) / 60;   // 0.2 to 0.6 px a frame
            sp.y = 170 - (i * 97) % 180;
            sp.par = 38 + 15 * (i % 5);                 // parallax 0.3 to 0.78, in 128ths
            sp.phase = (i * 57) % 360;
            sp.at = -1; sp.shown = true;
        }
        sp.y -= sp.speed * 2;
        if(sp.y < -10) sp.y += 180;
        fixed a = spark_wobble + sp.phase;
        if(a >= 360) a -= 360;
        int x = i * 131 - ((cx * sp.par) >> 7) + (bn::degrees_lut_sin(a) * 10).round_integer();
        x %= SCREEN_W;
        if(x < 0) x += SCREEN_W;
        int y = sp.y.round_integer();
        int at = (x << 16) | (y & 0xffff);
        if(at != sp.at) { sp.s->set_position(x - SCREEN_W / 2, y - SCREEN_H / 2); sp.at = at; }
        int b = blink + i % 6;
        bool shown = b != 6 && b != 0;   // flicker out now and then
        if(shown != sp.shown) { sp.s->set_visible(shown); sp.shown = shown; }
    }
}

void draw()
{
    bool want_zoom = boss_active() && (state == state_t::play || state == state_t::pause);
    if(want_zoom != zoom) set_zoom(want_zoom);
    if(zoom) update_zoom_view();
    update_active();
    world_visible = state == state_t::play;
    parts_visible = state == state_t::play || state == state_t::congrats;
    fixed sx = shake > 0 ? (rng.get_fixed(1) - fixed(0.5)) * shake : fixed(0);
    fixed sy = shake > 0 ? (rng.get_fixed(1) - fixed(0.5)) * shake : fixed(0);
    camera->set_position(cam_x + SCREEN_W / 2 + sx, cam_y + SCREEN_H / 2 + sy);
    const level_look& lk = look();
    draw_parallax(*bg_far, lk.far);
    if(bg_mid) draw_parallax(*bg_mid, lk.mid);
    draw_parallax(*bg_near, lk.near);

    for(crack_t& c : drawn(cracks, crack_span))
    {
        // (16x32 sprite, the block at its bottom reaching crack_rise px above its tile)
        bool show = tile(c.tx, c.ty) == 4 && near_view(c.tx * T, c.ty * T - crack_rise, 16, 16 + crack_rise);
        if(ensure(c.spr, show, bn::sprite_items::crack, lk.crack_frame, 10)) place(c.spr, c.tx * T + 8, c.ty * T);
    }
    // The boss arena's gate is part of the level grid while sealed; it's drawn as a column of stone sprites
    for(int i = 0; i < 7; ++i)
    {
        bool show = boss && ! zoom && tile(boss->gate, i * 2) == 1 && near_view(boss->gate * T, i * 32, 16, 32);   // the arena picture has the gate
        if(ensure(gate_spr[i], show, bn::sprite_items::gate, 0, 10)) place(gate_spr[i], boss->gate * T + 8, i * 32 + 16);
    }
    for(check_t& c : checks)
    {
        int f = c.lit ? 1 + ((frame / 13) & 1) : 0;
        if(ensure(c.spr, near_view(c.x, c.y - 16, 16, 32), bn::sprite_items::checkpoint, f, 12, false, true)) place(c.spr, c.x + 8, c.y);
    }
    if(ensure(door_spr, has_door && near_view(door.x - 8, door.y - 16, 32, 64), bn::sprite_items::door, 0, 12, false, true)) place(door_spr, door.x + 8, door.y + 16);
    for(mover_t& m : movers)
    {
        if(ensure(m.spr, near_view(m.x - 8, m.y - 12, 64, 32), bn::sprite_items::platform, lk.platform_frame, 11)) place(m.spr, m.x - 8 + 32, m.y - 12 + 16);
    }
    for(ember_t& em : drawn(embers, ember_span))
    {
        if(em.got) continue;
        fixed bob = sin_deg(em.t * fixed(4.58)) * fixed(1.5);
        if(ensure(em.spr, near_view(em.x - 4, em.y - 4, 8, 8), bn::sprite_items::ember, 0, 8, false, true)) place(em.spr, em.x, em.y + bob);
    }
    for(mush_t& m : drawn(mushes, mush_span, mushes_static))
    {
        if(m.got) continue;
        fixed bob = m.fall ? fixed(0) : sin_deg(m.t * fixed(3.44)) - 1;
        if(ensure(m.spr, near_view(m.x, m.y, 12, 12), bn::sprite_items::mush, m.blue ? 1 : 0, 8, false, true)) place(m.spr, m.x - 2 + 8, m.y + bob - 2 + 8);
    }
    for(coin_t& c : drawn(coins, coin_span, coins_static))
    {
        if(c.got) continue;
        // Spin by squeezing: |cos(t * 0.07)| picks one of five widths
        int ph = c.t % 45, tri = ph < 22 ? 22 - ph : ph - 22;   // 0..22 ~ |cos| x 22
        int f = tri > 20 ? 0 : tri > 16 ? 1 : tri > 11 ? 2 : tri > 6 ? 3 : 4;
        if(ensure(c.spr, near_view(c.x - 4, c.y - 5, 8, 10), bn::sprite_items::coin, f, 9, false, true)) place(c.spr, c.x, c.y);
    }
    for(oneup_t& u : oneups)
    {
        if(u.got) continue;
        fixed bob = u.rest ? sin_deg(u.t * fixed(3.44)) - 1 : fixed(0);
        if(ensure(u.spr, near_view(u.x, u.y, 14, 14), bn::sprite_items::oneup, 0, 8, false, true)) place(u.spr, u.x + 7, u.y + 7 + bob);
    }
    {
        fixed bob = sin_deg(flame_item.t * fixed(3.44)) - 1;
        if(ensure(flame_item.spr, flame_item.live && near_view(flame_item.x - 16, flame_item.y - 8, 32, 16), bn::sprite_items::flame_pickup, 0, 8, false, true))
            place(flame_item.spr, flame_item.x, flame_item.y + bob);
    }
    {
        fixed bob = rocket_item.fall ? fixed(0) : sin_deg(rocket_item.t * fixed(3.44)) - 1;
        if(ensure(rocket_item.spr, rocket_item.live && near_view(rocket_item.x - 16, rocket_item.y - 8, 32, 16), bn::sprite_items::rocket_pickup, 0, 8, false, true))
            place(rocket_item.spr, rocket_item.x, rocket_item.y + bob);
    }
    {
        // The star flashes from the moment it appears
        star_t& st = star_item;
        fixed bob = st.rest ? sin_deg(st.t * fixed(3.44)) - 1 : fixed(0);
        if(ensure(st.spr, st.live && ! st.got && near_view(st.x, st.y, 16, 16), bn::sprite_items::star, 0, 8, false, true))
        {
            place(st.spr, st.x + 8, st.y + 8 + bob);
            star_flash(st.spr, (frame / 4) % 4);
        }
    }
    for(enemy_t& e : drawn(enemies, enemy_span))
    {
        fixed cx = e.x + e.w / 2, foot = e.y + e.h;
        if(e.kind == FOE_ALIEN)
        {
            bool show = (e.alive || e.squish <= 24) && near_view(cx - 32, foot - 64, 64, 64);
            bool idle = e.alert || ! e.alive || e.a == e.b;   // (a guard: Expert's 2-column patrols)
            int gfx = idle ? 6 + (e.t / 8) % 4 : (e.x.floor_integer() / 4) % 6;
            bn::sprite_ptr* s = ensure(e.spr, show, bn::sprite_items::alien, gfx, 5, false, true);
            if(! s) continue;
            int dir = e.alert ? e.dir : e.a == e.b ? (p.x > e.x ? 1 : -1) : (e.vx > 0 ? 1 : -1);   // a guard watches Mario
            place(e.spr, cx, foot - 32, dir > 0);   // the art faces left
            set_flash(e.spr, ! e.alive || (e.hit > 0 && e.hit % 4 < 2) || (e.windup > 0 && (e.windup / 3) % 2), bn::sprite_items::alien, bn::sprite_palette_items::alien_flash);
            s->set_visible(e.alive || (e.squish / 2) % 2 == 0);
        }
        else if(e.kind == FOE_MECH)
        {
            // A mech walks (10 frames, stepped by distance) and stands still on its first frame while it attacks
            bool show = (e.alive || e.squish <= 24) && near_view(cx - 32, foot - 64, 64, 64);
            int gfx = e.alert || ! e.alive || e.a == e.b ? 0 : (e.x.floor_integer() / 3) % 10;
            bn::sprite_ptr* s = ensure(e.spr, show, bn::sprite_items::mech, gfx, 5, false, true);
            if(! s) continue;
            int dir = e.alert ? e.dir : e.a == e.b ? (p.x > e.x ? 1 : -1) : (e.vx > 0 ? 1 : -1);   // a guard watches Mario
            place(e.spr, cx, foot - 32, dir < 0);   // the art faces right
            set_flash(e.spr, ! e.alive || (e.hit > 0 && e.hit % 4 < 2) || (e.windup > 0 && (e.windup / 3) % 2), bn::sprite_items::mech, bn::sprite_palette_items::mech_flash);
            s->set_visible(e.alive || (e.squish / 2) % 2 == 0);
        }
        else if(e.kind == FOE_DEMON)
        {
            // A flying demon beats its wings (frames 0-5), faster while it chases Mario; its hitbox is the wing beat's box
            // (user, 2026-10-29), which sits demon_spr_dx2/dy2 half px off the sprite's centre; the art faces left
            int dir = e.alert || e.volley ? e.dir : (e.vx > 0 ? 1 : -1);
            fixed sx = cx + fixed(dir > 0 ? -demon_spr_dx2 : demon_spr_dx2) / 2, cy = e.y + e.h / 2 + fixed(demon_spr_dy2) / 2;
            bool show = (e.alive || e.squish <= 24) && near_view(sx - 32, cy - 32, 64, 64);
            int gfx = (e.t / (e.alert ? 3 : 5)) % 6;
            bn::sprite_ptr* s = ensure(e.spr, show, bn::sprite_items::demon, gfx, 5, false, true);
            if(! s) continue;
            place(e.spr, sx, cy, dir > 0);
            set_flash(e.spr, ! e.alive || (e.hit > 0 && e.hit % 4 < 2) || (e.windup > 0 && (e.windup / 3) % 2), bn::sprite_items::demon, bn::sprite_palette_items::demon_flash);
            s->set_visible(e.alive || (e.squish / 2) % 2 == 0);
        }
        else if(e.kind == FOE_REAPER)
        {
            // Buried, a reaper shows the first rise frame (a dirt mound); the 11 rise frames climb it out of the grave,
            // and played backwards they sink it when it's shot
            int gfx;
            if(! e.alive) gfx = 6 + bn::max(0, 10 - e.squish / 3);
            else if(e.rise < RISE_END) gfx = 6 + bn::min(10, e.rise / 5);
            else if(e.alert) gfx = 2;   // stands upright, lantern raised, while it attacks
            else gfx = (e.x.floor_integer() / 4) % 6;
            bool show = (e.alive || e.squish < DEATH_SINK) && near_view(cx - 32, foot - 64, 64, 64);
            bn::sprite_ptr* s = ensure(e.spr, show, bn::sprite_items::reaper, gfx, 5, false, true);
            if(! s) continue;
            int dir = e.alert ? e.dir : (e.vx > 0 ? 1 : -1);
            place(e.spr, cx, foot - 32, dir < 0);   // the art faces right
            set_flash(e.spr, ! e.alive || (e.hit > 0 && e.hit % 4 < 2) || (e.windup > 0 && (e.windup / 3) % 2), bn::sprite_items::reaper, bn::sprite_palette_items::reaper_flash);
        }
        else
        {
            // Hell beasts: idle, a breath wind-up, then a burning death; two sprites tall
            int f;
            if(! e.alive) f = 10 + bn::min(5, e.squish / 6);
            else if(e.windup > 0) f = 6 + bn::min(3, e.windup / 6);
            else f = (e.t / 8) % 6;
            bool show = (e.alive || e.squish < BEAST_BURN_END) && (e.alive || e.squish < 37 || (e.squish / 2) % 2 == 0) && near_view(cx - 32, foot - 128, 64, 128);
            bool flip = e.dir > 0;
            bool flash = e.alive && e.hit > 0 && e.hit % 4 < 2;
            if(ensure(e.spr, show, bn::sprite_items::beast, f * 2, 5, false, true))
            {
                place(e.spr, cx, foot - 96, flip);
                set_flash(e.spr, flash, bn::sprite_items::beast, bn::sprite_palette_items::beast_flash);
            }
            if(ensure(e.spr2, show, bn::sprite_items::beast, f * 2 + 1, 5, false, true))
            {
                place(e.spr2, cx, foot - 32, flip);
                set_flash(e.spr2, flash, bn::sprite_items::beast, bn::sprite_palette_items::beast_flash);
            }
        }
    }
    draw_boss();
    for(bullet_t& b : bullets)
    {
        if(b.vy != 0 && b.vx != 0)   // fired at 45 degrees
        {
            if(b.rocket ? ensure(b.spr, near_view(b.x - 16, b.y - 16, 32, 32), bn::sprite_items::rocket_diag, (frame / 3) & 1, 2, false, true)
                        : ensure(b.spr, near_view(b.x - 8, b.y - 8, 16, 16), bn::sprite_items::bullet_diag, 0, 2, false, true))
                place(b.spr, b.x, b.y, b.vx < 0);
        }
        else if(b.vy != 0)   // fired upward
        {
            if(b.rocket ? ensure(b.spr, near_view(b.x - 8, b.y - 16, 16, 32), bn::sprite_items::rocket_up, (frame / 3) & 1, 2, false, true)
                        : ensure(b.spr, near_view(b.x - 4, b.y - 8, 8, 16), bn::sprite_items::bullet_up, 0, 2, false, true))
                place(b.spr, b.x, b.y);
        }
        else if(b.rocket)
        {
            if(ensure(b.spr, near_view(b.x - 16, b.y - 8, 32, 16), bn::sprite_items::rocket, (frame / 3) & 1, 2, false, true))
                place(b.spr, b.x, b.y, b.vx < 0);
        }
        else if(ensure(b.spr, near_view(b.x - 8, b.y - 4, 16, 8), bn::sprite_items::bullet, 0, 2, false, true))
        {
            place(b.spr, b.x, b.y, b.vx < 0);
        }
    }
    for(grenade_t& g : grenades)   // (spinning the way it flies)
        if(ensure(g.spr, near_view(g.x - 4, g.y - 4, 8, 8), bn::sprite_items::grenade, (g.vx < 0 ? 400 - g.t / 4 : g.t / 4) % 4, 2, false, true))
            place(g.spr, g.x, g.y);
    for(boom_t& b : booms)
        if(ensure(b.spr, near_view(b.x - 32, b.y - 32, 64, 64), bn::sprite_items::boom, bn::min(b.t / BOOM_STEP, BOOM_FRAMES - 1), 1, false, true))
            place(b.spr, b.x, b.y);
    for(glob_t& g : globs)
    {
        if(g.fire)
        {
            if(ensure(g.spr, near_view(g.x - 8, g.y - 8, 16, 16), bn::sprite_items::fireball, g.dir * 3 + (g.t / 5) % 3, 2, false, true)) place(g.spr, g.x, g.y);
        }
        else if(ensure(g.spr, near_view(g.x - 4, g.y - 4, 8, 8), bn::sprite_items::glob, 0, 2, false, true)) place(g.spr, g.x, g.y);
    }
    {
        int pieces = state == state_t::play && ! p.dead ? (flame_len + 8) / 16 : 0, flicker = (frame / 3) % 3;
        fixed mx = muzzle_x(), my = muzzle_y();
        // The 45-degree pieces are another sprite size, so the pieces go when the aim swaps to or from it
        static bool drawn_diag = false;
        if(drawn_diag != (p.aim == AIM_DIAG)) { for(wspr& w : flame_spr) w.reset(); drawn_diag = p.aim == AIM_DIAG; }
        if(p.aim == AIM_DIAG)   // 32x32 pieces whose centres are 16 px apart on each axis (22.6 px along the flame)
        {
            pieces = state == state_t::play && ! p.dead ? (flame_len + 11) * 10 / 226 : 0;
            for(int i = 0; i < FLAME_PIECES; ++i)
                if(ensure(flame_spr[i], i < pieces && i < 5, bn::sprite_items::flame_diag, flicker * 5 + bn::min(i, 4), 2, false, true))
                    place(flame_spr[i], mx + p.face * (i * 16 + 8), my - (i * 16 + 8), p.face < 0);
            pieces = 0;
        }
        for(int i = 0; i < FLAME_PIECES; ++i)
        {
            if(p.aim == AIM_DIAG) {}
            else if(p.aim == AIM_UP)   // frames 18-35: the pieces turned to point up
            {
                if(ensure(flame_spr[i], i < pieces, bn::sprite_items::flame_beam, (3 + flicker) * FLAME_PIECES + i, 2, false, true))
                    place(flame_spr[i], mx, my - (i * 16 + 8));
            }
            else if(ensure(flame_spr[i], i < pieces, bn::sprite_items::flame_beam, flicker * FLAME_PIECES + i, 2, false, true))
                place(flame_spr[i], mx + p.face * (i * 16 + 8), my, p.face < 0);
        }
    }
    for(part_t& q : parts)
    {
        bool show = near_view(q.x, q.y, 2, 2) && (q.life * 4 > q.max || (q.life & 1));
        if(ensure(q.spr, show, bn::sprite_items::particle, q.colour * 2 + (q.size - 1), 1, true)) place(q.spr, q.x - fixed(0.5), q.y - fixed(0.5));
    }

    // Mario
    bool show_mario = state != state_t::title && ! p.dead && ! (p.inv > 0 && (p.inv / 4) % 2);
    // One sheet (and palette) per weapon: a new weapon means a new sprite
    static constexpr const bn::sprite_item* hero_items[4] = { &bn::sprite_items::hero, &bn::sprite_items::hero_flame,
                                                              &bn::sprite_items::hero_rocket, &bn::sprite_items::hero_grenade };
    static int shown_set = -1;
    int set = hero_set();
    if(set != shown_set) { mario_spr.reset(); shown_set = set; }
    // With the star the hero flashes, slower in its last two seconds
    int star_k = p.star <= 0 || p.dead ? 0 : (frame / (p.star > STAR_WARN ? 3 : 6)) % 4;
    // Lying prone (any weapon), when it's too long for one sprite (hero_prone_wide, see build-assets):
    // two 64x64 sprites side by side (<sheet>_prone frames 0 and 1, a 128x64 picture with the body centre on column 64, 63
    // when flipped)
    bool prone2 = set != HERO_GRENADE && hero_prone_wide[set] && hero_gfx() == HERO_CROUCH;
    const bn::sprite_item& prone_item = set == HERO_FLAME ? bn::sprite_items::hero_flame_prone :
        set == HERO_ROCKET ? bn::sprite_items::hero_rocket_prone : bn::sprite_items::hero_prone;
    static int prone_shown = -1;
    if(prone2 && set != prone_shown) { prone_spr[0].reset(); prone_spr[1].reset(); prone_shown = set; }
    if(ensure(mario_spr, show_mario && ! prone2, *hero_items[set], hero_gfx(), 0, false, true))
    {
        // 64x64 frames: body centre is column 24 of the frame (column 39 when flipped); the feet are on the bottom row
        fixed tlx = p.x + PW / 2 - (p.face > 0 ? 24 : 39);
        place(mario_spr, tlx + 32, p.y + p.h - 32, p.face < 0);
        star_flash(mario_spr, star_k);
    }
    for(int i = 0; i < 2; ++i)
    {
        // Facing left the picture is mirrored, so the left sprite shows the right half (flipped) and the other way round
        if(ensure(prone_spr[i], show_mario && prone2, prone_item, p.face > 0 ? i : 1 - i, 0, false, true))
        {
            fixed tlx = p.x + PW / 2 - (p.face > 0 ? 64 : 63);
            place(prone_spr[i], tlx + 32 + i * 64, p.y + p.h - 32, p.face < 0);
            star_flash(prone_spr[i], star_k);
        }
    }
    bool show_flash = state != state_t::title && ! p.dead && p.flash > 0;
    if(ensure(muzzle_spr, show_flash, bn::sprite_items::muzzle, p.aim == AIM_UP ? 1 : p.aim == AIM_DIAG ? 2 : 0, -1, false, true))
    {
        fixed mx = muzzle_x(), my = muzzle_y();
        if(p.aim == AIM_UP) place(muzzle_spr, mx, my - 4);
        else if(p.aim == AIM_DIAG) place(muzzle_spr, mx + p.face * 3, my - 3, p.face < 0);
        else place(muzzle_spr, p.face > 0 ? mx + 4 : mx - 4, my, p.face < 0);
    }
    update_darkness();
    draw_sparks();
    update_hud();
}

// ---------- Input ----------
#ifdef SMHB_AUTOPLAY
// Test build only: scripted input, picked with -DSMHB_SCENE=n.
//   1: Jungle - dropped at col 42 (cracked block, first aliens): stand and shoot, crouch, run and jump, then the door
//   2: Jungle - on the high ledge by the hidden 1-up block: shoot it and collect the 1-up
//   3: Graveyard - walk right shooting, so reapers rise and get shot
//   4: Hell - walk right shooting at the first hell beasts
//   5: Hell - dropped at the boss checkpoint: walk into the arena and fight the Fire Skull (with a cheat: no damage)
//   6: Graveyard - sound stress test
//   7: Jungle - visits the four GBA-only aliens (build-assets.js GBA_EXTRAS) every 3 seconds, standing still, kept alive
//   8: Graveyard - the same for its three GBA-only reapers
//   9: Graveyard - the game-over screen
//   10, 11, 12: World 1-2, 1-3, 1-4 (the GBA-only levels) - walk right shooting, like 3 and 4
//   23, 24: World 1-1, World 4-4 - the star: shoot its block, take it, then run left into the enemies (no shooting)
//   25, 26, 27: World 1-3, 2-4, 4-4 - the flamethrower: walk onto it, then hold fire at the aliens ahead (letting go now and then)
//       until it runs out
//   28: World 1-2 - the rocket launcher: the same as 25-27 with its pickup, then (~6.7 s) firing into a wall, then (~11.7 s)
//       at spikes
//   29, 30, 31: World 2-1, 3-2, 4-1 - their rocket launchers, as 28 without the wall and spikes
#ifndef SMHB_SCENE
#define SMHB_SCENE 1
#endif
[[maybe_unused]] void teleport(int col, int row)
{
    p.x = col * T + 1; p.y = row * T - PH; p.vx = 0; p.vy = 0;
    cam_x = clamp_cam_x(p.x - SCREEN_W / 2); cam_y = clamp_cam_y(p.y + p.h / 2 - SCREEN_H / 2);
}
#endif

void read_keys()
{
#ifdef SMHB_AUTOPLAY
    static int f = 0;
    ++f;
    int g = f - 150;
    K = keys_t();
    if(f == 150)
    {
        static constexpr int scene_level[] = { 0, 0, 0, 4, 15, 15, 4, 0, 4, 4, 1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 15, 2, 7, 15, 1, 4, 9, 12, 1, 2, 4, 12, 4, 3, 13, 2, 1, 0, 0, 4, 8, 0, 3, 0, 7, 12, 0, 0, 4, 6, 11, 0, 12, 11, 0, 6, 0, 15, 12, 0, 0, 3, 0, 0, 0, 0, 0, 1, 0, 4, 14, 4, 4, 14, 15, 14 };   // level indexes: 4 Graveyard, 8 City, 12 World 4-1, 15 Hell
#if SMHB_SCENE == 42 || SMHB_SCENE == 47 || SMHB_SCENE == 58 || SMHB_SCENE == 60
        return;   // this one starts from the title
#endif   // level indexes: 4 Graveyard, 8 City, 12 World 4-1, 15 Hell
        start_game(scene_level[SMHB_SCENE]);
        return;
    }
    if(g <= 0) return;
#if SMHB_SCENE == 1 || SMHB_SCENE == 50   // 50: the same with every enemy gone (Hard's all-enemies bonus)
    if(g == 2) teleport(42, 14);
#if SMHB_SCENE == 50
    if(g == 3) for(enemy_t& e : enemies) { e.alive = false; e.squish = 1000; }
#endif
    if(g == 420) teleport(433, 12);   // next to the exit door (col 436 since the level doubled), to check the level-clear screen
    K.down = g > 240 && g < 330;
    K.right = g >= 330 && (g < 420 || p.x + p.w / 2 < door.x + 8);   // stops in front of the door
    K.fire = g > 10 && g < 420;
    K.jump = g >= 330 && g < 420 && (g % 55) < 22;
    K.a_pressed = g >= 330 && g < 420 && (g % 55) == 1;
#elif SMHB_SCENE == 65
    // Standing rifle vs a wall in World 1-4 (Jungle 4): walks right into the 2-tile wall at col 59 (the rifle should stop
    // at its edge; ~1.5 s), firing from ~2 s
    if(g == 2) teleport(55, 14);
    K.right = g > 5 && g < 150;
    K.fire = g > 110;
#elif SMHB_SCENE == 64
    // Prone with the rifle in World 1-1: right up against the cracked block at col 65 (open floor behind him), facing it,
    // lies down (he backs off so his rifle stays out of it; ~1.5 s), fires (~2.3 s), then turned left (from ~3 s)
    if(g == 2) teleport(64, 14);
    K.right = g > 5 && g < 9;
    K.left = g >= 150 && g < 156;
    K.down = (g > 12 && g < 150) || g > 165;
    K.fire = (g > 110 && g < 150) || g > 165;
#elif SMHB_SCENE == 63
    // The rifle aimed at 45 degrees (UP + RIGHT, standing and walking) and straight up, firing, in World 1-1
    if(g == 2) teleport(42, 14);
    K.up = g > 10;
    K.right = g > 10 && g < 200;
    K.fire = g > 10;
#elif SMHB_SCENE == 76
    // Expert's own star block in World 2-1 (col 99, 2026-10-31): he shoots it from the checkpoint at col 97, then takes the
    // star (and stops short of the gap). On other difficulties there's no block and he just stands there.
    p.hp = MAX_HP;
    if(g == 2) { teleport(97, 14); p.face = 1; }
    K.fire = g > 40 && g < 100;
    K.right = g > 120 && g < 150 && p.x < 98 * T;
#elif SMHB_SCENE == 75
    // Expert's own 1-up block in World 2-1 (col 296, 2026-10-31): he shoots it from the right, then takes the 1-up. On other
    // difficulties there's no block and he just stands there.
    p.hp = MAX_HP;
    if(g == 2) { teleport(297, 11); p.face = -1; }
    K.fire = g > 40 && g < 100;
    K.left = g > 120 && g < 150;
#elif SMHB_SCENE == 78
    // World 4-4's embers over spikes (raised 2026-10-31): a full running jump at cols 44-45, a standing start at 464-465.
    // 4 embers and full health if it works
    if(g == 3) for(enemy_t& e : enemies) { e.alive = false; e.squish = 1000; }
    if(g == 2) { teleport(41, 13); p.face = 1; }
    if(g == 240) { teleport(462, 14); p.x = 462 * T; p.face = 1; }
    {
        int base = (g < 240 ? 41 : 462) * T;
        K.right = g > 5 && g != 240 && fixed(p.x) < base + 7 * T;
        K.jump = g > 5;
        K.a_pressed = p.ground && g < 240 ? fixed(p.x) >= base + 10 && fixed(p.x) < base + 2 * T : p.ground && g > 241 && fixed(p.x) < base + T;
    }
#elif SMHB_SCENE == 77
    // World 4-3's spikes before the wall (cols 308 and 658, moved left 2026-10-31): take the ember, jump the spikes, land
    // in the gap, jump onto the wall. Health stays full if it works
    if(g == 3) for(enemy_t& e : enemies) { e.alive = false; e.squish = 1000; }
    if(g == 2) { teleport(304, 13); p.face = 1; }
    if(g == 240) { teleport(654, 13); p.face = 1; }
    {
        int base = (g < 240 ? 300 : 650) * T;
        K.right = g > 5 && g != 240 && fixed(p.x) < base + 14 * T;
        K.jump = g > 5;
        K.a_pressed = p.ground && ((fixed(p.x) >= base + 5 * T + 6 && fixed(p.x) < base + 6 * T)
                                   || (fixed(p.x) >= base + 10 * T && fixed(p.x) < base + 13 * T));
    }
#elif SMHB_SCENE == 74
    // World 4-3's way on (col 368): the cracked block at the top of the wall has to be shot from a jump, then he climbs
    // the wall and goes on over the top (26 px wide hitbox, 2026-10-30)
    p.hp = MAX_HP;
    if(g == 2) { teleport(365, 12); p.face = 1; }
    K.right = (g > 5 && g < 30) || (g > 80 && p.x < 380 * T);
    K.jump = (g > 30 && g < 50) || (g > 80 && g < 100) || (g > 130 && g < 150);
    K.a_pressed = g == 31 || g == 81 || g == 131;
    K.fire = g > 30 && g < 70;
#elif SMHB_SCENE == 73
    // Respawn beside a wall (2026-10-30, hitbox 26 px wide): World 2-1's checkpoint at col 151 has a wall next to it; he
    // touches it at g 2, dies at g 60 and comes back clear of the wall
    if(g == 2) teleport(151, 13);
    if(g == 60) die();
    if(! p.dead) p.hp = MAX_HP;
#elif SMHB_SCENE == 72
    // Tall cracked blocks (2026-10-29: 27 px): World 1-1, jumping from col 62 onto the block at col 65 (~1 s), standing on
    // it (~1-3 s, shooting from ~2 s), then walking off the far side (~3.5 s)
    p.hp = MAX_HP;
    if(g == 2) { teleport(62, 14); p.face = 1; }
    K.right = (g > 10 && g < 26) || (g > 200 && g < 215);
    K.jump = g > 10 && g < 30;
    K.a_pressed = g == 11;
    K.fire = g > 120 && g < 200;
#elif SMHB_SCENE == 71
    // World 1-2's start, standing still with the rifle (2026-10-29: the user saw his feet ~10-15 px too low here)
    p.hp = MAX_HP;
#elif SMHB_SCENE == 69 || SMHB_SCENE == 70   // 70: the same with the rocket launcher
    // Prone with the flamethrower (2026-10-29): scene 64's spot in World 1-1 with the flamethrower, lying down facing the
    // block (~1.5 s), flaming (~2.5 s), then turned left still flaming (from ~3.5 s)
    if(g == 2) { teleport(64, 14); if(SMHB_SCENE == 69) p.flame = FLAME_TIME; else p.rocket = ROCKET_TIME; }
    K.right = g > 5 && g < 9;
    K.left = g >= 180 && g < 186;
    K.down = g > 12;
    K.fire = g > 110;
#elif SMHB_SCENE == 68
    // No grenades while down (2026-10-29): World 1-1 prone with R held for ~2 s (still x8), then standing with R held (x8 drops)
    // (at scene 64's spot, where there's room to lie down: col 64 facing the block at col 65)
    if(g == 2) teleport(64, 14);
    K.right = g > 5 && g < 9;
    K.down = g > 12 && g < 150;
    K.grenade = g > 40 && g < 220;
#elif SMHB_SCENE == 67
    // A new game's opening cards (2026-10-29): "WORLD 1" (2.5 s), then World 1's grenade card with x8 (5 s), then World 1-1
    if(g == 2) new_game();
#elif SMHB_SCENE == 66
    // World 1's and 2's grenades (2026-10-29): 8 in World 1-1, two thrown (6 left), carried through 1-2..1-4 (each left
    // as if cleared); a fresh 8 in World 2-1 (after the "WORLD 2" card and World 2's grenade card, ~g 950), one thrown
    // (7 left), carried through 2-2..2-4; a fresh 8 in World 3-1 (after the "WORLD 3" card and World 3's grenade card,
    // ~g 1850), three thrown (5 left), carried through 3-2..3-4, then a fresh 8 in World 4-1 on every difficulty (after
    // "WORLD 4", ~g 2450; Expert also has its grenade card first)
    K.grenade = (g > 10 && g < 50) || (g > 1000 && g < 1020) || (g > 1900 && g < 1980);
    if(g == 200 || g == 300 || g == 400 || g == 500 || g == 1100 || g == 1200 || g == 1300 || g == 1400 ||
       g == 2000 || g == 2100 || g == 2200 || g == 2300) leave_level();
#elif SMHB_SCENE == 62
    // The grenade launcher pose (Expert): World 4-1 at scene 56's spot, firing grenades back to back (the pose shows
    // while grenade_cool runs, so for ~4 s until the 8 are gone)
    if(g == 2) { teleport(431, 10); p.face = -1; }
    K.grenade = g > 10;
#elif SMHB_SCENE == 61
    // The Congratulations screen and its music: World 4-4 cleared straight away (captures from ~3 s show the screen)
    // then on to the stats (via the initials and the table when the score makes the top 5; the music carries on)
    if(g == 2) clear_level();
    K.a_pressed = g == 200 || g == 450;
    K.start_pressed = g == 350;
#elif SMHB_SCENE == 56
    // Grenades (Expert): World 4-1 between the beasts at columns 427 and 436 (the crowded stretch), facing left;
    // one thrown ahead, one aiming up-left, one aiming up, then one at the beasts after walking left. Shot captures show
    // the arcs, the blasts and the HUD count going 8, 7, 6, 5, 4.
    if(g == 2) { teleport(431, 10); p.face = -1; }
    K.up = (g > 70 && g < 110) || (g > 130 && g < 170);
    K.left = (g > 70 && g < 110) || (g > 200 && g < 230);
    K.grenade = g == 20 || g == 90 || g == 150 || g == 240 || g == 300;
#elif SMHB_SCENE == 58
    // Time Attack: the title menu's second item, RIGHT to World 1-2 and back, A to play World 1-1; teleported next to the
    // door, a run in; the clear screen, A to the initials, START to keep them, the table (new row blinking)
    if(g == 20) K.down_pressed = true;
    if(g == 40) K.a_pressed = true;
    if(g == 100) K.right_pressed = true;
    if(g == 160) K.left_pressed = true;
    if(g == 220) K.a_pressed = true;
    if(g < 600 && state == state_t::play && lvl.time == 2) teleport(431, 12);
    K.right = g < 600 && state == state_t::play && lvl.time > 2 && p.x + p.w / 2 < door.x + 8;
    if(state == state_t::clear && g % 60 == 0) K.a_pressed = true;
    if(state == state_t::initials && g % 60 == 30) K.start_pressed = true;
    // then World 1-1 again (A at g 700), paused (START at 760), quit with B (at 880) back to the table
    if(g == 700) K.a_pressed = true;
    if(g == 760) K.start_pressed = true;
    if(g == 880) K.b_pressed = true;
#elif SMHB_SCENE == 57
    // The Expert grenade card: World 3-4 left as if cleared, so the "WORLD 4" card then the grenade card, then World 4-1
    if(g == 2) leave_level();
#elif SMHB_SCENE == 55
    // Restarting a level (2026-10-24): 95 coins toward a 1-up carried in, run right until the coins buy a life, then
    // restart; the counter must read 0 and the life bought here must be gone
    static int restart_at = -1;
    if(g == 2 && restart_at < 0) { teleport(42, 14); run.life_coins = 95; for(enemy_t& e : enemies) { e.alive = false; e.squish = 1000; } }
    if(restart_at < 0 && lvl.coin_lives > 0) restart_at = g + 60;
    if(g == restart_at) { replay_level(); restart_at = 1 << 30; }
    K.right = g > 10 && restart_at < 0;
    K.jump = K.right && (g % 55) < 22;
    K.a_pressed = K.right && (g % 55) == 1;
    p.hp = MAX_HP;
#elif SMHB_SCENE == 2
    if(g == 2) { teleport(37, 8); p.face = -1; }
    static int got_at = -1;
    if(got_at < 0 && run.lives > MAX_LIVES) got_at = g;
    K.left = g == 5 || (g > 120 && (got_at < 0 || g < got_at + 15));
    K.fire = g > 10 && g < 40;
#elif SMHB_SCENE == 23 || SMHB_SCENE == 24 || SMHB_SCENE == 49 || SMHB_SCENE == 53 || SMHB_SCENE == 54 || SMHB_SCENE == 79   // 49: World 4-1, 53/54: World 2-3/3-4 (none on Expert), 24/79: World 4-4/4-3 (none on Hard alone)
    // (Expert's own block in World 4-1 is just past a pit: shot from 4 columns right of it instead, facing left)
    const bool from_right = difficulty == 2 && LV().expert_star.x >= 0;
    if(g == 2)
    {
        const point s = from_right ? LV().expert_star : LV().star_block;
        teleport(from_right ? s.x + 4 : s.x - 4, s.y + 1); p.face = from_right ? -1 : 1;
    }
    // (then left: World 1-1's star block is near a ledge's end, with an alien patrolling behind Mario)
    static int got_at = -1;
    if(got_at < 0 && p.star > 0) got_at = g;
    K.fire = g > 10 && g < 30;
    K.right = from_right ? false : g > 60 && got_at < 0;
    K.left = from_right ? g > 60 && got_at < 0 : got_at >= 0 && g > got_at + 20 && g < got_at + 80;   // into the alien, short of the pit, then waits it out
    K.jump = got_at >= 0 && g < got_at + 80 && (g % 80) < 20;
    K.a_pressed = got_at >= 0 && (g % 80) == 1;
#elif SMHB_SCENE >= 25 && SMHB_SCENE <= 27
    if(g == 2) { teleport(LV().flame_item.x, LV().flame_item.y + 1); p.face = 1; }   // on it (some stand on short planks)
    K.right = g > 10 && p.flame == 0 && g < 120;   // onto the pickup, then stands still
    K.fire = (p.flame > 0 && (g % 200) < 150) || g > 1000;
    p.hp = MAX_HP;
#elif SMHB_SCENE >= 28 && SMHB_SCENE <= 31
    if(g == 2) { teleport(LV().rocket_item.x, LV().rocket_item.y + 1); p.face = 1; }
    if(SMHB_SCENE == 28 && g == 400) { teleport(242, 12); p.face = 1; }   // then in front of the wall at col 248, to see rockets explode
    if(SMHB_SCENE == 28 && g == 700) { teleport(306, 12); p.face = 1; }   // then before the spikes at cols 311-312, row 11, to blow them away
    K.right = g > 10 && p.rocket == 0 && g < 120;
    K.fire = (p.rocket > 0 && (g % 200) < 150) || g > 1300;
    p.hp = MAX_HP;
#elif SMHB_SCENE == 39 || SMHB_SCENE == 40
    // Weapon back after a death (2026-10-21): World 1-3's flamethrower (39) / World 1-2's rocket launcher (40) is taken,
    // Mario dies at g 100 and respawns, then stands 3 cols left of the spot (pickup visible) and walks onto it again at g 330
    const auto& at = SMHB_SCENE == 39 ? LV().flame_item : LV().rocket_item;
    if(g == 2) { teleport(at.x, at.y + 1); p.face = 1; }
    if(g == 100) die();
    if(g == 250) { teleport(at.x - 3, at.y + 1); p.face = 1; }
    K.right = (g > 10 && g < 60) || (g > 330 && g < 400 && p.flame == 0 && p.rocket == 0);
    K.fire = g > 330;
    if(! p.dead) p.hp = MAX_HP;
#elif SMHB_SCENE == 59
    // World 2-3's 1-up block (moved off the plank onto the lawn at col 128, 2026-10-25): Mario 3 cols right of it shoots it,
    // then takes the 1-up (a reaper patrols there; HP is kept full)
    if(g == 2) { teleport(131, 11); p.face = -1; }
    K.fire = g > 40 && g < 160;
    K.left = g > 160 && g < 220 && p.x > 128 * T;
    p.hp = MAX_HP;
#elif SMHB_SCENE == 38
    // World 4-2's two 1-up blocks (moved onto the ground at cols 215 and 566, 2026-10-20): Mario lands on the ground 3 cols
    // right of each (beasts patrol there; HP is kept full) and shoots it, then takes the 1-up
    if(g == 2) { teleport(218, 14); p.face = -1; }
    if(g == 300) { teleport(569, 14); p.face = -1; }
    K.fire = (g > 40 && g < 160) || (g > 340 && g < 460);
    K.left = ((g > 160 && g < 200) || (g > 460 && g < 500)) && p.x > (g < 300 ? 215 : 566) * T;   // not off the ground's left end
    p.hp = MAX_HP;
#elif SMHB_SCENE == 37
    // World 1-4's 1-up block (moved to col 139 on the grass strip, 2026-10-20): Mario at col 135 shoots it, then takes the 1-up
    if(g == 2) { teleport(135, 10); p.face = 1; }
    K.fire = g > 60 && g < 200;
    K.right = g > 200 && g < 260;
    p.hp = MAX_HP;
#elif SMHB_SCENE == 36
    // Stomping flying demons (2026-10-20): World 2-1, Mario dropped 40 px above each of the first three demons in turn
    static int n = 0;
    if(g == 2 || g == 180 || g == 360)
    {
        int k = 0;
        for(enemy_t& e : enemies)
            if(e.kind == FOE_DEMON && k++ == n)
            {
                teleport(0, 0); p.x = e.x + e.w / 2 - p.w / 2; p.y = e.y - 40;
                cam_x = clamp_cam_x(p.x - SCREEN_W / 2); cam_y = clamp_cam_y(p.y + p.h / 2 - SCREEN_H / 2);
                break;
            }
        ++n; p.face = 1;
    }
    p.hp = MAX_HP;
#elif SMHB_SCENE == 34 || SMHB_SCENE == 35
    // The flying demons (2026-10-20): World 2-1 (34) or 4-1 (35), on the floor 4 rows under the first one (levels.js keeps
    // them over floor); watch it fly at Mario, then shoot it straight up
    static int col = 0;
    if(g == 2)
    {
        for(int i = 0; i < LV().enemies_n && ! col; ++i)
            if(LV().enemies[i].kind == FOE_DEMON) { col = LV().enemies[i].a / T + 2; teleport(col, LV().enemies[i].y / T + 4); }
        p.face = 1;
    }
    K.up = g > 240;
    K.fire = g > 240;
    if(p.ground) { p.vx = 0; p.x = col * T + 1; }   // stays put, aiming up
    p.hp = MAX_HP;
#elif SMHB_SCENE == 32 || SMHB_SCENE == 33
    // Aiming up (2026-10-20): the gun at the start, then onto the rocket launcher (32, World 1-2) or the flamethrower
    // (33, World 1-3) and firing it up, standing and jumping
    if(g == 200) { if(SMHB_SCENE == 32) teleport(LV().rocket_item.x, LV().rocket_item.y + 1); else teleport(LV().flame_item.x, LV().flame_item.y + 1); p.face = 1; }
    K.up = g > 10;
    K.fire = g > 10 && (g % 120) < 90;
    K.right = (g > 200 && g < 260 && p.rocket == 0 && p.flame == 0) || (g >= 700 && (g / 20) % 2 == 0);   // then 45 degrees, both ways
    K.left = g >= 700 && (g / 20) % 2 == 1;
    K.jump = g > 500 && (g % 90) < 20;
    K.a_pressed = g > 500 && (g % 90) == 1;
    if(p.ground && g > 260) p.vx = 0;
    p.hp = MAX_HP;
#elif SMHB_SCENE == 3 || SMHB_SCENE == 4 || (SMHB_SCENE >= 10 && SMHB_SCENE <= 22)
    // Walk right, stopping to shoot whenever something is in range; jump now and then
    K.fire = true;
    K.right = (g % 120) < 70;
    K.jump = (g % 90) < 18;
    K.a_pressed = (g % 90) == 1;
    if(g % 300 == 0) p.hp = MAX_HP;   // keep the test run alive long enough to see things
#elif SMHB_SCENE == 6
    // Sound stress test: a coin and a jump sound every frame, with the gun firing, on the Graveyard
    K.fire = true;
    if(state == state_t::play) { sfx::coin(); sfx::jump(); }
    p.hp = MAX_HP;
#elif SMHB_SCENE == 7
    // The GBA-only Jungle aliens: one stop every 180 frames (captures at about 5, 8, 11 and 14 s)
    if(g == 2) teleport(53, 14);     // the long floor after the spike pits (new alien at 59-64)
    if(g == 182) teleport(152, 8);   // the high ledge at the top of the elevator (157-161)
    if(g == 362) teleport(170, 14);  // the floor under the plank (174-178)
    if(g == 542) teleport(206, 12);  // before the exit door (213-215)
    p.hp = MAX_HP;
#elif SMHB_SCENE == 8
    // The GBA-only Graveyard reapers: one stop every 180 frames (captures at about 5, 8 and 11 s), close enough that
    // each rises out of its grave
    if(g == 2) teleport(52, 10);     // the first plank of Part 1's hops (new reaper on the 61-63 plank)
    if(g == 182) teleport(126, 12);  // the foot of the raised step (127-130)
    if(g == 362) teleport(168, 8);   // the plank before (172-174)
    p.hp = MAX_HP;
#elif SMHB_SCENE == 9
    // The game-over screen, straight away, on the Graveyard
    if(g == 30 && state == state_t::play) game_over();
#elif SMHB_SCENE == 41
    // High scores: game over with a made-up score, START to the initials, type "B.B" (from a new save's "AAA"), the
    // table, back to the title, then SELECT for the table from the title and B back
    if(g == 30 && state == state_t::play) { run.score = 12345 + rng.get_int(1000); game_over(); }
    if(g == 90) K.start_pressed = true;
    K.up = g >= 120 && g < 121;                       // A -> B
    if(g == 140) K.a_pressed = true;
    K.down = g >= 160 && g < 161;                     // A -> the space (the letters wrap round)
    K.down = K.down || (g >= 180 && g < 201);         // held 21 frames: a step, then repeats from 16 frames -> "-", "."
    if(g == 220) K.a_pressed = true;
    K.up = K.up || (g >= 240 && g < 241);
    if(g == 270) K.a_pressed = true;                  // the third letter: done
    if(g == 420) K.a_pressed = true;                  // table -> title
    if(g == 480) K.select_pressed = true;             // title -> table
    if(g == 600) K.b_pressed = true;                  // and back
#elif SMHB_SCENE == 42
    // Select Difficulty: START on the title, DOWN twice to Expert, A to start; then game over with a made-up score, START
    // to the initials, START to keep them, the Expert table, RIGHT to the Normal one (it wraps), A back to the title
    if(g == 30) K.start_pressed = true;
    if(g == 90 || g == 120) K.down_pressed = true;
    if(g == 150) K.a_pressed = true;
    if(g == 190 || g == 230) K.a_pressed = true;   // (skips the "WORLD 1" and grenade cards, 2026-10-29)
    if(g == 240 && state == state_t::play) { run.score = 23456; game_over(); }
    if(g == 300) K.start_pressed = true;
    if(g == 360) K.start_pressed = true;
    if(g == 480) K.right_pressed = true;
    if(g == 600) K.a_pressed = true;
#elif SMHB_SCENE == 43
    // World 2-1 beside one of Hard's extra reapers (columns 102-104, row 11): on Normal that floor is empty
    if(g == 2) teleport(97, 12);
    p.hp = MAX_HP;
#elif SMHB_SCENE == 44
    // World 3-1 beside a mech unit that Hard splits in two (columns 90-95, row 13): one mech on Normal, two on Hard
    if(g == 2) teleport(85, 14);
    p.hp = MAX_HP;
#elif SMHB_SCENE == 45
    // World 1-1 beside Hard's blue mushroom (column 118, row 8): nothing there on Normal
    if(g == 2) teleport(114, 9);
    p.hp = MAX_HP;
#elif SMHB_SCENE == 51
    // World 1-1 beside the alien patrol at columns 101-105 (row 11): one alien on Normal and Hard, on Expert a guard at
    // 101-102 that stands still facing Mario and an alien walking 103-105
    if(g == 2) teleport(96, 12);
    p.hp = MAX_HP;
#elif SMHB_SCENE == 52
    // World 2-1 on the plank at columns 207-209 (row 10), between two of Expert's new reapers (planks 202-204 and
    // 212-214, row 12): both empty on Normal and Hard
    if(g == 2) teleport(208, 10);
    p.hp = MAX_HP;
#elif SMHB_SCENE == 60
    // Hard (code Expert) locked (2026-10-26): Start Game, DOWN twice (Easy -> Normal -> skips Hard -> Easy); then unlocked
    // as if the game had been beaten on Normal, DOWN twice more lands on Hard
    if(g == 2) expert_unlocked = 0;
    if(g == 20) K.a_pressed = true;
    if(g == 80 || g == 140) K.down_pressed = true;
    if(g == 200) { unlock_expert(); draw_difficulty(); }
    if(g == 260 || g == 320) K.down_pressed = true;
#elif SMHB_SCENE == 47
    // The title menu: DOWN to Controls, A (captures about 1 s later), B back, DOWN to High Scores, A, B back, then DOWN
    // to Credits and A
    if(g == 30 || g == 40) K.down_pressed = true;   // (past Time Attack, second since 2026-10-25)
    if(g == 60) K.a_pressed = true;
    if(g == 180) K.b_pressed = true;
    if(g == 210) K.down_pressed = true;
    if(g == 240) K.a_pressed = true;
    if(g == 360) K.b_pressed = true;
    if(g == 390) K.down_pressed = true;
    if(g == 420) K.a_pressed = true;
#elif SMHB_SCENE == 5 || SMHB_SCENE == 46 || SMHB_SCENE == 48   // 46: the same in World 1-4 (the Alien Wasp; on Hard its blue mushroom); 48: World 2-4 (the Eye Demon)
    // From just before the boss checkpoint (6 tiles before the arena's gate), well into the arena
    if(g == 2) teleport(LV().boss_gate - 6, 14);
    K.right = g > 10 && p.x < (LV().boss_gate + 8) * T;
    K.fire = p.x >= (LV().boss_gate + 8) * T;
    K.jump = g > 200 && (g % 70) < 20;
    K.a_pressed = g > 200 && (g % 70) == 1;
    p.hp = MAX_HP; p.inv = bn::max(p.inv, 2);   // cheat: no damage, so the whole fight can be watched
    // Onto the rocket launcher once it has landed (it drops at a fifth of the skull's health; it may be on a high plank)
    if(rocket_item.live && ! rocket_item.fall && boss_active()) teleport(tdiv(rocket_item.x), tdiv(rocket_item.y + 9));
    // Once the skull is dead: walk to the door, then wait on the Congratulations screen before continuing to the stats
    if(! boss || boss->state == boss_t::dead) { K.right = state == state_t::play; K.fire = false; K.jump = false; K.a_pressed = false; }
    static int end_t = 0;
    if(state == state_t::congrats || state == state_t::win || state == state_t::initials || state == state_t::scores) ++end_t;
    if(state == state_t::congrats && end_t == 360) K.a_pressed = true;
    if(state == state_t::initials && end_t == 400) K.start_pressed = true;   // keep the last initials (a high score on a new save)
    if(state == state_t::scores && end_t == 440) K.a_pressed = true;
    if(state == state_t::win && end_t == 480) K.a_pressed = true;   // then "play again" from the stats, back to the title
    if(end_t >= 480 && (state == state_t::title || state == state_t::difficulty)) ++end_t;
    if(state == state_t::title && end_t == 560) K.a_pressed = true;   // and a new game from there
    if(state == state_t::difficulty && end_t == 600) K.a_pressed = true;
#endif
#else
    K.left = bn::keypad::left_held();
    K.right = bn::keypad::right_held();
    K.down = bn::keypad::down_held();
    K.up = bn::keypad::up_held();
    K.fire = bn::keypad::b_held();
    K.grenade = bn::keypad::r_pressed();
    K.jump = bn::keypad::a_held();
    K.a_pressed = bn::keypad::a_pressed();
    K.start_pressed = bn::keypad::start_pressed();
    K.select_pressed = bn::keypad::select_pressed();
    K.b_pressed = bn::keypad::b_pressed();
    K.left_pressed = bn::keypad::left_pressed();
    K.right_pressed = bn::keypad::right_pressed();
    K.up_pressed = bn::keypad::up_pressed();
    K.down_pressed = bn::keypad::down_pressed();
#endif
    if(K.a_pressed) jump_buf = 8;
}

void tick()
{
    if(state == state_t::pause) return;
    update_world();
    if(state == state_t::play)
    {
        update_player();
        update_bullets();
        update_grenades();
        if(state == state_t::play) update_foes();
        if(state == state_t::play) update_boss();
        // During the boss fight the camera stays inside the arena. The arena is one web-game screen wide, which is two GBA
        // screens, so instead of locking to it the camera frames a point between Mario and the skull (weighted to Mario).
        fixed target_x = p.x + p.w / 2 - SCREEN_W / 2 + p.face * 24;
        if(boss_active())
        {
            target_x = (p.x + p.w / 2) * fixed(0.6) + boss->x * fixed(0.4) - SCREEN_W / 2;
            target_x = bn::max(fixed(boss->gate * T - 8), target_x);
        }
        cam_x = clamp_cam_x(cam_x + (target_x - cam_x) * fixed(0.1));
        fixed target_y = boss_active() ? fixed(72) : p.y + p.h / 2 - SCREEN_H / 2 - 8;
        cam_y = clamp_cam_y(cam_y + (target_y - cam_y) * fixed(0.15));
    }
    else if(state == state_t::congrats)
    {
        // Ember fireworks bursting across the screen behind the Congratulations text
        if(rng.get_int(100) < 6)
        {
            fixed x = cam_x + 30 + rng.get_int(SCREEN_W - 60), y = cam_y + 20 + rng.get_int(80);
            BURST(x, y, 18, fixed(2.4), fixed(0.04), 60, P_GOLD, P_EMBER, P_FLAME, P_PALE, P_GREEN);
            sfx::pop();
        }
    }
    else if(title_screens())
    {
        ++title_t;
        // The title screen slowly pans across the level
        // The angle in whole degrees plus a fraction: title_t * 360 would overflow fixed (max 524287) after 24 s
        const int a = title_t % 4398 * 360;
        cam_x = (1 - cos_deg(fixed(a / 4398) + fixed(a % 4398) / 4398)) / 2 * max_cam_x();
        cam_y = max_cam_y();
    }
}

}

// The startup screens, before the title: a full-screen picture fades in from black, holds and fades out. A or START
// skips ahead to the fade-out (from wherever the fade-in had got to).
// hold < 0 waits for START instead (the title picture, at the user's request), with a blinking "PRESS START" once the
// picture is fully in: sprite text doesn't take the background fade, so it's hidden while fading.
static void show_splash(const bn::regular_bg_item& item, int hold)
{
    constexpr int fade_in = 30, fade_out = 30;
    const bool wait = hold < 0;
    const bn::color black(0, 0, 0);
    bn::bg_palettes::set_transparent_color(black);
    bn::regular_bg_ptr picture = item.create_bg(0, 0);
    bn::vector<bn::sprite_ptr, 8> prompt;
    if(wait)
    {
        text16->set_center_alignment();
        text16->set_bg_priority(0);
        // Below the hero's shoulders on the title picture (user, 2026-10-30; it was at y 30, and y 66 before that)
        text16->generate(0, 50, "PRESS START", prompt);
    }
    int t = 0, out = -1;   // out: frames into the fade-out, once it has started
    while(out < fade_out)
    {
        bool skip = wait ? bn::keypad::start_pressed() : bn::keypad::a_pressed() || bn::keypad::start_pressed();
        if(out < 0 && (skip || (! wait && t >= fade_in + hold))) out = t < fade_in ? fade_out * (fade_in - t) / fade_in : 0;
        fixed dark = out >= 0 ? fixed(out) / fade_out : t < fade_in ? fixed(fade_in - t) / fade_in : fixed(0);
        bn::bg_palettes::set_fade(black, dark);
        bool shown = out < 0 && t >= fade_in && (t - fade_in) % 60 < 40;   // on for two thirds of each second
        for(bn::sprite_ptr& s : prompt) s.set_visible(shown);
        bn::core::update();
        ++t;
        if(out >= 0) ++out;
    }
    bn::bg_palettes::set_fade(black, 0);
}

int main()
{
    bn::core::init();
#ifdef SMHB_AUTOPLAY
    mmSetVBlankHandler(mix_probe);
#endif
    // Direct Sound (the mixer's 8-bit output: music and samples) at 50% instead of Maxmod's 100%, with everything mixed at
    // double level to match: same loudness, but each 8-bit step is half as big, so its quantisation noise ("static" in the
    // quiet Graveyard track) is 6 dB lower. Bits 2 and 3 of SOUNDCNT_H; the DMG tones aren't affected.
    *reinterpret_cast<volatile uint16_t*>(0x4000082) &= uint16_t(~0x000C);
    dmg_sfx::init();

    camera = bn::camera_ptr::create(0, 0);
    text8.emplace(common::variable_8x8_sprite_font);
    text16.emplace(common::variable_8x16_sprite_font);

#ifndef SMHB_AUTOPLAY
    // Not in the self-playing test builds, whose scripts and capture times assume the title comes first
    show_splash(bn::regular_bg_items::dw_logo, 120);      // the Dreamweaver Studios logo, two seconds
    sync_music();                                         // the title music starts with the title picture
    show_splash(bn::regular_bg_items::title_art, -1);     // the Hellbound title picture, until START
#endif
    load_level(title_level);
    show_title();

    while(true)
    {
        ++frame;
        read_keys();

        if(state == state_t::title)
        {
            update_title();
        }
        else if(state == state_t::controls || state == state_t::credits)
        {
            if(K.b_pressed || K.a_pressed || K.start_pressed) show_title(true);
        }
        else if(state == state_t::difficulty)
        {
            if(K.start_pressed || K.a_pressed) { time_attack = false; new_game(SMHB_START_LEVEL); jump_buf = 0; }
            else update_difficulty();
        }
        else if(state == state_t::initials) update_initials();
        else if(state == state_t::ta_select) { update_ta_select(); jump_buf = 0; }
        else if(state == state_t::scores) { update_scores(); jump_buf = 0; }
        else if(state == state_t::play)
        {
            if(K.start_pressed)
            {
                state = state_t::pause; set_dim(true);
                // "Level 2 - Graveyard", but just "World 1-1" for a name that already says where it is
                bn::string_view name = LV().name;
                bn::string<32> l;
                if(name.size() < 6 || name.substr(0, 6) != "World ") { l = "Level "; l += bn::to_string<4>(li + 1); l += " - "; }
                l += name;
                overlay_line(-34, l);
                overlay_line(-20, "PAUSED", true);
                overlay_line(8, "START: resume     SELECT: restart level");
                if(time_attack) overlay_line(20, "B: quit to level select");
            }
        }
        else if(state == state_t::pause)
        {
            if(K.start_pressed) { state = state_t::play; clear_overlay(); set_dim(false); }
            else if(K.select_pressed) { state = state_t::play; replay_level(); }
            else if(time_attack && K.b_pressed) show_ta_select(-1);
        }
        else if(state == state_t::clear)
        {
            if(K.a_pressed) { if(time_attack) ta_continue(); else leave_level(); jump_buf = 0; }
            else if(K.select_pressed) { replay_level(); jump_buf = 0; }
        }
        else if(state == state_t::world_card)
        {
            ++world_card_t;
            if(world_card_t >= world_card_frames || (world_card_t >= WORLD_CARD_SKIP && (K.a_pressed || K.start_pressed)))
            {
                end_world_card(); jump_buf = 0;
            }
        }
        else if(state == state_t::congrats)
        {
            if(K.a_pressed || K.start_pressed) end_run(true);
        }
        else if(state == state_t::win)
        {
            if(K.a_pressed || K.start_pressed) { show_title(); jump_buf = 0; }
        }
        else if(state == state_t::over)
        {
            if(K.a_pressed) { retry_level(); jump_buf = 0; }
            else if(K.start_pressed) end_run(false);
        }

        tick();
        sync_music();
        flame_audio(state == state_t::play && ! p.dead && p.flame > 0 && K.fire && p.throw_t == 0);
        draw();
        dmg_sfx::update();
        bn::core::update();
#ifdef SMHB_AUTOPLAY
        {
            static int worst = 0, sum = 0;
            int cpu = (bn::core::last_cpu_usage() * 100).round_integer();
            worst = bn::max(worst, cpu); sum += cpu;
            if(state == state_t::play && frame > 160) { peak_all = bn::max(peak_all, cpu); if(cpu > 100) ++overruns; }
            measure_output();
            if(frame % 30 == 0) { peak_cpu = worst; avg_cpu = sum / 30; worst = 0; sum = 0; hud_cpu.value = -1; }
        }
#endif
    }
}
