// Direct include
// C system headers
// C++ standard library headers
#include <array>
#include <sstream>
#include <type_traits>
// Other libraries' .h files.
#include <gtest/gtest.h>
// Your project's .h files.
#include "namedtuple/namedtuple.h"
#include "namedtuple/util/foreach.h"

using namespace std;

DEFINE_NAMEDTUPLE(S1)
	NT_MEMBER(NT_TYPE(long)            , d_l)
	NT_MEMBER(NT_TYPE(unsigned)        , d_u)
	NT_MEMBER(NT_TYPE(array<short, 2>) , d_s2)
END_DEFINE_NAMEDTUPLE(S1)

// NOTE: if your typename does not have comma, then
// NT_TYPE is not necessary
DEFINE_NAMEDTUPLE(S2)
	NT_MEMBER(S1    , d_s1)
	NT_MEMBER(string, d_str)
END_DEFINE_NAMEDTUPLE(S2)

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
	unsigned& d_s1_u = s2.get<0>().get<1>();
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
	EXPECT_EQ(S1::get_name<0>(), "d_l");
	EXPECT_EQ(S1::get_name<1>(), "d_u");
	EXPECT_EQ(S1::get_name<2>(), "d_s2");
	EXPECT_EQ(S2::get_name<0>(), "d_s1");
	EXPECT_EQ(S2::get_name<1>(), "d_str");
}

DEFINE_NAMEDTUPLE(S3)
	NT_MEMBER(NT_TYPE(int)   , d_i)
	NT_MEMBER(NT_TYPE(string), d_str)
END_DEFINE_NAMEDTUPLE(S3)

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
			constexpr unsigned i = decltype(int_const)::value;
			member_names[i] = S3::get_name<i>();
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
			constexpr unsigned i = decltype(int_const)::value;
			member_names[i] = S3::get_name<i>();
		});
		EXPECT_EQ(member_names[0], "d_i");
		EXPECT_EQ(member_names[1], "d_str");
	}
	// List the member names (same as above)
	{
		array<string, S3::num_members> member_names;
		namedtuple::foreach_reversed<S3>([&s3, &member_names](auto int_const) {
			constexpr unsigned i = decltype(int_const)::value;
			member_names[i] = S3::get_name(int_const);
		});
		EXPECT_EQ(member_names[0], "d_i");
		EXPECT_EQ(member_names[1], "d_str");
	}
	// List the size_t of members
	{
		array<size_t, S3::num_members> member_sizes;
		namedtuple::foreach_reversed<S3>([&s3, &member_sizes](auto int_const) {
			constexpr unsigned i = decltype(int_const)::value;
			member_sizes[i] = sizeof(s3.get(int_const));
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

TEST(Generic, DISABLED_Sum) {
}

TEST(Generic, DISABLED_SumReversed) {
}

DEFINE_NAMEDTUPLE(S4)
	NT_MEMBER(int   , x)
	// will not compare member started with "_"
	NT_MEMBER(float , _y)
	NT_MEMBER(string, z)
END_DEFINE_NAMEDTUPLE(S4)

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
