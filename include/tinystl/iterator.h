#ifndef ITERATOR_H
#define ITERATOR_H

#include "type_traits.h"

namespace mystl{
    //Five types of iterator
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    template <typename Category, typename T, typename Distance = long int, typename Pointer = T*, typename Reference = T&>
    struct iterator{
        //The type of iterator
        using iterator_category = Category;
        using value_type = T;
        using difference_type = Distance;
        using pointer = Pointer;
        using reference = Reference;
    };

    template <typename iter>
    struct iterator_traits{
        using iterator_category = typename iter::iterator_category;
        using value_type = typename iter::value_type;
        using difference_type = typename iter::difference_type;
        using pointer = typename iter::pointer;
        using reference = typename iter::reference;
    };

    template <typename T>
    struct iterator_traits<T*>{
        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using difference_type = long int;
        using pointer = T*;
        using reference = T&;
    };

    //traits:
    //is_iterator:
    template <typename T, typename = void>
    struct is_iterator : false_type {};

    template <typename T>
    struct is_iterator<T, decltype(*(mystl::declval<T>()) = 0, void())> : true_type {};

    template <typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag){
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while(first != last){
            ++first;
            ++n;
        }
        return n;
    };

    template <typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last, random_access_iterator_tag){
        return last - first;
    };

    template <typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last){
        return distance_dispatch(first, last, typename iterator_traits<InputIterator>::iterator_category());
    };


    template <typename iter, typename distance>
    void advance_dispatch(iter& i, distance n, input_iterator_tag){
        while(n--)
            ++i;
    };

    template <typename iter, typename distance>
    void advance_dispatch(iter& i, distance n, bidirectional_iterator_tag){
        if(n >= 0)
            while(n--)
                ++i;
        else
            while(n++)
                --i;
    };
    
    template <typename iter, typename distance>
    void advance_dispatch(iter& i, distance n, random_access_iterator_tag){
        i += n;
    };

    template <typename iter, typename distance>
    void advance(iter& i, distance n){
        advance_dispatch(i, n, typename iterator_traits<iter>::iterator_category());
    };

}

#endif