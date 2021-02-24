
#include <iostream>

int main() {
    int a, x, b, y, t;
    std::cin >> a >> x >> b >> y >> t;
    int asum, bsum;
    asum = 21*x*(t-30) + a;
    bsum = 21*y*(t-45) + b;
    if (21*x*(t-30) < 0) {
        std::cout << a;
    }
    else {
        std::cout << asum;
    }
    std::cout << " ";
    if (21*y*(t-45) < 0) {
        std::cout << b;
    }
    else {
        std::cout << bsum;
    }
    return 0;
}
//сумарно сколько платить в месяц за велосипед
