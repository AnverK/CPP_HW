#include <iostream>
#include "my_list.h"
using namespace std;

int main()
{
    my_list <int> l;
    for(int i = 0; i < 10; i++)
    {
        l.push_back(i);
    }
//    my_list<int> l2;
//    l2 = l;

//    l2.splice(++++++l2.begin(), l, ++++++++++l.begin(), l.end());
//    for(auto el:l2)
//    {
//        cout << el << endl;
//    }
//    for(my_list<int>::reverse_iterator it = l.rbegin(); it != l.rend(); it++)
//    {
//        cout << *it << endl;
//    }
    return 0;
}

