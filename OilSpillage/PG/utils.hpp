#pragma once

#include <type_traits>

// constexpr integral power function for computing
// (main use: base as 2 to compute bit values at compile-time)
template <typename Integer> // TODO static_assert( is_integer_v<T_Integer> )
inline Integer constexpr cPow(Integer base, Integer exp) noexcept {
	static_assert(std::is_integral_v<Integer>, "Template arg Integer must be integral!");
	Integer  retval(1);
	while ( exp --> 0 )
		retval *= base;
	return retval;
}