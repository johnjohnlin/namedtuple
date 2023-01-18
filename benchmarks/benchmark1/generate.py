#!python3
from random import *
from typing import *

kNumClass = 40
kNativeTypes = ["int*", "long", "unsigned", "float", "string"]
kNativeMemberPerClass = 10
kNestedMemberPerClass = 5
kMinVarLen, kMaxVarLen = 10, 30

class Writer:
	fp: TextIO

	def Begin(self):
		self.fp.write(
"""#include <iostream>
#include <string>
using namespace std;\n"""
		)

	def End(self):
		self.fp.write("int main() {\n")
		for i in range(kNumClass):
			self.fp.write(f"	S{i} s{i}; cout << s{i} << endl;\n")

		self.fp.write("return 0;\n}")

	def Member(self, tname, mname):
		pass

	def BeginStruct(self, sname):
		pass

	def EndStruct(self, sname):
		pass

class HanaWriter(Writer):
	def __init__(self, fname):
		self.fp = open(fname, "w")
		super().__init__()

	def Begin(self):
		super().Begin()
		self.fp.write(
'''#include "boost/hana/define_struct.hpp"
#include "boost/hana/for_each.hpp"
namespace hana = boost::hana;
'''
		)

	def End(self):
		self.fp.write(
"""template<typename NT, typename = ::std::enable_if_t<hana::Struct<NT>::value>>
static ::std::ostream&
operator<<(::std::ostream& ost, const NT &rhs) {
	ost << "{\\n";
	hana::for_each(rhs, [&](const auto &member) {
		ost << hana::first(member).c_str() << ": " << hana::second(member) << ",\\n";
	});
	ost << "}";
	return ost;
}"""
		)
		super().End()

	def Member(self, tname, mname):
		self.mlist.append(f"({tname}, {mname})")

	def BeginStruct(self, sname):
		self.mlist = list()

	def EndStruct(self, sname):
		self.fp.write(f"struct {sname} {{\nBOOST_HANA_DEFINE_STRUCT({sname},\n")
		self.fp.write("\n\t,".join(self.mlist))
		self.fp.write("\n);};\n")

class NamedTupleWriter(Writer):
	def __init__(self, fname):
		self.fp = open(fname, "w")
		super().__init__()

	def Begin(self):
		super().Begin()
		self.fp.write(
'''#include "namedtuple/namedtuple.h"
#include "namedtuple/util/foreach.h"
'''
		)

	def End(self):
		self.fp.write(
"""template<typename NT, typename = ::std::enable_if_t<namedtuple::is_namedtuple_v<NT>>>
static ::std::ostream&
operator<<(::std::ostream& ost, const NT &rhs) {
	ost << "{\\n";
	namedtuple::foreach<NT>([&](auto int_const) {
		ost << rhs.get_name(int_const) << ": " << rhs.get(int_const) << ",\\n";
	});
	ost << "}";
	return ost;
}"""
		)
		super().End()

	def Member(self, tname, mname):
		self.fp.write(f"	NT_MEMBER(NT_TYPE({tname}),{mname})\n")

	def BeginStruct(self, sname):
		self.fp.write(f"DEFINE_NAMEDTUPLE({sname})\n")

	def EndStruct(self, sname):
		self.fp.write(f"END_DEFINE_NAMEDTUPLE({sname})\n")

class NativeWriter(Writer):
	def __init__(self, fname):
		self.fp = open(fname, "w")
		super().__init__()

	def Begin(self):
		super().Begin()

	def End(self):
		super().End()

	def Member(self, tname, mname):
		self.mlist.append(f"\n\t<< \"{mname}: \" << rhs.{mname} << endl")
		self.fp.write(f"\t{tname} {mname};\n")

	def BeginStruct(self, sname):
		self.mlist = list()
		self.fp.write(f"struct {sname} {{\n")

	def EndStruct(self, sname):
		self.fp.write("\n};\n")
		joined = ''.join(self.mlist)
		self.fp.write(
f"""static ::std::ostream& operator<<(::std::ostream& ost, const {sname} &rhs) {{
	ost << "{{\\n";
	ost {joined};
	ost << "}}";
	return ost;
}}"""
		)

def main(writers: List[Writer]):
	seed(12345)
	chars = "abcdefghijklmn"
	for w in writers:
		w.Begin()
	for i in range(kNumClass):
		sname = f"S{i}"
		for w in writers:
			w.BeginStruct(sname)
		members = [
				''.join(choice(chars) for j in range(randint(kMinVarLen, kMaxVarLen)))
				for i in range(kNativeMemberPerClass+kNativeMemberPerClass)
		]
		if i != 0:
			types = [
				choice(kNativeTypes) for j in range(kNativeMemberPerClass)
			] + [
				f"S{randrange(i)}" for j in range(kNestedMemberPerClass)
			]
		else:
			types = [
				choice(kNativeTypes) for j in range(kNativeMemberPerClass+kNestedMemberPerClass)
			]
		for t, m in zip(types, members):
			for w in writers:
				w.Member(t, m)
		for w in writers:
			w.EndStruct(sname)
	for w in writers:
		w.End()

if __name__ == "__main__":
	postfix = f"c{kNumClass:03d}m{kNativeMemberPerClass+kNestedMemberPerClass:03d}"
	main([
		HanaWriter(f"main_hana_{postfix}.cpp"),
		NamedTupleWriter(f"main_nt_{postfix}.cpp"),
		NativeWriter(f"main_native_{postfix}.cpp")
	])
