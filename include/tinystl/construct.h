#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include "util.h"

namespace mystl{
    template <class T>
    void construct(T* ptr){
        ::new((void*)ptr) T();
    }

    template <class T, class V>
    void construct(T* ptr, const V& value){
        ::new((void*)ptr) T(value);         //placement new, constructs a object at ptr instead of getting new memory and constructing a object.
    }

    template <class T, class... Args>
    void construct(T* ptr, Args&&... args){
        ::new((void*)ptr) T(mystl::forward<Args>(args)...);       //placement new
    }

    template<class T>
    void destroy(T* ptr){
        if(ptr != nullptr && mystl::is_destructible<T>::value){
            ptr->~T();
        }
    }

    template <typename iter>
    void destroy(iter first, iter last){
        for(;first != last; first++){
            destroy(first);
        }
    }
}

#endif