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

template<typename NT, typename = ::std::make_integer_sequence<unsigned, NT::num_members>>
struct collection;

template<typename NT, unsigned...indices>
struct collection<NT, ::std::integer_sequence<unsigned, indices...>> {
	static_assert(is_namedtuple_v<NT>, "NT must be a namedtuple");

	static ::std::ostream& print_func(::std::ostream& ost, const NT& rhs, unsigned indent = 0) {
		ost << "{\n";
		foreach<NT>([&](auto int_const) {
			auto &member = rhs.get(int_const);
			typedef ::std::remove_reference_t<decltype(member)> Member;
			ost << ::std::setfill('\t') << ::std::setw(1+indent*1) << ""
			    << "\"" << rhs.get_name(int_const) << "\": ";
			if constexpr (is_namedtuple_v<Member>) {
				collection<Member>::print_func(ost, member, indent+1);
			} else {
				ost << member;
			}
			ost << ",\n";
		});
		ost << ::std::setfill('\t') << ::std::setw(indent) << "" << "}";
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
