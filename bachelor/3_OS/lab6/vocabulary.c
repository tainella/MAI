#include "vocabulary.h"

voc* voc_create(void) {
    voc* v = malloc(sizeof(voc));
    if (!v) {
        fprintf(stderr, "ERROR: no memory\n");
        exit(-1);
    }
    v->head.name = "start";
    v->head.value = 0;
    v->next = NULL;
    return v;
}

word* voc_find(voc* v, ID n) {
    while (v != NULL) {
    if (strcmp(v->head.name, n) == 0) {
            return &(v->head);
        }
        else {
            v = v->next;
        }
    }
    return NULL;
}

void voc_add_w(voc* v, ID n, int val) {
    if (strcmp(v->head.name, n) == 0) {
        v->head.value = val;
    }
    else if (v->next == NULL) {
        v->next = malloc(sizeof(voc));
        v->next->head.name = n;
        v->next->head.value = val;
        v->next->next = NULL;
    }
    else {
        voc_add_w(v->next, n, val);
    }
}

void voc_print(voc* v) {
	if (v != NULL) {
	printf("%s %d\n", v->head.name, v->head.value);
	voc_print(v->next);
	}
}

void voc_destroy(voc* v) {
    if (v->next != NULL) {
        voc_destroy(v->next);
    }
    free(v);
}

int voc_get_val(voc* v) {
    if (v != NULL) {
    return v->head.value;
    }
    else {
        return -1;
    }
}
