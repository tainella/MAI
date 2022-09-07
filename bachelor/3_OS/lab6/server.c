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

volatile sig_atomic_t flag = 0;

void block_func(int sig)
{   int i;
    printf("Write what you want:\n0|Block/Unblock server\n1|Close server\nYour choice:");
    scanf("%d", &i);
    if(!i){
      if(flag){
        printf("Server blocked!\n");
        flag = 0;
      }else{
        printf("Server unblocked!\n");
        flag = 1;
      }
      signal(SIGINT, block_func);
      return;
    }else{
      printf("Close server!\n");
      exit(0);
    }
}
typedef struct _DB {
    int id;
    struct _DB* l;
    struct _DB* r;
} db;

db* db_create() {
	db* a = malloc(sizeof(db));
	if (!a) {
		fprintf(stderr, "ERROR: no memory\n");
        exit(-1);
    }
    a->id = -1;
    a->r = a->l = NULL;
    return a;
}

char* db_find(db* t, int a, char* path) {
	int i;
	if (t == NULL) {
        return NULL;
    }
    else {
    	if (t->id == a) {
        	char vr[5];
        	sprintf(vr,"%d",t->id);
        	path = strcat(path,vr);
        	path[strlen(path)] = '\0';
        	return path; 	
        }
        else {
        	char vr[5];
        	sprintf(vr,"%d",t->id);
        	char* c1 = malloc(sizeof(255));
        	strcpy(c1,path);
        	i = 0;
        	int j = strlen(c1);
        	while (i < strlen(vr)) {
        		c1[j] = vr[i];
        		i++;
        		j++;
        	}
        	c1 = strcat(c1," ");
        	char* c2 = malloc(sizeof(255));
        	strcpy(c2,path);
        	c1 = db_find(t->l, a, c1);
        	c2 = db_find(t->r, a, c2);
        	if (c1 == NULL) {
        		free(c1);
        		return c2;
        	}
        	else {
        		free(c2);
        		return c1;
        	}
        }
    }
}

db* db_find_k(db* t, int a) {
	if (t == NULL) {
        return NULL;
    }
    else {
        if (t->id == a) {
        	return t;
        }
        else {
        	db* b = t;
            while (b->r != NULL) {
                if (b->r->id == a) {
                    return b->r;
                }
                else {
                    b = b->r;
                }
            }
            db* c1 = db_find_k(t->l, a); 
            db* c2 = db_find_k(t->r, a);
         	if (c1 != NULL) {
         		return c1;
         	} 
         	else {
         		return c2;
         	}
        }
    }
}

void db_add(db* t, int id, int p) {
	db* b = db_find_k(t, p);
	db* a = malloc(sizeof(db));
	if (!a) {
		fprintf(stderr, "ERROR: no memory\n");
        exit(-1);
    }
    a->id = id;
    a->r = a->l = NULL;
    if (b->l == NULL) {
    	b->l = a;
    }
    else {
    	b = b->l;
    	while (b->r != NULL) {
    		b = b->r;
    	}
    	b->r = a;
    }
}

void del_br(db* t, db* lost) {
	db* r = lost;
	if (lost->r != NULL) {
		while (r->r != NULL ) {
			r = r->r;
		}
		r->r = t;
	}
	else {
		lost->r = t;
	}
}
void db_del_k(db* t, int p, db* lost) {
	if (t != NULL) {
		db* b = t;
		db* y = NULL;
		while (b->r != NULL) {
			if (b->r->id == p) {
				y = b->r->r;
				del_br(b->r->l, lost);
				b->r = y;
				break;
			}
			b = b->r;
		}
		if (t->l != NULL) {
			if (t->l->id == p) {
				y = b->l->r;
				del_br(b->l->l, lost);
				b->l = y;
			}
			else {
				db_del_k(t->l, p, lost);
			}
		}
	}
}

void print_tree (db* t, int i) {
    if (t != NULL) {
        for (int j = 0; j < i; j++) {
            printf(" ");
        }
        printf("%d\n", t->id);
        print_tree(t->l, i+1);
        print_tree(t->r, i);
    }
}

int main(int argc, char * argv[]) //ввод имя файла с командами
{
	int fp, vr, u;
	int n,i,j, p, va;
	char* th = malloc(255);
	if ((fp = open(argv[1], O_RDONLY)) < 0) {
		printf("Cannot open file.\n");
		exit(1);
	}
	knot* k = malloc(sizeof(knot));
	db* lost = malloc(sizeof(knot));
	lost->id = -1;
	k->id = -1;
	char name[20];
	for (i = 0; i < 20; i++) {
		name[i] = '\0';
	}
	char* path = NULL;
	db* work = db_create();
	char info[20],c;
	char* action = malloc(10);
	while (i < 10) {
		action[i] = '\0';
		i++;
	}
	i = 0;
	while(i < strlen(th)) {
		th[i] = '\0';
		i++;
	}
	while (1) {
		//signal(SIGINT, block_func);
		//if (!flag) {
			i = 0;
			do {
				if((n = read(fp,&c, 1)) > 0) {
				info[i] = c;
				i++;
				}
				else {
					printf("No more commands: %d\n",n);
					knot_destroy_brahch(k);
                    close(fp);
					exit(0);
				}
			} while (c != '\n' && c != '\0');
			info[i - 1] = '\0';
			i = 0;
			while (info[i] != ' ' && i < strlen(info)) {
				action[i] = info[i];
				i++;
			}
			action[i] = '\0';
			printf("\n read new command: %s\n", info);
			i++;
			// типы команд:
			// create id [parent]
			// remove id
			// exec id name value
			// heartbit time
			if (strcmp(action,"exec") == 0) {
				j = 0;
				while (info[i] != ' ' && i < strlen(info)) {
					j = j * 10 + info[i] - '0';
					i++;
				}
				i++;
				path = db_find(work, j, "");
				if (path != NULL) {
					u = 0;
					while (path[u] != ' ') {
						u++;
					}
					u++;
					vr = 0;
					while(u < strlen(path)) {
						th[vr] = path[u];
						u++;
						vr++;
					}
					u = 0;
					while (info[i] != ' ' && i < strlen(info)) {
						name[u] = info[i];
						i++;
						u++;
					}
					name[u] = '\0';
					p = 0;
					i++;
					if (i < strlen(info)) {
						while (info[i] != '\0') {
							p = p * 10 + info[i] - '0';
							i++;
						}
						send(k->r_fl, "exec", j, th, name, p);
					}
					else {
						send(k->r_fl, "exec", j, th, name, -1);
					}
				} 
				else {
					printf("Error: Not found\n");
				}
				for (i = 0; i <= strlen(name); i++) {
							name[i] = '\0';
				}
			}
			else if (strcmp(action,"remove") == 0) {
				j = 0;
				while (info[i] != '\0') {
					j = j * 10 + info[i] - '0';
					i++;
				}
				i++;
				if (j == -29) {
					printf("Ok:%d:removed\n", k->id);
					knot_destroy_brahch(k);
					close(fp);
					exit(0);
				}
				path = db_find(work, j,"");
					if (path != NULL) {
						u = 0;
						while (path[u] != ' ') {
							u++;
						}
						i = u + 1;
						while(i < strlen(path)) {
							th[i - u - 1] = path[i];
							i++;
						}
					send(k->r_fl, "remove", j, th, "", -1);
					for (i = 0; i < strlen(th); i++) {
							th[i] = '\0';
						}
					db_del_k(work, j, lost);
					print_tree (work, 0);
				} 
				else {
					printf("Error: Not found\n");
				}
			}
			else if (!strcmp(action,"ping")) {
				j = 0;
				while (info[i] != ' ' && info[i] != '\0') {
					j = j * 10 + info[i] - '0';
					i++;
				}
				if (db_find(work, j, "") != NULL) {
					printf("Ok: 1\n");
				}
				else if (db_find(lost, j, "") != NULL) {
					printf("Ok: 0\n");
				}
				else {
					printf("Error: Not found\n");
				}
			}
			else if (strcmp(action,"create") == 0) {
				j = 0;
				while (info[i] != ' ' && info[i] != '\0') {
					j = j * 10 + info[i] - '0';
					i++;
				}
				i++;
				if (db_find(work, j, "") != NULL) {
					printf("Error: Already exists\n");
				} else {
					p = 0;
					while (info[i] != '\0') {
						p = p * 10 + info[i] - '0';
						i++;
					}
					i++;
				if (p == -29) {
					db_add(work, j, -1);
					printf("Tree: \n");
					print_tree(work, 0);
					knot_add(k, j);
				}
				else {
				 	//нормально составить путь
					path = db_find(work, p,"");
					if (path != NULL) {
						db_add(work, j, p);
						u = 0;
						while (path[u] != ' ') {
							u++;
						}
						i = u + 1;
						while(i < strlen(path)) {
							th[i - u - 1] = path[i];
							i++;
						}
						send(k->r_fl, "create", j, th, "", -1);
						for (i = 0; i < strlen(th); i++) {
							th[i] = '\0';
						}
						print_tree (work, 0);
					} 
					else {
						printf("Error: Parent not found\n");
					}
				}
			}
			}
		i = 0;
		path = NULL;
		while (i < strlen(info)) {
			info[i] = '\0';
			i++;
		}
		i = 0;
		while (i < strlen(action)) {
			action[i] = '\0';
			i++;
		}
	}
}
