#ifndef C_ITEM_H_
#define C_ITEM_H_

#include "util.h"

//item rarity / value
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

//item types
enum {
	T_WEAP,
	T_ARMOR,
	T_FOOD,
	T_NUM,
};

//item subtypes
enum {
	TW_LIGHT,
	TW_NORMAL,
	TW_HEAVY,
	TW_VERYHEAVY,
	TW_NUM,
};
enum {
	TA_LEATHER,
	TA_CHAINMAIL,
	TA_PLATEMAIL,
	TA_CRYSTAL,
	TA_NUM,
};
enum {
	TF_STALE,
	TF_OLD,
	TF_PRESERVED,
	TF_FRESH,
	TF_NUM,
};

item item_new(void);

item item_new(void) {
	item ret;
	ret.type = (get_chance(50))? T_FOOD : ((get_chance(50))? T_ARMOR : T_WEAP);
	ret.val = (get_chance(50))? R_WORTHLESS : (unsigned char) get_rand(R_NUM);
	ret.subtype = (unsigned char) get_rand(TW_NUM);
	ret.trait = (unsigned char) get_rand(TR_NUM);
	return ret;
}
#endif //C_ITEM_H_
