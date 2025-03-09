#ifndef ALGOBASE_H
#define ALGOBASE_H

namespace mystl{

    template <typename T>
    const T& max(const T& lhs, const T& rhs){
        return lhs > rhs ? lhs : rhs;
    }

    template <typename T>
    const T& min(const T& lhs, const T& rhs){
        return lhs < rhs ? lhs : rhs;
    }

    template <typename InputIter1, typename InputIter2>
    bool equal(InputIter1 first1, InputIter1 last, InputIter2 first2){
        for(; first1 != last; first1++, first2++){
            if(*first1 != *first2)
                return false;
        }
        return true;
    }

    template <typename InputIter, typename OutputIter>
    OutputIter copy(InputIter first, InputIter last, OutputIter result){
        for(; first != last; first++, result++){
            *result = *first;
        }
        return result;
    }

    template <typename InputIter, typename OutputIter>
    OutputIter copy_backward(InputIter first, InputIter last, OutputIter result){
        for(; first <= last; last--, result--){
            *result = *last;
        }
        return result;
    }

    template <typename InputIter, typename OutputIter>
    OutputIter move(InputIter first, InputIter last, OutputIter result){
        for(; first != last; first++, result++){
            *result = mystl::move(*first);
        }
        return result;
    }
}

#endif