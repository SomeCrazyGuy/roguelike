#ifndef C_NPC_H_
#define C_NPC_H_

#include "types.h"
#include "util.h"

//npc names
enum {
	NAME_MAX = 40,
};
static const char* const npc_names[] = {
	[0] = "Anlow",
	[1] = "Arando",
	[2] = "Bram",
	[3] = "Cale",
	[4] = "Dalkon",
	[5] = "Daylen",
	[6] = "Dodd",
	[7] = "Dungarth",
	[8] = "Dyrk",
	[9] = "Eandro",
	[10] = "Falken",
	[11] = "Feck",
	[12] = "Fenton",
	[13] = "Gryphero",
	[14] = "Hagar",
	[15] = "Jeras",
	[16] = "Krynt",
	[17] = "Lavant",
	[18] = "Leyten",
	[19] = "Madian",
	[20] = "Malfier",
	[21] = "Markus",
	[22] = "Meklan",
	[23] = "Namen",
	[24] = "Navaren",
	[25] = "Nerle",
	[26] = "Nilus",
	[27] = "Ningyan",
	[28] = "Norris",
	[29] = "Quentin",
	[30] = "Semil",
	[31] = "Sevenson",
	[32] = "Steveren",
	[33] = "Talfen",
	[34] = "Tamond",
	[35] = "Taran",
	[36] = "Tavon",
	[37] = "Tegan",
	[38] = "Vanan",
	[39] = "Vincent",
};

npc* npc_create(int level);
void npc_destroy(npc* n);

npc* npc_create(int level) {
	npc* ret = calloc(1, sizeof(npc));
	ret->name = npc_names[get_rand(NAME_MAX)];
	ret->hp = 10 + (get_rand(level * 10));
	ret->xp = 0;
	ret->trait = (unsigned char) get_rand(TR_NUM);
	ret->level = (unsigned char) level;
	ret->inv = NULL;
	return ret;
}

void npc_destroy(npc* n) {
	if(n) {
		LIST_FREE(n->inv);
		free(n);
	}
}
#endif //C_NPC_H_
