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
#include "namedtuple/util/operators.h"

using namespace std;

struct S1 {
	long     d_l;
	unsigned d_u;
	short    d_s;
	MAKE_NAMEDTUPLE(d_l, d_u, d_s)
};

struct S2 {
	S1     d_s1;
	string d_str;
	MAKE_NAMEDTUPLE(d_s1, d_str)
};

struct S3 {
	S1             d_s1;
	vector<S2>     d_s2v;
	vector<string> d_strv;
	MAKE_NAMEDTUPLE(d_s1, d_s2v, d_strv)
};

TEST(operators, stream) {
	static_assert(namedtuple::has_cbegin<vector<int>>::value);
	static_assert(not namedtuple::has_cbegin<int>::value);
	S3 s3;
	stringstream ss;
	s3.d_s1.d_l = 101;
	s3.d_s1.d_u = 102;
	s3.d_s1.d_s = 103;
	s3.d_s2v.push_back({{7,8,9}, "ZZZ"});
	s3.d_strv.push_back("A");
	s3.d_strv.push_back("BB");
	ss << s3;
	EXPECT_EQ(
		ss.str(),
R"({
	"d_s1": {
		"d_l": 101,
		"d_u": 102,
		"d_s": 103
	},
	"d_s2v": [
		{
			"d_s1": {
				"d_l": 7,
				"d_u": 8,
				"d_s": 9
			},
			"d_str": "ZZZ"
		}
	],
	"d_strv": [
		"A",
		"BB"
	]
})"
	);
	// cout << s3 << endl;
}

TEST(operators, compare) {
	S2 s2a;
	S2 s2b;
	unsigned mismatch_pos;
	auto s1_names = namedtuple::collection<S1>::get_tuple_names();
	auto s2_names = namedtuple::collection<S2>::get_tuple_names();
	s2a.d_s1.d_l = 101;
	s2a.d_s1.d_u = 102;
	s2a.d_s1.d_s = 103;
	s2a.d_str = "104";
	s2b = s2a;

	mismatch_pos = namedtuple::collection<S2>::mismatch(s2a, s2b);
	EXPECT_EQ(mismatch_pos, 2);
	EXPECT_FALSE(s2a != s2b);
	EXPECT_TRUE(s2a == s2b);

	s2a.d_str = "1";
	mismatch_pos = namedtuple::collection<S2>::mismatch(s2a, s2b);
	EXPECT_EQ(mismatch_pos, 1);
	EXPECT_EQ(s2_names[mismatch_pos], "d_str"); // get the name of mismatched member
	EXPECT_FALSE(s2a == s2b);
	EXPECT_TRUE(s2a != s2b);

	s2a.d_s1.d_u = 0;
	mismatch_pos = namedtuple::collection<S2>::mismatch(s2a, s2b);
	EXPECT_EQ(mismatch_pos, 0);
	EXPECT_EQ(s2_names[mismatch_pos], "d_s1"); // get the name of mismatched member
	mismatch_pos = namedtuple::collection<S1>::mismatch(s2a.d_s1, s2b.d_s1);
	EXPECT_EQ(mismatch_pos, 1);
	EXPECT_EQ(s1_names[mismatch_pos], "d_u"); // get the name of mismatched member
	EXPECT_FALSE(s2a == s2b);
	EXPECT_TRUE(s2a != s2b);
}
