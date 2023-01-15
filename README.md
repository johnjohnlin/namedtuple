### What is namedtuple
In Python, there is a namedtuple

```python
Point = namedtuple('Point', ['x', 'y'])
p = Point(11, y=22)
p[0] + p[1] # 33
p.x + p.y # 33
```

In C++, we have tuple and struct, but we cannot connect them easily.

```cpp
struct S { int x; float y; string z; };
S s_struct
tuple<int, float, string> s_tuple;
s_struct.y; // float
s_tuple.get<1>(); // float
s_tuple.y; // ERROR
s_struct.get<1>(); // ERROR
```

We provide a macro for defining a namedtuple in C++ easily.

```cpp
DEFINE_NAMEDTUPLE(S2)
	NT_MEMBER(int   , x)
	NT_MEMBER(float , y)
	NT_MEMBER(string, z)
END_DEFINE_NAMEDTUPLE(S2)
S2 s_namedtuple;
s_namedtuple.y; // float
s_namedtuple.get<1>(); // float, the reference of y
static_assert(sizeof(S2) == sizeof(S)); // namedtuple does not add extra members!
static_assert(sizeof(S2::num_members) == 3u); // namedtuple also provides ::num_members
S2::get_name<1>(); // string("y")
```

### Usage
With the `get()` function, we can make use of parameter pack (C++11) of fold expression (C++17).
For example, if we want to compare two namedtuple, but ignore members start with an underscore:

```cpp
DEFINE_NAMEDTUPLE(S2)
	NT_MEMBER(int   , x)
	// will not compare member started with "_"
	NT_MEMBER(float , _y)
	NT_MEMBER(string, z)
END_DEFINE_NAMEDTUPLE(S2)

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

S2 a{1, 2, "3"};
S2 b{1, 3, "3"};
compare(a, b); // true
b.z = "";
compare(a, b); // false
```

### Implementations

The idea is simple. If we implement `get(X)`, where X is overloaded with different `integral_constant`, then we can implement `get<unsigned>()` easily.

```cpp
struct S2 {
	int    x;
	float  y;
	string z;
	int    get(integral_constant<unsigned, 0>) { return x; }
	float  get(integral_constant<unsigned, 1>) { return y; }
	string get(integral_constant<unsigned, 2>) { return z; }
	template<unsigned x> auto get() { return get(integral_constant<unsigned, x>()); }
};
```

The `__COUNTER__` preprocessor (not a C++ standard) provides the functionalities we need.
The counters is incremented by one when referenced in a file.

```cpp
// DEFINE_NAMEDTUPLE(S2)
struct S2 {
	static constexpr int Base = 100 + 1;
// NT_MEMBER
	int    x;
	int    get(integral_constant<unsigned, 101-Base>) { return x; }
// NT_MEMBER
	float  y;
	float  get(integral_constant<unsigned, 102-Base>) { return y; }
// NT_MEMBER
	string z;
	string get(integral_constant<unsigned, 103-Base>) { return z; }
// END_DEFINE_NAMEDTUPLE(S2)
	static constexpr int End = 104;
	static constexpr int num_members = End - Base;
	template<unsigned x> auto get() { return get(integral_constant<unsigned, x>()); }
};
```

### Requirements
Just copy everything under `include/` to anywhere you can include.
The core namedtuple is macro based, so it only requires C++11. However, `util/` might require
C++14 or C++17 features.

Also, it requires the `__COUNTER__` preprocessor, which is supported by most common compilers.
While template based counter also works, `__COUNTER__` might provides better compile time
since it is only a preprocessor.
