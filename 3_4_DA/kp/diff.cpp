#include <iostream>
#include <tuple>
#include <string>
#include <cmath>
#include <fstream>

class V {
public:
    V(int b, int e) {
        begin = b;
        end = e;
        data = new int[end - begin + 1];
    }
    
    ~V() {
        delete[](data);
    }
    
    int &operator[](int index) {
        return data[index - begin];
    }

private:
    int* data;
    int begin;
    int end;
};

int MyersDiff(std::string* a, int N, std::string* b, int M) {
    int MAX = M + N;
    V V(-MAX, MAX);
    V[1] = 0;
    int x, y;
    for (int D = 0; D <= MAX; D++) {
        for (int k = -D; k <= D; k += 2) {
            if (k == -D || (k != D && V[k - 1] < V[k + 1])) {
                x = V[k + 1];
            } else {
                x = V[k - 1] + 1;
            }
            y = x - k;
            while (x < N && y < M && a[x] == b[y]) {
                x += 1;
                y += 1;
            }
            V[k] = x;
            if (x >= N && y >= M) {
                return D;
            }
        }
    }
    return -1;
}

std::tuple<int, int, int, int, int> FindMiddleSnake(std::string* a, int N, std::string* b, int M) {
    int delta = N - M;
    int MAX = M + N;
    static V fv(-MAX, MAX);
    static V rv(-MAX, MAX);
    int x, y;
    fv[1] = 0;
    rv[delta + 1] = N + 1;
    for (int D = 0; D <= std::ceil((M + N) / 2.0); D++) {
        for (int k = -D; k <= D; k += 2) {
            if (k == -D || (k != D && fv[k - 1] < fv[k + 1])) {
                x = fv[k + 1];
            } else {
                x = fv[k - 1] + 1;
            }
            y = x - k;
            while (x < N && y < M && a[x] == b[y]) {
                x += 1;
                y += 1;
            }
            fv[k] = x;
            if (delta % 2 != 0 && k >= delta - (D - 1) && k <= delta + D - 1) {
                if (fv[k] >= rv[k]) {
                    return std::make_tuple(rv[k], rv[k] - k, x, y, 2 * D - 1);
                }
            }
        }
        for (int k = -D + delta; k <= D + delta; k += 2) {
            if (k == -D + delta || (k != D + delta && rv[k - 1] >= rv[k + 1])) {
                x = rv[k + 1] - 1;
            } else {
                x = rv[k - 1];
            }
            y = x - k;
            while (x > 0 && y > 0 && a[x - 1] == b[y - 1]) {
                x -= 1;
                y -= 1;
            }
            rv[k] = x;
            if (delta % 2 == 0 && k >= -D && k <= D) {
                if (fv[k] >= rv[k]) {
                    return std::make_tuple(x, y, fv[k], fv[k] - k, 2 * D);
                }
            }
        }
    }
    return {};
}

void SES(std::string* a, int N, std::string* b, int M) {
    std::string *startA = nullptr;
    if (startA == nullptr) {
        startA = a;
    }
    while (*a == *b && N > 0 && M > 0) {
        ++a;
        ++b;
        --N;
        --M;
    }
    while (*(a + N - 1) == *(b + M - 1) && N > 0 && M > 0) {
        --N;
        --M;
    }
    if (N > 0 && M > 0) {
        int x, y, u, v, D;
        std::tie(x, y, u, v, D) = FindMiddleSnake(a, N, b, M);
        SES(a, x, b, y);
        SES(a + u, N - u, b + v, M - v);
    } else if (N > 0) {
        for (int i = 0; i < N; i++) {
            std::cout << "-" << a[a + i - startA] << "\n";
        }
    } else if (M > 0) {
        for (int i = 0; i < M; i++) {
            std::cout << "+" << b[i] << "\n";
        }
    }
}

int main(int argc, const char * argv[]) {
    const char* name1 = argv[1];
    const char* name2 = argv[2];
    std::ifstream file1(name1);
    std::ifstream file2(name2);
    int N_a_max = 20;
    int N_a_size = 0;
    int N_b_max = 20;
    int N_b_size = 0;
    std::string* a = new std::string[N_a_max];
    std::string* b = new std::string[N_b_max];
    std::string buf;
    while(getline(file1, buf)){
        if (N_a_size >= N_a_max) {
            N_a_max = N_a_max * 2;
            std::string* aa = new std::string[N_a_max];
            for (int i = 0; i < N_a_size; i++) {
                aa[i] = a[i];
            }
            delete [] a;
            a = aa;
        }
        a[N_a_size] = buf;
        N_a_size++;
    }
    while(getline(file2, buf)){
        if (N_b_size >= N_b_max) {
            N_b_max = N_b_max * 2;
            std::string* bb = new std::string[N_b_max];
            for (int i = 0; i < N_b_size; i++) {
                bb[i] = b[i];
            }
            delete [] b;
            b = bb;
        }
        b[N_b_size] = buf;
        N_b_size++;
    }
    file1.close();
    file2.close();
    
    std::cout << "\nNumber of changes required: " << MyersDiff(a, N_a_size, b, N_b_size) << "\n" << std::endl;
    SES(a, N_a_size, b, N_b_size);
}
