#ifndef LIST_H
#define LIST_H

#include "allocator.h"
#include "exceptdef.h"
#include "iterator.h"
#include "util.h"

namespace mystl{

    template <typename T> struct base_list_node;
    template <typename T> struct list_node;

    template <typename T>
    struct node_traits{
        using base_ptr = base_list_node<T>*;
        using node_ptr = list_node<T>*;
    };

    template <typename T>
    struct base_list_node{
        using base_ptr = typename node_traits<T>::base_ptr;
        using node_ptr = typename node_traits<T>::node_ptr;

        base_ptr prev = nullptr;
        base_ptr next = nullptr;

        node_ptr as_node(){
            return static_cast<node_ptr>(this);
        }

        base_ptr self(){
            return this;
        }

    };

    template <typename T>
    struct list_node: public base_list_node<T>{
        using base_ptr = typename node_traits<T>::base_ptr;
        using node_ptr = typename node_traits<T>::node_ptr;

        T value;        //empty if head

        list_node() = default;
        list_node(const T& v) : value(v) {};
        list_node(T&& v) : value(mystl::move(v)) {};

        node_ptr self(){
            return this;
        }

        base_ptr as_base(){
            return static_cast<base_ptr>(this);
        }
    };

    template <typename T>
    struct list_iterator : public mystl::iterator<mystl::bidirectional_iterator_tag, T>{
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using base_ptr = base_list_node<T>*;
        using node_ptr = list_node<T>*;
        using self = list_iterator<T>;

        //pointer to the currence node.
        base_ptr _node;

        //Construct
        list_iterator() = default;
        list_iterator(base_ptr p) : _node(p) {};
        list_iterator(const self& p) : _node(p._node) {};

        //Reload reference
        reference operator*(){
            return _node->as_node()->value;
        }

        pointer operator&(){
            return &(operator*());
        }

        //++it
        self& operator++(){
            _node = _node->next;
            return *this;
        }

        //it++
        self operator++(int){
            self temp = *this;
            ++*this;
            return temp;
        }

        self operator+(int n){
            self temp = *this;
            for(; n > 0; n--){
                temp++;
            }
            return temp;
        }

        //--it
        self& operator--(){
            _node = _node->prev;
            return *this;
        }
        
        //it--
        self operator--(int){
            self temp = *this;
            --*this;
            return temp;
        }

        self operator-(int n){
            self temp = *this;
            for(; n > 0; n--){
                temp--;
            }
            return temp;
        }

        bool operator==(const self& rhs){
            return _node == rhs._node;
        }

        bool operator!=(const self& rhs){
            return _node != rhs._node;
        }

    };

    template <typename T>
    struct const_list_iterator : public mystl::iterator<mystl::bidirectional_iterator_tag, T>{
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using base_ptr = base_list_node<T>*;
        using node_ptr = list_node<T>*;
        using self = const_list_iterator<T>;

        //pointer to the currence node.
        base_ptr _node;

        //Construct
        const_list_iterator() = default;
        const_list_iterator(base_ptr p) : _node(p) {};
        const_list_iterator(const self& p) : _node(p._node) {};

        //Reload reference
        reference operator*() const {
            return _node->as_node()->value;
        }

        pointer operator&() const {
            return &(operator*());
        }

        //++it
        self& operator++(){
            _node = _node->next;
            return *this;
        }

        //it++
        self operator++(int){
            self temp = *this;
            ++*this;
            return temp;
        }

        self operator+(int n){
            self temp = *this;
            for(; n > 0; n--){
                ++temp;
            }
            return temp;
        }

        //--it
        self& operator--(){
            _node = _node->prev;
            return *this;
        }
        
        //it--
        self operator--(int){
            self temp = *this;
            --*this;
            return temp;
        }

        self operator-(int n){
            self temp = *this;
            for(; n > 0; n--){
                ++temp;
            }
            return temp;
        }

        bool operator==(const self& rhs){
            return _node == rhs._node;
        }

        bool operator!=(const self& rhs){
            return _node != rhs._node;
        }

    };

    template <typename T, typename alloc = mystl::allocator<T>>
    class list {
    
    public:
        using allocator_type = alloc;
        using data_allocator = alloc;

        using value_type = typename allocator_type::value_type;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        using reference = typename allocator_type::reference;
        using const_reference = typename allocator_type::const_reference;
        using size_type = typename allocator_type::size_type;
        using difference_type = typename allocator_type::difference_type;

        using iterator = list_iterator<T>;
        using const_iterator = const_list_iterator<T>;

        using _base_list_node = base_list_node<T>;
        using _list_node = list_node<T>;
        using base_ptr = _base_list_node*;
        using node_ptr = _list_node*;

    private:
        //Point to the end of the list.
        base_ptr _node;
        //size_type _size;

    protected:
        using base_list_node_allocator = mystl::allocator<_base_list_node>;
        using list_node_allocator = mystl::allocator<_list_node>;

        base_ptr allocate_base_node(){ return base_list_node_allocator::allocate(); }
        node_ptr allocate_node(){ return list_node_allocator::allocate(); }
        
        void deallocate_node(base_ptr p){ base_list_node_allocator::deallocate(p); }
        void deallocate_node(node_ptr p){ list_node_allocator::deallocate(p); }

        //construct_node
        base_ptr construct_node(base_ptr p){
            base_list_node_allocator::construct(p);
            return p;
        }

        node_ptr construct_node(node_ptr p){
            list_node_allocator::construct(p);
            return p;
        }

        node_ptr construct_node(node_ptr p, const value_type& v){
            list_node_allocator::construct(p, v);
            return p;
        }

        node_ptr construct_node(node_ptr p, value_type&& v){
            list_node_allocator::construct(p, mystl::move(v));
            return p;
        }

        //destroy_node
        void destroy_node(base_ptr p){
            base_list_node_allocator::destroy(p);
        }

        void destroy_node(node_ptr p){
            list_node_allocator::destroy(p);
        }

        //create_node
        base_ptr create_node(){
            base_ptr p = allocate_base_node();
            construct_node(p);
            return p;
        }

        node_ptr create_node(const value_type& v){
            node_ptr p = allocate_node();
            construct_node(p, v);
            return p;
        }

        node_ptr create_node(value_type&& v){
            node_ptr p = allocate_node();
            construct_node(p, mystl::move(v));
            return p;
        } 

        //delete_node
        void delete_node(node_ptr p){
            destroy_node(p);
            deallocate_node(p);
        }

    public:
        //construct
        list(){
            _node = create_node();
            _node->next = _node;
            _node->prev = _node;
            //_size = 0;
        }

        template <typename iter, 
        typename mystl::enable_if<mystl::is_iterator<iter>::value, int>::type = 0,
        typename mystl::enable_if<mystl::is_the_same_type<typename mystl::iterator_traits<iter>::value_type, value_type>::value, int>::type = 0>
        list(iter first, iter end){
            copy_init(first, end);
        }

        list(const list& rhs){
            copy_init(rhs.begin(), rhs.end());
        }

        ~list(){
            clear();
            base_list_node_allocator::deallocate(_node);
            _node = nullptr;
        }

        /********************/

        // size_type size(){
        //     return _size;
        // }

        bool empty(){
            return _node->next == _node;
        }

        iterator begin(){
            return _node->next;
        }

        const_iterator cbegin(){
            return _node->next;
        }
        
        iterator end(){
            return _node;
        }

        const_iterator cend(){
            return _node;
        }
        
        iterator find(const value_type& value){
            iterator cur = begin();
            for (; cur != end(); cur++){
                if(*cur == value)
                    return cur;
            }
            return cur;
        }

        /**
       *  @brief  Inserts given value into list before specified iterator.
       *  @param  pos  A const_iterator into the list.
       *  @param  value  Data to be inserted.
       *  @return  An iterator that points to the inserted data.
       * */
        iterator insert(iterator pos, const value_type& v){
            if(pos._node == nullptr)
                return nullptr;
            node_ptr p = create_node(v);
            p->next = pos._node;
            p->prev = pos._node->prev;
            pos._node->prev->next = p;
            pos._node->prev = p;
            //_size++;
            return p;
        }

        /**
       *  @brief  Remove element at given position.
       *  @param  pos Iterator pointing to element to be erased.
       *  @return  An iterator pointing to the next element (or end()).
       *
       *  This function will erase the element at the given position and thus
       *  shorten the list by one.
       **/
        iterator erase(const_iterator pos){
            THROW_ERROR_RANGE_IF((pos == cend() || pos == nullptr), "Error");
            auto next = pos._node->next;
            unlink_node(pos._node, pos._node);
            delete_node(pos._node->as_node());
            //_size--;
            return iterator(next);
        }

        iterator push_front(const value_type& value){
            return insert(iterator(begin()._node), value);
        }

        iterator push_back(const value_type& value){
            return insert(iterator(end()._node), value);
        }

        void pop_front(){
            erase(cbegin());
        }

        void pop_back(){
            erase(--cend());
        }

        /** 
            @brief Remove element whose value equal to v.
            @param v The value need to be removed.
        **/
        void remove(const value_type& v){
            const_iterator cur = cbegin();
            const_iterator last = cend();
            while(cur != last){
                if(*cur == v)
                    erase(cur++);
                else
                    ++cur;
            }
        }

        void unique(){
            const_iterator cur = ++cbegin();
            const_iterator last = cend();
            while(cur != last){
                if(*cur == *(cur - 1))
                    erase(cur++);
                else
                    ++cur;
            }
        }

        /**
         *  @brief Transfer the elements between [first, last] to the front of pos
         *  @param pos
         *  @param first
         *  @param last
         */
        void transfer(iterator pos, iterator first, iterator last){
            //[first,last]
            unlink_node(first._node, last._node);
            first._node->prev = pos._node->prev;
            last._node->next = pos._node;
            //the previous node of pos
            pos._node->prev->next = first._node;
            //the node of pos
            pos._node->prev = last._node;
        }

        void clear(){
            if(_node->next != _node->prev){
                base_ptr cur = _node->next;
                base_ptr next = cur->next;
                for(; cur != _node; cur = next, next = next->next){
                    delete_node(cur->as_node());
                }
                //_size = 0;
            }
        }

        void swap(list& rhs){
            mystl::swap(_node, rhs._node);
            //mystl::swap(_size, rhs._size);
        }

        void swap(list& lhs, list& rhs){
            lhs.swap(rhs);
        }

    private:

        void unlink_node(base_ptr first, base_ptr last){
            first->prev->next = last->next;
            last->next->prev = first->prev;
            last->next = nullptr;
            first->prev = nullptr;
        }

        template <typename iter>
        void copy_init(iter first, iter end){
            _node = create_node();
            _node->next = _node;
            _node->prev = _node;
            for(; first != end; first++){
                node_ptr p = create_node(*first);
                p->prev = _node->prev;
                p->next = _node;
                _node->prev->next = p;
                _node->prev = p;
                if(_node->next == _node)
                    _node->next = p;
            }
        }
    };

}

#endif