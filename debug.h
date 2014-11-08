#ifndef C_DEBUG_H_
#define C_DEBUG_H_
C_DEBUG_H_//clang

#include "dungeon.h"


////declarations - debug
void debug_menu(dungeon*);
void debug_room(const room* const);
void debug_dungeon(const dungeon* const);
void debug_script(dungeon*);


void debug_menu(dungeon* d) {
	printf (
		"[0] Exit debug\n"
		"[1] Debug room\n"
		"[2] Debug dungeon\n"
		//"[3] Run Script\n"
	);

	switch (input_number("Debug", 0, 3)) {
		case 0: break;
		case 1: debug_room(d->cur_room); break;
		case 2: debug_dungeon(d); break;
		//case 3: debug_script(d); break;
		default: break;
	}
}

void debug_room(const room* const r) {
	printf (
		"Chest:   [%d]\n"
		"Event:   [%d]\n"
		"Boss:    [%d]\n"
		"Visited: [%d]\n"
		"Sky:     [%d]\n"
		"Trait:   [%d]\n"
		"CD:      [%d]\n"
		"Exits:   [%d]\n",
		r->chest,
		r->event,
		r->boss_room,
		r->visited,
		r->sky_view,
		r->trait,
		r->cd,
		r->exits
	);
}

void debug_dungeon(const dungeon* const d) {
	printf (
		"Rooms:    [%p]\n"
		"cur_room: [%p]\n"
		"room_chest: [%p]\n"
		"room_npc:   [%p]\n"
		"Wid: [%d]    Hei: [%d]\n"
		"Loc: [%d]\n"
		"Spawn: [%d]\n"
		"End:   [%d]\n"
		"Player_buff: [%d]\n"
		"NPC_buff:    [%d]\n"
		"Ticks:  [%d]\n"
		"Update: [%d]\n",
		d->rooms,
		d->cur_room,
		d->room_chest,
		d->room_npc,
		d->width, d->height,
		d->location,
		d->spawn_room,
		d->end_room,
		d->player_buff,
		d->npc_buff,
		d->ticks,
		d->update
	);
}

/*
void debug_script(dungeon* d) {
	enum {
		PTR_NULL,
		PTR_INV,
		PTR_NPC,
		PTR_NUM,
	};
	char scr[128] = {0};
	void* selection = NULL;
	int ptr_type = PTR_NULL;
	int num = 0;
	int pos = 0;
	int error = FALSE;

	input_string(scr, 127, "Script");
	for (;scr[pos] && pos<128;) {
		while(error) {
			if(scr[pos] == '\0' || scr[pos] == '?') {
				error = FALSE;
			}
			++pos;
		}

		switch(scr[pos]) {
			case 'p':
				selection = (void*) &(d->player);
				ptr_type = PTR_NPC;
				break;
			case 'n':
				if (!d->room_npc) {
					error = TRUE;
				} else {
					selection = (void*) d->room_npc;
					ptr_type = PTR_NPC;
				}
				break;
			case 'c':
				if (!d->room_chest) {
					error = TRUE;
				} else {
					selection = (void*) d->room_chest;
					ptr_type = PTR_INV;
				}
				break;
			case ';':
				goto END_SCRIPT;
			case '?':
				error = TRUE;
				break;
			case '!':
				error = TRUE;
				break;
			case 'i':
				if(selection && ptr_type == PTR_NPC) {
					selection = (void*) ((npc*)selection)->inv;
					ptr_type = PTR_INV;
				} else if (ptr_type == PTR_NULL || ptr_type == PTR_NUM) {
					error = TRUE;
				}
				break;
			case 'm':
				if(!selection || ptr_type != PTR_INV) {
					error = TRUE;
				} else {
					num = select_item((list*)selection, A_SELECT);
					if (num < 0 || num == LIST_NUM_ITEMS((list*)selection)) {
						error = TRUE;
					}
				}
				break;
			case 'd':
				if(!selection || ptr_type != PTR_INV) {
					error = TRUE;
				} else {
					LIST_REMOVE((list*)selection, num);
				}
				break;
			case 'C':
				d->cur_room->chest ^= TRUE;
				break;
			case 'N':
				d->cur_room->boss_room ^= TRUE;
				break;
			case 'R':
				d->update = TRUE;
				set_stage(d);
				break;
			default:
				printf("Script Error: %02X\n", scr[pos]);
				goto END_SCRIPT;

		}
		++pos;
	}
	END_SCRIPT:
		;
}
*/
#endif //C_DEBUG_H_
