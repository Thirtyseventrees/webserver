#ifndef MAP_H
#define MAP_H

#include "allocator.h"
#include "exceptdef.h"
#include "iterator.h"
#include "util.h"
#include "rb_tree.h"

namespace mystl{

    template <typename K, typename V, typename Compare = mystl::compare_less>
    class map{
        public:
        using key_type = K;
        using mapped_type = V;
        using value_type = mystl::pair<const key_type, mapped_type>;
        using key_compare = Compare;

        class value_compare{
            friend class map<key_type, mapped_type, key_compare>;
            protected:
            Compare comp;
            value_compare(Compare c) : comp(c) {}
            public:
            bool operator()(const value_type& lhs, const value_type& rhs){
                return comp(lhs.first, rhs.first);   
            }
        };

        private:
        struct key_of_value{
            key_type operator()(const value_type& value) const {
                return value.first;
            }
        };

        private:
        using rep_type = rb_tree<value_type, key_compare, key_of_value>;
        rep_type _tree;

        public:
        using allocator_type = typename rep_type::allocator_type;
        using data_allocator = typename rep_type::allocator_type;
        using pointer = typename rep_type::pointer;
        using const_pointer = typename rep_type::const_pointer;
        using reference = typename rep_type::reference;
        using const_reference = typename rep_type::const_reference;
        using iterator = typename rep_type::iterator;
        using const_iterator = typename rep_type::const_iterator;
        using size_type = typename rep_type::size_type;

        //construct
        map() : _tree() {};

        template <typename iter>
        map(iter first, iter last) : _tree() { _tree.insert_unique(first, last); };

        map(const map& rhs) : _tree(rhs._tree) {};

        map(map&& rhs) : _tree(mystl::move(rhs._tree)) {};

        //operator
        map& operator=(const map& rhs){
            _tree = rhs._tree;
            return *this;
        }

        map& operator=(map&& rhs){
            _tree = mystl::move(rhs._tree);
            return *this;
        }

        mapped_type& operator[](const key_type& key){
            return (*(insert(value_type(key, mapped_type())).first)).second;
        }

        //iterator
        iterator begin(){
            return _tree.begin();
        }

        const_iterator begin() const{
            return _tree.cbegin();
        }

        iterator end(){
            return _tree.end();
        }

        const_iterator cend() const{
            return _tree.cend();
        }

        bool empty() const{
            return _tree.empty();
        }

        size_type size() const{
            return _tree.size();
        }

        iterator find(const key_type& key){
            return _tree.find(key);
        }

        const_iterator find(const key_type& key) const{
            return _tree.find(key);
        }

        size_type count(const key_type& key) const{
            return _tree.count(key);
        }

        iterator lower_bound(const key_type& key){
            return _tree.lower_bound(key);
        }

        const_iterator lower_bound(const key_type& key) const {
            return _tree.lower_bound(key);
        }

        iterator upper_bound(const key_type& key){
            return _tree.upper_bound(key);
        }

        const_iterator upper_bound(const key_type& key) const {
            return _tree.upper_bound(key);
        }

        mystl::pair<iterator, iterator> equal_range(const key_type& key){
            return _tree.equal_range(key);
        }

        mystl::pair<iterator, bool> insert(const value_type& value){
            return _tree.insert_unique(value);
        }

    };

}

#endif