#include "knot.h"

char* BindURLPort(int port) {
	char* str = malloc(6);
	char* dest = malloc(16);
	sprintf(dest, "%s", "tcp://*:");
	sprintf(str, "%d", port);
	return strncat(dest, str, strlen(str));
}

char* ConURLPort(int port) {
	char* dest = malloc(24);
	char* str = malloc(6);
	sprintf(dest, "%s", "tcp://localhost:");
	sprintf(str, "%d", port);
	return strncat(dest, str, strlen(str));
}

int TakePort(void* socket)
{
	int port = BEGIN_PORT;
	while (zmq_bind(socket, BindURLPort(port)) != 0) {
		port++;
		}
	return port;
}
// создаю узел
knot* knot_create(char * id, char * pport) {
		int inPort = 0;
		int i = 0;
		while (i < strlen(pport)) {
			inPort = inPort * 10 + pport[i] - '0';
			i++;
		}
		knot* k = malloc(sizeof(knot));
		i = 0;
		int j = 0;
		while ((id[i] - '0') >= 0 && (id[i] - '0') < 10) {
			j = j * 10 + id[i] - '0';
			i++;
		}
		k->id = j;
		k->pid = getpid();
		k->context_me = zmq_ctx_new();
    	k->r_me = zmq_socket(k->context_me, ZMQ_SUB);
    	zmq_connect(k->r_me, ConURLPort(inPort));
    	zmq_setsockopt(k->r_me, ZMQ_SUBSCRIBE, "", 0);
		k->v = voc_create();
    	k->port_fl = 0;
    	k->context_fl = NULL;
    	k->r_fl = NULL;
    	printf("Ok:%d:created\n",k->id);
    	return k;
	}
void knot_add(knot* k, int d) {
	char* str1 = malloc(10);
	char* str = malloc(10);
		sprintf(str, "%d", d);
		if (k->port_fl == 0) {
    		k->context_fl = zmq_ctx_new();
    		k->r_fl = zmq_socket(k->context_fl, ZMQ_PUB);
    		k->port_fl = TakePort(k->r_fl);
    	}
    	sprintf(str1, "%d", k->port_fl);
    	sprintf(str, "%d", d );
    	pid_t pid = fork();
    	if (pid == 0) {
    		execl("client", str, str1, NULL);
    	}
    	else {
    		free(str);
    		free(str1);
    	}
}	

void knot_destroy(knot* k) {
	zmq_close (k->r_me);
    zmq_ctx_destroy(k->context_me);
    free(k);
}

void knot_destroy_brahch(knot* k) {
//отправить сообщение об удалении сыновей
	if (k->r_fl != NULL) {
		send(k->r_fl, "remove", -1, " ", " ", -1);
	}
	//удалиться
	zmq_close (k->r_me);
    zmq_ctx_destroy(k->context_me);
    free(k);
}
