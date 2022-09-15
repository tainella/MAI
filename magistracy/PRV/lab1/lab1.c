#include <stdio.h>
#include <stdlib.h>
#include <time.h>

 void kernel(int* v1, long long n, int a0, int d) {
	int i = 0;
	while (i < n) {
		v1[i] = a0 + d * i;
		i += 1;
	}
}

int main() {
	long long n;
	//scanf("%lld", &n);

    int a0 = 20;
    //scanf("%d", &a0);

    int d;
    //scanf("%d", &d);
    for (n = 4; n <= 256; n = n * 2) {
        int* v1 = (int*)malloc(n * sizeof(int)); //элементы прогрессии 

        double t0 = clock();
        kernel(v1, n, a0, d);
        double t1 = clock();
        printf("%lf\n",(t1 - t0) * 100/ (CLOCKS_PER_SEC));

        /*for (long long i = 0; i < n; i++) {
            printf("%d ", v1[i]);
        }
        printf("\n");
        */
        free(v1);
    }
	return 0;
}