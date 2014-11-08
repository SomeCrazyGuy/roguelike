#ifndef C_LIST_H_
#define C_LIST_H_
C_LIST_H_//stop clang warning

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//unsafe list macros
#define LIST_UNSAFE_GET(X,Y) (&(X->mem[(Y)*X->elim_size]))
#define LIST_UNSAFE_INSERT(X,Y,Z) (memcpy(LIST_UNSAFE_GET(X, (Y)), (Z), (unsigned long)X->elim_size))

//safe list macros
#define LIST_NEW(X,Y) (list_new(sizeof(X), (Y)))
#define LIST_FREE(X) (list_free(&(X)))
#define LIST_ADD(X,Y) (list_add((X), (void*)(Y)))
#define LIST_GET_AS(X,Y,Z) ((Z)list_get((X), (Y)))
#define LIST_INSERT(X,Y,Z) (list_insert((X), (Y), (void*)(Y)))
#define LIST_NUM_ITEMS(X) ((X)->elim_count)
#define LIST_MAX_ITEMS(X) ((X)->max_elim_count)
#define LIST_REMOVE(X,Y) (list_remove((X),(Y)))

typedef struct {
	char* mem;
	int elim_size;
	int elim_count;
	int max_elim_count;
	int __padding__;
} list;

void  list_free(list** const v);
void  list_add(list* const v, const void* const elim);
void  list_remove(list* const v, const int index);
void* list_get(const list* const v, const int index);
list* list_new(const int elim_size, const int size);
void list_insert(list* const v, const int index, const void* const elim);

list* list_new(const int elim_size, const int size) {
	list* ret = malloc(sizeof(list));
	ret->mem = malloc((unsigned long)(size * elim_size));
	ret->elim_size = elim_size;
	LIST_MAX_ITEMS(ret) = size;
	LIST_NUM_ITEMS(ret) = 0;
	return ret;
}

void list_free(list** const v) {
	if (*v) {
		free((*v)->mem);
		free(*v);
		*v = NULL;
	}
}

void list_insert(list* const v, const int index, const void* const elim) {
	if (index < LIST_NUM_ITEMS(v)) {
		LIST_UNSAFE_INSERT(v, index, elim);
	}
}

void* list_get(const list* const v, const int index) {
	void* ret = NULL;
	if (index < LIST_NUM_ITEMS(v)) {
		ret = LIST_UNSAFE_GET(v, index);
	}
	return ret;
}

void list_add(list* const v, const void* const elim) {
	if (LIST_NUM_ITEMS(v) == LIST_MAX_ITEMS(v)) {
		LIST_MAX_ITEMS(v) <<= 1;
		v->mem = realloc(v->mem, (unsigned long)(LIST_MAX_ITEMS(v) * v->elim_size));
	}
	LIST_UNSAFE_INSERT(v, LIST_NUM_ITEMS(v), elim);
	LIST_NUM_ITEMS(v)++;
}

void list_remove(list* const v, const int index) {
	if(index < LIST_NUM_ITEMS(v)) {
		LIST_NUM_ITEMS(v)--;
		if (index < LIST_NUM_ITEMS(v)) {
			LIST_UNSAFE_INSERT(v, index, LIST_UNSAFE_GET(v, LIST_NUM_ITEMS(v)));
		}
	}
}

#endif //C_LIST_H_
