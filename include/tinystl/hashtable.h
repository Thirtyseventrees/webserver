#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "allocator.h"
#include "exceptdef.h"
#include "iterator.h"
#include "util.h"
#include "vector.h"

namespace mystl{


    static const int num_primes = 44;
    
    static constexpr size_t ht_prime_list[] = {
        101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u, 
        10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
        402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
        10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
        176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
        2006689337u, 3010034021u, 4294967291u,
    };
    
    inline size_t next_prime(size_t n){
        int first_index = 0;
        int last_index = num_primes;
        int mid;
        while (first_index < last_index){
            mid = (first_index + last_index) / 2;
            if(n <= ht_prime_list[mid])
                last_index = mid;
            else
                first_index = mid + 1;
        }
        return ht_prime_list[first_index];
    };

    template <typename T>
    struct hashtable_node{
        hashtable_node* next;
        T value;

        //construct
        hashtable_node() = default;
        hashtable_node(const T& v) : next(nullptr), value(v) {};
        hashtable_node(const hashtable_node& node) : next(node.next), value(node.value) {};
        hashtable_node(hashtable_node&& node) : next(node.next), value(mystl::move(node.value)) {
            node.next = nullptr;
        };

    };

    template <typename T, typename HashFunc, typename KeyOfValue, typename EqualKey>
    class hashtable;

    template <typename T, typename HashFunc, typename KeyOfValue, typename EqualKey>
    struct hashtable_iterator;

    template <typename T, typename HashFunc, typename KeyOfValue, typename EqualKey>
    struct const_hashtable_iterator;

    template <typename T, typename HashFunc, typename KeyOfValue, typename EqualKey>
    struct hashtable_iterator : public mystl::iterator<forward_iterator_tag, T>{
        using hashtable_ = mystl::hashtable<T, HashFunc, KeyOfValue, EqualKey>;
        using iterator = mystl::hashtable_iterator<T, HashFunc, KeyOfValue, EqualKey>;
        using const_iterator = mystl::const_hashtable_iterator<T, HashFunc, KeyOfValue, EqualKey>;
        using node_ptr = hashtable_node<T>*;
        using const_node_ptr = const node_ptr;
        using contain_ptr = hashtable_*;
        using const_contain_ptr = const contain_ptr;
        using size_type = size_t;

        using value_type = T;
        using key_type = decltype(KeyOfValue()(mystl::declval<value_type>()));
        using mapped_type = decltype(HashFunc()(mystl::declval<key_type>()));
        using pointer = value_type*;
        using const_pointer = const pointer;
        using reference = value_type&;
        using const_reference = const reference;

        node_ptr node;
        contain_ptr _hashtable;

        //construct
        hashtable_iterator() = default;
        
        hashtable_iterator(const node_ptr& n, const contain_ptr& ht){
            node = n;
            _hashtable = ht;
        }
        
        hashtable_iterator(const iterator& rhs){
            node = rhs.node;
            _hashtable = rhs._hashtable;
        }

        hashtable_iterator(const const_iterator& rhs){
            node = rhs.node;
            _hashtable = rhs._hashtable;
        }

        //operator
        iterator& operator=(const iterator& rhs){
            if(this != &rhs){
                node = rhs.node;
                _hashtable = rhs._hashtable;
            }
            return *this;
        }

        iterator& operator=(const const_iterator& rhs){
            if(this != &rhs){
                node = rhs.node;
                _hashtable = rhs._node;
            }
            return *this;
        }

        reference operator*() const{
            return node->value;
        }

        pointer operator->() const{
            return &(node->value);
        }

        bool operator==(const iterator& rhs){
            return node == rhs.node;
        }

        bool operator==(const const_iterator& rhs){
            return node == rhs.node;
        }

        bool operator!=(const iterator& rhs){
            return node != rhs.node;
        }

        bool operator!=(const const_iterator& rhs){
            return node != rhs.node;
        }

        iterator& operator++(){
            if(node == nullptr){
                node = _hashtable->buckets[0];
                return *this;
            }
            const key_type key = _hashtable->_key_of_value(node->value);  
            node = node->next;
            if(node == nullptr){
                mapped_type index = _hashtable->hash(key);
                while(node == nullptr && ++index < _hashtable->buckets_size){
                    node = _hashtable->buckets[index];
                }
            }
            return *this;
        }

        iterator operator++(int){
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

    };

    template <typename T, typename HashFunc, typename KeyOfValue, typename EqualKey>
    struct const_hashtable_iterator : public mystl::iterator<forward_iterator_tag, T>{
        using hashtable_ = mystl::hashtable<T, HashFunc, KeyOfValue, EqualKey>;
        using iterator = mystl::hashtable_iterator<T, HashFunc, KeyOfValue, EqualKey>;
        using const_iterator = mystl::const_hashtable_iterator<T, HashFunc, KeyOfValue, EqualKey>;
        using node_ptr = const hashtable_node<T>*;
        using const_node_ptr = node_ptr;
        using contain_ptr = const hashtable_*;
        using const_contain_ptr = contain_ptr;
        using size_type = size_t;

        using value_type = T;
        using key_type = decltype(KeyOfValue()(mystl::declval<value_type>()));
        using mapped_type = decltype(HashFunc()(mystl::declval<key_type>()));
        using pointer = const value_type*;
        using const_pointer = pointer;
        using reference = const value_type&;
        using const_reference = reference;

        node_ptr node;
        contain_ptr _hashtable;

        //construct
        const_hashtable_iterator() = default;
        
        const_hashtable_iterator(const node_ptr& n, const contain_ptr& ht){
            node = n;
            _hashtable = ht;
        }
        
        const_hashtable_iterator(const iterator& rhs){
            node = rhs.node;
            _hashtable = rhs._hashtable;
        }

        const_hashtable_iterator(const const_iterator& rhs){
            node = rhs.node;
            _hashtable = rhs._hashtable;
        }

        //operator
        const_iterator& operator=(const iterator& rhs){
            if(this != &rhs){
                node = rhs.node;
                _hashtable = rhs._hashtable;
            }
            return *this;
        }

        const_iterator& operator=(const const_iterator& rhs){
            if(this != &rhs){
                node = rhs.node;
                _hashtable = rhs._node;
            }
            return *this;
        }

        bool operator==(const iterator& rhs){
            return node == rhs.node;
        }

        bool operator==(const const_iterator& rhs){
            return node == rhs.node;
        }

        bool operator!=(const iterator& rhs){
            return node != rhs.node;
        }

        bool operator!=(const const_iterator& rhs){
            return node != rhs.node;
        }

        reference operator*() const{
            return node->value;
        }

        pointer operator->() const{
            return &(node->value);
        }

        iterator& operator++(){
            const key_type key = _hashtable->_key_of_value(node->value);  
            node = node->next;
            if(node == nullptr){
                mapped_type index = _hashtable->hash(key);
                while(node == nullptr && ++index < _hashtable->buckets_size){
                    node = _hashtable->buckets[index];
                }
            }
            return *this;
        }

        iterator operator++(int){
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

    };

    template <typename T, typename HashFunc, typename KeyOfValue, typename EqualKey>
    class hashtable{

        friend struct mystl::hashtable_iterator<T, HashFunc, KeyOfValue, EqualKey>;
        friend struct mystl::const_hashtable_iterator<T, HashFunc, KeyOfValue, EqualKey>;

        public:
        using value_type = T;
        using key_type = decltype(KeyOfValue()(mystl::declval<value_type>()));
        using mapped_type = decltype(HashFunc()(mystl::declval<key_type>()));
        using hasher = HashFunc;
        using key_of_value = KeyOfValue;
        using key_equal = EqualKey;

        using node_type = hashtable_node<value_type>;
        using node_ptr = node_type*;
        using bucket_type = mystl::vector<node_ptr>;

        using iterator = mystl::hashtable_iterator<T, HashFunc, KeyOfValue, EqualKey>;
        using const_iterator = mystl::const_hashtable_iterator<T, HashFunc, KeyOfValue, EqualKey>;

        using data_allocator = mystl::allocator<T>;
        using node_allocator = mystl::allocator<node_type>;

        using pointer = value_type*;
        using const_pointer = const pointer;
        using reference = value_type&;
        using const_reference = const reference;
        using size_type = size_t;

        private:
        size_type buckets_size;
        bucket_type buckets;
        size_type num_elements;
        hasher hash;
        key_equal equals;
        key_of_value _key_of_value;

        public:
        //construct
        hashtable(size_type bucket_count, const hasher& h = hasher(), const key_equal& ke = key_equal(), const key_of_value kov = key_of_value())
            : buckets_size(mystl::next_prime(bucket_count)), buckets(mystl::vector<node_ptr>(buckets_size, nullptr)), num_elements(0), hash(h), equals(ke), _key_of_value(kov) {};

        ~hashtable(){
            clear();
        }

        //iterator
        iterator begin() noexcept{
            return M_begin();
        }

        const_iterator begin() const noexcept{
            return M_begin();
        }

        iterator end() noexcept{
            return iterator(nullptr, this);
        }

        const_iterator end() const noexcept{
            return const_iterator(nullptr, this);
        }

        //function
        size_type size() const noexcept{
            return num_elements;
        }

        mystl::pair<iterator, bool> insert_unique(const value_type& value){
            resize(num_elements + 1);
            return insert_unique_without_resize(value);
        }

        bool erase_unique(const key_type& key){
            const size_type index = bkt_index_by_key(key);
            node_ptr cur = buckets[key];
            if(cur){
                if(equals(key, _key_of_value(cur->value))){
                    buckets[index] = cur->next;
                    destroy_node(cur);
                    --num_elements;
                    return true;
                }
                else{
                    node_ptr next = cur->next;
                    while(next){
                        if(equals(key, _key_of_value(next->value))){
                            cur->next = next->next;
                            destroy_node(next);
                            --num_elements;
                            return true;
                        }
                        cur = next;
                        next = cur->next;
                    }
                }
            }
            return false;
        }

        mystl::pair<iterator, bool> find(const key_type& key){
            const size_type index = bkt_index_by_key(key);
            node_ptr cur = buckets[index];
            for(; cur && !equals(key, _key_of_value(cur->value)); cur = cur->next){}
            if(cur)
                return mystl::make_pair(iterator(cur, this), true);
            else
                return mystl::make_pair(iterator(cur, this), false);
        }

        size_type count(const key_type& key){
            const size_type index = bkt_index_by_key(key);
            size_type result = 0;
            for(node_ptr cur = buckets[index]; cur; cur = cur->next){
                if(equals(key, _key_of_value(cur->value)))
                    ++result;
            }
            return result;
        }

        void clear(){
            for(size_type i = 0; i < buckets_size; ++i){
                node_ptr cur = buckets[i];
                while(cur){
                    node_ptr next = cur->next;
                    destroy_node(cur);
                    cur = next;
                }
                buckets[i] = nullptr;
            }
            num_elements = 0;
        }

        private:
        iterator M_begin() noexcept{
            for(size_type i = 0; i < buckets_size; ++i){
                if(buckets[i])
                    return iterator(buckets[i], this);
            }
            return iterator(nullptr, this);
        }

        const_iterator M_begin() const noexcept{
            for(size_type i = 0; i < buckets_size; ++i){
                if(buckets[i])
                    return const_iterator(buckets[i], this);
            }
            return const_iterator(nullptr, this);
        }

        template <typename ...Args>
        node_ptr create_node(Args&& ...args){
            node_ptr node = node_allocator::allocate();
            node->next = nullptr;
            data_allocator::construct(&node->value, mystl::forward<Args>(args)...);
            return node;
        }

        void destroy_node(node_ptr n){
            data_allocator::destroy(&n->value);
            node_allocator::deallocate(n);
        }

        //get the index in buckets based on key or value.
        size_type bkt_index_by_key(const key_type& key){
            return hash(key) % buckets_size;
        }
        
        size_type bkt_index_by_value(const value_type& value){
            return hash(_key_of_value(value)) % buckets_size;
        }

        //reset buckets and buckets_size based on num.
        void resize(size_type num){
            if(num > buckets_size){
                const size_type new_buckets_size = mystl::next_prime(num);
                const size_type old_buckets_size = buckets_size;
                if(new_buckets_size > old_buckets_size){
                    buckets_size = new_buckets_size;
                    bucket_type tmp(new_buckets_size, nullptr);
                    node_ptr cur;
                    for (size_type i = 0; i < old_buckets_size; i++){
                        cur = buckets[i];
                        while(cur){
                            size_type new_index = bkt_index_by_value(cur->value);
                            buckets[i] = cur->next;
                            cur->next = tmp[new_index];
                            tmp[new_index] = cur;
                            cur = buckets[i];
                        }
                    }
                    buckets.swap(tmp);
                }
            }
        }

        mystl::pair<iterator, bool> insert_unique_without_resize(const value_type& value){
            key_type key = _key_of_value(value);
            const size_type index = bkt_index_by_key(key);
            node_ptr first = buckets[index];
            for(node_ptr cur = first; cur; cur = cur->next){
                if(equals(key, _key_of_value(cur->value)))
                    return mystl::make_pair(iterator(cur, this), false);
            }
            node_ptr new_node = create_node(value);
            new_node->next = first;
            buckets[index] = new_node;
            ++num_elements;
            return mystl::make_pair(iterator(new_node, this), true);
        }

    };

}

#endif