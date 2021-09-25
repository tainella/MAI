#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() {
    long long n;
    scanf("%lld", &n);

    double* v1 = (double*)malloc(n * sizeof(double));
    double* v2 = (double*)malloc(n * sizeof(double));

    for(long long i = 0; i < n; i++) {
        v1[i] = i;
    }
    for(long long i = 0; i < n; i++) {
        v2[i] = n - i;
    }
    
    clock_t begin = clock();
    for (long long i =0; i < n; i++) {
        if (v1[i] > v2[i]) {
            v1[i] = v2[i];
        }
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) * 1000 / CLOCKS_PER_SEC;
    printf("%lf\n", time_spent);
    printf("\n");
    free(v1);
    free(v2);
    return 0;
}
