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

std::tuple<int, int, int, int, int> MiddleSnake(std::string* a, int N, std::string* b, int M) {
    int delta = N - M;
    int max = M + N;
    static V fv(-max, max);
    static V rv(-max, max);
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

void SES(std::string* a, int N, std::string* b, int M, int pos, int pos1) {
    std::string *startA = nullptr;
    if (startA == nullptr) {
        startA = a;
    }
    while (*a == *b && N > 0 && M > 0) {
        ++a;
        ++pos;
        ++pos1;
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
        std::tie(x, y, u, v, D) = MiddleSnake(a, N, b, M);
        SES(a, x, b, y, pos, pos1);
        SES(a + u, N - u, b + v, M - v, pos+u, pos1+v);
    } else if (N > 0) {
        std::cout << pos +1 << "," << N + pos << "-"  << ":\n";
        for (int i = 0; i < N; i++) {
            std::cout << "- " <<"\x1b[41m" << a[a + i - startA] << "\x1b[40m" << "\n";
        }
        std::cout << "-----------------\n";
    } else if (M > 0) {
        std::cout << "+" << pos1+1 << "," << M + pos1 << ":\n";
        for (int i = 0; i < M; i++) {
            std::cout << "+ " <<"\x1b[42m" << b[i] << "\x1b[40m" <<"\n";
        }
        std::cout << "-----------------\n";
    }
}

int main(int argc, const char * argv[]) {
    const char* name1 = argv[1];
    if (!strcmp(name1,"-help")) {
        std::cout << "MYERS diff algorythm \n+[start position],[end position] shows which lines are added on new positions\n[start position],[end position]- shows which lines are removed from their positions\nEnjoy!\n";
    }
    else {
        const char* name2 = argv[2];
        std::ifstream file1(name1);
        if (!(file1.is_open())){
            std::cout << "No file " << name1 << " found\n";
            return -1;
        }
        std::ifstream file2(name2);
        if (!(file2.is_open())){
            std::cout << "No file " << name2 << " found\n";
            return -1;
        }
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
        SES(a, N_a_size, b, N_b_size, 0, 0);
    }
}
