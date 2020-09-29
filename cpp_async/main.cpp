#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file

#include <iostream>
#include <sstream>
#include <optional>

#include "catch.hpp"

namespace future {
/*
template <typename F, typename NextFut> class then;

template <typename R>
class future{
public:
  using output_type = R;
  future(void) = delete;
  ~future(void) noexcept = default;

  
};


template <typename F, typename NextFut>
class then {
public:

private:

};

*/

template <typename R>
class value_future {
  public:
    using output_type = R;
    value_future(void) = delete;
    ~value_future(void) noexcept = default;
    value_future(R &&v) : _value(std::forward<R>(v)) {}

    std::optional<output_type> poll(void) {
        return _value;
    }

  private:
    R _value;
};

template <typename T>
class add_one_future {
  public:
    using output_type = typename std::remove_reference_t<T>::output_type;

    add_one_future(T&& fut)
      : _future(std::forward<T>(fut)){}

    std::optional<output_type> poll(void) {
      auto val = _future.poll();
      if(val.has_value()){
        return val.value() + 1;
      }
      return std::nullopt;
    }
  private:
    T _future;
};

template <typename R>
static value_future<R> ready(R &&r) {
    return value_future{std::forward<R>(r)};
}

template <typename T>
static add_one_future<T> make_add_one_future(T&& fut) {
    return add_one_future<T>{std::forward<T>(fut)};
}

} // namespace future

TEST_CASE("future_interface", "async") {
    auto f = future::ready(4);
    REQUIRE(f.poll() == 4);
    auto aof = future::make_add_one_future(f);
    REQUIRE(aof.poll() == 5);
}