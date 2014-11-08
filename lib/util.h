#ifndef C_UTIL_H_
#define C_UTIL_H_
C_UTIL_H_//clang

#include <stdlib.h>

#include "types.h"

int get_rand(int);
int get_chance(int);
int get_alignment(unsigned char, unsigned char);
int get_level(const dungeon* const d);

int get_rand(int max) {
	return (max)?(rand() % max):0;
}

int get_chance(int percent) {
	return (get_rand(100) < percent);
}

//determine the difference in alignment (+2, +1, or 0)
int get_alignment(unsigned char you, unsigned char them) {
	char lnc = (you % 3) == (them % 3);
	char gne = (you / 3) == (them / 3);
	return (lnc + gne); //WTF am I doing?
}

int get_level(const dungeon* const d) {
	double max_level = (d->width * d->height);
	double cur_xp = (d->player.xp + 1);
	return (int) ((cur_xp / max_level) * 100) + 1;
}
#endif //C_UTIL_H_
