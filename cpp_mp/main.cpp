#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <iostream>
#include <sstream>

#include "catch.hpp"
#include "fmt.hpp"
#include "mp.hpp"

void log_tests() {
    std::stringstream ss;
    conv<ch<'s'>>::proc(ss, nullptr);
    std::cout << ss.str() << std::endl;
    conv<
        type_list<
            ch<'0'>, ch<'1'>, ch<'2'>, ch<'3'>>>::proc(ss, nullptr);
    std::cout << ss.str() << std::endl;
    ss = std::stringstream();
    conv<timestamp>::proc(ss, nullptr);
    std::cout << ss.str() << std::endl;

    ss = std::stringstream();
    conv<type_list<
        ch<'['>, timestamp, ch<']'>>>::proc(ss, nullptr);
    std::cout << ss.str() << std::endl;

    ss = std::stringstream();
    conv<sb_wrap<timestamp>>::proc(ss, nullptr);
    std::cout << ss.str() << std::endl;

    ss = std::stringstream();
    conv<
        type_list<sb_wrap<timestamp>, sb_wrap<timestamp>>>::proc(ss, nullptr);
    std::cout << ss.str() << std::endl;
}

void mp_tests();

TEST_CASE("mp_test", "[mp]") {
    mp_tests();
    REQUIRE(type_index<int, type_list<float, int>>::value == 1);
    REQUIRE(type_index<int, type_list<float, float, int>>::value == 2);
}

TEST_CASE("log_test", "[log]") {
    log_tests();
}

struct local {
    template <typename... Ts>
    static void print(Ts&&... args) {
        ((std::cout << args << " "), ...);
        std::cout << std::endl;
    }

    template <std::size_t... Is, typename... Ts>
    static void print_tuple(idx_seq<Is...>&&, std::tuple<Ts...>& args) {
        local::print(std::get<Is>(args)...);
    }

    template <typename... Ts>
    static void print(std::tuple<Ts...>& args) {
        local::print_tuple(
            typename mk_idx_seq<sizeof...(Ts)>::type{},
            args);
    }
};

TEST_CASE("idx_seq_tuple", "[mp]") {
    local::print(0, "a", 3.14);
    auto args = std::make_tuple("tesla", "nvidia", 3.14);
    local::print(args);
}

void mp_tests(void) {
    static_assert(
        std::is_same<
            mk_idx_seq<4>::type,
            idx_seq<0, 1, 2, 3>>::value);

    static_assert(
        std::is_same<
            type_list<int, int, int>::rebind<std::tuple>::type,
            std::tuple<int, int, int>>::value);

    static_assert(
        std::is_same<
            push_back<type_list<int>, float>::type,
            type_list<int, float>>::value);

    static_assert(
        std::is_same<
            push_front<type_list<int, int>, float>::type,
            type_list<float, int, int>>::value);

    static_assert(
        std::is_same<
            at<0, type_list<int, float, double>>::type,
            int>::value);

    static_assert(
        std::is_same<
            at<2, type_list<int, float, double>>::type,
            double>::value);

    static_assert(
        std::is_same<
            remove_at<0, type_list<int, float>>::type,
            type_list<float>>::value);

    static_assert(
        std::is_same<
            pop_back<type_list<int, float, double>>::type,
            type_list<int, float>>::value);

    static_assert(
        std::is_same<
            pop_front<type_list<int, float, double>>::type,
            type_list<float, double>>::value);
}