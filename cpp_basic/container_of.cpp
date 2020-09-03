#include <cstddef>

#include "catch.hpp"

// [from boost::intrinsic][[
template <class Parent, class Member>
std::ptrdiff_t offset_of(const Member Parent::*ptr_to_member) {
    const Parent *const parent = nullptr;
    const char *const member = static_cast<const char *>(
        static_cast<const void *>(&(parent->*ptr_to_member)));
    std::ptrdiff_t val(
        member - static_cast<const char *>(static_cast<const void *>(parent)));
    return val;
}

template <class Parent, class Member>
inline Parent *container_of(Member *member,
                            const Member Parent::*ptr_to_member) {
    return static_cast<Parent *>(
        static_cast<void *>(static_cast<char *>(static_cast<void *>(member)) -
                            offset_of(ptr_to_member)));
}

template <class Parent, class Member>
inline const Parent *container_of(const Member *member,
                                  const Member Parent::*ptr_to_member) {
    return static_cast<const Parent *>(static_cast<const void *>(
        static_cast<const char *>(static_cast<const void *>(member)) -
        offset_of(ptr_to_member)));
}
// ]]

TEST_CASE("lib_feature", "container_of") {
#ifdef WIN32
    #pragma pack(push,1)
    struct foo{
        int bar;
        void* context;
    };
    #pragma pack(pop)

    foo contained;
    void** ctx_ptr = &(contained.context);
    foo* ptr_contained = container_of(ctx_ptr, &foo::context);

    REQUIRE(offset_of(&foo::context) == sizeof(foo::bar));   
    REQUIRE(ptr_contained == &contained);

#endif
}