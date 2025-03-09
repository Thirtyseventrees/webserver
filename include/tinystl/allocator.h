#ifndef MYTINYSTL_ALLOCATOR
#define MYTINYSTL_ALLOCATOR

#include "construct.h"
#include "util.h"

#include <cstddef>

namespace mystl{
    template <typename T>
    class allocator{
        public:
            using value_type = T;
            using pointer = T*;
            using const_pointer = const T*;
            using reference = T&;
            using const_reference = const T&;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;

            static T* allocate();
            static T* allocate(size_type n);

            static void deallocate(T* p);
            static void deallocate(T* p, size_type n);

            static void construct(T* ptr);
            static void construct(T* ptr, const T& value);      //copy construct
            static void construct(T* prt, T&& value);           //move construct
            template <typename... Args>
            static void construct(T* ptr, Args&&... args);

            static void destroy(T* ptr);
            static void destroy(T* first, T* last);
            template <typename iter>
            static void destroy(iter first, iter last);
    };

    template <typename T>
    T* allocator<T>::allocate(){
        return static_cast<T*>(::operator new(sizeof(T)));
    }

    template <typename T>
    T* allocator<T>::allocate(size_type n){
        if(n == 0)
            return nullptr;
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    template<typename T>
    void allocator<T>::deallocate(T* ptr){
        if(ptr == nullptr)
            return;
        ::operator delete(ptr);
    }

    template<typename T>
    void allocator<T>::deallocate(T* ptr, size_type n){
        if(ptr == nullptr)
            return;
        ::operator delete(ptr, n * sizeof(T));
    }

    template<typename T>
    void allocator<T>::construct(T* ptr){
        mystl::construct(ptr);
    }

    template<typename T>
    void allocator<T>::construct(T* ptr, const T& value){
        mystl::construct(ptr, value);
    }

    template<typename T>
    void allocator<T>::construct(T* ptr, T&& value){
        mystl::construct(ptr, mystl::move(value));
    }

    template<typename T>
    template<typename... Args>
    void allocator<T>::construct(T* ptr, Args&& ...args){
        mystl::construct(ptr, mystl::forward<Args>(args)...);
    }

    template<typename T>
    void allocator<T>::destroy(T* ptr){
        mystl::destroy(ptr);
    }

    template <typename T>
    void allocator<T>::destroy(T* first, T* last){
        mystl::destroy(first, last);
    }

    template <typename T>
    template <typename iter>
    void allocator<T>::destroy(iter first, iter last){
        mystl::destroy(first, last);
    }

}

#endif