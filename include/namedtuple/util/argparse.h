/*
	This file is a generic argparse for namedtuple.
*/
#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
// Other libraries' .h files.
// Your project's .h files.
#include "namedtuple/namedtuple.h"

namespace namedtuple {

namespace detail {

struct ArgparserForTypes {
	const char **argv;
	int &argc;
	int cur_arg_idx;
	bool parse_successfully;

	const char* GetCurArg() {
		return argv[cur_arg_idx];
	}

	void ParseInt(int& arg) {
		arg = ::std::stoi(GetCurArg());
		NullifyCurArgAndAdvanceArg();
	}

	void ParseFloat(float& arg) {
		arg = ::std::stod(GetCurArg());
		NullifyCurArgAndAdvanceArg();
	}

	void ParseStr(::std::string& arg) {
		arg = GetCurArg();
		NullifyCurArgAndAdvanceArg();
	}

	void Parse(bool& arg) {
		arg = true;
		NullifyCurArgAndAdvanceArg();
	}

	void Parse(int& arg) {
		NullifyCurArgAndAdvanceArg();
		ParseInt(arg);
	}

	void Parse(float& arg) {
		NullifyCurArgAndAdvanceArg();
		ParseFloat(arg);
	}

	void Parse(::std::string& arg) {
		arg = GetCurArg();
		NullifyCurArgAndAdvanceArg();
	}

	void Parse(::std::vector<int>& arg) {
		NullifyCurArgAndAdvanceArg();
		int num;
		ParseInt(num);
		for (int i = 0; i < num; ++i) {
			arg.emplace_back();
			ParseInt(arg.back());
		}
	}

	void Parse(::std::vector<float>& arg) {
		NullifyCurArgAndAdvanceArg();
		int num;
		ParseInt(num);
		for (int i = 0; i < num; ++i) {
			arg.emplace_back();
			ParseFloat(arg.back());
		}
	}

	void Parse(::std::vector<::std::string>& arg) {
		NullifyCurArgAndAdvanceArg();
		int num;
		ParseInt(num);
		for (int i = 0; i < num; ++i) {
			arg.emplace_back();
			ParseStr(arg.back());
		}
	}

	bool StillHasArg() {
		return cur_arg_idx < argc;
	}

	void AdvanceArg() {
		cur_arg_idx++;
	}

	void NullifyCurArgAndAdvanceArg() {
		argv[cur_arg_idx++] = nullptr;
	}

	void RemoveAllNullifiedArguments() {
		argc = ::std::remove(argv, argv+argc, nullptr) - argv;
	}
};

template<typename NT, typename Ind> struct Argparser;
template<typename NT, unsigned ...j>
struct Argparser<NT, ::std::integer_sequence<unsigned, j...>>:
	public ArgparserForTypes
{
	static constexpr unsigned NOT_FOUND = -1;

	unsigned FindMatchedNameIdx(const char *cur_arg) {
		const static ::std::string names[]{ (::std::string("--") + NT::get_name(j))... };
		for (unsigned k = 0; k < NT::num_members; ++k) {
			if (cur_arg == names[k]) {
				return k;
			}
		}
		return NOT_FOUND;
	};

	Argparser(NT& arg, int& argc_, const char** argv_):
		ArgparserForTypes{argv_, argc_, 0, true}
	{
		while (StillHasArg()) {
			unsigned found_name_idx = FindMatchedNameIdx(GetCurArg());
			if (found_name_idx == NOT_FOUND) {
				AdvanceArg();
			} else {
				((
					found_name_idx == j and (Parse(arg.template get<j>()), true),
					found_name_idx == j
				) or ...);
			}
		}
		RemoveAllNullifiedArguments();
	}
};

} // namespace detail

template<typename NT>
bool Argparse(NT& arg, int &argc, const char** argv) {
	detail::Argparser<NT, typename NT::indices_t> parser(arg, argc, argv);
	// always return success for now
	return parser.parse_successfully;
}

}
