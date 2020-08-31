#ifndef __cpp_type_list_h__
#define __cpp_type_list_h__

template < std::size_t ... S > struct idx_seq{};

template < std::size_t N, std::size_t ... S> struct mk_idx_seq
    : mk_idx_seq<N-1, N-1, S...>{};

template < std::size_t ... S > struct mk_idx_seq<0, S...>{
    using type = idx_seq<S...>;
};

template < typename ... Ts >
struct type_list{
    template < template <typename ...> class U >
    struct rebind{ using type = U<Ts...>;};
};

template < typename T, typename ... Us > struct push_back;

template < template < typename ... > class T,
        typename ... Ts , 
        typename ... Us > struct push_back< T<Ts...>, Us...>
{ using type = T<Ts...,Us...>;};

template < typename T, typename ... Us > struct push_front;

template < template < typename ... > class T,
        typename ... Ts , 
        typename ... Us > struct push_front< T<Ts...>, Us...>
{ using type = T<Us...,Ts...>;};

template < std::size_t I, typename T > struct at;

template <std::size_t I, typename Head, template < typename ... > class T, 
        typename ... Ts> struct at<I, T<Head,Ts...>>
{ using type = typename at<I-1,T<Ts...>>::type; };

template <typename Head, template < typename ... > class T, 
        typename ... Ts> struct at<0, T<Head,Ts...>>
{ using type = Head; };


template < std::size_t I, typename T, typename U> struct _remove_at_impl;

template < std::size_t I, template < typename ... > class T,
        typename Type, typename ... Ts, typename ... Us> 
        struct _remove_at_impl<I, T<Ts...>, T<Type, Us...>>
{
    using type = typename _remove_at_impl<I-1, T<Ts...,Type>, T<Us...>>::type;
};

template < template < typename ... > class T,
        typename Type, typename ... Ts, typename ... Us> 
        struct _remove_at_impl<0, T<Ts...>, T<Type, Us...>>
{
    using type = T<Ts...,Us...>;
};

template < std::size_t I, typename T > struct remove_at;

template < std::size_t I, 
        template < typename ... > class T,
        typename ... Ts > struct remove_at<I, T<Ts...>>{
    using type = typename _remove_at_impl<I, T<>, T<Ts...>>::type;
};



#endif