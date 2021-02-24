#ifndef _arr_H_
#define _arr_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

typedef int32_t ElemType;

typedef struct Massive{
    int size;
    ElemType* data;
} Massive;

void create_massive(Massive* mass, int Lenght);
void delete_massive(Massive* mass);
int length_massive(Massive* mass);
ElemType* get_elem_massive(Massive* mass, int index);
bool empty_massive(Massive* mass);
void print_massive(Massive* mass);

#endif
