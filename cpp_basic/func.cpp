#include <iostream>

#include "catch.hpp"

template <typename T>
struct func;

template <typename R, typename... Ts>
struct func<R(Ts...)> {
    struct handler {
        handler(void) = default;
        virtual ~handler(void) = default;
        virtual R operator()(Ts&&... ts) = 0;
    };

    template <typename H>
    struct handler_imp : handler {
        handler_imp(H&& h) : _raw_handler(std::forward<H>(h)) {}
        virtual ~handler_imp(void) = default;
        virtual R operator()(Ts&&... ts) override {
            return _raw_handler(std::forward<Ts>(ts)...);
        }
        H _raw_handler;
    };

    template <typename T, std::enable_if_t<
                              !std::is_same_v<func, std::decay_t<T>>,
                              std::nullptr_t> = nullptr>
    explicit func(T&& t)
        : _handler(std::make_shared<handler_imp<T>>(std::forward<T>(t))) {
        is_construct = true;
        is_copy_construct = false;
    }

    explicit func(const func& rhs) : _handler(rhs._handler) {
        is_construct = false;
        is_copy_construct = true;
    }

    R operator()(Ts&&... ts) {
        return (*_handler)(std::forward<Ts>(ts)...);
    }

    std::shared_ptr<handler> _handler;

   public:
    bool is_construct;
    bool is_copy_construct;
};

TEST_CASE("func", "lib_feature") {
    func<void()> fn([] {
        //std::cout << "test inner" << std::endl;
    });
    func<void()> cp_fn{fn};
    REQUIRE(fn.is_construct);
    REQUIRE(cp_fn.is_copy_construct);
}