#include "/Users/amelia/Documents/arr.h"

#define EXPEREMENTAL_SIZE 10

//Бронников Максим
//Использование библиотеки, запущенной на этапе линковки

int main(int argc, char const *argv[]) {
    Massive mass;
    create_massive(&mass, EXPEREMENTAL_SIZE);
    for(int i=0; i < lenght_massive(&mass); i++){
        *(get_elem_massive(&mass, i)) = i;
    }
    *(get_elem_massive(&mass, 0)) = 233;
    print_massive(&mass);
    *(get_elem_massive(&mass, EXPEREMENTAL_SIZE - 1)) = 98;
    print_massive(&mass);
    delete_massive(&mass);
    return 0;
}
