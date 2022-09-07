#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <time.h>

void swap(double* a, int k, int y, int n) {
    double w;
    for(int i = 0; i < n+1; i++) {
        w = a[i * n + k];
        a[i * n + k] = a[i * n + y];
        a[i * n + y] = w;
    }
}

int main() {
    int n;
    std::cin >> n;
    double* a = (double*)malloc(sizeof(double) * n * (n+1));
    double* x = (double*)malloc(sizeof(double) * n);
    double p;
  
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cin >> p;
            a[j*n + i] = p;
        }
    }

    for (int i = 0; i < n; i++) {
            std::cin >> p;
            int s = n*n + i;
            a[s] = p;
    }
    double max = 0;
    int max_i = -1;
    clock_t begin = clock();
    for(int m = 0; m < n-1; m++) {
        for(int s = 0; s < n; s++) {
            if (abs(a[m*n + s]) > max) {
                max = abs(a[m*n + s]);
                max_i = s;
            }
        }
        if(m != max_i) {
            swap(a, m, max_i, n);
        }
        for (int j = m+1; j < n; j++) {
            double d = a[m*n+j] / a[m*n+m];
            for (int i = m; i < n + 1; i++) { //для приписанной матрицы
                a[i*n+j] = a[i*n+j] - a[i*n+m] * d;
            }
        }
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) * 1000 / CLOCKS_PER_SEC;
    printf("%lf\n", time_spent);
    printf("\n");

    x[n - 1] = a[(n+1)*n - 1] / a[(n+1)*n - n - 1];
    for(int k = n-1; k >= 0; k--) {
        double d = 0;
        for (int j = k + 1; j < n; j++) {
            d = d + a[j*n + k] * x[j];
        }
        x[k] = (a[n*n + k] - d) / a[k*n + k];
    }

    for (int i = 0; i < n; i++) {
          std::cout << x[i] << " ";

    }
    return 0;
}
