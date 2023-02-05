// Direct include
// C system headers
// C++ standard library headers
#include <array>
#include <cstring>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
// Other libraries' .h files.
#include <gtest/gtest.h>
// Your project's .h files.
#include "namedtuple/namedtuple.h"
#include "namedtuple/util/argparse.h"

using namespace std;

struct S1 {
	bool version = false;
	bool quiet = false;
	float timeout = 10.0f;
	MAKE_NAMEDTUPLE(version, quiet, timeout)
};

TEST(Parse, Test1) {
	S1 s1;
	int argc;
	const char* params[] {
		"./a.out",
		"--timeout",
		"128.25",
		"file.txt",
		"--version",
		"file2.txt"
	};
	const char* argv[100];
	argv[0] = params[0];
	argv[1] = params[1];
	argv[2] = params[2];
	argv[3] = params[3];
	argv[4] = params[4];
	argv[5] = params[5];
	argc = 6;
	namedtuple::Argparse(s1, argc, argv);
	EXPECT_EQ(argc, 3);
	EXPECT_EQ(argv[0], params[0]);
	EXPECT_EQ(argv[1], params[3]);
	EXPECT_EQ(argv[2], params[5]);
	EXPECT_TRUE(s1.version);
	EXPECT_FALSE(s1.quiet);
	EXPECT_EQ(s1.timeout, 128.25);
}

struct S2 {
	bool help = false;
	int max_length = 0;
	vector<string> file_list = {};
	vector<int> size_limit = {};
	MAKE_NAMEDTUPLE(help, max_length, file_list, size_limit)
};

TEST(Parse, Test2) {
	S2 s2;
	int argc;
	const char* params[] {
		"./run.exe",
		"--file_list",
		"3",
		"a.txt",
		"b.txt",
		"c.txt",
		"--size_limit",
		"0",
		"concated.txt",
		"--max_length",
		"5566",
	};
	const char* argv[100];
	argv[ 0] = params[ 0];
	argv[ 1] = params[ 1];
	argv[ 2] = params[ 2];
	argv[ 3] = params[ 3];
	argv[ 4] = params[ 4];
	argv[ 5] = params[ 5];
	argv[ 6] = params[ 6];
	argv[ 7] = params[ 7];
	argv[ 8] = params[ 8];
	argv[ 9] = params[ 9];
	argv[10] = params[10];
	argc = 11;
	namedtuple::Argparse(s2, argc, argv);
	EXPECT_EQ(argc, 2);
	EXPECT_EQ(argv[0], params[0]);
	EXPECT_EQ(argv[1], params[8]);
	EXPECT_FALSE(s2.help);
	EXPECT_EQ(s2.max_length, 5566);
	EXPECT_EQ(s2.file_list, (vector<string>{"a.txt", "b.txt", "c.txt"}));
	EXPECT_EQ(s2.size_limit, vector<int>{});
}
