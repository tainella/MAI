/*
  Given a list of words consisting of only capital English letters, create a list by reversing them and printing the reversals in alphabetical order.
Input
Each input will consist of a single test case.
The first line of input will contain an integer n (1 ≤ n ≤ 1, 000) indicating the number of words. On the
following n lines will be the words, one per line.
The words will be from 1 to 100 letters long. The words will consist of only capital letters, and there will
be no spaces or blank lines.
Output
Output the words, reversed and sorted, one word per line.
*/

#include <iostream>
#include <vector>
#include <string.h>
#include <stdlib.h>
int comp1 (const void * a, const void * b)
{
    return strcmp(*(char**)a,*(char**)b);
}

void reverse(char* str) {
    char* buf = new char[strlen(str)];
    int i = 0;
    for (int y = (int)(strlen(str) - 1); y >= 0; y--) {
        buf[i] = str[y];
        i++;
    }
    buf[i] = '\0';
    str = buf;
}

int main() {
    int n;
    std::cin >> n; //число слов
    char* a[n];
    char temp;
    for (int y = 0; y < n; y++) {
        char* str = new char[100];
        std::cin >> str;
        for (int i=0;i<strlen(str)/2;i++) {
            temp=str[i];
            str[i]=str[strlen(str)-i-1];
            str[strlen(str)-i-1]=temp;
        }
        a[y] = str;
    }
    
    qsort(a, n, sizeof(char**), comp1); //about and hi i that the this url
    
    //ввывод результатов на экран
    for(int i=0; i< n; i++)
        std::cout << a[i]<< std::endl;
    return 0;
}
