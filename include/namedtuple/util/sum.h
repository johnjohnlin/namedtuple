#pragma once
// Direct include
#include "namedtuple.h"
// C system headers
// C++ standard library headers
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.

namespace namedtuple {

namespace detail {

template<unsigned m, unsigned c, unsigned...i, typename ToSummable, typename Init>
void sum(ToSummable to_summable, const Init& init) {
	(init + ... + to_summable(::std::integral_constant<unsigned, m*i+c>{}));
}

template<unsigned m, unsigned c, unsigned...i, typename ToSummable>
void sum(ToSummable to_summable) {
	(... + to_summable(::std::integral_constant<unsigned, m*i+c>{}));
}

} // namespace detail

template<typename NT, typename ToSummable, typename Init>
auto sum(ToSummable to_summable, const Init& init) {
	return detail::sum<1, 0>(
		::std::make_integer_sequence<unsigned, NT::num_members>{}, to_summable, init
	);
}
template<typename NT, typename ToSummable>
auto sum(ToSummable to_summable) {
	return detail::sum<1, 0>(
		::std::make_integer_sequence<unsigned, NT::num_members>{}, to_summable
	);
}

template<typename NT, typename ToSummable, typename Init>
auto sum_reversed(ToSummable to_summable, const Init& init) {
	return detail::sum<unsigned(-1), NT::num_members-1>(
		::std::make_integer_sequence<unsigned, NT::num_members>{}, to_summable, init
	);
}

template<typename NT, typename ToSummable>
auto sum_reversed(ToSummable to_summable) {
	return detail::sum<unsigned(-1), NT::num_members-1>(
		::std::make_integer_sequence<unsigned, NT::num_members>{}, to_summable
	);
}

} // namespace namedtuple
