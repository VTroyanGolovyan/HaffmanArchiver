#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>

using std::string;
using std::vector;
using std::pair;
using std::priority_queue;
using std::ofstream;
using std::ifstream;
using std::cout;

class Hafman{
public:
    int byteValue;
    int priority;
    Hafman* left;
    Hafman* right;
    /* Конструктор */
    Hafman(int byteValue, int priority, Hafman* left, Hafman* right) {
        this->byteValue = byteValue;
        this->priority = 0;
        this->left = left;
        this->right = right;
        this->priority = priority;
    }
    /* Деструктор */
    ~Hafman() {
        if (this->left != nullptr)
            delete this->left;
        if (this->right != nullptr)
            delete this->right;
    }
    /* Получение словаря */
    void getDictionary(Hafman* ht, vector <pair <unsigned char, string> >& dictionary, string code) {
        if (ht->byteValue == -1){
            if (ht->left != nullptr)
                getDictionary(ht->left,dictionary, code + "0");
            if (ht->right != nullptr)
                getDictionary(ht->right,dictionary, code + "1");
        } else {
            dictionary.push_back(pair <int, string> (ht->byteValue, code));
        }
    }
    /* декодирование вектора с помощью итератора */
    int Decode(Hafman* ht, string* s, unsigned long long& iterator) {
        if (ht != nullptr) {
            if (iterator > s->length())
                return -1;
            if (ht->byteValue != -1) {
                return ht->byteValue;
            }
            if (ht->left != nullptr && s->at(iterator) == '0') {
                iterator++;
                return Decode(ht->left, s, iterator);
            }
            if (ht->right != nullptr && s->at(iterator) == '1') {
                iterator++;
                return Decode(ht->right, s, iterator);
            }
            return -1;
        } else return -1;
    }
};
/* компаратор для сравнения деревьев Хафмана */
class cmp{
public:
    bool operator() (Hafman* & a, Hafman* & b) const {
        return a->priority > b->priority;
    }
};
/* Построение дерева Хафмана на основании статистики */
Hafman* BuildTree(int* statistic, int n) {
    priority_queue <Hafman*, vector<Hafman*>, cmp> pq( (cmp()) );
    for (int i = 0; i < n; i++) {
        if (statistic[i] != 0) {
            pq.push(new Hafman(i, statistic[i], nullptr, nullptr));
        }
    }
    while(pq.size() != 1) {
        Hafman* min1 = pq.top();
        pq.pop();
        Hafman* min2 = pq.top();
        pq.pop();
        Hafman* newNode = new Hafman(-1, min1->priority + min2->priority, min1, min2);
        pq.push(newNode);
    }
    Hafman* resTree = pq.top();
    pq.pop();
    return resTree;
}
/* Функция для заполнения массива */
void Fill(int* res, int n, int a) {
    for (int i = 0; i < n; i++)
        res[i] = a;
}
/* Построение статистики вхождения байтов в файл */
void MakeStatistic(vector <unsigned char>& bytes, int *res, int n) {
    Fill(res, n, 0);
    for (int i = 0; i < bytes.size(); i++)
        res[bytes[i]]++;
}
/* Получить код байта из словаря */
string GetCode(vector <pair <unsigned char, string> >& dictionary, unsigned char byte) {
    for (int i = 0; i < dictionary.size(); i++) {
        if (dictionary[i].first == byte) {
            return dictionary[i].second;
        }
    }
    return "";
}
/* Кодирование байтов в биты, представленные ввиде последовательности 0 и 1 */
string* Archive(vector <pair <unsigned char, string> >& dictionary, vector<unsigned char>& bytes) {
    string* result = new string();
    for (int i = 0; i < bytes.size(); i++) {
        *result += GetCode(dictionary, bytes[i]);
    }
    return result;
}
/* Декодирование оригинальной строки из строки кода */
string* DecodeFromString(Hafman* HafmanTree, string *s) {
    unsigned long long i = 0;
    string* res = new string();
    while (i < s->length()) {
        unsigned char ch = HafmanTree->Decode(HafmanTree, s, i);
        if (ch != -1) {
            *res += (unsigned char)ch;
        }
    }
    return res;
}
/* возведение в степень */
unsigned long long pow(int d, int a) {
    unsigned long long res = 1;
    for (int i = 0; i <= d; i++){
        res*=a;
    }
    return res;
}
/* Запись инта в результирующий набор байтов */
void IntToBytes(vector <unsigned char>& bytes, int a, int count) {
    int byte = a % 256;
    if (count != 1)
        IntToBytes(bytes, a / 256, count - 1 );
    bytes.push_back(byte);
}
/* Считывание int из байтов */
int IntFromBytes(vector <unsigned char>& bytes, int start, int count) {
    int res = 0;
    for (int i = start; i < start + count; i++) {
        res+= bytes[i] * pow(count - i + start - 2, 256);
    }
    return res;
}
/* Long Long в байты */
void LongLongToBytes(vector <unsigned char>& bytes, unsigned long long a, int count) {
    int byte = a % 256;
    if (count != 1)
        LongLongToBytes(bytes, a / 256, count - 1 );
    bytes.push_back(byte);
}
/* Long Long из байтов */
unsigned long long LongLongFromBytes(vector <unsigned char>& bytes, int start, int count) {
    unsigned long long res = 0;
    for (int i = start; i < start + count; i++) {
        res+= bytes[i] * pow(count - i + start - 2, 256);
    }
    return res;
}
/*Функция, которая устанавливает бит с номером N в нужное нам значение */
void setNthBit(unsigned char &a, int n, short val) {
    if (val == 1) {
        a |= 1 << (n - 1);
    }
    if (val == 0) {
        a &= ~(1 << (n - 1));
    }
}
/* Функция кодирования строки в байты */
void CodeStringToBytes(string* s, vector <unsigned char>& bytes) {
    int n = s->length() / 8;
    for (int i = 0; i < n; i++) {
        unsigned char byte = 0;
        int j = 0;
        while (j < 8) {
            if (s->at(i * 8 + j) == '0')
                setNthBit(byte, 8 - j, 0);
            else setNthBit(byte, 8 - j, 1);
            j++;
        }
        bytes.push_back(byte);
    }
    if (s->length() % 8 != 0) {
        unsigned char byte = 0;
        int j = 0;
        while (j < s->length() % 8) {
            if (s->at(n * 8 + j) == '0')
                setNthBit(byte, 8 - j, 0);
            else setNthBit(byte, 8 - j, 1);
            j++;
        }
        bytes.push_back(byte);
    }
}
/*Кодирование в байты */
void EncodeToBytes(int* statistic, int n, string* s, vector <unsigned char>& bytes) {
    for (int i = 0; i < n; i++) {
        IntToBytes(bytes, statistic[i], 4);
    }
    unsigned long long size = s->length();
    LongLongToBytes(bytes, size, 8);
    CodeStringToBytes(s, bytes);
}
/* получить n-ый бит вектора, в котором хранятся байты */
int GetNthBit(vector <unsigned char>& bytes, int n) {
    int index = n / 8;
    switch (n % 8) {
        case 0 : {
            return ( (bytes[index] & (1 << 7)) == (1 << 7) ) ? 1 : 0;
            break;
        }
        case 1 : {
            return ( (bytes[index] & (1 << 6)) == (1 << 6) ) ? 1 : 0;
            break;
        }
        case 2 : {
            return ( (bytes[index] & (1 << 5)) == (1 << 5) ) ? 1 : 0;
            break;
        }
        case 3 : {
            return ( (bytes[index] & (1 << 4)) == (1 << 4) ) ? 1 : 0;
            break;
        }
        case 4 : {
            return ( (bytes[index] & (1 << 3)) == (1 << 3) ) ? 1 : 0;
            break;
        }
        case 5 : {
            return ( (bytes[index] & (1 << 2)) == (1 << 2) ) ? 1 : 0;
            break;
        }
        case 6 : {
            return ( (bytes[index] & (1 << 1)) == (1 << 1) ) ? 1 : 0;
            break;
        }
        case 7 : {
            return ( (bytes[index] & (1 << 0)) == (1 << 0) ) ? 1 : 0;
            break;
        }

    }
}
/* Представление последовательности битов из байтов в виде строки */
void CodeStringFromBytes(vector <unsigned char>& bytes, string* CodeString, int start, unsigned long long size) {
    int i = start;
    while (i < start + size) {
        if (GetNthBit(bytes, i) == 0) {
            *CodeString += "0";
        } else {
            *CodeString += "1";
        }
        i++;
    }
}
/* Функция, выполняющая декодирование строки из байтов */
string* HafmanDecode(int n, vector <unsigned char>& bytes) {
    int* statistic = new int[256];
    for (int i = 0; i < n; i++) {
        statistic[i] = IntFromBytes(bytes, i * 4, 4);
    }
    Hafman* HafmanTree = BuildTree(statistic, 256);
    unsigned long long size = LongLongFromBytes(bytes, 256 * 4, 8);
    string* CodeString = new string();
    CodeStringFromBytes(bytes, CodeString, 256 * 4 * 8 + 8 * 8, size);
    string* result = DecodeFromString(HafmanTree, CodeString);
    /* Чистим мусор */
    delete [] statistic;
    delete HafmanTree;
    delete CodeString;
    return result;
}
/* Функция, выполняющая кодирование, записывает результат в вектор*/
void HafmanEncode(vector <unsigned char> &bytes, vector <unsigned char> &result) {
    int* statistic = new int[256];
    MakeStatistic(bytes, statistic, 256);

    Hafman* HafmanTree = BuildTree(statistic, 256);

    vector < pair <unsigned char, string> > dictionary;
    HafmanTree->getDictionary(HafmanTree, dictionary, "");

    string* CodeString = Archive(dictionary, bytes);
    EncodeToBytes(statistic, 256, CodeString, result);
    /* Чистим мусор */
    delete []statistic;
    delete HafmanTree;
    delete CodeString;
}

void BytesFromFile(vector <unsigned char>& data, string name) {
    ifstream fin(name);
    while (!fin.fail()) {
        unsigned char ch;
        ch = fin.get();
        if (ch != EOF) {
            data.push_back((unsigned char) ch);
        }
    }
}

void BytesToFile(vector <unsigned char>& data, string name) {
    ofstream fos(name);
    for (int i = 0; i < data.size(); i++) {
        fos << data[i];
    }
}

void BytesToCharFile(vector <unsigned char>& data, string name) {
    ofstream fos(name);
    for (int i = 0; i < data.size(); i++) {
        fos << data[i];
    }
}

int main() {
    vector <unsigned char> bytes;
    BytesFromFile(bytes, "input.txt");
    vector <unsigned  char> result;
    HafmanEncode(bytes, result);

    BytesToFile(result, "output.txt");
    result.clear();
    BytesFromFile(result, "output.txt");
    string* res = HafmanDecode(256, result);
    vector <unsigned  char> decoded;
    for (int i = 0; i < res->length(); i++)
       decoded.push_back((unsigned char)res->at(i));
    BytesToFile(decoded, "final_output.txt");
    delete res;
    return 0;
}