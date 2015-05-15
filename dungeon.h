#ifndef C_DUNGEON_H_
#define C_DUNGEON_H_

#include <stdio.h>

#include "lib/types.h"
#include "lib/input.h"
#include "lib/util.h"
#include "lib/room.h"
#include "lib/item.h"
#include "lib/npc.h"

void dungeon_create_rooms(dungeon*);
dungeon* dungeon_create(int,int);
void dungeon_destroy(dungeon*);
dungeon* dungeon_load(void);
int dungeon_save(const dungeon* const);
void dungeon_room_create(room* r);
void dungeon_set_stage(dungeon* d);

void dungeon_room_create(room* r) {
	r->exits = (unsigned) get_rand(E_NUM);
	r->cd = (get_chance(70))? CD_N : (unsigned) get_rand(CD_NUM);
	r->chest = (unsigned) get_chance(20);
	r->event = (unsigned) get_chance(20);
	r->boss_room = (unsigned) get_chance(10);
	r->trait = (unsigned) get_rand(TR_NUM);
	r->sky_view = (unsigned) get_chance(5);
}

void dungeon_create_rooms(dungeon* d) {
	const int w = d->width;
	const int h = d->height;
	const int num = (w*h);
	int x;
	d->rooms = calloc((unsigned long)num, sizeof(room));

	for(x=0; x<num; ++x) {
		dungeon_room_create(&(d->rooms[x]));
	}

	//create paths:
	d->rooms[d->spawn_room].exits = E_NSEW;
	d->rooms[d->spawn_room].boss_room = FALSE; //spawn cannot be boss
	d->rooms[d->spawn_room].visited = TRUE; //spawn is familiar

	d->rooms[d->end_room].exits = E_NSEW;
	d->rooms[d->end_room].boss_room = TRUE; //end must be boss room

	for(x=0; x<num; ++x) {
		//make sure no exits lead outside the map
		if (x < w) {
			d->rooms[x].exits &= (E_NSEW ^ D_NORTH);
		}
		if (x > (num-w)) {
			d->rooms[x].exits &= (E_NSEW ^ D_SOUTH);
		}
		if ((x % w) == (w - 1)) {
			d->rooms[x].exits &= (E_NSEW ^ D_EAST);
		}
		if ((x % w) == 0) {
			d->rooms[x].exits &= (E_NSEW ^ D_WEST);
		}

		//make sure all paths are valid
		if (d->rooms[x].exits & D_NORTH) {
			d->rooms[x - w].exits |= D_SOUTH;
		}
		if (d->rooms[x].exits & D_SOUTH) {
			d->rooms[x + w].exits |= D_NORTH;
		}
		if (d->rooms[x].exits & D_EAST) {
			d->rooms[x + 1].exits |= D_WEST;
		}
		if (d->rooms[x].exits & D_WEST) {
			d->rooms[x - 1].exits |= D_EAST;
		}
	}
}

dungeon* dungeon_create(int w, int h) {
	item x = item_new();
	npc *p = npc_create(1);
	dungeon* ret = calloc(1, sizeof(dungeon));
	ret->player = *p;
	ret->player.inv = LIST_NEW(item, 1);
	LIST_ADD(ret->player.inv, &x);
	free(p);
	ret->width = w;
	ret->height = h;
	ret->spawn_room = get_rand(w*h);
	ret->end_room = get_rand(w*h);
	ret->location = ret->spawn_room;
	dungeon_create_rooms(ret);
	ret->cur_room = &(ret->rooms[ret->location]);
	return ret;
}

dungeon* dungeon_load(void) {
	dungeon* d = NULL;
	FILE* save = NULL;
	char *savename;

	savename = input_string("Savefile Name");
	save = fopen(savename, "r");

	if(save == NULL){
		perror("save_dungeon");
		exit(1);
	}

	//restore dungeon
	d = malloc(sizeof(dungeon));
	fread(d, sizeof(dungeon), 1, save);

	//restore rooms
	d->rooms = malloc((unsigned long)(d->width * d->height) * sizeof(room));
	fread(d->rooms, sizeof(room), (unsigned long)(d->width * d->height), save);

	//restore inventory
	d->player.inv = malloc(sizeof(list));
	fread(d->player.inv, sizeof(list), 1, save);

	//restore inventory items
	d->player.inv->mem = malloc(sizeof(item) * (unsigned long)LIST_MAX_ITEMS(d->player.inv));
	fread(d->player.inv->mem, sizeof(item), (unsigned long)LIST_NUM_ITEMS(d->player.inv), save);

	//resore player name
	d->player.name = calloc(1, MAX_NAME_LENGTH);
	fread((void*)d->player.name, 1, (unsigned long)(MAX_NAME_LENGTH - 1), save);

	fclose(save);

	//fix invalid pointers
	d->room_chest = NULL;
	d->room_npc = NULL;
	d->cur_room = NULL;

	d->update = TRUE; //possibly needed
    free(savename);
	return d;
}

//save a dungeon
int dungeon_save(const dungeon* const d) {
	FILE* save = NULL;
	char *savename;

	savename = input_string("Savefile Name");
	save = fopen(savename, "w");

	if(save == NULL){
		perror("save_dungeon");
		return -1;
	}

	//store dungeon struct
	fwrite(d, sizeof(dungeon), 1, save);

	//store rooms
	fwrite(d->rooms, sizeof(room), (unsigned long)(d->width * d->height), save);

	//store player inventory
	fwrite(d->player.inv, sizeof(list), 1, save);

	//store inventory items
	fwrite(d->player.inv->mem, sizeof(item), (unsigned long)LIST_NUM_ITEMS(d->player.inv), save);

	//store player name
	fwrite(d->player.name, 1, MAX_NAME_LENGTH, save);

	fclose(save);
    free(savename);
	return 0;
}

void dungeon_destroy(dungeon* d) {
	if (d->room_chest) {
		LIST_FREE(d->room_chest);
	}
	LIST_FREE(d->player.inv);
	npc_destroy(d->room_npc);
	free((void*)d->player.name);
	free(d->rooms);
	free(d);
}

void dungeon_set_stage(dungeon* d) {
	if (d->update) {
		d->update = FALSE;
		d->player.level = (unsigned char) get_level(d);
		d->cur_room = &(d->rooms[d->location]);
		d->player_buff = 0;
		d->npc_buff = 0;
		LIST_FREE(d->room_chest);
		npc_destroy(d->room_npc);
		d->room_npc = NULL;

		if(d->cur_room->chest) {
			int x;
			item y;
			d->room_chest = LIST_NEW(item, (d->height + d->width)/2);
			for(x=0; x< LIST_MAX_ITEMS(d->room_chest); ++x) {
				y = item_new();
				LIST_ADD(d->room_chest, &y);
			}
		}

		if(d->cur_room->boss_room) {
			d->room_npc = npc_create(get_rand(d->height + d->width));
		}

		if(d->cur_room->cd & CD_C) {
			d->player_buff -= 1;
		}

		if(d->cur_room->cd & CD_D) {
			d->npc_buff += 1;
		}

		if(d->cur_room->sky_view) {
			d->player_buff += 2;
		}

		if(d->cur_room->visited) {
			d->player_buff += 1;
		} else {
			d->player.xp += 1;
			d->cur_room->visited = TRUE;
		}

		d->player_buff += (get_alignment(d->player.trait, d->player.weapon.trait)-1);
		d->npc_buff += (get_alignment(d->player.trait, d->player.armor.trait)-1);
		d->player_buff += (get_alignment(d->player.trait, (unsigned char)d->cur_room->trait)-1);
	}
	d->ticks++;
}
#endif //C_DUNGEON_H_
