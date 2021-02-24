#include "/Users/amelia/Documents/arr.h"

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define EXPEREMENTAL_SIZE 10

int main(int argc, char const *argv[]) {
    Massive mass;
    void* handle = dlopen ("libmy_array.so", RTLD_LAZY);
    if (!handle) {
        fputs (dlerror(), stderr);
        exit(1);
    }
    void (*creater)(Massive*, int);
    creater = dlsym(handle, "create_massive");
    (*creater)(&mass, EXPEREMENTAL_SIZE);
    ElemType* (*getter)(Massive*, int);
    getter = dlsym(handle, "get_elem_massive");
    int (*lenghter)(Massive*);
    lenghter = dlsym(handle, "lenght_massive");
    for(int i=0; i < (*lenghter)(&mass); i++){
        *((*getter)(&mass, i)) = i;
    }
    *((*getter)(&mass, 0)) = 233;
    void (*voider)(Massive*);
    voider = dlsym(handle, "print_massive");
    (*voider)(&mass);
    void (*resizer)(Massive*, int);
    (*voider)(&mass);
    voider = dlsym(handle, "delete_massive");
    (*voider)(&mass);
    return 0;
}

