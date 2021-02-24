#include "message.h"

void send(void* r, char* action, int id, char* pat, ID nam, int value) {
	zmq_msg_t actionMes;
    zmq_msg_t idMes;
    zmq_msg_t pathMes;
    zmq_msg_t nameMes;
    zmq_msg_t valueMes;
    
    char str1[10];
    sprintf(str1, "%d", id);
    
    char str[10];
    sprintf(str, "%d", value);
    
    zmq_msg_init_data (&actionMes, action, sizeof(action), NULL, NULL);
	zmq_msg_init_data (&idMes, str1, sizeof(str1), NULL, NULL);
	zmq_msg_init_data (&pathMes, pat, sizeof(pat), NULL, NULL);
	zmq_msg_init_data (&nameMes, nam,sizeof(nam), NULL, NULL);
	zmq_msg_init_data (&valueMes, str,sizeof(str), NULL, NULL);
	
	//ZMQ_SNDMORE
	sleep(2);
	zmq_msg_send(&actionMes, r, ZMQ_SNDMORE);
	sleep(1);
	zmq_msg_send(&pathMes, r, ZMQ_SNDMORE);
	sleep(1);
	zmq_msg_send(&idMes, r, ZMQ_SNDMORE);
	sleep(1);
	zmq_msg_send(&nameMes, r, ZMQ_SNDMORE);
	sleep(1);
	zmq_msg_send(&valueMes, r, 0);
	sleep(1);
	
	zmq_msg_close(&actionMes);
	zmq_msg_close(&idMes);
	zmq_msg_close(&pathMes);
	zmq_msg_close(&nameMes);
	zmq_msg_close(&valueMes);
}

message* rec(void* r) {
	message* m = malloc(sizeof(message));
	zmq_msg_t actionMes;
    zmq_msg_t idMes;
    zmq_msg_t pathMes;
    zmq_msg_t nameMes;
    zmq_msg_t valueMes;
    
    zmq_msg_init(&actionMes);
    zmq_msg_init(&idMes);
    zmq_msg_init(&pathMes);
    zmq_msg_init(&nameMes);
    zmq_msg_init(&valueMes);
    
    zmq_msg_recv(&actionMes, r, 0);
    int length = zmq_msg_size(&actionMes);
	m->action = malloc(length);
	memcpy(m->action, zmq_msg_data(&actionMes), length);
    zmq_msg_recv(&pathMes, r, 0);
    length = zmq_msg_size(&pathMes);
    m->path = malloc(length);
    memcpy(m->path, zmq_msg_data(&pathMes), length);
    zmq_msg_recv(&idMes, r, 0);
    length = zmq_msg_size(&idMes);
    char* str = malloc(length);
    memcpy(str, zmq_msg_data(&idMes), length);
    int k = 0;
    int i = 0;
    while (str[i] != '\0') {
    	k = k * 10 + str[i] - '0';
    	i++;
    }
    m->id = k;
	zmq_msg_recv(&nameMes, r, 0);
    length = zmq_msg_size(&nameMes);
	m->name = malloc(length);
	memcpy(m->name, zmq_msg_data(&nameMes), length);
    zmq_msg_recv(&valueMes, r, 0);
    length = zmq_msg_size(&valueMes);
    str = malloc(length);
    memcpy(str, zmq_msg_data(&valueMes), length);
    k = 0;
    i = 0;
    while (str[i] != '\0') {
    	k = k * 10 + str[i] - '0';
    	i++;
    }
    m->value = k;
    
    zmq_msg_close(&actionMes);
	zmq_msg_close(&idMes);
	zmq_msg_close(&pathMes);
	zmq_msg_close(&nameMes);
	zmq_msg_close(&valueMes);
	return m;
}
