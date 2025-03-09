#ifndef QUEUE_H
#define QUEUE_H

#include "algobase.h"
#include "allocator.h"
#include "iterator.h"
#include "deque.h"

namespace mystl{

    template <typename T, typename Sequence = mystl::deque<T>>
    class queue{
        public:
        using value_type = typename Sequence::value_type;
        using size_type = typename Sequence::size_type;
        using pointer = typename Sequence::pointer;
        using const_pointer = typename Sequence::const_pointer;
        using reference = typename Sequence::reference;
        using const_reference = typename Sequence::const_reference;
        using self = queue<T, Sequence>;

        private:
        Sequence _c;

        public:
        //construct
        queue() = default;

        queue(size_type n) : _c(n) {};

        queue(size_type n, const value_type& value) : _c(n, value) {};

        template <typename iter, 
        typename mystl::enable_if<mystl::is_iterator<iter>::value, int>::type = 0,
        typename mystl::enable_if<mystl::is_the_same_type<typename mystl::iterator_traits<iter>::value_type, value_type>::value, int>::type = 0>
        queue(iter first, iter last) : _c(first, last) {};

        ~queue() = default;

        bool empty() const {
            return _c.empty();
        }

        size_type size() const {
            return _c.size();
        }

        reference front(){
            return _c.front();
        }

        reference back(){
            return _c.back();
        }

        template <typename ...Args>
        void emplace(Args&& ...args){
            _c.emplace_back(mystl::forward<Args>(args)...);
        }

        void push(const value_type& value){
            _c.push_back(value);
        }

        void pop(){
            _c.pop_front();
        }

        bool operator==(const self& rhs){
            return _c == rhs._c;
        }

        void swap(self& rhs){
            _c.swap(rhs._c);
        }

    };
    
}


#endif