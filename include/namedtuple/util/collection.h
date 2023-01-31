/*
	This file makes use of foreach.h to build a few
	useful functions; all under class collection.
*/
#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <array>
#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.
#include "namedtuple/namedtuple.h"
#include "namedtuple/util/foreach.h"

namespace namedtuple {

template<typename T, typename = void>
struct has_cbegin: ::std::false_type {};
template<typename T>
struct has_cbegin<T, ::std::enable_if_t<sizeof(&T::cbegin) != 0>>: ::std::true_type {};

template<typename T>
static ::std::ostream& print_func(::std::ostream& ost, const T& rhs, unsigned indent = 0, bool skip_first = false) {
	::std::string pad(indent, '\t');
	if (not skip_first) {
		ost << pad;
	}
	if constexpr (is_namedtuple_v<T>) {
		ost << "{\n";
		foreach<T>([&](auto int_const) {
			ost << pad << "\t\"" << rhs.get_name(int_const) << "\": ";
			print_func(ost, rhs.get(int_const), indent+1, true);
			if constexpr (int_const() == T::num_members-1) {
				ost << "\n";
			} else {
				ost << ",\n";
			}
		});
		ost << pad << '}';
	} else if constexpr (::std::is_same_v<T, ::std::string>) {
		ost << '"' << rhs << '"';
	} else if constexpr (has_cbegin<T>::value) {
		ost << "[\n";
		auto it = rhs.cbegin();
		for (size_t i = 0; i < rhs.size(); ++i, ++it) {
			print_func(ost, *it, indent+1);
			if (i == rhs.size()-1) {
				ost << "\n";
			} else {
				ost << ",\n";
			}
		}
		ost << pad << ']';
	} else {
		ost << rhs;
	}
	return ost;
}

template<typename NT, typename = ::std::make_integer_sequence<unsigned, NT::num_members>>
struct collection;

template<typename NT, unsigned...indices>
struct collection<NT, ::std::integer_sequence<unsigned, indices...>> {
	static_assert(is_namedtuple_v<NT>, "NT must be a namedtuple");

	static unsigned mismatch(const NT& lhs, const NT& rhs) {
		unsigned i = 0;
		(
			((lhs.template get<indices>() == rhs.template get<indices>()) and bool(++i))
			and ...
		);
		return i;
	}

	static bool equal_func(const NT& lhs, const NT& rhs) {
		return (
			(lhs.template get<indices>() == rhs.template get<indices>())
			and ...
		);
	}
};

} // namespace namedtuple
