#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <inttypes.h>
#include <stdlib.h>
#include </usr/local/opt/zeromq/include/zmq.h>

typedef char* ID;

// типы команд:
// create id [parent]
// remove id
// exec id name value
// heartbit time

typedef struct _msg {
    char* action;
    int id;
    char* path;
    ID name;
    int value;
} message;

void send(void* r, char* action, int id, char* path, ID name, int value);
message* rec(void* r);

#endif
