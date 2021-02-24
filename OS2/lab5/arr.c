
#include "/Users/amelia/Documents/arr.h"
#include <errno.h>


void create_massive(Massive* mass, int Length){
    if(Length >= 0){
        mass->size = Length;
        mass->data = malloc(sizeof(ElemType) * Length);
        printf("Massive created!\n");
    } else{
        printf("Error! Wrong size of massive!\n");
        exit(EXIT_FAILURE);
    }
    return;
}

void delete_massive(Massive* mass){
    mass->size = 0;
    free(mass->data);
    mass->data = NULL;
}

ElemType* get_elem_massive(Massive* mass, int index){
    if (index > mass->size || index < 0) {
        printf("Error: wrong index \n");
        return NULL;
    }
    ElemType* curr = mass->data;
    int k = 0;
    while (k < index) {
        k++;
        curr = curr + sizeof(ElemType);
    }
    return (curr);
}

int lenght_massive(Massive* mass){
    return mass->size;
}

bool empty_massive(Massive* mass){
    return (mass->size == 0);
}

void print_massive(Massive* mass){
    printf("Massive:\n Elems:%d \n|", mass->size);
    ElemType* curr = mass->data;
    for (int i = 0; i < mass->size; i++) {
        printf("%d|", *curr);
        curr = curr + sizeof(ElemType);
    }
    printf("|\n");
    return;
}
    

