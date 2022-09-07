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

int Sort(int* pipe_fd, const char* argv) // сортировка вставкой
{
    int fp, n = 100;
    if ((fp=open(argv, O_RDONLY)) < 0) {
        printf("Cannot open file.\n");
        printf("%d \n", fp);
        exit(-1);
        return -1;
    }
    char mass[n][6];
    char newElement[6], s[6], ss[6];
    int i = -1;
    close(pipe_fd[0]);
    while(read(fp,&newElement,(sizeof(char)* 5)) > 0) {
        if (i >= n) {
            printf("above limit \n");
            break;
        }
        i++;
        for (int f = 0; f < 6; f++) {
             mass[i][f] = newElement[f];
             }
    }
    for (int k = 0; k <= i; k++)
    {
        for (int f = 0; f < 6; f++) {
            s[f] = mass[k][f];
        }
        printf("%s", s);
    }
    printf(" \n");
    for (int j = 0; j <= i; j++)
    {
        for (int k = (i - 1); k > j; k--) // для всех элементов после i-ого
        {
            for (int f = 0; f < 6; f++) {
                s[f] = mass[k][f];
                ss[f] = mass[k - 1][f];
            }
           /* printf(" \n");
            printf("%s \n", s);
            printf("%s \n", ss);
            printf(" \n");
            */
            if (strcmp(ss,s) < 0) // если текущий элемент меньше предыдущего
            {
                strcpy(newElement,ss); // меняем их местами
                for (int f = 0; f < 6; f++) {
                    mass[k - 1][f] = mass[k][f];
                }
                for (int f = 0; f < 6; f++) {
                    mass[k][f] = newElement[f];
                }
            }
        }
    }
    printf(" \n");
    close(fp);
    close(pipe_fd[0]);
    for(int j = 0; j <= i; j++) {
        write(pipe_fd[1],mass[j],sizeof(char) * 6);
    }
    
    close(pipe_fd[1]);
    return 0;
}

int main(int argc, const char * argv[]) {
    pid_t pid1, pid2;
    char s[6],ss[6];
    int status1, status2;
    int pipe_fd1[2];
    int pipe_fd2[2];
    if (pipe(pipe_fd1) == -1) {
        perror("pipe");
    }
    if (pipe(pipe_fd2) == -1) {
        perror("pipe");
    }
    pid1 = fork();
    if (pid1 == 0) { // child process 1
        printf(" child process 1 \n");
        exit(Sort(pipe_fd1, argv[1]));
    } else if (pid1 < 0) {
        perror("fork");
    } else {
        printf("in parent process \n");
        pid2 = fork();
        if (pid2 == 0) { // child process 2
            printf(" child process 2 \n");
            exit(Sort(pipe_fd2, argv[2]));
        }
        else if (pid2 < 0) {
            perror("fork");
        }
        if (waitpid(pid1, &status1, 0) == -1) {
            perror("waitpid");
        }
        else {
            printf("\n ended child process 1: %d \n", status1);
            printf("\n begining of file1 \n");
            close(pipe_fd1[1]);
            while (read(pipe_fd1[0], &s, sizeof(char) * 5) > 0)
            {
                for (int f = 0; f < 6; f++) {
                    printf("%c", s[f]);
                }
            }
            printf("\n end of file1 \n");
            close(pipe_fd1[0]);
        }
        if (waitpid(pid2, &status2, 0) == -1) {
            perror("waitpid");
        }
        else {
            printf("\n ended child process 2: %d \n", status2);
            printf("\n begining of file2 \n");
            close(pipe_fd2[1]);
            while (read(pipe_fd2[0], &ss, sizeof(char) * 5) > 0)
            {
                for (int f = 0; f < 6; f++) {
                    printf("%c", ss[f]);
                }
            }
            printf("\n end of file2 \n");
            close(pipe_fd2[0]);
        }
    }
    return 0;
}

