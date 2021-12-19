#include <iostream>
/*
 You are given six positive integers a, b, c, d, e and f. You need to find out whether there exists a
 triangle ABC with the following properties.
 • AB/BC = a/b;
 • BC/CA = c/d;
 • CA/AB = e/f;
 • Area of ABC is positive;
 • The perimeter of the triangle is integer.
 If such triangle exists, find the minimum possible value of its perimeter.
 */
int main() {
    float a, b, c, d, e, f;
    std::cin >> a >> b >> c >> d >> e >> f;
    float f1 = b/a + e/f;
    float f2 = a/b + d/c;
    float f3 = f/e + c/d;
    
    if ((1 >= f1) || (1 >= f2) || (1 >= f3) || (d*b/(a*c) != (e/f) )) {
        std::cout << "-1";
    }
    else {
        std::cout << "1";
    }
    return 0;
}
