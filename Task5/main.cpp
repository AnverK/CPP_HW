#include <iostream>
#include <time.h>
#include <vector>
#include <algorithm>
#include <cassert>
#include "smart_pointers/linked_ptr.h"
#include "smart_pointers/shared_ptr.h"
#include "persistent_set.h"

using std::cout;
using std::endl;
int main()
{
    srand(time(0));
    int cnt = 0;
    while(cnt < 20){
        cout << "Random test #" << cnt+1 << " " << endl;
        int n = 5000 + rand()%10000;
        int max_n = 10*n + rand()%(10*n);
        persistent_set<int, shared_ptr> a;
        for(int i = 0; i < n; i++){
            int b = rand()%max_n;
            a.insert(b);
        }
        persistent_set <int, shared_ptr> b(a), c(a);
        cout << "a is initialized after " << static_cast<double>(clock())/1000000 << " seconds" << endl;

        std::vector <int> v(max_n);
        for(int i = 0; i < max_n; i++){
            v[i] = i;
        }
        random_shuffle(v.begin(), v.end());
        for(int i = 0; i < max_n; i++){
            a.erase(a.find(v[i]));
        }
        cout << "a is clear after " << static_cast<double>(clock())/1000000 << " seconds" << endl;
        while (a.begin() != a.end()) {
            a.erase(--a.end());
        }
        cout << "a is clear after " << static_cast<double>(clock())/1000000 << " seconds" << endl;
        while (b.begin() != b.end()) {
            b.erase(b.begin());
        }
        cout << "b is clear after " << static_cast<double>(clock())/1000000 << " seconds" << endl;
        random_shuffle(v.begin(), v.end());
        for(int i = 0; i < max_n; i++){
            c.erase(c.find(v[i]));
        }
        cout << "c is clear after " << static_cast<double>(clock())/1000000 << " seconds" << endl;
        assert(a.begin() == a.end());
        assert(b.begin() == b.end());
        assert(c.begin() == c.end());
        cnt++;
        cout << endl;
    }


    return 0;
}
