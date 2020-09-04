#include "catch.hpp"
#include <vector>
#include <iostream>

template <typename T>
class riter_vec{
public:
    riter_vec(std::vector<T>& c) : _container(c){}
    using iterator = typename std::vector<T>::reverse_iterator;
    iterator begin(void){ return _container.rbegin(); }
    iterator end(void){ return _container.rend(); }
private:
    std::vector<T>& _container;
};

template <typename T>
riter_vec<T> reverse_iter(std::vector<T>& c){ return riter_vec<T>{c};}

TEST_CASE("riter", "freak"){
    std::vector<int> sample{ 0, 1, 2, 3, 4};
    for( int v : reverse_iter(sample)){
        //std::cout << v << std::endl;
    }
}