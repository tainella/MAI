/*
You decide to change and start to think positively. You begin with an array of integers (if you
think that it's the strangest legend that you've ever read, you should read some other problems in
this contest). In one operation you can choose any number and either decrease or increase it by 1.
Calculate minimum number of operations to make product of all numbers in this array positive.

Input
The first line contains one integer t number of test cases.
Description of each test case starts with one integer n - length of an array.
Next line contains n integers a1, …, an

Output
For each test case print one integer - minimum number of operations to make product of all numbers in this array positive.
*/
#include <iostream>
#include <climits>

int main() {
    int n, t;
    std::cin >> t;
    for (int j = 0; j < t; j++) {
        std::cin >> n;
        int a[n];
        int maxotr = INT_MIN;
        int minpol = INT_MAX;
        int nol = 0;
        int d = 0;
        int m = 0;
        for(int i = 0; i < n; i++) {
            std::cin >> a[i];
            if (maxotr < a[i] && a[i] < 0) {
                maxotr = a[i];
            }
            if (minpol > a[i] && a[i] > 0) {
                minpol = a[i];
            }
            if (a[i] < 0) {
                m++;
            }
            if (a[i] == 0) {
                nol++;
            }
        }
        //std::cout << "maxotr " << maxotr << "m " << m<< "\n";
        if (m % 2 == 1) {
            if (nol == 0) {
                if (abs(maxotr) < abs(minpol)) {
                    d = d - maxotr + 1; //0 - (-2) + 1 = 3
                }
                else {
                    d = d + minpol + 1;
                }
            }
        }
        d = d + nol;
        std::cout << d << "\n";
    }
    
    return 0;
}
