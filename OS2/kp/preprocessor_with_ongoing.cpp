//  preprocessor for language C++
// Полей-Добронравова Амелия

#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <map>
#include <string>
#include <dirent.h>
#include <sys/stat.h>

enum Lexeme { //препроцессорные лексемы
    CONST, NAME, IDENTIFIER, KEYWORD, OPERATOR, NUMBER, SEPARATOR
};

class Define { //хранит все пары для замены в тексте файла
    std::map<char*, char*> d;
public:
    void define_insert(char* k1, char* k2) {
        char* p1 = new char[sizeof(k1)];
        for (int j = 0; j <= sizeof(k1); j++) {
            p1[j] = k1[j];
        }
        char* p2 = new char[sizeof(k2)];
        for (int j = 0; j <= sizeof(k2); j++) {
            p2[j] = k2[j];
        }
        d[p1] = p2;
    }
    bool define_check(char* k1) {
        std::map<char*, char*> :: iterator it = d.begin();
        for (; it != d.end(); it++) {  // выводим их
            if (!strcmp(it->first, k1)) {
                return true;
            }
        }
        return false;
    }
    void define_delete(char* k1) {
        std::map<char*, char*> :: iterator it = d.begin();
        for (; it != d.end(); it++) {  // выводим их
            if (!strcmp(it->first, k1)) {
                break;
            }
        }
        if (it != d.end()) {
            delete [] it->first;
            delete [] it->second;
            d.erase(it);
        }
    }
    void define_change(char* k1, char* k2) {
        std::map<char*, char*> :: iterator it = d.begin();
        for (; it != d.end(); it++) {  // выводим их
            if (!strcmp(it->first, k1)) {
                it->second = k2;
                break;
            }
        }
        if (it == d.end()) {
            std::cout << "No pair to change\n";
        }
    }
    char* define_second (char* k1) {
        std::map<char*, char*> :: iterator it = d.begin();
        for (; it != d.end(); it++) {  // выводим их
            if (!strcmp(it->first, k1)) {
                return it->second;
            }
        }
        if (it == d.end()) {
            std::cout << "No pair to change\n";
        }
        return NULL;
    }
    void define_cout () {
        std::map<char*, char*> :: iterator it = d.begin();
        for (int k = 0; it != d.end(); it++, k++) {  // выводим их
            std::cout << k << ") Ключ " << it->first << ", значение " << it->second << std::endl;
        }
        std::cout << "\n";
    }
    ~Define() {
        std::map<char*, char*> :: iterator it = d.begin();
        for (; it != d.end(); it++) {  // выводим их
            delete [] it->first;
            delete [] it->second;
        }
    }
    
};

struct FileMapping {
    int fl;
    size_t fsize;
    char* dataPtr;
};

ssize_t getFileSize(const char * fileName)
{
    struct stat file_stat;
    stat(fileName, &file_stat);
    return file_stat.st_size;
}

int main(int argc, const char * argv[]) { //ввод имя файла
    Define d;
    int _LINE_ = 1;
    struct dirent* entry;
    int result;
    DIR *dir = NULL;
    char* _FILE_ = new char[sizeof(&argv[1])];
    char com[8] = {'\0'};
    com[0] = '\0';
    for (int j = 0; j <= strlen(argv[1]); j++) {
        _FILE_[j] = argv[1][j];
    }
    int fp, fl, lib, state = -1, i = 0, i1 = 0;
    ssize_t n = 1;
    char c = '\0', b = '\0';
    struct FileMapping g;
    char buf[255] = {'\0'};
    int bu = 0;
    char path_include[] = "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1";
    char dop[30] = {'\0'};
    char dop1[30] = {'\0'};
    char* uk = NULL;
    dop[0] = '\0';
    dop1[0] = '\0';
    char flag = ' ', flag1 = '+'; //for include
    if ((fp = open(argv[1], O_RDONLY)) < 0) {
        printf("Cannot open file.\n");
        exit(1);
    }
    g.fl = fp;
    g.fsize = getFileSize(argv[1]);
    ftruncate(g.fl, g.fsize);
    g.dataPtr = (char*)mmap(NULL, g.fsize, PROT_READ, MAP_SHARED, g.fl, 0); //Создаем отображение файла в память
    // PROT_READ страницы могут быть прочитаны
    // PROT_WRITE стр могут быть описаны
    //  MAP_SHARED стр могут сипользоваться совместно с др процессами, которые также проектируют этот объект в память
    if (g.dataPtr == MAP_FAILED)
        // при ошибке возвращается значение MAP_FAILED
    {
        perror("Map");
        printf("FileMappingCreate - open failed, fname = %s \n", argv[1]);
        close(g.fl);
        exit(-1);
    }
    if ((fl = creat("lprework.txt", 0666)) == -1) { //буфер для редактирования
        printf("Cannot create file.\n");
        exit(1);
    }
    /* список состояний:
     -3 после define + комбинация
     -2 пропускаю пробелы до директивы
     -1 обработка не символов для препроцессора
     0 обработка директивы
     1 удаляем комментарии пока встречаемый символ не \0
     2 удаляем комментарии пока не встретится * /
     3 подозрение на коментарий
     4 подозрение на закрытие комментария
     5 после директивы
     6 чтение лексем после директивы
     7 после include <
     8 после include "
     */
    c = ' ';
    ssize_t pos = 0;
    while (pos < g.fsize) {
        c = g.dataPtr[pos];
        pos++;
            if (c == '\n') {
                _LINE_++;
            }
            switch (state)
            {
                case -1:
                    if (c == '#') {
                        state = -2;
                        i = 0;
                    }
                    else if (c == '/') {
                        state = 3;
                    }
                    else if ((c == ' ' || c == '\n' || c == '\0') && flag1 == '+') {
                        dop[i] = '\0';
                        while (d.define_check(dop)) {
                            strcpy(dop, d.define_second(dop)); //нашли окончательную замену макроса на число в строковом виде
                        }
                        for (i = 0; i < strlen(dop); i++) {
                            if (write(fl, &dop[i], 1) != 1)
                                printf("Error in writing in.\n");
                        }
                        if (write(fl, &c, 1) != 1)
                            printf("Error in writing in.\n");
                        for (i = 0; i <= strlen(dop); i++) {
                            dop[i] = '\0';
                        }
                        i = 0;
                    }
                    else if (flag1 == '+') {
                        if (i < 30) {
                            dop[i] = c;
                            i++;
                        }
                        else {
                            std::cout << "Lexem is too big\n";
                            munmap(g.dataPtr, g.fsize);
                            close(fp);
                            close(fl);
                            //return 0;
                            exit(0);
                        }
                    }
                    break;
                case -2:
                    if (c == '\n') {
                        state = -1;
                    }
                    else if (c == ' ') {
                        
                    }
                    else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                        com[i] = c;
                        i++;
                        state = 0;
                    }
                    else {
                        state = 1;
                    }
                    break;
                case 0:
                    if (i == 8) {
                        printf("Error in directive 1\n");
                        for (int j = 0; j <= i;j++) {
                            com[j] = '\0';
                        }
                        i = 0;
                        state = 1;
                    }
                    if (c == ' ') {
                        com[i] = '\0';
                        i = 0;
                        std::cout << "КОМАНДА " << com << "|\n";
                        state = 5;
                    }
                    else if (c == '\n') {
                        com[i] = '\0';
                        i = 0;
                        std::cout << "КОМАНДА " << com << "|\n";
                        if (!strcmp(com,"else")) {
                            if (flag1 == '-') {
                                flag1 = '+';
                            }
                            else {
                                flag1 = '-';
                            }
                            for (int j = 0; j <= 5;j++) {
                                com[j] = '\0';
                            }
                            state = -1;
                        }
                        else if (!strcmp(com,"endif")) {
                            flag1 = '+';
                            for (int j = 0; j <= 5;j++) {
                                com[j] = '\0';
                            }
                            state = -1;
                        }
                        else {
                            state = 5;
                        }
                    }
                    else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                        com[i] = c;
                        i++;
                    }
                    else {
                        printf("Error in directive 2\n");
                        for (int j = 0; j <= i;j++) {
                            com[j] = '\0';
                        }
                        i = 0;
                        state = 1; //ожидание конца строки
                    }
                    break;
                case 1:
                    if (c == '\n') {
                        /*if (write(fl, &c, 1) != 1)
                            printf("Error in writing in.\n");
                         */
                        state = -1;
                    }
                    break;
                case -3: //after define + combination
                    if (c == '\n') {
                        state = -1;
                        if (!strcmp(com,"define")) {
                            if (!d.define_check(dop1)) {
                                d.define_insert(dop1, dop);
                            }
                            d.define_cout();
                            for (int j = 0; j <= i1;j++) {
                                dop1[j] = '\0';
                            }
                            for (int j = 0; j <= i;j++) {
                                dop[j] = '\0';
                            }
                            i = 0;
                            i1 = 0;
                            for (int j = 0; j <= 7;j++) {
                                com[j] = '\0';
                            }
                        }
                        else if (!strcmp(com,"undef")) {
                            if (d.define_check(dop)) {
                                d.define_delete(dop);
                            }
                            d.define_cout();
                            for (int j = 0; j <= i;j++) {
                                dop[j] = '\0';
                            }
                            i = 0;
                            for (int j = 0; j <= 6;j++) {
                                com[j] = '\0';
                            }
                        }
                    }
                    else if (c == ' ') {
                        
                    }
                    else {
                        state = 1;
                        if (!strcmp(com,"define")) {
                            for (int j = 0; j <= i1;j++) {
                                dop1[j] = '\0';
                            }
                            for (int j = 0; j <= i;j++) {
                                dop[j] = '\0';
                            }
                            i = 0;
                            i1 = 0;
                            for (int j = 0; j <= 7;j++) {
                                com[j] = '\0';
                            }
                        }
                        else if (!strcmp(com,"undef")) {
                            for (int j = 0; j <= strlen(dop);j++) {
                                dop[j] = '\0';
                            }
                            i = 0;
                            for (int j = 0; j <= 6;j++) {
                                com[j] = '\0';
                            }
                        }
                    }
                    break;
                case 2:
                    if (c == '*') {
                        state = 4;
                    }
                    break;
                case 3:
                    if (c == '/') {
                        c = '\n';
                        if (write(fl, &c, 1) != 1)
                            printf("Error in writing in.\n");
                        state = 1;
                    }
                    else if (c == '*') {
                        state = 2;
                    }
                    else {
                        b = '/';
                        if (write(fl, &b, 1) != 1)
                            printf("Error in writing in.\n");
                        if (write(fl, &c, 1) != 1)
                            printf("Error in writing in.\n");
                        state = -1;
                    }
                    break;
                case 4:
                    if (c == '/') {
                        state = -1;
                    }
                    else {
                        state = 2;
                    }
                    break;
                case 5: //after directive
                    if (c == '\n') {
                        if (!strcmp(com,"error")) {
                            std::cout << "\n";
                            munmap(g.dataPtr, g.fsize);
                            close(fp);
                            close(fl);
                            break;
                        }
                        else if ((!strcmp(com,"line"))) {
                            for (int y = 0; y < i1; y++) {
                                if (dop1[y] < '0' || dop1[y] > '9') {
                                    flag = 'f';
                                    break;
                                }
                            }
                            if (flag == 'f') {
                                std::cout << "Error in #line: line\n\n";
                                flag = ' ';
                            }
                            else {
                                _LINE_ = 0;
                                for (int y = 0; y < i1; y++) {
                                    _LINE_ = _LINE_ * 10 + dop1[y] - '0';
                                }
                                std::cout << "CHANGED LINE: " << _LINE_ << "\n";
                                state = -1;
                                for (int y = 0; y <= i1; y++) {
                                    dop1[y] ='\0';
                                }
                                i1 = 0;
                            }
                        }
                        else if (!strcmp(com,"endif")) {
                            flag1 = '+';
                            for (int j = 0; j <= 6;j++) {
                                com[j] = '\0';
                            }
                            for (int p = 0; p <= i; p++) {
                                dop[p] = '\0';
                            }
                            i = 0;
                        }
                        state = -1;
                        for (int p = 0; p <= i; p++) {
                            dop[p] = '\0';
                        }
                        for (int p = 0; p <= i1; p++) {
                            dop1[p] = '\0';
                        }
                        i = 0;
                        i1 = 0;
                        for (int j = 0; j <= 8;j++) {
                            com[j] = '\0';
                        }
                        flag = ' ';
                    }
                    else if (c == ' ') {
                        
                    }
                    else {
                        i = 0;
                        dop[i] = c;
                        i++;
                        state = 6;
                    }
                    break;
                case 6: //read lexeme, what next?
                    if (c == '\n' || c == ' ') {
                        dop[i] = '\0';
                        if (!strcmp(com,"define") && flag1 == '+') { //без пробелов в именах лексем!!!!
                            if (dop1[0] == '\0') {
                                if (c == '\n') {
                                    dop[0] = '\0';
                                    state = -1;
                                }
                                else {
                                    for (int j = 0; j <= i;j++) {
                                        dop1[j] = dop[j];
                                    }
                                    for (int j = 0; j <= i;j++) {
                                        dop[j] = '\0';
                                    }
                                    i1 = i;
                                    i = 0;
                                    state = 5;
                                }
                            }
                            else {
                                if (c == '\n') {
                                    state = -1;
                                    if (!d.define_check(dop1)) {
                                        d.define_insert(dop1, dop);
                                    }
                                    d.define_cout();
                                    for (int j = 0; j <= i1;j++) {
                                        dop1[j] = '\0';
                                    }
                                    for (int j = 0; j <= i;j++) {
                                        dop[j] = '\0';
                                    }
                                    for (int j = 0; j <= 7;j++) {
                                        com[j] = '\0';
                                    }
                                    i = 0;
                                    i1 = 0;
                                }
                                else {
                                    state = -3;
                                }
                            }
                        }
                        else if (!strcmp(com,"undef") && flag1 == '+') {
                            if (c == '\n') {
                                if (d.define_check(dop)) {
                                    d.define_delete(dop);
                                }
                                d.define_cout();
                                for (int j = 0; j <= i;j++) {
                                    dop[j] = '\0';
                                }
                                i = 0;
                                for (int j = 0; j <= 6;j++) {
                                    com[j] = '\0';
                                }
                                state = -1;
                            }
                            else {
                                state = -3;
                            }
                        }
                        else if (!strcmp(com,"include") && flag1 == '+') {
                            if (flag == ' ') {
                                if (!strcmp(dop,"<") || !strcmp(dop,"\"")) {
                                    flag = dop[0];
                                    state = 5;
                                    dop[0] = '\0';
                                    i = 0;
                                }
                                else if (dop[0] == '<') {
                                    if (dop[i - 1] == '>') {
                                        //поиск <>
                                        dop[i - 1] = '\0';
                                        for (i = 0; i < strlen(dop); i++) {
                                            dop1[i] = dop[i+1];
                                        }
                                        std::cout << "ПОИСК " << dop1 << "|\n\n";
                                        dir = opendir(path_include);
                                        if( dir == NULL ) {
                                            printf( "Error opening dir \n");
                                        }
                                        entry = readdir(dir);
                                        while (entry != NULL) {
                                            if (!strcmp(entry->d_name, dop1)) {
                                                char* path = new char[strlen(path_include) + 30 + 1];
                                                flag = '+';
                                                strcat(path, path_include);
                                                strcat(path, "/");
                                                strcat(path, dop1);
                                                if ((lib = open(path, O_RDONLY)) < 0) {
                                                    printf("Cannot open file.\n");
                                                    exit(1);
                                                }
                                                delete[] path;
                                                while ((n = read(lib,&c, 1)) > 0) {
                                                    if (write(fl, &c, n) != n)
                                                        printf("Error in writing in.\n");
                                                }
                                                close(lib);
                                                break;
                                            }
                                            entry = readdir(dir);
                                        }
                                        closedir(dir);
                                        if (flag != '+') {
                                            std::cout << "Error: file " << dop1 << " not found\n";
                                        }
                                        for (int p = 0; p <= strlen(dop1); p++) {
                                            dop1[p] = '\0';
                                        }
                                        for (int p = 0; p <= strlen(com); p++) {
                                            com[p] = '\0';
                                        }
                                        flag = ' ';
                                        state = -1;
                                    }
                                    else {
                                        flag = dop[0];
                                        state = 5;
                                        for (int p = 0; p < i; p++) {
                                            dop1[p] = dop[p + 1];
                                        }
                                        i1 = i;
                                    }
                                }
                                else if (dop[0] == '"') {
                                    if (dop[i - 1] == '"') {
                                        //поиск ""
                                        dir = opendir(".");
                                        if( dir == NULL ) {
                                            printf( "Error opening dir \n");
                                        }
                                        dop[i - 1] = '\0';
                                        for (i = 0; i < strlen(dop); i++) {
                                            dop1[i] = dop[i+1];
                                        }
                                        entry = readdir(dir);
                                        while (entry != NULL) {
                                            if (!strcmp(entry->d_name, dop1)) {
                                                flag = '+';
                                                if ((lib = open(dop1, O_RDONLY)) < 0) {
                                                    printf("Cannot open file.\n");
                                                    exit(1);
                                                }
                                                while ((n = read(lib,&c, 1)) > 0) {
                                                    if (write(fl, &c, n) != n)
                                                        printf("Error in writing in.\n");
                                                }
                                                close(lib);
                                                break;
                                            }
                                            entry = readdir(dir);
                                        }
                                        closedir(dir);
                                        if (flag != '+') { //не нашли в текущем каталоге
                                            dir = opendir(path_include);
                                            if( dir == NULL ) {
                                                printf( "Error opening dir \n");
                                            }
                                            entry = readdir(dir);
                                            while (entry != NULL) {
                                                if (!strcmp(entry->d_name, dop1)) {
                                                    flag = '+';
                                                    char* path = new char[strlen(path_include) + 30 + 1];
                                                    strcat(path, path_include);
                                                    strcat(path, "/");
                                                    strcat(path, dop1);
                                                    if ((lib = open(path, O_RDONLY)) < 0) {
                                                        printf("Cannot open file.\n");
                                                        exit(1);
                                                    }
                                                    delete [] path;
                                                    while ((n = read(lib,&c, 1)) > 0) {
                                                        if (write(fl, &c, n) != n)
                                                            printf("Error in writing in.\n");
                                                    }
                                                    close(lib);
                                                    break;
                                                }
                                                entry = readdir(dir);
                                            }
                                            closedir(dir);
                                            if (flag != '+') {
                                                std::cout << "Error: file " << dop1 << " not found\n";
                                            }
                                        }
                                        std::cout << "ПОИСК " << dop << "|\n\n";
                                        for (int p = 0; p <= strlen(dop1); p++) {
                                            dop1[p] = '\0';
                                        }
                                        for (int p = 0; p <= strlen(com); p++) {
                                            com[p] = '\0';
                                        }
                                        flag = ' ';
                                        state = -1;
                                    }
                                    else {
                                        flag = dop[0];
                                        state = 5;
                                        for (int p = 0; p < i; p++) {
                                            dop1[p] = dop[p + 1];
                                        }
                                        i1 = i;
                                    }
                                }
                                else {
                                    for (int j = 0; j <= 8;j++) {
                                        com[j] = '\0';
                                    }
                                    if (c == '\n') {
                                        state = -1;
                                    }
                                    else {
                                        state = 1;
                                    }
                                }
                            }
                            else {
                                if (c == '\n' && dop[0] == '\0') {
                                    for (int j = 0; j <= 8;j++) {
                                        com[j] = '\0';
                                    }
                                    for (int p = 0; p <= strlen(dop1); p++) {
                                        dop1[p] = '\0';
                                    }
                                    i1 = 0;
                                    flag = ' ';
                                    state = -1;
                                }
                                else {
                                    if (dop[i-1] == flag || dop[0] == flag || (flag == '<' && dop[0] == '>')) {
                                        if (flag == '"') {
                                            //поиск "
                                            dir = opendir(".");
                                            if( dir == NULL ) {
                                                printf( "Error opening dir \n");
                                            }
                                            entry = readdir(dir);
                                            while (entry != NULL) {
                                                if (!strcmp(entry->d_name, dop1)) {
                                                    flag = '+';
                                                    if ((lib = open(dop1, O_RDONLY)) < 0) {
                                                        printf("Cannot open file.\n");
                                                        exit(1);
                                                    }
                                                    while ((n = read(lib,&c, 1)) > 0) {
                                                        if (write(fl, &c, n) != n)
                                                            printf("Error in writing in.\n");
                                                    }
                                                    close(lib);
                                                    break;
                                                }
                                                entry = readdir(dir);
                                            }
                                            closedir(dir);
                                            if (flag != '+') { //не нашли в текущем каталоге
                                                dir = opendir(path_include);
                                                if( dir == NULL ) {
                                                    printf( "Error opening dir \n");
                                                }
                                                entry = readdir(dir);
                                                while (entry != NULL) {
                                                    if (!strcmp(entry->d_name, dop1)) {
                                                        flag = '+';
                                                        char* path = new char[strlen(path_include) + 30 + 1];
                                                        strcat(path, path_include);
                                                        strcat(path, "/");
                                                        strcat(path, dop1);
                                                        if ((lib = open(path, O_RDONLY)) < 0) {
                                                            printf("Cannot open file.\n");
                                                            exit(1);
                                                        }
                                                        delete[] path;
                                                        while ((n = read(lib,&c, 1)) > 0) {
                                                            if (write(fl, &c, n) != n)
                                                                printf("Error in writing in.\n");
                                                        }
                                                        close(lib);
                                                        break;
                                                    }
                                                    entry = readdir(dir);
                                                }
                                                closedir(dir);
                                                if (flag != '+') {
                                                    std::cout << "Error: file " << dop1 << " not found\n";
                                                }
                                            }
                                            std::cout << "ПОИСК \"" << dop1 << dop << "|\n\n";
                                            for (int p = 0; p <= strlen(dop1); p++) {
                                                dop1[p] = '\0';
                                            }
                                            for (int p = 0; p <= strlen(com); p++) {
                                                com[p] = '\0';
                                            }
                                            flag = ' ';
                                            state = -1;
                                        }
                                        else {
                                            dir = opendir(path_include);
                                            if( dir == NULL ) {
                                                printf( "Error opening dir \n");
                                            }
                                            entry = readdir( dir );
                                            while (entry != NULL) {
                                                if (!strcmp(entry->d_name, dop1)) {
                                                    flag = '+';
                                                    char* path = new char[strlen(path_include) + 30 + 1];
                                                    strcat(path, path_include);
                                                    strcat(path, "/");
                                                    strcat(path, dop1);
                                                    if ((lib = open(path, O_RDONLY)) < 0) {
                                                        printf("Cannot open file.\n");
                                                        exit(1);
                                                    }
                                                    delete[] path;
                                                    while ((n = read(lib,&c, 1)) > 0) {
                                                        if (write(fl, &c, n) != n)
                                                            printf("Error in writing in.\n");
                                                    }
                                                    close(lib);
                                                    break;
                                                }
                                                entry = readdir( dir );
                                            }
                                            closedir( dir );
                                            if (flag != '+') {
                                                std::cout << "Error: file " << dop1 << " not found\n";
                                            }
                                            std::cout << "ПОИСК <" << dop1 << dop << "|\n\n";
                                            for (int p = 0; p <= strlen(dop1); p++) {
                                                dop1[p] = '\0';
                                            }
                                            for (int p = 0; p <= strlen(com); p++) {
                                                com[p] = '\0';
                                            }
                                            flag = ' ';
                                            state = -1;
                                        }
                                    }
                                    else if (dop1[0] == '\0' && c != '\n') {
                                        for (int p = 0; p <= i; p++) {
                                            dop1[p] = dop[p];
                                        }
                                        i1 = i;
                                        state = 5;
                                    }
                                    else {
                                        for (int j = 0; j <= 8;j++) {
                                            com[j] = '\0';
                                        }
                                        state = -1;
                                        for (int p = 0; p <= strlen(dop1); p++) {
                                            dop1[p] = '\0';
                                        }
                                        i1 = 0;
                                        flag = ' ';
                                    }
                                }
                            }
                            for (int j = 0; j <= strlen(dop);j++) {
                                dop[j] = '\0';
                            }
                            i = 0;
                        }
                        else if (!strcmp(com,"if") && flag1 == '+') { //считаем до конца строки и проанализируем
                            bu = 0;
                            b = ' ';
                            if (c != '\n') {
                                while (b != '\n' && pos < g.fsize) {
                                    b = g.dataPtr[pos];
                                    pos++;
                                    buf[bu] = b;
                                    bu++;
                                }
                                if (pos == g.fsize) {
                                    munmap(g.dataPtr, g.fsize);
                                    close(fp);
                                    close(fl);
                                    break;
                                }
                                buf[bu - 1] = '\0';
                            }
                            dop[i] = '\0';
                            _LINE_++;
                            //далее анализируем строку
                            uk = strstr(dop, "defined");
                            if (uk != NULL) {
                                bu = (int)(uk - dop + 7);
                                if (dop[uk - dop - 1] == '!') {
                                    flag = '!';
                                }
                                if (dop[bu] == '(' && !strcmp(buf,"\0")) { //без пробелов defined(NAME) или !defined(NAME)
                                    i = 0;
                                    bu++;
                                    while (bu < strlen(dop)) {
                                        if (dop[bu] == ')') {
                                            dop1[i] = '\0';
                                            break;
                                        }
                                        else {
                                            dop1[i] = dop[bu];
                                            i++;
                                            bu++;
                                        }
                                    }
                                    if (dop[bu] == ')') {
                                        if (flag == '!') {
                                            if (!d.define_check(dop1)) {
                                                flag1 = '+';
                                            }
                                            else {
                                                flag1 = '-';
                                            }
                                        }
                                        else {
                                            if (!d.define_check(dop1)) {
                                                flag1 = '-';
                                            }
                                            else {
                                                flag1 = '+';
                                            }
                                        }
                                    }
                                    else {
                                        flag1 = '-';
                                    }
                                }
                                else { // defined NAME или !defined NAME
                                    i = 0;
                                    while (i < strlen(buf) && buf[i] != ' ') {
                                        dop1[i] = buf[i];
                                        i++;
                                    }
                                    dop1[i] = '\0';
                                    for (; i < strlen(buf); i++) {
                                        if (buf[i] != ' ') {
                                            std::cout << "Error: IF; line:" << _LINE_ <<"\n";
                                            munmap(g.dataPtr, g.fsize);
                                            close(fp);
                                            close(fl);
                                            exit(0);
                                        }
                                    }
                                    if (flag == '!') {
                                        if (!d.define_check(dop1)) {
                                            flag1 = '+';
                                        }
                                        else {
                                            flag1 = '-';
                                        }
                                    }
                                    else {
                                        if (!d.define_check(dop1)) {
                                            flag1 = '-';
                                        }
                                        else {
                                            flag1 = '+';
                                        }
                                    }
                                }
                            }
                            else {
                                if (d.define_check(dop)) {
                                    while (d.define_check(dop)) {
                                        strcpy(dop, d.define_second(dop)); //нашли окончательную замену макроса на число в строковом виде
                                    }
                                    i = 0;
                                    while (buf[i] == ' ') {
                                        i++;
                                    }
                                    if (buf[i] == '>') { // только if NAME > INT
                                        // if NAME < INT
                                        // if NAME == INT
                                        flag = '>';
                                    }
                                    else if (buf[i] == '<') {
                                        flag = '<';
                                    }
                                    else if (buf[i] == '=' && buf[i + 1] == '=') {
                                        flag = '=';
                                        i++;
                                    }
                                    else {
                                        std::cout << "Error: IF; line:" << _LINE_ <<"\n";
                                        munmap(g.dataPtr, g.fsize);
                                        close(fp);
                                        close(fl);
                                        exit(0);
                                    }
                                    i++;
                                    bu = 0;
                                    while (buf[i] == ' ') {
                                        i++;
                                    }
                                    while (buf[i] != ' ' && i < strlen(buf)) {
                                        if (buf[i] < '0' || buf[i] > '9') {
                                            std::cout << "Error: IS'T INT:" << buf[i] << "\n";
                                            munmap(g.dataPtr, g.fsize);
                                            close(fp);
                                            close(fl);
                                            exit(0);
                                        }
                                        bu = bu * 10 + buf[i] - '0';
                                        i++;
                                    }
                                    i1 = 0;
                                    for (i = 0; i < strlen(dop); i++) {
                                        if (dop[i] < '0' || dop[i] > '9') {
                                            std::cout << "Error: MACROS IS'T INT:" << dop[i] <<"\n";
                                            munmap(g.dataPtr, g.fsize);
                                            close(fp);
                                            close(fl);
                                            exit(0);
                                        }
                                        i1 = i1 * 10 + dop[i] - '0';
                                    }
                                    if (flag == '>') {
                                        if (i1 > bu) {
                                            flag1 = '+';
                                        }
                                        else {
                                            flag1 = '-';
                                        }
                                    }
                                    else if (flag == '<') {
                                        if (i1 < bu) {
                                            flag1 = '+';
                                        }
                                        else {
                                            flag1 = '-';
                                        }
                                    }
                                    else if (flag == '=') {
                                        if (i1 == bu) {
                                            flag1 = '+';
                                        }
                                        else {
                                            flag1 = '-';
                                        }
                                    }
                                    else {
                                        std::cout << "Error: FLAG IS NONE; line:" << _LINE_ <<"\n";
                                        munmap(g.dataPtr, g.fsize);
                                        close(fp);
                                        close(fl);
                                        exit(0);
                                    }
                                }
                                else {
                                    std::cout << "Error: MACROS not defined\n";
                                    munmap(g.dataPtr, g.fsize);
                                    close(fp);
                                    close(fl);
                                    exit(0);
                                }
                            }
                            flag = ' ';
                            for (int j = 0; j <= 8;j++) {
                                com[j] = '\0';
                            }
                            for (int p = 0; p <= strlen(dop); p++) {
                                dop[p] = '\0';
                            }
                            for (int p = 0; p <= strlen(buf); p++) {
                                buf[p] = '\0';
                            }
                            for (int p = 0; p <= strlen(dop1); p++) {
                                dop1[p] = '\0';
                            }
                            i = 0;
                            i1 = 0;
                            bu = 0;
                            state = -1;
                        }
                        else if (!strcmp(com,"ifdef") && flag1 == '+') {
                            if (d.define_check(dop)) {
                                flag1 = '+';
                            }
                            else {
                                flag1 = '-';
                            }
                            state = -1;
                            for (int j = 0; j <= 6;j++) {
                                com[j] = '\0';
                            }
                            for (int p = 0; p <= i; p++) {
                                dop[p] = '\0';
                            }
                            i = 0;
                        }
                        else if (!strcmp(com,"ifndef") && flag1 == '+') {
                            if (!d.define_check(dop)) {
                                flag1 = '+';
                            }
                            else {
                                flag1 = '-';
                            }
                            state = -1;
                            for (int j = 0; j <= 6;j++) {
                                com[j] = '\0';
                            }
                            for (int p = 0; p <= i; p++) {
                                dop[p] = '\0';
                            }
                            i = 0;
                        }
                        else if (!strcmp(com,"else") && flag1 == '-') {
                            flag1 = '+';
                            for (int j = 0; j <= 6;j++) {
                                com[j] = '\0';
                            }
                            for (int p = 0; p <= i; p++) {
                                dop[p] = '\0';
                            }
                            i = 0;
                            state = -1;
                        }
                        else if (!strcmp(com,"elif") && flag1 == '-') {
                            
                        }
                        else if (!strcmp(com,"endif")) {
                            flag1 = '+';
                            for (int j = 0; j <= 6;j++) {
                                com[j] = '\0';
                            }
                            for (int p = 0; p <= i; p++) {
                                dop[p] = '\0';
                            }
                            i = 0;
                            state = -1;
                        }
                        else if (!strcmp(com,"line") && flag1 == '+') {
                            if (dop1[0] == '\0') {
                                if (c == '\n') {
                                    for (int y = 0; y < i; y++) {
                                        if (dop[y] < '0' || dop[y] > '9') {
                                            flag = 'f';
                                            break;
                                        }
                                    }
                                    if (flag == 'f') {
                                        std::cout << "Error in #line: line\n";
                                        flag = ' ';
                                        for (int y = 0; y <= 5; y++) {
                                            com[i] = '\0';
                                        }
                                        state = 1;
                                    }
                                    else {
                                        _LINE_ = 0;
                                        for (int y = 0; y < i; y++) {
                                            _LINE_ = _LINE_ * 10 + dop[y] - '0';
                                        }
                                        std::cout << "CHANGED LINE: " << _LINE_ << "\n";
                                        state = -1;
                                    }
                                    for (int y = 0; y <= strlen(dop); y++) {
                                        dop[y] = '\0';
                                    }
                                    i = 0;
                                }
                                else {
                                    state = 5;
                                    for (int p = 0; p <= i; p++) {
                                        dop1[p] = dop[p];
                                    }
                                    i1 = i;
                                    for (int p = 0; p <= i; p++) {
                                        dop[p] = '\0';
                                    }
                                    i = 0;
                                }
                            }
                            else {
                                for (int y = 0; y < i1; y++) {
                                    if (dop1[y] < '0' || dop1[y] > '9') {
                                        flag = 'f';
                                        break;
                                    }
                                }
                                if (flag == 'f') {
                                    std::cout << "Error in #line: line\n";
                                    flag = ' ';
                                    for (int y = 0; y <= 5; y++) {
                                        com[i] = '\0';
                                    }
                                    for (int p = 0; p <= i1; p++) {
                                        dop1[p] = '\0';
                                    }
                                    i1 = 0;
                                    state = 1;
                                }
                                else {
                                    _LINE_ = 0;
                                    for (int y = 0; y < i1; y++) {
                                        _LINE_ = _LINE_ * 10 + dop1[y] - '0';
                                    }
                                    std::cout << "CHANGED LINE: " << _LINE_ << "\n";
                                    for (int p = 0; p <= i1; p++) {
                                        dop1[p] = '\0';
                                    }
                                    i1 = 0;
                                    dir = opendir(".");
                                    if( dir == NULL ) {
                                        printf( "Error opening dir \n");
                                    }
                                    entry = readdir( dir );
                                    while (entry != NULL) {
                                        if (!strcmp(entry->d_name, dop)) {
                                            flag = 'f';
                                            std::cout << "Error in #line: filename\n";
                                            break;
                                        }
                                        entry = readdir( dir );
                                    }
                                    closedir( dir );
                                    if (flag == ' ') {
                                        result= rename(_FILE_, dop);
                                        if ( result == 0 )
                                            puts ( "File successfully renamed" );
                                        else
                                            perror( "Error renaming file" );
                                        for (int y = 0; y <= i; y++) {
                                            _FILE_[y] = dop[y];
                                        }
                                    }
                                    flag = ' ';
                                    for (int j = 0; j <= 5;j++) {
                                        com[j] = '\0';
                                    }
                                    for (int y = 0; y <= strlen(dop); y++) {
                                        dop[y] = '\0';
                                    }
                                    for (int y = 0; y <= strlen(dop1); y++) {
                                        dop1[y] = '\0';
                                    }
                                    state = -1;
                                }
                            }
                        }
                        else if (!strcmp(com,"error") && flag1 == '+') {
                            if (flag == ' ') {
                                std::cout << "Fatal error: ";
                                flag = 'f';
                            }
                            std::cout << dop << " ";
                            if (c == '\n') {
                                std::cout << "\n";
                                munmap(g.dataPtr, g.fsize);
                                close(fp);
                                close(fl);
                                break;
                            }
                            else {
                                state = 5;
                                for (int p = 0; p <= i; p++) {
                                    dop[p] = '\0';
                                }
                                i = 0;
                            }
                        }
                        else if (c == ' ') {
                            for (int j = 0; j <= 8;j++) {
                                com[j] = '\0';
                            }
                            for (int p = 0; p <= i; p++) {
                                dop[p] = '\0';
                            }
                            i = 0;
                            state = 1;
                        }
                        else if (c == '\n') {
                            for (int j = 0; j <= 8;j++) {
                                com[j] = '\0';
                            }
                            for (int p = 0; p <= i; p++) {
                                dop[p] = '\0';
                            }
                            i = 0;
                            state = -1;
                        }
                    }
                    else {
                        if (i == 30) {
                            printf("Lexem is bigger than 30\n");
                            for (int j = 0; j < i;j++) {
                                dop[j] = '\0';
                            }
                            state = 1;
                        }
                        else {
                            dop[i] = c;
                            i++;
                        }
                    }
                    break;
            };
    }
    delete [] _FILE_;
    munmap(g.dataPtr, g.fsize);
    close(fp);
    close(fl);
    //return 0;
    exit(0);
}
