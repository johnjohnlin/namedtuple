/*
	This is the only file you need to define a compatible namedtuple
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
#define DEFINE_NAMEDTUPLE(name) \
struct name {\
	static constexpr unsigned kCounterBegin = __COUNTER__+1;\
	using Str = ::std::string;\
	template<unsigned x> using Constant = ::std::integral_constant<unsigned, x>;\
	static constexpr bool is_namedtuple = true;
#define END_DEFINE_NAMEDTUPLE(name)\
	template<unsigned x> auto& get() { return get(Constant<x>{}); }\
	template<unsigned x> const auto& get() const { return get(Constant<x>{}); }\
	template<unsigned x> static Str get_name() { return get_name(Constant<x>{}); }\
	static constexpr int kCounterEnd = __COUNTER__;\
	static constexpr int num_members = kCounterEnd - kCounterBegin;\
};
#define NT_TYPE(...) __VA_ARGS__
#define _NT_MEMBER(tname, name, cnt) \
	auto& get(Constant<cnt-kCounterBegin>) { return name;}\
	const auto& get(Constant<cnt-kCounterBegin>) const { return name;}\
	static Str get_name(Constant<cnt-kCounterBegin>) { return ::std::string(#name); }\
	tname name;
#define NT_MEMBER(tname, name) _NT_MEMBER(NT_TYPE(tname), name, __COUNTER__)

namespace namedtuple {

template<typename T, typename = void>
struct is_namedtuple: ::std::false_type {};
template<typename T>
struct is_namedtuple<T, typename ::std::enable_if_t<T::is_namedtuple>>: public ::std::true_type {};
template<typename T> inline constexpr bool is_namedtuple_v = is_namedtuple<T>::value;

} // namespace namedtuple
