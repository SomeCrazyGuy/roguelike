#ifndef C_TYPES_H_
#define C_TYPES_H_
C_TYPES_H_//clang

#include "list.h"

//true false flags
enum {
	FALSE,
	TRUE,
};

//directions
enum {
	D_NORTH = 1,
	D_SOUTH = 2,
	D_EAST = 4,
	D_WEST = 8,
};

//exits
enum {
	E_N = D_NORTH,
	E_S = D_SOUTH,
	E_E = D_EAST,
	E_W = D_WEST,
	E_NS = E_N | E_S,
	E_NE = E_N | E_E,
	E_NW = E_N | E_W,
	E_SE = E_S | E_E,
	E_SW = E_S | E_W,
	E_EW = E_E | E_W,
	E_NSE = E_N | E_S | E_E,
	E_NSW = E_N | E_S | E_W,
	E_NEW = E_N | E_E | E_W,
	E_SEW = E_S | E_E | E_W,
	E_NSEW = E_N | E_S | E_E | E_W,
	E_NUM
};

//limits
enum {
	MAX_NAME_LENGTH = 32,
};

//alignment types
enum {
	TR_LG,
	TR_LN,
	TR_LE,
	TR_NG,
	TR_NN,
	TR_NE,
	TR_CG,
	TR_CN,
	TR_CE,
	TR_NUM,
};

/*
 *    S T R U C T U R E S
 */

typedef struct {
	unsigned chest: 1; //room has chest
	unsigned event: 1; //room triggers random event (currently unused)
	unsigned boss_room: 1; //room has boss
	unsigned visited: 1; //room was visited
	unsigned sky_view: 1; //can view the sky from here
	unsigned trait: 4; //room traits
	unsigned cd: 2; //is the room cold, dark, both, or neither
	unsigned exits: 4; //room exits
	unsigned __padding__: 17; //padding
} room;

typedef struct {
	unsigned type: 2; // weapon, armor, or food
	unsigned subtype: 3;
	unsigned val: 3; //value / rarity
	unsigned trait: 4; //alignment
	unsigned __padding__: 20;
} item;

typedef struct {
	const char* name;
	list* inv;
	item weapon;
	item armor;
	int hp;
	unsigned char trait;
	unsigned char level;
	unsigned short xp;
} npc;

typedef struct {
	room* rooms;
	room* cur_room;
	list* room_chest;
	npc* room_npc;
	npc player;
	int location;
	int width;
	int height;
	int spawn_room;
	int end_room;
	int player_buff;
	int npc_buff;
	int ticks;
	int update;
	int __padding__;
} dungeon;

#endif //C_TYPES_H_
