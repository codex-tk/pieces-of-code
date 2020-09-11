#include "mp.hpp"

#include <iostream>
#include <sstream>
#include <vector>

#include "catch.hpp"
#include "fmt.hpp"

template <typename S, typename T>
void print(S& s, const T& t) {
    s << t;
}

template <typename S, typename... Ts>
void prints(S& s, Ts&&... ts) {
    (print(s, ts), ...);
}

template <typename S, std::size_t... Is, typename... Ts>
void prints(S& s, std::tuple<Ts...>& t, idx_seq<Is...>&&) {
    prints(s, std::get<Is>(t)...);
}

template <typename S, typename... Ts>
void prints(S& s, std::tuple<Ts...>& t) {
    prints(s, t, typename mk_idx_seq<sizeof...(Ts)>::type{});
}

TEST_CASE("variadic_template", "language_feature") {
    std::ostringstream oss;
    prints(oss, 0, 1, 2, 3, 4);
    REQUIRE(oss.str() == "01234");

    auto tp = std::make_tuple("tesla", "nvidia", 3.14);
    std::ostringstream oss0;
    prints(oss0, tp);
    REQUIRE(oss0.str() == "teslanvidia3.14");
}

TEST_CASE("comma_evaluation_order_check", "language_feature") {
    /*
    Unary right fold (E op ...) becomes (E1 op (... op (EN-1 op EN)))
    Unary left fold (... op E) becomes (((E1 op E2) op ...) op EN)
    */
    std::ostringstream unary_right_fold;
    (unary_right_fold << 'a', (unary_right_fold << 'b', unary_right_fold << 'c'));
    std::ostringstream unary_left_fold;
    (((unary_left_fold << 'a', unary_left_fold << 'b'), unary_left_fold << 'c'));
    REQUIRE(unary_left_fold.str() == unary_left_fold.str());
}

TEST_CASE("log_test", "language_feature") {
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
    conv<chars<'a', 'b', 'c', 'd', 'e'>>::proc(ss, nullptr);
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

TEST_CASE("mp_test", "language_feature") {
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

    REQUIRE(type_index<int, type_list<float, int>>::value == 1);
    REQUIRE(type_index<int&, type_list<float, int>>::value == 1);
    REQUIRE(type_index<int[], type_list<float, int*>>::value == 1);
    REQUIRE(type_index<int, type_list<float, float, int>>::value == 2);
}