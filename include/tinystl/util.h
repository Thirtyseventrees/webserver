#ifndef UTIL_H
#define UTIL_H

#include "type_traits.h"

namespace mystl{
    // template <typename T>
    // constexpr T&& forward(T&& t){
    //     return static_cast<T&&>(t);
    // }

    template <typename T>
    constexpr T&&
    forward(typename mystl::remove_reference<T>::type& t){
        return static_cast<T&&>(t);
    }

    template <typename T>
    constexpr T&&
    forward(typename mystl::remove_reference<T>::type&& t){
        return static_cast<T&&>(t);
    }

    template <typename T>
    constexpr typename mystl::remove_reference<T>::type&& move(T&& t){
        return static_cast<typename mystl::remove_reference<T>::type&&>(t);
    }

    template <typename T>
    void swap(T& lhs, T& rhs){
        auto tmp(mystl::move(lhs));
        lhs = mystl::move(rhs);
        rhs = mystl::move(tmp);
    }

    struct compare_less{
        template <typename T>
        bool operator()(const T& lhs, const T& rhs) const{
            return lhs < rhs;
        }
    };

    struct key_of_value{
        template <typename T>
        T operator()(const T& value) const{
            return value;
        }
    };

    template <typename T1, typename T2>
    struct pair{
    
        T1 first;
        T2 second;

        constexpr pair() : first(), second() {};
        constexpr pair(const T1& a, const T2& b) : first(a), second(b) {};
        constexpr pair(T1&& a, T2&& b) : first(mystl::forward<T1>(a)), second(mystl::forward<T2>(b)) {};

        pair(const pair& rhs) = default;
        pair(pair&& rhs) = default;
        ~pair() = default;

        pair& operator=(const pair& rhs){
            first = rhs.first;
            second = rhs.second;
            return *this;
        }

    };

    template <typename T1, typename T2>
    pair<T1, T2> make_pair(T1&& first, T2&& second){
        return pair<T1, T2>(mystl::forward<T1>(first), mystl::forward<T2>(second));
    }
}

#endif