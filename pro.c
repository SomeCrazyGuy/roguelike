#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

#define IF(X,Y) ((X))?(Y):""

//TODO: weapon subtype effects
//TODO: friendly npcs
//TODO: quests
//TODO: lore and backstory

//direction
enum {
	D_NORTH,
	D_SOUTH,
	D_EAST,
	D_WEST,
};

//input type
enum {
	I_CHAR,
	I_NUM,
	I_BOOL,
};

//rarity
enum {
	R_WORTHLESS,
	R_VERYCOMMON,
	R_COMMON,
	R_UNCOMMON,
	R_STRANGE,
	R_VERYRARE,
	R_RARE,
	R_LEGENDARY,
	R_NUM,
};
static const char* const adj_rarity[] = {
	[R_WORTHLESS] = "worthless",
	[R_VERYCOMMON] = "very common",
	[R_COMMON] = "common",
	[R_UNCOMMON] = "uncommon",
	[R_STRANGE] = "strange",
	[R_RARE] = "rare",
	[R_VERYRARE] = "very rare",
	[R_LEGENDARY] = "legendary",
};

//item types
enum {
	T_WEAP,
	T_ARMOR,
	T_FOOD,
	T_NUM,
};
static const char* const noun_type[] = {
	[T_WEAP] = "weapon",
	[T_ARMOR] = "armor",
	[T_FOOD] = "food",
};

//weapon subtype
enum {
	TW_DAGGER,
	TW_SWORD,
	TW_LONGSWORD,
	TW_BATTLEAXE,
	TW_NUM,
};
static const char* const adj_weap[] = {
	[TW_DAGGER] = "light",
	[TW_SWORD] = "sword-type",
	[TW_LONGSWORD] = "heavy",
	[TW_BATTLEAXE] = "very heavy",
};

//armor subtype
enum {
	TA_LEATHER,
	TA_CHAINMAIL,
	TA_PLATEMAIL,
	TA_STEEL,
	TA_NUM,
};
static const char* const adj_armor[] = {
	[TA_LEATHER] = "leather",
	[TA_CHAINMAIL] = "chainmail",
	[TA_PLATEMAIL] = "platemail",
	[TA_STEEL] = "steel",
};

//food subtype
enum {
	TF_STALE,
	TF_AGED,
	TF_PRESERVED,
	TF_FRESH,
	TF_NUM,
};
static const char* const adj_food[] = {
	[TF_STALE] = "stale",
	[TF_AGED] = "aged",
	[TF_PRESERVED] = "preserved",
	[TF_FRESH] = "fresh"
};

//item traits
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
static const char* const adj_trait[] = {
	[TR_LG] = "lawful good",
	[TR_LN] = "lawful neutral",
	[TR_LE] = "lawful evil",
	[TR_NG] = "neutral good",
	[TR_NN] = "true neutral",
	[TR_NE] = "neutral evil",
	[TR_CG] = "chaotic good",
	[TR_CN] = "chaotic neutral",
	[TR_CE] = "chaotic evil",
};

//
static const char * const * const subtype_desc[] = { //WTF am I doing?
	[T_WEAP] = adj_weap,
	[T_ARMOR] = adj_armor,
	[T_FOOD] = adj_food,
};

//player inventory actions
enum {
	A_INVENTORY,
	A_DROP,
	A_PUTON,
	A_ARM,
	A_CONSUME,
	A_LOOT,
};
static const char * const action_prompt[] = {
	[A_INVENTORY] = "",
	[A_DROP] = "Which item should I drop?",
	[A_PUTON] = "Which armor should I wear?",
	[A_ARM] = "Which weapon should I arm myself with?",
	[A_CONSUME] = "Which food should I eat?",
	[A_LOOT] = "Which item do I want from this chest?",
};

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


/*
 *    S T R U C T U R E S
 */

typedef struct {
	//exits:
	int n: 1;
	int s: 1;
	int e: 1;
	int w: 1;
	int chest: 1; //room has chest
	int event: 1; //room triggers random event
	int boss_room: 1; //room has boss
	int visited: 1; //room was visited
	int dark: 1; //room is dark
	int cold: 1; //room is cold
	int sky_view: 1; //can view the sky from here
	int trait: 4; //room traits
	int __padding__: 17; //padding
} room;

typedef struct {
	unsigned char type;
	unsigned char val;
	unsigned char subtype;
	unsigned char trait;
} item;

typedef struct {
	item* items;
	int num_items;
	int size;
} container;

typedef struct {
	const char* name;
	container inventory;
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
	container* room_chest;
	npc* room_npc;
	npc player;
	int location;
	int width;
	int height;
	int difficulty;
	int spawn_room;
	int end_room;
	int player_buff;
	int npc_buff;
} dungeon;

////declarations - debug
const char* tfget(const int);
void dump_room(room);
void dump_item(item);
void dump_container(container*);
void dump_npc(npc*);
void dump_dungeon(dungeon*);

////declarations - utility
int get_rand(int);
int get_chance(int);
int get_level(dungeon* d);
int alignment(unsigned char,unsigned char);
container* container_init(int);
void container_fill(container*);
void container_add(container*, item);
item container_remove(container*, int);
int input(const char* const, int);
const char* get_string(char* out, const int max, const char* const prompt);

////declarations - procedural generation
item generate_item(void);
void generate_room(room*);
npc* create_npc(int);
void generate_dungeon_rooms(dungeon*);
dungeon* generate_dungeon(int,int);
void set_stage(dungeon* d);
void get_desc(dungeon*);
void get_item_desc(int, item);

////declarations - gameplay
void help(void);
void move(dungeon*, int);
void fight(dungeon*);
void action(dungeon*, int);
void tutorial(dungeon*);
void reflect(npc* p);

/*
 *    D E B U G
 */
const char* tfget(const int boolean) {
	return (boolean)? "True" : "False";
}

void dump_room(room r) {
	printf(
		"Room {\n"
		"\tN: %s\n"
		"\tS: %s\n"
		"\tE: %s\n"
		"\tW: %s\n"
		"\tChest: %s\n"
		"\tEvent: %s\n"
		"\tBoss: %s\n"
		"\tVisited: %s\n"
		"\tDark: %s\n"
		"\tCold: %s\n"
		"\tSky: %s\n"
		"\tTrait: %s\n"
		"}\n",
		tfget(r.n),
		tfget(r.s),
		tfget(r.e),
		tfget(r.w),
		tfget(r.chest),
		tfget(r.event),
		tfget(r.boss_room),
		tfget(r.visited),
		tfget(r.dark),
		tfget(r.cold),
		tfget(r.sky_view),
		adj_trait[r.trait]
	);
}

void dump_item(item i) {
	printf("%s %s VAL=[%d] SUBTYPE=[%d]\n",
		adj_trait[i.trait], noun_type[i.type], i.val, i.subtype);
}

void dump_container(container* c) {
	int x;
	printf("Container<%p> {\n\tItems: (\n", c);
	for(x=0; x<c->num_items; ++x) {
		dump_item(c->items[x]);
	}
	printf(
		"\t)\n"
		"\tNUM_ITEMS: %d\n"
		"\tSIZE: %d\n"
		"}\n", c->num_items, c->size
	);
}

void dump_npc(npc* n) {
	printf("NPC<%p> {\n", n);
	dump_container(&(n->inventory));
	printf("\tHP: %d\n", n->hp);
	dump_item(n->weapon);
	dump_item(n->armor);
}

void dump_dungeon(dungeon* d) {
	room* r;
	int h, w;
	printf("Dungeon<%p> {\n", d);
	for (h=0; h<d->height; ++h) {
		for(w=0; w<d->width; ++w) {
			r = &(d->rooms[(h*d->width)+w]);
			printf(" | %c%c%c%c",
				(r->n)?'N':'.',
				(r->s)?'S':'.',
				(r->e)?'E':'.',
				(r->w)?'W':'.');
		}
		printf(" |\n");
	}
	printf(
		"\n"
		"\tLocation: %d\n"
		"\tWidth: %d\n"
		"\tHeight: %d\n"
		"\tDiff: %d\n"
		"\tSpawn: %d\n"
		"\tEnd: %d\n"
		"}\n",
		d->location,
		d->width,
		d->height,
		d->difficulty,
		d->spawn_room,
		d->end_room
	);
	dump_npc(&(d->player));
	dump_room(*(d->cur_room));
}

/*
 *    U T I L I T Y    F U N C T I O N S
 */
int get_rand(int max) {
	return (rand() % max);
}

int get_chance(int percent) {
	return (get_rand(100) < percent);
}

int get_level(dungeon* d) {
	float max_r = (d->width * d->height) * 2;
	return (int)((d->player.xp / max_r) * 100) + 1;
}

//determine the difference in alignment (+2, +1, or 0)
int alignment(unsigned char you, unsigned char them) {
	char lnc = (you % 3) == (them % 3);
	char gne = (you / 3) == (them / 3);
	return (lnc + gne); //WTF am I doing?
}

container* container_init(int size) {
	container* ret = calloc(1, sizeof(container));
	ret->items = calloc((unsigned long) size, sizeof(item));
	ret->size = size;
	return ret;
}

void container_fill(container* c) {
	int x;
	c->num_items = c->size;
	for(x=0; x < c->size; ++x) {
		c->items[x] = generate_item();
	}
}

void container_add(container* c, item i) {
	if(!(c->num_items < c->size)) {
		c->size <<= 1;
		c->items = realloc(c->items, (sizeof(item)*(unsigned long)c->size));
	}
	c->items[c->num_items - 1] = i;
	c->num_items++;
}

item container_remove(container* c, int index) {
	item ret = c->items[index];
	c->num_items--;
	c->items[index] = c->items[c->num_items];
	return ret;
}

int input(const char* const prompt, int type) {
	int ret;
	printf("%s> ", prompt);

	if(type == I_NUM) {
		scanf("%d", &ret);
		return ret;
	} else if(type == I_BOOL || type == I_CHAR) {
		for(;;) {
			ret = tolower(getchar());
			if (isalpha(ret)) {
				if(type == I_CHAR) {
					return ret;
				} else {
					if(ret == 'y') {
						return 1;
					} else if(ret == 'n') {
						return 0;
					}
				}
			}
		}
	}

	printf("input error!\n");
	return 0;
}

const char* get_string(char* out, const int max, const char* const prompt) {
	printf("%s>", prompt);
	fgets(out, max, stdin);
	return out;
}

/*
 *    G E N E R A T I O N     F U N C T I O N S
 */
void generate_room(room* r) {
	r->n = get_chance(20);
	r->s = get_chance(20);
	r->e = get_chance(20);
	r->w = get_chance(20);
	r->chest = get_chance(20);
	r->event = get_chance(20);
	r->boss_room = get_chance(10);
	r->trait = get_rand(TR_NUM);
	r->dark = get_chance(10);
	r->cold = get_chance(10);
	r->sky_view = get_chance(5);
}

item generate_item(void) {
	item ret;
	ret.type = (get_chance(50))? T_FOOD : ((get_chance(50))? T_ARMOR : T_WEAP);
	ret.val = (get_chance(50))? R_WORTHLESS : (unsigned char) get_rand(R_NUM);
	ret.subtype = (unsigned char) get_rand(TW_NUM);
	ret.trait = (unsigned char) get_rand(TR_NUM);
	return ret;
}

npc* create_npc(int level) {
	npc* ret = calloc(1, sizeof(npc));
	ret->name = npc_names[get_rand(NAME_MAX)];
	ret->hp = 10 + (get_rand(level * 10));
	ret->xp = 0;
	ret->trait = (unsigned char) get_rand(TR_NUM);
	ret->level = (unsigned char) level;
	ret->inventory = *container_init(level);
	container_fill(&(ret->inventory));
	return ret;
}

void generate_dungeon_rooms(dungeon* d) {
	const int w = d->width;
	const int h = d->height;
	int x;
	d->rooms = calloc((unsigned long)(w*h), sizeof(room));

	for(x=0; x<(w*h); ++x) {
		generate_room(&(d->rooms[x]));
	}

	//create paths:
	d->rooms[d->spawn_room].n = 1;
	d->rooms[d->spawn_room].s = 1;
	d->rooms[d->spawn_room].e = 1;
	d->rooms[d->spawn_room].w = 1;
	d->rooms[d->spawn_room].boss_room = 0; //spawn cannot be boss
	d->rooms[d->spawn_room].visited = 1; //spawn is familiar


	d->rooms[d->end_room].n = 1;
	d->rooms[d->end_room].s = 1;
	d->rooms[d->end_room].e = 1;
	d->rooms[d->end_room].w = 1;
	d->rooms[d->end_room].boss_room = 1; //end must be boss room

	//make sure all paths are valid
	for(x=0; x<(w*h); ++x) {
		if (d->rooms[x].n) {
			if(x < w) {
				d->rooms[x].n = 0;
			} else {
				d->rooms[x - w].s = 1;
			}
		}

		if (d->rooms[x].s) {
			if(x < ((w*h) - w)) {
				d->rooms[x + w].n = 1;
			} else {
				d->rooms[x].s = 0;
			}
		}

		if (d->rooms[x].w) {
			if(x == 0) {
				d->rooms[x].w = 0;
			} else {
				d->rooms[x-1].e = 1;
			}
		}

		if (d->rooms[x].e) {
			if(x == ((w*h)-1)) {
				d->rooms[x].e = 0;
			} else {
				d->rooms[x+1].w = 1;
			}
		}
	}
}

dungeon* generate_dungeon(int w, int h) {
	dungeon* ret = calloc(1, sizeof(dungeon));
	ret->player = *create_npc(1);
	ret->width = w;
	ret->height = h;
	ret->spawn_room = get_rand(w*h);
	ret->end_room = get_rand(w*h);
	ret->location = ret->spawn_room;
	generate_dungeon_rooms(ret);
	return ret;
}

/*
 *    G A M E    F U N C T I O N S
 */
void get_item_desc(int in, item it) {
	printf("[%d] %s %s %s %s\n",
		in,
		adj_rarity[it.val],
		adj_trait[it.trait],
		subtype_desc[it.type][it.subtype],
		noun_type[it.type]
	);
}

void set_stage(dungeon* d) {
	d->player.level = (unsigned char) get_level(d);
	d->cur_room = &(d->rooms[d->location]);
	d->player_buff = 0;
	d->npc_buff = 0;
	d->room_chest = NULL;
	d->room_npc	= NULL;

	if(d->cur_room->chest) {
		d->room_chest = container_init((d->height + d->width)/2);
		container_fill(d->room_chest);
	}

	if(d->cur_room->boss_room) {
		d->room_npc = create_npc(get_rand(d->height + d->width));
	}

	if(d->cur_room->cold) {
		d->player_buff -= 1;
	}

	if(d->cur_room->dark) {
		d->npc_buff += 1;
	}

	if(d->cur_room->sky_view) {
		d->player_buff += 2;
	}

	if(d->cur_room->visited) {
		d->player_buff += 1;
	} else {
		d->player.xp += 1;
	}

	d->player_buff += (alignment(d->player.trait, d->player.weapon.trait)-1);
	d->npc_buff += (alignment(d->player.trait, d->player.armor.trait)-1);
	d->player_buff += (alignment(d->player.trait, (unsigned char)d->cur_room->trait)-1);
}

void get_desc(dungeon* d) {
	room r = *(d->cur_room);

	printf(
		"\t%s"	//journey conclusion
		"%s"	//low health
		"%s"	//visited
		"%s"	//boss room
		"%s"	//chest room
		"%s"	//dark
		"%s"	//cold
		"%s"	//sky view
		"%s"	//trait
		"%s"	//exit north
		"%s"	//exit south
		"%s"	//exit east
		"%s\n",	//exit west
		IF(d->location==d->end_room, " I believe my journey shall soon end."),
		IF(d->player.hp<3, " I feel the wounds of battle taking their toll."),
		IF(r.visited, " This room seems familiar to me."),
		IF(r.boss_room, " This is a boss room."),
		IF(r.chest, " I see a chest... I wonder what's inside."),
		IF(r.dark, " This room is very dark."),
		IF(r.cold, " This room is very cold."),
		IF(r.sky_view, " I can see the stars, they remind me to be strong."),
		IF(d->player.trait==r.trait, " You feel at home in this room."),
		IF(r.n, " I see a passage to the north."),
		IF(r.s, " I see a passage to the south."),
		IF(r.e, " I see a passage to the east."),
		IF(r.w, " I see a passage to the west.")
	);
}

void move(dungeon* d, int dir) {
	room cur = d->rooms[d->location];

	if (dir == D_NORTH && cur.n) {
		d->location -= d->width;
	} else if (dir == D_SOUTH && cur.s) {
		d->location += d->width;
	} else if (dir == D_EAST && cur.e) {
		d->location += 1;
	} else if (dir == D_WEST && cur.w) {
		d->location -= 1;
	} else {
		printf("You cannot move in that direction!\n");
	}
}

void action(dungeon* d, int action) {
	int sel, x;
	container* con = (action==A_LOOT)? d->room_chest : &(d->player.inventory);
	item selection = {0,0,0,0};

	if(action == A_LOOT && !d->cur_room->chest) {
		printf("There is no chest in this room!");
		goto INVALID_SELECTION;
	}

	TOP:
	for (x=0; x < con->num_items; ++x) {
		get_item_desc(x, con->items[x]);
	}

	if(action != A_INVENTORY) {
		printf("[%d] Exit selection\n", con->num_items);
		for(;;) {
			sel = input(action_prompt[action], I_NUM);

			if(sel > con->num_items || sel < 0) {
				goto INVALID_SELECTION;
			}

			if(sel == con->num_items) {
				break;
			}

			if(action == A_DROP || action == A_LOOT) {
				selection = container_remove(con, sel);
				if(action == A_LOOT) {
					container_add(&(d->player.inventory), selection);
					get_item_desc(d->player.inventory.num_items - 1, selection);
					goto TOP;
				}
				break;
			} else if (action == A_PUTON && con->items[sel].type == T_ARMOR) {
				selection = container_remove(con, sel);
				d->player.armor = selection;
				get_item_desc(sel, selection);
				break;
			} else if (action == A_ARM && con->items[sel].type == T_WEAP) {
				selection = container_remove(con, sel);
				d->player.weapon = selection;
				get_item_desc(sel, selection);
				break;
			} else if (action == A_CONSUME && con->items[sel].type == T_FOOD) {
				selection = container_remove(con, sel);
				get_item_desc(sel, selection);
				d->player.hp += get_rand(selection.val * 2);
				break;
			}

			INVALID_SELECTION:
			printf("I believe I've made a mistake in my selection.\n");
			if(!input("Should I try again (y/n)?", I_BOOL)) {
				break;
			}
		}
	}
}

void reflect(npc* p) {
	printf(
		"Name: %s\t HP: %d\t Level: %d\n"
		"Alignment: %s\n",
		p->name,
		p->hp,
		p->level,
		adj_trait[p->trait]
	);
	get_item_desc(0, p->weapon);
	get_item_desc(0, p->armor);
}

void fight(dungeon* d) {
	int damage;
	if (d->room_npc->hp == 0) {
		printf("I cannot fight what is not here.\n");
	} else {
		if (alignment(d->room_npc->trait, d->player.trait) == 2) {
			if(!input("Really fight an ally?", I_BOOL)) {
				goto END_FIGHT;
			}
		}
		for(;;) {
			damage = get_rand(d->player.level + d->player.weapon.val + d->player_buff);
			damage -= d->room_npc->armor.val;
			damage = (damage<0)?0:damage;
			printf("I land a blow that deals %d damage.\n", damage);
			d->room_npc->hp -= damage;

			damage = get_rand(d->room_npc->level + d->room_npc->weapon.val + d->npc_buff);
			damage -= d->player.armor.val;
			damage = (damage<0)?0:damage;
			printf("%s counters with an attack of %d damage.\n", d->room_npc->name, damage);
			d->player.hp -= damage;

			if(d->room_npc->hp <1) {
				printf("I have conquered over %s!\n", d->room_npc->name);
				d->player.xp += get_rand(d->room_npc->level) + 1;
				break;
			}

			if(d->player.hp <1) {
				printf("I have succumbed to battle!\n");
				exit(1);
			}
		}
		if(d->cur_room->boss_room) {
			d->cur_room->boss_room = 0;
			d->cur_room->chest = 1;
		}
		if(d->location == d->end_room) {
			printf("Congratulations! You have won the game.\n");
			exit(0);
		}
	}
	END_FIGHT:
		;
}

int main() {
	srand(0); //seed generator with 0 for debugging
	dungeon *game = generate_dungeon(10, 20);

	tutorial(game);

	for(;;) {
		set_stage(game);
		get_desc(game);

		switch(input("", I_CHAR)) {
			case 'q': goto END_GAME;
			case 'n': move(game, D_NORTH); break;
			case 's': move(game, D_SOUTH); break;
			case 'e': move(game, D_EAST); break;
			case 'w': move(game, D_WEST); break;
			case 'i': action(game, A_INVENTORY); break;
			case 'd': action(game, A_DROP); break;
			case 'l': action(game, A_LOOT); break;
			case 'p': action(game, A_PUTON); break;
			case 'a': action(game, A_ARM); break;
			case 'c': action(game, A_CONSUME); break;
			case 'f': fight(game); break;
			case 'h': help(); break;
			case 'r': reflect(&(game->player)); break;
			case 'z': dump_dungeon(game); break;
			default: printf("Invalid Command!\n");
		}
	}
	END_GAME:
	return 0;
}
//extremely straightforward stuff below here

//tutorial intro sequence
void tutorial(dungeon* d) {
	static char* name[32];
	int x;

	printf(
		"    I am an adventurer of sorts."
		" I comb the dungeons of old looking for treasures and challenges."
		" This has been my life for as long as i've had life."
		" Today is different however, I sense the end of my journey nearing."
		" It's a feeling that is hard to describe, and I'm not sure what it means."
		" I shall soon find out whether it is my advenures or my life that is ending."
		"\n"
	);

	d->player.name = get_string((char*)name, 31, "What is my name?");

	printf(
		"    I must be going crazy... forgetting my name. Who would forget their name?"
		" Looking up at the night sky I can see why the ancients have made me so motivated."
		" It is my unwavering alignment that makes me so motivated."
		"\n"
	);

	for (x=0; x<TR_NUM; ++x) {
		printf("[%d] %s\n", x, adj_trait[x]);
	}

	for(;;) {
		x = input("What is my Alignment?", I_NUM);
		if(x < 0 || x >= TR_NUM) {
			continue;
		}
		d->player.trait = (unsigned char)x;
		break;
	}

	printf(
		"    Forgetting my alignment, I really am going crazy..."
		" At least I still know that 'h' is for help."
		"\n"
	);

	while(input("Press 'h' then enter", I_CHAR) != 'h');

	help();

	printf(
		"    I dream that someday I'll be able to defeat the ultimate boss of this dungeon."
		" On that day, I may finally rest. Now... where was I?"
		"\n"
	);
}

//show help
void help() {
	printf(
		"\t\tHelp\n"
		"\t========================\n"
		"\th - this help text\n"
		"\tq - quit game\n"
		"\tn - move north\n"
		"\ts - move south\n"
		"\te - move east\n"
		"\tw - move west\n"
		"\ti - show inventory\n"
		"\td - drop item\n"
		"\tl - loot container\n"
		"\tf - fight boss\n"
		"\tp - put on armor\n"
		"\ta - arm yourself\n"
		"\tc - consume food\n"
		"\tr - reflect upon myself\n"
		"\tz - debug dump gamestate\n"
	);
}
