#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

struct FileMapping {
    int fl;
    size_t fsize;
    char* dataPtr;
};

int Sort(const char* argv, const char* name, char* dataPtr) // сортировка вставкой
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
    for (int k = 0; k < i; k++)
    {
        for (int d = 0; d < 6; d++) {
            printf("%c", mass[k][d]);
        }
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
    int fl = open(name, O_WRONLY, 0); //Открытие для записи
    if(fl < 0) {
        printf("FileMappingOpen - open failed, fname = %s \n", name);
        return(-1);
    }
    printf(" \n");
    int k = 0;
    for(int j = 0; j <= i; j++) {
        for (int y = 0; y < 6; y++) {
            dataPtr[k] = mass[j][y];
            k++;
        }
    }
    close(fl);
    return 0;
}

int main(int argc, const char * argv[]) {
    pid_t pid1, pid2;
    int status1, status2;
    struct FileMapping g1, g2;
    g1.fl = open(argv[3], O_CREAT | O_APPEND | O_RDWR, S_IWUSR | S_IRUSR);
    //  O_RDWR открытие для записи и чтения
    g2.fl = open(argv[4], O_CREAT | O_APPEND | O_RDWR, S_IWUSR | S_IRUSR);
    //  O_RDWR открытие для записи и чтения
    g1.fsize = 200;
    g2.fsize = 200;
    ftruncate(g1.fl, g1.fsize); // усечение файла до размера 100
    ftruncate(g2.fl, g2.fsize);
    g1.dataPtr = (char*)mmap(NULL, g1.fsize, PROT_READ | PROT_WRITE, MAP_SHARED, g1.fl, 0); //Создаем отображение файла в память
    // PROT_READ страницы могут быть прочитаны
    // PROT_WRITE стр могут быть описаны
    //  MAP_SHARED стр могут сипользоваться совместно с др процессами, которые также проектируют этот объект в память
    if (g1.dataPtr == MAP_FAILED)
        // при ошибке возвращается значение MAP_FAILED
    {
        perror("Map");
        printf("FileMappingCreate - open failed 2, fname = %s \n", argv[3]);
        close(g1.fl);
        exit(-1);
    }
    g2.dataPtr = (char*)mmap(NULL, g2.fsize, PROT_READ | PROT_WRITE, MAP_SHARED, g2.fl, 0); //Создаем отображение файла в память
    // PROT_READ страницы могут быть прочитаны
    // PROT_WRITE стр могут быть описаны
    //  MAP_SHARED стр могут сипользоваться совместно с др процессами, которые также проектируют этот объект в память
    if (g2.dataPtr == MAP_FAILED)
        // при ошибке возвращается значение MAP_FAILED
    {
        perror("Map");
        printf("FileMappingCreate - open failed 2, fname = %s \n", argv[4]);
        close(g2.fl);
        exit(-1);
    }
    
    pid1 = fork();
    if (pid1 == 0) { // child process 1
        printf(" child process 1 \n");
        exit(Sort(argv[1], argv[3],g1.dataPtr));
    } else if (pid1 < 0) {
        perror("fork");
    } else {
        printf("in parent process \n");
        pid2 = fork();
        if (pid2 == 0) { // child process 2
            printf(" child process 2 \n");
            exit(Sort(argv[2], argv[4],g2.dataPtr));
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
            for(int k = 0; k <= g1.fsize; k++){
                printf("%c", g1.dataPtr[k]);
            }
            printf("\n end of file1 \n");
        }
        if (waitpid(pid2, &status2, 0) == -1) {
            perror("waitpid");
        }
        else {
            printf("\n ended child process 2: %d \n", status2);
            printf("\n begining of file2 \n");
            for(int k = 0; k <= g2.fsize; k++){
                printf("%c", g2.dataPtr[k]);
            }
            printf("\n end of file2 \n");
        }
    }
    munmap(g1.dataPtr,g1.fsize);
    munmap(g2.dataPtr,g2.fsize);
    close(g1.fl);
    close(g2.fl);
    return 0;
}

