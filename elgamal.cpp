#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>
#include <unordered_set>
#include <sstream>
#include <codecvt>
#include <fstream>

using namespace std;

class PublicKey {
public:
    long long p;
    long long g;
    long long y;

    PublicKey(long long _p, long long _g, long long _y) {
        p = _p;
        g = _g;
        y = _y;
    }
};

class PrivateKey {
public:
    long long p;
    long long x;

    PrivateKey(long long _x, long long _p) {
        x = _x;
        p = _p;
    }
};

bool isPrime(long long number) {
    if (number == 0) {
        return false;
    }
    for (int i = 2; i < number; i++) {
        if (number % i == 0) {
            return false;
        }
    }
    return true;
}

long long findModularRemainder(long long base, long long exponent, long long mod) {
    long long x = 1;
    long long y = base;

    while (exponent > 0) {
        if (exponent % 2 == 1) {
            x = (x * y) % mod;
        }
        y = (y * y) % mod;
        exponent = exponent / 2;
    }

    return x % mod;
}

long long mul(long long a, long long b, long long n) {
    int sum = 0;

    for (int i = 0; i < b; i++) {
        sum += a;

        if (sum >= n) {
            sum -= n;
        }
    }

    return sum;
}

void findPrimeFactors(unordered_set<int> &primeFactorsSet, long long number) {
    while (number % 2 == 0) {
        primeFactorsSet.insert(2);
        number = number / 2;
    }

    for (int i = 3; i <= sqrt(number); i = i + 2) {
        while (number % i == 0) {
            primeFactorsSet.insert(i);
            number = number / i;
        }
    }

    if (number > 2) {
        primeFactorsSet.insert(number);
    }
}

int findPrimitiveRoot(long long p) {
    unordered_set<int> primeFactorsSet = unordered_set<int>();

    if (!isPrime(p)) {
        return -1;
    }

    long long eulerPhi = p - 1;

    findPrimeFactors(primeFactorsSet, eulerPhi);

    for (long long primitiveRoot = 2; primitiveRoot <= eulerPhi; primitiveRoot++) {
        bool flag = false;

        for (int primeFactor : primeFactorsSet) {
            if (findModularRemainder(primitiveRoot, eulerPhi / primeFactor, p) == 1) {
                flag = true;
                break;
            }
        }

        if (!flag) {
            return primitiveRoot;
        }
    }

    return -1;
}

long long findPrime(long long min, long long max) {
    long long range = max - min + 1;

    while (true) {
        long long randomPrime = rand() % range + min;

        if (isPrime(randomPrime)) {
            return randomPrime;
        }
    }
}

tuple<PublicKey, PrivateKey> generateKeys() {
    long long p = findPrime(100000, 1000000);
    cout << "Выбрано простое число P: " << p << endl;

    long long g = findPrimitiveRoot(p);
    cout << "Вычислен первообразный корень G числа P: " << g << endl;

    long long x = rand() % (long long) ((p - 1) / 2) + 1;
    cout << "Выбрано взаимно простое с P - 1 число X: " << x << endl;

    long long y = findModularRemainder(g, x, p);
    cout << "Вычислено число Y: " << y << endl << endl;

    PublicKey publicKey(p, g, y);
    cout << "Собран открытый ключ (Y = " << y << ", G = " << g << ", P = " << p << ")" << endl;

    PrivateKey privateKey(x, p);
    cout << "Собран закрытый ключ (X = " << x << ", P = " << p << ")" << endl << endl;

    return make_tuple(publicKey, privateKey);
}

string encrypt(PublicKey publicKey, vector<char> text) {
    long long p = publicKey.p;
    long long g = publicKey.g;
    long long y = publicKey.y;

    string encryptedText;

    for (auto &symbol : text) {
        if (symbol > 0) {
            long long k = rand() % ((p - 1) / 2) + 1;
            long long a = findModularRemainder(g, k, p);
            long long b = mul(findModularRemainder(y, k, p), symbol, p);

            encryptedText += to_string(a) + " " + to_string(b) + " ";
        }
    }

    return encryptedText;
}

string decrypt(PrivateKey pk, vector<vector<long long>> pairs) {
    long long p = pk.p;
    long long x = pk.x;

    string decryptedText;

    for (auto &pair : pairs) {
        long long a = pair[0];
        long long b = pair[1];

        if (a != 0 && b != 0) {
            int decryptedSymbol = mul(b, findModularRemainder(a, p - 1 - x, p), p);

            decryptedText += (char) decryptedSymbol;
        }
    }

    return decryptedText;
}

int main() {
    cout << "Добро пожаловать в мир Эль-Гамаля!" << endl << endl;

    tuple<PublicKey, PrivateKey> keys = generateKeys();

    string text;
    ifstream inputFileStream;
    inputFileStream.open("../input.txt");

    stringstream stringStream;
    stringStream << inputFileStream.rdbuf();

    text = stringStream.str();

    vector<char> bytes(text.begin(), text.end());

    string encryptedText = encrypt(get<0>(keys), bytes);

    cout << "Зашифрованное сообщение: " << endl;
    cout << encryptedText << endl << endl;

    vector<vector<long long>> pairs;
    vector<long long> values;

    stringstream stream(encryptedText);
    long long n;
    while (stream >> n) {
        values.push_back(n);
    }

    for (vector<int>::size_type i = 0; i != values.size(); i = i + 2) {
        long long a = values[i];
        long long b = values[i + 1];

        vector<long long> pair;
        pair.push_back(a);
        pair.push_back(b);

        pairs.push_back(pair);
    }

    string decryptedText = decrypt(get<1>(keys), pairs);

    cout << "Расшифрованное сообщение: " << endl;
    cout << decryptedText << endl;

    return 0;
}
