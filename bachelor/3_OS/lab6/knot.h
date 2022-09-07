#ifndef KNOT_H
#define KNOT_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "vocabulary.h"
#include "message.h"

int BEGIN_PORT = 8000;
//char* BIND_URL = "tcp://*:";
//char* CON_URL = "tcp://localhost:";

char* BindURLPort(int port);

char* ConURLPort(int port);

int TakePort(void* socket);

typedef struct _n {
	int id;
    void* context_me;
    void* r_me;
    pid_t pid;
    voc* v;
    int port_fl;
    void* context_fl;
    void* r_fl;
} knot;

knot* knot_create(char* id, char* pport);
void knot_add(knot* k, int d);
void knot_destroy(knot* k);
void knot_destroy_brahch(knot* k);

#endif
