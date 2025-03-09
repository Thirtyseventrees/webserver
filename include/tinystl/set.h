#ifndef SET_H
#define SET_H

#include "allocator.h"
#include "exceptdef.h"
#include "iterator.h"
#include "util.h"
#include "rb_tree.h"


namespace mystl{

    template <typename T, typename Compare = mystl::compare_less, typename alloc = mystl::allocator<T>>
    class set{

        public:
        using key_type = T;
        using value_type = T;
        using key_compare = Compare;
        using value_compare = Compare;

        private:
        struct key_of_value{
            key_type operator()(const value_type& value) const{
                return value;
            }
        };

        using rep_type = rb_tree<value_type, key_compare, key_of_value, alloc>;
        rep_type _tree;

        public:
        using pointer = typename rep_type::const_pointer;
        using reference = typename rep_type::const_reference;
        using iterator = typename rep_type::const_iterator;
        using size_type = typename rep_type::size_type;
        using self = set<T, Compare, alloc>;

        public:
        //construct
        set() : _tree() {};

        template <typename iter>
        set(iter first, iter last) : _tree() { _tree.insert_unique(first, last); };

        set(const self& rhs) : _tree(rhs._tree) {};

        set(self&& rhs) : _tree(mystl::move(rhs._tree)) {};

        self& operator=(const self& rhs){
            _tree = rhs._tree;
            return *this;
        };

        set& operator=(self&& rhs){
            _tree = mystl::move(rhs._tree);
            return *this;
        }

        iterator begin() const {
            return _tree.cbegin();
        }

        iterator end() const{
            return _tree.cend();
        }

        bool empty() const{
            return _tree.empty();
        }

        size_type size() const{
            return _tree.size();
        }

        void swap(self& rhs){
            _tree.swap(rhs._tree);
        }

        mystl::pair<iterator, bool> insert(const value_type& value){
            return _tree.const_insert_unique(value);
        }

        template <typename iter>
        void insert(iter first, iter last){
            _tree.insert_unique(first, last);
        }

        bool erase(const value_type& value){
            return _tree.erase_unique(value);
        }

        void clear(){
            _tree.clear();
        }

        iterator find(const value_type& value) const {
            return _tree.find(value);
        }

    };

}

#endif