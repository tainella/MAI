#include <iostream>
#include <vector>
#include <cmath>

#define PI 3.14159265

double angle(int a, int b, int x, int y) {
    return ((a*x + b*y) / (sqrt(a*a + b*b) * sqrt(x*x + y*y)));
}

int main() {
    int n, a, b;
    std::vector<std::pair<int, int>> v;
    std::cin >> n >> a >> b;
    int w;
    for (int i = 0; i < n; i++) {
        std::cin >> w;
        std::pair<int, int> r;
        r.first = w;
        std::cin >> w;
        r.second = w;
        v.push_back(r);
    }
    double ang, min = PI;
    int min_i = 0, min_j = 0;
    for(int i = 0; i < n; i++) {
        for(int j = i+1; j < n; j++) {
            ang = angle(a, b, (v[j].first - v[i].first), (v[j].second - v[i].second));
            if (ang < 0) {
                ang *= -1;
                if (acos(ang) < min) {
                    min = acos(ang);
                    min_i = j;
                    min_j = i;
                }
            }
            else {
                if (acos(ang) < min) {
                    min = acos(ang);
                    min_i = i;
                    min_j = j;
                }
            }
        }
    }
    std::cout << min_i+1 << " " << min_j+1;
    return 0;
}
