#include "catch.hpp"
#include <iostream>

#define ENABLE_CLASS_TEMPLATE_ARGUMENT_DEDUCTION 1

template <typename ...Ts> struct overloads: Ts...{
#if ENABLE_CLASS_TEMPLATE_ARGUMENT_DEDUCTION == 0
    overloads(Ts&&... ts) : Ts(ts)... {}
#endif
};

#if ENABLE_CLASS_TEMPLATE_ARGUMENT_DEDUCTION > 0
template <typename ...Ts> overloads(Ts...) -> overloads<Ts...>;
#endif


template <typename T>
class Foo{
public:
    Foo(const T& a, const T& b):data(a+b){}
    T data;
};

template <typename T, typename U>
Foo(T,U) -> Foo<std::common_type_t<T,U>>;

TEST_CASE("cpp_language_feature", "CATD") {
    using namespace std;
    
    auto a = overloads{
        []() { cout << "()" << endl; },
        [](int) { cout << "(int)" << endl; },
        [](float) { cout << "(float)" << endl; },
    };
    
    a();
    a(42);
    a(42.0f);

    auto f = Foo(1, 1.1);
}