#ifndef VGL_ALGORITHM_HPP
#define VGL_ALGORITHM_HPP

#include <algorithm>

namespace vgl
{

using std::max;
using std::min;

/// Component-wise maximum.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Result value.
template<typename T> constexpr typename T::CrtpType max(const T& lhs, const T& rhs) noexcept
{
    typename T::CrtpType ret;
    for(unsigned ii = 0; (ii < N); ++ii)
    {
        ret[ii] = max(lhs[ii], rhs[ii]);
    }
    return ret;
}

/// Component-wise minimum.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Result value.
template<typename T> constexpr typename T::CrtpType min(const T& lhs, const T& rhs) noexcept
{
    typename T::CrtpType ret;
    for(unsigned ii = 0; (ii < T::data_size); ++ii)
    {
        ret[ii] = min(lhs[ii], rhs[ii]);
    }
    return ret;
}

}

#endif
