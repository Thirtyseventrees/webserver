#ifndef DEQUE_H
#define DEQUE_H

#ifndef DEQUE_INIT_SIZE
#define DEQUE_INIT_SIZE 8
#endif

#include "algobase.h"
#include "allocator.h"
#include "iterator.h"

namespace mystl{
    template <typename T, size_t BuffSize>
    struct deque_iterator : public iterator<random_access_iterator_tag, T>{
        public:

        const size_t num_element_of_buffer = sizeof(T) < 256 ? BuffSize / sizeof(T) : 16;

        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using size_type = size_t;
        using difference_type = size_t;
        using value_pointer = T*;
        using map_pointer = T**;

        using iterator = deque_iterator<T, BuffSize>;
        using const_iterator = deque_iterator<T, BuffSize>;
        using self = deque_iterator;

        value_pointer cur;
        value_pointer first;
        value_pointer last;
        map_pointer node;

        //construct
        deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {};
        deque_iterator(value_pointer c, map_pointer n) : cur(c), first(*n), last(*n + num_element_of_buffer), node(n) {};
        deque_iterator(const iterator& iter) : cur(iter.cur), first(iter.first), last(iter.last), node(iter.node) {};

        self& operator=(const iterator& rhs){
            if(this != &rhs){
                cur = rhs.cur;
                first = rhs.first;
                last = rhs.last;
                node = rhs.node;
            }
            return *this;
        }

        reference operator*() const {
            return *cur;
        }

        pointer operator->() const {
            return cur;
        }

        difference_type operator-(const self& rhs){
            return num_element_of_buffer * (node - rhs.node) + (cur - first) - (rhs.cur - rhs.first);
        }

        self& operator++(){
            ++cur;
            if(cur == last){
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }

        self operator++(int){
            self temp = *this;
            ++*this;
            return temp;
        }

        self& operator--(){
            if(cur == first){
                set_node(node - 1);
                cur = last - 1;
            }
            else{
                --cur;
            }
            return *this;
        }

        self operator--(int){
            self temp = *this;
            --*this;
            return temp;
        }

        self& operator+=(difference_type n){
            if(n < 0){
                return *this -= -n;
            }
            if(n < static_cast<difference_type>(last - cur)){
                cur += n;
            }
            else{
                difference_type offset = n - (last - cur);
                set_node(node + 1 + offset / num_element_of_buffer);
                cur = first + offset % num_element_of_buffer;
            }
            return *this;
        }

        self& operator-=(difference_type n){
            if(n < 0){
                return *this += -n;
            }
            if(n <= static_cast<difference_type>(cur - first)){
                cur -= n;
            }
            else{
                difference_type offset = n - (cur - first);
                set_node(node - 1 - offset / num_element_of_buffer);
                cur = last - offset % num_element_of_buffer;
            }
            return *this;
        }

        self operator+(difference_type n){
            self temp = *this;
            return temp += n;
        }

        self operator-(difference_type n){
            self temp = *this;
            return temp -= n;
        }

        reference operator[](difference_type n){
            return *(*this + n);
        }

        bool operator==(const self& rhs) const {
            return cur == rhs.cur;
        }

        bool operator!=(const self& rhs) const {
            return !(*this == rhs);
        }

        bool operator<(const self& rhs) const {
            return (node == rhs.node) ? (cur < rhs.cur) : (node < rhs.node);
        }

        bool operator>(const self& rhs) const {
            return rhs < *this;
        }

        bool operator<=(const self& rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const self& rhs) const {
            return !(*this < rhs);
        }

        void set_node(map_pointer new_node){
            node = new_node;
            first = *new_node;
            last = *new_node + num_element_of_buffer;
        }
    };

    template <typename T, typename alloc = mystl::allocator<T>, size_t BuffSize = 512>
    class deque{
        public:
        using allocator_type = alloc;
        using data_allocator = alloc;
        using map_allocator = mystl::allocator<T*>;

        using value_type = typename allocator_type::value_type;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        using reference = typename allocator_type::reference;
        using const_reference = typename allocator_type::const_reference;
        using size_type = typename allocator_type::size_type;
        using difference_type = typename allocator_type::difference_type;
        
        using map_pointer = pointer*;
        using const_map_pointer = const_pointer*;

        using iterator = deque_iterator<T, BuffSize>;
        using const_iterator = deque_iterator<T, BuffSize>;

        const size_t num_element_of_buffer = sizeof(T) < 256 ? BuffSize / sizeof(T) : 16;

        private:
        iterator _begin;
        iterator _end;
        map_pointer _map;
        size_type _map_size;

        public:

        //construct
        deque(){
            fill_init(0, value_type());
        }

        deque(size_type n){
            fill_init(n, value_type());
        }

        deque(size_type n, const value_type& value){
            fill_init(n, value);
        }

        template <typename iter, 
        typename mystl::enable_if<mystl::is_iterator<iter>::value, int>::type = 0,
        typename mystl::enable_if<mystl::is_the_same_type<typename mystl::iterator_traits<iter>::value_type, value_type>::value, int>::type = 0>
        deque(iter first, iter last){
            copy_init(first, last);
        }

        deque(const deque& rhs){
            copy_init(rhs._begin, rhs._end);
        }

        deque(deque&& rhs) : _begin(mystl::move(rhs._begin)), _end(mystl::move(rhs._end)), _map(mystl::move(rhs._map)), _map_size(mystl::move(rhs._map_size)){
            rhs._map = nullptr;
            rhs._map_size = 0;
        }

        ~deque(){
            for(map_pointer cur = _begin.node + 1; cur < _end.node; cur++){
                data_allocator::destroy(*cur, *cur + num_element_of_buffer);
            }
            if(_begin.node != _end.node){
                data_allocator::destroy(_begin.cur, _begin.last);
                data_allocator::destroy(_end.first, _end.cur);
            }
            else{
                data_allocator::destroy(_begin.cur, _end.cur);
            }
            destroy_buffer(_begin.node, _end.node);
            map_allocator::destroy(_map);
            map_allocator::deallocate(_map);
        }

        iterator begin() noexcept { return _begin; }

        const_iterator cbegin() const noexcept { return _begin; }

        iterator end() noexcept { return _end; }

        const_iterator cend() const noexcept { return _end; }

        bool empty() const noexcept { return cbegin() == cend(); }

        reference operator[](size_type n){
            return _begin[n];
        }

        reference front(){
            return *begin();
        }

        reference back(){
            return *(end() - 1);
        }

        size_type size(){
            return _end - _begin;
        }

        size_type capacity(){
            return _map_size * num_element_of_buffer;
        }

        iterator find(iterator first, iterator last, const value_type& value){
            for(; first != last; ++first){
                if(*first == value)
                    break;
            }
            return first;
        }

        void pop_front(){
            if(empty())
                return;
            if(_begin.cur != _begin.last - 1){
                data_allocator::destroy(_begin.cur);
                ++_begin;
            }
            else{
                data_allocator::destroy(_begin.cur);
                ++_begin;
                destroy_buffer(_begin.node - 1, _begin.node - 1);
            }
        }

        void pop_back(){
            if(empty())
                return;
            if(_end.cur != _end.first){
                --_end;
                data_allocator::destroy(_end.cur);
            }
            else{
                --_end;
                data_allocator::destroy(_end.cur);
                destroy_buffer(_end.node + 1, _end.node + 1);
            }
        }

        template <typename ...Args>
        void emplace_back(Args&& ...args){
            if(_end.cur != _end.last - 1){
                data_allocator::construct(_end.cur, mystl::forward<Args>(args)...);
                ++_end;
            }
            else{
                require_and_alloc_node_at_back(1);
                data_allocator::construct(_end.cur, mystl::forward<Args>(args)...);
                ++_end;
            }
        }

        void push_back(const value_type& value){
            if(_end.cur != _end.last - 1){
                data_allocator::construct(_end.cur, value);
                ++_end;
            }
            else{
                require_and_alloc_node_at_back(1);
                data_allocator::construct(_end.cur, value);
                ++_end;
            }
        }

        void push_front(const value_type& value){
            if(_begin.cur != _begin.first){
                --_begin;
                data_allocator::construct(_begin.cur, value);
            }
            else{
                require_and_alloc_node_at_front(1);
                --_begin;
                data_allocator::construct(_begin.cur, value);
            }
        }

        iterator insert(iterator pos, const value_type& value){
            if(pos.cur == _begin.cur){
                push_front(value);
                return _begin;
            }
            else if(pos.cur == _end.cur){
                push_back(value);
                return _end - 1;
            }
            else{
                return insert_aux(pos, value);
            }
        }

        iterator erase(iterator pos){
            if(pos.cur == _begin.cur){
                pop_front();
                return _begin;
            }
            else if(pos.cur == _end.cur){
                pop_back();
                return _end - 1;
            }
            else{
                return erase_aux(pos);
            }
        }

        private:
        void fill_init(size_type n, const value_type& value){
            init_map(n);
            if(n != 0){
                for(map_pointer cur = _begin.node; cur < _end.node; cur++){
                    mystl::uninitialized_fill(*cur, num_element_of_buffer, value);
                }
                mystl::uninitialized_fill(_end.first, _end.cur, value);
            }
        }

        template <typename iter>
        void copy_init(iter first, iter last){
            const size_type n = mystl::distance(first, last);
            init_map(n);
            for(map_pointer cur = _begin.node; cur < _end.node; cur++){
                auto next = first;
                mystl::advance(next, num_element_of_buffer);
                mystl::uninitialized_copy(first, next, *cur);
                first = next;
            }
            mystl::uninitialized_copy(first, last, _end.first);
        }

        void init_map(size_type n){
            size_type num_buffer = n / num_element_of_buffer + 1;
            _map_size = mystl::max(static_cast<size_type>(DEQUE_INIT_SIZE), num_buffer);
            _map = create_map(_map_size);
            map_pointer nstart = _map + (_map_size - num_buffer) / 2;
            map_pointer nfinish = nstart + num_buffer - 1;
            create_buffer(nstart, nfinish);
            _begin.set_node(nstart);
            _end.set_node(nfinish);
            _begin.cur = _begin.first;
            _end.cur = _end.first + n % num_element_of_buffer;
        }

        map_pointer create_map(size_type n){
            map_pointer mp = map_allocator::allocate(n);
            for(size_type i = 0; i < n; i++){
                *(mp + i) = nullptr;
            }
            return mp;
        }

        void create_buffer(map_pointer nstart, map_pointer nfinish){
            map_pointer cur;
            for(cur = nstart; cur <= nfinish; cur++){
                *cur = data_allocator::allocate(num_element_of_buffer);
            }
        }

        void destroy_buffer(map_pointer nstart, map_pointer nfinish){
            for(map_pointer cur = nstart; cur <= nfinish; cur++){
                data_allocator::deallocate(*cur, num_element_of_buffer);
            }
        }

        void require_and_alloc_node_at_front(size_type n){
            if(_map <= _begin.node - n){
                create_buffer(_begin.node - n, _begin.node - 1);
            }
            else{
                size_type old_num_buffer = _end.node - _begin.node + 1;
                size_type new_num_buffer = old_num_buffer + n;
                size_type new_map_size = mystl::max(_map_size << 1, new_num_buffer + DEQUE_INIT_SIZE);
                map_pointer new_map = create_map(new_map_size);
                map_pointer nstart = new_map + (new_map_size - new_num_buffer) / 2;
                map_pointer ostart = nstart + n;
                map_pointer nend = nstart + new_num_buffer - 1;
                auto cur = ostart;
                for(auto begin1 = _begin.node; begin1 <= _end.node; cur++, begin1++)
                    *cur = *begin1;
                create_buffer(nstart, ostart - 1);
                map_allocator::deallocate(_map, _map_size);
                _map = new_map;
                _map_size = new_map_size;
                _begin = iterator(*ostart + static_cast<int>(_begin.cur - _begin.first), ostart);
                _end = iterator(*nend + static_cast<int>(_end.cur - _end.first), nend);
            }
        }

        void require_and_alloc_node_at_back(size_type n){
            if(_map + _map_size > _end.node + n){
                create_buffer(_end.node + 1, _end.node + n);
            }
            else{
                size_type old_num_buffer = _end.node - _begin.node + 1;
                size_type new_num_buffer = old_num_buffer + n;
                size_type new_map_size = mystl::max(_map_size << 1, new_num_buffer + DEQUE_INIT_SIZE);
                map_pointer new_map = create_map(new_map_size);
                map_pointer nstart = new_map + (new_map_size - new_num_buffer) / 2;
                map_pointer oend = nstart + old_num_buffer - 1;
                map_pointer nend = nstart + new_num_buffer - 1;
                auto cur = nstart;
                for(auto begin1 = _begin.node; begin1 <= _end.node; cur++, begin1++){
                    *cur = *begin1;
                }
                create_buffer(oend + 1, nend);
                map_allocator::deallocate(_map, _map_size);
                _map = new_map;
                _map_size = new_map_size;
                _begin = iterator(*nstart + static_cast<int>(_begin.cur - _begin.first), nstart);
                _end = iterator(*oend + static_cast<int>(_end.cur - _end.first), oend);
            }
        }

        iterator insert_aux(iterator pos, const value_type& value){
            size_t num_elements_before_pos = pos - _begin;
            if(num_elements_before_pos < size() / 2){
                push_front(front());
                mystl::copy(_begin + 2, _begin + 1 + num_elements_before_pos, _begin + 1);
            }
            else{
                push_back(back());
                mystl::copy_backward(_begin + num_elements_before_pos, _end - 3, _end - 2);
            }
            *(_begin + num_elements_before_pos) = value;
            return _begin + num_elements_before_pos;
        }

        iterator erase_aux(iterator pos){
            size_t num_elements_before_pos = pos - _begin;
            if(num_elements_before_pos < size() / 2){
                mystl::copy_backward(_begin, pos - 1, pos);
                pop_front();
            }
            else{
                mystl::copy(pos + 1, _end, pos);
                pop_back();
            }
            return _begin + num_elements_before_pos;
        }

    };
}

#endif