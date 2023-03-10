// Direct include
// C system headers
// C++ standard library headers
#include <array>
#include <sstream>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
// Other libraries' .h files.
#include <gtest/gtest.h>
// Your project's .h files.
#include "namedtuple/namedtuple.h"
#include "namedtuple/util/foreach.h"
#include "namedtuple/util/sum.h"

using namespace std;

struct S1 {
	long            d_l;
	unsigned        d_u;
	array<short, 2> d_s2;
	MAKE_NAMEDTUPLE(d_l, d_u, d_s2)
};

struct S2 {
	S1     d_s1;
	string d_str;
	MAKE_NAMEDTUPLE(d_s1,    d_str)
};

struct S1_native {
	long            d_l;
	unsigned        d_u;
	array<short, 2> d_s2;
};

struct S2_native {
	S1     d_s1;
	string d_str;
};

TEST(Basic, Access) {
	// namedtuple is exactly the same as native type!
	static_assert(sizeof(S1) == sizeof(S1_native));
	static_assert(sizeof(S2) == sizeof(S2_native));
	// it is standard layout as its members standard layout
	static_assert(is_standard_layout<S1>::value);
	// you can use ::num_members
	static_assert(S1::num_members == 3);
	static_assert(S2::num_members == 2);
	// is_namedtuple_v is the type traits
	static_assert(namedtuple::is_namedtuple_v<S1>);
	static_assert(namedtuple::is_namedtuple_v<S2>);
	static_assert(not namedtuple::is_namedtuple_v<S1_native>);
	static_assert(not namedtuple::is_namedtuple_v<S2_native>);

	S1 s1;
	S2 s2;

	// written by name and read by tuple
	s1.d_l = 123;
	s1.d_u = 456;
	s1.d_s2[0] = 78;
	s1.d_s2[1] = 90;
	s2.d_s1.d_u = 5566;
	s2.d_str = "pekopeko";
	long&     d_l = s1.get<0>();
	unsigned& d_u = s1.get<1>();
	short&    d_s20 = s1.get<2>()[0];
	short&    d_s21 = s1.get<2>()[1];
	unsigned& d_s1_u = s2.get<0>().template get<1>();
	string&   d_str = s2.get<1>();
	EXPECT_EQ(d_l, 123);
	EXPECT_EQ(d_u, 456);
	EXPECT_EQ(d_s20, 78);
	EXPECT_EQ(d_s21, 90);
	EXPECT_EQ(d_s1_u, 5566);
	EXPECT_EQ(d_str, "pekopeko");

	// written by tuple and read by name
	s1.get<0>() = 111;
	s1.get<1>() = 222;
	s1.get<2>()[0] = 333;
	s1.get<2>()[1] = 444;
	s2.get<0>().get<1>() = 6655;
	s2.get<1>() = "nicomaki";
	EXPECT_EQ(s1.d_l, 111);
	EXPECT_EQ(s1.d_u, 222);
	EXPECT_EQ(s1.d_s2[0], 333);
	EXPECT_EQ(s1.d_s2[1], 444);
	EXPECT_EQ(s2.d_s1.d_u, 6655);
	EXPECT_EQ(s2.d_str, "nicomaki");

	// they are exactly the same pointer
	EXPECT_EQ(&s1.d_l     , &d_l);
	EXPECT_EQ(&s1.d_u     , &d_u);
	EXPECT_EQ(&s1.d_s2[0] , &d_s20);
	EXPECT_EQ(&s1.d_s2[1] , &d_s21);
	EXPECT_EQ(&s2.d_s1.d_u, &d_s1_u);
	EXPECT_EQ(&s2.d_str   , &d_str);

	// get member name (it is static)
	EXPECT_EQ(S1::get_name(0), "d_l");
	EXPECT_EQ(S1::get_name(1), "d_u");
	EXPECT_EQ(S1::get_name(2), "d_s2");
	EXPECT_EQ(S2::get_name(0), "d_s1");
	EXPECT_EQ(S2::get_name(1), "d_str");
}

struct S3 {
	int    d_i;
	string d_str;
	MAKE_NAMEDTUPLE(d_i, d_str)
};

TEST(Generic, Foreach) {
	static_assert(S3::num_members == 2);

	S3 s3;
	s3.d_i = 123;
	s3.d_str = "abc";

	// List the member names
	// int_const is ::std::integral_constant<unsigned, i>
	{
		array<string, S3::num_members> member_names;
		namedtuple::foreach<S3>([&s3, &member_names](auto int_const) {
			constexpr unsigned i = int_const();
			member_names[i] = S3::get_name(i);
		});
		EXPECT_EQ(member_names[0], "d_i");
		EXPECT_EQ(member_names[1], "d_str");
	}
	// List the member names (same as above)
	{
		array<string, S3::num_members> member_names;
		namedtuple::foreach<S3>([&s3, &member_names](auto int_const) {
			constexpr unsigned i = decltype(int_const)::value;
			member_names[i] = S3::get_name(int_const);
		});
		EXPECT_EQ(member_names[0], "d_i");
		EXPECT_EQ(member_names[1], "d_str");
	}
	// List the size_t of members
	{
		array<size_t, S3::num_members> member_sizes;
		namedtuple::foreach<S3>([&s3, &member_sizes](auto int_const) {
			constexpr unsigned i = decltype(int_const)::value;
			member_sizes[i] = sizeof(s3.get(int_const));
		});
		EXPECT_EQ(member_sizes[0], sizeof(int));
		EXPECT_EQ(member_sizes[1], sizeof(string));
	}
	// A simple generic operaotr<<(ostream)
	{
		stringstream ss;
		namedtuple::foreach<S3>([&s3, &ss](auto int_const) {
			ss << decltype(s3)::get_name(int_const) << ":" << s3.get(int_const) << "\n";
		});
		EXPECT_EQ(ss.str(), "d_i:123\nd_str:abc\n");
	}
}

TEST(Generic, ForeachReversed) {
	// Do Foreach reversely
	S3 s3;
	s3.d_i = 123;
	s3.d_str = "abc";

	// List the member names
	// int_const is ::std::integral_constant<unsigned, i>
	// NOTE: the array is NOT reversed since we are using arr[i] = get_name(i)
	// that is, we are filling the array reversely
	{
		array<string, S3::num_members> member_names;
		namedtuple::foreach_reversed<S3>([&s3, &member_names](auto int_const) {
			constexpr unsigned i = int_const();
			member_names[i] = S3::get_name(i);
		});
		EXPECT_EQ(member_names[0], "d_i");
		EXPECT_EQ(member_names[1], "d_str");
	}
	// List the member names (same as above)
	{
		array<string, S3::num_members> member_names;
		namedtuple::foreach_reversed<S3>([&s3, &member_names](auto int_const) {
			member_names[int_const()] = S3::get_name(int_const);
		});
		EXPECT_EQ(member_names[0], "d_i");
		EXPECT_EQ(member_names[1], "d_str");
	}
	// List the size_t of members
	{
		array<size_t, S3::num_members> member_sizes;
		namedtuple::foreach_reversed<S3>([&s3, &member_sizes](auto int_const) {
			member_sizes[int_const()] = sizeof(s3.get(int_const));
		});
		EXPECT_EQ(member_sizes[0], sizeof(int));
		EXPECT_EQ(member_sizes[1], sizeof(string));
	}
	// A simple generic operaotr<<(ostream)
	// NOTE: the string is reversed
	{
		stringstream ss;
		namedtuple::foreach_reversed<S3>([&s3, &ss](auto int_const) {
			ss << decltype(s3)::get_name(int_const) << ":" << s3.get(int_const) << "\n";
		});
		EXPECT_EQ(ss.str(), "d_str:abc\nd_i:123\n");
	}
}

/*
template<unsigned v>
struct SumOfSizeof_: public integral_constant<unsigned, v> {
	template<unsigned v_rhs>
	constexpr auto operator+(SumOfSizeof_<v_rhs>) {
		return SumOfSizeof_<v+v_rhs>();
	}
};

template<typename T>
constexpr unsigned SumOfSizeof() {
	T* tp = nullptr;
	auto ret = namedtuple::sum<T>(
		[tp](auto int_const) { return SumOfSizeof_<sizeof(decltype(tp->get(int_const)))>(); }
	);
	return ret();
}

TEST(Generic, Sum) {
	static_assert(SumOfSizeof<S1>() == sizeof(long) + sizeof(unsigned) + sizeof(short)*2);
	static_assert(SumOfSizeof<S2>() == sizeof(string) + sizeof(S1));
}

TEST(Generic, DISABLED_SumReversed) {
}

struct S4 {
	int    x;
	// will not compare member started with "_"
	float  _y;
	string z;
	MAKE_NAMEDTUPLE(x, _y, z)
};

template<typename NT, unsigned...indices>
bool compare_impl(const NT& lhs, const NT& rhs, integer_sequence<unsigned, indices...>) {
	return (
		(
			NT::template get_name<indices>()[0] == '_' or
			lhs.template get<indices>() == rhs.template get<indices>()
		)
		and ...
	);
}

template<typename NT>
bool compare(const NT& lhs, const NT& rhs) {
	return compare_impl(lhs, rhs, make_integer_sequence<unsigned, NT::num_members>{});
}

TEST(Customized, Comparison) {
	S4 a{1, 2, "3"};
	S4 b{1, 3, "3"};
	EXPECT_TRUE(compare(a, b)); // true
	b.z = "";
	EXPECT_FALSE(compare(a, b)); // false
}

struct S5 {
	int a;
	vector<int> b1;
	vector<float> b2;
	vector<void*> b3;
	unordered_map<string, int> c1;
	unordered_map<int, int> c2;
	unordered_set<int> d1;
	MAKE_NAMEDTUPLE(b1, b2, b3, c1, c2, d1)

	template<typename U> static void ClearOneVector(U& u) {}
	template<typename U> static void ClearOneVector(vector<U>& u) { u.clear(); }
	template<typename U> static void ClearOneUnorderedMap(U& u) {}
	template<typename U, typename V> static void ClearOneUnorderedMap(unordered_map<U, V>& u) { u.clear(); }

	void ClearAllVectors() {
		namedtuple::foreach<S5>([this](auto int_const) {
			ClearOneVector(get(int_const));
		});
	}
	void ClearAllUnorderedMaps() {
		namedtuple::foreach<S5>([this](auto int_const) {
			ClearOneUnorderedMap(get(int_const));
		});
	}
};

TEST(Customized, Filter) {
	S5 s5;
	s5.a = 1;
	s5.b1.push_back(1);
	s5.b2.push_back(1.5);
	s5.b3.push_back(nullptr);
	s5.c1.emplace("", 1);
	s5.c2.emplace(1.5, 1);
	s5.d1.insert(99);
	EXPECT_EQ(s5.a, 1);
	EXPECT_FALSE(
		s5.b2.empty() or
		s5.b3.empty() or
		s5.c1.empty() or
		s5.c2.empty() or
		s5.d1.empty()
	);

	s5.ClearAllUnorderedMaps();
	EXPECT_EQ(s5.a, 1);
	EXPECT_TRUE(
		s5.c1.empty() and
		s5.c2.empty()
	);
	EXPECT_FALSE(
		s5.b2.empty() or
		s5.b3.empty() or
		s5.d1.empty()
	);

	s5.ClearAllVectors();
	EXPECT_EQ(s5.a, 1);
	EXPECT_TRUE(
		s5.b2.empty() and
		s5.b3.empty() and
		s5.c1.empty() and
		s5.c2.empty()
	);
	EXPECT_FALSE(
		s5.d1.empty()
	);
}

*/
