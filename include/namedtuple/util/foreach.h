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

template<unsigned...i, typename Func>
void foreach(::std::integer_sequence<unsigned, i...>, Func func) {
	(func(::std::integral_constant<unsigned, i>{}), ...);
}

template<unsigned...i, typename Func>
void foreach_reversed(::std::integer_sequence<unsigned, i...>, Func func) {
	constexpr unsigned first_i = sizeof...(i)-1;
	(func(::std::integral_constant<unsigned, first_i-i>{}), ...);
}

} // namespace detail

template<typename NT, typename Func>
void foreach(Func func) {
	detail::foreach(::std::make_integer_sequence<unsigned, NT::num_members>{}, func);
}

template<typename NT, typename Func>
void foreach_reversed(Func func) {
	detail::foreach_reversed(::std::make_integer_sequence<unsigned, NT::num_members>{}, func);
}

} // namespace namedtuple
