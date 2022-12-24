/// @file
/// @brief Файл с реализацией основного потока приложения

#include <iostream>
#include <map>
#include "allocator.h"
#include "factorial.h"
#include "simple_fwdlist.h"

int main()
{
    try
    {
        {
            std::map<int, int> map;
            for (int i = 0; i < 10; ++i)
            {
                map.insert(std::make_pair(i, factorial(i)));
            }
        }
        {
            std::map<int, int, std::less<int>, FixedAllocator<std::pair<int, int>, 10>> mymap;
            for (int i = 0; i < 10; ++i)
            {
                mymap.insert(std::make_pair(i, factorial(i)));
            }
            for (const auto& p : mymap)
            {
                std::cout << p.first << " " << p.second << "\n";
            }
        }
        {
            SimpleFwdList<int> mylist;
            auto it = mylist.before_begin();
            for (int i = 1; i < 10; ++i)
            {
                it = mylist.insert_after(it, i);
            }
        }
        {
            SimpleFwdList<int, FixedAllocator<int, 10>> mylist;
            auto it = mylist.before_begin();
            for (int i = 1; i < 10; ++i)
            {
                it = mylist.insert_after(it, i);
            }
            for (const auto& n : mylist)
            {
                std::cout << n << "\n";
            }
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
