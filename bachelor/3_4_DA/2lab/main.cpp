#include <cstring>
#include <iostream>


class TPatricaia {
    public:
        TPatricaia(): root(nullptr){}
        unsigned long long Find(char* key) {
            if(root) {
                Node* tmp;
                if (root->left == root) {
                    tmp = root;
                } else {
                    tmp = root->left->Find(key);
                }
                if (strcmp(tmp->key, key) == 0) {
                    return tmp->value;
                }
            }
            throw "No Elem";
        }
        bool Add(char* key, unsigned long long value) {
            if(root) {
                Node* tmp;
                if (root->left == root) {
                    tmp = root;
                } else {
                    tmp = root->left->Find(key);
                }

                int NewBit = getFirstDiffBit(key, tmp->key);
                //std::cout << NewBit << '\n';

                if (NewBit == -1) {
                    return false;
                }
                Node** child;
                if (root->left == root || root->left->Nbit > NewBit) {
                    tmp = root;
                    child = &(tmp->left);
                } else {
                    tmp = root->left->Find(key, NewBit);
                    if (getNBit(key, tmp->Nbit)) {
                        child = &(tmp->left);
                    } else {
                        child = &(tmp->right);
                    }
                }
        
                Node* NewNode = new Node(key, value, NewBit);
                NewNode->parantPtr = child;
                if (getNBit(NewNode->key, NewNode->Nbit)) {
                    NewNode->left = NewNode;
                    NewNode->right = *child;
                    if ((*child)->Nbit > NewNode->Nbit) {
                        (*child)->parantPtr = &(NewNode->right);
                    }
                } else {
                    NewNode->right = NewNode;
                    NewNode->left = *child;
                    if ((*child)->Nbit > NewNode->Nbit) {
                        (*child)->parantPtr = &(NewNode->left);
                    }
                }
                *(NewNode->parantPtr) = NewNode;
                return true;
            } else {
                root = new Node(key, value, -1);
                root->right = nullptr;
                root->left = root;
                return true;
            }
        }
        bool Del(char* key) {
            if(root) {
                Node* tmp;
                if (root->left == root) {
                    if (strcmp(root->key, key) == 0) {
                        delete root;
                        root = nullptr;
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    tmp = root->left->Find(key,  __INT_MAX__ - 1);
                    Node** child;
                    
                    if (getNBit(key, tmp->Nbit)) {
                        child = &(tmp->left);
                    } else {
                        child = &(tmp->right);
                    }
                    if (strcmp((*child)->key, key) != 0) {
                        return false;
                    }
                    if ((*child) != root) {
                        if ((*child) == tmp) {
                            Node* NormalChild;
                            if (getNBit(key, tmp->Nbit)) {
                                NormalChild = tmp->right;
                            } else {
                                NormalChild = tmp->left;
                            }
                            if (NormalChild->Nbit > tmp->Nbit) {
                                NormalChild->parantPtr = tmp->parantPtr;
                            }
                            *(tmp->parantPtr) = NormalChild;
                            tmp->left = nullptr;
                            tmp->right = nullptr;
                            delete tmp;
                            return true;

                        }
                    }
                    

                    //Case 2
                    strcpy((*child)->key, tmp->key);
                    (*child)->value = tmp->value;

                    Node* prev = tmp->Find(tmp->key,  __INT_MAX__ - 1);
                    if (getNBit(tmp->key, prev->Nbit)) {
                        prev->left = *child;
                    } else {
                        prev->right = *child;
                    }
                    
                    Node* NormalChild;
                    if (getNBit(key, tmp->Nbit)) {
                        NormalChild = tmp->right;
                    } else {
                        NormalChild = tmp->left;
                    }

                    if (NormalChild->Nbit > tmp->Nbit) {
                        NormalChild->parantPtr = tmp->parantPtr;
                    }
                    *(tmp->parantPtr) = NormalChild;
                    tmp->left = nullptr;
                    tmp->right = nullptr;
                    delete tmp;
                    return true;
                }

            } else {
                return false;
            }
        }

        bool Save(char* path) {
            FILE* stor = fopen(path, "wb");
            if (!stor) {
                return false;
            }
            if (root) {
                root->Save(stor);
            }
            int s = 0;
            fwrite(&s, sizeof(int), 1, stor);
            fclose(stor);
            return true;
        }
        bool Load(char* path) {
            FILE* stor = fopen(path, "rb");
            if (!stor) {
                return false;
            }
            if(root){
                delete root;
                root = nullptr;
            }
            int flag;
            char key[257] = {0};
            unsigned long long value;
            size_t res;
            res = fread(&flag, sizeof(int), 1, stor);
            if (res != 1) {
            	return false;
            }
            while (flag) {
                res = fread(key, sizeof(char) * 257, 1, stor);
                if (res != 1) {
            		return false;
            	}
                res = fread(&value, sizeof(unsigned long long), 1, stor);
                if (res != 1) {
            		return false;
            	}
                Add(key, value);
                res = fread(&flag, sizeof(int), 1, stor);
                if (res != 1) {
            		return false;
            	}
            }
            return true;
        }

        ~TPatricaia(){
            if(root){
                delete root;
            }
        }

    private:
        static int getFirstDiffBit(char* a, char* b) {
            int out = 0;
            int i = 0;
            while(a[i] != '\0' && b[i] != '\0') {
                if (a[i] == b[i]) {
                    out += 8;
                } else {
                    for (int k = 0; k < 8; k++) {
                        if ((a[i] & (1u<<k)) == (b[i] & (1u<<k))) {
                            out += 1;
                        } else {
                            return out;
                        }
                    }
                }
                i++;
            }
            for (int k = 0; k < 8; k++) {
                if ((a[i] & (1u<<k)) == (b[i] & (1u<<k))) {
                    out += 1;
                } else {
                    return out;
                }
            }
            return -1;
        }
        static int getNBit(char* InKey, int NBit) {
            return (bool)(InKey[(int)(NBit/8)] & (1u << (NBit % 8)));
        }
        struct Node {
            char key[257];
            unsigned long long value;
            int Nbit;
            Node* left;
            Node* right;
            Node** parantPtr;

            Node(char* InKey,unsigned long long value,  int NBit): key{0}, value(value),  Nbit(NBit){
                strcpy(key, InKey);
            }

            Node* Find(char* InKey, int NewBit = __INT_MAX__) {
                if (getNBit(InKey, Nbit)) {
                    if (left->Nbit > NewBit) {
                        return this;
                    }
                    if (left->Nbit <= Nbit) {
                        if (NewBit == __INT_MAX__) {
                            return left;
                        } else {
                            return this;
                        }
                    }
                    return left->Find(InKey, NewBit);
                } else {
                    if (right->Nbit > NewBit) {
                        return this;
                    }
                    if (right->Nbit <= Nbit) {
                        if (NewBit == __INT_MAX__) {
                            return right;
                        } else {
                            return this;
                        }
                    }
                    return right->Find(InKey, NewBit);
                }
            }
            int Save(FILE* stor) {
                int s = 1;
                fwrite(&s, sizeof(int), 1, stor);
                fwrite(key, sizeof(char) * 257, 1, stor);
                fwrite(&value, sizeof(unsigned long long), 1, stor);
                if (left) {    
                    if (left->Nbit > Nbit) {
                        left->Save(stor);
                    }
                }
                if (right) {
                    if (right->Nbit > Nbit) {
                        right->Save(stor);
                    }
                }
                return true;
            }
            ~Node(){
                if (left) {
                    if (left->Nbit > Nbit) {
                        delete left;
                    }
                }
                if (right) {
                    if (right->Nbit > Nbit) {
                        delete right;
                    }
                }
            }
        };

        Node* root;
};

void tolow(char* a) {
    for (int i = 0; a[i] != '\0'; i++) {
        if (a[i] >= 'A' && a[i] <= 'Z') {
            a[i] = tolower(a[i]);
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    char comm[50];
    char key[257] = {'\0'};
    unsigned long long value;

    TPatricaia stor;

    while(std::cin >> comm) {
        if (comm[0] == '+') {
            std::cin >> key >> value;
            tolow(key);
            if(stor.Add(key,value)) {
                puts("OK");
            } else {
                puts("Exist");
            }
        } else if (comm[0] == '-') {
            std::cin >> key;
            tolow(key);
            if(stor.Del(key)) {
                puts("OK");
            } else {
                puts("NoSuchWord");
            }
        } else if (comm[0] == '!') {
            std::cin >> comm >> key;
            if (comm[0] == 'S') {
                if(stor.Save(key)) {
                    puts("OK");
                } else {
                    puts("ERROR");
                }
            } else if (comm[0] == 'L') {
                if(stor.Load(key)) {
                    puts("OK");
                } else {
                    puts("ERROR");
                }
            }
        } else {
            try {
                tolow(comm);
                value = stor.Find(comm);
                printf("OK: %llu\n", value);
            } catch (...) {
                puts("NoSuchWord");
            }
        }
    }
    return 0;
}
