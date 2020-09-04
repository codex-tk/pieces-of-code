#include <iostream>

#include "catch.hpp"

template <bool, typename T = void*>
struct enable_if {};

template <typename T>
struct enable_if<true, T> {
    using type = T;
};

template <bool Cond, typename T = void*>
using enable_if_t = typename enable_if<Cond, T>::type;

namespace {
static const std::string float_result("float");
static const std::string int_result("int");
static const std::string general_result("general");
}  // namespace

template <typename T, enable_if_t<
                          !std::is_floating_point_v<T> &&
                          !std::is_integral_v<T>> = nullptr>
std::string foo(const T& val) {
    return general_result;
}

template <typename T, enable_if_t<std::is_floating_point_v<T>> = nullptr>
std::string foo(const T& val) {
    return float_result;
}

template <typename T, enable_if_t<std::is_integral_v<T>> = nullptr>
std::string foo(const T& val) {
    return int_result;
}

TEST_CASE("SFINE", "language_feature") {
    REQUIRE(foo(3.14) == float_result);
    REQUIRE(foo(10) == int_result);
    REQUIRE(foo(std::string{}) == general_result);
}