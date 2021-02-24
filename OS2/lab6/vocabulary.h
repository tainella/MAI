#ifndef _VOCABULARY_H_
#define _VOCABULARY_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

typedef char* ID;

typedef struct _word {
	ID name;
    int value;
} word; // слово словаря

typedef struct _v {
	word head;
	struct _v* next;
} voc;

voc* voc_create(void); 
void voc_add_w(voc* v, ID n, int val);
void voc_print(voc* v);
void voc_destroy(voc* v);
int voc_get_val(voc* v);
word* voc_find(voc* v, ID n);

#endif
