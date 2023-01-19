/*
	This flie implements templates for iterating a namedtuple.
*/
#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.

namespace namedtuple {

namespace detail {

template<unsigned m, unsigned c, unsigned...i, typename Func>
void foreach(::std::integer_sequence<unsigned, i...>, Func func) {
	(func(::std::integral_constant<unsigned, m*i+c>{}), ...);
}

} // namespace detail

template<typename NT, typename Func>
void foreach(Func func) {
	detail::foreach<1, 0>(::std::make_integer_sequence<unsigned, NT::num_members>{}, func);
}

template<typename NT, typename Func>
void foreach_reversed(Func func) {
	detail::foreach<unsigned(-1), NT::num_members-1>(
		::std::make_integer_sequence<unsigned, NT::num_members>{}, func
	);
}

} // namespace namedtuple
