#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

namespace mystl{
    
    //Remove reference
    template <typename T>
    struct remove_reference{
        using type = T;
    };

    template <typename T>
    struct remove_reference<T&>{
        using type = T;
    };
    
    template<typename T>
    struct remove_reference<T&&>{
        using type = T;
    };
    
    template <typename T, T v>
    struct integral_constant{
        static constexpr T value = v;
    };

    using true_type = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;

    template <bool B, typename = void>
    struct enable_if {};

    template <typename T>
    struct enable_if<true, T>{
        using type = T;
    };

    template <typename...>
    using void_t = void;
    
    //Return a virtual object of T
    template <typename T>
    constexpr T&& declval() noexcept;

    //is_destructible
    template <typename T, typename = void>
    struct is_destructible : false_type {};

    template <typename T>
    struct is_destructible<T, decltype(mystl::declval<T>().~T(), void())> : true_type {};

    //is_copy_assignable
    template <typename T, typename = void>
    struct is_copy_assignable : false_type {};

    template <typename T>
    struct is_copy_assignable<T, decltype(mystl::declval<T>() = mystl::declval<T const &>(), void())> : true_type {};

    //is_move_assignable
    template <typename T, typename = void>
    struct is_move_assignable : false_type {};

    template <typename T>
    struct is_move_assignable<T, decltype(mystl::declval<T>() = mystl::declval<T&&>(), void())> : true_type {};

    //is_rvalue_reference
    template <typename T>
    struct is_rvalue_reference : false_type {};

    template <typename T>
    struct is_rvalue_reference<T&&> : true_type {};

    //is_the_same_type
    template <typename T1, typename T2>
    struct is_the_same_type : false_type {};

    template <typename T>
    struct is_the_same_type<T, T> : true_type {};

}

#endif