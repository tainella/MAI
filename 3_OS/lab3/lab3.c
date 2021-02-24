#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

struct tree {
    int node;
    struct tree* l;
    struct tree* r;
};
struct i_t_s {
    struct tree* t;
    int a;
    int path[16];
    int pos;
};

struct tree* answer = NULL;

struct tree* find1 (struct tree* t, int a) { // есть ли шаг из текущего узла в узел со значением а
    if (t == NULL) {
        return NULL;
    }
    else {
        if (t->node == a) {
            return t;
        }
        else {
            while (t->r != NULL) {
                if (t->r->node == a) {
                    return t->r;
                }
                else {
                    t = t->r;
                }
            }
        }
        return NULL;
    }
}

void* search_tree(void* k) {
    struct tree* b;
    struct i_t_s* p = (struct i_t_s*)k;
    b = p->t;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    if (b != NULL) { //Пока не встретится пустой узел
        if (b->node == p->a) {
            pthread_mutex_lock(&mutex);
            answer = p->t;
            pthread_mutex_unlock(&mutex);
            printf("%d in path:", p->a);
            for (int y = 0; y <= p->pos; y++) {
                printf(" %d /", p->path[y]);
            }
            printf("\n");
            return answer;
        }
        else {
            pthread_t thread1, thread2;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            int result1, result2;
            struct i_t_s* j1 = malloc(sizeof(struct i_t_s));
            j1->t = p->t->l;
            j1->a = p->a;
            for (int y = 0; y <= p->pos; y++) {
                j1->path[y] = p->path[y];
            }
            j1->pos = p->pos + 1;
            j1->path[j1->pos] = p->t->node;
            struct i_t_s* j2 = malloc(sizeof(struct i_t_s));
            j2->t = p->t->r;
            j2->a = p->a;
            for (int y = 0; y <= p->pos; y++) {
                j2->path[y] = p->path[y];
            }
            j2->pos = p->pos;
            for (int y = 0; y <= p->pos; y++) {
                j2->path[y] = p->path[y];
            }
            result1 = pthread_create(&thread1, NULL, search_tree, (void*)j1); //функция для левого поддерева
            result2 = pthread_create(&thread2, NULL, search_tree, (void*)j2); //функция для правого поддерева
            if (result1 != 0) {
                perror("Joining the thread");
                pthread_mutex_destroy(&mutex);
                return NULL;
            }
            if (result2 != 0) {
                perror("Joining the thread");
                pthread_mutex_destroy(&mutex);
                return NULL;
            }
            int status_addr1, status_addr2;
            int status1 = pthread_join(thread1, (void**)&status_addr1);
            if (status1 != 0) {
                printf("main error: can't join thread, status = %d\n", status1);
                exit(-1);
            }
            int status2 = pthread_join(thread2, (void**)&status_addr2);
            if (status2 != 0) {
                printf("main error: can't join thread, status = %d\n", status2);
                exit(-1);
            }
        }
    }
    pthread_mutex_destroy(&mutex);
    return answer;
}
void print_tree (struct tree* t, int i) {
    if (t != NULL) {
        for (int j = 0; j < i; j++) {
            printf(" ");
        }
        printf("%d\n", t->node);
        print_tree(t->l, i+1);
        print_tree(t->r, i);
    }
}
void create_tree(struct tree* t,const char* s) {
    int fd, i,j = 0, vst, p1, fl, fl2;
    struct tree* b, *where;
    char buf[16], c;
    printf("Starting creating tree \n");
    fd = open(s, O_RDONLY);
    while(read(fd, &c, sizeof(char)) > 0) {
        if (c != '\n' && j < 16) {
            buf[j] = c;
            j++;
        }
        else {
            j = 0;
        printf("Read line \n");
            for (i = 0; i < 16; i++) {
                printf("%c", buf[i]);
            }
            printf("\n");
        i = 0;
        while (i < 16) {
            while (buf[i] == ' ' && i < 16) {
                i++;
            }
        if (buf[i] == '+') {
            fl = 1;
            i++;
            while (buf[i] == ' ' && i < 16) {
                i++;
            }
            vst = 0;
           while (buf[i] != ' ' && i < 16) {
                vst = vst * 10 + buf[i] - '0';
                i++;
            }
            while (buf[i] == ' ' && i < 16) {
                i++;
            }
            struct tree* a = malloc(sizeof(struct tree));
            a->l = t;
            where = a;
            fl2 = 0;
            p1 = 0;
            while(buf[i] != ' ' || i >= 16) {
            if (buf[i] == '/' && buf[i + 1] == ' ') {
                break;
            }
            else {
                p1 = 0;
                fl2 = 2;
                while (buf[i] != '/' && i < 16) {
                    p1 = p1 * 10 + buf[i] - '0';
                    i++;
                }
                where = where->l;
                where = find1(where, p1);
                if (where == NULL) {
                    printf("Error wrong path on %d\n", p1);
                    while (buf[i] != ' ' && i < 16) {
                        i++;
                    }
                    fl2 = -1;
                    break;
                }
                i++;
            }
            }
                i++;
                b = malloc(sizeof(struct tree));
                b->node = vst;
                b->l = NULL;
                b->r = NULL;
                if (fl2 > 0) {
                    if (where->l != NULL) {
                        where = where->l;
                    while (where->r != NULL) {
                        where = where->r;
                    }
                        where->r = b;
                    }
                    else {
                        where->l = b;
                    }
                    i++;
                printf("Inserted \n");
                } else if (fl2 == 0) {
                    t->node = b->node;
                    t->l = NULL;
                    t->r = NULL;
                    printf("Inserted in top\n");
                }
                    printf("\n Tree now \n");
                    print_tree(t, 0);
                    printf("\n");
                for (i = 0; i < 16; i++) {
                    buf[i] = ' ';
                }
        }
        }
        }
    }
}

int main(int argc, const char * argv[]) {
    struct tree* t = malloc(sizeof(struct tree));
    t->l = NULL;
    t->r = NULL;
    void* answer;
    int a;
    create_tree(t, argv[1]);
    print_tree(t, 0);
    printf("\n Enter number for searching \n");
    scanf("%d", &a);
    struct i_t_s* j1 = malloc(sizeof(struct i_t_s));
    j1->t = t;
    j1->a = a;
    j1->path[0] = t->node;
    j1->pos = -1;
    answer = search_tree((void*)j1);
    if (answer != NULL) {
        printf("\n There is it in the tree \n");
    }
    else {
        printf("\n There is NOT it in the tree \n");
    }
    free(t);
    free(j1);
    return 0;
}
