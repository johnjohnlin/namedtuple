/*
	This is the only file you need to define a compatible namedtuple
*/
#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <iosfwd>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
// Other libraries' .h files.
// Your project's .h files.
#define MAKE_NAMEDTUPLE(...)\
	static constexpr unsigned namedtuple_tag = 0u;\
	auto get_tuple() { return ::std::forward_as_tuple(__VA_ARGS__); }\
	auto get_tuple() const { return ::std::forward_as_tuple(__VA_ARGS__); }\
	static const ::std::vector<::std::string>& get_names() {\
		static ::std::vector<::std::string> names = namedtuple::split_member_names(#__VA_ARGS__);\
		return names;\
	}\
	template<unsigned i> auto& get() { return ::std::get<i>(get_tuple()); }\
	template<unsigned i> auto& get() const { return ::std::get<i>(get_tuple()); }\
	template<unsigned i> static const ::std::string& get_name() { return get_names()[i]; }\
	template<unsigned i> auto& get(::std::integral_constant<unsigned, i>) { return ::std::get<i>(get_tuple()); }\
	template<unsigned i> auto& get(::std::integral_constant<unsigned, i>) const { return ::std::get<i>(get_tuple()); }\
	template<unsigned i> static const ::std::string& get_name(::std::integral_constant<unsigned, i>) { return get_names()[i]; }\
	static constexpr unsigned num_members = ::std::tuple_size_v<decltype(::std::make_tuple(__VA_ARGS__))>;

namespace namedtuple {

// is_namedtuple(_v)
template<typename T, typename = void>
struct is_namedtuple: ::std::false_type {};
template<typename T>
struct is_namedtuple<
	T,
	::std::enable_if_t<sizeof(T::namedtuple_tag) != 0>
>: public ::std::true_type {};
template<typename T>
inline constexpr bool is_namedtuple_v = is_namedtuple<T>::value;
// is_namedtuple_of_id(_v)
template<typename T, unsigned tag_id, typename = void>
struct is_namedtuple_of_id: ::std::false_type {};
template<typename T, unsigned tag_id>
struct is_namedtuple_of_id<
	T, tag_id,
	::std::enable_if_t<tag_id == T::nametuple_tag>
>: public ::std::true_type {};
template<typename T, unsigned tag_idx>
inline constexpr bool is_namedtuple_of_id_v = is_namedtuple_of_id<T, tag_idx>::value;
// num_members(_v)

::std::vector<::std::string> split_member_names(const char *members) {
	::std::vector<::std::string> ret;
	if (*members == '\0') {
		return ret;
	}
	for (size_t i = 0;;) {
		size_t j = i;
		while (members[j] != ',' and members[j] != '\0') {
			++j;
		}
		ret.emplace_back(members+i, members+j);
		i = j;
		if (members[i] == '\0') {
			break;
		}
		i += 2;
	}
	return ret;
}

} // namespace namedtuple
