#ifndef UNINIT_H
#define UNINIT_H

#include "algobase.h"
#include "type_traits.h"
#include "construct.h"
#include "iterator.h"

namespace mystl{
    template <typename InputIter, typename ForwardIter>
    ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result){
        auto cur = result;
        for(; first != last; first++){
            mystl::construct(&*cur, *first);
            cur++;
        }
        return cur;
    }

    template <typename InputIter, typename ForwardIter>
    ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result){
        for(; first != last; first++, result++){
            mystl::construct(&*result, mystl::move(*first));
        }
        return result;
    }

    template <typename ForwardIter, typename Size, typename T>
    ForwardIter uninitialized_fill(ForwardIter pos, Size n, const T& value){
        for(; n > 0; n--, pos++){
            mystl::construct(&*pos, value);
        }
        return pos;
    }

    template <typename ForwardIter, typename T>
    ForwardIter uninitialized_fill(ForwardIter first, ForwardIter last, const T& value){
        for(; first != last; first++){
            mystl::construct(&*first, value);
        }
        return last;
    }
}

#endif