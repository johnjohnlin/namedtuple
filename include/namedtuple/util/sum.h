#pragma once
// Direct include
#include "namedtuple/namedtuple.h"
// C system headers
// C++ standard library headers
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.

namespace namedtuple {

namespace detail {

template<unsigned m, unsigned c, unsigned...i, typename ToSummable, typename Init>
constexpr auto sum(::std::integer_sequence<unsigned, i...>, ToSummable to_summable, const Init& init) {
	return (init + ... + to_summable(::std::integral_constant<unsigned, m*i+c>{}));
}

template<unsigned m, unsigned c, unsigned...i, typename ToSummable>
constexpr auto sum(::std::integer_sequence<unsigned, i...>, ToSummable to_summable) {
	return (... + to_summable(::std::integral_constant<unsigned, m*i+c>{}));
}

} // namespace detail

template<typename NT, typename ToSummable, typename Init>
constexpr auto sum(ToSummable to_summable, const Init& init) {
	return detail::sum<1, 0>(
		::std::make_integer_sequence<unsigned, NT::num_members>{}, to_summable, init
	);
}

template<typename NT, typename ToSummable>
constexpr auto sum(ToSummable to_summable) {
	return detail::sum<1, 0>(
		::std::make_integer_sequence<unsigned, NT::num_members>{}, to_summable
	);
}

template<typename NT, typename ToSummable, typename Init>
constexpr auto sum_reversed(ToSummable to_summable, const Init& init) {
	return detail::sum<unsigned(-1), NT::num_members-1>(
		::std::make_integer_sequence<unsigned, NT::num_members>{}, to_summable, init
	);
}

template<typename NT, typename ToSummable>
constexpr auto sum_reversed(ToSummable to_summable) {
	return detail::sum<unsigned(-1), NT::num_members-1>(
		::std::make_integer_sequence<unsigned, NT::num_members>{}, to_summable
	);
}

} // namespace namedtuple
