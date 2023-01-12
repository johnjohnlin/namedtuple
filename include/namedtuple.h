#include <string>
#include <type_traits>

#define DEFINE_NAMEDTUPLE(name) \
struct name {\
	static constexpr unsigned kCounterBegin = __COUNTER__+1;\
	using Str = ::std::string;\
	template<unsigned x> using Constant = ::std::integral_constant<unsigned, x>;
#define END_DEFINE_NAMEDTUPLE(name)\
	template<unsigned x> auto get() { return get_member(Constant<x>{}); }\
	template<unsigned x> Str get_name() { return get_name(Constant<x>{}); }\
	static constexpr int kCounterEnd = __COUNTER__;\
	static constexpr int num_member = kCounterBegin - kCounterEnd;\
};
#define TYPE(...) __VA_ARGS__
#define MEMBER2(tname, name, cnt) \
	static Str get_name(Constant<cnt-kCounterBegin>) { return ::std::string(#name); }\
	auto& get_member(Constant<cnt-kCounterBegin>) { return name;}\
	tname name;
#define MEMBER(tname, name) MEMBER2(tname, name, __COUNTER__)
