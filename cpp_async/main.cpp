#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file

#include <iostream>
#include <sstream>

#include "catch.hpp"

namespace future {

template <typename R>
class value_future {
  public:
    value_future(void) = delete;
    ~value_future(void) noexcept = default;
    value_future(R &&v) : _value(std::forward<R &&>(v)) {}

    R poll(void) {
        return _value;
    }

  private:
    R _value;
};

template <typename R>
static value_future<R> ready(R &&r) {
    return value_future{std::forward<R>(r)};
}
} // namespace future

TEST_CASE("future_interface", "async") {
    auto f = future::ready(4);
    REQUIRE(f.poll() == 4);
}