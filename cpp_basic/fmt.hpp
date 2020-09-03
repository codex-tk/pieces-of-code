#ifndef __cpp_log_h__
#define __cpp_log_h__

#include <chrono>

#include "mp.hpp"

template <typename T>
struct conv {
    template <typename Stream, typename Context>
    static void proc(Stream& s, Context&& c) {
        T::impl(s, c);
    }
};

template <typename... Ts>
struct conv<type_list<Ts...>> {
    template <typename Stream, typename Context>
    static void proc(Stream& s, Context&& c) {
        ((conv<Ts>::proc(s, c)), ...);
    }
};

template <char C>
struct ch {
    template <typename Stream, typename Context>
    static void impl(Stream& s, Context&&) {
        s << C;
    }
};

template <typename ... Ts>
using sb_wrap = type_list<ch<'['>, type_list<Ts...>, ch<']'>>;

struct timestamp{
    template < typename Stream , typename Context >
    static void impl(Stream& s, Context&& ){
        auto tp = std::chrono::system_clock::now();
        std::time_t now = std::chrono::system_clock::to_time_t(tp);
        struct std::tm tm;
#if defined(_WIN32) || defined(__WIN32__)
        localtime_s(&tm, &now);
#else
        localtime_r(&now, &tm);
#endif
        char time_str[32] = {0,};
        snprintf(time_str, 32, "%04d-%02d-%02d %02d:%02d:%02d.%06d", 
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                tm.tm_hour, tm.tm_min, tm.tm_sec,
                static_cast<int>(
                    std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count() % 1000000));
        s << time_str;
    }
};

#endif