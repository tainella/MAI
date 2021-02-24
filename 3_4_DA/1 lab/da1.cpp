#include <iostream>
#include <cstring>

const int LWORD = 65;
const int LKEY = 7;

struct TElement {
    char index[LKEY] = {};
    unsigned long long key = 0;
    char word[LWORD] = {};
};

class TVector {
    size_t vSize;
    size_t capacity;
    TElement* data;
public:
    TVector() {
        vSize = 0;
        capacity = 0;
        data = new TElement[2]();
    }
    TVector(size_t s) {
        vSize = 0;
        capacity = s;
        data = new TElement[s]();
    }
    TVector(size_t s, TElement* d) {
        vSize = s;
        capacity = s;
        data = new TElement[s]();
        if (s != 0) {
            for (size_t i = 0; i < s; i++) {
                data[i] = d[i];
            }
        }
    }
    
    TElement& operator[] (size_t i) {
        return data[i];
    }
    size_t Size() {
        return vSize;
    }
    
    size_t Capacity() {
        return capacity;
    }
    void Resize() {
        if (capacity == 0) {
            capacity = 1;
        }
        capacity *= 2;
        TElement* temp = new TElement[capacity]();
         if(temp == 0x0 ) {
             exit(-1);
         }
        for (size_t i = 0; i < vSize; i++) {
            temp[i] = data[i];
        }
        delete [] data;
        data = temp;
    }
    void PushBack(TElement elem) {
        if (vSize == capacity) {
            Resize();
        }
        data[vSize++] = elem;
    }
    ~TVector() {
        delete [] data;
    }
};

void CountingSort(TVector &v) {
    TVector temp(v.Size());
    unsigned long long max = v[0].key;
    int i;
    for (size_t j = 1; j < v.Size(); j++) {
        if (v[j].key > max) {
            max = v[j].key;
        }
    }
    max++;
    TVector c((size_t)max);
    for (size_t j = 0; j < c.Capacity(); j++) {
        c[j].key = 0;
    }
    for (size_t j = 0; j < v.Size(); j++) {
        ++c[v[j].key].key;
    }
    for (size_t j = 1; j < c.Capacity(); j++) {
        c[j].key = c[j].key + c[j - 1].key;
    }
    for (int j = v.Size() - 1; j >= 0; j--) {
        c[v[j].key].key--;
        temp[c[v[j].key].key] = v[j];
    }
    for (size_t m = 0; m < v.Size(); m++) {
        v[m] = temp[m];
    }
}

int main(void) {
    std::ios::sync_with_stdio(false);
    
    TVector dataVector;
    TElement element;
    size_t i = 0;
    char str[72] = {};
    while (true) {
        if (std::cin.eof()) {
            break;
        }
        str[0] = '\0';
        std::cin.getline(str, 72);
         if (str[0] != '\0') {
            element.key = 0;
            for (i = 0; i < 6; i++) {
                element.index[i] = str[i];
                element.key = element.key * 10 + str[i] - '0';
            }
            i++;
            for (; i < 72; i++) {
                if (str[i] == '\0') {
                    break;
                }
                element.word[i - 7] = str[i];
            }
            element.word[64] = '\0';
            element.index[6] = '\0';
            dataVector.PushBack(element);
            for (i = 0; i < 72; i++) {
                str[i] = 0;
            }
             for (i = 0; i < 64; i++) {
                 element.word[i] = 0;
             }
             for (i = 0; i < 6; i++) {
                 element.index[i] = 0;
             }
        }
        
    }
    if (dataVector.Size() != 0) {
        CountingSort(dataVector);
        for (size_t j = 0; j < dataVector.Size(); j++) {
            std::cout << dataVector[j].index << "\t"<<  dataVector[j].word << std::endl;
        }
    }
    return 0;
}
