#ifndef GAMECONSTANTS_H_
#define GAMECONSTANTS_H_

#include <random>
#include <utility>

// image IDs for the game objects

const int IID_GHOST_RACER = 0;
const int IID_YELLOW_BORDER_LINE = 1;
const int IID_WHITE_BORDER_LINE = 2;
const int IID_OIL_SLICK = 3;
const int IID_HUMAN_PED = 4;
const int IID_ZOMBIE_PED = 5;
const int IID_ZOMBIE_CAB = 6;
const int IID_HOLY_WATER_PROJECTILE = 7;
const int IID_HEAL_GOODIE = 8;
const int IID_SOUL_GOODIE = 9;
const int IID_HOLY_WATER_GOODIE = 10;

// sounds

const int SOUND_PLAYER_DIE = 0;
const int SOUND_PED_DIE = 1;
const int SOUND_PED_HURT = 2;
const int SOUND_PLAYER_SPRAY = 3;
const int SOUND_OIL_SLICK = 4;
const int SOUND_FINISHED_LEVEL = 5;
const int SOUND_VEHICLE_DIE = 6;
const int SOUND_VEHICLE_HURT = 7;
const int SOUND_VEHICLE_CRASH = 8;
const int SOUND_THEME = 9;
const int SOUND_GOT_GOODIE = 10;
const int SOUND_GOT_SOUL = 11;
const int SOUND_ZOMBIE_ATTACK = 12;
const int SOUND_NONE = -1;

// keys the user can hit

const int KEY_PRESS_LEFT   = 1000;
const int KEY_PRESS_RIGHT  = 1001;
const int KEY_PRESS_UP     = 1002;
const int KEY_PRESS_DOWN   = 1003;
const int KEY_PRESS_SPACE  = ' ';
const int KEY_PRESS_ESCAPE = '\x1b';
const int KEY_PRESS_TAB    = '\t';
const int KEY_PRESS_ENTER  = '\r';

// board and sprite dimensions

const int VIEW_WIDTH = 256;
const int VIEW_HEIGHT = 256;

const int SPRITE_WIDTH = 8;
const int SPRITE_HEIGHT = 8;

const double SPRITE_WIDTH_GL = .3; // note - this is tied implicitly to SPRITE_WIDTH due to carey's sloppy openGL programming
const double SPRITE_HEIGHT_GL = .25; // note - this is tied implicitly to SPRITE_HEIGHT due to carey's sloppy openGL programming

// Ghost Racer specific constants

const int ROAD_WIDTH = 150;
const int ROAD_CENTER = VIEW_WIDTH / 2;

// status of each tick (did the player die?)

const int GWSTATUS_PLAYER_DIED    = 0;
const int GWSTATUS_CONTINUE_GAME  = 1;
const int GWSTATUS_PLAYER_WON     = 2;
const int GWSTATUS_FINISHED_LEVEL = 3;
const int GWSTATUS_LEVEL_ERROR    = 4;

// test parameter constants

const int NUM_TEST_PARAMS = 1;

// Return a uniformly distributed random int from min to max, inclusive

inline
int randInt(int min, int max)
{
    if (max < min)
        std::swap(max, min);
    static std::random_device rd;
    static std::default_random_engine generator(rd());
    std::uniform_int_distribution<> distro(min, max);
    return distro(generator);
}

#endif // GAMECONSTANTS_H_
