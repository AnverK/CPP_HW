#include <iostream>
#include "persistent_set.h"
#include <time.h>
#include <set>
using namespace std;

//const long long MAX_NUM = LONG_LONG_MAX;

int main()
{
    persistent_set <int> a;
    int n = 100000;
    for(int i = 0; i < n; i++){
        //long long b = static_cast<long long>(rand()*rand());
        a.insert(rand());
    }
    cout << clock() << endl;
    return 0;
    persistent_set <int> b(a);
    cout << clock() << endl;
    int k = 0;
    while (a.begin() != a.end()) {
        a.erase(a.begin());
        k++;
    }
    cout << clock() << endl;
    while (b.begin() != b.end()) {
        b.erase(b.begin());
        k--;
    }
    cout << clock() << endl;
    cout << (a.begin() == a.end()) << endl;
    cout << (b.begin() == b.end()) << endl;
    cout << k << endl;

    return 0;
}
