/*
	This file makes use of collection.h to define global
	namespace operators for namedtuples with SFINAE
*/
#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <iosfwd>
#include <string>
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.
#include "namedtuple/namedtuple.h"
#include "namedtuple/util/collection.h"

template<typename NT, typename = ::std::enable_if_t<namedtuple::is_namedtuple_v<NT>>>
static ::std::ostream&
operator<<(::std::ostream& ost, const NT &rhs) {
	return namedtuple::collection<NT>::print_func(ost, rhs);
}

template<typename NT, typename = ::std::enable_if_t<namedtuple::is_namedtuple_v<NT>>>
static bool operator==(const NT &lhs, const NT &rhs) {
	return namedtuple::collection<NT>::equal_func(lhs, rhs);
}

template<typename NT, typename = ::std::enable_if_t<namedtuple::is_namedtuple_v<NT>>>
static bool operator!=(const NT &lhs, const NT &rhs) {
	return not (lhs == rhs);
}
