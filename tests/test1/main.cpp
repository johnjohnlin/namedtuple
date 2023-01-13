// Direct include
// C system headers
// C++ standard library headers
#include <array>
#include <type_traits>
// Other libraries' .h files.
#include <gtest/gtest.h>
// Your project's .h files.
#include "namedtuple.h"

using namespace std;

struct S1_native {
	long            d_l;
	unsigned        d_u;
	array<short, 2> d_s2;
};

DEFINE_NAMEDTUPLE(S1)
	NT_MEMBER(NT_TYPE(long)            , d_l)
	NT_MEMBER(NT_TYPE(unsigned)        , d_u)
	NT_MEMBER(NT_TYPE(array<short, 2>) , d_s2)
END_DEFINE_NAMEDTUPLE(S1)

DEFINE_NAMEDTUPLE(S2)
END_DEFINE_NAMEDTUPLE(S2)

TEST(Test, Simple) {
	// namedtuple is exactly the same as native type!
	static_assert(sizeof(S1) == sizeof(S1_native));
	// it is standard layout as its members standard layout
	static_assert(is_standard_layout<S1>::value);
	// you can use ::num_members
	static_assert(S1::num_members == 3);

	S1 s1;

	// written by name and read by tuple
	s1.d_l = 123;
	s1.d_u = 456;
	s1.d_s2[0] = 78;
	s1.d_s2[1] = 90;
	long&     d_l = s1.get<0>();
	unsigned& d_u = s1.get<1>();
	short&    d_s20 = s1.get<2>()[0];
	short&    d_s21 = s1.get<2>()[1];
	EXPECT_EQ(d_l, 123);
	EXPECT_EQ(d_u, 456);
	EXPECT_EQ(d_s20, 78);
	EXPECT_EQ(d_s21, 90);

	// written by tuple and read by name
	s1.get<0>() = 111;
	s1.get<1>() = 222;
	s1.get<2>()[0] = 333;
	s1.get<2>()[1] = 444;
	EXPECT_EQ(s1.d_l, 111);
	EXPECT_EQ(s1.d_u, 222);
	EXPECT_EQ(s1.d_s2[0], 333);
	EXPECT_EQ(s1.d_s2[1], 444);

	// they are exactly the same pointer
	EXPECT_EQ(&s1.d_l    , &d_l);
	EXPECT_EQ(&s1.d_u    , &d_u);
	EXPECT_EQ(&s1.d_s2[0], &d_s20);
	EXPECT_EQ(&s1.d_s2[1], &d_s21);

	// get member name (it is static)
	EXPECT_EQ(S1::get_name<0>(), "d_l");
	EXPECT_EQ(S1::get_name<1>(), "d_u");
	EXPECT_EQ(S1::get_name<2>(), "d_s2");
}

TEST(Test, NestedNamedTuple) {
}
