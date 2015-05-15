#include "dungeon.h"
#include "debug.h"

#define IF(X,Y) ((X))?(Y):""

//player inventory actions
enum {
	A_INVENTORY, //view items
	A_PUTON, //select armor items
	A_ARM, //select weapon items
	A_CONSUME, //select food items
	A_LOOT, //select multiple / all items
	A_SELECT, //select any one item
	A_EXAMINE, //closely examine an item
};

static const char * const action_prompt[] = {
	[A_INVENTORY] = "",
	[A_SELECT] = "Select an item.",
	[A_PUTON] = "Which armor should I wear?",
	[A_ARM] = "Which weapon should I arm myself with?",
	[A_CONSUME] = "Which food should I eat?",
	[A_LOOT] = "Which item do I want from this chest?",
};

static const char* const adj_rarity[] = {
	[R_WORTHLESS] = " worthless",
	[R_VERYCOMMON] = " very common",
	[R_COMMON] = " common",
	[R_UNCOMMON] = " uncommon",
	[R_STRANGE] = " strange",
	[R_RARE] = " rare",
	[R_VERYRARE] = " very rare",
	[R_LEGENDARY] = " legendary",
};

static const char* const noun_type[] = {
	[T_WEAP] = " weapon",
	[T_ARMOR] = " armor",
	[T_FOOD] = " food",
};

static const char * const * const subtype_desc[] = {
	[T_WEAP] = (const char* const[]) {
			[TW_LIGHT] = " light",
			[TW_NORMAL] = " normal",
			[TW_HEAVY] = " heavy",
			[TW_VERYHEAVY] = " very heavy",
		},
	[T_ARMOR] = (const char* const[]) {
			[TA_LEATHER] = " leather",
			[TA_CHAINMAIL] = " chainmail",
			[TA_PLATEMAIL] = " platemail",
			[TA_CRYSTAL] = " crystal",
		},
	[T_FOOD] = (const char* const[]) {
			[TF_STALE] = " stale",
			[TF_OLD] = " old",
			[TF_PRESERVED] = " preserved",
			[TF_FRESH] = " fresh",
		},
};

static const char* const adj_trait[] = {
	[TR_LG] = " lawful good",
	[TR_LN] = " lawful neutral",
	[TR_LE] = " lawful evil",
	[TR_NG] = " neutral good",
	[TR_NN] = " true neutral",
	[TR_NE] = " neutral evil",
	[TR_CG] = " chaotic good",
	[TR_CN] = " chaotic neutral",
	[TR_CE] = " chaotic evil",
};

static const char* const * const room_desc[] = {
	[RD_COLD_DARK] = (const char* const[]) {
		[CD_N] = "", //neither cold nor dark
		[CD_C] = " I feel cold in this room.", //room is cold
		[CD_D] = " I can't see well in this room.", //room is dark
		[CD_B] = " I feel cold and cannot see very well.", //cold + dark
	},
	[RD_TRAIT] = (const char* const[]) {
		[TR_LG] = " Those that are lawful and good are feeling empowered.",
		[TR_LN] = " Keeping level headed and lawful will be rewarded.",
		[TR_LE] = " Darkness shall rule... and it demands respect and obedience.",
		[TR_NG] = " Being guided by a selfless belief has karmic rewards.",
		[TR_NN] = " There is a balance to everything, believe that and prosper.",
		[TR_NE] = " Do what is convenient to get what you want.",
		[TR_CG] = " There are no laws, only what is right.",
		[TR_CN] = " Those that know what they want shall recieve it.",
		[TR_CE] = " Watch others suffer... and be rejuvenated."
	},
	[RD_EXITS] = (const char* const[]) {
		[0] = " I appear to have found myself in a room without exits.",
		[E_N] = " I see one exit to the North.",
		[E_S] = " I see one exit to the South.",
		[E_E] = " I see one exit to the East.",
		[E_W] = " I see one exit to the West.",
		[E_NS] = " I see two exits: North and South.",
		[E_NE] = " I see two exits: North and East.",
		[E_NW] = " I see two exits: North and West.",
		[E_SE] = " I see two exits: South and East.",
		[E_SW] = " I see two exits: South and West.",
		[E_EW] = " I see two exits: East and West.",
		[E_NSE] = " I see three ways to exit: North, South, and East.",
		[E_NSW] = " I see three ways to exit: North, South, and West.",
		[E_NEW] = " I see three ways to exit: North, East, and West.",
		[E_SEW] = " I see three ways to exit: South, East, and West.",
		[E_NSEW] = " I could travel in any direction from here.",
	}
};

////declarations - gameplay
void action_help(void);
int action_move(dungeon*, const int);
void action_fight(dungeon*);
void action_inv(dungeon*, const int);
void action_reflect(const npc* const);
void action_examine(dungeon*);

//other
int util_attack(npc* attacker, npc* defender, const int atkbuff);
void tutorial(dungeon*);
void get_desc(const dungeon* const);
int select_item(const list* const, const int);

/*
 *    G A M E    F U N C T I O N S
 */
int select_item(const list* const c, const int action) {
	const item* y;
	int x, ret = 0;
	if(action != A_INVENTORY) {
		printf("[0] Exit Selection\n");
	}
	for(x=0; x < LIST_NUM_ITEMS(c);) {
		y = LIST_GET_AS(c, x, item*);
		printf(
			"[%d]%s%s%s%s\n",
			++x,
			adj_rarity[y->val],
			IF(action == A_EXAMINE, adj_trait[y->trait]),
			IF(action == A_EXAMINE, subtype_desc[y->type][y->subtype]),
			noun_type[y->type]
		);
	}
	if(action == A_LOOT) {
		printf("[%d] Take All\n", ++x);
	}
	if(action != A_INVENTORY) {
		ret = input_number(action_prompt[action], 0, x) - 1;
	}
	return ret;
}

void get_desc(const dungeon* const d) {
	room r = d->rooms[d->location];

	printf(
		"\t%s"	//journey conclusion
		"%s"	//low health
		"%s"	//visited
		"%s"	//boss room
		"%s"	//chest room
		"%s"	//sky view
		"%s"	//trait
		"%s"	//cold / dark
		"%s"	//room trait hint
		"%s\n",	//room exits
		IF(d->location==d->end_room, " I believe my journey shall soon end."),
		IF(d->player.hp<3, " I feel the wounds of battle taking their toll."),
		IF(r.visited, " This room seems familiar to me."),
		IF(r.boss_room, " This is a boss room."),
		IF(r.chest, " I see a chest... I wonder what's inside."),
		IF(r.sky_view, " I can see the stars, they remind me to be strong."),
		IF(d->player.trait==r.trait, " I feel at home in this room."),
		room_desc[RD_COLD_DARK][r.cd],
		room_desc[RD_TRAIT][r.trait],
		room_desc[RD_EXITS][r.exits]
	);
}

int action_move(dungeon* d, const int dir) {
	int ret = FALSE;
	room cur = d->rooms[d->location];

	if(cur.exits & dir) {
		ret = TRUE;
		switch (dir) {
			case D_NORTH: d->location -= d->width; break;
			case D_SOUTH: d->location += d->width; break;
			case D_EAST: d->location += 1; break;
			case D_WEST: d->location -= 1; break;
		}
	} else {
		printf("You cannot move in that direction!\n");
	}
	return ret;
}

void action_inv(dungeon* d, const int action) {
	int sel;
	list* con = d->player.inv;

	if(action == A_LOOT) {
		if(d->room_chest) {
			con = d->room_chest;
		} else {
			printf("There is no chest in this room!");
			goto END;
		}
	}
	TOP:
	sel = select_item(con, action);

	if(action != A_INVENTORY) {
		if(sel == -1) {
			goto END;
		}
		if(action == A_SELECT) {
			LIST_REMOVE(con, sel);
		} else if (action == A_LOOT) {
			if(sel == LIST_NUM_ITEMS(con)) {
				for(;LIST_NUM_ITEMS(con);) {
					LIST_ADD(d->player.inv, LIST_GET_AS(con, 0, item*));
					LIST_REMOVE(con, 0);
				}
			} else {
				LIST_ADD(d->player.inv, LIST_GET_AS(con, sel, item*));
				LIST_REMOVE(con, sel);
				goto TOP;
			}
		} else if (action == A_PUTON && LIST_GET_AS(con, sel, item*)->type == T_ARMOR) {
			LIST_ADD(con, &d->player.armor);
			d->player.armor = *LIST_GET_AS(con, sel, item*);
			LIST_REMOVE(con, sel);
			printf("Armor Equipped\n");
		} else if (action == A_ARM && LIST_GET_AS(con, sel, item*)->type == T_WEAP) {
			LIST_ADD(con, &d->player.armor);
			d->player.weapon = *LIST_GET_AS(con, sel, item*);
			LIST_REMOVE(con, sel);
			printf("Weapon Equipped\n");
		} else if (action == A_CONSUME && LIST_GET_AS(con, sel, item*)->type == T_FOOD) {
			d->player.hp += get_rand(LIST_GET_AS(con, sel, item*)->val);
			LIST_REMOVE(con, sel);
			printf("Food Consumed\n");
		}
	}
	END:
	if(action == A_LOOT) {
		d->cur_room->chest = FALSE; //remove the chest when looting is complete
	}
}

void action_reflect(const npc* const p) {
	printf(
		"Name: %s\t HP: %d\t Level: %d\n"
		"Alignment: %s\n",
		p->name,
		p->hp,
		p->level,
		adj_trait[p->trait]
	);
	//TODO: weapon and armor
}

int util_attack(npc* attacker, npc* defender, const int atkbuff) {
	int damage;

	damage = get_rand(attacker->level + attacker->weapon.val + atkbuff);
	damage -= defender->armor.val;
	damage = (damage < 0)? 0 : damage;
	defender->hp -= damage;
	printf("%s lands a blow that deals %d damage!\n", attacker->name, damage);

	return (defender->hp < 1);
}

void action_fight(dungeon* d) {
	if(!d->room_npc) {
		printf("I can't fight anything in this room.");
	} else {
		for(;;) {
			if(util_attack(&(d->player), d->room_npc, d->player_buff)) {
				printf("I have conquered over %s!\n", d->room_npc->name);
				d->player.xp += get_rand(d->room_npc->level) + 1;
				d->cur_room->boss_room = FALSE;
				d->cur_room->chest = TRUE;
				d->update = TRUE;
				break;
			} else if (util_attack(d->room_npc, &(d->player), d->npc_buff)) {
				printf("I have succumbed to battle!\n");
				exit(1);
			}
		}
		if(d->location == d->end_room) {
			printf("Congratulations! You have won the game.\n");
			exit(0);
		}
	}
}

void action_examine(dungeon* d) {
	int x;
	printf(
		"[0] %s\n[1] %s\n[2] Exit\n",
		(d->room_chest)? "Examine Chest" : "Exit: There is no chest to examine",
		(d->room_npc)? "Examine NPC" : "Exit: There is no NPC to examine"
	);
	x = input_number("Enter Selection", 0, 2);
	if(x == 0 && d->room_chest) {

	} else if (x == 1 && d->room_npc) {
		action_reflect(d->room_npc);
	}
}

int main() {
	srand(0); //seed generator with 0 for debugging
	dungeon *game = dungeon_create(10, 20);

	tutorial(game); //tutorial needs to be reworked

	for(;;) {
		get_desc(game);
		dungeon_set_stage(game);
		switch(input_char("")) {
			case 'n': game->update = action_move(game, D_NORTH); break;
			case 's': game->update = action_move(game, D_SOUTH); break;
			case 'e': game->update = action_move(game, D_EAST); break;
			case 'w': game->update = action_move(game, D_WEST); break;
			case 'i': action_inv(game, A_INVENTORY); break;
			case 'd': action_inv(game, A_SELECT); break;
			case 'l': action_inv(game, A_LOOT); break;
			case 'p': action_inv(game, A_PUTON); break;
			case 'f': action_fight(game); break;
			case 'h': action_help(); break;
			case 'r': action_reflect(&(game->player)); break;
			case 'x': action_examine(game); break;

			case 'E': action_inv(game, A_CONSUME); break;
			case 'W': action_inv(game, A_ARM); break;
			case 'Q': dungeon_destroy(game); goto END_GAME;
			case 'S': dungeon_save(game); break;
			case 'L': dungeon_destroy(game); game = dungeon_load(); break;

			case '~': debug_menu(game); break;
			default: printf("Invalid Command!\n");
		}
	}
	END_GAME:
	return 0;
}
//less important stuff below here

//show help
void action_help() {
	printf(
		"\t\tHelp\n"
		"\t========================\n"
		"\tn - move north\n"
		"\ts - move south\n"
		"\te - move east\n"
		"\tw - move west\n"
		"\ti - show inventory\n"
		"\td - drop item\n"
		"\tl - loot container\n"
		"\tf - fight boss\n"
		"\tp - put on armor\n"
		"\tW - wield a weapon\n"
		"\tE - eat food\n"
		"\tr - reflect upon myself\n"
		"\tx - examine\n"
		"\th - this help text\n"
		"\tS - save game\n"
		"\tL - load game\n"
		"\tQ - quit game\n"
		"\t~ - debug menu\n"
	);
}

//tutorial intro sequence
void tutorial(dungeon* d) {
	int x;

	d->player.name = input_string("What is my name?");
	for (x=0; x<TR_NUM; ++x) {
		printf("[%d] %s\n", x, adj_trait[x]);
	}
	d->player.trait = (unsigned char) input_number("What is my Alignment?", 0, TR_NUM);
}
