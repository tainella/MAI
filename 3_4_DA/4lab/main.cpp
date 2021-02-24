#include <iostream>
#include <vector>
#include <limits>

//unsigned long k = ULONG_MAX;

class Node {
private:
    Node* son[11];                            // массив сыновей Node son[k], 11 элемент - разделитель
    Node* go[11];                                  // массив переходов (запоминаем переходы в ленивой рекурсии), используемый для вычисления суффиксных ссылок
    Node* parent;                                 // вершина родитель
    Node* suffLink;                               // суффиксная ссылка (вычисляем в ленивой рекурсии)
    Node* up;                                     // сжатая суффиксная ссылка
    
public:
    char charToParent;                           // символ, ведущий к родителю
    bool isLeaf;                                 // флаг, является ли вершина терминалом
    int count; //общее число подстрок
    int max;
    std::vector<unsigned long> leafPatternNumber;
    Node(Node* cur, char c) {
        suffLink = nullptr;
        up = nullptr;
        parent = cur;
        charToParent = c;
        isLeaf = false;
        for (int i = 0; i < 11; i++) {
            son[i] = nullptr;
            go[i] = nullptr;
        }
        count = 0;
        max = 0;
    }
    Node() {
        suffLink = nullptr;
        up = nullptr;
        parent = nullptr;
        charToParent = '0';
        isLeaf = false;
        for (int i = 0; i < 11; i++) {
            son[i] = nullptr;
            go[i] = nullptr;
        }
        count = 0;
        max = 0;
    }
    
    void deleteall() {
        for (int i = 0; i < 11; i++) {
            if (son[i]) {
                son[i]->deleteall();
                delete son[i];
            }
        }
    }
    
    Node* getLink(Node* v, int c, Node* root) {
        int u = c - '0';
        if (c-'0' < 0) {
            u = 10;
        }
        if (v->go[u] == nullptr) {   // если переход по символу c ещё не вычислен
            if (v->son[u]) {
                //std::cout << " v->son[c-'0'] " << u << "\n";
                v->go[u] = v->son[u];
            }
            else {
                if (v == root) {
                    v->go[u] = root;
                    //std::cout << "root\n";
                }
                else {
                    //std::cout << "getLink не готов\n";
                    v->go[u] = getLink(getSuffLink(v,root), c, root);
                }
            }
        }
        return v->go[u];
    }
    
    Node* getSuffLink(Node* v, Node* root) {
        if (v->suffLink == nullptr) { // если суффиксная ссылка ещё не вычислена
            if (v == root || v->parent == root) {
                v->suffLink = root;
            }
            else {
                v->suffLink = getLink(getSuffLink(v->parent, root), v->charToParent, root);
            }
        }
        return v->suffLink;
    }
    
    Node* getUp(Node* v, Node* root) {
        if (v->up == nullptr) { // если сжатая суффиксная ссылка ещё не вычислена
            if (getSuffLink(v,root)->isLeaf) {
                //std::cout << "UP v->up = getSuffLink(v,root)\n";
                v->up = getSuffLink(v,root);
            }
            else {
                if (getSuffLink(v, root) == root) {
                    //std::cout << "UP root\n";
                    v->up = root;
                }
                else {
                    //std::cout << "UP getUp(getSuffLink(v,root),root)\n";
                    v->up = getUp(getSuffLink(v,root),root);
                }
            }
        }
        return v->up;
    }
    
    void addString(std::string s, int patternNumber, Node* root) {
        Node* cur = root;
        char c;
        int i = 0;
        int pos = 0;
        if (s[i] == ' ') {
            i++;
            //pos++;
        }
        bool slovo = false;
        for (; i < s.length(); i++) {
            c = s[i];
            if (c-'0' < 0) {
                slovo = false;
                
                pos++;
                slovo = false;
                if (cur->son[10] == nullptr) {
                    cur->son[10] = new Node(cur, ' ');
                }
                cur->isLeaf = false;
                cur = cur->son[10];
                  //////!!!!!!
            }
            else {
                if (c=='0') {
                     if (slovo != false) {
                         if (cur->son[c - '0'] == nullptr) {
                             cur->son[c- '0'] = new Node(cur, c);
                         }
                         cur->isLeaf = false;
                         cur = cur->son[c- '0'];
                     }
                }
                else {
                    if (slovo == false) {
                        slovo = true;
                        pos++;
                    }
                    if (cur->son[c - '0'] == nullptr) {
                        cur->son[c- '0'] = new Node(cur, c);
                    }
                    cur->isLeaf = false;
                    cur = cur->son[c- '0'];
                }
            }
        }
        if (pos > 0) {
            root->count = root->count + pos;
            cur->isLeaf = true;
            patternNumber--;
            //std::cout << "pos = "<< pos<<" patternNumber == " << patternNumber << "\n";
            cur->leafPatternNumber.push_back(patternNumber);
            if (root->max < patternNumber) {
                root->max = patternNumber;
            }
        }
    }
};

Node* create_bor(std::string str) {
    Node* bor = new Node();
    int pos = 0, h, count = 0;
    int number=0;
    int len = 0;
    bool slovo = false;
    //разбиение первой сохраненной строки на подстроки без масок и добавление в бор
    while (pos < str.length()) {
        if (str[pos] == '?') {
            pos++;
            number++;
            len++;
            bor->addString("0", number - len, bor);
            count++;
        }
        else {
            h = pos;
            while (str[h] != '?' && h < str.length()) {
                if (str[h] == ' ') {
                    slovo = false;
                }
                else if (str[h] != ' ' && slovo == false) {
                    number++;
                    slovo = true;
                }
                h++;
            }
            number++;
            //std::cout <<"h: " << h << "| "<< str.substr(pos,h-pos) << " pos " << pos + 1 << std::endl;
            //std::cout << "number - len == " << number - len<< "\n";
            //std::cout << "+ ветка\n";
            bor->addString(str.substr(pos,h-pos), number, bor); //кусок строки
            len = 0;
            count++;
            if (str.substr(pos,h-pos) == " ") {
            	count--;
            }
            pos = h+1; //начало следующей подстроки
        }
    }
    bor->count = count;
    return bor;
}

struct dop {
    unsigned long long pos1; //позиция в тексте
    unsigned long long pos;
    int val;
};

std::vector<std::pair<int, int> > korasik(Node* bor, std::vector<std::string> lines, int i) {
    std::vector<std::pair<int, int> > answer;
    Node* state = bor; //начинаем из корня
    std::vector<struct dop> c;
    char cimbol = '0';
    char cimbol2 = '0';
    int pos = 0, postrue = 0;
    //int cifres = 0;
    int y;
    bool dopflag = true;
    bool slovo = false;
    //std::cout << "начало корасика" << std::endl;
    for (int line = i; line < lines.size(); line++) {
        //state = bor;
        //std::cout << "LINEEEEE LINEEE\n";
        for (int k = 0; k < lines[line].length(); k++) {
            postrue++;
            cimbol2 = cimbol;
            cimbol = lines[line][k]; //очередной символ текста
            if (!(cimbol == '0' && dopflag == true) || (cimbol == '0' && k + 1 < lines[line].length() && (lines[line][k+1] == ' ' || lines[line][k+1] == '\n'))) {
                dopflag = false;
                if (cimbol == ' ') {
                    if (slovo == true) {
                        //pos++;
                        slovo = false;
                        dopflag = false;
                    }
                    else {
                    	dopflag = true;
                    }
                }
                else if (slovo == false) {
                    slovo = true;
                    dopflag = false;
                    pos++;
                }
                //std::cout <<"\ncimbol = " <<cimbol << "|\n";
                if (state->isLeaf && dopflag == false) {
                    //std::cout << "берем getUp\n";
                    state = state->getUp(state, bor);
                }
                else if (dopflag == false){
                    //std::cout << "берем getLink\n";
                    state = state->getLink(state, cimbol, bor);
                }
                if (state == bor) {
                	if (cimbol2 != ' ') {
                    	//std::cout << "on root\n";
                    }
                    else {
                    	//state = state->getLink(state, cimbol, bor);
                    }
                }
                for (int index = 0; index < state->leafPatternNumber.size(); index++) {
                    //std::cout <<"\npos " << pos << " leafPatternNumber" << state->leafPatternNumber[index] << "cimbol = " << lines[line][k]<<"\n";
                    if (state->leafPatternNumber[index] > 0 && pos >= state->leafPatternNumber[index]) {
                        for (y = 0; y < c.size(); y++) {
                            if (c[y].pos == pos - state->leafPatternNumber[index]) {
                                c[y].val++;
                                c[y].pos1 = postrue;
                                if (state->isLeaf == true) {
                                	state = bor;
                                }
                                //std::cout << "с позиции " << pos <<" увеличил " << pos + 1- state->leafPatternNumber[index] << " теперь " << c[y].val << "\n";
                                //len = 0;
                                break;
                            }
                        }
                        if (y == c.size()) {
                            //std::cout << "с позиции " << pos <<" увеличил " << pos + 1- state->leafPatternNumber[index] << " теперь 1\n";
                            struct dop sre;
                            sre.pos = pos - state->leafPatternNumber[index];
                            sre.pos1 = postrue;
                            sre.val = 1;
                            //len = 0;
                            c.push_back(sre);
                            if (state->isLeaf == true) {
                                	state = bor;
                            }
                        }
                    }
                }
            }
        }
        //std::cout << "после строки\n";
        if (slovo == true) {
            slovo = false;
        }
        state = state->getLink(state, ' ', bor);
            dopflag = true;
            for (int index = 0; index < state->leafPatternNumber.size(); index++) {
                //std::cout <<"\npos " << pos  << " leafPatternNumber" << state->leafPatternNumber[index] << "cimbol = _" << "\n";
                if (state->leafPatternNumber[index] > 0 && pos >= state->leafPatternNumber[index]) {
                    for (y = 0; y < c.size(); y++) {
                        if (c[y].pos == pos - state->leafPatternNumber[index]) {
                            c[y].val++;
                            c[y].pos1 = postrue;
                            //std::cout << "с позиции " << pos <<" увеличил " << pos + 1- state->leafPatternNumber[index] << " теперь " << c[y].val << "\n";
                            //len = 0;
                            break;
                        }
                    }
                    if (y == c.size()) {
                        struct dop sre;
                        //std::cout << "с позиции " << pos <<" увеличил " << pos + 1 - state->leafPatternNumber[index] << " теперь 1\n";
                        sre.pos = pos - state->leafPatternNumber[index];
                        sre.val = 1;
                        sre.pos1 = postrue;
                        c.push_back(sre);
                    }
                }
            }
            state = state->getLink(state, ' ', bor);
    }
    //std::cout << "\nПОСЛЕ" << std::endl;
    /*for (y = 0; y < c.size(); y++) {
        std::cout << c[y].pos << " " << c[y].val << "\n";
    }
    std::cout << "\n";
    */
    //ищем максимальный паттерннамбер
    slovo = false;
    pos = 0;
    int word_number;
    //std::cout << "bor->count = " << bor->count << "\n";
    for (y = 0; y < c.size(); y++) {
        if (c[y].val >= bor->count) {
            pos = -1;
            bool flag; // на разделителе - true; на слове - false
            //вхождение всего образца в текст
            //нахождение пары строка+слово
            //std::cout << "END\n";
            for (int line = i; line < lines.size(); line++) {
                word_number = 0;
                flag = true;
                for (int k = 0; k < lines[line].length(); k++) {
                    if ((lines[line][k] != ' ' && lines[line][k] != '\n' && lines[line][k] != '\0') && flag == true) {
                        word_number++;
                        pos++;
                        flag = false;
                    }
                    else if ((lines[line][k] == ' ' || lines[line][k] == '\n' || lines[line][k] == '\0') && flag == false) {
                        flag = true;
                    }
                    if (pos == c[y].pos) {
                		
                        //std::cout << "+++\n";
                        std::pair<int, int> buf = std::make_pair(line - i + 1, word_number);
                        answer.push_back(buf);
                        break;
                    }
                }
                if (pos == c[y].pos) {
                	break;
                }
            }
        }
    }
    c.clear();
    return answer;
}

int main() {
    std::ios::sync_with_stdio(false);
    Node* bor = nullptr;
    std::string str;
    std::string buf; //для выделения подстроки
    std::vector<std::string> lines;
    std::vector<std::pair<int, int> > co;
    while(getline(std::cin, str)) { //построение бора
        lines.push_back(str);
    }
    if (lines.size() > 0) {
        bor = create_bor(lines[0]);
        //выполнение алгоритма Ахо-Корасик по строкам lines от 1 до последней
        co = korasik(bor, lines, 1);
        for (int u = 0; u < co.size(); u++) {
            std::cout << co[u].first << ", " << co[u].second << std::endl;
        }
        bor->deleteall();
        delete bor;
    }
    return 0;
}
