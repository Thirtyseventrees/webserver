#ifndef RB_TREE_H
#define RB_TREE_H

#include "allocator.h"
#include "exceptdef.h"
#include "iterator.h"
#include "util.h"
#include "queue.h"

namespace mystl {

using node_color = bool;

static constexpr node_color red = false;
static constexpr node_color black = true;

template <typename T>
struct base_rb_tree_node;
template <typename T>
struct rb_tree_node;

template <typename T>
struct base_rb_tree_node {

    using base_ptr = base_rb_tree_node<T> *;
    using node_ptr = rb_tree_node<T> *;

    node_color color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    node_ptr as_node() {
        return static_cast<node_ptr>(this);
    }

    static base_ptr minimum(base_ptr x) {
        while (x->left != nullptr)
            x = x->left;
        return x;
    }

    static base_ptr maxmimum(base_ptr x) {
        while (x->right != nullptr)
            x = x->right;
        return x;
    }
};

template <typename T>
struct rb_tree_node : public base_rb_tree_node<T> {

    using base_ptr = base_rb_tree_node<T> *;
    using node_ptr = rb_tree_node<T> *;

    T value;

    base_ptr as_base() {
        return static_cast<base_ptr>(this);
    }
};

template <typename T>
struct base_rb_tree_iterator : public mystl::iterator<mystl::bidirectional_iterator_tag, T> {

    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using base_ptr = base_rb_tree_node<T> *;
    using node_ptr = rb_tree_node<T> *;
    using _base_rb_tree_node = base_rb_tree_node<T>;
    using _rb_tree_node = rb_tree_node<T>;

    base_ptr node;

    // Construct
    base_rb_tree_iterator() {};
    base_rb_tree_iterator(base_ptr n) : node(n) {};

    void increment() {
        if (node->right != nullptr) {
            node = node->right;
            while (node->left != nullptr)
                node = node->left;
        } else {
            base_ptr father = node->parent;
            while (father->right == node) {
                node = father;
                father = node->parent;
            }
            // Check the case that find the next of the root node which has no right child.
            if (node->right != father)
                node = father;
        }
    }

    void decrement() {
        // If node is head node
        if (node->parent->parent == node && !(node->color))
            node = node->right;
        // has left child
        else if (node->left != nullptr) {
            node = node->left;
            while (node->right != nullptr)
                node = node->right;
        }
        // no left child and not head node
        else {
            base_ptr father = node->parent;
            while (father->left == node) {
                node = father;
                father = node->parent;
            }
            node = father;
        }
    }

    bool operator==(const base_rb_tree_iterator &rhs) {
        return node == rhs.node;
    }

    bool operator!=(const base_rb_tree_iterator &rhs) {
        return node != rhs.node;
    }
};

template <typename T>
struct rb_tree_iterator : public base_rb_tree_iterator<T> {

    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using base_ptr = base_rb_tree_node<T> *;
    using node_ptr = rb_tree_node<T> *;
    using _base_rb_tree_node = base_rb_tree_node<T>;
    using _rb_tree_node = rb_tree_node<T>;

    using iterator = rb_tree_iterator<T>;
    using self = iterator;

    using base_rb_tree_iterator<T>::node;

    // Construct
    rb_tree_iterator() : base_rb_tree_iterator<T>() {};
    rb_tree_iterator(base_ptr n) : base_rb_tree_iterator<T>(n) {};
    rb_tree_iterator(node_ptr n) : base_rb_tree_iterator<T>(n->as_node()) {};
    rb_tree_iterator(const iterator &rhs) : base_rb_tree_iterator<T>((rhs.node)->as_node()) {};

    reference operator*() const {
        return node->as_node()->value;
    }

    pointer operator->() const {
        return &(operator*());
    }

    self &operator++() {
        this->increment();
        return *this;
    }

    self operator++(int) {
        self temp = *this;
        this->increment();
        return temp;
    }

    self &operator--() {
        this->decrement();
        return *this;
    }

    self operator--(int) {
        self temp = *this;
        this->decrement();
        return temp;
    }
};

template <typename T>
struct rb_tree_const_iterator : public base_rb_tree_iterator<T> {
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T*;
    using reference = T &;
    using const_reference = const T&;
    using base_ptr = base_rb_tree_node<T> *;
    using node_ptr = rb_tree_node<T> *;
    using _base_rb_tree_node = base_rb_tree_node<T>;
    using _rb_tree_node = rb_tree_node<T>;

    using iterator = rb_tree_iterator<T>;
    using const_iterator = rb_tree_const_iterator<T>;
    using self = const_iterator;

    using base_rb_tree_iterator<T>::node;

    //construct
    rb_tree_const_iterator() : base_rb_tree_iterator<T>() {};
    rb_tree_const_iterator(base_ptr n) : base_rb_tree_iterator<T>(n) {};
    rb_tree_const_iterator(node_ptr n) : base_rb_tree_iterator<T>(n->as_node()) {};
    rb_tree_const_iterator(const iterator &rhs) : base_rb_tree_iterator<T>((rhs.node)->as_node()) {};

    const_reference operator*() const {
        return node->as_node()->value;
    }

    const_pointer operator->() const {
        return &(operator*());
    }

    self &operator++() {
        this->increment();
        return *this;
    }

    self operator++(int) {
        self temp = *this;
        this->increment();
        return temp;
    }

    self &operator--() {
        this->decrement();
        return *this;
    }

    self operator--(int) {
        self temp = *this;
        this->decrement();
        return temp;
    }
};

template <typename T, typename comp, typename KeyOfValue, typename alloc = mystl::allocator<T>>
class rb_tree {

  public:
    using base_ptr = base_rb_tree_node<T> *;
    using node_ptr = rb_tree_node<T> *;
    using _base_rb_tree_node = base_rb_tree_node<T>;
    using _rb_tree_node = rb_tree_node<T>;

    using allocator_type = alloc;
    using data_allocator = alloc;
    using base_node_allocator = mystl::allocator<_base_rb_tree_node>;
    using node_allocator = mystl::allocator<_rb_tree_node>;

    using value_type = T;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using reference = value_type &;
    using const_reference = const value_type &;
    using key_compare = comp;
    using key_of_value = KeyOfValue;
    using size_type = size_t;
    using iterator = rb_tree_iterator<T>;
    using const_iterator = rb_tree_const_iterator<T>;
    using self = rb_tree<value_type, key_compare, key_of_value, allocator_type>;
    using key_type = decltype(KeyOfValue()(mystl::declval<value_type>()));

  private:
    size_type node_count;
    base_ptr header;
    key_compare _key_comp;
    key_of_value _key_of_value;

  public:
    // Construct
    rb_tree(const key_compare &c = comp(), const key_of_value &k = KeyOfValue()) : _key_comp(c), _key_of_value(k) { rb_tree_init(); }

    //copy construct
    rb_tree(const self& rhs) : _key_comp(comp()), _key_of_value(KeyOfValue()) {
        rb_tree_init();
        node_count = rhs.node_count;
        if(rhs.node_count != 0){
            header->parent = copy_subtree(rhs.root_ptr(), header);
            header->left = rb_tree_min_ptr(header->parent);
            header->right = rb_tree_max_ptr(header->parent);
        }
    }

    //move construct
    rb_tree(self&& rhs) : node_count(mystl::move(rhs.node_count)), header(mystl::move(rhs.header)), _key_comp(comp()), _key_of_value(KeyOfValue()) {
        rhs.node_count = 0;
        rhs.header = nullptr;
    }

    ~rb_tree() {
        clear();
        base_node_allocator::deallocate(header);
    }

    //copy operator
    self& operator=(const self& rhs){
        if(this != &rhs){
            clear();
            node_count = rhs.node_count;
            if(rhs.node_count != 0){
                header->parent = copy_subtree(rhs.root_ptr(), header);
                header->left = rb_tree_min_ptr(header->parent);
                header->right = rb_tree_max_ptr(header->parent);
            }
        }
        return *this;
    }

    //move operator
    self& operator=(self&& rhs){
        clear();
        header = mystl::move(rhs.header);
        node_count = mystl::move(rhs.node_count);
        rhs.node_count = 0;
        rhs.header = nullptr;
        return *this;
    }

    bool empty(){
        return header == header->parent || header == nullptr;
    }

    size_type size() const{
        return node_count;
    }

    // iterator
    iterator begin() const {
        return iterator(leftmost_ptr());
    }

    const_iterator cbegin() const {
        return const_iterator(leftmost_ptr());
    }

    iterator end() const {
        return iterator(header);
    }

    const_iterator cend() const{
        return const_iterator(header);
    }

    iterator root() const {
        return iterator(header->parent);
    }

    const_iterator croot() const {
        return const_iterator(header->parent);
    }

    iterator leftmost() const {
        return iterator(leftmost_ptr());
    }

    iterator rightmost() const {
        return iterator(rightmost_ptr());
    }

    iterator find(const key_type &key){
        return iterator(find_ptr(key));
    }

    const_iterator find(const key_type &key) const {
        return const_iterator(find_ptr(key));
    }

    mystl::pair<iterator, iterator> equal_range(const key_type& key) const {
        return range_of_multi_equal_key(key);
    }

    iterator lower_bound(const key_type& key) {
        return iterator(prev_ptr(find_ptr(key)));
    }

    const_iterator lower_bound(const key_type& key) const {
        return const_iterator(prev_ptr(find_ptr(key)));
    }

    iterator upper_bound(const key_type& key) {
        return iterator(next_ptr(find_ptr(key)));
    }

    const_iterator upper_bound(const key_type& key) const {
        return const_iterator(next_ptr(find_ptr(key)));
    }

    iterator insert_equal(const value_type &value) {
        return iterator(insert_node(value));
    }

    mystl::pair<iterator, bool> insert_unique(const value_type &value) {
        return insert_unique_node(value);
    }

    mystl::pair<const_iterator, bool> const_insert_unique(const value_type &value) {
        return const_insert_unique_node(value);
    }

    template <typename iter>
    void insert_unique(iter first, iter last){
        for(; first != last; ++first)
            insert_unique(*first);
    }

    iterator erase(iterator hint){
        iterator next = iterator(next_ptr(hint.node));
        erase_node(hint);
        return next;
    }

    void erase(iterator first, iterator last){
        if(first == begin() && last == end())
            clear();
        else{
            iterator cur = first;
            for(; cur != last; first = cur){
                ++cur;
                erase_node(first);
            }
        }
    }

    bool erase_unique(const key_type& key){
        return erase_node(key);
    }

    size_type erase_multi(const key_type& key){
        mystl::pair<iterator, iterator> range = range_of_multi_equal_key(key);
        size_type n = mystl::distance(range.first, range.second);
        erase(range.first, range.second);
        return n;
    }

    size_type count_unique(const key_type& key){
        return find_ptr(key) == header ? 0 : 1;
    }
    
    size_type count_multi(const key_type& key){
        size_type n = 0;
        base_ptr mid = find_ptr(key);
        if(mid == header)
            return 0;
        base_ptr cur = mid;
        while(cur != header && !_key_comp(key, _key_of_value(cur->as_node()->value)) && !_key_comp(_key_of_value(cur->as_node()->value), key)){
            cur = next_ptr(cur);
            ++n;
        }
        cur = mid;
        while(cur != header && !_key_comp(key, _key_of_value(cur->as_node()->value)) && !_key_comp(_key_of_value(cur->as_node()->value), key)){
            cur = prev_ptr(cur);
            ++n;
        }
        return n - 1;
    }

    void swap(self& rhs){
        if(this != &rhs){
            mystl::swap(header, rhs.header);
            mystl::swap(node_count, rhs.node_count);
        }
    }

    void swap(self& lhs, self& rhs){
        lhs.swap(rhs);
    }

    /***************************************/
    // private function
  private:
  
    base_ptr &root_ptr() const {
        return header->parent;
    }

    base_ptr &leftmost_ptr() const {
        return header->left;
    }

    base_ptr &rightmost_ptr() const {
        return header->right;
    }

    void rb_tree_init() {
        header = base_node_allocator::allocate();
        header->color = red;
        header->parent = nullptr;
        header->left = header;
        header->right = header;
        node_count = 0;
    }

    node_ptr create_node(const value_type &value) {
        node_ptr p = node_allocator::allocate();
        p->color = red;
        p->left = nullptr;
        p->right = nullptr;
        data_allocator::construct(&(p->value), value);
        return p;
    }

    template <typename... Args>
    node_ptr create_node(Args &&...args) {
        node_ptr p = node_allocator::allocate();
        p->color = red;
        p->left = nullptr;
        p->right = nullptr;
        p->parent = nullptr;
        data_allocator::construct(&(p->value), mystl::forward<Args>(args)...);
        return p;
    }

    // copy value and color
    node_ptr copy_node(const node_ptr &rhs) {
        node_ptr p = create_node(rhs->value);
        p->color = rhs->color;
        return p;
    }

    void destroy_node(node_ptr p) {
        data_allocator::destroy(&(p->value));
        node_allocator::deallocate(p);
    }

    void erase_sub_tree(base_ptr r) {
        while (r != nullptr) {
            erase_sub_tree(r->right);
            base_ptr l = r->left;
            destroy_node(r->as_node());
            --node_count;
            r = l;
        }
    }

    void clear() {
        if (node_count != 0) {
            erase_sub_tree(root_ptr());
            header->left = header;
            header->right = header;
            header->parent = nullptr;
            node_count = 0;
        }
    }

    base_ptr prev_ptr(base_ptr node) const {
        if(node->left != nullptr){
            node = node->left;
            while(node->right != nullptr)
                node = node->right;
        }
        else{
            base_ptr father = node->parent;
            while(father->left == node){
                node = father;
                father = node->parent;
                if(node == header)
                    return header;
            }
            node = father;
        }
        return node;
    }

    base_ptr next_ptr(base_ptr node) const {
        if (node->right != nullptr) {
            node = node->right;
            while (node->left != nullptr)
                node = node->left;
        } 
        else {
            base_ptr father = node->parent;
            while (father->right == node) {
                node = father;
                father = node->parent;
                if(node == header)
                    return header;
            }
            node = father;
        }
        return node;
    }

    //x is root of the copied subtree. parent is the father of the new tree.
    base_ptr copy_subtree(base_ptr x, base_ptr parent){
        if(x == nullptr)
            return nullptr;
        node_ptr new_node = create_node(x->as_node()->value);
        new_node->color = x->color;
        new_node->parent = parent;
        new_node->left = copy_subtree(x->left, new_node);
        new_node->right = copy_subtree(x->right, new_node);
        return new_node;
    }

    base_ptr find_ptr(const key_type &key) const {
        node_ptr cur = header->parent->as_node();
        while (cur != nullptr) {
            auto cur_key = _key_of_value(cur->value);
            if (_key_comp(key, cur_key)) {
                cur = cur->left->as_node();
            } else {
                if (!_key_comp(cur_key, key))
                    return cur;
                cur = cur->right->as_node();
            }
        }
        return header;
    }

    //return two iterator [start, end)
    mystl::pair<iterator, iterator> range_of_multi_equal_key(const key_type& key) const{
        base_ptr mid = find_ptr(key);
        base_ptr cur = mid;
        while(cur != header && !_key_comp(key, _key_of_value(cur->as_node()->value)) && !_key_comp(_key_of_value(cur->as_node()->value), key))
            cur = next_ptr(cur);
        iterator end(cur);
        cur = mid;
        while(cur != header && !_key_comp(key, _key_of_value(cur->as_node()->value)) && !_key_comp(_key_of_value(cur->as_node()->value), key)){
            mid = cur;
            cur = prev_ptr(cur);
        }
        iterator start(mid);
        return mystl::pair(start, end);
    }

    base_ptr rb_tree_min_ptr(base_ptr node){
        while(node->left != nullptr)
            node = node->left;
        return node;
    }

    base_ptr rb_tree_max_ptr(base_ptr node){
        while(node->right != nullptr)
            node = node->right;
        return node;
    }

    base_ptr insert_node(const value_type &value) {
        node_ptr n = create_node(value);
        insert_node_at(get_parent_pos_of_key(_key_of_value(value)), n);
        return n;
    }

    bool erase_node(const key_type &key) {
        base_ptr node = find_ptr(key);
        if(node == header)
            return false;
        if(node == header->left){
            if(node_count == 1){
                header->left = header;
                header->right = header;
            }
            else
                header->left = next_ptr(node);
        }
        else if(node == header->right){
            if(node_count == 1){
                header->left = header;
                header->right = header;
            }
            else
                header->right = prev_ptr(node);
        }
        if(node->left != nullptr && node->right != nullptr)
            exchange_node(node, next_ptr(node));
        node_color original_color = node->color;
        base_ptr father = node->parent;
        base_ptr child = (node->left != nullptr) ? node->left : node->right;
        if (child != nullptr)
            child->parent = father;
        if(father == header)
            father->parent = child;
        else if (father->left == node)
            father->left = child;
        else
            father->right = child;
        destroy_node(node->as_node());
        if (original_color == black)
            rb_tree_erase_reblance(child, father);
        --node_count;
        return true;
    }

    bool erase_node(iterator hint) {
        base_ptr node = hint.node;
        if(node == header || node == nullptr)
            return false;
        if(node == header->left){
            if(node_count == 1){
                header->left = header;
                header->right = header;
            }
            else
                header->left = next_ptr(node);
        }
        else if(node == header->right){
            if(node_count == 1){
                header->left = header;
                header->right = header;
            }
            else
                header->right = prev_ptr(node);
        }
        if(node->left != nullptr && node->right != nullptr)
            exchange_node(node, next_ptr(node));
        node_color original_color = node->color;
        base_ptr father = node->parent;
        base_ptr child = (node->left != nullptr) ? node->left : node->right;
        if (child != nullptr)
            child->parent = father;
        if(father == header)
            father->parent = child;
        else if (father->left == node)
            father->left = child;
        else
            father->right = child;
        destroy_node(node->as_node());
        if (original_color == black)
            rb_tree_erase_reblance(child, father);
        --node_count;
        return true;
    }

    mystl::pair<iterator, bool> insert_unique_node(const value_type &value) {
        node_ptr father = header->as_node();
        key_type key = _key_of_value(value);
        if(node_count != 0){
            father = get_parent_pos_of_key(key)->as_node();
            if (!_key_comp(key, _key_of_value(father->value)) && !_key_comp(_key_of_value(father->value), key))
                return mystl::pair<iterator, bool>(iterator(father), false);
        }
        node_ptr n = create_node(value);
        insert_node_at(father, n);
        return mystl::pair<iterator, bool>(iterator(n), true);
    }

    mystl::pair<const_iterator, bool> const_insert_unique_node(const value_type &value) {
        node_ptr father = header->as_node();
        key_type key = _key_of_value(value);
        if(node_count != 0){
            father = get_parent_pos_of_key(key)->as_node();
            if (!_key_comp(key, _key_of_value(father->value)) && !_key_comp(_key_of_value(father->value), key))
                return mystl::pair<const_iterator, bool>(end(), false);
        }
        node_ptr n = create_node(value);
        insert_node_at(father, n);
        return mystl::pair<const_iterator, bool>(const_iterator(n), true);
    }

    base_ptr get_parent_pos_of_key(const key_type &key) {
        base_ptr x = root_ptr();
        base_ptr father = header;
        while (x != nullptr) {
            father = x;
            x = _key_comp(key, _key_of_value(x->as_node()->value)) ? x->left : x->right;
        }
        return father;
    }

    /**
     *  @brief  Inserts given node to the child of father
     *  @param  father  The parent of node after insert.
     *  @param  node  Node to be inserted.
     *  @return  pointer of node.
     * */
    base_ptr insert_node_at(base_ptr father, node_ptr node) {
        node->parent = father;
        base_ptr base_node = node->as_base();
        // If node is root
        if (father == header) {
            father->parent = base_node;
            father->left = base_node;
            father->right = base_node;
        }
        // node is the left child of father
        else if (_key_comp(_key_of_value(node->value), _key_of_value(father->as_node()->value))) {
            father->left = base_node;
            if (header->left == father)
                header->left = base_node;
        }
        // node is right child of father
        else {
            father->right = base_node;
            if (header->right == father)
                header->right = base_node;
        }
        rb_tree_insert_reblance(base_node);
        ++node_count;
        return base_node;
    }

    /*-------------------------------------------------*\
    |    r <- b -> r    |    r <- b      |   b -> r     |
    |   / \       / \       / \                  / \    |
    |  1   2     3   4     5   6                7   8   |
    \*-------------------------------------------------*/
    void rb_tree_insert_reblance(base_ptr node) {
        // node is not root and it's parent color is red(false)
        while (node != header->parent && !(node->parent->color)) {
            // case 5 and 6
            if (node->parent->parent->right == nullptr || node->parent->parent->right->color == black) {
                // case 5(LL)
                if (node->parent->left == node) {
                    right_rotate(node->parent->parent);
                    node->parent->color = black;
                    node->parent->right->color = red;
                }
                // case 6(LR)
                else {
                    node->color = black;
                    node->parent->parent->color = red;
                    left_rotate(node->parent);
                    right_rotate(node->parent);
                }
            }
            // case 7 and 8
            else if (node->parent->parent->left == nullptr || node->parent->parent->left->color == black) {
                // case 8(RR)
                if (node->parent->right == node) {
                    left_rotate(node->parent->parent);
                    node->parent->color = black;
                    node->parent->left->color = red;
                }
                // case 7(RL)
                else {
                    node->color = black;
                    node->parent->parent->color = red;
                    right_rotate(node->parent);
                    left_rotate(node->parent);
                }
            }
            // case 1 and 2
            else if (node->parent->parent->left == node->parent) {
                node->parent->color = black;
                node->parent->parent->right->color = black;
                node->parent->parent->color = red;
                node = node->parent->parent;
            }
            // case 3 and 4
            else {
                node->parent->color = black;
                node->parent->parent->left->color = black;
                node->parent->parent->color = red;
                node = node->parent->parent;
            }
        }
        header->parent->color = black;
    }

    void rb_tree_erase_reblance(base_ptr node, base_ptr father) {
        while (node != header->parent && (node == nullptr || node->color == black)) {
            base_ptr brother;
            // left child
            if (node == father->left) {
                brother = father->right;
                // the color of brother is red
                if (brother->color == red) {
                    brother->color = black;
                    father->color = red;
                    left_rotate(father);
                }
                node_color father_color = father->color;
                brother = father->right;
                // if the color of brother is black
                if (brother->color == black) {
                    // if brother node has child
                    if (brother->right != nullptr && brother->right->color == red) {
                        left_rotate(brother->parent);
                        brother->color = father_color;
                        brother->left->color = black;
                        brother->right->color = black;
                        // finish, exit
                        node = header->parent;
                    } else if (brother->left != nullptr && brother->left->color == red) {
                        right_rotate(brother);
                        left_rotate(father);
                        father->color = black;
                        brother->color = black;
                        brother->parent->color = father_color;
                        // finish, exit
                        node = header->parent;
                    }
                    // brother has no child
                    else {
                        // the color of father is red
                        if (father_color == red) {
                            brother->color = red;
                            father->color = black;
                            // finish, exit
                            node = header->parent;
                        }
                        // the color of father is black
                        else {
                            brother->color = red;
                            node = father;
                            father = father->parent;
                        }
                    }
                }
            }
            // right child
            else {
                brother = father->left;
                // the color of brother is red
                if (brother->color == red) {
                    brother->color = black;
                    father->color = red;
                    right_rotate(father);
                }
                node_color father_color = father->color;
                brother = father->left;
                // if the color of brother is black
                if (brother->color == black) {
                    // if brother node has child
                    if (brother->left != nullptr && brother->left->color == red) {
                        right_rotate(father);
                        brother->color = father_color;
                        brother->left->color = black;
                        brother->right->color = black;
                        // finish, exit
                        node = header->parent;
                    } else if (brother->right != nullptr && brother->right->color == red) {
                        left_rotate(brother);
                        right_rotate(father);
                        father->color = black;
                        brother->color = black;
                        brother->parent->color = father_color;
                        // finish, exit
                        node = header->parent;
                    }
                    // brother has no child
                    else {
                        // the color of father is red
                        if (father_color == red) {
                            brother->color = red;
                            father->color = black;
                            // finish, exit
                            node = header->parent;
                        }
                        // the color of father is black
                        else{
                            brother->color = red;
                            node = father;
                            father = father->parent;
                        }
                    }
                }
            }
        }
        if (node != nullptr)
            node->color = black;
    }

    /*---------------------------------------*\
    |       p                         p       |
    |      / \                       / \      |
    |     x   d    rotate left      y   d     |
    |    / \       ===========>    / \        |
    |   a   y                     x   c       |
    |      / \                   / \          |
    |     b   c                 a   b         |
    \*---------------------------------------*/
    void left_rotate(base_ptr x) {
        base_ptr y = x->right;
        // deal with node b
        x->right = y->left;
        if (y->left != nullptr)
            y->left->parent = x;
        // deal with node p
        y->parent = x->parent;
        // If x is root
        if (x == header->parent) {
            header->parent = y;
        }
        // x is right child
        else if (x->parent->right == x) {
            x->parent->right = y;
        }
        // x is left child
        else {
            x->parent->left = y;
        }
        // deal with x and y
        y->left = x;
        x->parent = y;
    }

    /*----------------------------------------*\
    |     p                         p          |
    |    / \                       / \         |
    |   d   x      rotate right   d   y        |
    |      / \     ===========>      / \       |
    |     y   a                     b   x      |
    |    / \                           / \     |
    |   b   c                         c   a    |
    \*----------------------------------------*/
    void right_rotate(base_ptr x) {
        base_ptr y = x->left;
        // deal with node c
        x->left = y->right;
        if (y->right != nullptr)
            y->right->parent = x;
        // deal with node p
        y->parent = x->parent;
        // If x is root
        if (x == header->parent) {
            header->parent = y;
        }
        // x is right child
        else if (x->parent->right == x) {
            x->parent->right = y;
        }
        // x is left child
        else {
            x->parent->left = y;
        }
        // deal with x and y
        y->right = x;
        x->parent = y;
    }

    void exchange_node(base_ptr x, base_ptr y) {
        if (x->left != nullptr)
            x->left->parent = y;
        if (x->right != nullptr)
            x->right->parent = y;
        if (y->left != nullptr)
            y->left->parent = x;
        if (y->right != nullptr)
            y->right->parent = x;
        mystl::swap(x->left, y->left);
        mystl::swap(x->right, y->right);
        // If x is root
        if (x == header->parent) {
            // if y is left child
            if (y->parent->left == y)
                y->parent->left = x;
            // y is right child
            else
                y->parent->right = x;
            x->parent->parent = y;
            mystl::swap(x->parent, y->parent);
        }
        // if y is root
        else if (y == header->parent) {
            // if x is left child
            if (x->parent->left == x)
                x->parent->left = y;
            // x is right child
            else
                x->parent->right = y;
            y->parent->parent = x;
            mystl::swap(x->parent, y->parent);
        } else {
            // if y is left child
            if (y->parent->left == y)
                y->parent->left = x;
            // y is right child
            else
                y->parent->right = x;
            // if x is left child
            if (x->parent->left == x)
                x->parent->left = y;
            // x is right child
            else
                x->parent->right = y;
            mystl::swap(x->parent, y->parent);
        }
        mystl::swap(x->color, y->color);
    }
};

template <typename T, typename Compare, typename KeyOfValue>
void print_rb_tree(mystl::rb_tree<T, Compare, KeyOfValue>& tree)
{
    using base_ptr = typename mystl::rb_tree<T, Compare, KeyOfValue>::base_ptr;

    // BFS
    mystl::queue<base_ptr> q;
    q.push(tree.root().node);

    auto header_ptr = tree.end().node; 

    while (!q.empty())
    {
        base_ptr node = q.front();
        q.pop();
        if (!node)
            continue;
        
        if (node == header_ptr)
            continue;

        printf("Node value: %d", node->as_node()->value);
        printf(" | color: %s", (node->color == mystl::black ? "black" : "red"));

        if (node->parent == nullptr || node->parent == header_ptr){
            printf(" | parent: null");
        }
        else{
            printf(" | parent: %d", node->parent->as_node()->value);
        }

        if (node->left){
            printf(" | left child: %d", node->left->as_node()->value);
            q.push(node->left);
        }
        else{
            printf(" | left child: null");
        }

        if (node->right){
            printf(" | right child: %d", node->right->as_node()->value);
            q.push(node->right);
        }
        else{
            printf(" | right child: null");
        }

        printf("\n");
    }
};

}

#endif