//сокращение размерностей
//тесты: 
//kg/(kg/(m/s))
//kg*a/(Fs*B)*A*Kt
#include <iostream>
#include <vector>
#include <algorithm>

std::string alph = "AaBbCbDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";

struct StudentCmp {
    bool operator()(const std::string& lhs, const std::string& rhs) const {
        for (int u = 0; u < lhs.size() && u < rhs.size(); u++) {
            if (alph.find(lhs[u]) == alph.find(rhs[u])) {
                
            }
            else {
                return alph.find(lhs[u]) < alph.find(rhs[u]);
            }
        }
        if (lhs.size() < rhs.size()) {
            return 1;
        }
        else {
            return 0;
        }
        return alph.find(lhs[0]) < alph.find(rhs[0]);
    }
};
    
int main(int argc, const char * argv[]) {
    std::string s;
    std::cin >> s;
    std::vector<std::string> v1;
    std::vector<std::string> v2;
    bool fl = true;
    bool to_turn = true;
    std::string buf = "";
    for (int i = 0; i < s.size(); i++) {
        switch (s[i]) {
            case '/':
                if (fl && buf != "") {
                    v1.push_back(buf);
                    fl = false;
                }
                else if (buf != "") {
                    v2.push_back(buf);
                    fl = true;
                }
                buf = "";
                break;
            case '*': {
                if (fl && to_turn && buf != "") {
                    v1.push_back(buf);
                }
                else if (buf != "") {
                    v2.push_back(buf);
                }
                buf = "";
                break;
            }
            case '(': {
                if (i > 0) {
                    if (s[i-1] == '/') {
                        to_turn = false;
                    }
                }
                break;
            }
            case ')': {
                if (fl && to_turn) {
                    v1.push_back(buf);
                }
                else {
                    v2.push_back(buf);
                }
                if (to_turn == false) {
                    to_turn = true;
                }
                    if (fl == false) {
                        fl = true;
                    }
                    else {
                        fl = false;
                    }
                
                buf = "";
                break;
            }
            default: {
                buf += s[i];
                break;
            }
        }
        //std::cout << "s[i]=" << s[i] << "; fl=" << fl << "\n";
    }
    if (fl) {
        v1.push_back(buf);
    }
    else {
        v2.push_back(buf);
    }
    
    /*std::cout << "v1:\n";
    for (int i = 0; i < v1.size(); i++) {
        std::cout << v1[i] << " ";
    }
    std::cout << "\nv2:\n";
    for (int i = 0; i < v2.size(); i++) {
        std::cout << v2[i] << " ";
    }*/
    
    //сокращение
    for (int i = 0; i < v1.size(); i++) {
        for (int j = 0; j < v2.size(); j++) {
            if (v1[i] == v2[j]) {
                v1.erase(v1.begin() + i);
                v2.erase(v2.begin() + j);
            }
        }
    }
    
    /*std::cout << "\n_________\n";
    std::cout << "v1:\n";
    for (int i = 0; i < v1.size(); i++) {
        std::cout << v1[i] << " ";
    }
    std::cout << "\nv2:\n";
    for (int i = 0; i < v2.size(); i++) {
        std::cout << v2[i] << " ";
    }*/
    
    //сортировка
    sort(v1.begin(), v1.end(), StudentCmp());
    sort(v2.begin(), v2.end(), StudentCmp());
    //вывод
    std::cout << v1[0];
    for (int i = 1; i < v1.size(); i++) {
        std::cout << "*" << v1[i];
    }
    std::cout << "\n" << v2[0];
    for (int i = 1; i < v2.size(); i++) {
        std::cout << "*" << v2[i];
    }
    return 0;
}
