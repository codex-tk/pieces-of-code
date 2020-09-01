#include <iostream>

#include "mp.hpp"

int main(int argc, char* argv[]) {
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

    return 0;
}