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
#include <string.h>

#include "knot.h"
#include "message.h"

int main(int argc, char * argv[])
{
	message* mes;
	int vr,u;
	char* th = malloc(255);
	int i;
	knot* k = knot_create(argv[0], argv[1]);
	while(true) {
		mes = rec(k->r_me);
		if (strcmp(mes->action,"exec") == 0) {
			if (mes->id == k->id) {
				printf("Ok:%d",k->id);
				if (mes->value != -29) { //добавить в словарь
					voc_add_w(k->v, mes->name, mes->value);
					printf("\n");
				}
				else { //найти в словаре
					word* w = voc_find(k->v, mes->name);
					if (w == NULL) {
						printf(":'%s' not found\n",mes->name);
					}
					else {
						printf(": %d\n", w->value);
					}
				}
			}
			else {
				vr = 0;
				u = 0;
				while (mes->path[u] != ' ' && mes->path[u] != '\0') {
					vr = vr * 10 + mes->path[u] - '0';
					u++;
				}
				if (vr == k->id) {
					u++;
					i = u;
					vr = 0;
					while(i < strlen(mes->path)) {
						th[vr] = mes->path[i];
						i++;
						vr++;
					}
					if (mes->value == -29) {
						mes->value = -1;
					}
					send(k->r_fl, "exec", mes->id, th, mes->name, mes->value);
					i = 0;
					while(i < strlen(th)) {
						th[i] = '\0';
						i++;
					}
				}
			}
		}
		else if (strcmp(mes->action,"remove") == 0) {
			if (mes->id == k->id || mes->id == -29) {
				printf("Ok:%d:removed\n", k->id);
				knot_destroy(k);
				exit(0);
			}
			else if (k->port_fl != 0) {
				u = 0;
				vr = 0;
				while (mes->path[u] != ' ' && mes->path[u] != '\0') {
					vr = vr * 10 + mes->path[u] - '0';
					u++;
				}
				if (vr == k->id) {
					u++;
					i = u;
					vr = 0;
					while(i < strlen(mes->path)) {
						th[vr] = mes->path[i];
						i++;
						vr++;
					}
					send(k->r_fl, "remove", mes->id, th, "", -1);
					i = 0;
					while(i < strlen(th)) {
						th[i] = '\0';
						i++;
					}
				}
			}
		}
		else if (strcmp(mes->action,"create") == 0) {
			vr = 0;
			u = 0;
			while (mes->path[u] != ' ' && mes->path[u] != '\0') {
				vr = vr * 10 + mes->path[u] - '0';
				u++;
			}
			if (vr == k->id) {
				if (strlen(mes->path) == u) {
					knot_add(k, mes->id);
				}
				else if (k->port_fl != 0) {
					u++;
					i = u;
					vr = 0;
					while(i < strlen(mes->path)) {
						th[vr] = mes->path[i];
						i++;
						vr++;
					}
					send(k->r_fl, "create", mes->id, th, "", -1);
					i = 0;
					while (i < strlen(th)) {
						th[i] = '\0';
						i++;
					}
				}
			}
			free(mes);
		}
	}
	return 0;
}
