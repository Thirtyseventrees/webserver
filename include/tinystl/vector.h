#ifndef VECTOR_H
#define VECTOR_H

#include "algobase.h"
#include "allocator.h"
#include "exceptdef.h"
#include "uninitialized.h"

namespace mystl{

    template <typename T, typename alloc = mystl::allocator<T>>
    class vector{

    public:
        using allocator_type = alloc;
        using data_allocator = alloc;

        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using iterator = pointer;
        using const_iterator = const_pointer;
        
        allocator_type get_allocator(){
            return data_allocator();
        }

    protected:
        iterator _begin;        //The first element
        iterator _end;          //The next position of the end element
        iterator _cap;          //The end of storage
    
    public:
        iterator begin() const noexcept{
            return _begin;
        }

        iterator end() const noexcept{
            return _end;
        }

        size_type size() const noexcept{
            return static_cast<size_type>(_end - _begin);
        }

        size_type capacity() const noexcept{
            return static_cast<size_type>(_cap - _begin);
        }
        
        size_type max_size() const noexcept{
            return static_cast<size_type>(-1) / sizeof(T);
        }

        bool empty() const noexcept{
            return _begin == _end;
        }

        reference operator[](size_type n){
            return *(_begin + n);
        }

        const_reference operator[](size_type n) const{
            return *(_begin + n);
        }

        reference front(){
            return *(_begin);
        }

        const_reference front() const{
            return *(_begin);
        }

        reference back(){
            return *(_end - 1);
        }

        const_reference back() const{
            return *(_end - 1);
        }
        
        //Construct and operator
        vector(){
            _begin = data_allocator::allocate(16);
            _end = _begin;
            _cap = _begin + 16;
        }

        vector(size_type n){
            fill_init(n, value_type());
        }

        vector(size_type n, const value_type& value){
            fill_init(n, value);
        }

        //Copy construct
        vector(const vector<T>& v){
            range_init(v._begin, v._end);
        }

        //Move construct
        vector(vector<T>&& v) : _begin(v._begin), _end(v._end), _cap(v._cap)
        {
            v._begin = nullptr;
            v._end = nullptr;
            v._cap = nullptr;
        }

        //Copy operator
        vector<T>& operator=(const vector<T>& rhs){
            if(this != &rhs){
                const auto len = rhs.size();
                if(len > capacity()){
                    vector<T> tmp(rhs);
                    swap(tmp);
                }
                else if(len > size()){
                    mystl::copy(rhs._begin, rhs._begin + size(), _begin);
                    mystl::uninitialized_copy(rhs._begin + size(), rhs._end, _end);
                    _end = _begin + len;
                }
                else{
                    auto i = mystl::copy(rhs._begin, rhs._end, _begin);
                    data_allocator::destroy(i, _end);
                    _end = _begin + len;
                }
            }
            return *this;
        }

        //Move operator
        vector<T>& operator=(vector<T>&& rhs){
            if(this != &rhs){
                data_allocator::destroy(_begin, _end);
                data_allocator::deallocate(_begin, capacity());
                _begin = rhs._begin;
                _end = rhs._end;
                _cap = rhs._cap;
                rhs._begin = nullptr;
                rhs._end = nullptr;
                rhs._cap = nullptr;
            }
            return *this;
        }

        ~vector(){
            data_allocator::destroy(_begin, _end);
            data_allocator::deallocate(_begin, capacity());
            _begin = _end = _cap = nullptr;
        }

        /******************/

        reference at(size_type n){
            THROW_OUT_OF_RANGE_IF((n > size()), "out of range");
            return (*this)[n];
        }

        const_reference at(size_type n) const{
            THROW_OUT_OF_RANGE_IF((n > size()), "out of range");
            return (*this)[n];
        }

        template <typename... Args>
        void emplace_back(Args&& ...args){
            if(_end != _cap){
                data_allocator::construct(_end, mystl::forward<Args>(args)...);
                _end++;
            }
            else{
                realloc_and_emplace(_end, mystl::forward<Args>(args)...);
            }
        }

        void push_back(value_type&& value){
            emplace_back(mystl::move(value));
        }

        void push_back(const value_type& value){
            if(_end != _cap){
                data_allocator::construct(_end, value);
                _end++;
            }
            else{
                realloc_and_insert(_end, value);
            }
        }

        void pop_back(){
            _end--;
            data_allocator::destroy(_end);
        }

        void reserve(size_type n){
            if(capacity() < n){
                auto new_begin = data_allocator::allocate(n);
                auto new_end = mystl::uninitialized_move(_begin, _end, new_begin);
                data_allocator::deallocate(_begin, capacity());
                _begin = new_begin;
                _end = new_end;
                _cap = _begin + n;
            }
        }

        void resize(size_type n, const value_type& value){
            if(n < size()){
                erase(_begin + n, _end);
            }
            else{
                insert(_end, n - size(), value);
            }
        }

        void shrink_to_fit(){
            if(_cap > _end){
                size_type new_size = size();
                auto new_begin = data_allocator::allocate(new_size);
                auto new_end = mystl::uninitialized_move(_begin, _end, new_begin);
                data_allocator::deallocate(_begin, _cap - _begin);
                _begin = new_begin;
                _end = _cap = new_end;
            }
        }

      /**
       *  @brief  Remove element at given position.
       *  @param  pos Iterator pointing to element to be erased.
       *  @return  An iterator pointing to the next element (or end()).
       *
       *  This function will erase the element at the given position and thus
       *  shorten the %vector by one.
       **/
        iterator erase(const_iterator pos){
            iterator xpos = end() - 1;
            if(pos < begin() || pos >= end())
                return end();
            else if(pos + 1 != end()){
                xpos = begin() + (pos - begin());
                mystl::move(xpos + 1, _end, xpos);
            }
            data_allocator::destroy(_end - 1);
            _end--;
            return xpos;
        }

        /**
       *  @brief  Remove element between two given iterators.
       *  @param  first The iterator of the first element.
       *  @param  last  The iterator of the last element.
       *  @return  An iterator pointing to the next element (or end()).
       *
       *  This function will erase the element at the given position and thus
       *  shorten the %vector by one.
       **/
        iterator erase(const_iterator first, const_iterator last){
            iterator xlast = _begin + (last - begin());
            iterator xfirst = _begin + (first - begin());
            THROW_ERROR_RANGE_IF((first < begin() || last > end() || first >= last), "Iterator first or last error");
            const auto erase_size = last - first;
            const auto move_size = static_cast<size_type>(mystl::min(erase_size, _end - last));
            data_allocator::destroy(mystl::move(xlast, xlast + move_size, xfirst), _end);
            _end = _end - erase_size;
            return xfirst;
        }

        /**
       *  @brief  Inserts given value into %vector before specified iterator.
       *  @param  pos  A const_iterator into the %vector.
       *  @param  value  Data to be inserted.
       *  @return  An iterator that points to the inserted data.
       * */
        iterator insert(const_iterator pos, const value_type& value){
            iterator xpos = begin() + (pos - begin());
            const auto n = pos - _begin;
            if(xpos < begin() || xpos > end())
                return end();
            else if(_end != _cap){
                if(xpos == _end){
                    data_allocator::construct(_end, value);
                    _end++;
                }
                else{
                    data_allocator::construct(_end, *(_end - 1));
                    mystl::copy_backward(xpos, _end - 2, _end - 1);
                    *xpos = mystl::move(value);
                    _end++;
                }
            }
            //_end == _cap
            else{
                realloc_and_insert(xpos, value);
            }
            return _begin + n;
        }

        /**
       *  @brief  Inserts n given values into %vector before specified iterator.
       *  @param  pos  A const_iterator into the %vector.
       *  @param  n  The number of inserter element.
       *  @param  value  Data to be inserted.
       *  @return  An iterator that points to the first inserted data.
       * */
        iterator insert(const_iterator pos, size_type n, const value_type& value){
            THROW_OUT_OF_RANGE_IF((pos < begin() || pos > end()), "out of range");
            iterator xxpos = _begin + (pos - begin());
            const size_type xpos = static_cast<size_type>(pos - _begin);
            if(n <= 0)
                return xxpos;
            if(static_cast<size_type>(_cap - _end) > n){
                _end = mystl::uninitialized_move(xxpos, _end, xxpos + n);
                mystl::uninitialized_fill(xxpos, n, value);
                return xxpos;
            }
            else{
                realloc_and_insert(xxpos, n, value);
                return _begin + xpos;
            }
        }

        /**
       *  @brief  Inserts n given values into %vector before specified iterator.
       *  @param  pos  A const_iterator into the %vector.
       *  @param  first  The iterator of the first inserted element.
       *  @param  last  The iterator of the last inserted element.
       *  @return  An iterator that points to the first inserted data.
       * */
        template <typename IIter>
        iterator insert(const_iterator pos, IIter first, IIter last){
            THROW_OUT_OF_RANGE_IF((pos < begin() || pos > end()), "out of range");
            iterator xxpos = _begin + (pos - begin());
            const size_type xpos = static_cast<size_type>(pos - _begin);
            const size_type n = static_cast<size_type>(last - first);
            if(n <= 0)
                return xxpos;
            if(static_cast<size_type>(_cap - _end) > n){
                _end = mystl::uninitialized_move(xxpos, _end, xxpos + n);
                mystl::uninitialized_copy(first, last, xxpos);
                return xxpos;
            }
            else{
                realloc_and_insert(xxpos, first, last);
                return _begin + xpos;
            }
        }

    /********************/

        //Reload some operator
        bool operator==(const vector<T>& rhs){
            if(size() != rhs.size())
                return false;
            return mystl::equal(begin(), end(), rhs.begin());
        }

        bool operator!=(const vector<T>& rhs){
            return !(*this == rhs);
        }

        void swap(vector<T>& rhs) noexcept{
            if(this != &rhs){
                mystl::swap(_begin, rhs._begin);
                mystl::swap(_end, rhs._end);
                mystl::swap(_cap, rhs._cap);
            }
        }

    //Private help function
    private:
        void realloc_and_insert(iterator pos, const value_type& value){
            const size_type old_size = size();
            const size_type new_size = old_size != 0 ? old_size * 2 : 1;
            iterator new_begin = data_allocator::allocate(new_size);
            iterator new_end = new_begin;
            try{
                new_end = mystl::uninitialized_move(_begin, pos, new_begin);
                data_allocator::construct(new_end, value);
                new_end++;
                new_end = mystl::uninitialized_move(pos, _end, new_end);
            }
            catch(...){
                data_allocator::destroy(new_begin, new_end);
                data_allocator::deallocate(new_begin, new_size);
                throw;
            }

            // destroy the original vector
            data_allocator::destroy(_begin, _end);
            data_allocator::deallocate(_begin, old_size);

            _begin = new_begin;
            _end = new_end;
            _cap = _begin + new_size;
        }

        void realloc_and_insert(iterator pos, size_type n, const value_type& value){
            const size_type new_size = (size() + n) * 2;
            const size_type xpos = static_cast<size_type>(pos - _begin);
            iterator new_begin = data_allocator::allocate(new_size);
            iterator new_end = new_begin;
            try{
                new_end = mystl::uninitialized_move(_begin, pos, new_begin);
                new_end = mystl::uninitialized_fill(new_begin + xpos, n, value);
                new_end = mystl::uninitialized_move(pos, _end, new_end);
            }
            catch(...){
                data_allocator::destroy(new_begin, new_end);
                data_allocator::deallocate(new_begin, new_size);
                throw;
            }
            data_allocator::deallocate(_begin, _cap - _begin);
            _begin = new_begin;
            _end = new_end;
            _cap = _begin + new_size;
        }

        template <typename IIter>
        void realloc_and_insert(iterator pos, IIter first, IIter last){
            const size_type n = static_cast<size_type>(last - first);
            const size_type new_size = (size() + n) * 2;
            const size_type xpos = static_cast<size_type>(pos - _begin);
            iterator new_begin = data_allocator::allocate(new_size);
            iterator new_end = new_begin;
            try{
                new_end = mystl::uninitialized_move(_begin, pos, new_begin);
                new_end = mystl::uninitialized_copy(first, last, new_begin + xpos);
                new_end = mystl::uninitialized_move(pos, _end, new_end);
            }
            catch(...){
                data_allocator::destroy(new_begin, new_end);
                data_allocator::deallocate(new_begin, new_size);
                throw;
            }
            data_allocator::deallocate(_begin, _cap - _begin);
            _begin = new_begin;
            _end = new_end;
            _cap = _begin + new_size;
        }

        template <typename... Args>
        void realloc_and_emplace(iterator pos, Args&& ...args){
            const size_type old_size = size();
            const size_type new_size = old_size != 0 ? old_size * 2 : 1;
            iterator new_begin = data_allocator::allocate(new_size);
            iterator new_end = new_begin;
            try{
                new_end = mystl::uninitialized_move(_begin, pos, new_begin);
                data_allocator::construct(new_end, mystl::forward<Args>(args)...);
                new_end++;
                new_end = mystl::uninitialized_move(pos, _end, new_end);
            }
            catch(...){
                data_allocator::destroy(new_begin, new_end);
                data_allocator::deallocate(new_begin, new_size);
                throw;
            }

            // destroy the original vector
            data_allocator::destroy(_begin, _end);
            data_allocator::deallocate(_begin, old_size);

            _begin = new_begin;
            _end = new_end;
            _cap = _begin + new_size;
        }

        void range_init(iterator first, iterator last){
            size_type n = static_cast<size_type>(last - first);
            _begin = data_allocator::allocate(n);
            _end = _begin + n;
            _cap = _end;
            mystl::uninitialized_copy(first, last, _begin);
        }

        void fill_init(size_type n, const value_type& value){
            _begin = allocate_and_fill(n, value);
            _end = _begin + n;
            _cap = _end;
        }

        iterator allocate_and_fill(size_type n, const value_type& value){
            iterator result = data_allocator::allocate(n);
            mystl::uninitialized_fill(result, n, value);
            return result;
        }

    };

    template <typename T>
    struct is_mystl_vector : mystl::false_type {};

    template <typename T, typename A>
    struct is_mystl_vector<mystl::vector<T, A>> : mystl::true_type {};

}

#endif