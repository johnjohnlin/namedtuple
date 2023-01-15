/*
	This file makes use of foreach.h to build a few
	useful functions; all under class collection.
*/
#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <array>
#include <iosfwd>
#include <string>
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.
#include "namedtuple/namedtuple.h"
#include "namedtuple/util/foreach.h"

namespace namedtuple {

template<typename NT, typename = ::std::make_integer_sequence<unsigned, NT::num_members>>
struct collection;

template<typename NT, unsigned...indices>
struct collection<NT, ::std::integer_sequence<unsigned, indices...>> {
	static_assert(is_namedtuple_v<NT>, "NT must be a namedtuple");

	static ::std::ostream& print_func(::std::ostream& ost, const NT& rhs) {
		ost << "{\n";
		foreach<NT>([&](auto int_const) {
			ost << rhs.get_name(int_const) << ": " << rhs.get(int_const) << ",\n";
		});
		ost << "}";
		return ost;
	}

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

	static ::std::array<::std::string, NT::num_members> get_tuple_names() {
		::std::array<::std::string, NT::num_members> ret;
		((ret[indices] = NT::template get_name<indices>()), ...);
		return ret;
	}
};

} // namespace namedtuple
