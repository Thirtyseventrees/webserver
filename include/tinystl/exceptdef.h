#ifndef EXCEPTDEF_H
#define EXCEPTDEF_H

#include <stdexcept>

namespace mystl{

    #define THROW_ERROR_RANGE_IF(expr, what) \
    if((expr)) throw std::invalid_argument(what)

    #define THROW_OUT_OF_RANGE_IF(expr, what) \
    if ((expr)) throw std::out_of_range(what)
}

#endif